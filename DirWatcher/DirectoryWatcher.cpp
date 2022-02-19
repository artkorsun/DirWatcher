#include "DirectoryWatcher.h"
#include "CopyAction.h"
#include "DeleteAction.h"
#include "RenameAction.h"

#include <windows.h>
#include <optional>
#include <regex>
#include <sstream>
#include <map>

namespace {

std::optional<std::time_t> get_delete_timestamp(const std::wstring& filename)
{
    const std::wregex re(LR"(.*delete_(\d{4}-\d{2}-\d{2}T\d{2}-\d{2})_.*)");
    std::wsmatch match;

    std::regex_match(filename, match, re);
    if (!match.empty()) {

        auto s = match.size();

        if (match.size() < 2) {
            return std::nullopt;
        }

        std::wistringstream ss{ match[1].str() };
        std::tm dt = {0};
        ss >> std::get_time(&dt, L"%Y-%m-%dT%H-%M");
        return std::mktime(&dt);
    }

    return std::nullopt;
}
}

DirectoryWatcher::DirectoryWatcher(const std::wstring& source,
					const std::wstring& target,
					ActionQueue& action_queue,
					const std::atomic<bool>& stop,
                    Logger& logger)
	:	_source(source),
	    _target(target),
		_action_queue(action_queue),
	    _stop(stop),
        _logger(logger)
{
}

bool DirectoryWatcher::run(HANDLE io_completion_port)
{
    HANDLE source_dir_handle = CreateFileW(_source.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);

    if (source_dir_handle == INVALID_HANDLE_VALUE) {
        _logger.error(L"Can't open " + _source + L" direcotry for watching");
        return false;
    }

    if (!CreateIoCompletionPort(source_dir_handle, io_completion_port, NULL, 1)) {
        _logger.error(L"Cannot create IOCP: " + GetLastError());
        return false;
    }

    std::optional<std::wstring> old_name;

    while (!_stop.load()) {
        OVERLAPPED overlapped = {0};
        
        uint8_t change_buf[1024];
        BOOL success = ReadDirectoryChangesW(
            source_dir_handle, change_buf, 1024, FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            NULL, &overlapped, NULL);

        if (!success) {
            _logger.error(L"Failed to read changes in the source directory: " + GetLastError());
            return false;
        }

        DWORD number_of_bytes = 0;
        ULONG_PTR completion_key = 0;
        OVERLAPPED* overlapped_res = nullptr;
        BOOL res = FALSE;
        while ((res = GetQueuedCompletionStatus(io_completion_port, 
                                                &number_of_bytes, 
                                                &completion_key, 
                                                &overlapped_res, 
                                                INFINITE)) != FALSE) {
            if (completion_key == 0 && number_of_bytes == 0) {
                // Signal to stop listening
                return true;
            }
            else {
                FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf;

                std::vector<std::shared_ptr<Action>> actions;
                std::multimap<std::time_t, std::shared_ptr<Action>> deferred_actions;

                while (true) {
                    std::wstring filename(event->FileName, event->FileNameLength / sizeof(wchar_t));

                    if (!std::filesystem::is_directory(_source + L"\\" + filename)) {
                        switch (event->Action) {
                        case FILE_ACTION_ADDED: {
                            std::wstring source = _source + L"\\" + filename;
                            std::wstring target = _target + L"\\" + filename + L".bak";
                            actions.emplace_back(std::make_shared<CopyAction>(source, target));
                        } break;

                        case FILE_ACTION_REMOVED: {
                            std::wstring target = _target + L"\\" + filename + L".bak";
                            actions.emplace_back(std::make_shared<DeleteAction>(target));
                        } break;

                        case FILE_ACTION_MODIFIED: {
                            std::wstring source = _source + L"\\" + filename;
                            std::wstring target = _target + L"\\" + filename + L".bak";
                            actions.emplace_back(std::make_shared<CopyAction>(source, target));
                        } break;

                        case FILE_ACTION_RENAMED_OLD_NAME: {
                            old_name = filename;
                        } break;

                        case FILE_ACTION_RENAMED_NEW_NAME: {
                            std::wstring source_old = _source + L"\\" + *old_name;
                            std::wstring source_new = _source + L"\\" + filename;
                            std::wstring target_old = _target + L"\\" + *old_name + L".bak";;
                            std::wstring target_new = _target + L"\\" + filename + L".bak";;

                            if (filename.substr(0, 7) == L"delete_") {

                                if (std::optional<std::time_t> timestamp_to_delete = get_delete_timestamp(filename); timestamp_to_delete) {
                                    deferred_actions.insert({ *timestamp_to_delete, std::make_shared<DeleteAction>(source_new) });
                                    deferred_actions.insert({ *timestamp_to_delete, std::make_shared<DeleteAction>(target_old) });
                                }
                                else {
                                    actions.emplace_back(std::make_shared<DeleteAction>(source_new));
                                    actions.emplace_back(std::make_shared<DeleteAction>(target_old));
                                }
                            }
                            else {
                                actions.emplace_back(std::make_shared<RenameAction>(target_old, target_new));
                            }

                            old_name = std::nullopt;

                        } break;

                        default: {
                            _logger.warning(L"Unexpected action with file " + filename);
                        } break;
                        }
                    }
                    else {
                        _logger.warning(L"Ignore action with subdirectory " + filename);
                    }

                    // Handle next event if any
                    if (event->NextEntryOffset) {
                        *((uint8_t**)&event) += event->NextEntryOffset;
                    }
                    else {
                        break;
                    }
                }

                if (!actions.empty()) {
                    _action_queue.add(actions);
                }

                if (!deferred_actions.empty()) {
                    _action_queue.add_deferred(deferred_actions);
                }
            }

            BOOL success = ReadDirectoryChangesW(
                source_dir_handle, change_buf, 1024, FALSE,
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                NULL, &overlapped, NULL);

            if (!success) {
                _logger.error(L"Failed to read changes in the source directory: " + GetLastError());
                return false;
            }
        }
    }

    return true; // should never come here
}
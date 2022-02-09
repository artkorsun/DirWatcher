#include "DirectoryWatcher.h"
#include "CopyAction.h"
#include "DeleteAction.h"
#include "RenameAction.h"

#include <windows.h>
#include <iostream> //@@ replace with a logger
#include <optional>

DirectoryWatcher::DirectoryWatcher(const std::wstring& source,
					const std::wstring& target,
					ActionQueue& action_queue,
					const std::atomic<bool>& stop)
	:	_source(source),
	    _target(target),
		_action_queue(action_queue),
	    _stop(stop)
{
}

void DirectoryWatcher::run(HANDLE& watch_event)
{
    HANDLE source_dir_handle = CreateFileW(_source.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);

    if (source_dir_handle == INVALID_HANDLE_VALUE)
    {
        std::wcerr << "Can't open " << _source << " direcotry for watching\n";
        return;
    }

    std::optional<std::wstring> old_name;

    while (!_stop.load()) {
        OVERLAPPED overlapped;
        ResetEvent(watch_event);
        overlapped.hEvent = watch_event;

        uint8_t change_buf[1024];
        BOOL success = ReadDirectoryChangesW(
            source_dir_handle, change_buf, 1024, TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            NULL, &overlapped, NULL);

        if (!success) {
            // TODO Report win error
        }

        DWORD bytes_read = 0;
        bool overlapped_res = GetOverlappedResult(source_dir_handle, &overlapped, &bytes_read, TRUE);

        if (!overlapped_res) {
            // TODO Report win error
        }

        FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf;

        std::vector<std::shared_ptr<Action>> actions;

        while (true) {
            std::wstring filename(event->FileName, event->FileNameLength / sizeof(wchar_t));

            switch (event->Action) {
            case FILE_ACTION_ADDED: {
                std::wstring source = _source + L"\\" + filename;
                std::wstring target = _target + L"\\" + filename;
                actions.emplace_back(std::make_shared<CopyAction>(source, target));
            } break;

            case FILE_ACTION_REMOVED: {
                std::wstring target = _target + L"\\" + filename;
                actions.emplace_back(std::make_shared<DeleteAction>(target));
            } break;

            case FILE_ACTION_MODIFIED: {
                std::wstring source = _source + L"\\" + filename;
                std::wstring target = _target + L"\\" + filename;
                actions.emplace_back(std::make_shared<CopyAction>(source, target));
            } break;

            case FILE_ACTION_RENAMED_OLD_NAME: {
                old_name = filename;
            } break;

            case FILE_ACTION_RENAMED_NEW_NAME: {
                if (!old_name) {
                    // std::wcout << "  Old name is not obtained for renamed file " << filename << ", skipping\n";
                    // TODO report to logger
                    break;
                }

                std::wstring source = _target + L"\\" + *old_name;
                std::wstring target = _target + L"\\" + filename;
                actions.emplace_back(std::make_shared<RenameAction>(source, target));

                old_name = std::nullopt;

            } break;

            default: {
                std::wcout << "Unexpected action with file " << filename << " \n";
            } break;
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
    }
}
#include "ActionQueue.h"
#include "DirectoryWatcher.h"
#include "StartUpSynchronizer.h"
#include "StartUpSynchronizer.h"
#include "CommandManager.h"

#include <windows.h>
#include <tchar.h>

#include <filesystem>
#include <iostream>
#include <optional>
#include <thread>

void run(const std::wstring&, const std::wstring&);

void _tmain(int argc, TCHAR* argv[])
{
    if (argc != 2)
    {
        _tprintf(TEXT("Usage: %s <source dir> <target dir>\n"), argv[0]);
        return;
    }

    std::string source_dir = argv[1];
    std::string target_dir = argv[2];

    run(std::wstring{std::begin(source_dir), std::end(source_dir)},
        std::wstring{std::begin(target_dir), std::end(target_dir)});
}

void run(const std::wstring& source, const std::wstring& target) {
    
    std::wcout << L"Welcome to DirWatcher!\n";
    
    if (!std::filesystem::is_directory(source)) {
        std::wcout << source << L" is not a directory, quit" << "\n";
        return;
    }

    if (!std::filesystem::is_directory(target)) {
        std::wcout << target << L" is not a directory, quit" << "\n";
        return;
    }

    std::wcout << L"Source directory: " << source << "\n";
    std::wcout << L"Target directory: " << target << "\n";

    Logger logger;
    logger.info(L"Application started");

    std::wcout << L"Performing start up synchronization...\n";

    ActionQueue action_queue;
    StartUpSynchronizer start_sync(source, target, action_queue, logger);
    start_sync.perform();

    std::atomic<bool> stop;

    HANDLE io_completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);

    std::thread dir_watching_thread([&]() {
        DirectoryWatcher dw(source, target, action_queue, stop, logger);
        if (!dw.run(io_completion_port)) {
            std::wcout << "Error during directory changes watching occured, use \"log\" command for details\n";
        }
    });

    std::thread action_thread([&]() {
        while (true) {
            auto actions = action_queue.retrieve();

            if (!actions.empty()) {
                logger.info(L"Found " + std::to_wstring(actions.size()) + L" actions to process");

                for (auto action : actions)
                    action->execute(logger);
            }
            if (stop.load()) {
                break;
            }
            else {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    });

    CommandManager command_manager([&logger]() {
                                        logger.output(std::wcout);
                                    }, 
                                   [&stop, &io_completion_port]() {
                                        std::wcout << "Closing application...\n";
                                        stop.store(true);
                                        PostQueuedCompletionStatus(io_completion_port, 
                                                                   0, 
                                                                   reinterpret_cast<ULONG_PTR>(nullptr), 
                                                                   nullptr);
                                    }, 
                                   stop);

    command_manager.run();    

    dir_watching_thread.join();
    action_thread.join();

    std::wcout << "Buy!\n";
}
#include "ActionQueue.h"
#include "DirectoryWatcher.h"
#include "StartUpSynchronizer.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <optional>
#include <thread>

void run(const std::wstring&, const std::wstring&);

void _tmain(int argc, TCHAR* argv[])
{/*
    if (argc != 2)
    {
        _tprintf(TEXT("Usage: %s <dir>\n"), argv[0]);
        return;
    }
    */
    std::wstring hot_dir = L"C:\\test1"; // = argv[1];
    std::wstring clone_dir = L"C:\\test2"; // = argv[2];

    run(hot_dir, clone_dir);
}

void run(const std::wstring& source, const std::wstring& target) {
    
    Logger logger;

    ActionQueue action_queue;
    StartUpSynchronizer start_sync(source, target, action_queue);
    start_sync.perform();

    std::atomic<bool> stop;

    HANDLE watch_event = CreateEvent(NULL, TRUE, 0, NULL);

    std::thread dir_watching_thread([&]() {
        DirectoryWatcher dw(source, target, action_queue, stop);
        dw.run(watch_event);
    });

    std::thread action_thread([&]() {
        while (true) {
            auto actions = action_queue.retrieve();

            for (auto action : actions)
                action->execute(logger);

            if (stop.load()) {
                break;
            }
            else {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    });

    std::wcout << "Enter q to quit\n";
    while (true) {
        std::wcout << ">";
        std::wstring input;
        std::wcin >> input;

        if (input == L"q" || input == L"quit") {
            stop.store(true);
            CancelIo(watch_event);
            break;
        }
        else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    dir_watching_thread.join();
    action_thread.join();
}
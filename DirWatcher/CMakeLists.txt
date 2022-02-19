cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

add_executable (DirWatcher "DirWatcher.cpp" "DirWatcher.h" "Action.h" "CopyAction.h" "CopyAction.cpp" "Logger.h" "Logger.cpp" "Utils.h" "Utils.cpp" "DeleteAction.h" "DeleteAction.cpp"  "RenameAction.h" "RenameAction.cpp" "DirectoryWatcher.h" "DirectoryWatcher.cpp" "ActionQueue.h" "ActionQueue.cpp" "StartUpSynchronizer.h" "StartUpSynchronizer.cpp" "CommandManager.h" "CommandManager.cpp")

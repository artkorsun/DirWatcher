# CMakeList.txt: проект CMake для NordSecTest; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (NordSecTest "FolderCloner.cpp" "FolderCloner.h" "Action.h" "CopyAction.h" "CopyAction.cpp" "Logger.h" "Logger.cpp" "Utils.h" "Utils.cpp" "DeleteAction.h" "DeleteAction.cpp"  "RenameAction.h" "RenameAction.cpp" "DirectoryWatcher.h" "DirectoryWatcher.cpp" "ActionQueue.h" "ActionQueue.cpp" "StartUpSynchronizer.h" "StartUpSynchronizer.cpp" "CommandManager.h" "CommandManager.cpp")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.

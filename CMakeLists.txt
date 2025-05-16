cmake_minimum_required(VERSION 3.10)
project(FileManager)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Iinclude -Isrc/core -Isrc/ui -Isrc/operations -Isrc/utils -Isrc/config -Isrc/error")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lncurses")

# Поиск исходных файлов
file(GLOB SOURCES "src/**/*.c")

# Создание исполняемого файла
add_executable(file_manager ${SOURCES})

# Добавление необходимых заголовочных директорий
target_include_directories(file_manager PRIVATE include src/core src/ui src/operations src/utils src/config src/error)
target_link_libraries(file_manager ncurses)

# Установка
install(TARGETS file_manager DESTINATION /usr/local/bin)

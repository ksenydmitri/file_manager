cmake_minimum_required(VERSION 3.30)
project(kurs_osisp C)

set(CMAKE_C_STANDARD 23)

add_executable(kurs_osisp main.c
        ops.c
        ops.h
        path_utils.c
        path_utils.h
        file_utils.c
        file_utils.h
        clipboard.c
        clipboard.h)

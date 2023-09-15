cmake_minimum_required(VERSION 3.0)
project(hw-1)

add_executable(task-1 1.c)
target_compile_options(task-1 PRIVATE -Wall -Wextra)


add_executable(task-2 2.c)
target_compile_options(task-2 PRIVATE -Wall -Wextra)



add_executable(task-3 3.c)
target_compile_options(task-3 PRIVATE -Wall -Wextra)


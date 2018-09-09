cmake_minimum_required(VERSION 3.9)
project(PArserJSON C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -Wextra")

add_executable(ParserJSON Parser.c Parser.h Main.c Main.h)
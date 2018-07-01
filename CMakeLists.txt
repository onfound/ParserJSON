cmake_minimum_required(VERSION 3.9)
project(PArserJSON C)

set(CMAKE_C_STANDARD 99)

add_executable(ParserJSON Parser.c Parser.h Main.c)
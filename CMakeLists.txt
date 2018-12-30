cmake_minimum_required(VERSION 3.0)

project (url-base-64)

set (CMAKE_C_STANDARD 11)

add_executable (url-base-64
                URLBase64.c
                URLBase64.h
                main.c)

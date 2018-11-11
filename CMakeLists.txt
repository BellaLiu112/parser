cmake_minimum_required(VERSION 3.6)
project(parser)


set(SOURCE_CODE
    parser/main.cpp
    parser/hashtable.c
    parser/hashtable.h)
  
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "parser")
  
add_executable(parser ${SOURCE_CODE})
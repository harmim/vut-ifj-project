# Author: Dominik Harmim <xharmi00@stud.fit.vutbr.cz>

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_C_FLAGS "-Wall -std=c99 -pedantic -g")

PROJECT(ifj)
FILE(GLOB HEADERS "*.h")
FILE(GLOB SOURCES "*.c")

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${HEADERS})

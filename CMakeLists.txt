# Author: Dominik Harmim <xharmi00@stud.fit.vutbr.cz>

CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

IF(UNIX)
	SET(CMAKE_C_COMPILER gcc)
	SET(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror -g")
ENDIF()

PROJECT(ifj17_compiler)

FILE(GLOB SOURCES "*.c")
FILE(GLOB HEADERS "*.h")

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${HEADERS})

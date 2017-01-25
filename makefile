CC = g++
STD = -std=c++11
SRC = main.cpp
OUT = raregen.exe

all : raregen

raregen : ${SRC}
	${CC} ${STD} ${SRC} -o ${OUT}

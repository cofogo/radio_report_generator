CC = g++
STD = -std=c++11
SRC = main.cpp
OUT = radio_rep_gen.exe

all : radio_rep_gen

radio_rep_gen : ${SRC}
	${CC} ${STD} ${SRC} -o ${OUT}

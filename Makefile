PROG=asteroids
CC=gcc
FLAGS=-lm `pkg-config -libs raylib`

.DEFAULT_GOAL := run

${PROG}: ${PROG}.c
	${CC} ${PROG}.c -o ${PROG} ${FLAGS}

run: ${PROG}
	./${PROG}

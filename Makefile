PROG=asteroids
CC=gcc
FLAGS=-lm -lraylib 

.DEFAULT_GOAL := install 

${PROG}: ${PROG}.c
	${CC} ${PROG}.c -o ${PROG} ${FLAGS}

install: ${PROG}
	cp ${PROG} ~/.local/bin/

clean: ${PROG}
	rm ${PROG}

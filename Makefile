PROG=asteroids
CC=gcc
FLAGS=-lm -lraylib 

.DEFAULT_GOAL := install 

install: ${PROG}
	cp ${PROG} ~/.local/bin/

build: ${PROG}.c
	${CC} ${PROG}.c -o ${PROG} ${FLAGS}

build-osx: ${PROG}
	clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a ${PROG}.c -o ${PROG}

clean: ${PROG}
	rm ${PROG}

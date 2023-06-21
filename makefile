CC = mpic++
FILES = main.cpp
CFLAGS = -std=c++20 -O3 -march=native -g -Wall -Wextra

all:
	${CC} ${FILES} ${CFLAGS}

pogo-use:
	${CC} ${FILES} $(CFLAGS) -fprofile-use

pogo-generate:
	${CC} ${FILES} ${CFLAGS} -fprofile-generate

valgrind:
	mpiexec valgrind --leak-check=yes --track-origins=yes -s --suppressions=/usr/share/openmpi/openmpi-valgrind.supp ./a.out > valgrind.txt 2>&1

vtune:
	mpiexec vtune -quiet -collect performance-snapshot -trace-mpi -result-dir vtune-results ./a.out
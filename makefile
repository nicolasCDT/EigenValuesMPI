compile_sync:
	gcc -o eigenvalues main.c matrix.c -lm

run_sync:
	gcc -o eigenvalues main.c matrix.c -lm

make_mpi:
	mpicc -o eigenvalue main.c matrix.c -lm

run_mpi:
	mpirun -hostfile hosts -np 4 ./eigenvalue

default:
	mpicc -o eigenvalue main.c matrix.c -lm
	mpirun -hostfile hosts -np 4 ./eigenvalue
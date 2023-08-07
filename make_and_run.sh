mpicc -o eigenvalue main.c matrix.c -lm
mpirun -hostfile hosts -np 4 ./eigenvalue 

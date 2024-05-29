cmake_minimum_required(VERSION 3.25)
project(Edge_Finding C)

set(CMAKE_C_STANDARD 11)

add_executable(Edge_Finding source/main_noMPI.c include/pnmio.h source/pnmio.c source/main_MPI_old.c source/main_MPI_broadcast.c source/main_MPI_scatter.c)

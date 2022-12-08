#include <iostream>
#include <mpi.h>

using namespace std;

int main(int narg, char **argv)
{
  // initialize variables
  size_t Narray = 10;

  // mpi vars
  int rank, worldSize;
  MPI_Init(&narg, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  // read imput
  if( narg < 2)
  {
    cout << "Wrong number of arguments. Use as follows: " << endl;
    cout << "    mpirun -n 2 pingpong.p arrayLength " << endl;
    cout << "where " << std::endl;
    cout << "  arrayLength is the length of the array to be used" << endl;
    return 1;
  }else 
  {
    Narray = stoi(argv[1]);
  }

  float *A = new float[Narray];
  double time = 0., tmp;

  // repeat the process 100 times
  size_t repeat = 100;
  for(size_t n = 0; n < repeat; n++)
  {
    // code for process o
    if(rank==0)
    {
      tmp = MPI_Wtime();
      MPI_Send(A,Narray,MPI_FLOAT,1,0,MPI_COMM_WORLD);
      MPI_Recv(A,Narray,MPI_FLOAT,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      time += MPI_Wtime() - tmp;
    }else
    {
      MPI_Recv(A,Narray,MPI_FLOAT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Send(A,Narray,MPI_FLOAT,0,0,MPI_COMM_WORLD);
    }
  }

  time = time / double(repeat);

  if(rank==0) cout << 2.*double(sizeof(float)*Narray)/time / 1e6 << " GBytes/sec" << endl;

  delete[] A;

  MPI_Finalize();
  return 0;
}
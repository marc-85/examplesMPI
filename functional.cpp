/*
 In this example  there will be a master process which will distribute work to 
 different workers. In particular, there will be a worker which will add the
 elements in even array positions while the other worker will add those at
 odd array positions

 Created by Marc Bolinches for the purpose of illustrating functional parallelism
*/

#include <mpi.h>
#include <iostream>

using namespace std;

inline float addOdd(float *A, int N);
inline float addEven(float *A, int N);

int main(int argc, char **argv)
{
  // init MPI
  MPI_Init(&argc, &argv);
  int rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int Narray = 1;

  if(argc<2)
  {
    cerr << "Wring number of arguments" << endl;
    cout << "run as follows: mpirun -n 3 functional.p Narray" << endl;
    cout << "     Narray is the number of elements of the array" << endl;
    MPI_Finalize();
    return 1;
  } else if( world_size != 3)
  {
    cerr << "This program is to be run with 3 processes!" << endl;
  }
  else
  {
    Narray = stoi(argv[1]);
  }

  // all processes allocate the array
  float *A = new float[Narray];

  // separate tasks
  switch(rank)
  {
    case 0: // this is the master branch
    {
      srand(0);
      for(int n = 0; n < Narray; n++) A[n] = double(rand() % 100) * 2. - 100.;

      // send the array to process 1 and 2
      MPI_Bcast(A,Narray, MPI_FLOAT, 0, MPI_COMM_WORLD);

      float even, odd;
      // wait to get results from processes 1 and 2
      MPI_Recv(&even,1, MPI_FLOAT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(&odd,1, MPI_FLOAT, 2, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      cout << "even: " << even << endl;
      cout << "odd: " << odd << endl;

      break;
    }  
    case 1: // even
    {
      // receive data
      MPI_Bcast(A,Narray, MPI_FLOAT, 0, MPI_COMM_WORLD);
      float even = addEven(A, Narray);

      // send result to 0
      MPI_Send(&even, 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
      break;
    }
    case 2: // odd
    {
      // receive data
      MPI_Bcast(A,Narray, MPI_FLOAT, 0, MPI_COMM_WORLD);
      float odd = addOdd(A, Narray);
      
      // send result to 0
      MPI_Send(&odd, 1, MPI_FLOAT, 0, 2, MPI_COMM_WORLD);
      break;
    }
  }

  delete[] A;
  MPI_Finalize();
  return 0;
}

float addEven(float *A, int N)
{
  float sum = 0.;
  for(int n = 0; n < N; n+=2) sum += A[n];
  return sum;
}

float addOdd(float *A, int N)
{
  float sum = 0.;
  for(int n = 1; n < N; n+=2) sum += A[n];
  return sum;
}

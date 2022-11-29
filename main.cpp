#include <mpi.h>
#include <iostream>
#include <string>

enum OPERATION {
  SUM_VEC = 0,
  MIN_VEC,
  BAD_OPERATION = -1
};

inline double getMin(double *A, int size);
inline double getSUM(double *A, int size);

using namespace std;

int main(int narg, char **argv)
{
  // size vectors
  int N = 5;

  // mpi vars
  int rank, worldSize;
  MPI_Init(&narg, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  OPERATION myOperation = MIN_VEC;

  // generate a random vector
  double *A = new double[N];
  srand(rank*1000);
  for(int n = 0; n < N; n++)
  {
    A[n] = rand();
  }

  // result of the operations
  double result;
  
  try
  {
    switch(myOperation)
    {
      case SUM_VEC:
        break;
      case MIN_VEC:
        double localMin;
        localMin = getMin(A,N);
        MPI_Allreduce(&localMin, &result, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        break;
      default:
        string message = "A valid operatioon was not selected. Selected option was ";
        message = message + to_string(myOperation);
        throw runtime_error(message.c_str());
        break;
    }
  }
  catch(std::exception &e)
  {
    if(rank==0) cerr << e.what() << endl;

    delete[] A;
    MPI_Finalize();
    return 0;
  }

  // unify vector
  // 1st - send all vectors to process 0
  for(int proc = 1; proc < worldSize; proc++)
    if(rank==proc) MPI_Send(A, N, MPI_DOUBLE, 0, proc, MPI_COMM_WORLD);
  
  // 2nd -receive and store in one array
  if(rank==0)
  {
    double *unifiedVector = new double[worldSize*N];
    for(int n = 0; n <N; n++)
      unifiedVector[n] = A[n];

    // get data from other processes
    for(int proc = 1; proc < worldSize; proc++)
    {
      MPI_Status status;
      MPI_Recv(A, N, MPI_DOUBLE, proc, proc, MPI_COMM_WORLD, &status);
      for(int n = 0; n < N; n++)
        unifiedVector[n + proc*N] = A[n];
    }

    // print the vector
    for(int n = 0; n < worldSize*N; n++) cout << unifiedVector[n] << endl;

    delete[] unifiedVector;
  }

  cout << "Result of your operation: " << result << endl;

  delete[] A;
  MPI_Finalize();
  return 0;
 }

////////////////////////////////////////////////
//           AUXILIARY FUNCTIONS
////////////////////////////////////////////////

double getMin(double *A, int size)
{
  // very basic min computation loop
  double min = 1e35;
  for(int n = 0; n < size; n++)
  {
    if(A[n]<min) min = A[n];
  }
  return min;
}
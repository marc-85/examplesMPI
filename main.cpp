/*
* Example created by Marc Bolinches for the purpose of illustrating MPI operations
*/

#include <mpi.h>
#include <iostream>
#include <string>

enum OPERATION {
  SUM_VEC = 0,
  MIN_VEC = 1,
  BAD_OPERATION = -1
};

inline double getMin(double *A, int size);
inline double getSum(double *A, int size);

using namespace std;

int main(int narg, char **argv)
{
  // size vectors
  int N = 5;

  OPERATION myOperation = MIN_VEC;

  // mpi vars
  int rank, worldSize;
  MPI_Init(&narg, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  // Read arguments
  if(narg<3)
  {
    if(rank==0) cerr << "Wrong number of arguments" << endl;
    if(rank==0)
    {
      cout << "Use this programs as follows: mpirun -n Nproc ./vec_operations.p arraySize operation" << endl;
      cout << " Nproc is the number of MPI processes" << endl;
      cout << " arraySize is size of the parallel arrays" << endl;
      cout << " operarion is an integer that corresponds with the operation to be made" << endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Abort(MPI_COMM_WORLD, -1);
  }else
  {
    N           = stoi(argv[1]);
    myOperation = static_cast<OPERATION>(stoi(argv[2]));
  }

  // generate a random vector
  double *A = new double[N];
  srand(rank*1000);
  for(int n = 0; n < N; n++)
  {
    A[n] = rand() % 1000;
  }

  // result of the operations
  double result;
  
  // counting the time
  double time = MPI_Wtime();

  try
  {
    switch(myOperation)
    {
      case SUM_VEC:
        double localSum;
        localSum = getSum(A,N);
        MPI_Allreduce(&localSum, &result, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
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

  time = MPI_Wtime() - time;

  // MPI_Barrier(MPI_COMM_WORLD);
  if(rank==0) cout << "Result parallel: " << result << " time: " << time << " s " << endl;

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
    //for(int n = 0; n < worldSize*N; n++) cout << unifiedVector[n] << endl;

    time = MPI_Wtime();
    double serialResult;
    switch(myOperation)
    {
      case SUM_VEC:
        serialResult = getSum(unifiedVector,N*worldSize);
        break;
      case MIN_VEC:
        serialResult = getMin(unifiedVector,N*worldSize);
        break;
    }

    time = MPI_Wtime() -time;
    cout << "Result serial: " << serialResult << " time: " << time << " s " << endl;

    delete[] unifiedVector;
  }

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

double getSum(double *A, int size)
{
  // very basic sum computation loop
  double sum = 0.;
  for(int n = 0; n < size; n++)
  {
    sum += A[n];
  }
  return sum;
}
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include "exec_time.h"

#define DATASET_SIZE 4194304  // 2^22
#define THREADS_PER_BLOCK 256

#define MAX_DATA_SIZE 1048576//2^20

// Kernel function to add the elements of two arrays
__global__ 
void add(int n, float *d_x, float *d_y)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int passo = blockDim.x * gridDim.x;
  for (int i = index; i < n; i += passo) {
    d_y[i] = d_x[i] + d_y[i];
  }
}

int main_func(int argc, char **argv)
{
  float *h_x, *h_y;
  float *d_x, *d_y;
  cudaError_t cudaError;
  int i;
  struct timeval start, stop;

  // Disable buffering entirely
  setbuf(stdout, NULL);

  // Allocating arrays on host
  printf("Allocating arrays h_x and h_y on host...");
  gettimeofday(&start, NULL);

  h_x = (float*)malloc(DATASET_SIZE*sizeof(float));
  h_y = (float*)malloc(DATASET_SIZE*sizeof(float));

  // check malloc memory allocation
  if (h_x == NULL || h_y == NULL) { 
	printf("Error: malloc unable to allocate memory on host.");
        return 1;
  }

  gettimeofday(&stop, NULL);
  printf("%f ms\n", timedifference_msec(start, stop));

  // Allocating array on device
  printf("Allocating array d_x and d_y on device...");
  gettimeofday(&start, NULL);

  cudaError = cudaMalloc(&d_x, MAX_DATA_SIZE*sizeof(float));

  // check cudaMalloc memory allocation
  if (cudaError != cudaSuccess) {
	printf("cudaMalloc d_x returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
        return 1;
  }

  cudaError = cudaMalloc(&d_y, MAX_DATA_SIZE*sizeof(float));

  // check cudaMalloc memory allocation
  if (cudaError != cudaSuccess) {
	printf("cudaMalloc d_y returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
        return 1;
  }

  gettimeofday(&stop, NULL);
  printf("%f ms\n", timedifference_msec(start, stop));

  // Initialize host memory
  printf("Initializing array h_x and h_y on host...");
  gettimeofday(&start, NULL);

  for (i =0; i < DATASET_SIZE; ++i) {
	h_x[i] = 1.0f;
	h_y[i] = 2.0f;
  }

  gettimeofday(&stop, NULL);
  printf("%f ms\n", timedifference_msec(start, stop));

  float *x,*y;
  float timeToCopy = 0.0f;
  float timeToAdd = 0.0f;
  float timeToCopyBack = 0.0f;
  x = h_x;
  y = h_y;
  int reps = DATASET_SIZE / MAX_DATA_SIZE;
  int rest = DATASET_SIZE % MAX_DATA_SIZE;
  for( int i = 0; i <= reps; i++) {
    int data_size = MAX_DATA_SIZE;
    if (i == reps) { // caso o data_set não seja divisivel por 2^20
      if(rest == 0) { // resto eh 0, acabou
        break;
      }
      data_size = rest;
    }
    gettimeofday(&start, NULL);
    cudaError = cudaMemcpy(d_x, x, data_size*sizeof(float), cudaMemcpyHostToDevice);  
    if (cudaError != cudaSuccess) {
      printf("cudaMemcpy (h_x -> d_x) returned error %s (code %d), line(%d)\n", cudaGetErrorString(cudaError), cudaError, __LINE__);
      return 1;
     }
    cudaError = cudaMemcpy(d_y, y, data_size*sizeof(float), cudaMemcpyHostToDevice);  
    if (cudaError != cudaSuccess) {
      printf("cudaMemcpy (h_x -> d_x) returned error %s (code %d), line(%d)\n", cudaGetErrorString(cudaError), cudaError, __LINE__);
      return 1;
    }
    gettimeofday(&stop, NULL);
    timeToCopy += timedifference_msec(start, stop);
    
    // Run kernel on elements on the GPU
    //printf("Running kernel on elemnts of d_x and d_y...");
    gettimeofday(&start, NULL);
    int blockSize = THREADS_PER_BLOCK;
    int numBlocks = (data_size + blockSize - 1) / blockSize;
    add<<<numBlocks, blockSize>>>(data_size, d_x, d_y);

    // Wait for GPU to finish before accessing on host
    cudaDeviceSynchronize();
    gettimeofday(&stop, NULL);
    timeToAdd += timedifference_msec(start, stop);
    
    // Copy array from device to host
    gettimeofday(&start, NULL);

    cudaError = cudaMemcpy(y, d_y, data_size*sizeof(float), cudaMemcpyDeviceToHost);

    if (cudaError != cudaSuccess)
    {
      printf("cudaMemcpy (d_y -> h_y) returned error %s (code %d), line(%d)\n", cudaGetErrorString(cudaError), cudaError, __LINE__);
      return 1;
    }
    gettimeofday(&stop, NULL);
    timeToCopyBack += timedifference_msec(start, stop);

    y += data_size;
    x += data_size;
  }
  

  

  printf("Copying arrays from host to device...%f ms\n", timeToCopy);
  printf("Running kernel on elemnts of d_x and d_y...%f ms\n", timeToAdd);
  printf("Copying array from device (d_y) to host (h_y)...%f ms\n", timeToCopyBack);
  
  
  // Check for errors (all values should be 3.0f)
  printf("Checking for processing errors...");
  gettimeofday(&start, NULL);

  float maxError = 0.0f;
  float diffError = 0.0f;
  for (i = 0; i < DATASET_SIZE; i++) {
    maxError = (maxError > (diffError=fabs(h_y[i]-3.0f)))? maxError : diffError;
    //printf("%d -> %f\n", i, h_y[i]);
  }

  gettimeofday(&stop, NULL);
  printf("%f ms\n", timedifference_msec(start, stop));
  printf("Max error: %f\n", maxError);

  // Free memory
  printf("Freeing memory...");
  gettimeofday(&start, NULL);
  cudaFree(d_x);
  cudaFree(d_y);
  free(h_x);
  free(h_y);
  gettimeofday(&stop, NULL);
  printf("%f ms\n", timedifference_msec(start, stop));
  
  return 0;
}

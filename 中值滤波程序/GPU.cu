//
//  GPU.cpp
//  hpalab5
//
//  Created by Harshdeep Singh Chawla on 10/11/16.
//  Copyright ?2016 Harshdeep Singh Chawla. All rights reserved.
//
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>
#include "MedianFilter.h"
#include "Bitmap.h"
void CUDA_CALL(cudaError_t x)
{
	if (x != cudaSuccess)
	{
		printf("\nCUDA Error:%s(err_num=%d)\n",cudaGetErrorString(x),x);
		cudaDeviceReset(); 
		cudaSetDevice(0);
		//exit(0);
	}
}
///Kernelk function
__global__ void MFKernel(unsigned char *inputImage, unsigned char *outputImage, int Width, int Height)
{
	// indexing for thread.
	 int idy = blockIdx.y * blockDim.y + threadIdx.y;
	 int idx = blockIdx.x * blockDim.x + threadIdx.x;

	//filter mask
	 char filter[9];
	 if ((idy >= Height) || (idx >= Width))
		 return;
	/////checking boundry conditions
	if((idy==0) || (idx==0) || (idy==Height-1) || (idx==Width-1))
				outputImage[idy*Width+idx] = 0;
	else {
		for (int x = 0; x < WINDOW_SIZE; x++) { 
			for (int y = 0; y < WINDOW_SIZE; y++){
				filter[x*WINDOW_SIZE+y] = inputImage[(idy+y-1)*Width+(idx+x-1)];   // setup the filterign window.
			}
		}
		////Sorting in filter
		for (int i = 0; i < 9; i++) {
			for (int j = i + 1; j < 9; j++) {
				if (filter[i] > filter[j]) { 
					//Swap the variables.
					char tmp = filter[i];
					filter[i] = filter[j];
					filter[j] = tmp;
				}
			}
		}
		outputImage[idy*Width+idx] = filter[4];   //Set output variables.
	}
}
///GPU Function
void MedianFilterGPU(Bitmap* image, Bitmap* outputImage){

	//Cuda error and image values.
	/*cudaError_t status;*/
	int w = image->Width();
	int h = image->Height();
	size_t bytes = w * h * sizeof(unsigned char);
	//initialize images.
	cudaError_t err;
	unsigned char *inputimage_d;
	err=cudaMalloc((void**)&inputimage_d, bytes);
	CUDA_CALL(err);
	err = cudaSuccess;
	err=cudaMemcpy(inputimage_d, image->image, bytes, cudaMemcpyHostToDevice);
	CUDA_CALL(err);
	err = cudaSuccess;
	unsigned char *outputImage_d;
	err=cudaMalloc((void**)&outputImage_d, bytes);
	CUDA_CALL(err);
	err = cudaSuccess;
	//set block and grids.
	int TILE_SIZE = 16;
	dim3 dimBlock(TILE_SIZE, TILE_SIZE,1);
	dim3 dimGrid((w+15)/16,(h+15)/16,1);
	//std::cout << "width=:" << w << "height=:" << h << std::endl;
	MFKernel <<<dimGrid,dimBlock>>>(inputimage_d, outputImage_d, w, h);
	cudaDeviceSynchronize();
	err=cudaGetLastError();
	CUDA_CALL(err);
	err = cudaSuccess;
	// save output image to host.
	err=cudaMemcpy(outputImage->image, outputImage_d, bytes, cudaMemcpyDeviceToHost);
	CUDA_CALL(err);
	err = cudaSuccess;
	//Free the memory
	cudaFree(inputimage_d);
	cudaFree(outputImage_d);
	return ;
}

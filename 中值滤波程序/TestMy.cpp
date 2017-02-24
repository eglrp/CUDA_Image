//
//  main.cpp
//  hpalab5
//
//  Created by Harshdeep Singh Chawla on 10/10/16.
//  Copyright © 2016 Harshdeep Singh Chawla. All rights reserved.
//
#include "Bitmap.h"
#include "MedianFilter.h"
#include <ctime>
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>
#include"RandInt.h"
#include<cmath>
#include<ctime>

const int itrs = 10;//循环迭代次数
const int bmp_size = 128*4;
void testMy()
{
	Bitmap *inputimage = new Bitmap(bmp_size, bmp_size);
	Bitmap *CPUo_pimage = new Bitmap(bmp_size, bmp_size);
	Bitmap *GPUo_pimage = new Bitmap(bmp_size, bmp_size);
	for (int i = 0; i < bmp_size; i++)
		for (int j = 0; j < bmp_size; j++)
			inputimage->SetPixel(i, j, generate_rand(i,j));

	inputimage->Save("Hello.bmp");
	//getting height and width of the image
	int w = inputimage->Width();
	int h = inputimage->Height();
	std::cout << "Width is " << w << "  Height is " << h << std::endl;

	/////////////CPU///////////////////
	float start = clock();
	for (int i = 0; i<itrs; i++)
		MedianFilterCPU(inputimage, CPUo_pimage);
	float end = clock();
	float tcpu = (float)(end - start) * 1000 / (float)CLOCKS_PER_SEC;
	std::cout << "Average Time per CPU Iteration is: " << tcpu << " ms" << std::endl << std::endl;


	//////Waking call for gpu//////

	MedianFilterGPU(inputimage, GPUo_pimage, false);

	/////////////GPU without shared memory/////////////
	float start1 = clock();
	for (int j = 0; j < itrs; j++)
		MedianFilterGPU(inputimage, GPUo_pimage, false);
	float end1 = clock();
	float tgpu = (float)(end1 - start1) * 1000 / (float)CLOCKS_PER_SEC;
	std::cout << "Average Time per GPU Iteration with global Memory is: " << tgpu << " ms" << std::endl;
	std::cout << "Speedup : " << tcpu / tgpu << std::endl;


	/////////////GPU with shared memory/////////////

	MedianFilterGPU(inputimage, GPUo_pimage, true);
	float start2 = clock();
	for (int j = 0; j < itrs; j++)
		MedianFilterGPU(inputimage, GPUo_pimage, true);
	float end2 = clock();
	float tgpu1 = (float)(end2 - start2) * 1000 / (float)CLOCKS_PER_SEC;
	std::cout << "Average Time per GPU Iteration with Shared memory is: " << tgpu1 << " ms" << std::endl;
	std::cout << "Speedup : " << tcpu / tgpu1 << std::endl;
	//////cpu.bmp-gpushared.bmp///////////////

}
int main() {
	testMy();
	return 0;
}

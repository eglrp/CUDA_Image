#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
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
#include<vector>
#include<string>
#include<fstream>
#include<cmath>
using std::string;
using std::fstream;
using std::thread;
using std::vector;
const int itrs = 10;//循环迭代次数

float whole_speedup = 0;// GPU/CPU 加速比
float whole_cpu_time = 0;// CPU 单次执行时间
vector<float> Ac_gpu;//GPU/CPU 加速比
vector<float>Ac_whole;//GPU+CPU/CPU 加速比 结合了CPU和GPU协同运算的加速比
void testMy2(int bmp_size)
{
	int CPU_size = (int)((bmp_size / whole_speedup));
	Bitmap *inputimage = new Bitmap(bmp_size, bmp_size);
	Bitmap *inputimage_cpu = new Bitmap(CPU_size, bmp_size);
	//Bitmap *inputimage_cpu1 = new Bitmap(CPU_size, bmp_size);
	Bitmap *inputimage_gpu = new Bitmap(bmp_size , bmp_size);
	Bitmap *CPUo_pimage = new Bitmap(CPU_size, bmp_size);
	//Bitmap *CPUo_pimage1 = new Bitmap(CPU_size, bmp_size);
	Bitmap *GPUo_pimage = new Bitmap(bmp_size , bmp_size);
	for (int i = 0; i < bmp_size; i++)
	   for (int j = 0; j < bmp_size; j++)
	      inputimage->SetPixel(i, j, generate_rand(i, j));
	
	for (int i = 0; i < CPU_size; i++)
		for (int j = 0; j < bmp_size; j++)
			inputimage_cpu->SetPixel(i, j, inputimage->GetPixel(i, j));
	/*for (int i = 0; i < CPU_size; i++)
		for (int j = 0; j < bmp_size; j++)
			inputimage_cpu1->SetPixel(i, j, inputimage->GetPixel(i, j));*/
	for (int i = 0; i < bmp_size; i++)
		for (int j = 0; j < bmp_size; j++)
			inputimage_gpu->SetPixel(i, j, inputimage->GetPixel(i, j));
			
	/////////////CPU+GPU///////////////////
	float totle_time=0.0;
	MedianFilterGPU1(inputimage_gpu, GPUo_pimage);//GPU预热一下
	float start = clock();
	for (int i = 0; i < 10; i++)
       {
		 //  thread t_cpu(MedianFilterCPU, inputimage_cpu, CPUo_pimage);
		  // thread t_cpu1(MedianFilterCPU, inputimage_cpu1, CPUo_pimage1);
		   //thread t_gpu(MedianFilterGPU1, inputimage_gpu, GPUo_pimage);
		  // t_cpu1.join();
		  // t_cpu.join();
		   //t_gpu.join();
		   MedianFilterGPU1(inputimage_gpu, GPUo_pimage);
		 
		   
	   }
	float end = clock();
	float tcpu = end-start;
	std::cout << "Average Time per CPU+GPU Itera  tion is: " << tcpu << " ms" << std::endl << std::endl;
	std::cout << "Speedup : " <<whole_cpu_time /tcpu<< std::endl;
	Ac_whole.push_back(whole_cpu_time / tcpu);
	delete inputimage;
	delete inputimage_cpu;
	delete inputimage_gpu;
	delete CPUo_pimage;
	delete GPUo_pimage;
}
void testMy(int bmp_size)
{
	Bitmap *inputimage = new Bitmap(bmp_size, bmp_size);
	Bitmap *CPUo_pimage = new Bitmap(bmp_size, bmp_size);
	Bitmap *GPUo_pimage = new Bitmap(bmp_size, bmp_size);
	for (int i = 0; i < bmp_size; i++)
		for (int j = 0; j < bmp_size; j++)
			inputimage->SetPixel(i, j, generate_rand(i, j));
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
	whole_cpu_time = tcpu;

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
	whole_speedup = tcpu / tgpu;
	Ac_gpu.push_back(tcpu/tgpu);
}
void test_whole()
{
	int bmp_size = 256;
	testMy(256);
	testMy2(256);
}
/*
filename1 要写入的文件的名字
//times      要写入的文件的名字的标号
*/
void save_to_text(string filename1, vector<float> &p)
{
	using namespace std;
	fstream outfile;

	string tempfilenanme = filename1;
	outfile.open(tempfilenanme, ios::out | ios::trunc);
	for (auto i = 0; i < p.size(); i++)
		outfile << p[i] << endl;
	outfile.close();
}
int main() {
	test_whole();
	save_to_text("GPU.txt", Ac_gpu);
	save_to_text("whole.txt", Ac_whole);
	return 0;
}

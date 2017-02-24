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
#include<cmath>
#include<ctime>
#include<vector>
#include<string>
#include<fstream>
#include<cmath>
#include<cstdlib>
#include"MyTime.h"
using std::string;
using std::fstream;
using std::thread;
using std::vector;
const int itrs = 10;//循环迭代次数

float whole_speedup = 0;// GPU/CPU 加速比
float whole_cpu_time = 0;// CPU 单次执行时间
vector<float> Ac_gpu;//GPU/CPU 加速比
vector<float>Ac_whole;//GPU+CPU/CPU 加速比 结合了CPU和GPU协同运算的加速比
vector<float>Qilin_whole;//采用Qilin模型的加速比
vector<float> CPU_Time;
vector<float> GPU_Time;
vector<float> GPU_CPU_Time;
vector<float> Qilin_Time;
Bitmap load_bmp(string name);
void testMy3(Bitmap &temp)
{ 
	
	int width = temp.Width();
	int height = temp.Height();
	int CPU_size = 14;
	double speedup = 0;
	/*switch (width)
	{
	case 256:CPU_size = 5; break;
	case 512:CPU_size = 10; break;
	case 1024:CPU_size = 20; break;
	case 2048:CPU_size = 40; break;
	case 4096:CPU_size = 80; break;
	case 8192:CPU_size = 160; break;
	}*/
	int w1 = height - CPU_size;
	Bitmap *inputimage_cpu = new Bitmap(width, CPU_size);;
	Bitmap *inputimage_gpu = new Bitmap(width, w1);
	Bitmap *CPUo_pimage = new Bitmap(width, CPU_size);
	Bitmap *GPUo_pimage = new Bitmap(width, w1);
	for (int i = 0; i <width; i++)
		for (int j = 0; j < CPU_size; j++)
			inputimage_cpu->SetPixel(i, j, temp.GetPixel(i, j));
	for (int i = 0; i <width; i++)
		for (int j = 0; j <w1; j++)
			inputimage_gpu->SetPixel(i, j, temp.GetPixel(i, j + CPU_size));

	/////////////CPU+GPU///////////////////
	MedianFilterGPU(inputimage_gpu, GPUo_pimage);//GPU预热一下
	MyTimer timer;
	timer.start();
	for (int i = 0; i <itrs; i++)
	{
		thread t_cpu(MedianFilterCPU, inputimage_cpu, CPUo_pimage);
		thread t_gpu(MedianFilterGPU, inputimage_gpu, GPUo_pimage);
		t_cpu.join();
		t_gpu.join();
	}
	timer.stop();
	std::cout << "Average Time per Qilin Iteration is: " << timer.elapse()/itrs << " ms" << std::endl << std::endl;
	speedup = whole_cpu_time/(timer.elapse()/itrs);
	std::cout << "Speedup : " << speedup << std::endl;
	Qilin_whole.push_back(speedup);
	Qilin_Time.push_back(timer.elapse()/itrs);
	delete inputimage_cpu;
	delete inputimage_gpu;
	delete CPUo_pimage;
	delete GPUo_pimage;
}
void testMy2(Bitmap &temp)
{
	int width = temp.Width();
	int height = temp.Height();
	int CPU_size = (int)(height/(whole_speedup + 1));
	double speedup = 0;
	CPU_size *= 0.9;
	int w1 = height - CPU_size;
	Bitmap *inputimage_cpu = new Bitmap(width, CPU_size);;
	Bitmap *inputimage_gpu = new Bitmap(width, w1);
	Bitmap *CPUo_pimage = new Bitmap(width, CPU_size);
	Bitmap *GPUo_pimage = new Bitmap(width, w1);
	for (int i = 0; i <width; i++)
		for (int j = 0; j < CPU_size; j++)
			inputimage_cpu->SetPixel(i, j, temp.GetPixel(i, j));
	for (int i = 0; i <width; i++)
		for (int j = 0; j < w1; j++)
			inputimage_gpu->SetPixel(i, j, temp.GetPixel(i, j + CPU_size));

	/////////////CPU+GPU///////////////////
	MedianFilterGPU(inputimage_gpu, GPUo_pimage);//GPU预热一下
	MyTimer timer;
	timer.start();
	for (int i = 0; i <itrs; i++)
	{
		  thread t_cpu(MedianFilterCPU, inputimage_cpu, CPUo_pimage);
		  thread t_gpu(MedianFilterGPU, inputimage_gpu, GPUo_pimage);
		  t_cpu.join();
		  t_gpu.join();
	}
	timer.stop();
	std::cout << "Average Time per CPU+GPU Iteration is: " << timer.elapse() /itrs << " ms" << std::endl << std::endl;
	speedup = whole_cpu_time / (timer.elapse() / itrs);
	std::cout << "Speedup : " << speedup << std::endl;
	Ac_whole.push_back(speedup);
	GPU_CPU_Time.push_back(timer.elapse()/itrs);
	delete inputimage_cpu;
	delete inputimage_gpu;
	delete CPUo_pimage;
	delete GPUo_pimage;
}
void testMy(Bitmap &temp)
{
	static int times = 0;
	Bitmap *inputimage = new Bitmap(temp.Width(), temp.Height());
	Bitmap *CPUo_pimage = new Bitmap(temp.Width(), temp.Height());
	Bitmap *GPUo_pimage = new Bitmap(temp.Width(), temp.Height());
	for (int i = 0; i < temp.Width(); i++)
		for (int j = 0; j < temp.Height(); j++)
			inputimage->SetPixel(i, j, temp.GetPixel(i,j));
	int w = inputimage->Width();
	int h = inputimage->Height();
	std::cout << "Width is " << w << "  Height is " << h << std::endl;
	MyTimer timer;
	/////////////CPU///////////////////
	timer.start();
	for (int i = 0; i<itrs; i++)
		MedianFilterCPU(inputimage,CPUo_pimage);
	timer.stop();
	printf("Time Elapsed: %lf\n", timer.elapse());
	std::cout << "Average Time per CPU Iteration is: " << timer.elapse()/itrs << " ms" << std::endl << std::endl;
	whole_cpu_time = timer.elapse()/itrs;
	CPU_Time.push_back(whole_cpu_time);
	//////Waking call for gpu//////

	MedianFilterGPU(inputimage, GPUo_pimage);

	/////////////GPU without shared memory/////////////
	MyTimer timer1;
	timer1.start();
	for (int j = 0; j < itrs; j++)
		//MedianFilterGPU(inputimage, GPUo_pimage, false);
	MedianFilterGPU(inputimage, GPUo_pimage);
	timer1.stop();
	printf("Time Elapsed: %lf\n", timer1.elapse());
	std::cout << "Average Time per GPU Iteration with global Memory is: " << timer1.elapse()/itrs << " ms" << std::endl;
	std::cout << "Speedup : " << timer.elapse() / timer1.elapse() << std::endl;
	GPU_Time.push_back(timer1.elapse()/itrs);
	//if (times==0)
		whole_speedup = timer.elapse() / timer1.elapse();
	Ac_gpu.push_back(whole_speedup);
	times++;
	delete inputimage;
	delete CPUo_pimage;
	delete GPUo_pimage;
}
Bitmap load_bmp(string name)
{
	Bitmap temp;
	temp.Load(name.c_str());
	return temp;
}
void test_whole()
{
	char name1[10] = {'r','1','.','b','m','p'};
	char name10[10] = { 'r', '1','0', '.', 'b', 'm', 'p' };
	vector<string>pname;
	for (int i = 1; i < 10; i++)
	{
		name1[1] = '0' + i;
		pname.push_back(name1);
	}
	for (int i = 0; i <= 15-10; i++)
	{
		name10[2] = '0' + i;
		pname.push_back(name10);
	}
	for (int i = 0; i <15; i++)
	{
		std::cout << "这是第" << i << "次执行\n";
		Bitmap temp = load_bmp(pname[i]);
		testMy(temp);
		testMy2(temp);
		testMy3(temp);
	}
	/*Bitmap temp = load_bmp("r2.bmp");
	testMy(temp);
	testMy2(temp);*/
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
// file为读取文件的名字
void readTxt(string file, vector<string> &d)
{
	using namespace std;
	char buffer[256];
	ifstream in(file);
	if (!in.is_open())
	{
		cout << "Error opening file"; exit(1);
	}
	while (!in.eof())
	{
		in.getline(buffer, 100);
		//cout << buffer << endl;
		d.push_back(buffer);
	}
	for (int i = 0; i < d.size(); i++)
		std::cout << d[i] << std::endl;
}
void test_varible()
{
	double sum_gpu = 0;
	double sum_whole = 0;
	vector<string> d1, d2;
	readTxt("GPU_time.txt", d1);
	readTxt("whole_time.txt", d2);
	for (int i = 0; i < 15; i++)
	{
		sum_gpu += atof(d1[i].c_str());
		sum_whole += atof(d2[i].c_str());
	}
	std::cout << sum_gpu << std::endl;
	std::cout << sum_whole << std::endl;
	double var1=0, var2=0;
	for (int i = 0; i < 2; i++)
	{
		var1 += (atof(d1[i].c_str()) - sum_gpu/15)*(atof(d1[i].c_str()) - sum_gpu/15);
		var2 += (atof(d2[i].c_str()) - sum_whole/15)*(atof(d2[i].c_str()) - sum_whole/15);
	}
	std::cout << var1 << std::endl;
	std::cout << var2 << std::endl;
}
int main() {
	test_whole();
	save_to_text("GPU_Ac.txt", Ac_gpu);
	save_to_text("CPU_GPU_Ac.txt", Ac_whole);
	save_to_text("Qilin_Ac.txt", Qilin_whole);
	save_to_text("CPU_time.txt", CPU_Time);
	save_to_text("GPU_time.txt", GPU_Time);
	save_to_text("CPU_GPU_time.txt", GPU_CPU_Time);
	save_to_text("Qilin_Time.txt", Qilin_Time);
	//test_varible();
	system("pause");
	return 0;
}

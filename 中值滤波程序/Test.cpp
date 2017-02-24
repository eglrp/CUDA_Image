#include<iostream>
#include"Bitmap.h"
#include"RandInt.h"
#include<cmath>
#include<ctime>
using namespace std;
void test()
{
	Bitmap a(1200,1200);
	for (int i = 0; i < 1200; i++)
		for (int j = 0; j < 1200; j++)
			a.SetPixel(i, j, generate_rand(0,254));
	a.Save("helloworld.bmp");
	Bitmap b;
	b.Load("Lenna.bmp");
	cout << b.Height() <<" "<< b.Width() << endl;
	for (int i = 0; i < b.Width(); i++)
		for (int j = 0; j < b.Height(); j++)
			cout <<int( b.GetPixel(i, j)) << " ";
}
void main0()
{
	test();
	cout << "hello woorld" << endl;
}
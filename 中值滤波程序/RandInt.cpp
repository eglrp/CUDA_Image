#include"RandInt.h"
#include<cmath>
#include<ctime>
#include<iostream>
int Random(int m, int n)
{
	using namespace std;
	int pos, dis;
	if (m == n)
	{
		return m;
	}
	else if (m > n)
	{
		pos = n;
		dis = m - n + 1;
		return rand() % dis + pos;
	}
	else
	{
		pos = m;
		dis = n - m + 1;
		return rand() % dis + pos;
	}
}
int generate_rand(int m, int n)
{
	using namespace std;
	srand((int)time(NULL));
	return Random(m, n);
}
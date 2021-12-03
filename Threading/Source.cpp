#include<iostream>
#include"Threading.h"

using namespace std;

struct circle
{
	float r;
	float posX, posY;
};

#define count 1000
circle c[count] = { 0 };

void f()
{
	for (int i = 0; i < 100; i++)
	{
		bool isHit = false;
		for (int j = 0; j < 100; j++)
		{
			if (i == j)
			{
				continue;
			}

			const float posX = c[i].posX - c[j].posX;
			const float posY = c[i].posY - c[j].posY;
			const float r2 = c[i].r + c[j].r;
			if ((posX * posX) + (posY * posY) < r2 * r2)
			{
				isHit = true;
			}
		}
	}
}

int main()
{
	Threading pool;

	// 変数 i のところにブレークポイントおけば、処理速度の違いがわかるよ！ｗ
	int i = 0;

	// 通常処理
	for (int i = 0; i < 1000; i++)
	{
		f();
	}

	i = 0;

	// 並列処理
	for (int i = 0; i < 1000; i++)
	{
		pool.AddTask(f);
	}
	pool.Start();
	pool.WaitUntilTheEnd();

	i = 0;
}
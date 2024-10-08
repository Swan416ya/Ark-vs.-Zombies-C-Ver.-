#include <iostream>
#include <graphics.h>
#include <Windows.h>
#include "tools.h"
#include <cmath>
#include <set>
#include <vector>
#include <algorithm>
#include <mmsystem.h>
#include <conio.h> // 包含 _kbhit() 函数的头文件
#include <windows.h>
#include <easyx.h>
#include <wingdi.h>
#pragma comment(lib, "winmm.lib")//播放音乐
#pragma comment(lib, "Msimg32.lib")

#define WIDTH 1350
#define HEIGHT 800

enum { Myrtle, Chilchuck, Kroos, Mizuki, Cuora, Hoshiguma, Typhon, Card_cnt };
enum { B2, Runner, Ghost, C0, Surprise, S10, S2, Enemy_Type_Cnt };

IMAGE background;//草坪背景
IMAGE gamebar;//卡槽
IMAGE card[Card_cnt];//种子卡
IMAGE* plant_attack[Card_cnt][60];//植物攻击动画
IMAGE* plant_idle[Card_cnt][60];//植物静止动画
IMAGE Chosing_Template;//选中卡片时的模板
IMAGE shadow;//阴影
IMAGE card_in_shadow;
IMAGE Crane;//铲子
IMAGE Crane_Moving;//铲子移动
IMAGE gameOverBackground; // 声明背景图像变量
IMAGE gameOverImage;
// 散装植物定义：
int curX, curY;//选中卡片后拖动时的当前鼠标位置，用于绘制卡片的移动效果
int nowPlant;//当前选中的植物,0为未选中
int map_hard = 1;//地图难度 分别会使用不同的地图
int shoot_time[Card_cnt] = { 40,40,40,40,40,1000,40 };//射击间隔
int danger_zone[Card_cnt] = { 1,10,10,1,1,1,10 };//危险区域
int hurt_point[Card_cnt] = { 10,5,20,15,3,0,30 };//伤害值
int plant_sun_cost[Card_cnt] = { 50,75,100,175,50,150,150 };//植物需要花费的阳光数
int plant_health_point[Card_cnt] = { 500,500,500,500,5000,8000,500 };//植物的血量

int sunshine_cnt;//阳光数量

//判断游戏结束
bool gameOver = false;

bool Crane_Choosing;

struct plants_on_lawn {
	int type;//0是空地，1是植物
	int movetype;//1是攻击，2是静置
	int frameIndex;//当前帧
	int Sun_Producting_Timer;//阳光产生计时器
	int Shoot_Timer;//射击计时器
	// 植物格状态
	bool catched; //是否被僵尸捕获
	int deadTime; //死亡倒计时 每一帧吃一口 到一定次数被吃掉
	int health_point; //血条
};

struct plants_on_lawn scence[5][9];

struct SunshineBall {
	int x, y;
	int frameIndex;//当前帧
	int desty;//目标y坐标
	bool used;//是否被使用
	int timer;//计时器
	int isfalling;//是否正在下落
	int quality;//阳光的价值
};
struct SunshineBall sunshine[10];
IMAGE sunshine_img[22];

bool fileExist(const char* name)
{
	FILE* fp = fopen(name, "r");
	if (fp == NULL)
		return false;
	else
	{
		fclose(fp);
		return true;
	}
}

//敌人的相关操作
struct B2 {
	int x, y;
	int row;
	int frameIndex; //帧序号
	int movetype;//1是攻击，2是死亡，3是移动
	bool used; //是否登场
	int speed;
	int health_point; //血条
	bool operator < (const B2& b) const
	{
		return x < b.x;
	}
	bool eating; //正在吃植物
	int hurt_point; //攻击伤害
	int Eat_timer;
};//源石虫β
struct B2 b2[10];
IMAGE* b2Image[3][20];

struct Runner {
	int x, y;
	int row;
	int frameIndex;
	int movetype;//1是攻击，2是死亡，3是移动，4是跑步
	bool used;
	int speed;
	int health_point;
	bool operator < (const Runner& b) const
	{
		return x < b.x;
	}
	bool eating; //正在吃植物
	int hurt_point; //攻击伤害
	int Eat_timer;
};//染污躯壳：受到攻击后变为跑步动画，移动速度变为4
struct Runner runner[10];
IMAGE* runnerImage[4][20];

struct Ghost {
	int x, y;
	int row;
	int frameIndex;
	int movetype;//1是移动，2是死亡
	bool used;
	int speed;
	int health_point;
	bool operator < (const Ghost& b) const
	{
		return x < b.x;
	}
};
struct Ghost ghost[10];
IMAGE* ghostImage[2][20];

struct C0 {
	int x, y;
	int row;
	int frameIndex;
	int movetype;//1是攻击，2是死亡，3是移动
	bool used;
	int speed;
	int health_point;
	bool operator < (const C0& b) const
	{
		return x < b.x;
	}
	bool eating; //正在吃植物
	int hurt_point; //攻击伤害
	int Eat_timer;
};
struct C0 c0[10];
IMAGE* c0Image[3][20];

struct S2 {
	int x, y;
	int row;
	int frameIndex; //帧序号
	int movetype;//1是攻击，2是死亡，3是移动
	bool used; //是否登场
	int speed;
	int health_point; //血条
	bool operator < (const S2& b) const
	{

		return x < b.x;
	}
	bool eating; //正在吃植物
	int hurt_point; //攻击伤害
	int Eat_timer;
};//萨卡兹大剑手
struct S2 s2[10];
IMAGE* s2Image[3][20];

struct Sprise {
	int x, y;
	int row;
	int frameIndex; //帧序号
	int movetype;//1是攻击，2是死亡，3是移动，4是入场
	bool used; //是否登场
	int speed;
	int health_point; //血条
	bool operator < (const Sprise& b) const
	{
		return x < b.x;
	}
	bool eating; //正在吃植物
	int hurt_point; //攻击伤害
	int Eat_timer;
};
struct Sprise sprise[10];
IMAGE* spriseImage[4][20];

struct S10 {
	int x, y;
	int row;
	int frameIndex; //帧序号
	int movetype;//1是攻击，2是死亡，3是移动
	bool used; //是否登场
	int speed;
	int health_point; //血条
	bool operator < (const S10& b) const
	{
		return x < b.x;
	}
	bool eating; //正在吃植物
	int hurt_point; //攻击伤害
	int Eat_timer;
};//萨卡兹穿刺手：移动时速度不断提升，第一次伤害受到移动速度加成
struct S10 s10[10];
IMAGE* s10Image[3][20];

void Enemy_Ini() //初始化敌人图像
{
	memset(b2, 0, sizeof(b2));
	memset(b2Image, 0, sizeof(b2Image));
	char path[64];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			sprintf_s(path, sizeof(path), "res/Zombie/B2/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				b2Image[i][j] = new IMAGE;
				loadimage(b2Image[i][j], path);
			}
			else
				break;
		}
	}

	memset(runner, 0, sizeof(runner));
	memset(runnerImage, 0, sizeof(runnerImage));
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(path, sizeof(path), "res/Zombie/Runner/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				runnerImage[i][j] = new IMAGE;
				loadimage(runnerImage[i][j], path);
			}
			else
				break;
		}
	}

	memset(ghost, 0, sizeof(ghost));
	memset(ghostImage, 0, sizeof(ghostImage));
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(path, sizeof(path), "res/Zombie/Ghost/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				ghostImage[i][j] = new IMAGE;
				loadimage(ghostImage[i][j], path);
			}
			else
				break;
		}
	}

	memset(c0, 0, sizeof(c0));
	memset(c0Image, 0, sizeof(c0Image));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(path, sizeof(path), "res/Zombie/C0/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				c0Image[i][j] = new IMAGE;
				loadimage(c0Image[i][j], path);
			}
			else
				break;
		}
	}

	memset(s2, 0, sizeof(s2));
	memset(s2Image, 0, sizeof(s2Image));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			sprintf_s(path, sizeof(path), "res/Zombie/S2/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				s2Image[i][j] = new IMAGE;
				loadimage(s2Image[i][j], path);
			}
			else
				break;
		}
	}

	memset(sprise, 0, sizeof(sprise));
	memset(spriseImage, 0, sizeof(spriseImage));
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(path, sizeof(path), "res/Zombie/Surprise/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				spriseImage[i][j] = new IMAGE;
				loadimage(spriseImage[i][j], path);
			}
			else
				break;
		}
	}

	memset(s10, 0, sizeof(s10));
	memset(s10Image, 0, sizeof(s10Image));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(path, sizeof(path), "res/Zombie/S10/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				s10Image[i][j] = new IMAGE;
				loadimage(s10Image[i][j], path);
			}
			else
				break;
		}
	}
}

void B2_Create() //创建B2敌人
{
	int i;
	int B2MAX = sizeof(b2) / sizeof(b2[0]);
	for (i = 0; i < B2MAX and b2[i].used != 0; i++);
	if (i >= B2MAX)
		return;
	memset(&b2[i], 0, sizeof(b2[i]));
	b2[i].used = true;
	b2[i].frameIndex = 0;
	b2[i].movetype = 3;
	b2[i].speed = 1;
	b2[i].x = 1250;
	int row = rand() % 5;
	b2[i].row = row;
	b2[i].y = 70 + row * 103;
	b2[i].health_point = 50 + rand() % 40;
	b2[i].hurt_point = 30;
	printf("create B2 at row %d\n", row);
}

void Runner_Create() //创建Runner敌人
{
	int i;
	int RunnerMAX = sizeof(runner) / sizeof(runner[0]);
	for (i = 0; i < RunnerMAX and runner[i].used != 0; i++);
	if (i >= RunnerMAX)
		return;
	runner[i].used = true;
	runner[i].frameIndex = 0;
	runner[i].movetype = 3;
	runner[i].speed = 1;
	runner[i].x = 1250;
	int row = rand() % 5;
	runner[i].row = row;
	runner[i].y = 70 + row * 103;
	runner[i].health_point = 120 + rand() % 40;
	runner[i].hurt_point = 30;
	printf("create Runner at row %d\n", row);
}

void Ghost_Create() //创建Ghost敌人
{
	int i;
	int GhostMAX = sizeof(ghost) / sizeof(ghost[0]);
	for (i = 0; i < GhostMAX and ghost[i].used != 0; i++);
	if (i >= GhostMAX)
		return;
	ghost[i].used = true;
	ghost[i].frameIndex = 0;
	ghost[i].movetype = 1;
	ghost[i].speed = 1;
	ghost[i].x = 1250;
	int row = rand() % 5;
	ghost[i].row = row;
	ghost[i].y = 70 + row * 103;
	ghost[i].health_point = 80 + rand() % 30;
	// printf("create Ghost at row %d\n", row);
}

void C0_Create() //创建C0敌人
{
	int i;
	int C0MAX = sizeof(c0) / sizeof(c0[0]);
	for (i = 0; i < C0MAX and c0[i].used != 0; i++);
	if (i >= C0MAX)
		return;
	c0[i].used = true;
	c0[i].frameIndex = 0;
	c0[i].movetype = 3;
	c0[i].speed = 2;
	c0[i].x = 1250;
	int row;
	if (map_hard == 3)
		row = 1 + rand() % 3;
	else
		row = rand() % 5;
	c0[i].row = row;
	c0[i].y = 70 + row * 103;
	c0[i].health_point = 60 + rand() % 20;
	c0[i].hurt_point = 30;
	printf("create C0 at row %d\n", row);
}

void S2_Create() //创建B2敌人
{
	int i;
	int S2MAX = sizeof(s2) / sizeof(s2[0]);
	for (i = 0; i < S2MAX and s2[i].used != 0; i++);
	if (i >= S2MAX)
		return;
	memset(&s2[i], 0, sizeof(s2[i]));
	s2[i].used = true;
	s2[i].frameIndex = 0;
	s2[i].movetype = 3;
	s2[i].speed = 1;
	s2[i].x = 1250;
	int row = rand() % 5;
	s2[i].row = row;
	s2[i].y = 70 + row * 103;
	s2[i].health_point = 100 + rand() % 30;
	s2[i].hurt_point = 50;
	printf("create S2 at row %d\n", row);
}

void Surprise_Create() //创建B2敌人
{
	int i;
	int SpriseMAX = sizeof(sprise) / sizeof(sprise[0]);
	for (i = 0; i < SpriseMAX and sprise[i].used != 0; i++);
	if (i >= SpriseMAX)
		return;
	memset(&sprise[i], 0, sizeof(sprise[i]));
	sprise[i].used = true;
	sprise[i].frameIndex = 0;
	sprise[i].movetype = 4;
	sprise[i].speed = 0;
	sprise[i].x = 950 + rand() % 350;
	int row = rand() % 5;
	sprise[i].row = row;
	sprise[i].y = 70 + row * 103;
	sprise[i].health_point = 100 + rand() % 30;
	sprise[i].hurt_point = 25;
	printf("create Sprise at (%d,%d)\n", sprise[i].x, sprise[i].y);
}

void S10_Create()
{
	int i;
	int S10MAX = sizeof(s10) / sizeof(s10[0]); // s10容量
	for (i = 0; i < S10MAX and s10[i].used != 0; i++);  //循环遍历找到未使用的s10
	if (i >= S10MAX)
		return;
	memset(&s10[i], 0, sizeof(s10[i]));
	s10[i].used = true;
	s10[i].frameIndex = 0;
	s10[i].movetype = 3;
	s10[i].speed = 1;
	s10[i].x = 1250;
	int row;
	if (map_hard == 3)
		row = 1 + rand() % 3;
	else
		row = rand() % 5;
	//	初始化创造位置
	s10[i].row = row;
	s10[i].y = 70 + row * 103;
	s10[i].health_point = 100 + rand() % 30;
	s10[i].hurt_point = 30;
	// printf("create S10 at row %d\n", row);
}

int Enemy_timer = 0;
int Enemy_frequncy = 800;
int Enemy_flag = 0;//波数越高，僵尸数量和强度越高

void Enemy_Create() //根据敌人出现频率创建不同敌人
{
	Enemy_timer++;
	//printf("Enemy_timer=%d\n", Enemy_timer);
	if (Enemy_timer > Enemy_frequncy)
	{
		//Enemy_frequncy = 300 + rand() % 400;
		Enemy_frequncy = 200 + rand() % 200; //更新了频率让敌人早点出现
		Enemy_timer = 0;
		Enemy_flag++;
		int cnt = rand() % min((int)Enemy_flag, 40) + 1;
		for (int i = 0; i < cnt; i++)
		{
			int type = rand() % min(Enemy_flag * Enemy_flag, Enemy_Type_Cnt) + 1;
			if (type == 1) B2_Create();
			else if (type == 2) Runner_Create();
			else if (type == 3) Ghost_Create();
			else if (type == 4) C0_Create();
			else if (type == 5) Surprise_Create();
			else if (type == 6) S2_Create();
			else if (type == 7)
				if (Enemy_flag > 15) // 已经创造过一定敌人数量后，s10才出现
					S10_Create();
		}
	}
}

void Enemy_Update() //更新敌人状态
{
	//B2的更新
	int B2MAX = sizeof(b2) / sizeof(b2[0]);
	for (int i = 0; i < B2MAX; i++)
	{
		if (b2[i].used)
		{
			b2[i].frameIndex++;
			if (b2Image[b2[i].movetype - 1][b2[i].frameIndex] == NULL)
			{
				if (b2[i].movetype == 2)
				{
					b2[i].used = false; // 如果僵尸死亡，标记为未使用
					printf("B2 died completely at %d %d\n", b2[i].x, b2[i].y);
					continue;
				}
				b2[i].frameIndex = 0;
			}
			if (!b2[i].eating) // 没有在吃植物
			{
				b2[i].x -= b2[i].speed;//更新僵尸的位置
				if (b2[i].x < 390)
				{
					printf("Game Over\n");
					//MessageBox(NULL, "Game Over", "Game Over", 0
					gameOver = true;
					//exit(0);
				}
			}
			if (b2[i].health_point <= 0 and b2[i].movetype != 2)
			{
				b2[i].movetype = 2;
				b2[i].speed = 0;
				b2[i].frameIndex = 0;
				b2[i].used = false;
			}//更新僵尸的血量
			// 碰撞检测
			bool stop = false;
			int j;
			for (j = 0; j < 9; j++)
			{
				// 遍历地图，僵尸距离植物小于+78就停下
				if (scence[b2[i].row][j].type != 0 and scence[b2[i].row][j].type != 4 and b2[i].x < 458 + j * 87 + 78)
				{
					stop = true; // 僵尸停止
					break;
				}
			}
			if (stop)
			{
				b2[i].eating = true;
				b2[i].speed = 0;
				b2[i].Eat_timer++;
				if (b2[i].Eat_timer >= 10)
				{
					b2[i].Eat_timer = 0;
					scence[b2[i].row][j].health_point -= b2[i].hurt_point;
					if (scence[b2[i].row][j].type == 6)
					{
						b2[i].health_point -= b2[i].hurt_point / 5;
					}
				}
				b2[i].movetype = 1;
				b2[i].frameIndex = 0;
			}
			else
			{
				b2[i].eating = false;
				b2[i].speed = 1;
				if (b2[i].movetype == 1)
				{
					b2[i].movetype = 3;
					b2[i].frameIndex = 0;
				}
			}
		}
	}

	//Runner的更新
	int RunnerMAX = sizeof(runner) / sizeof(runner[0]);
	for (int i = 0; i < RunnerMAX; i++)
	{
		if (runner[i].used)
		{
			runner[i].frameIndex++;
			// printf("%d", runner[i].movetype);
			if (runnerImage[runner[i].movetype - 1][runner[i].frameIndex] == NULL)
			{
				if (runner[i].movetype == 2)
				{
					//printf("YES \n");
					runner[i].used = false; // 这里死亡动画演示结束了
					// runner[i].x = 0;
					// runner[i].y = 0;
					// runner[i].movetype = 3;
					printf("Runner died completely at %d %d\n", runner[i].x, runner[i].y);
					continue;
				}
				runner[i].frameIndex = 0;
			}
			if (!runner[i].eating)
			{
				runner[i].x -= runner[i].speed; // 前移
				if (runner[i].movetype == 1)
					runner[i].speed = 1;
				else if (runner[i].movetype == 3)
					runner[i].speed = 2;
				if (runner[i].x < 390)
				{
					printf("Game Over\n");
					//MessageBox(NULL, "Game Over", "Game Over", 0
					gameOver = true;
					//exit(0);
				}
			}
			if (runner[i].health_point <= 0 and runner[i].movetype != 2)
			{
				//printf("OK");
				runner[i].movetype = 2;
				runner[i].frameIndex = 0;
				runner[i].speed = 0;
			}
			// 碰撞检测
			bool stop = false;
			int j;
			for (j = 0; j < 9; j++)
			{
				if (scence[runner[i].row][j].type != 0 and scence[runner[i].row][j].type != 4 and runner[i].x < 458 + j * 87 + 78)
				{
					stop = true;
					break;
				}
			}
			if (stop and runner[i].movetype != 2)
			{
				runner[i].eating = true;
				runner[i].speed = 0;
				runner[i].Eat_timer++;
				if (runner[i].Eat_timer >= 10)
				{
					runner[i].Eat_timer = 0;
					scence[runner[i].row][j].health_point -= runner[i].hurt_point;
				}
				if (runner[i].movetype != 1)
				{
					runner[i].frameIndex = 0;
					runner[i].movetype = 1;
				}
				if (scence[b2[i].row][j].type == 6)
				{
					b2[i].health_point -= b2[i].hurt_point / 5;
				}
			}
			else if (runner[i].movetype == 1)
			{
				runner[i].eating = false;
				runner[i].movetype = 3;
				runner[i].speed = 1;
				runner[i].frameIndex = 0;
			}
		}
	}

	//Ghost的更新
	int GhostMAX = sizeof(ghost) / sizeof(ghost[0]);
	for (int i = 0; i < GhostMAX; i++)
	{
		if (ghost[i].used)
		{
			ghost[i].frameIndex++;
			if (ghostImage[ghost[i].movetype - 1][ghost[i].frameIndex] == NULL)
			{
				if (ghost[i].movetype == 2)
				{
					ghost[i].used = false; // 如果僵尸死亡，标记为未使用
					// printf("Ghost died completely at %d %d\n", ghost[i].x, ghost[i].y);
					continue;
				}
				ghost[i].frameIndex = 0;
			}
			ghost[i].x -= ghost[i].speed;//更新僵尸的位置
			if (ghost[i].x < 390)
			{
				printf("Game Over\n");
				//MessageBox(NULL, "Game Over", "Game Over", 0
				gameOver = true;
				//exit(0);
			}
			if (ghost[i].health_point <= 0 and ghost[i].movetype != 2)
			{
				ghost[i].movetype = 2;
				ghost[i].speed = 0;
				ghost[i].frameIndex = 0;
				ghost[i].used = false;
			}//更新僵尸的血量
		}
	}

	//C0的更新
	int C0MAX = sizeof(c0) / sizeof(c0[0]);
	for (int i = 0; i < C0MAX; i++)
	{
		if (c0[i].used)
		{
			c0[i].frameIndex++;
			if (c0Image[c0[i].movetype - 1][c0[i].frameIndex] == NULL)
			{
				if (c0[i].movetype == 2)
				{
					c0[i].used = false; // 如果僵尸死亡，标记为未使用
					printf("C0 died completely at %d %d\n", c0[i].x, c0[i].y);
					continue;
				}
				c0[i].frameIndex = 0;
			}
			if (!c0[i].eating) // 没有在吃植物
			{
				c0[i].x -= c0[i].speed;//更新僵尸的位置
				if (c0[i].x < 390)
				{
					//MessageBox(NULL, "Game Over", "Game Over", 0
					gameOver = true;
					//exit(0);
				}
			}
			if (c0[i].health_point <= 0 and c0[i].movetype != 2)
			{
				c0[i].movetype = 2;
				c0[i].speed = 0;
				c0[i].frameIndex = 0;
				c0[i].used = false;
			}//更新僵尸的血量
			// 碰撞检测
			bool stop = false;
			int j;
			for (j = 0; j < 9; j++)
			{
				// 遍历地图，僵尸距离植物小于+78就停下
				if (scence[c0[i].row][j].type != 0 and scence[c0[i].row][j].type != 4 and c0[i].x < 458 + j * 87 + 78)
				{
					stop = true; // 僵尸停止
					break;
				}
			}
			if (stop)
			{
				c0[i].eating = true;
				c0[i].speed = 0;
				c0[i].Eat_timer++;
				if (c0[i].Eat_timer >= 10)
				{
					c0[i].Eat_timer = 0;
					scence[c0[i].row][j].health_point -= c0[i].hurt_point;
				}
				if (c0[i].movetype != 1)
				{
					c0[i].frameIndex = 0;
					c0[i].movetype = 1;
				}
				if (scence[b2[i].row][j].type == 6)
				{
					b2[i].health_point -= b2[i].hurt_point / 5;
				}
			}
			else
			{
				c0[i].eating = false;
				c0[i].speed = 2;
				if (c0[i].movetype == 1)
				{
					c0[i].movetype = 3;
					c0[i].frameIndex = 0;
				}
			}
		}
	}

	//S2的更新
	int S2MAX = sizeof(s2) / sizeof(s2[0]);
	for (int i = 0; i < S2MAX; i++)
	{
		if (s2[i].used)
		{
			s2[i].frameIndex++;
			if (s2Image[s2[i].movetype - 1][s2[i].frameIndex] == NULL)
			{
				if (s2[i].movetype == 2)
				{
					s2[i].used = false;
					printf("S2 died completely at %d %d\n", s2[i].x, s2[i].y);
					continue;
				}
				s2[i].frameIndex = 0;
			}
			s2[i].x -= s2[i].speed;
			if (s2[i].x < 390)
			{
				printf("Game Over\n");
				//MessageBox(NULL, "Game Over", "Game Over", 0
				gameOver = true;
				//exit(0);
			}
			if (s2[i].health_point <= 0 and s2[i].movetype != 2)
			{
				s2[i].movetype = 2;
				s2[i].speed = 0;
				s2[i].frameIndex = 0;
				s2[i].used = false;
			}
			bool stop = false;
			int j;
			for (j = 0; j < 9; j++)
			{
				if (scence[s2[i].row][j].type != 0 and scence[s2[i].row][j].type != 4 and s2[i].x < 458 + j * 87 + 78)
				{
					stop = true;
					break;
				}
			}
			if (stop)
			{
				s2[i].eating = true;
				s2[i].speed = 0;
				s2[i].Eat_timer++;
				if (s2[i].Eat_timer >= 10)
				{
					s2[i].Eat_timer = 0;
					scence[s2[i].row][j].health_point -= s2[i].hurt_point;
				}
				if (s2[i].movetype != 1)
				{
					s2[i].frameIndex = 0;
					s2[i].movetype = 1;
				}
				if (scence[b2[i].row][j].type == 6)
				{
					b2[i].health_point -= b2[i].hurt_point / 5;
				}
			}
			else
			{
				s2[i].eating = false;
				s2[i].speed = 1;
				if (s2[i].movetype == 1)
				{
					s2[i].movetype = 3;
					s2[i].frameIndex = 0;
				}
			}
		}
	}

	//Sprise的更新
	int SpriseMAX = sizeof(sprise) / sizeof(sprise[0]);
	for (int i = 0; i < SpriseMAX; i++)
	{
		if (sprise[i].used)
		{
			sprise[i].frameIndex++;
			if (spriseImage[sprise[i].movetype - 1][sprise[i].frameIndex] == NULL)
			{
				if (sprise[i].movetype == 2)
				{
					sprise[i].used = false;
					printf("Sprise died completely at %d %d\n", sprise[i].x, sprise[i].y);
					continue;
				}
				if (sprise[i].movetype == 4)
				{
					sprise[i].movetype = 3;
					sprise[i].speed = 1;
					sprise[i].frameIndex = 0;
				}
				sprise[i].frameIndex = 0;
			}
			sprise[i].x -= sprise[i].speed;
			if (sprise[i].x < 390)
			{
				printf("Game Over\n");
				//MessageBox(NULL, "Game Over", "Game Over", 0
				gameOver = true;
				//exit(0);
			}
			if (sprise[i].health_point <= 0 and sprise[i].movetype != 2)
			{
				sprise[i].movetype = 2;
				sprise[i].speed = 0;
				sprise[i].frameIndex = 0;
				sprise[i].used = false;
			}
			bool stop = false;
			int j;
			for (j = 0; j < 9; j++)
			{
				if (scence[sprise[i].row][j].type != 0 and scence[sprise[i].row][j].type != 4 and sprise[i].x < 458 + j * 87 + 78)
				{
					stop = true;
					break;
				}
			}
			if (stop)
			{
				sprise[i].eating = true;
				sprise[i].speed = 0;
				sprise[i].Eat_timer++;
				if (sprise[i].Eat_timer >= 10)
				{
					sprise[i].Eat_timer = 0;
					scence[sprise[i].row][j].health_point -= sprise[i].hurt_point;
				}
				if (sprise[i].movetype != 1)
				{
					sprise[i].frameIndex = 0;
					sprise[i].movetype = 1;
				}
				if (scence[b2[i].row][j].type == 6)
				{
					b2[i].health_point -= b2[i].hurt_point / 5;
				}
			}
			else
			{
				sprise[i].eating = false;
				sprise[i].speed = 1;
				if (sprise[i].movetype == 1)
				{
					sprise[i].movetype = 3;
					sprise[i].frameIndex = 0;
				}
			}
		}
	}

	//S10的更新
	int S10MAX = sizeof(s10) / sizeof(s10[0]);
	for (int i = 0; i < S10MAX; i++)
	{
		if (s10[i].used)
		{
			s10[i].frameIndex++;
			if (s10Image[s10[i].movetype - 1][s10[i].frameIndex] == NULL)
			{
				if (s10[i].movetype == 2)
				{
					s10[i].used = false;
					printf("S10 died completely at %d %d\n", s10[i].x, s10[i].y);
					continue;
				}
				s10[i].frameIndex = 0;
			}
			s10[i].x -= s10[i].speed;
			s10[i].speed++;
			if (s10[i].x < 390)
			{
				printf("Game Over\n");
				//MessageBox(NULL, "Game Over", "Game Over", 0
				gameOver = true;
				//exit(0);
			}
			if (s10[i].health_point <= 0 and s10[i].movetype != 2)
			{
				s10[i].movetype = 2;
				s10[i].speed = 0;
				s10[i].frameIndex = 0;
				s10[i].used = false;
			}
			bool stop = false;
			int j;
			for (j = 0; j < 9; j++)
			{
				if (scence[s10[i].row][j].type != 0 and scence[s10[i].row][j].type != 4 and s10[i].x < 458 + j * 87 + 78)
				{
					scence[s10[i].row][j].health_point -= s10[i].speed * 5;
					//printf("S10 attack %d at %d %d a damage of %d\n", scence[s10[i].row][j].type, s10[i].x, s10[i].y, s10[i].speed * 5);
					stop = true;
					break;
				}
			}
			if (stop)
			{
				s10[i].eating = true;
				s10[i].speed = 0;
				s10[i].Eat_timer++;
				if (s10[i].Eat_timer >= 10)
				{
					s10[i].Eat_timer = 0;
					scence[s10[i].row][j].health_point -= s10[i].hurt_point;
				}
				if (s10[i].movetype != 1)
				{
					s10[i].frameIndex = 0;
					s10[i].movetype = 1;
				}
				if (scence[b2[i].row][j].type == 6)
				{
					b2[i].health_point -= b2[i].hurt_point / 5;
				}
			}
			else
			{
				s10[i].eating = false;
				s10[i].speed++;
				if (s10[i].movetype == 1)
				{
					s10[i].movetype = 3;                                                                              
					s10[i].frameIndex = 0;
				}
			}
		}
	}
}

void Enemy_Draw() //绘制敌人
{
	//B2的渲染
	int B2MAX = sizeof(b2) / sizeof(b2[0]);
	for (int i = 0; i < B2MAX; i++)
	{
		if (b2[i].used)
		{
			putimagePNG(b2[i].x, b2[i].y, &shadow);
			putimagePNG(b2[i].x, b2[i].y, b2Image[b2[i].movetype - 1][b2[i].frameIndex]);
		}
	}

	//Runner的渲染
	int RunnerMAX = sizeof(runner) / sizeof(runner[0]);
	for (int i = 0; i < RunnerMAX; i++)
	{
		if (runner[i].used)
		{
			putimagePNG(runner[i].x, runner[i].y, &shadow);
			putimagePNG(runner[i].x, runner[i].y, runnerImage[runner[i].movetype - 1][runner[i].frameIndex]);
		}
	}

	//Ghost的渲染
	int GhostMAX = sizeof(ghost) / sizeof(ghost[0]);
	for (int i = 0; i < GhostMAX; i++)
	{
		if (ghost[i].used)
		{
			putimagePNG(ghost[i].x, ghost[i].y, &shadow);
			putimagePNG(ghost[i].x, ghost[i].y, ghostImage[ghost[i].movetype - 1][ghost[i].frameIndex]);
		}
	}

	//C0的渲染
	int C0MAX = sizeof(c0) / sizeof(c0[0]);
	for (int i = 0; i < C0MAX; i++)
	{
		if (c0[i].used)
		{
			putimagePNG(c0[i].x, c0[i].y, &shadow);
			putimagePNG(c0[i].x, c0[i].y, c0Image[c0[i].movetype - 1][c0[i].frameIndex]);
		}
	}

	//S2的渲染
	int S2MAX = sizeof(s2) / sizeof(s2[0]);
	for (int i = 0; i < S2MAX; i++)
	{
		if (s2[i].used)
		{
			putimagePNG(s2[i].x, s2[i].y, &shadow);
			putimagePNG(s2[i].x, s2[i].y, s2Image[s2[i].movetype - 1][s2[i].frameIndex]);
		}
	}

	//Sprise的渲染
	int SpriseMAX = sizeof(sprise) / sizeof(sprise[0]);
	for (int i = 0; i < SpriseMAX; i++)
	{
		if (sprise[i].used)
		{
			putimagePNG(sprise[i].x, sprise[i].y, &shadow);
			putimagePNG(sprise[i].x, sprise[i].y, spriseImage[sprise[i].movetype - 1][sprise[i].frameIndex]);
		}
	}

	//S10的渲染
	int S10MAX = sizeof(s10) / sizeof(s10[0]);
	for (int i = 0; i < S10MAX; i++)
	{
		if (s10[i].used)
		{
			putimagePNG(s10[i].x, s10[i].y, &shadow);
			putimagePNG(s10[i].x, s10[i].y, s10Image[s10[i].movetype - 1][s10[i].frameIndex]);
		}
	}
}
//敌人结束

//子弹操作部分
struct bullet {
	int x, y;
	int type;//代表是第几种植物的子弹
	int row;
	bool used;
	int speed;
	int hurt_point; //攻击伤害（？
};
struct bullet bullets[514];
IMAGE bulletImage[Card_cnt];
std::set<int> bullet_set; // 子弹set优化

void Bullet_Ini() //初始化子弹图像
{
	memset(bullets, 0, sizeof(bullets));
	char path[64];
	for (int i = 0; i < Card_cnt; i++)
	{
		sprintf_s(path, sizeof(path), "res/Bullet/%d.png", i + 1);
		loadimage(&bulletImage[i], path);
	}
}

void shoot(int PlantType, int x, int y) //根据植物类型生成相应子弹（传入植物类型，x、y坐标）
{
	int i;
	int bulletMAX = sizeof(bullets) / sizeof(bullets[0]);
	for (i = 0; i < bulletMAX and bullets[i].used != 0; i++);
	if (i >= bulletMAX)
		return;
	else
		bullet_set.insert(i);
	bullets[i].used = true;
	bullets[i].type = PlantType; // 传入植物类型，生成对应的子弹
	bullets[i].y = y;
	if (PlantType == 1)
		bullets[i].x = x + 30;
	else if (PlantType == 2)
		bullets[i].x = x + 63;
	else if (PlantType == 3)
		bullets[i].x = x + 28;
	else if (PlantType == 7)
		bullets[i].x = x - 42;
	bullets[i].row = (y - 70) / 103;
	bullets[i].speed = 8;
	bullets[i].hurt_point = hurt_point[PlantType - 1];
}

bool haveZombie(int PlantType, int row, int col) //判断是否有僵尸存在
{
	int B2MAX = sizeof(b2) / sizeof(b2[0]);
	for (int i = 0; i < B2MAX; i++)
	{
		if (b2[i].used and b2[i].row == row and b2[i].x < 458 + col * 87 + danger_zone[PlantType - 1] * 87)
			return true;
	}
	int RunnerMAX = sizeof(runner) / sizeof(runner[0]);
	for (int i = 0; i < RunnerMAX; i++)
	{
		if (runner[i].used and runner[i].row == row and runner[i].x < 458 + col * 87 + danger_zone[PlantType - 1] * 87)
			return true;
	}
	int GhostMAX = sizeof(ghost) / sizeof(ghost[0]);
	for (int i = 0; i < GhostMAX; i++)
	{
		if (ghost[i].used and ghost[i].row == row and ghost[i].x < 458 + col * 87 + danger_zone[PlantType - 1] * 87)
			return true;
	}
	int C0MAX = sizeof(c0) / sizeof(c0[0]);
	for (int i = 0; i < C0MAX; i++)
	{
		if (c0[i].used and c0[i].eating and c0[i].row == row and c0[i].x < 458 + col * 87 + danger_zone[PlantType - 1] * 87)
			return true;
	}
	int S2MAX = sizeof(s2) / sizeof(s2[0]);
	for (int i = 0; i < S2MAX; i++)
	{
		if (s2[i].used and s2[i].row == row and s2[i].x < 458 + col * 87 + danger_zone[PlantType - 1] * 87)
			return true;
	}
	int SpriseMAX = sizeof(sprise) / sizeof(sprise[0]);
	for (int i = 0; i < SpriseMAX; i++)
	{
		if (sprise[i].used and (sprise[i].movetype == 1 or sprise[i].movetype == 3) and sprise[i].row == row and sprise[i].x < 458 + col * 87 + danger_zone[PlantType - 1] * 87)
			return true;
	}
	int S10MAX = sizeof(s10) / sizeof(s10[0]);
	for (int i = 0; i < S10MAX; i++)
	{
		if (s10[i].used and s10[i].row == row and s10[i].x < 458 + col * 87 + danger_zone[PlantType - 1] * 87)
			return true;
	}
	return false;
}

// 没看懂这段是干啥，晚点回来看。                                                     
bool haveZombieMizuki(int row, int col)//所在格上下左右一圈有僵尸
{
	int B2MAX = sizeof(b2) / sizeof(b2[0]);
	for (int i = 0; i < B2MAX; i++)
	{
		if (b2[i].used and b2[i].row >= row - 1 and b2[i].row <= row + 1 and b2[i].x < 458 + col * 84 + 84 and b2[i].x>458 + col * 84 - 84)
			return true;
	}
	int RunnerMAX = sizeof(runner) / sizeof(runner[0]);
	for (int i = 0; i < RunnerMAX; i++)
	{
		if (runner[i].used and runner[i].row >= row - 1 and runner[i].row <= row + 1 and runner[i].x < 458 + col * 84 + 84 and runner[i].x>458 + col * 84 - 84)
			return true;
	}
	int GhostMAX = sizeof(ghost) / sizeof(ghost[0]);
	for (int i = 0; i < GhostMAX; i++)
	{
		if (ghost[i].used and ghost[i].row >= row - 1 and ghost[i].row <= row + 1 and ghost[i].x < 458 + col * 84 + 84 and ghost[i].x>458 + col * 84 - 84)
			return true;
	}
	int C0MAX = sizeof(c0) / sizeof(c0[0]);
	for (int i = 0; i < C0MAX; i++)
	{
		if (c0[i].used and c0[i].eating and c0[i].row >= row - 1 and c0[i].row <= row + 1 and c0[i].x < 458 + col * 84 + 84 and c0[i].x>458 + col * 84 - 84)
			return true;
	}
	int S2MAX = sizeof(s2) / sizeof(s2[0]);
	for (int i = 0; i < S2MAX; i++)
	{
		if (s2[i].used and s2[i].row >= row - 1 and s2[i].row <= row + 1 and s2[i].x < 458 + col * 84 + 84 and s2[i].x>458 + col * 84 - 84)
			return true;
	}
	int SpriseMAX = sizeof(sprise) / sizeof(sprise[0]);
	for (int i = 0; i < SpriseMAX; i++)
	{
		if (sprise[i].used and (sprise[i].movetype == 1 or sprise[i].movetype == 3) and sprise[i].row >= row - 1 and sprise[i].row <= row + 1 and sprise[i].x < 458 + col * 84 + 84 and sprise[i].x>458 + col * 84 - 84)
			return true;
	}
	int S10MAX = sizeof(s10) / sizeof(s10[0]);
	for (int i = 0; i < S10MAX; i++)
	{
		if (s10[i].used and s10[i].row >= row - 1 and s10[i].row <= row + 1 and s10[i].x < 458 + col * 84 + 84 and s10[i].x>458 + col * 84 - 84)
			return true;
	}
	return false;
}

void attack_zombie_mizuki(int row, int col)//攻击僵尸
{
	int B2MAX = sizeof(b2) / sizeof(b2[0]);
	for (int i = 0; i < B2MAX; i++)
	{
		if (b2[i].used and b2[i].row >= row - 1 and b2[i].row <= row + 1 and b2[i].x < 458 + col * 84 + 84 and b2[i].x>458 + col * 84 - 84)
		{
			b2[i].health_point -= hurt_point[3];
			if (b2[i].health_point <= 0)
			{
				b2[i].movetype = 2;
				b2[i].frameIndex = 0;
				b2[i].speed = 0;
			}
			printf("Mizuki hit %d damage to B2 at %d %d\n", hurt_point[3], b2[i].x, b2[i].y);
		}
	}
	int RunnerMAX = sizeof(runner) / sizeof(runner[0]);
	for (int i = 0; i < RunnerMAX; i++)
	{
		if (runner[i].used and runner[i].row >= row - 1 and runner[i].row <= row + 1 and runner[i].x < 458 + col * 84 + 84 and runner[i].x>458 + col * 84 - 84)
		{
			runner[i].health_point -= hurt_point[3];
			if (runner[i].health_point <= 0)
			{
				runner[i].movetype = 2;
				runner[i].frameIndex = 0;
				runner[i].speed = 0;
			}
			if (runner[i].movetype == 3)
			{
				runner[i].speed = 4;
				runner[i].movetype = 4;
				runner[i].frameIndex = 0;
			}
			// printf("Mizuki hit %d damage to Runner at %d %d\n", hurt_point[3], runner[i].x, runner[i].y);
		}
	}
	int GhostMAX = sizeof(ghost) / sizeof(ghost[0]);
	for (int i = 0; i < GhostMAX; i++)
	{
		if (ghost[i].used and ghost[i].row >= row - 1 and ghost[i].row <= row + 1 and ghost[i].x < 458 + col * 84 + 84 and ghost[i].x>458 + col * 84 - 84)
		{
			ghost[i].health_point -= hurt_point[3];
			if (ghost[i].health_point <= 0)
			{
				ghost[i].movetype = 2;
				ghost[i].frameIndex = 0;
				ghost[i].speed = 0;
			}
			// printf("Mizuki hit %d damage to Ghost at %d %d\n", hurt_point[3], ghost[i].x, ghost[i].y);
		}
	}
	int C0MAX = sizeof(c0) / sizeof(c0[0]);
	for (int i = 0; i < C0MAX; i++)
	{
		if (c0[i].used and c0[i].eating and c0[i].row >= row - 1 and c0[i].row <= row + 1 and c0[i].x < 458 + col * 84 + 84 and c0[i].x>458 + col * 84 - 84)
		{
			c0[i].health_point -= hurt_point[3];
			if (c0[i].health_point <= 0)
			{
				c0[i].movetype = 2;
				c0[i].frameIndex = 0;
				c0[i].speed = 0;
			}
			// printf("Mizuki hit %d damage to C0 at %d %d\n", hurt_point[3], c0[i].x, c0[i].y);
		}
	}
	int S2MAX = sizeof(s2) / sizeof(s2[0]);
	for (int i = 0; i < S2MAX; i++)
	{
		if (s2[i].used and s2[i].row >= row - 1 and s2[i].row <= row + 1 and s2[i].x < 458 + col * 84 + 84 and s2[i].x>458 + col * 84 - 84)
		{
			s2[i].health_point -= hurt_point[3];
			if (s2[i].health_point <= 0)
			{
				s2[i].movetype = 2;
				s2[i].frameIndex = 0;
				s2[i].speed = 0;
			}
			// printf("Mizuki hit %d damage to S2 at %d %d\n", hurt_point[3], s2[i].x, s2[i].y);
		}
	}
	int SpriseMAX = sizeof(sprise) / sizeof(sprise[0]);
	for (int i = 0; i < SpriseMAX; i++)
	{
		if (sprise[i].used and (sprise[i].movetype == 1 or sprise[i].movetype == 3) and sprise[i].row >= row - 1 and sprise[i].row <= row + 1 and sprise[i].x < 458 + col * 84 + 84 and sprise[i].x>458 + col * 84 - 84)
		{
			sprise[i].health_point -= hurt_point[3];
			if (sprise[i].health_point <= 0)
			{
				sprise[i].movetype = 2;
				sprise[i].frameIndex = 0;
				sprise[i].speed = 0;
			}
			// printf("Mizuki hit %d damage to Sprise at %d %d\n", hurt_point[3], sprise[i].x, sprise[i].y);
		}
	}
	int S10MAX = sizeof(s10) / sizeof(s10[0]);
	for (int i = 0; i < S10MAX; i++)
	{
		if (s10[i].used and s10[i].row >= row - 1 and s10[i].row <= row + 1 and s10[i].x < 458 + col * 84 + 84 and s10[i].x>458 + col * 84 - 84)
		{
			s10[i].health_point -= hurt_point[3];
			if (s10[i].health_point <= 0)
			{
				s10[i].movetype = 2;
				s10[i].frameIndex = 0;
				s10[i].speed = 0;
			}
			// printf("Mizuki hit %d damage to S10 at %d %d\n", hurt_point[3], s10[i].x, s10[i].y);
		}
	}
}

std::vector<int> delect_num;
void Bullet_update()//更新子弹位置，判断是否击中敌人
{
	delect_num.clear();
	int bulletMAX = sizeof(bullets) / sizeof(bullets[0]);
	for(auto it = bullet_set.begin(); it != bullet_set.end(); ++it)
	{
		int i = *it;
		if (bullets[i].used)
		{
			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > 1370)
			{
				bullets[i].used = 0;
				continue;
			}

			int B2MAX = sizeof(b2) / sizeof(b2[0]);
			for (int j = 0; j < B2MAX; j++)
			{
				if (b2[j].used and b2[j].movetype != 2 and b2[j].row == bullets[i].row and b2[j].x < bullets[i].x and b2[j].x + 78 > bullets[i].x)
				{
					b2[j].health_point -= bullets[i].hurt_point;
					if (bullets[i].type == 2)
					{
						sunshine_cnt += 3;
					}
					bullets[i].used = 0;
					break;
				}
			}//子弹的碰撞检测 击中扣血 用完消失

			int RunnerMAX = sizeof(runner) / sizeof(runner[0]);
			for (int j = 0; j < RunnerMAX; j++)
			{
				if (runner[j].used and runner[j].movetype != 2 and runner[j].row == bullets[i].row and runner[j].x < bullets[i].x and runner[j].x + 78 > bullets[i].x)
				{
					runner[j].health_point -= bullets[i].hurt_point;
					if (runner[j].movetype == 3)
					{
						runner[j].speed = 4;
						runner[j].movetype = 4;
						runner[j].frameIndex = 0;
					}
					if (bullets[i].type == 2)
					{
						sunshine_cnt += 3;
					}
					bullets[i].used = 0;
					break;
				}
			}

			int GhostMAX = sizeof(ghost) / sizeof(ghost[0]);
			for (int j = 0; j < GhostMAX; j++)
			{
				if (ghost[j].used and ghost[j].movetype != 2 and ghost[j].row == bullets[i].row and ghost[j].x < bullets[i].x and ghost[j].x + 78 > bullets[i].x)
				{
					ghost[j].health_point -= bullets[i].hurt_point;
					if (bullets[i].type == 2)
					{
						sunshine_cnt += 3;
					}
					bullets[i].used = 0;
					break;
				}
			}

			int C0MAX = sizeof(c0) / sizeof(c0[0]);
			for (int j = 0; j < C0MAX; j++)
			{
				if (c0[j].used and c0[j].movetype != 2 and c0[j].eating and c0[j].row == bullets[i].row and c0[j].x < bullets[i].x and c0[j].x + 78 > bullets[i].x)
				{
					c0[j].health_point -= bullets[i].hurt_point;
					if (bullets[i].type == 2)
					{
						sunshine_cnt += 3;
					}
					bullets[i].used = 0;
					break;
				}
			}

			int S2MAX = sizeof(s2) / sizeof(s2[0]);
			for (int j = 0; j < S2MAX; j++)
			{
				if (s2[j].used and s2[j].movetype != 2 and s2[j].row == bullets[i].row and s2[j].x < bullets[i].x and s2[j].x + 78 > bullets[i].x)
				{
					s2[j].health_point -= bullets[i].hurt_point;
					if (bullets[i].type == 2)
					{
						sunshine_cnt += 3;
					}
					bullets[i].used = 0;
					break;
				}
			}

			int SpriseMAX = sizeof(sprise) / sizeof(sprise[0]);
			for (int j = 0; j < SpriseMAX; j++)
			{
				if (sprise[j].used and sprise[j].movetype != 2 and (sprise[j].movetype == 1 or sprise[j].movetype == 3) and sprise[j].row == bullets[i].row and sprise[j].x < bullets[i].x and sprise[j].x + 78 > bullets[i].x)
				{
					sprise[j].health_point -= bullets[i].hurt_point;
					if (bullets[i].type == 2)
					{
						sunshine_cnt += 3;
					}
					bullets[i].used = 0;
					break;
				}
			}

			int S10MAX = sizeof(s10) / sizeof(s10[0]);
			for (int j = 0; j < S10MAX; j++)
			{
				if (s10[j].used and s10[j].movetype != 2 and s10[j].row == bullets[i].row and s10[j].x < bullets[i].x and s10[j].x + 78 > bullets[i].x)
				{
					s10[j].health_point -= bullets[i].hurt_point;
					if (bullets[i].type == 2)
					{
						sunshine_cnt += 3;
					}
					bullets[i].used = 0;
					break;
				}
			}
		}
		if (!bullets[i].used)
			delect_num.push_back(i);
	}
	for (int i=0;i<delect_num.size();i++)
	{
		bullet_set.erase(delect_num[i]);
	}
}
//子弹结束

void iniGame()//初始化游戏
{
	loadimage(&gamebar, "res/bar.png");
	loadimage(&Chosing_Template, "res/ChoosingPlant.png");
	loadimage(&shadow, "res/shadow.png");
	loadimage(&card_in_shadow, "res/CardInShadow.png");
	loadimage(&Crane, "res/Crane.png");
	loadimage(&Crane_Moving, "res/Crane_Moving.png");

	loadimage(&gameOverBackground, "res/gameOverBackground.png"); // 加载背景图像
	loadimage(&gameOverImage, "res/gameOverImage.png");//加载弹出动画

	memset(scence, 0, sizeof(scence));
	memset(plant_attack, 0, sizeof(plant_attack));

	//初始化卡牌
	char path[64];
	for (int i = 0; i < Card_cnt; i++)
	{
		sprintf_s(path, sizeof(path), "res/Cards/Card_%d.png", i + 1);
		loadimage(&card[i], path);

		for (int j = 0; j < 60; j++)
		{
			sprintf_s(path, sizeof(path), "res/Plants/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				plant_attack[i][j] = new IMAGE;
				loadimage(plant_attack[i][j], path);
			}
			else
				break;
		}
	}
	for (int i = 0; i < Card_cnt; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			sprintf_s(path, sizeof(path), "res/Plants_idle/%d/%d.png", i + 1, j + 1);
			if (fileExist(path))
			{
				plant_idle[i][j] = new IMAGE;
				loadimage(plant_idle[i][j], path);
			}
			else
				break;
		}
	}

	//初始化阳光
	memset(sunshine, 0, sizeof(sunshine));
	for (int i = 0; i < 22; i++)
	{
		sprintf_s(path, sizeof(path), "res/sunshine/Sun_%d.png", i + 1);
		loadimage(&sunshine_img[i], path);
	}

	srand(time(NULL));//随机数种子

	Enemy_Ini();

	Bullet_Ini();

	initgraph(WIDTH, HEIGHT, 1);
	nowPlant = 0;

	//设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿
	settextstyle(&f);
	setbkmode(TRANSPARENT);//背景透明
	setcolor(BLACK);//字体颜色
}

void WindowUpdate()
{
	BeginBatchDraw();

	putimage(0, 0, &background);

	putimagePNG(124, 5, &gamebar);

	Enemy_Draw();//敌人的渲染

	//(63,4)->(149,7)
	for (int i = 0; i < Card_cnt; i++)
	{
		int x = 215 + i * 105;
		int y = 8;
		putimagePNG(x, y, &card[i]);
		if (sunshine_cnt < plant_sun_cost[i])
			putimagePNG(x + 2, y + 2, &card_in_shadow);
	}
	putimagePNG(1225, 5, &Crane);

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (scence[i][j].type != 0)
			{
				int x = 458 + j * 87;
				int y = 80 + i * 103;
				int plantType = scence[i][j].type - 1;
				int frameIndex = scence[i][j].frameIndex;
				putimagePNG(x, y, &shadow);
				if (scence[i][j].movetype == 1)
					putimagePNG(x, y, plant_attack[plantType][frameIndex]);
				else
					putimagePNG(x, y, plant_idle[plantType][frameIndex]);
			}
		}
	}

	//子弹的渲染
	for (int i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++)
	{
		if (bullets[i].used)
		{
			putimagePNG(bullets[i].x, bullets[i].y, &bulletImage[bullets[i].type - 1]);
		}
	}

	//阳光
	for (int i = 0; i < sizeof(sunshine) / sizeof(sunshine[0]); i++)
	{
		if (sunshine[i].used)
		{
			putimagePNG(sunshine[i].x, sunshine[i].y, &sunshine_img[sunshine[i].frameIndex]);
		}
	}

	//渲染拖动过程中的植物
	if (nowPlant != 0)
	{
		int x = 212 + (nowPlant - 1) * 105;
		int y = 3;
		putimagePNG(x, y, &Chosing_Template);
		IMAGE* img = plant_idle[nowPlant - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - (img->getheight() / 2 + 20), img);
	}

	if (Crane_Choosing == true)
	{
		putimagePNG(curX - 62, curY - 68, &Crane_Moving);
	}

	char sunshine_str[64];
	sprintf_s(sunshine_str, sizeof(sunshine_str), "%d", sunshine_cnt);
	outtextxy(144, 75, sunshine_str);

	EndBatchDraw();
}

int Sunshine_timer = 0;
int Sunshine_frequncy = 100;

void Sunshine_creat()
{
	Sunshine_timer++;
	//printf("Sunshine_timer=%d\n", Sunshine_timer);

	if (Sunshine_timer > Sunshine_frequncy)
	{
		Sunshine_frequncy = 100 + rand() % 150;
		Sunshine_timer = 0;

		int ballMAX = sizeof(sunshine) / sizeof(sunshine[0]);
		int i;
		for (i = 0; i < ballMAX and sunshine[i].used != 0; i++);
		if (i >= ballMAX)
			return;
		sunshine[i].used = true;
		sunshine[i].frameIndex = 0;
		//从550到1260
		sunshine[i].x = 550 + rand() % 710;
		//从370到600
		sunshine[i].desty = 370 + rand() % 230;
		sunshine[i].y = 120;
		sunshine[i].timer = 0;
		sunshine[i].isfalling = 1;//正在下落
		sunshine[i].quality = 25;
		printf("create sunshine at %d %d\n", sunshine[i].x, sunshine[i].y);
	}
}

void Sunshine_update() // 阳光更新
{
	for (int i = 0; i < sizeof(sunshine) / sizeof(sunshine[0]); i++)
	{
		if (sunshine[i].used)
		{
			sunshine[i].frameIndex++;
			if (sunshine[i].frameIndex >= 22)
				sunshine[i].frameIndex = 0;
			if (sunshine[i].isfalling)
			{
				if (sunshine[i].timer == 0)
				{
					sunshine[i].y += 2;
				}
				if (sunshine[i].y >= sunshine[i].desty)
				{
					sunshine[i].timer++;
					if (sunshine[i].timer > 100)
					{
						sunshine[i].used = 0;
					}
				}
			}
			else
			{
				if (sunshine[i].y > 33)
					sunshine[i].y -= 20;
				else
					sunshine[i].y = 13;
				if (sunshine[i].x > 186)
					sunshine[i].x -= 50;
				else
					sunshine[i].x = 136;

				if (sunshine[i].x == 136 and sunshine[i].y == 13)
				{
					sunshine[i].used = 0;
					sunshine_cnt += sunshine[i].quality;
				}
			}
		}
	}
}

void Sunshine_creat_Little(int i, int j)
{
	int ballMAX = sizeof(sunshine) / sizeof(sunshine[0]);
	int k;
	for (k = 0; k < ballMAX and sunshine[k].used != 0; k++);
	if (k >= ballMAX)
		return;
	sunshine[k].used = true;
	sunshine[k].frameIndex = 0;
	sunshine[k].x = 458 + j * 87 + 142 + rand() % 10;
	sunshine[k].y = 80 + i * 103 + 162 + rand() % 10;
	sunshine[k].desty = 13;
	sunshine[k].timer = 0;
	sunshine[k].isfalling = 1;
	sunshine[k].quality = 25;
}

void UpdateGame()
{
	// 遍历地图，检查植物和僵尸状态并相应修改状态
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)   // 遍历
		{
			if (scence[i][j].type != 0)
			{
				if (scence[i][j].health_point <= 0) // 处理植物死亡

				{
					scence[i][j].type = 0;
					scence[i][j].movetype = 0;
					scence[i][j].frameIndex = 0;
					scence[i][j].Sun_Producting_Timer = 0;
					scence[i][j].Shoot_Timer = 0;
					scence[i][j].health_point = 0;
				}
				if (scence[i][j].type == 4) // 植物类型：type==4                                                                                                                                                                                    
				{
					scence[i][j].frameIndex++;
					int plantType = scence[i][j].type - 1;
					if (plant_attack[plantType][scence[i][j].frameIndex] == NULL and scence[i][j].movetype == 1)
						scence[i][j].frameIndex = 0;
					if (plant_idle[plantType][scence[i][j].frameIndex] == NULL and scence[i][j].movetype == 2)
						scence[i][j].frameIndex = 0;
					if (haveZombieMizuki(i, j) == true)
					{
						if (scence[i][j].movetype == 2)
						{
							scence[i][j].movetype = 1;
							scence[i][j].frameIndex = 0;
						}
						scence[i][j].Shoot_Timer++;
						if (scence[i][j].Shoot_Timer >= shoot_time[3])
						{
							attack_zombie_mizuki(i, j);
							scence[i][j].Shoot_Timer = 0;
						}
					}
					else if (scence[i][j].movetype == 1)
					{
						scence[i][j].movetype = 2;
						scence[i][j].frameIndex = 0;
					}
					continue;
				}
				if (scence[i][j].type != 7) // 如果不是提丰
				{
					if (haveZombie(scence[i][j].type, i, j) == true)
					{
						if (scence[i][j].movetype == 2)
							scence[i][j].movetype = 1;
					}
					else
					{
						if (scence[i][j].movetype == 1)
							scence[i][j].movetype = 2;
					}
					if (scence[i][j].movetype == 1)
					{
						scence[i][j].frameIndex++;
						int plantType = scence[i][j].type - 1;
						if (plant_attack[plantType][scence[i][j].frameIndex] == NULL)
							scence[i][j].frameIndex = 0;
						scence[i][j].Shoot_Timer++;
						if (scence[i][j].Shoot_Timer >= shoot_time[plantType])
						{
							shoot(scence[i][j].type, 458 + j * 87, 80 + i * 103);
							scence[i][j].Shoot_Timer = 0;
						}
					}
					else if (scence[i][j].movetype == 2) // 闲置状态更新
					{
						scence[i][j].frameIndex++;
						int plantType = scence[i][j].type - 1;
						if (plant_idle[plantType][scence[i][j].frameIndex] == NULL)
							scence[i][j].frameIndex = 0;
						if (scence[i][j].type == 1)
						{
							scence[i][j].Sun_Producting_Timer++;
							if (scence[i][j].Sun_Producting_Timer >= 250)
							{
								Sunshine_creat_Little(i, j);
								scence[i][j].Sun_Producting_Timer = 0;
							}
						}
					}
				}
				else // 如果是提丰
				{
					if (haveZombie(scence[i][j].type, i, j) == true or haveZombie(scence[i][j].type, i - 1, j) == true or haveZombie(scence[i][j].type, i + 1, j) == true)
					{
						if (scence[i][j].movetype == 2) // 检查僵尸的存在并修改状态
							scence[i][j].movetype = 1;
					}
					else
					{
						if (scence[i][j].movetype == 1)
							scence[i][j].movetype = 2;
					}
					if (scence[i][j].movetype == 1) // 如果此时提丰位于攻击状态
					{
						scence[i][j].frameIndex++;
						int plantType = scence[i][j].type - 1;
						if (plant_attack[plantType][scence[i][j].frameIndex] == NULL)
							scence[i][j].frameIndex = 0;
						scence[i][j].Shoot_Timer++;
						if (scence[i][j].Shoot_Timer >= shoot_time[plantType])  // 防止越界
						{
							if (i >= 1) // 确保 i-1 不越界
								shoot(scence[i][j].type, 458 + j * 87, 80 + (i - 1) * 103);
							shoot(scence[i][j].type, 458 + j * 87, 80 + i * 103);
							if (i < 4) // 确保 i+1 不越界
								shoot(scence[i][j].type, 458 + j * 87, 80 + (i + 1) * 103);
							scence[i][j].Shoot_Timer = 0;
						}
					}
					else if (scence[i][j].movetype == 2)
					{
						scence[i][j].frameIndex++;
						int plantType = scence[i][j].type - 1;
						if (plant_idle[plantType][scence[i][j].frameIndex] == NULL)
							scence[i][j].frameIndex = 0;
					}
				}
			}
		}
	}

	if (map_hard == 1)
	{
		Sunshine_creat();
	}//自然阳光的生成
	Sunshine_update();//阳光的更新

	Enemy_Create();//敌人的生成
	Enemy_Update();//敌人的更新

	Bullet_update();//子弹的更新
}

void Sunshine_collect(int msgx, int msgy)
{
	// 阳光高度和宽度都是78
	int ballMAX = sizeof(sunshine) / sizeof(sunshine[0]);
	for (int i = 0; i < ballMAX; i++)
	{
		if (sunshine[i].used and sunshine[i].isfalling)
		{
			if (msgx >= sunshine[i].x and msgx <= sunshine[i].x + 78 and msgy >= sunshine[i].y and msgy <= sunshine[i].y + 78)
			{
				sunshine[i].isfalling = 0;
				mciSendString("play res/sunshine/sunshine.mp3", 0, 0, 0);
				printf("collect sunshine at %d %d\n", sunshine[i].x, sunshine[i].y);
			}
		}
	}
}

void userClick()
{
	ExMessage msg;
	static int status = 0;
	if (peekmessage(&msg))
	{
		if (msg.message == WM_LBUTTONDOWN)
		{
			if (msg.x >= 215 and msg.x <= 215 + 105 * Card_cnt and msg.y >= 8 and msg.y <= 106)
			{
				int index = (msg.x - 215) / 105;
				printf("click card %d\n", index);
				// 检查阳光数量是否足够
				if (sunshine_cnt >= plant_sun_cost[index])
				{
					status = 1;
					nowPlant = index + 1;
				}
				else
				{
					printf("Not enough sunshine to get this plant!\n");
				}
			}
			else if (msg.x >= 1235 and msg.y >= 9 and msg.x <= 1337 and msg.y <= 110)
			{
				if (sunshine_cnt >= 25)
				{
					status = 1;
					Crane_Choosing = true;
				}
				else
					printf("Not enough sunshine to crane plant!\n");
			}
			else
			{
				Sunshine_collect(msg.x, msg.y);
			}
		}
		else if (msg.message == WM_MOUSEMOVE and status == 1)
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP)
		{
			int row = (msg.y - 220) / 103;
			int col = (msg.x - 550) / 87;

			if ((map_hard != 3 and row >= 0 and row < 5 and col >= 0 and col < 9) or (map_hard == 3 and row >= 1 and row < 4 and col >= 0 and col < 9))
			{
				if (scence[row][col].type == 0 and nowPlant != 0)
				{
					printf("planted on lawn %d %d\n", row, col);
					scence[row][col].type = nowPlant;
					scence[row][col].movetype = 2;//默认静置
					scence[row][col].frameIndex = 0;
					scence[row][col].Sun_Producting_Timer = 0;
					scence[row][col].Shoot_Timer = 0;
					scence[row][col].health_point = plant_health_point[nowPlant - 1];
					// 扣除阳光数量
					if (nowPlant != 0)
					{
						sunshine_cnt -= plant_sun_cost[nowPlant - 1];
					}
				}
				else if (scence[row][col].type != 0 and Crane_Choosing == true)
				{
					scence[row][col].type = 0;
					sunshine_cnt -= 25;
				}
			}

			nowPlant = 0;
			Crane_Choosing = false;
			status = 0;
		}
	}
	//按m键立即结束游戏 调试用
	if (GetAsyncKeyState(0x4D))
	{
		gameOver = true;
	}
}

void startUI()
{
	IMAGE menu1_normal, menu1_choosing, menu2_normal, menu2_choosing, menu3_normal, menu3_choosing;
	loadimage(&menu1_normal, "res/starter/hard_1_normal.png");
	loadimage(&menu1_choosing, "res/starter/hard_1_choosing.png");
	loadimage(&menu2_normal, "res/starter/hard_2_normal.png");
	loadimage(&menu2_choosing, "res/starter/hard_2_choosing.png");
	loadimage(&menu3_normal, "res/starter/hard_3_normal.png");
	loadimage(&menu3_choosing, "res/starter/hard_3_choosing.png");
	IMAGE Stbg[32];
	char path[64];
	for (int i = 0; i < 32; i++)
	{
		sprintf_s(path, sizeof(path), "res/starter/background/%d.png", i + 1);
		loadimage(&Stbg[i], path);
	}
	IMAGE whitebg, charcters;
	loadimage(&whitebg, "res/starter/whitebg.png");
	loadimage(&charcters, "res/starter/moving_characters.png");

	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	int bgtimer = 0;
	int deltimer = 1;
	int bgchangetimer = 0;
	int charcter_y = 0;
	while (true)
	{
		BeginBatchDraw();
		putimage(0, 0, &whitebg);
		putimagePNG(0, -charcter_y, &charcters);
		putimagePNG(0, 0, &Stbg[bgtimer]);
		if (bgchangetimer > 2)
		{
			bgchangetimer = 0;
			bgtimer += deltimer;
			if (bgtimer == 31 or bgtimer == 0)
				deltimer = -deltimer;
			charcter_y++;
			if (charcter_y > HEIGHT)
				charcter_y = -1350;
		}
		else
		{
			bgchangetimer++;
		}
		putimagePNG(598, 300, flag2 ? &menu2_choosing : &menu2_normal);
		putimagePNG(539, 95, flag1 ? &menu1_choosing : &menu1_normal);
		putimagePNG(539, 499, flag3 ? &menu3_choosing : &menu3_normal);
		EndBatchDraw();
		// 鼠标放在按钮上就会改变flag的值，然后在下一次渲染时就会渲染成选中状态
		// 鼠标点击按钮就会改变map_hard的值，然后跳出循环
		ExMessage msg;
		if (peekmessage(&msg))
		{
			if (msg.message == WM_MOUSEMOVE)
			{
				if (msg.x >= 598 + 150 and msg.x <= 598 + 404 and msg.y >= 300 + 47 and msg.y <= 300 + 125)
				{
					flag1 = 0;
					flag2 = 1;
					flag3 = 0;
				}
				else if (msg.x >= 539 + 98 and msg.x <= 539 + 473 and msg.y >= 95 + 66 and msg.y <= 95 + 154)
				{
					flag1 = 1;
					flag2 = 0;
					flag3 = 0;
				}
				else if (msg.x >= 539 + 139 and msg.x <= 539 + 438 and msg.y >= 499 + 59 and msg.y <= 499 + 161)
				{
					flag1 = 0;
					flag2 = 0;
					flag3 = 1;
				}
				else
				{
					flag1 = 0;
					flag2 = 0;
					flag3 = 0;
				}
			}
			else if (msg.message == WM_LBUTTONDOWN)
			{
				if (msg.x >= 539 + 98 and msg.x <= 539 + 473 and msg.y >= 95 + 66 and msg.y <= 95 + 154)
				{
					map_hard = 1;
					break;
				}
				else if (msg.x >= 598 + 150 and msg.x <= 598 + 404 and msg.y >= 300 + 47 and msg.y <= 300 + 125)
				{
					map_hard = 2;
					break;
				}
				else if (msg.x >= 539 + 139 and msg.x <= 539 + 438 and msg.y >= 499 + 59 and msg.y <= 499 + 161)
				{
					map_hard = 3;
					break;
				}
			}
		}
	}
}

void iniBackground(int map_hard)
{
	if (map_hard == 1)
	{
		loadimage(&background, "res/background1.jpg");
		sunshine_cnt = 100;
	}
	else if (map_hard == 2)
	{
		loadimage(&background, "res/background2.jpg");
		sunshine_cnt = 150;
	}
	else if (map_hard == 3)
	{
		loadimage(&background, "res/background3.jpg");
		sunshine_cnt = 150;
	}
}


// 显示游戏结束画面
void ShowGameOverScreen() {
	// 检查图像是否有效
	if (gameOverImage.getwidth() == 0 || gameOverImage.getheight() == 0) {
		printf("Image dimensions are zero!\n");
		return; // 退出函数
	}

	// 绘制背景
	//putimagePNG(0, 0, &gameOverBackground);

	int originalWidth = gameOverImage.getwidth();
	int originalHeight = gameOverImage.getheight();

	IMAGE gameOverImage;
	loadimage(&gameOverImage, "res/gameOverImage.png");

	for (int i = 0; i < 30; i++) { //循环次数也可以改
		double scale = 0.5 + i * 0.02; // 0.6是初始大小 每帧放大2%（可根据自己感觉舒适度修改）
		int newWidth = static_cast<int>(originalWidth * scale);
		int newHeight = static_cast<int>(originalHeight * scale);
		int x = (WIDTH - newWidth) / 2;
		int y = (HEIGHT - newHeight) / 2;

		// 获取当前窗口的设备上下文
		HDC hdc = GetImageHDC();

		// 创建一个内存设备上下文（内存DC）
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBitmap = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
		SelectObject(memDC, memBitmap);

		// 清除内存DC的背景
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0)); // 创建一个黑色画刷
		RECT rect = { 0, 0, WIDTH, HEIGHT };
		FillRect(memDC, &rect, hBrush);
		DeleteObject(hBrush);

		// 使用 AlphaBlend 函数进行背景透明度控制
		BLENDFUNCTION blendFunction;
		blendFunction.BlendOp = AC_SRC_OVER;
		blendFunction.BlendFlags = 0;
		blendFunction.SourceConstantAlpha = 0; // 透明度为0，清除上一次绘制的内容
		blendFunction.AlphaFormat = 0;

		AlphaBlend(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, WIDTH, HEIGHT, blendFunction);

		// 逐渐增加背景的透明度
		static int alpha = 0;
		if (alpha < 128) {
			alpha += 4; // 每帧增加透明度
		}

		blendFunction.SourceConstantAlpha = alpha; // 透明度（0-255）

		AlphaBlend(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, WIDTH, HEIGHT, blendFunction);

		// 将 EasyX 图像转换为位图
		HDC imageDC = GetImageHDC(&gameOverImage);

		// 使用 TransparentBlt 函数抠除 gameOverImage 中的黑色部分
		TransparentBlt(hdc, x, y, newWidth, newHeight, imageDC, 0, 0, originalWidth, originalHeight, RGB(0, 0, 0));

		// 清理资源
		DeleteObject(memBitmap);  // 删除位图对象
		DeleteDC(memDC);          // 删除内存设备上下文

		FlushBatchDraw();         // 刷新绘制
		Sleep(60);                // 每帧延迟60毫秒
	}



	// 添加询问用户返回主界面还是退出游戏
	int userChoice = MessageBox(NULL, "Game Over!\nDo you want to return to the main screen or exit?", "Game Over", MB_ICONQUESTION | MB_OKCANCEL);

	// 根据用户选择进行相应操作
	if (userChoice == IDOK) {
		// 返回主界面
		gameOver = false;
		iniGame();
		startUI();
	}
	else {
		// 退出游戏
		exit(0);
	}
}


int main() {
	iniGame();
	startUI();
	iniBackground(map_hard);

	int timer = 0;
	bool flag = true;
	while (true) {
		userClick();
		timer += getDelay();
		if (timer > 50) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			WindowUpdate();
			UpdateGame();

			// 检查游戏是否结束
			if (gameOver) {
				ShowGameOverScreen();
				// 结束后根据用户选择返回或退出
			}
		}
	}

	system("pause");
	return 0;
}

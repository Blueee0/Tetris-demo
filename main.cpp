#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<graphics.h>//easyx图形库
#include<conio.h>
#include<stdbool.h>
#include<Windows.h>//鼠标操作
#include<mmsystem.h>
#include <ctime>
#include <time.h>//随机时间
#include <easyx.h>//easyx
#pragma comment(lib,"winmm.lib")

struct focks {
	int i;
	int j;
}fock[4];//4*4方块坐标
struct forefock {
	int i;
	int j;
}forefock[4];//4*4预测方块
struct GRAPH {
	int num[4][4];
}graph[19];//图形样式19种

void CHANGESCENE();
void Game_Init();
void drawprompt();
void drawblock();
void Read_Graph();
void initRandomNum();
void GetFockXY();
void blockcolor(int forenum);

void Move();
int MoveFockDown();
void MoveFockLeft();
void MoveFockRight();
void MoveRotate();

bool checkLine(const unsigned& line);
void clearLine();

void BGM();
void BACKGROUND();
void MOUSE();
void BGMCHANGE();
bool checkgame();

int latt[32][16];
int forelatt[4][4];
int graph_num;//图形样式
int forenum;//存
int reserve;//存
int score = 0;
int downtime = 300;
int bol = 0;
clock_t start = 0;
clock_t end;
COLORREF color;

void Game_Init() {

	//绘画边框、侧栏
	roundrect(10, 10, 430, 830, 10, 10);
	drawprompt();
	loadimage(NULL, _T("sea.jpg"));
	//初始化所有网格，确保没有任何方块存在
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 16; j++) {
			latt[i][j] = 0;
		}
	}
	Read_Graph();//把所有图形样式读进来

	forenum = rand() % 19;
	graph_num = forenum;

	int count = 0;
	for (int i = 0; i < 4; i++)//graph[graph_num]中的i
		for (int j = 0; j < 4; j++)//graph[graph_num]中的j
			if (graph[graph_num].num[i][j] == 2)
			{
				fock[count].i = i;
				fock[count].j = j + 6;
				latt[fock[count].i][fock[count].j] = 2;
				count++;
			}

	initRandomNum();
	//预测方块先清空
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			forelatt[i][j] = 0;
		}
	}

	//预测方块
	int cnt = 0;
	for (int i = 0; i < 4; i++)//graph[graph_num]中的i
		for (int j = 0; j < 4; j++)//graph[graph_num]中的j
			if (graph[forenum].num[i][j] == 2)
			{
				forefock[cnt].i = i;
				forefock[cnt].j = j;
				forelatt[forefock[cnt].i][forefock[cnt].j] = 2;
				cnt++;
			}

	reserve = forenum;
	drawblock();//把最开始的格子画出来
}

void drawprompt() {

	//画横线
	setlinestyle(PS_SOLID, 10);
	setfillcolor(RED);
	line(20, 120, 420, 120);

	//还原设置
	setlinestyle(PS_SOLID, 1);
	setfillcolor(BLACK);

	// 设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 28;
	wcscpy_s(f.lfFaceName, L"微软雅黑");
	f.lfQuality = DEFAULT_QUALITY;
	settextstyle(&f);
	settextcolor(WHITE);

	// 绘制 “下一方块” 提示
	outtextxy(470, 50, L"下一方块");

	// 绘制 “分数” 和 “时间”
	f.lfHeight = 24;
	settextstyle(&f);

	wchar_t str[10];
	wsprintf(str, L"分数：%u", score);
	outtextxy(470, 300, str);

	int gametime = clock();
	wsprintf(str, L"时间：%u秒", gametime / 1000);
	outtextxy(470, 500, str);

}

void drawblock()//画出网格，空格子0，移动格子2，固定格子1
{
	//画游戏中的格子
	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 16; j++) {
			if (latt[i][j] == 0) {
				setfillcolor(RGB(128,138,135));
				fillrectangle(j * 25 + 20, i * 25 + 20, (j + 1) * 25 + 20, (i + 1) * 25 + 20);
			}
			else if(latt[i][j]==1) {
				setfillcolor(RGB(2,25,56));
				fillrectangle(j * 25 + 20, i * 25 + 20, (j + 1) * 25 + 20, (i + 1) * 25 + 20);
			}
			else if (latt[i][j] == 2) {
				blockcolor(graph_num);
				setfillcolor(color);
				fillrectangle(j * 25 + 20, i * 25 + 20, (j + 1) * 25 + 20, (i + 1) * 25 + 20);
			}
		}

	//画预测的格子
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (forelatt[i][j] == 0) {
				setfillcolor(RGB(128, 138, 135));
				fillrectangle(j * 25 + 467, i * 25 + 90, (j + 1) * 25 + 467, (i + 1) * 25 + 90);
			}
			else {
				blockcolor(forenum);
				setfillcolor(color);
				fillrectangle(j * 25 + 467, i * 25 + 90, (j + 1) * 25 + 467, (i + 1) * 25 + 90);
			}
		}
	}
}

//设置方块颜色
void blockcolor(int forenum) {
	int type = 0;
	if (forenum == 0) {
		type = 0;
	}
	else if (forenum > 0 && forenum < 3) {
		type = 1;
	}
	else if (forenum > 2 && forenum < 7) {
		type = 2;
	}
	else if (forenum > 6 && forenum < 11) {
		type = 3;
	}
	else if (forenum > 10 && forenum < 15) {
		type = 4;
	}
	else if (forenum > 14 && forenum < 17) {
		type = 5;
	}
	else if (forenum > 16 && forenum < 19) {
		type = 6;
	}
	switch (type) {
	case 0:
		color = RGB(0,255,255);
		break;
	case 1:
		color = RGB(255,255,0);
		break;
	case 2:
		color = RGB(255, 0, 0);
		break;
	case 3:
		color = RGB(255, 0, 255);
		break;
	case 4:
		color = RGB(255, 97, 0);
		break;
	case 5:
		color = RGB(0, 255, 127);
		break;
	case 6:
		color = RGB(160, 32, 240);
		break;
	}
}

//生成随机图形,读入19个情况
void Read_Graph()
{
	FILE* fp;
	fp = fopen("方块样式.txt", "r");
	if (fp == NULL)
		exit(-1);
	else {
		for (int i = 0; i < 19; i++)
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
					fscanf_s(fp, "%d", &graph[i].num[j][k]);
	}
}

//生成随机数要随着时间变化。
void initRandomNum()
{
	srand((unsigned)time(NULL));
	forenum = rand() % 19;//num = 0 - 18
}

//获得随机生成的图形的坐标
void GetFockXY()
{
	graph_num = reserve;
	initRandomNum();

	//预测方块先清空
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			forelatt[i][j] = 0;
		}
	}

	//预测方块
	int cnt = 0;
	for (int i = 0; i < 4; i++)//graph[graph_num]中的i
		for (int j = 0; j < 4; j++)//graph[graph_num]中的j
			if (graph[forenum].num[i][j] == 2)
			{
				forefock[cnt].i = i;
				forefock[cnt].j = j;
				forelatt[forefock[cnt].i][forefock[cnt].j] = 2;
				cnt++;
			}

	//游戏中的方块
	int count = 0;
	for (int i = 0; i < 4; i++)//graph[graph_num]中的i
		for (int j = 0; j < 4; j++)//graph[graph_num]中的j
			if (graph[reserve].num[i][j] == 2)
			{
				fock[count].i = i;
				fock[count].j = j + 6;
				latt[fock[count].i][fock[count].j] = 2;
				count++;
			}
	reserve = forenum;
}

void Move() {
	//每过400毫秒，方块下落一格,同时检测是否有方块下移
	end = clock();
	if ((end - start) >= downtime) {
		if (MoveFockDown() == 0)
		{
			initRandomNum();//如果方块固定了，生成新的随机图形
			GetFockXY();//获取新的方块坐标
		}
		start = clock();
		if (score == 20 && bol ==0) {
			downtime =200;
			bol = 1;
			MessageBox(GetHWnd(), L"Speed Up", L"Warning", MB_ICONASTERISK);
			
		}
		else if (score == 40 && bol ==1) {
			downtime = 100;
			bol = 2;
			MessageBox(GetHWnd(), L"Speed Up", L"Warning", MB_ICONASTERISK);
		}
		else if (score == 60 && bol==2) {
			downtime = 50;
			bol = 3;
			MessageBox(GetHWnd(), L"Speed Up", L"Warning", MB_ICONASTERISK);
		}

	}

	//(异步函数，多个按键同时按下)
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)//判断按键是否按下 
		MoveFockLeft();

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)//判断按键是否按下 
		MoveFockRight();

	if (GetAsyncKeyState(VK_UP) & 0x8000)//判断按键是否按下
		MoveRotate();

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)//判断按键是否按下 
		MoveFockDown();

	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		exit(-1);
	
	Sleep(70);

	
}

int MoveFockDown() //纵向移动
{
	for (int i = 0; i < 4; i++) {
		if (fock[i].i == 31) //把四个方块的位置固定
		{
			for (int m = 0; m < 4; m++) {
				latt[fock[m].i][fock[m].j] = 1;//如果方块到底,返回0
			}
			return 0;
		}
	}

	for (int j = 0; j < 4; j++)
	{
		if (latt[fock[j].i + 1][fock[j].j] == 1) //如果下面存在方块，则返回0
		{
			for (int n = 0; n < 4; n++) {
				latt[fock[n].i][fock[n].j] = 1;//把四个方块的位置固定
			}
			return 0;
		}
	}

	for (int k = 0; k < 4; k++)
		latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

	for (int p = 0; p < 4; p++)
		fock[p].i++;//位置坐标向下移动一格

	for (int q = 0; q < 4; q++)
		latt[fock[q].i][fock[q].j] = 2;//变成移动中的格子

	return 1;
}

void MoveFockLeft()
{
	for (int i = 0; i < 4; i++) //左边碰壁
	{
		if (fock[i].j <= 0)
			return;
	}
	for (int j = 0; j < 4; j++) //左边有方块
	{
		if (latt[fock[j].i][fock[j].j - 1] == 1)
			return;
	}
	for (int k = 0; k < 4; k++)
		latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

	for (int p = 0; p < 4; p++)
		fock[p].j--;//位置坐标向左移动一格

	for (int q = 0; q < 4; q++)
		latt[fock[q].i][fock[q].j] = 2;//变成移动中的格子
}

void MoveFockRight()
{

	for (int i = 0; i < 4; i++) {
		if (fock[i].j >= 15)//右边碰壁
			return;
	}
	for (int j = 0; j < 4; j++) {
		if (latt[fock[j].i][fock[j].j + 1] == 1)//右边有方块
			return;
	}

	for (int k = 0; k < 4; k++)
		latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

	for (int p = 0; p < 4; p++)
		fock[p].j++;//位置坐标向右移动一格

	for (int q = 0; q < 4; q++)
		latt[fock[q].i][fock[q].j] = 2;
}

void MoveRotate() {
	switch (graph_num)
	{
	case 0:
		break;

	case 1:
		if (fock[0].j - 2 > 0 && fock[0].j < 15)//不能越过边界
		{
			if (latt[fock[3].i][fock[3].j - 2] != 1 && latt[fock[3].i][fock[3].j - 1] != 1 && latt[fock[3].i][fock[3].j + 1] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i += 3;//开始改变移动方块的位置
				fock[0].j -= 2;
				fock[1].i += 2;
				fock[1].j -= 1;
				fock[2].i++;
				fock[3].j++;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num++;
			}
		}
		break;

	case 2:
		if (latt[fock[2].i - 1][fock[2].j] != 1 && latt[fock[2].i - 2][fock[2].j] != 1 && latt[fock[2].i - 3][fock[2].j] != 1) //不能覆盖原有方块
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i -= 3;//开始改变移动方块的位置
			fock[0].j += 2;
			fock[1].i -= 2;
			fock[1].j += 1;
			fock[2].i--;
			fock[3].j--;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num--;
		}
		break;

	case 3:
		if (latt[fock[0].i - 1][fock[0].j] != 1 && latt[fock[0].i - 1][fock[0].j + 1] != 1) //不能覆盖原有方块
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i--;//开始改变移动方块的位置
			fock[1].i -= 2;
			fock[1].j++;
			fock[2].i--;
			fock[2].j--;
			fock[3].j -= 2;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num++;
		}
		break;

	case 4:
		if (fock[0].j - 1 > 0 && fock[1].j + 1 < 15)//不能越过边界
		{
			if (latt[fock[2].i][fock[2].j + 1] != 1 && latt[fock[2].i][fock[2].j + 2] != 1 && latt[fock[3].i][fock[3].j + 2] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i++;//开始改变移动方块的位置
				fock[1].i++;
				fock[2].j += 2;
				fock[3].j += 2;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num++;
			}
		}
		break;
	

	case 5:
		if (latt[fock[1].i - 1][fock[1].j] != 1 && latt[fock[3].i][fock[3].j - 1] != 1 && latt[fock[3].i][fock[3].j - 2] != 1)
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i--;//开始改变移动方块的位置
			fock[0].j++;
			fock[2].i++;
			fock[2].j -= 2;
			fock[3].j--;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num++;
		}
		break;

	case 6:
		if (fock[1].j + 1 < 15)//不能越过边界
		{
			if (latt[fock[2].i - 1][fock[2].j] != 1 && latt[fock[2].i][fock[2].j + 2] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i++;//开始改变移动方块的位置
				fock[0].j--;
				fock[1].i++;
				fock[1].j--;
				fock[2].j++;
				fock[3].j++;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num = 3;
			}
		}
		break;

	case 7:
		if (latt[fock[1].i - 1][fock[1].j] != 1 && latt[fock[1].i - 2][fock[1].j] != 1) //不能覆盖原有方块
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i--;//开始改变移动方块的位置
			fock[0].j -= 2;
			fock[1].i--;
			fock[2].j--;
			fock[3].j--;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num++;
		}
		break;

	case 8:
		if (fock[3].j + 1 < 15)//不能越过边界
		{
			if (latt[fock[1].i][fock[1].j + 1] != 1 && latt[fock[1].i][fock[1].j + 2] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i++;//开始改变移动方块的位置
				fock[1].j++;
				fock[2].i--;
				fock[2].j += 2;
				fock[3].j--;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num++;
			}
		}
		break;

	case 9:
		if (latt[fock[0].i - 1][fock[0].j] != 1 && latt[fock[1].i - 1][fock[1].j] != 1) //不能覆盖原有方块
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i--;//开始改变移动方块的位置
			fock[1].i--;
			fock[2].j--;
			fock[3].j++;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num++;
		}
		break;

	case 10:
		if (fock[3].j + 1 < 15)//不能越过边界
		{
			if (latt[fock[2].i][fock[2].j + 1] != 1 && latt[fock[3].i][fock[3].j - 1] != 1 && latt[fock[3].i][fock[3].j + 1] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i++;//开始改变移动方块的位置
				fock[0].j += 2;
				fock[1].j--;
				fock[1].i += 2;
				fock[2].i++;
				fock[3].j++;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num = 7;
			}
		}
		break;

	case 11:
		if (latt[fock[0].i - 1][fock[0].j] != 1 && latt[fock[0].i][fock[0].j + 1] != 1) //不能覆盖原有方块
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i--;//开始改变移动方块的位置
			fock[1].i--;
			fock[1].j++;
			fock[2].i--;
			fock[2].j++;
			fock[3].j--;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num++;
		}
		break;

	case 12:
		if (fock[2].j + 1 < 15)//不能越过边界
		{
			if (latt[fock[2].i][fock[2].j + 1] != 1 && latt[fock[2].i + 1][fock[2].j] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i++;//开始改变移动方块的位置
				fock[1].j++;
				fock[2].j++;
				fock[3].j++;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num++;
			}
		}
		break;

	case 13:
		if (latt[fock[1].i - 1][fock[1].j] != 1) //不能覆盖原有方块
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i--;//开始改变移动方块的位置
			fock[0].j++;
			fock[1].j--;
			fock[2].j--;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num++;
		}
		break;

	case 14:
		if (fock[2].j - 1 > 0)//不能越过边界
		{
			if (latt[fock[3].i][fock[3].j - 1] != 1 && latt[fock[3].i][fock[3].j - 2] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i++;//开始改变移动方块的位置
				fock[0].j--;
				fock[1].i++;
				fock[1].j--;
				fock[2].i++;
				fock[2].j--;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num = 11;
			}
		}
		break;

	case 15:
		if (latt[fock[1].i - 1][fock[1].j + 1] != 1 && latt[fock[1].i][fock[1].j + 1] != 1) //不能覆盖原有方块
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i--;//开始改变移动方块的位置
			fock[0].j += 2;
			fock[2].i--;
			fock[2].j++;
			fock[3].j--;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num++;
		}
		break;

	case 16:
		if (fock[1].j - 1 > 0)//不能越过边界
		{
			if (latt[fock[1].i][fock[1].j - 1] != 1 && latt[fock[3].i][fock[3].j + 1] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i++;//开始改变移动方块的位置
				fock[0].j -= 2;
				fock[2].i++;
				fock[2].j--;
				fock[3].j++;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num--;
			}
		}
		break;

	case 17:
		if (latt[fock[0].i - 1][fock[0].j - 1] != 1 && latt[fock[0].i][fock[0].j - 1] != 1) //不能覆盖原有方块
		{
			for (int k = 0; k < 4; k++)
				latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

			fock[0].i--;//开始改变移动方块的位置
			fock[0].j--;
			fock[1].j -= 2;
			fock[2].i--;
			fock[2].j++;

			for (int q = 0; q < 4; q++)//变成移动中的格子
				latt[fock[q].i][fock[q].j] = 2;
			graph_num++;
		}
		break;

	case 18:
		if (fock[2].j + 1 < 15)//不能越过边界
		{
			if (latt[fock[1].i][fock[1].j - 1] != 1 && latt[fock[3].i][fock[3].j + 1] != 1) //不能覆盖原有方块
			{
				for (int k = 0; k < 4; k++)
					latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

				fock[0].i++;//开始改变移动方块的位置
				fock[0].j++;
				fock[1].j += 2;
				fock[2].i++;
				fock[2].j--;

				for (int q = 0; q < 4; q++)//变成移动中的格子
					latt[fock[q].i][fock[q].j] = 2;
				graph_num--;
			}
		}
		break;
	}
}

void clearLine()
{
	int line = -1;
	// 判断哪一行满行
	for (int i = 0; i < 32; i++)
	{
		if (checkLine(i))
		{
			line = i;
			break;
		}
	}

	if (line != -1)
	{	
		for (int k = 0; k < 4; k++)
			latt[fock[k].i][fock[k].j] = 0;//把原来变成空的格子

		for (int p = 0; p < 4; p++)
			fock[p].i++;//位置坐标向下移动一格

		// 将上一行移至满行
		for (int i = line ; i - 1 > 0; i--)
		{		
			for (int j = 0; j < 16; j++)
			{
				latt[i][j] = latt[i - 1][j];
			}
		}
		score += 10;	// 将游戏分数加 10
	}
	drawprompt();
}
bool checkLine(const unsigned& line)//判断满行
{
	int result = true;
	for (int i = 0; i < 16; i++)
	{
		if (latt[line][i] != 1) {
			result = false;
		}
	}
	return result;
}

bool checkgame() {
	for (int j = 0; j < 16; j++) {
		if (latt[2][j] == 1)
			return true;
	}
	return false;
}

void BGM()
{
	mciSendString(_T("open music\\第四个.mp3"), NULL, 0, NULL);//打开音乐四
	mciSendString(_T("open music\\elsfk.mp3"), NULL, 0, NULL);//打开音乐一
	mciSendString(_T("open music\\第二个.mp3"), NULL, 0, NULL);//打开音乐二
	mciSendString(_T("open music\\第三个.mp3"), NULL, 0, NULL);//打开音乐三
}

void BACKGROUND()
{
	loadimage(NULL, _T("end7.jpg"));//插入背景
}

void MOUSE()
{
	MOUSEMSG m;    //存放鼠标的值
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {		//如果按下鼠标左键实现相应功能.
			closegraph();
			printf("HHHHHHHHHHHHHHHH\n");
			initgraph(600, 840);
			setfillcolor(GREEN);
			getchar;
			closegraph();
			break;
		}
	}
}

void BGMCHANGE()//切换BGM
{
	MOUSEMSG i;
	while (1)
	{
		i = GetMouseMsg();
		if (i.x >= 57 && i.x <= 552 && i.y >= 142 && i.y <= 185 && i.uMsg == WM_LBUTTONDOWN)//俄罗斯方块（魏春晓）
		{
			mciSendString(_T("close music\\第四个.mp3"), NULL, 0, NULL);//暂停之前的音乐
			mciSendString(_T("close music\\elsfk.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第二个.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第三个.mp3"), NULL, 0, NULL);
			mciSendString(_T("play music\\elsfk.mp3 repeat"), NULL, 0, NULL);//循环播放
		}
		if (i.x >= 57 && i.x <= 552 && i.y >= 256 && i.y <= 300 && i.uMsg == WM_LBUTTONDOWN)//utopyan - Ü Should Stay Lost
		{
			mciSendString(_T("close music\\第四个.mp3"), NULL, 0, NULL);//暂停之前的音乐
			mciSendString(_T("close music\\elsfk.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第二个.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第三个.mp3"), NULL, 0, NULL);
			mciSendString(_T("play music\\第二个.mp3 repeat"), NULL, 0, NULL);//循环播放
		}
		if (i.x >= 57 && i.x <= 552 && i.y >= 378 && i.y <= 422 && i.uMsg == WM_LBUTTONDOWN)//Drive
		{
			mciSendString(_T("close music\\第四个.mp3"), NULL, 0, NULL);//暂停之前的音乐
			mciSendString(_T("close music\\elsfk.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第二个.mp3"), NULL, 0, NULL);
			mciSendString(_T("play music\\第三个.mp3 repeat"), NULL, 0, NULL);//循环播放
		}
		if (i.x >= 57 && i.x <= 552 && i.y >= 504 && i.y <= 548 && i.uMsg == WM_LBUTTONDOWN)//前前前世
		{
			mciSendString(_T("close music\\第四个.mp3"), NULL, 0, NULL);//暂停之前的音乐
			mciSendString(_T("close music\\elsfk.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第二个.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第三个.mp3"), NULL, 0, NULL);
			mciSendString(_T("play music\\第四个.mp3 repeat"), NULL, 0, NULL);//循环播放
		}
		if (i.x >= 57 && i.x <= 552 && i.y >= 624 && i.y <= 668 && i.uMsg == WM_LBUTTONDOWN)//前前前世
		{
			mciSendString(_T("close music\\第四个.mp3"), NULL, 0, NULL);//暂停之前的音乐
			mciSendString(_T("close music\\elsfk.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第二个.mp3"), NULL, 0, NULL);
			mciSendString(_T("close music\\第三个.mp3"), NULL, 0, NULL);
			mciSendString(_T("play music\\第四个.mp3 repeat"), NULL, 0, NULL);//循环播放
		}
		if (i.x >= 57 && i.x <= 552 && i.y >= 751 && i.y <= 796 && i.uMsg == WM_LBUTTONDOWN)
		{
			closegraph();
			initgraph(600, 840);
			BGM();
			BACKGROUND();
			CHANGESCENE();
			closegraph();
		}
	}



}

void ABOUTUS()//关于我们
{
	closegraph();
	initgraph(2000, 1000);
	IMAGE picture;
	loadimage(NULL, _T("关于我们.jpg"));
	MOUSEMSG j;
	while (1)
	{
		j = GetMouseMsg();
		if (j.x >= 875 && j.x <= 1081 && j.y >= 778 && j.y <= 841 && j.uMsg == WM_LBUTTONDOWN)
		{
			initgraph(600, 840);
			mciSendString(_T("play music\\elsfk.mp3 repeat"), NULL, 0, NULL);//循环播放
			BGM();
			BACKGROUND();
			CHANGESCENE();
		}
	}
	getchar;
	closegraph();
}

void CHANGESCENE()//屏幕切换
{
	MOUSEMSG n;
	while (1)
	{
		n = GetMouseMsg();
		if (n.x >= 28 && n.x <= 153 && n.y >= 697 && n.y <= 731 && n.uMsg == WM_LBUTTONDOWN)
		{
			closegraph();
			initgraph(600, 840);//创建窗口
			BeginBatchDraw();//刷新绘画窗口
			Game_Init();
			
			while (1)
			{			
				Move();	
				if (checkgame() == true)
				{
					MessageBox(GetHWnd(), L"游戏结束！", L"提示", MB_ICONWARNING);
					goto exit;
				}

				drawblock();
				clearLine();
				FlushBatchDraw();
				Sleep(30);
			}
		exit:
			EndBatchDraw();
			break;
		}

		//音乐切换
		if (n.x >= 246 && n.x <= 370 && n.y >= 697 && n.y <= 731 && n.uMsg == WM_LBUTTONDOWN)
		{
			closegraph();
			initgraph(600, 840);
			loadimage(NULL, _T("bgm.jpg"));
			BGMCHANGE();
			getchar;
			closegraph();
			break;
		}

		//退出
		if (n.x >= 455 && n.x <= 580 && n.y >= 697 && n.y <= 731 && n.uMsg == WM_LBUTTONDOWN)
		{
			closegraph();
			exit(-1);
			break;
		}

		//关于我们
		if (n.x >= 198 && n.x <= 406 && n.y >= 777 && n.y <= 810 && n.uMsg == WM_LBUTTONDOWN)
		{
			ABOUTUS();
			break;
		}
	}
}

int main()
{
	initgraph(600, 840);
	mciSendString(_T("play music\\elsfk.mp3 repeat"), NULL, 0, NULL);//循环播放
	BGM();
	BACKGROUND();
	CHANGESCENE();
	closegraph();
	return 0;
}
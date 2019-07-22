#include <iostream>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include "Screen.h"
using namespace std;

#pragma warning(disable:4996)

typedef enum _GAME_STATE { INIT, READY, RUNNING, SUCCESS, FAILED, STOP, RESULT } GAME_STATE;
typedef enum _DIRECT { TOP, RIGHT_TOP, RIGHT_DOWN, DOWN, LEFT_DOWN, LEFT_TOP } DIRECT;
typedef enum _WALL { WALL_TOP, WALL_RIGHT, WALL_DOWN, WALL_LEFT } WALL;

typedef enum _ITEMSORT {ITEM_SLOW, ITEM_FAST, ITEM_EXPAND, ITEM_SHORTEN} ITEMSORT;

#define BLOCK_ROW 2
#define BLOCK_COL 20

#define WIDTH 22
#define HEIGHT 23

#define BORDER_LEFT 0		// ���� �� ��� ��ǥ
#define BORDER_RIGHT 20		// ������ �� ��� ��ǥ
#define BORDER_TOP 1		// ���� �� ��� ��ǥ
#define BORDER_DOWN 22		// �Ʒ��� �� ��� ��ǥ

int g_StateTable[4][6] = { // ���� �浹�ϰ� �Ǹ� ���� ����
	{  3,  2, -1, -1, -1,  4 },
	{ -1,  5,  4, -1, -1, -1 },
	{ -1, -1,  1,  0,  5, -1 },
	{ -1, -1, -1, -1,  2,  1 }
};

int g_StateBlock[6] = { 3, 2, 1, 0, 5, 4 }; // ���� �浹�ϰ� �Ǹ� ���� ����

class Item
{
private:
	int		 originX, originY; // ó�� ��ǥ
	int		 nX, nY;     // ��ǥ
	ITEMSORT sort;
	int nLife;
	clock_t OldTime;
	clock_t MoveTime;

public:
	Item(int x, int y, ITEMSORT _sort)
	{
		originX = x;
		originY = y;
		nX = x; 
		nY = y;
		sort = _sort;
		nLife = 0;
		OldTime = clock();
		MoveTime = 300;
	}

	int getOriginX() { return originX; }
	void setOriginX(int _nX) { originX = _nX; }
	int getOriginY() { return originY; }
	void setOriginY(int _nY) { originY = _nY; }
	int getNX() { return nX; }
	void setNX(int _nX) { nX = _nX; }
	int getNY() { return nY; }
	void setNY(int _nY) { nY = _nY; }
	ITEMSORT getSort() { return sort; }
	void setSort(ITEMSORT _sort) { sort = _sort; }
	int getNLife() { return nLife; }
	void setNLife(int _life) { nLife = _life; }
	clock_t getOldTime() { return OldTime; }
	void setOldTime(clock_t time) { OldTime = time; }
	clock_t getMoveTime() { return MoveTime; }
	void setMoveTime(clock_t time) { MoveTime = time; }

	void DrawSlowItem()
	{
		ScreenPrint(nX, nY, "��");
	}

	void DrawFastItem()
	{
		ScreenPrint(nX, nY, "��");
	}

	void DrawExpandItem()
	{
		ScreenPrint(nX, nY, "��");
	}

	void DrawShortenItem()
	{
		ScreenPrint(nX, nY, "��");
	}

	bool IsOriginPOS()
	{
		if (nX == originX && nY == originY)
			return true;
		else
			return false;
	}

	void MoveDown()
	{
		nY += 1;
	}
};

class Block
{
private:
	int		 nX, nY;     // ��ǥ
	int		 nLife;
public:
	Block()
	{

	}

	Block(int x, int y, int life)
	{
		nX = x;
		nY = y;
		nLife = life;
	}

	int getNX() { return nX; }
	void setNX(int _nX) { nX = _nX; }
	int getNY() { return nY; }
	void setNY(int _nY) { nY = _nY; }
	int getNLife() { return nLife; }
	void setNLife(int _life) { nLife = _life; }

	void DrawBlock()
	{
		if (nLife == 1) {
			ScreenPrint(nX, nY, "��");
		}
		else if (nLife == 2) {
			ScreenPrint(nX, nY, "��");
		}
	}
};

class Ball
{
private:
	int		 nReady;     // 1 �̸� �غ� ����, 0�̸� �̵� ����
	int		 nHP;        // ����
	int		 nX, nY;     // ��ǥ
	DIRECT   nDirect;    // �̵� ����
	clock_t  MoveTime;   // �̵� �ð� ����
	clock_t  OldTime;	 // ���� �̵� �ð�

public:
	Ball() :nReady(1), nHP(3), nX(10), nY(20), nDirect(RIGHT_TOP), OldTime(clock()), MoveTime(150)
	{

	}

	int getNReady() { return nReady; }
	void setNReady(int _nReady) { nReady = _nReady; }
	int getNHP() { return nHP; }
	void setNHP(int _nHP) { nHP = _nHP; }
	int getNX() { return nX; }
	void setNX(int _nX) { nX = _nX; }
	int getNY() { return nY; }
	void setNY(int _nY) { nY = _nY; }
	DIRECT getNDirect() { return nDirect; }
	void setNDirect(DIRECT _nDirect) { nDirect = _nDirect; }
	clock_t getOldTime() { return OldTime; }
	void setOldTime(clock_t time) { OldTime = time; }
	clock_t getMoveTime() { return MoveTime; }
	void setMoveTime(clock_t time) { MoveTime = time; }

	void DrawBall()
	{
		ScreenPrint(nX, nY, "��");
	}

	void MoveLeft()
	{
		nX -= 1;
	}

	void MoveRight()
	{
		nX += 1;
	}

	void MoveTop()
	{
		nY -= 1;
	}

	void MoveDown()
	{
		nY += 1;
	}

	void decHP()
	{
		nHP--;
	}
};

class Bar
{
private:
	int nX;				// x�� �߽� ��ǥ�̴�
	int nY;
	int length;			// Ȧ���θ� ���� (�ּ� 3)
	clock_t OldTime;
	clock_t MoveTime;

public:
	Bar() : nX(0), nY(0), length(3), OldTime(clock()), MoveTime(10)
	{
		
	}

	int getNX() { return nX; }
	void setNX(int _nX) { nX = _nX; }
	int getNY() { return nY; }
	void setNY(int _nY) { nY = _nY; }
	int getLength() { return length; }
	void setLength(int _length) { length = _length; }
	clock_t getOldTime() { return OldTime; }
	void setOldTime(clock_t time) { OldTime = time; }
	clock_t getMoveTime() { return MoveTime; }
	void setMoveTime(clock_t time) { MoveTime = time; }

	void DrawBar()
	{
		int half = length / 2;
		for (int i = -half; i <= half; i++)
		{
			ScreenPrint(nX + i, nY, "��");
		}
	}

	void MoveLeft()
	{
		int half = length / 2;

		if (nX - half > BORDER_LEFT + 1) // ��� ���� �浹 üũ
		{
			nX -= 1;
		}
	}

	void MoveRight()
	{
		int half = length / 2;

		if (nX + half < BORDER_RIGHT) // ��� ���� �浹 üũ
		{
			nX += 1;
		}
	}
};

class MapInfo
{
private:
	int blockIdx;
	int itemIdx;

public:
	MapInfo()
	{
		itemIdx = -1;
	}
	int getBlockIdx() { return blockIdx; }
	void setBlockIdx(int idx) { blockIdx = idx; }
	int getItemIdx() { return itemIdx; }
	void setItemIdx(int idx) { itemIdx = idx; }
};

Bar  g_Bar;
Ball g_Ball;
Block* g_Block[HEIGHT+1][WIDTH+1];
Item* g_item[100];		// ���Ƿ� 100��
int itemIdx = -1; 
int blockNum = 0;  // ���� �����ϴ� �� �� (��Ȯ���� �����ϴ� ���� life ��)

MapInfo g_MapInfo[HEIGHT + 1][WIDTH + 1];

bool g_StageInitInfo[3] = { false, false, false }; // �������� �� �ʱ�ȭ�� ���� �÷��� (false�� ��� �ѹ��� �ʱ�ȭ ���ش�.)

int g_nGrade = 0;
int g_nTotalGrade = 0;
int g_nStage = -1;
int g_nGameState = INIT;
clock_t	g_GameStartTime, g_UpdateOldTime, g_RemainTime;

bool IsCollision(int x, int y)
{
	DIRECT currentDirect = g_Ball.getNDirect();
	int nCount = 0;

	// ���� ������ �浹
	int tempX = x;
	if (g_Ball.getNDirect() == LEFT_DOWN || g_Ball.getNDirect() == LEFT_TOP)
	{
		tempX = x + 1;
	}

	if (g_Ball.getNDirect() == RIGHT_DOWN || g_Ball.getNDirect() == RIGHT_TOP)
	{
		tempX = x - 1;
	}
	
	if (g_Block[y][tempX]->getNLife() >= 1 && g_Block[y][tempX]->getNY() == y)
	{
		if (g_Block[y][tempX]->getNX() == tempX || g_Block[y][tempX]->getNX() + 1 == tempX)
		{
			g_Ball.setNDirect((DIRECT)g_StateBlock[currentDirect]);
			g_Block[y][tempX]->setNLife(g_Block[y][tempX]->getNLife() - 1);
			blockNum--;
			nCount++;
			g_nGrade += 10;

			// ������ ����
			if (g_MapInfo[y][tempX].getItemIdx() != -1)
			{
				g_item[g_MapInfo[y][tempX].getItemIdx()]->setOldTime(clock());
				g_item[g_MapInfo[y][tempX].getItemIdx()]->setNLife(1);
			}
		}
	}
	else if (g_Block[y][x]->getNLife() >= 1 &&g_Block[y][tempX]->getNY() == y)
	{
		if (g_Block[y][x]->getNX() == x || g_Block[y][x]->getNX()+1 == x )
		{
			g_Ball.setNDirect((DIRECT)g_StateBlock[currentDirect]);
			g_Block[y][x]->setNLife(g_Block[y][x]->getNLife() - 1);
			blockNum--;
			nCount++;
			g_nGrade += 10;

			if (g_MapInfo[y][x].getItemIdx() != -1)
			{
				g_item[g_MapInfo[y][x].getItemIdx()]->setOldTime(clock());
				g_item[g_MapInfo[y][x].getItemIdx()]->setNLife(1);
			}
		}
	}
	
	// �浹 üũ 
	if (nCount != 0)
	{
		return true;
	}

	// ���� ����� �浹
	int half = g_Bar.getLength() / 2;
	for (int i = -half; i <= half; i++)
	{
		if (y == g_Bar.getNY() - 1)
		{
			int half = g_Bar.getLength() / 2;
			if ((x >= g_Bar.getNX() - (half)) && (x <= g_Bar.getNX() + (half)) || (x+1 >= g_Bar.getNX() - (half)) && (x+1 <= g_Bar.getNX() + (half)))
			{
				g_Ball.setNDirect((DIRECT)g_StateTable[WALL_DOWN][currentDirect]);
			}
		}
	}

	// ��� ���� �浹 üũ
	if (x < BORDER_LEFT + 1)	// ����
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_LEFT][currentDirect]);
		return true;
	}

	if (x > BORDER_RIGHT) // ������
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_RIGHT][currentDirect]);
		return true;
	}

	if (y < BORDER_TOP) // ��
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_TOP][currentDirect]);
		return true;
	}

	if (y > g_Bar.getNY()) // �Ʒ�
	{
		g_Ball.decHP();

		if (g_Ball.getNHP() == 0)
		{	// ���� �������� 
			g_nGameState = STOP;
		}
		else
		{
			g_Ball.setNX(g_Bar.getNX());
			g_Ball.setNY(g_Bar.getNY() - 1);
			g_Ball.setNReady(1);
			g_Ball.setNDirect(RIGHT_TOP);
		}
		
		return true;
	}

	return false;
}

void RunningScreen()
{
	ScreenPrint(0, 0, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
	ScreenPrint(0, 1, "��                                         ��");
	ScreenPrint(0, 2, "��                                         ��");
	ScreenPrint(0, 3, "��                                         ��");
	ScreenPrint(0, 4, "��                                         ��");
	ScreenPrint(0, 5, "��                                         ��");
	ScreenPrint(0, 6, "��                                         ��");
	ScreenPrint(0, 7, "��                                         ��");
	ScreenPrint(0, 8, "��                                         ��");
	ScreenPrint(0, 9, "��                                         ��");
	ScreenPrint(0, 10, "��                                         ��");
	ScreenPrint(0, 11, "��                                         ��");
	ScreenPrint(0, 12, "��                                         ��");
	ScreenPrint(0, 13, "��                                         ��");
	ScreenPrint(0, 14, "��                                         ��");
	ScreenPrint(0, 15, "��                                         ��");
	ScreenPrint(0, 16, "��                                         ��");
	ScreenPrint(0, 17, "��                                         ��");
	ScreenPrint(0, 18, "��                                         ��");
	ScreenPrint(0, 19, "��                                         ��");
	ScreenPrint(0, 20, "��                                         ��");
	ScreenPrint(0, 21, "��                                         ��");
	ScreenPrint(0, 22, "��                                         ��");
	ScreenPrint(0, 23, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");

}

void ReadyScreen()
{
	ScreenPrint(0, 0, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
	ScreenPrint(0, 1, "��                                        ��");
	ScreenPrint(0, 2, "��                                        ��");
	ScreenPrint(0, 3, "��                                        ��");
	ScreenPrint(0, 4, "��                                        ��");
	ScreenPrint(0, 5, "��                                        ��");
	ScreenPrint(0, 6, "��                                        ��");
	ScreenPrint(0, 7, "��              S t a g e                 ��");
	ScreenPrint(0, 8, "��                            ��          ��");
	ScreenPrint(0, 9, "��                      �С리��          ��");
	ScreenPrint(0, 10, "��                        �С린          ��");
	ScreenPrint(0, 11, "��                          ������        ��");
	ScreenPrint(0, 12, "��                                        ��");
	ScreenPrint(0, 13, "��                                        ��");
	ScreenPrint(0, 14, "��                                        ��");
	ScreenPrint(0, 15, "��                                        ��");
	ScreenPrint(0, 16, "��                                        ��");
	ScreenPrint(0, 17, "��                                        ��");
	ScreenPrint(0, 18, "��                                        ��");
	ScreenPrint(0, 19, "��                                        ��");
	ScreenPrint(0, 20, "��                                        ��");
	ScreenPrint(0, 21, "��                                        ��");
	ScreenPrint(0, 22, "��                                        ��");
	ScreenPrint(0, 23, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
}

void SuccessScreen()
{
	ScreenPrint(0, 0, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
	ScreenPrint(0, 1, "��      *.                                ��");
	ScreenPrint(0, 2, "��              .*            .*          ��");
	ScreenPrint(0, 3, "��     *.                        .*       ��");
	ScreenPrint(0, 4, "��       *.             .*                ��");
	ScreenPrint(0, 5, "��           *.                           ��");
	ScreenPrint(0, 6, "��            *.             .*           ��");
	ScreenPrint(0, 7, "��     *.      *.��������.*��    .*       ��");
	ScreenPrint(0, 8, "��             ��������������             ��");
	ScreenPrint(0, 9, "��     *.      ��������������      .*     ��");
	ScreenPrint(0, 10, "��             ��������������~            ��");
	ScreenPrint(0, 11, "��        *.   ������������               ��");
	ScreenPrint(0, 12, "��                                        ��");
	ScreenPrint(0, 13, "��                          .*            ��");
	ScreenPrint(0, 14, "��  *.           You Win!!          *.    ��");
	ScreenPrint(0, 15, "��                                        ��");
	ScreenPrint(0, 16, "��                                        ��");
	ScreenPrint(0, 17, "��                       *.               ��");
	ScreenPrint(0, 18, "��    *.         *.                       ��");
	ScreenPrint(0, 19, "��                                 *.     ��");
	ScreenPrint(0, 20, "��                      *.                ��");
	ScreenPrint(0, 21, "��  *.                             *.     ��");
	ScreenPrint(0, 22, "��                                        ��");
	ScreenPrint(0, 23, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
}

void FailureScreen()
{
	ScreenPrint(0, 0, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
	ScreenPrint(0, 1, "��  �������������������  ��");
	ScreenPrint(0, 2, "��  �������������������  ��");
	ScreenPrint(0, 3, "��                                        ��");
	ScreenPrint(0, 4, "��                                        ��");
	ScreenPrint(0, 5, "��                                        ��");
	ScreenPrint(0, 6, "��                                        ��");
	ScreenPrint(0, 7, "��             *�Ѧ�  �Ѧ�*               ��");
	ScreenPrint(0, 8, "��          *.  �Ӥӡ�Ӥ�   .*           ��");
	ScreenPrint(0, 9, "��   ���ѤѤѤѤѤѤѤѤѤѤѤѤѤѦ�     ��");
	ScreenPrint(0, 10, "��   ������ �� ��ȸ�� �̿��� �ּ��䦭     ��");
	ScreenPrint(0, 11, "��   ���ѤѤѤѤѤѤѤѤѤѤѤѤѤѦ�     ��");
	ScreenPrint(0, 12, "��                                        ��");
	ScreenPrint(0, 13, "��                                        ��");
	ScreenPrint(0, 14, "��    (*�ҡ�*)                            ��");
	ScreenPrint(0, 15, "��   ��O������O����������������������     ��");
	ScreenPrint(0, 16, "��   ������Ͻðڽ��ϱ�? <y/n>     ��     ��");
	ScreenPrint(0, 17, "��   ��������������������������������     ��");
	ScreenPrint(0, 18, "��                                        ��");
	ScreenPrint(0, 19, "��                                        ��");
	ScreenPrint(0, 20, "��                                        ��");
	ScreenPrint(0, 21, "��  �������������������  ��");
	ScreenPrint(0, 22, "��  �������������������  ��");
	ScreenPrint(0, 23, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
}

void ResultScreen()
{
	ScreenPrint(0, 0, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
	ScreenPrint(0, 1, "��                                        ��");
	ScreenPrint(0, 2, "��                                        ��");
	ScreenPrint(0, 3, "��                                        ��");
	ScreenPrint(0, 4, "��                                        ��");
	ScreenPrint(0, 5, "��                                        ��");
	ScreenPrint(0, 6, "��                                        ��");
	ScreenPrint(0, 7, "��   (((    ���� ����  ���� Score  ))))   ��");
	ScreenPrint(0, 8, "��                                        ��");
	ScreenPrint(0, 9, "��                                        ��");
	ScreenPrint(0, 10, "��                                        ��");
	ScreenPrint(0, 11, "��           �� ����  :                   ��");
	ScreenPrint(0, 12, "��                                        ��");
	ScreenPrint(0, 13, "��                                        ��");
	ScreenPrint(0, 14, "��                                        ��");
	ScreenPrint(0, 15, "��                                        ��");
	ScreenPrint(0, 16, "��                                        ��");
	ScreenPrint(0, 17, "��                                        ��");
	ScreenPrint(0, 18, "��                                        ��");
	ScreenPrint(0, 19, "��                                        ��");
	ScreenPrint(0, 20, "��                                        ��");
	ScreenPrint(0, 21, "��                                        ��");
	ScreenPrint(0, 22, "��                                        ��");
	ScreenPrint(0, 23, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
}

void InitScreen() // ���� �Ұ� ȭ��
{
	ScreenPrint(0, 0, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
	ScreenPrint(0, 1, "��                                        ��");
	ScreenPrint(0, 2, "��    �����������������    ��");
	ScreenPrint(0, 3, "��      ���������������      ��");
	ScreenPrint(0, 4, "��        �������������        ��");
	ScreenPrint(0, 5, "��          �����������          ��");
	ScreenPrint(0, 6, "��          ��������������������          ��");
	ScreenPrint(0, 7, "��          ���� ���� ����  �ڦ�          ��");
	ScreenPrint(0, 8, "��          ��������������������          ��");
	ScreenPrint(0, 9, "��              /)/)    �� ��             ��");
	ScreenPrint(0, 10, "��           ��(*'')/                     ��");
	ScreenPrint(0, 11, "��          �����������          ��");
	ScreenPrint(0, 12, "��        �������������        ��");
	ScreenPrint(0, 13, "��      ���������������      ��");
	ScreenPrint(0, 14, "��    �����������������    ��");
	ScreenPrint(0, 15, "��                                        ��");
	ScreenPrint(0, 16, "��                                        ��");
	ScreenPrint(0, 17, "��    �� �����̽�Ű�� �����ּ���!! ��     ��");
	ScreenPrint(0, 18, "��                                        ��");
	ScreenPrint(0, 19, "��     J:��  L:�� K: ���߻�               ��");
	ScreenPrint(0, 20, "��                                        ��");
	ScreenPrint(0, 21, "��                                        ��");
	ScreenPrint(0, 22, "��                                        ��");
	ScreenPrint(0, 23, "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��");
}

void Init()
{
	if (g_nStage == -1) // 1���� �ʱ�ȭ�� �Ǵ� �κп� ���� ó�� ����
	{
		g_nStage = 0;

		// ��� �ʱ�ȭ
		for (int row = 0; row <= HEIGHT; row++)
		{
			for (int col = 0; col <= WIDTH; col++)
			{
				g_Block[row][col] = new Block(col, row, 0);
			}
		}

		g_nGrade = 0;
	}

	if (g_nStage == 0 && !g_StageInitInfo[0]) // �������� 1�϶� �ʱ�ȭ
	{
		itemIdx = -1;
		blockNum = 0;
		g_StageInitInfo[0] = true;

		// ���������� �´� ��, ������ ����
		//for (int row = 3; row <= BLOCK_ROW + 1; row++)
		//{
		//	for (int col = 10; col <= BLOCK_COL - 8; col++)
		//	{
		//		g_Block[row][col]->setNLife(1);
		//		blockNum++;

		//		/*g_item[++itemIdx] = new Item(col, row, ITEM_EXPAND);
		//		g_MapInfo[row][col].setItemIdx(itemIdx);*/
		//	}
		//}

		g_Block[3][8]->setNLife(2);
		blockNum += 2;

		g_Block[3][10]->setNLife(1);
		blockNum += 1;

		g_Block[3][12]->setNLife(2);
		blockNum += 2;

		g_Block[3][14]->setNLife(1);
		blockNum += 1;

		g_item[++itemIdx] = new Item(8, 3, ITEM_SLOW);
		g_MapInfo[3][8].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(10, 3, ITEM_EXPAND);
		g_MapInfo[3][10].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(12, 3, ITEM_FAST);
		g_MapInfo[3][12].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(14, 3, ITEM_SHORTEN);
		g_MapInfo[3][14].setItemIdx(itemIdx);
	}
	else if (g_nStage == 1 && !g_StageInitInfo[1]) // �������� 2�϶� �ʱ�ȭ
	{
		itemIdx = -1;
		blockNum = 0;
		g_StageInitInfo[1] = true;

		g_Block[3][10]->setNLife(1);
		blockNum += 1;

		g_Block[3][12]->setNLife(2);
		blockNum += 2;

		g_Block[3][14]->setNLife(1);
		blockNum += 1;

		g_Block[2][10]->setNLife(2);
		blockNum += 2;

		g_Block[2][14]->setNLife(2);
		blockNum += 2;

		g_item[++itemIdx] = new Item(10, 3, ITEM_SLOW);
		g_MapInfo[3][10].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(12, 3, ITEM_EXPAND);
		g_MapInfo[3][12].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(14, 3, ITEM_FAST);
		g_MapInfo[3][14].setItemIdx(itemIdx);
	}
	else if (g_nStage == 2 && !g_StageInitInfo[2]) // �������� 2�϶� �ʱ�ȭ
	{
		itemIdx = -1;
		blockNum = 0;
		g_StageInitInfo[2] = true;

		g_Block[2][6]->setNLife(2);
		blockNum += 2;

		g_Block[2][8]->setNLife(1);
		blockNum += 1;

		g_Block[2][10]->setNLife(1);
		blockNum += 1;

		g_Block[2][12]->setNLife(1);
		blockNum += 1;

		g_Block[2][14]->setNLife(1);
		blockNum += 1;

		g_Block[2][16]->setNLife(2);
		blockNum += 2;

		g_Block[3][6]->setNLife(1);
		blockNum += 1;

		g_Block[3][16]->setNLife(1);
		blockNum += 1;

		g_Block[4][6]->setNLife(1);
		blockNum += 1;

		g_Block[4][10]->setNLife(2);
		blockNum += 2;

		g_Block[4][12]->setNLife(2);
		blockNum += 2;

		g_Block[4][16]->setNLife(1);
		blockNum += 1;

		g_Block[5][6]->setNLife(1);
		blockNum += 1;

		g_Block[5][16]->setNLife(1);
		blockNum += 1;

		g_Block[6][6]->setNLife(2);
		blockNum += 2;

		g_Block[6][8]->setNLife(1);
		blockNum += 1;

		g_Block[6][10]->setNLife(1);
		blockNum += 1;

		g_Block[6][12]->setNLife(1);
		blockNum += 1;

		g_Block[6][14]->setNLife(1);
		blockNum += 1;

		g_Block[6][16]->setNLife(2);
		blockNum += 2;

		g_item[++itemIdx] = new Item(6, 2, ITEM_SLOW);
		g_MapInfo[2][6].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(6, 6, ITEM_SLOW);
		g_MapInfo[6][6].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(10, 4, ITEM_EXPAND);
		g_MapInfo[4][10].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(16, 2, ITEM_FAST);
		g_MapInfo[2][16].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(12, 4, ITEM_FAST);
		g_MapInfo[4][12].setItemIdx(itemIdx);

		g_item[++itemIdx] = new Item(16, 6, ITEM_FAST);
		g_MapInfo[6][16].setItemIdx(itemIdx);
	}

	// ������
	for (int idx = 0; idx <= itemIdx; idx++)
	{
		g_item[idx]->setNLife(0);
	}

	// ��
	g_Ball.setNHP(3);
	g_Ball.setNX(10);
	g_Ball.setNY(20 + 1);
	g_Ball.setOldTime(clock());
	g_Ball.setNReady(1);
	g_Ball.setNDirect(RIGHT_TOP);
	g_Ball.setMoveTime(150);

	// �÷��̾�
	g_Bar.setLength(3);
	g_Bar.setNX(g_Ball.getNX());
	g_Bar.setNY(g_Ball.getNY() + 1);
	g_Bar.setOldTime(clock());
}

void Update()
{
	clock_t CurTime = clock();

	switch (g_nGameState)
	{
	case READY:
		if (CurTime - g_UpdateOldTime > 3000)  // 2��
		{
			g_nGameState = RUNNING;
			g_GameStartTime = CurTime;
		}
		break;

	case RUNNING:
		g_RemainTime = ((g_nStage + 1)*60*1000 - (CurTime - g_GameStartTime)) / 1000; // ���� ���� ���� �ð�

		if ((CurTime - g_GameStartTime) > (g_nStage + 1) * 60 * 1000 || blockNum == 0)
		{
			g_nGameState = STOP;
			return;
		}
		else if (g_Ball.getNReady() == 0)
		{
			if (CurTime - g_Ball.getOldTime() > g_Ball.getMoveTime())
			{
				g_Ball.setOldTime(CurTime);

				switch (g_Ball.getNDirect())
				{
				case TOP:
					if (!IsCollision(g_Ball.getNX(), g_Ball.getNY() - 1))
					{
						g_Ball.MoveTop();
					}
					break;
				case RIGHT_TOP:
					if (!IsCollision(g_Ball.getNX() + 1, g_Ball.getNY() - 1))
					{
						g_Ball.MoveRight();
						g_Ball.MoveTop();
					}
					break;
				case RIGHT_DOWN:
					if (!IsCollision(g_Ball.getNX() + 1, g_Ball.getNY() + 1))
					{
						g_Ball.MoveRight();
						g_Ball.MoveDown();
					}
					break;
				case DOWN:
					if (!IsCollision(g_Ball.getNX(), g_Ball.getNY() + 1))
						g_Ball.MoveDown();
					break;
				case LEFT_DOWN:
					if (!IsCollision(g_Ball.getNX() - 1, g_Ball.getNY() + 1))
					{
						g_Ball.MoveLeft();
						g_Ball.MoveDown();
					}
					break;
				case LEFT_TOP:
					if (!IsCollision(g_Ball.getNX() - 1, g_Ball.getNY() - 1))
					{
						g_Ball.MoveLeft();
						g_Ball.MoveTop();
					}
					break;
				}
			}
		}

		// item  ������ ����
		for (int i = 0; i <= itemIdx; i++)
		{
			// �ٴ� �Ʒ��� ���� ���ش�.
			if (g_item[i]->getNY() > BORDER_DOWN)
			{
				g_item[i]->setNLife(0);
			}

			// bar�� �浹�ϸ� �ش� ������ ����
			int half = g_Bar.getLength() / 2;
			if (g_item[i]->getNY() == g_Bar.getNY() - 1 && g_item[i]->getNLife() == 1)
			{
				int x = g_item[i]->getNX();

				if ((x >= g_Bar.getNX() - (half)) && (x <= g_Bar.getNX() + (half)))
				{
					switch (g_item[i]->getSort())
					{
					case ITEM_SLOW:
						g_Ball.setMoveTime(g_Ball.getMoveTime() + 100);
						break;

					case ITEM_FAST:
						if (g_Ball.getMoveTime() > 100)
							g_Ball.setMoveTime(g_Ball.getMoveTime() - 100);
						break;

					case ITEM_EXPAND:
						g_Bar.setLength(g_Bar.getLength() + 2);
						break;

					case ITEM_SHORTEN:
						if (g_Bar.getLength() > 3)
							g_Bar.setLength(g_Bar.getLength() - 2);
						break;
					}

					g_item[i]->setNLife(0);
					g_MapInfo[g_item[i]->getNY()][g_item[i]->getNX()].setItemIdx(-1);
				}

			}

			if (CurTime - g_item[i]->getOldTime() > g_item[i]->getMoveTime())
			{
				g_item[i]->setOldTime(CurTime);

				//if (!IsItemCollision(g_item[i], g_item[i]) && g_item[i]->getNLife() == 1 && !g_item[i]->IsOriginPOS())
				if (g_item[i]->getNLife() == 1)
				{
					g_item[i]->MoveDown();
				}
			}
		}
		break;

	case STOP:

		g_nTotalGrade += g_nGrade;
		g_nGrade = 0;

		// �����̳� ���и� �Ǵ����־ ����� ���ִ� �κ��� �;� �Ѵ�.  
		if (blockNum == 0 && g_Ball.getNHP() > 0 && g_RemainTime > 0)
		{
			if (g_nStage == 2) // ������ �������� �����̸� ��� ȭ������
			{
				g_nGameState = RESULT;
				break;
			}
			g_UpdateOldTime = CurTime;
			g_nGameState = SUCCESS;
		}
		else {
			g_nGameState = FAILED;
		}

		break;

	case SUCCESS:
		if (CurTime - g_UpdateOldTime > 3000)
		{
			g_UpdateOldTime = CurTime;
			++g_nStage;
			Init();
			g_nGameState = READY;
		}
		break;
	}
}

void Render()
{
	char string[100];

	ScreenClear();

	switch (g_nGameState)
	{
	case INIT:
		if (g_nStage == 0)
			InitScreen();
		break;

	case READY:
		ReadyScreen();
		sprintf(string, "%d", g_nStage + 1);
		ScreenPrint(13, 7, string);
		break;

	case RUNNING:
		RunningScreen();

		sprintf(string, "�������� : %d", g_nStage + 1);
		ScreenPrint(45, 2, string);

		sprintf(string, "���� : %d ", g_Ball.getNHP());
		ScreenPrint(45, 4, string);

		sprintf(string, "���� : %d ", g_nGrade);
		ScreenPrint(45, 6, string);

		sprintf(string, "���� �ð� : %d �� %d ��", g_RemainTime / 60, g_RemainTime % 60);
		ScreenPrint(45, 8, string);

		sprintf(string, "�� : 1ȸ �浹 �ʿ� ");
		ScreenPrint(45, 12, string);

		sprintf(string, "�� : 2ȸ �浹 �ʿ� ");
		ScreenPrint(45, 14, string);

		sprintf(string, "�� : �� ������ ");
		ScreenPrint(45, 18, string);

		sprintf(string, "�� : �� ������ ");
		ScreenPrint(45, 20, string);

		sprintf(string, "�� : Bar ���� �ø��� ");
		ScreenPrint(45, 22, string);

		sprintf(string, "�� : Bar ���� ���̱� ");
		ScreenPrint(45, 24, string);

		for (int row = 0; row < HEIGHT; row++)
		{
			for (int col = 0; col < WIDTH; col++)
			{
				if (g_Block[row][col]->getNLife() >= 1)
					g_Block[row][col]->DrawBlock();

				for (int i = 0; i <= itemIdx; i++)
				{
					if (g_item[i]->getNLife() == 1 && !g_item[i]->IsOriginPOS())
					{
						switch (g_item[i]->getSort())
						{
						case ITEM_SLOW:
							g_item[i]->DrawSlowItem();
							break;

						case ITEM_FAST:
							g_item[i]->DrawFastItem();
							break;

						case ITEM_EXPAND:
							g_item[i]->DrawExpandItem();
							break;

						case ITEM_SHORTEN:
							g_item[i]->DrawShortenItem();
							break;
						}
					}
				}
			}
		}

		g_Bar.DrawBar();

		g_Ball.DrawBall();

		break;

	case SUCCESS:
		SuccessScreen();
		sprintf(string, "%d Stage", g_nStage + 1);
		ScreenPrint(19, 12, string);
		sprintf(string, "����: %d", g_nTotalGrade);
		ScreenPrint(25, 16, string);
		break;

	case FAILED:
		FailureScreen();
		sprintf(string, "%d Stage!", g_nStage + 1);
		ScreenPrint(16, 4, string);
		break;

	case RESULT:
		ResultScreen();
		sprintf(string, "%d", g_nTotalGrade);
		ScreenPrint(25, 11, string);
		break;
	}

	ScreenFlipping();
}

void Realese()
{

}

int main()
{
	int nKey;
	clock_t CurTime;

	ScreenInit();
	Init();

	while(1)
	{
		if (_kbhit())
		{
			if (g_nGameState == RESULT)
			{

				// bar ��ġ �ʱ�ȭ
				g_Bar.setNX(g_Ball.getNX());
				g_Bar.setNY(g_Ball.getNY() + 1);
				g_Bar.setOldTime(clock());
				break;
			}

			nKey = _getch();

			switch (nKey)
			{
			case ' ':
				if (g_nGameState == INIT && g_nStage == 0)
				{
					g_nGameState = READY;
					g_UpdateOldTime = clock();  // ready�� �����ð� ������ �ֱ� ���� 							
				}
				break;

			case 'k': //�� �߻�
				if (g_nGameState == RUNNING && g_Ball.getNReady() == 1)
				{
					g_Ball.setNReady(0);  // �̵� ����
					g_Ball.setOldTime(clock());
				}
				break;

			case 'j':
				if (g_nGameState == RUNNING)
				{
					CurTime = clock();

					if (CurTime - g_Bar.getOldTime() > g_Bar.getMoveTime())
					{
						g_Bar.setOldTime(CurTime);
						g_Bar.MoveLeft();

						if (g_Ball.getNReady() == 1)
						{
							g_Ball.setNX(g_Bar.getNX());
						}
					}
				}

				break;
			case 'l':
				CurTime = clock();

				if (CurTime - g_Bar.getOldTime() > g_Bar.getMoveTime())
				{
					g_Bar.setOldTime(CurTime);
					g_Bar.MoveRight();

					if (g_Ball.getNReady() == 1)
					{
						g_Ball.setNX(g_Bar.getNX());
					}
				}
				break;

			case 'y':
			case 'Y':
				if (g_nGameState == FAILED)
				{
					Init();
					g_nGameState = READY;
					g_UpdateOldTime = clock();
				}
				break;
			case 'n':
			case 'N':
				if (g_nGameState == FAILED)
				{
					g_nGameState = RESULT;
				}
				break;
			}
		}

		Update();
		Render();
	}

	Realese();
	ScreenRelease();

	return 0;
}
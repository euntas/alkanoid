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

#define BLOCK_ROW 2
#define BLOCK_COL 20

#define BORDER_LEFT 0		// ���� �� ��� ��ǥ
#define BORDER_RIGHT 20		// ������ �� ��� ��ǥ
#define BORDER_TOP 1		// ���� �� ��� ��ǥ
#define BORDER_DOWN 22		// ������ �� ��� ��ǥ

int g_StateTable[4][6] = { // ���� �浹�ϰ� �Ǹ� ���� ����
	{  3,  2, -1, -1, -1,  4 },
	{ -1,  5,  4, -1, -1, -1 },
	{ -1, -1,  1,  0,  5, -1 },
	{ -1, -1, -1, -1,  2,  1 }
};

int g_StateBlock[6] = { 3, 2, 1, 0, 5, 4 }; // ���� �浹�ϰ� �Ǹ� ���� ����

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
		ScreenPrint(nX, nY, "��");
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
	Ball() :nReady(1), nHP(3), nX(10), nY(20), nDirect(RIGHT_TOP), OldTime(clock()), MoveTime(200)
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

Bar  g_Bar;
Ball g_Ball;
Block* g_Block[BLOCK_ROW][BLOCK_COL];
int blockNum = 0;  // ���� �����ϴ� �� ��

bool IsCollision(int x, int y)
{
	DIRECT currentDirect = g_Ball.getNDirect();
	int nCount = 0;

	// ���� ������ �浹
	for (int row = 0; row < BLOCK_ROW; row++)
	{
		for (int col = 0; col < BLOCK_COL; col++)
		{
			if (g_Block[row][col]->getNLife() == 1)
			{
				if (y == g_Block[row][col]->getNY()) 
				{
					if ((g_Block[row][col]->getNX() == x ) && (g_Block[row][col]->getNX() + 1 == x+1))
					{
						g_Ball.setNDirect((DIRECT)g_StateBlock[currentDirect]);
						g_Block[row][col]->setNLife(0);
						blockNum--;
						nCount++;
					}
				}
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

	if (y > BORDER_DOWN) // �Ʒ�
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_DOWN][currentDirect]);
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

void Init()
{
	// ���
	for (int row = 0; row < BLOCK_ROW; row++)
	{
		for (int col = 0; col < BLOCK_COL; col++)
		{
			g_Block[row][col] = new Block(col + 1, row + 1, 1);
			blockNum++;
		}
	}

	// ��
	g_Ball.setOldTime(clock());

	// �÷��̾�
	g_Bar.setNX(g_Ball.getNX());
	g_Bar.setNY(g_Ball.getNY() + 1);
	g_Bar.setOldTime(clock());
}

void Update()
{
	clock_t CurTime = clock();

	if (g_Ball.getNReady() == 0)
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
}

void Render()
{
	char string[100];

	ScreenClear();

	RunningScreen();

	sprintf(string, "�������� : %d", 1);
	ScreenPrint(45, 2, string);

	sprintf(string, "���� : %d ", 1);
	ScreenPrint(45, 4, string);

	sprintf(string, "���� : %d ", 1);
	ScreenPrint(45, 6, string);

	sprintf(string, "���� �ð� : %d �� %d ��", 1, 1);
	ScreenPrint(45, 8, string);

	for (int row = 0; row < BLOCK_ROW; row++)
	{
		for (int col = 0; col < BLOCK_COL; col++)
		{
			if (g_Block[row][col]->getNLife() == 1)
				g_Block[row][col]->DrawBlock();
		}
	}
	g_Bar.DrawBar();
	g_Ball.DrawBall();

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
			nKey = _getch();
			switch (nKey)
			{
			case 'k': //�� �߻�
				if (g_Ball.getNReady() == 1)
				{
					g_Ball.setNReady(0);  // �̵� ����
					g_Ball.setOldTime(clock());
				}
				break;

			case 'j':
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
			}
		}

		Update();
		Render();
	}

	Realese();
	ScreenRelease();

	return 0;
}
#include <iostream>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include "Screen.h"

#pragma warning(disable:4996)

typedef enum _GAME_STATE { INIT, READY, RUNNING, SUCCESS, FAILED, STOP, RESULT } GAME_STATE;
typedef enum _DIRECT { TOP, LEFT_TOP, LEFT_DOWN, DOWN, RIGHT_DOWN, RIGHT_TOP } DIRECT;

class Ball
{
private:
	int		 nReady;     // 1 이면 준비 상태, 0이면 이동 상태
	int		 nHP;        // 생명
	int		 nX, nY;     // 좌표
	DIRECT   nDirect;    // 이동 방향
	clock_t  MoveTime;   // 이동 시간 간격
	clock_t  OldTime;	 // 이전 이동 시각

public:
	Ball() :nReady(1), nHP(3), nX(19), nY(20), nDirect(TOP), OldTime(clock()), MoveTime(50)
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
		ScreenPrint(nX, nY, "●");
	}

	void MoveLeft()
	{
		nX -= 2;
	}

	void MoveRight()
	{
		nX += 2;
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
	int nX;				// x축 중심 좌표이다
	int nY;
	int length;			// 홀수로만 지정 (최소 3)
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
			ScreenPrint(nX + (2*i), nY, "▣");
		}
	}

	void MoveLeft()
	{
		int half = length / 2;

		if (nX - 2*half > 2) // 경계 영역 충돌 체크
		{
			nX -= 2;
		}
	}

	void MoveRight()
	{
		int half = length / 2;

		if (nX + 2*half < 40) // 경계 영역 충돌 체크
		{
			nX += 2;
		}
	}
};

Bar  g_Bar;
Ball g_Ball;

bool IsCollision(int x, int y)
{
	// 경계 영역 충돌 체크
	if (x < 2 || x > 39 || y < 1 || y > 22)
	{
		return true;
	}
	
	// 공과 막대기 충돌
	int half = g_Bar.getLength() / 2;
	for (int i = -half; i <= half; i++)
	{
		if (g_Ball.getNY() == g_Bar.getNY())
		{
			int half = g_Bar.getLength() / 2;
			if ((g_Ball.getNX() >= (-2) * g_Bar.getNX() * half) && (g_Ball.getNX() <= (2 * g_Bar.getNX() * half) + 2))
			{
				return true;
			}
		}
	}

	return false;
}

void RunningScreen()
{
	ScreenPrint(0, 0, "┏━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0, 1, "┃                                        ┃");
	ScreenPrint(0, 2, "┃                                        ┃");
	ScreenPrint(0, 3, "┃                                        ┃");
	ScreenPrint(0, 4, "┃                                        ┃");
	ScreenPrint(0, 5, "┃                                        ┃");
	ScreenPrint(0, 6, "┃                                        ┃");
	ScreenPrint(0, 7, "┃                                        ┃");
	ScreenPrint(0, 8, "┃                                        ┃");
	ScreenPrint(0, 9, "┃                                        ┃");
	ScreenPrint(0, 10, "┃                                        ┃");
	ScreenPrint(0, 11, "┃                                        ┃");
	ScreenPrint(0, 12, "┃                                        ┃");
	ScreenPrint(0, 13, "┃                                        ┃");
	ScreenPrint(0, 14, "┃                                        ┃");
	ScreenPrint(0, 15, "┃                                        ┃");
	ScreenPrint(0, 16, "┃                                        ┃");
	ScreenPrint(0, 17, "┃                                        ┃");
	ScreenPrint(0, 18, "┃                                        ┃");
	ScreenPrint(0, 19, "┃                                        ┃");
	ScreenPrint(0, 20, "┃                                        ┃");
	ScreenPrint(0, 21, "┃                                        ┃");
	ScreenPrint(0, 22, "┃                                        ┃");
	ScreenPrint(0, 23, "┗━━━━━━━━━━━━━━━━━━━━┛");
}

void Init()
{
	g_Ball.setOldTime(clock());

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
				/*if (Collision(g_sBall.nX, g_sBall.nY - 1) == 0)
					g_sBall.nY--;*/
				if (!IsCollision(g_Ball.getNX(), g_Ball.getNY()))
				{
					g_Ball.MoveTop();
				}
				break;
			/*case LEFT_TOP:
				if (Collision(g_sBall.nX + 1, g_sBall.nY - 1) == 0)
				{
					g_sBall.nX++;
					g_sBall.nY--;
				}
				break;
			case LEFT_DOWN:
				if (Collision(g_sBall.nX + 1, g_sBall.nY + 1) == 0)
				{
					g_sBall.nX++;
					g_sBall.nY++;
				}
				break;
			case DOWN:
				if (Collision(g_sBall.nX, g_sBall.nY + 1) == 0)
					g_sBall.nY++;
				break;
			case RIGHT_DOWN:
				if (Collision(g_sBall.nX - 1, g_sBall.nY + 1) == 0)
				{
					g_sBall.nX--;
					g_sBall.nY++;
				}
				break;
			case RIGHT_TOP:
				if (Collision(g_sBall.nX - 1, g_sBall.nY - 1) == 0)
				{
					g_sBall.nX--;
					g_sBall.nY--;
				}
				break;*/
			}
		}
	}
}

void Render()
{
	char string[100];

	ScreenClear();

	RunningScreen();

	sprintf(string, "스테이지 : %d", 1);
	ScreenPrint(45, 2, string);

	sprintf(string, "생명 : %d ", 1);
	ScreenPrint(45, 4, string);

	sprintf(string, "점수 : %d ", 1);
	ScreenPrint(45, 6, string);

	sprintf(string, "남은 시간 : %d 분 %d 초", 1, 1);
	ScreenPrint(45, 8, string);

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
			case 'k': //공 발사
				if (g_Ball.getNReady() == 1)
				{
					g_Ball.setNReady(0);  // 이동 상태
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
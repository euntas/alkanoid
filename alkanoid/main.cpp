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

#define WIDTH 22
#define HEIGHT 23

#define BORDER_LEFT 0		// 왼쪽 벽 경계 좌표
#define BORDER_RIGHT 20		// 오른쪽 벽 경계 좌표
#define BORDER_TOP 1		// 위쪽 벽 경계 좌표
#define BORDER_DOWN 22		// 아래쪽 벽 경계 좌표

int g_StateTable[4][6] = { // 벽과 충돌하게 되면 방향 상태
	{  3,  2, -1, -1, -1,  4 },
	{ -1,  5,  4, -1, -1, -1 },
	{ -1, -1,  1,  0,  5, -1 },
	{ -1, -1, -1, -1,  2,  1 }
};

int g_StateBlock[6] = { 3, 2, 1, 0, 5, 4 }; // 블럭과 충돌하게 되면 방향 상태

class Block
{
private:
	int		 nX, nY;     // 좌표
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
		ScreenPrint(nX, nY, "▩");
	}
};

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
		ScreenPrint(nX, nY, "●");
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
			ScreenPrint(nX + i, nY, "▣");
		}
	}

	void MoveLeft()
	{
		int half = length / 2;

		if (nX - half > BORDER_LEFT + 1) // 경계 영역 충돌 체크
		{
			nX -= 1;
		}
	}

	void MoveRight()
	{
		int half = length / 2;

		if (nX + half < BORDER_RIGHT) // 경계 영역 충돌 체크
		{
			nX += 1;
		}
	}
};

Bar  g_Bar;
Ball g_Ball;
Block* g_Block[HEIGHT+1][WIDTH+1];
int blockNum = 0;  // 현재 존재하는 블럭 수
int g_nGrade = 0;
int g_nTotalGrade = 0;
int g_nStage = -1;
int g_nGameState = INIT;
clock_t	g_GameStartTime, g_UpdateOldTime, g_RemainTime;

bool IsCollision(int x, int y)
{
	DIRECT currentDirect = g_Ball.getNDirect();
	int nCount = 0;

	// 공과 벽돌의 충돌
	int tempX = x;
	if (g_Ball.getNDirect() == LEFT_DOWN || g_Ball.getNDirect() == LEFT_TOP)
	{
		tempX = x + 1;
	}

	if (g_Ball.getNDirect() == RIGHT_DOWN || g_Ball.getNDirect() == RIGHT_TOP)
	{
		tempX = x - 1;
	}
	
	if (g_Block[y][tempX]->getNLife() == 1 && g_Block[y][tempX]->getNY() == y)
	{
		if (g_Block[y][tempX]->getNX() == tempX || g_Block[y][tempX]->getNX() + 1 == tempX)
		{
			g_Ball.setNDirect((DIRECT)g_StateBlock[currentDirect]);
			g_Block[y][tempX]->setNLife(0);
			blockNum--;
			nCount++;
			g_nGrade += 10;
		}
	}
	else if (g_Block[y][x]->getNLife() == 1 &&g_Block[y][tempX]->getNY() == y)
	{
		if (g_Block[y][x]->getNX() == x || g_Block[y][x]->getNX()+1 == x )
		{
			g_Ball.setNDirect((DIRECT)g_StateBlock[currentDirect]);
			g_Block[y][x]->setNLife(0);
			blockNum--;
			nCount++;
			g_nGrade += 10;
		}
	}
	
	// 충돌 체크 
	if (nCount != 0)
	{
		return true;
	}

	// 공과 막대기 충돌
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

	// 경계 영역 충돌 체크
	if (x < BORDER_LEFT + 1)	// 왼쪽
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_LEFT][currentDirect]);
		return true;
	}

	if (x > BORDER_RIGHT) // 오른쪽
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_RIGHT][currentDirect]);
		return true;
	}

	if (y < BORDER_TOP) // 위
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_TOP][currentDirect]);
		return true;
	}

	if (y > g_Bar.getNY()) // 아래
	{
		g_Ball.decHP();

		if (g_Ball.getNHP() == 0)
		{	// 게임 판정으로 
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
	ScreenPrint(0, 0, "┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓");
	ScreenPrint(0, 1, "┃                                         ┃");
	ScreenPrint(0, 2, "┃                                         ┃");
	ScreenPrint(0, 3, "┃                                         ┃");
	ScreenPrint(0, 4, "┃                                         ┃");
	ScreenPrint(0, 5, "┃                                         ┃");
	ScreenPrint(0, 6, "┃                                         ┃");
	ScreenPrint(0, 7, "┃                                         ┃");
	ScreenPrint(0, 8, "┃                                         ┃");
	ScreenPrint(0, 9, "┃                                         ┃");
	ScreenPrint(0, 10, "┃                                         ┃");
	ScreenPrint(0, 11, "┃                                         ┃");
	ScreenPrint(0, 12, "┃                                         ┃");
	ScreenPrint(0, 13, "┃                                         ┃");
	ScreenPrint(0, 14, "┃                                         ┃");
	ScreenPrint(0, 15, "┃                                         ┃");
	ScreenPrint(0, 16, "┃                                         ┃");
	ScreenPrint(0, 17, "┃                                         ┃");
	ScreenPrint(0, 18, "┃                                         ┃");
	ScreenPrint(0, 19, "┃                                         ┃");
	ScreenPrint(0, 20, "┃                                         ┃");
	ScreenPrint(0, 21, "┃                                         ┃");
	ScreenPrint(0, 22, "┃                                         ┃");
	ScreenPrint(0, 23, "┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┛");

}

void ReadyScreen()
{
	ScreenPrint(0, 0, "┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓");
	ScreenPrint(0, 1, "┃                                        ┃");
	ScreenPrint(0, 2, "┃                                        ┃");
	ScreenPrint(0, 3, "┃                                        ┃");
	ScreenPrint(0, 4, "┃                                        ┃");
	ScreenPrint(0, 5, "┃                                        ┃");
	ScreenPrint(0, 6, "┃                                        ┃");
	ScreenPrint(0, 7, "┃              S t a g e                 ┃");
	ScreenPrint(0, 8, "┃                            ●          ┃");
	ScreenPrint(0, 9, "┃                      ☜〓━┃          ┃");
	ScreenPrint(0, 10, "┃                        ☜〓┛          ┃");
	ScreenPrint(0, 11, "┃                          ┏┻┓        ┃");
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
	ScreenPrint(0, 23, "┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┛");
}

void SuccessScreen()
{
	ScreenPrint(0, 0, "┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓");
	ScreenPrint(0, 1, "┃      *.                                ┃");
	ScreenPrint(0, 2, "┃              .*            .*          ┃");
	ScreenPrint(0, 3, "┃     *.                        .*       ┃");
	ScreenPrint(0, 4, "┃       *.             .*                ┃");
	ScreenPrint(0, 5, "┃           *.                           ┃");
	ScreenPrint(0, 6, "┃            *.             .*           ┃");
	ScreenPrint(0, 7, "┃     *.      *.┏┓┏┓.*대    .*       ┃");
	ScreenPrint(0, 8, "┃             ┏┻┫┣┻┓단             ┃");
	ScreenPrint(0, 9, "┃     *.      ┃━┫┣━┃해      .*     ┃");
	ScreenPrint(0, 10, "┃             ┃━┫┣━┃요~            ┃");
	ScreenPrint(0, 11, "┃        *.   ┗━┛┗━┛               ┃");
	ScreenPrint(0, 12, "┃                                        ┃");
	ScreenPrint(0, 13, "┃                          .*            ┃");
	ScreenPrint(0, 14, "┃  *.           You Win!!          *.    ┃");
	ScreenPrint(0, 15, "┃                                        ┃");
	ScreenPrint(0, 16, "┃                                        ┃");
	ScreenPrint(0, 17, "┃                       *.               ┃");
	ScreenPrint(0, 18, "┃    *.         *.                       ┃");
	ScreenPrint(0, 19, "┃                                 *.     ┃");
	ScreenPrint(0, 20, "┃                      *.                ┃");
	ScreenPrint(0, 21, "┃  *.                             *.     ┃");
	ScreenPrint(0, 22, "┃                                        ┃");
	ScreenPrint(0, 23, "┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┛");
}

void FailureScreen()
{
	ScreenPrint(0, 0, "┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓");
	ScreenPrint(0, 1, "┃  ■■■■■■■■■■■■■■■■■■  ┃");
	ScreenPrint(0, 2, "┃  ■■■■■■■■■■■■■■■■■■  ┃");
	ScreenPrint(0, 3, "┃                                        ┃");
	ScreenPrint(0, 4, "┃                                        ┃");
	ScreenPrint(0, 5, "┃                                        ┃");
	ScreenPrint(0, 6, "┃                                        ┃");
	ScreenPrint(0, 7, "┃             *┭┮  ┭┮*               ┃");
	ScreenPrint(0, 8, "┃          *.  ㅣㅣ▽ㅣㅣ   .*           ┃");
	ScreenPrint(0, 9, "┃   ┏ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ┓     ┃");
	ScreenPrint(0, 10, "┃   ┃다음 번 기회를 이용해 주세요┃     ┃");
	ScreenPrint(0, 11, "┃   ┗ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ┛     ┃");
	ScreenPrint(0, 12, "┃                                        ┃");
	ScreenPrint(0, 13, "┃                                        ┃");
	ScreenPrint(0, 14, "┃    (*⌒⌒*)                            ┃");
	ScreenPrint(0, 15, "┃   ┏O━━━O━━━━━━━━━━┓     ┃");
	ScreenPrint(0, 16, "┃   ┃계속하시겠습니까? <y/n>     ┃     ┃");
	ScreenPrint(0, 17, "┃   ┗━━━━━━━━━━━━━━┛     ┃");
	ScreenPrint(0, 18, "┃                                        ┃");
	ScreenPrint(0, 19, "┃                                        ┃");
	ScreenPrint(0, 20, "┃                                        ┃");
	ScreenPrint(0, 21, "┃  ■■■■■■■■■■■■■■■■■■  ┃");
	ScreenPrint(0, 22, "┃  ■■■■■■■■■■■■■■■■■■  ┃");
	ScreenPrint(0, 23, "┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┛");
}

void ResultScreen()
{
	ScreenPrint(0, 0, "┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓");
	ScreenPrint(0, 1, "┃                                        ┃");
	ScreenPrint(0, 2, "┃                                        ┃");
	ScreenPrint(0, 3, "┃                                        ┃");
	ScreenPrint(0, 4, "┃                                        ┃");
	ScreenPrint(0, 5, "┃                                        ┃");
	ScreenPrint(0, 6, "┃                                        ┃");
	ScreenPrint(0, 7, "┃   (((    벽돌 깨기  게임 Score  ))))   ┃");
	ScreenPrint(0, 8, "┃                                        ┃");
	ScreenPrint(0, 9, "┃                                        ┃");
	ScreenPrint(0, 10, "┃                                        ┃");
	ScreenPrint(0, 11, "┃           총 점수  :                   ┃");
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
	ScreenPrint(0, 23, "┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┛");
}

void InitScreen() // 게임 소개 화면
{
	ScreenPrint(0, 0, "┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓");
	ScreenPrint(0, 1, "┃                                        ┃");
	ScreenPrint(0, 2, "┃    ■■■■■■■■■■■■■■■■    ┃");
	ScreenPrint(0, 3, "┃      ■■■■■■■■■■■■■■      ┃");
	ScreenPrint(0, 4, "┃        ■■■■■■■■■■■■        ┃");
	ScreenPrint(0, 5, "┃          ■■■■■■■■■■          ┃");
	ScreenPrint(0, 6, "┃          ┏━━━━━━━━┓          ┃");
	ScreenPrint(0, 7, "┃          ┃★ 벽돌 깨기  ★┃          ┃");
	ScreenPrint(0, 8, "┃          ┗━━━━━┳━━┛          ┃");
	ScreenPrint(0, 9, "┃              /)/)    ┃ ☆             ┃");
	ScreenPrint(0, 10, "┃           ★(*'')/                     ┃");
	ScreenPrint(0, 11, "┃          ■■■■■■■■■■          ┃");
	ScreenPrint(0, 12, "┃        ■■■■■■■■■■■■        ┃");
	ScreenPrint(0, 13, "┃      ■■■■■■■■■■■■■■      ┃");
	ScreenPrint(0, 14, "┃    ■■■■■■■■■■■■■■■■    ┃");
	ScreenPrint(0, 15, "┃                                        ┃");
	ScreenPrint(0, 16, "┃                                        ┃");
	ScreenPrint(0, 17, "┃    ★ 스페이스키를 눌러주세요!! ★     ┃");
	ScreenPrint(0, 18, "┃                                        ┃");
	ScreenPrint(0, 19, "┃     J:좌  L:우 K: 공발사               ┃");
	ScreenPrint(0, 20, "┃                                        ┃");
	ScreenPrint(0, 21, "┃                                        ┃");
	ScreenPrint(0, 22, "┃                                        ┃");
	ScreenPrint(0, 23, "┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┛");
}

void Init()
{
	if (g_nStage == -1) // 1번만 초기화가 되는 부분에 대한 처리 사항
	{
		g_nStage = 0;

		// 블록
		for (int row = 0; row <= HEIGHT; row++)
		{
			for (int col = 0; col <= WIDTH; col++)
			{
				g_Block[row][col] = new Block(col, row, 0);
			}
		}

		for (int row = 3; row <= BLOCK_ROW + 1; row++)
		{
			for (int col = 10; col <= BLOCK_COL - 8; col++)
			{
				g_Block[row][col]->setNLife(1);
				blockNum++;
			}
		}

		g_nGrade = 0;
	}

	// 공
	g_Ball.setNHP(3);
	g_Ball.setNX(10);
	g_Ball.setNY(20 + 1);
	g_Ball.setOldTime(clock());

	// 플레이어
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
		if (CurTime - g_UpdateOldTime > 3000)  // 2초
		{
			g_nGameState = RUNNING;
			g_GameStartTime = CurTime;
		}
		break;

	case RUNNING:
		//g_RemainTime = (g_StageInfo[g_nStage].LimitTime - (CurTime - g_GameStartTime)) / 1000; // 게임 진행 남은 시간
		g_RemainTime = (60*1000 - (CurTime - g_GameStartTime)) / 1000; // 게임 진행 남은 시간

		//if ((CurTime - g_GameStartTime) > g_StageInfo[g_nStage].LimitTime || g_nBlockCount == g_StageInfo[g_nStage].nBlockCount)
		if ((CurTime - g_GameStartTime) > 60 * 1000 || blockNum == 0)
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
		break;

	case STOP:
		// 성공이냐 실패를 판단해주어서 출력을 해주는 부분이 와야 한다.  
		//if (g_nBlockCount == g_StageInfo[g_nStage].nBlockCount && g_sBall.nHP > 0 && g_RemainTime > 0)
		if (blockNum == 0)
		{
			g_UpdateOldTime = CurTime;
			g_nGameState = SUCCESS;
			g_nTotalGrade += g_nGrade;
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

		sprintf(string, "스테이지 : %d", g_nStage + 1);
		ScreenPrint(45, 2, string);

		sprintf(string, "생명 : %d ", g_Ball.getNHP());
		ScreenPrint(45, 4, string);

		sprintf(string, "점수 : %d ", g_nGrade);
		ScreenPrint(45, 6, string);

		sprintf(string, "남은 시간 : %d 분 %d 초", g_RemainTime / 60, g_RemainTime % 60);
		ScreenPrint(45, 8, string);

		for (int row = 0; row < HEIGHT; row++)
		{
			for (int col = 0; col < WIDTH; col++)
			{
				if (g_Block[row][col]->getNLife() == 1)
					g_Block[row][col]->DrawBlock();
			}
		}

		g_Bar.DrawBar();

		g_Ball.DrawBall();

		break;

	case SUCCESS:
		SuccessScreen();
		sprintf(string, "%d Stage", g_nStage + 1);
		ScreenPrint(19, 12, string);
		sprintf(string, "총점: %d", g_nTotalGrade);
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

				// bar 위치 초기화
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
					g_UpdateOldTime = clock();  // ready를 일정시간 지속해 주기 위해 							
				}
				break;

			case 'k': //공 발사
				if (g_nGameState == RUNNING && g_Ball.getNReady() == 1)
				{
					g_Ball.setNReady(0);  // 이동 상태
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
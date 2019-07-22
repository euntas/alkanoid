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

#define BORDER_LEFT 0		// 豭薹 漁 唳啗 謝ル
#define BORDER_RIGHT 20		// 螃艇薹 漁 唳啗 謝ル
#define BORDER_TOP 1		// 嬪薹 漁 唳啗 謝ル
#define BORDER_DOWN 22		// 嬴楚薹 漁 唳啗 謝ル

int g_StateTable[4][6] = { // 漁婁 醱給ж啪 腎賊 寞щ 鼻鷓
	{  3,  2, -1, -1, -1,  4 },
	{ -1,  5,  4, -1, -1, -1 },
	{ -1, -1,  1,  0,  5, -1 },
	{ -1, -1, -1, -1,  2,  1 }
};

int g_StateBlock[6] = { 3, 2, 1, 0, 5, 4 }; // 綰楣婁 醱給ж啪 腎賊 寞щ 鼻鷓

class Item
{
private:
	int		 originX, originY; // 籀擠 謝ル
	int		 nX, nY;     // 謝ル
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
		ScreenPrint(nX, nY, "≧");
	}

	void DrawFastItem()
	{
		ScreenPrint(nX, nY, "≠");
	}

	void DrawExpandItem()
	{
		ScreenPrint(nX, nY, "﹢");
	}

	void DrawShortenItem()
	{
		ScreenPrint(nX, nY, "﹣");
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
	int		 nX, nY;     // 謝ル
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
			ScreenPrint(nX, nY, "〤");
		}
		else if (nLife == 2) {
			ScreenPrint(nX, nY, "十");
		}
	}
};

class Ball
{
private:
	int		 nReady;     // 1 檜賊 遽綠 鼻鷓, 0檜賊 檜翕 鼻鷓
	int		 nHP;        // 儅貲
	int		 nX, nY;     // 謝ル
	DIRECT   nDirect;    // 檜翕 寞щ
	clock_t  MoveTime;   // 檜翕 衛除 除問
	clock_t  OldTime;	 // 檜瞪 檜翕 衛陝

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
		ScreenPrint(nX, nY, "≒");
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
	int nX;				// x蹴 醞褕 謝ル檜棻
	int nY;
	int length;			// �汝鷊庛� 雖薑 (譆模 3)
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
			ScreenPrint(nX + i, nY, "〡");
		}
	}

	void MoveLeft()
	{
		int half = length / 2;

		if (nX - half > BORDER_LEFT + 1) // 唳啗 艙羲 醱給 羹觼
		{
			nX -= 1;
		}
	}

	void MoveRight()
	{
		int half = length / 2;

		if (nX + half < BORDER_RIGHT) // 唳啗 艙羲 醱給 羹觼
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
Item* g_item[100];		// 歜曖煎 100偃
int itemIdx = -1; 
int blockNum = 0;  // ⑷營 襄營ж朝 綰楣 熱 (薑�旅鰲� 襄營ж朝 綰楣曖 life 熱)

MapInfo g_MapInfo[HEIGHT + 1][WIDTH + 1];

bool g_StageInitInfo[3] = { false, false, false }; // 蝶纔檜雖 滌 蟾晦�飛� 嬪и Ы楚斜 (false橾 唳辦 и廓虜 蟾晦�� п遽棻.)

int g_nGrade = 0;
int g_nTotalGrade = 0;
int g_nStage = -1;
int g_nGameState = INIT;
clock_t	g_GameStartTime, g_UpdateOldTime, g_RemainTime;

bool IsCollision(int x, int y)
{
	DIRECT currentDirect = g_Ball.getNDirect();
	int nCount = 0;

	// 奢婁 漁給曖 醱給
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

			// 嬴檜蠱 儅貲
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
	
	// 醱給 羹觼 
	if (nCount != 0)
	{
		return true;
	}

	// 奢婁 虞渠晦 醱給
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

	// 唳啗 艙羲 醱給 羹觼
	if (x < BORDER_LEFT + 1)	// 豭薹
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_LEFT][currentDirect]);
		return true;
	}

	if (x > BORDER_RIGHT) // 螃艇薹
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_RIGHT][currentDirect]);
		return true;
	}

	if (y < BORDER_TOP) // 嬪
	{
		g_Ball.setNDirect((DIRECT)g_StateTable[WALL_TOP][currentDirect]);
		return true;
	}

	if (y > g_Bar.getNY()) // 嬴楚
	{
		g_Ball.decHP();

		if (g_Ball.getNHP() == 0)
		{	// 啪歜 っ薑戲煎 
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
	ScreenPrint(0, 0, "旨 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旬");
	ScreenPrint(0, 1, "早                                         早");
	ScreenPrint(0, 2, "早                                         早");
	ScreenPrint(0, 3, "早                                         早");
	ScreenPrint(0, 4, "早                                         早");
	ScreenPrint(0, 5, "早                                         早");
	ScreenPrint(0, 6, "早                                         早");
	ScreenPrint(0, 7, "早                                         早");
	ScreenPrint(0, 8, "早                                         早");
	ScreenPrint(0, 9, "早                                         早");
	ScreenPrint(0, 10, "早                                         早");
	ScreenPrint(0, 11, "早                                         早");
	ScreenPrint(0, 12, "早                                         早");
	ScreenPrint(0, 13, "早                                         早");
	ScreenPrint(0, 14, "早                                         早");
	ScreenPrint(0, 15, "早                                         早");
	ScreenPrint(0, 16, "早                                         早");
	ScreenPrint(0, 17, "早                                         早");
	ScreenPrint(0, 18, "早                                         早");
	ScreenPrint(0, 19, "早                                         早");
	ScreenPrint(0, 20, "早                                         早");
	ScreenPrint(0, 21, "早                                         早");
	ScreenPrint(0, 22, "早                                         早");
	ScreenPrint(0, 23, "曲 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旭");

}

void ReadyScreen()
{
	ScreenPrint(0, 0, "旨 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旬");
	ScreenPrint(0, 1, "早                                        早");
	ScreenPrint(0, 2, "早                                        早");
	ScreenPrint(0, 3, "早                                        早");
	ScreenPrint(0, 4, "早                                        早");
	ScreenPrint(0, 5, "早                                        早");
	ScreenPrint(0, 6, "早                                        早");
	ScreenPrint(0, 7, "早              S t a g e                 早");
	ScreenPrint(0, 8, "早                            ≒          早");
	ScreenPrint(0, 9, "早                      Ｂ㏑收早          早");
	ScreenPrint(0, 10, "早                        Ｂ㏑旭          早");
	ScreenPrint(0, 11, "早                          旨朴旬        早");
	ScreenPrint(0, 12, "早                                        早");
	ScreenPrint(0, 13, "早                                        早");
	ScreenPrint(0, 14, "早                                        早");
	ScreenPrint(0, 15, "早                                        早");
	ScreenPrint(0, 16, "早                                        早");
	ScreenPrint(0, 17, "早                                        早");
	ScreenPrint(0, 18, "早                                        早");
	ScreenPrint(0, 19, "早                                        早");
	ScreenPrint(0, 20, "早                                        早");
	ScreenPrint(0, 21, "早                                        早");
	ScreenPrint(0, 22, "早                                        早");
	ScreenPrint(0, 23, "曲 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旭");
}

void SuccessScreen()
{
	ScreenPrint(0, 0, "旨 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旬");
	ScreenPrint(0, 1, "早      *.                                早");
	ScreenPrint(0, 2, "早              .*            .*          早");
	ScreenPrint(0, 3, "早     *.                        .*       早");
	ScreenPrint(0, 4, "早       *.             .*                早");
	ScreenPrint(0, 5, "早           *.                           早");
	ScreenPrint(0, 6, "早            *.             .*           早");
	ScreenPrint(0, 7, "早     *.      *.旨旬旨旬.*渠    .*       早");
	ScreenPrint(0, 8, "早             旨朴朽曳朴旬欽             早");
	ScreenPrint(0, 9, "早     *.      早收朽曳收早п      .*     早");
	ScreenPrint(0, 10, "早             早收朽曳收早蹂~            早");
	ScreenPrint(0, 11, "早        *.   曲收旭曲收旭               早");
	ScreenPrint(0, 12, "早                                        早");
	ScreenPrint(0, 13, "早                          .*            早");
	ScreenPrint(0, 14, "早  *.           You Win!!          *.    早");
	ScreenPrint(0, 15, "早                                        早");
	ScreenPrint(0, 16, "早                                        早");
	ScreenPrint(0, 17, "早                       *.               早");
	ScreenPrint(0, 18, "早    *.         *.                       早");
	ScreenPrint(0, 19, "早                                 *.     早");
	ScreenPrint(0, 20, "早                      *.                早");
	ScreenPrint(0, 21, "早  *.                             *.     早");
	ScreenPrint(0, 22, "早                                        早");
	ScreenPrint(0, 23, "曲 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旭");
}

void FailureScreen()
{
	ScreenPrint(0, 0, "旨 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旬");
	ScreenPrint(0, 1, "早  ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥  早");
	ScreenPrint(0, 2, "早  ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥  早");
	ScreenPrint(0, 3, "早                                        早");
	ScreenPrint(0, 4, "早                                        早");
	ScreenPrint(0, 5, "早                                        早");
	ScreenPrint(0, 6, "早                                        早");
	ScreenPrint(0, 7, "早             *老考  老考*               早");
	ScreenPrint(0, 8, "早          *.  太太∩太太   .*           早");
	ScreenPrint(0, 9, "早   旨天天天天天天天天天天天天天天旬     早");
	ScreenPrint(0, 10, "早   早棻擠 廓 晦�虜� 檜辨п 輿撮蹂早     早");
	ScreenPrint(0, 11, "早   曲天天天天天天天天天天天天天天旭     早");
	ScreenPrint(0, 12, "早                                        早");
	ScreenPrint(0, 13, "早                                        早");
	ScreenPrint(0, 14, "早    (*÷÷*)                            早");
	ScreenPrint(0, 15, "早   旨O收收收O收收收收收收收收收收旬     早");
	ScreenPrint(0, 16, "早   早啗樓ж衛啊蝗棲梱? <y/n>     早     早");
	ScreenPrint(0, 17, "早   曲收收收收收收收收收收收收收收旭     早");
	ScreenPrint(0, 18, "早                                        早");
	ScreenPrint(0, 19, "早                                        早");
	ScreenPrint(0, 20, "早                                        早");
	ScreenPrint(0, 21, "早  ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥  早");
	ScreenPrint(0, 22, "早  ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥  早");
	ScreenPrint(0, 23, "曲 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旭");
}

void ResultScreen()
{
	ScreenPrint(0, 0, "旨 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旬");
	ScreenPrint(0, 1, "早                                        早");
	ScreenPrint(0, 2, "早                                        早");
	ScreenPrint(0, 3, "早                                        早");
	ScreenPrint(0, 4, "早                                        早");
	ScreenPrint(0, 5, "早                                        早");
	ScreenPrint(0, 6, "早                                        早");
	ScreenPrint(0, 7, "早   (((    漁給 梟晦  啪歜 Score  ))))   早");
	ScreenPrint(0, 8, "早                                        早");
	ScreenPrint(0, 9, "早                                        早");
	ScreenPrint(0, 10, "早                                        早");
	ScreenPrint(0, 11, "早           識 薄熱  :                   早");
	ScreenPrint(0, 12, "早                                        早");
	ScreenPrint(0, 13, "早                                        早");
	ScreenPrint(0, 14, "早                                        早");
	ScreenPrint(0, 15, "早                                        早");
	ScreenPrint(0, 16, "早                                        早");
	ScreenPrint(0, 17, "早                                        早");
	ScreenPrint(0, 18, "早                                        早");
	ScreenPrint(0, 19, "早                                        早");
	ScreenPrint(0, 20, "早                                        早");
	ScreenPrint(0, 21, "早                                        早");
	ScreenPrint(0, 22, "早                                        早");
	ScreenPrint(0, 23, "曲 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旭");
}

void InitScreen() // 啪歜 模偃 �飛�
{
	ScreenPrint(0, 0, "旨 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旬");
	ScreenPrint(0, 1, "早                                        早");
	ScreenPrint(0, 2, "早    ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥    早");
	ScreenPrint(0, 3, "早      ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥      早");
	ScreenPrint(0, 4, "早        ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥        早");
	ScreenPrint(0, 5, "早          ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥          早");
	ScreenPrint(0, 6, "早          旨收收收收收收收收旬          早");
	ScreenPrint(0, 7, "早          早≠ 漁給 梟晦  ≠早          早");
	ScreenPrint(0, 8, "早          曲收收收收收有收收旭          早");
	ScreenPrint(0, 9, "早              /)/)    早 ≧             早");
	ScreenPrint(0, 10, "早           ≠(*'')/                     早");
	ScreenPrint(0, 11, "早          ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥          早");
	ScreenPrint(0, 12, "早        ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥        早");
	ScreenPrint(0, 13, "早      ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥      早");
	ScreenPrint(0, 14, "早    ﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥﹥    早");
	ScreenPrint(0, 15, "早                                        早");
	ScreenPrint(0, 16, "早                                        早");
	ScreenPrint(0, 17, "早    ≠ 蝶む檜蝶酈蒂 揚楝輿撮蹂!! ≠     早");
	ScreenPrint(0, 18, "早                                        早");
	ScreenPrint(0, 19, "早     J:謝  L:辦 K: 奢嫦餌               早");
	ScreenPrint(0, 20, "早                                        早");
	ScreenPrint(0, 21, "早                                        早");
	ScreenPrint(0, 22, "早                                        早");
	ScreenPrint(0, 23, "曲 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 收 旭");
}

void Init()
{
	if (g_nStage == -1) // 1廓虜 蟾晦�降� 腎朝 睡碟縑 渠и 籀葬 餌о
	{
		g_nStage = 0;

		// 綰煙 蟾晦��
		for (int row = 0; row <= HEIGHT; row++)
		{
			for (int col = 0; col <= WIDTH; col++)
			{
				g_Block[row][col] = new Block(col, row, 0);
			}
		}

		g_nGrade = 0;
	}

	if (g_nStage == 0 && !g_StageInitInfo[0]) // 蝶纔檜雖 1橾陽 蟾晦��
	{
		itemIdx = -1;
		blockNum = 0;
		g_StageInitInfo[0] = true;

		// 蝶纔檜雖縑 蜃朝 綰楣, 嬴檜蠱 撲薑
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
	else if (g_nStage == 1 && !g_StageInitInfo[1]) // 蝶纔檜雖 2橾陽 蟾晦��
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
	else if (g_nStage == 2 && !g_StageInitInfo[2]) // 蝶纔檜雖 2橾陽 蟾晦��
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

	// 嬴檜蠱
	for (int idx = 0; idx <= itemIdx; idx++)
	{
		g_item[idx]->setNLife(0);
	}

	// 奢
	g_Ball.setNHP(3);
	g_Ball.setNX(10);
	g_Ball.setNY(20 + 1);
	g_Ball.setOldTime(clock());
	g_Ball.setNReady(1);
	g_Ball.setNDirect(RIGHT_TOP);
	g_Ball.setMoveTime(150);

	// Ы溯檜橫
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
		if (CurTime - g_UpdateOldTime > 3000)  // 2蟾
		{
			g_nGameState = RUNNING;
			g_GameStartTime = CurTime;
		}
		break;

	case RUNNING:
		g_RemainTime = ((g_nStage + 1)*60*1000 - (CurTime - g_GameStartTime)) / 1000; // 啪歜 霞ч 陴擎 衛除

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

		// item  遺霜歜 嬪л
		for (int i = 0; i <= itemIdx; i++)
		{
			// 夥款 嬴楚煎 陛賊 橈撻棻.
			if (g_item[i]->getNY() > BORDER_DOWN)
			{
				g_item[i]->setNLife(0);
			}

			// bar諦 醱給ж賊 п渡 嬴檜蠱 褒ч
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

		// 撩奢檜傍 褒ぬ蒂 っ欽п輿橫憮 轎溘擊 п輿朝 睡碟檜 諦撿 и棻.  
		if (blockNum == 0 && g_Ball.getNHP() > 0 && g_RemainTime > 0)
		{
			if (g_nStage == 2) // 葆雖虞 蝶纔檜雖 撩奢檜賊 唸婁 �飛橉虞�
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

		sprintf(string, "蝶纔檜雖 : %d", g_nStage + 1);
		ScreenPrint(45, 2, string);

		sprintf(string, "儅貲 : %d ", g_Ball.getNHP());
		ScreenPrint(45, 4, string);

		sprintf(string, "薄熱 : %d ", g_nGrade);
		ScreenPrint(45, 6, string);

		sprintf(string, "陴擎 衛除 : %d 碟 %d 蟾", g_RemainTime / 60, g_RemainTime % 60);
		ScreenPrint(45, 8, string);

		sprintf(string, "〤 : 1�� 醱給 в蹂 ");
		ScreenPrint(45, 12, string);

		sprintf(string, "十 : 2�� 醱給 в蹂 ");
		ScreenPrint(45, 14, string);

		sprintf(string, "≧ : 奢 替葬啪 ");
		ScreenPrint(45, 18, string);

		sprintf(string, "≠ : 奢 緒腦啪 ");
		ScreenPrint(45, 20, string);

		sprintf(string, "﹢ : Bar 望檜 棺葬晦 ");
		ScreenPrint(45, 22, string);

		sprintf(string, "﹣ : Bar 望檜 還檜晦 ");
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
		sprintf(string, "識薄: %d", g_nTotalGrade);
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

				// bar 嬪纂 蟾晦��
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
					g_UpdateOldTime = clock();  // ready蒂 橾薑衛除 雖樓п 輿晦 嬪п 							
				}
				break;

			case 'k': //奢 嫦餌
				if (g_nGameState == RUNNING && g_Ball.getNReady() == 1)
				{
					g_Ball.setNReady(0);  // 檜翕 鼻鷓
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
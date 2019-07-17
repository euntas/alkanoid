#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include "Screen.h"

#pragma warning(disable:4996)

typedef enum _GAME_STATE { INIT, READY, RUNNING, SUCCESS, FAILED, STOP, RESULT } GAME_STATE ;
typedef enum _DIRECT { TOP, LEFT_TOP, LEFT_DOWN, DOWN, RIGHT_DOWN, RIGHT_TOP } DIRECT;

typedef struct _STAGE_INFO
{
		int      nBlockCount; // 벽돌 개수
		clock_t  LimitTime;  // 제한 시간
} STAGE_INFO;

typedef struct _BALL
{
	int		 nReady;     // 1 이면 준비 상태, 0이면 이동 상태
	int		 nHP;        // 생명
    int		 nX, nY;     // 좌표
	DIRECT   nDirect;    // 이동 방향
	clock_t  MoveTime;   // 이동 시간 간격
	clock_t  OldTime;	 // 이전 이동 시각
} BALL;

typedef struct _BLOCK
{
       int  nLife;
       int  nX, nY; 
} BLOCK;

typedef struct _BAR
{
      int nX[3];
      int nY;		
      clock_t OldTime;
      clock_t MoveTime;
} BAR;

BAR  g_sBar;

BALL g_sBall;
int g_StateTable[4][6] = { // 벽과 충돌하게 되면 방향 상태
	{  3,  2, -1, -1, -1,  4 },
	{ -1,  5,  4, -1, -1, -1 },
	{ -1, -1,  1,  0,  5, -1 },
	{ -1, -1, -1, -1,  2,  1 }
};

int g_BlockState[6] = { 3, 2, 1, 0, 5, 4 };
BLOCK g_sBlock[200];
GAME_STATE	g_nGameState = INIT;
clock_t		g_GameStartTime;
int         g_nIsSuccess = 0;
int			g_nStage = -1;
int			g_nGrade = 0; // 점수 
int			g_nStageCount = 1;
clock_t		g_GameStartTime, g_UpdateOldTime, g_RemainTime;
int			g_nTotalGrade = 0; // 총점
STAGE_INFO  g_StageInfo[3] = { { 30, 60*1000 }, { 50, 2*60*1000 }, { 100, 3*60*1000 } };

int			g_nBlockCount = 0;   // 맞춘 벽돌


void InitScreen() // 게임 소개 화면
{		      
	ScreenPrint(0,  0, "┏━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0,  1, "┃                                        ┃"); 
	ScreenPrint(0,  2, "┃    ■■■■■■■■■■■■■■■■    ┃"); 
	ScreenPrint(0,  3, "┃      ■■■■■■■■■■■■■■      ┃"); 
	ScreenPrint(0,  4, "┃        ■■■■■■■■■■■■        ┃"); 
	ScreenPrint(0,  5, "┃          ■■■■■■■■■■          ┃"); 
	ScreenPrint(0,  6, "┃          ┏━━━━━━━━┓          ┃"); 
	ScreenPrint(0,  7, "┃          ┃★ 벽돌 깨기  ★┃          ┃"); 
	ScreenPrint(0,  8, "┃          ┗━━━━━┳━━┛          ┃"); 
	ScreenPrint(0,  9, "┃              /)/)    ┃ ☆             ┃"); 
	ScreenPrint(0, 10, "┃           ★(*'')/                     ┃"); 
	ScreenPrint(0, 11, "┃          ■■■■■■■■■■          ┃"); 
	ScreenPrint(0, 12, "┃        ■■■■■■■■■■■■        ┃"); 
	ScreenPrint(0, 13, "┃      ■■■■■■■■■■■■■■      ┃"); 
	ScreenPrint(0, 14, "┃    ■■■■■■■■■■■■■■■■    ┃"); 
	ScreenPrint(0, 15, "┃                                        ┃"); 
	ScreenPrint(0, 16, "┃                                        ┃"); 
	ScreenPrint(0, 17, "┃    ★ 스페이스키를 눌러주세요!! ★     ┃"); 
	ScreenPrint(0, 18, "┃                                        ┃"); 
	ScreenPrint(0, 19, "┃     J:좌  L:우 K: 공발사 I: 공잡기     ┃"); 
	ScreenPrint(0, 20, "┃     ( 공 발사 방향 )                   ┃"); 
	ScreenPrint(0, 21, "┃       1: 왼족 2: 위쪽 3: 오른쪽        ┃"); 
	ScreenPrint(0, 22, "┃                                        ┃"); 
	ScreenPrint(0, 23, "┗━━━━━━━━━━━━━━━━━━━━┛");	
}

void ReadyScreen()
{		
	ScreenPrint(0,  0, "┏━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0,  1, "┃                                        ┃"); 
	ScreenPrint(0,  2, "┃                                        ┃"); 
	ScreenPrint(0,  3, "┃                                        ┃"); 
	ScreenPrint(0,  4, "┃                                        ┃"); 
	ScreenPrint(0,  5, "┃                                        ┃"); 
	ScreenPrint(0,  6, "┃                                        ┃"); 
	ScreenPrint(0,  7, "┃              S t a g e                 ┃"); 
	ScreenPrint(0,  8, "┃                            ●          ┃"); 
	ScreenPrint(0,  9, "┃                      ☜〓━┃          ┃"); 
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
	ScreenPrint(0, 23, "┗━━━━━━━━━━━━━━━━━━━━┛");		
}

void SuccessScreen()
{	
	ScreenPrint(0,  0, "┏━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0,  1, "┃      *.                                ┃"); 
	ScreenPrint(0,  2, "┃              .*            .*          ┃"); 
	ScreenPrint(0,  3, "┃     *.                        .*       ┃"); 
	ScreenPrint(0,  4, "┃       *.             .*                ┃"); 
	ScreenPrint(0,  5, "┃           *.                           ┃"); 
	ScreenPrint(0,  6, "┃            *.             .*           ┃"); 
	ScreenPrint(0,  7, "┃     *.      *.┏┓┏┓.*대    .*       ┃"); 
	ScreenPrint(0,  8, "┃             ┏┻┫┣┻┓단             ┃"); 
	ScreenPrint(0,  9, "┃     *.      ┃━┫┣━┃해      .*     ┃"); 
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
	ScreenPrint(0, 23, "┗━━━━━━━━━━━━━━━━━━━━┛");			
}

void FailureScreen()
{		
	ScreenPrint(0,  0, "┏━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0,  1, "┃  ■■■■■■■■■■■■■■■■■■  ┃"); 
	ScreenPrint(0,  2, "┃  ■■■■■■■■■■■■■■■■■■  ┃"); 
	ScreenPrint(0,  3, "┃                                        ┃"); 
	ScreenPrint(0,  4, "┃                                        ┃"); 
	ScreenPrint(0,  5, "┃                                        ┃"); 
	ScreenPrint(0,  6, "┃                                        ┃"); 
	ScreenPrint(0,  7, "┃             *┭┮  ┭┮*               ┃"); 
	ScreenPrint(0,  8, "┃          *.  ㅣㅣ▽ㅣㅣ   .*           ┃"); 
	ScreenPrint(0,  9, "┃   ┏ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ┓     ┃"); 
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
	ScreenPrint(0, 23, "┗━━━━━━━━━━━━━━━━━━━━┛");		
}

void RunningScreen()
{	
	ScreenPrint(0,  0, "┏━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0,  1, "┃                                        ┃"); 
	ScreenPrint(0,  2, "┃                                        ┃"); 
	ScreenPrint(0,  3, "┃                                        ┃"); 
	ScreenPrint(0,  4, "┃                                        ┃"); 
	ScreenPrint(0,  5, "┃                                        ┃"); 
	ScreenPrint(0,  6, "┃                                        ┃"); 
	ScreenPrint(0,  7, "┃                                        ┃"); 
	ScreenPrint(0,  8, "┃                                        ┃"); 
	ScreenPrint(0,  9, "┃                                        ┃"); 
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

void ResultScreen()
{
	ScreenPrint( 0, 0, "┏━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint( 0, 1, "┃                                        ┃");
	ScreenPrint( 0, 2, "┃                                        ┃");
	ScreenPrint( 0, 3, "┃                                        ┃");
	ScreenPrint( 0, 4, "┃                                        ┃");
	ScreenPrint( 0, 5, "┃                                        ┃");
	ScreenPrint( 0, 6, "┃                                        ┃");
	ScreenPrint( 0, 7, "┃   (((    벽돌 깨기  게임 Score  ))))   ┃");	
	ScreenPrint( 0, 8, "┃                                        ┃");
	ScreenPrint( 0, 9, "┃                                        ┃");
	ScreenPrint( 0,10, "┃                                        ┃");
	ScreenPrint( 0,11, "┃           총 점수  :                   ┃");
	ScreenPrint( 0,12, "┃                                        ┃");
	ScreenPrint( 0,13, "┃                                        ┃");
	ScreenPrint( 0,14, "┃                                        ┃");
	ScreenPrint( 0,15, "┃                                        ┃");
	ScreenPrint( 0,16, "┃                                        ┃");
	ScreenPrint( 0,17, "┃                                        ┃");	
	ScreenPrint( 0,18, "┃                                        ┃");
	ScreenPrint( 0,19, "┃                                        ┃");
	ScreenPrint( 0,20, "┃                                        ┃");
	ScreenPrint( 0,21, "┃                                        ┃");
	ScreenPrint( 0,22, "┃                                        ┃");
	ScreenPrint( 0,23, "┗━━━━━━━━━━━━━━━━━━━━┛");
}

int Search( int nEnd, int nX, int nY )
{
     int i;
     for( i = 0 ; i < nEnd ; i++ )
     {
		if( g_sBlock[i].nY == nY )
		{
		   if( g_sBlock[i].nX == nX || ( g_sBlock[i].nX + 1 ) == nX || 
			   g_sBlock[i].nX == nX + 1 || (g_sBlock[i].nX + 1) == nX + 1 )
			 	 return 1; // 같은 것이 있으면
		}
      }
      return 0; // 같은 것이 없으면 
}

void SetBlock( int nBlockCount )
{
     int nX, nY, i;

	 srand( (unsigned int )time(NULL) ); 
     for( i = 0 ; i < nBlockCount ; i++ )
     {
		g_sBlock[i].nLife = 1;
	
		while( 1 )
		{               
			 nX = rand() % 39 + 2; // 2 ~ 40 범위 안에서
			 nY = rand() % 10 + 1; // 1 ~ 10 까지의 범위
			
			 if( Search( i, nX, nY ) == 0 )
			 {
				 g_sBlock[i].nX = nX;
				 g_sBlock[i].nY = nY;
				 break;
			 }     
		 }
     }
}


int Collision( int nX, int nY )
{	
	int i, nCount = 0; // 충돌 개수

	// 공과 벽돌의 충돌
	for( i = 0 ; i < g_StageInfo[ g_nStage ].nBlockCount ; i++ )
	{
		if( g_sBlock[i].nLife == 1 )
		{
			if( g_sBlock[i].nY == nY )
			{
			   if( ( g_sBlock[i].nX == nX || ( g_sBlock[i].nX + 1 ) == nX ) ||
				   (g_sBlock[i].nX == (nX+1) || ( g_sBlock[i].nX + 1 ) == (nX+1) ) )
			  {
				   g_sBall.nDirect = g_BlockState[ g_sBall.nDirect ];
				   g_sBlock[i].nLife = 0;
				   g_nBlockCount++; 				   
				   nCount++;				  
			  }
			}			
		}
	}

	// 충돌 체크 
	if( nCount != 0 )
	{
        return 1;
	}

	// 공과 막대기 충돌
	for( i = 0 ; i < 3 ; i++ )
	{
		if( nY == g_sBar.nY )
		{
			if( ( nX >= g_sBar.nX[0] && nX <= ( g_sBar.nX[2] + 1 ) ) || ( ( nX + 1 ) >= g_sBar.nX[0] && ( nX + 1 ) <= ( g_sBar.nX[2] + 1 ) ) )
			{
				 g_sBall.nDirect = g_BlockState[ g_sBall.nDirect ];
				 return 1;
			}
		}
	}

	// Note: 위쪽  
    if( nY < 1 )
    {
		g_sBall.nDirect = g_StateTable[0][g_sBall.nDirect];		
		return 1 ;
     }

     // Note: 오른쪽
     if( nX > 39 )
     {
		g_sBall.nDirect = g_StateTable[1][g_sBall.nDirect];		
		return 1 ;
     }

     // Note: 공의 초기화 및 생명 감소
     if( nY > 22 )
     {
		g_sBall.nHP--;

		if( g_sBall.nHP == 0 )
		{	// 게임 판정으로 
			g_nGameState = STOP;			
		}else{
			g_sBall.nX = g_sBar.nX[1];
			g_sBall.nY = g_sBar.nY - 1;
			g_sBall.nReady = 1;
		}
				
		return 1 ;
     }

      if( nX < 2 )
      {
		g_sBall.nDirect = g_StateTable[3][g_sBall.nDirect];	
		return 1 ;
      }

     return 0;
}

void Init()
{	
	if(  g_nStage == -1 ) // 1번만 초기화가 되는 부분에 대한 처리 사항
	{
		g_nStage = 0;
	}

	g_nGrade = 0;
	SetBlock( g_StageInfo[g_nStage].nBlockCount );

	// Note: 공 초기화
	g_sBall.nX = 19;
	g_sBall.nHP = 3;
	g_sBall.nY = 20;
	g_sBall.nDirect = TOP;
	g_sBall.OldTime = clock();
	g_sBall.nReady = 1;
	g_sBall.MoveTime = 50;

	// Note: 막대기 초기화
	g_sBar.nY = g_sBall.nY + 1;
    g_sBar.nX[0] = g_sBall.nX - 2;
    g_sBar.nX[1] = g_sBall.nX;
    g_sBar.nX[2] = g_sBall.nX + 2;
    g_sBar.MoveTime = 10;
    g_sBar.OldTime= clock();

	g_nBlockCount = 0;
}

void Update()
{	 
	 clock_t CurTime = clock();

	  switch( g_nGameState )
	 {
	 case READY :
				if( CurTime - g_UpdateOldTime > 3000 )  // 2초
				{					
					g_nGameState = RUNNING;
					g_GameStartTime = CurTime;	
				}
				break;

	case RUNNING :
				g_RemainTime = ( g_StageInfo[g_nStage].LimitTime - ( CurTime - g_GameStartTime) ) / 1000 ; // 게임 진행 남은 시간

				if( (CurTime - g_GameStartTime) > g_StageInfo[g_nStage].LimitTime || g_nBlockCount == g_StageInfo[ g_nStage ].nBlockCount )
				{
					g_nGameState = STOP;	
					return ;
				}else if( g_sBall.nReady == 0 ){
				
					if( CurTime - g_sBall.OldTime > g_sBall.MoveTime )
					{
						g_sBall.OldTime = CurTime;
					
						switch( g_sBall.nDirect )
						{
						  case TOP :
										  if( Collision( g_sBall.nX, g_sBall.nY - 1 ) == 0 )
											  g_sBall.nY--;							 
										  break;
						  case LEFT_TOP :
										  if( Collision( g_sBall.nX + 1, g_sBall.nY - 1 ) == 0 )
										  {
											  g_sBall.nX++;
											  g_sBall.nY--;
										  }
										  break;
						  case LEFT_DOWN :
										  if( Collision( g_sBall.nX + 1, g_sBall.nY + 1 ) == 0 )
										  {
											  g_sBall.nX++;
											  g_sBall.nY++;
										  }
										  break;
						  case DOWN :	
										 if( Collision( g_sBall.nX, g_sBall.nY + 1 ) == 0 )
											  g_sBall.nY++;
										 break;
						  case RIGHT_DOWN :
										 if( Collision( g_sBall.nX - 1, g_sBall.nY + 1 ) == 0 )
										 {
											 g_sBall.nX--;
											 g_sBall.nY++;
										 }
										 break;
						  case RIGHT_TOP :
										 if( Collision( g_sBall.nX - 1, g_sBall.nY - 1 ) == 0 )
										 {
											 g_sBall.nX--;
											 g_sBall.nY--;
										 }
							 break;
						}
				   }	
				}				
				break;
	case STOP :
				// 성공이냐 실패를 판단해주어서 출력을 해주는 부분이 와야 한다.  
				if( g_nBlockCount == g_StageInfo[g_nStage].nBlockCount && g_sBall.nHP > 0  && g_RemainTime > 0)
				{
					g_UpdateOldTime = CurTime;
					g_nGameState = SUCCESS;		
					g_nTotalGrade += g_nBlockCount * 10;
				}else{
					g_nGameState = FAILED;
				}				
				break;
	case SUCCESS :				
				if( CurTime - g_UpdateOldTime > 3000 )
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
	int i;
	char string[100];

	ScreenClear();

	switch( g_nGameState )
	{
	case INIT :		
				if( g_nStage == 0 )
					InitScreen();	
				break;

	case READY :
				ReadyScreen();	
				sprintf( string, "%d", g_nStage + 1);
				ScreenPrint( 13, 7, string);
				break;
	case RUNNING:
		        RunningScreen();

				sprintf( string, "스테이지 : %d", g_nStage + 1 );
				ScreenPrint( 45, 2, string );	

				sprintf( string, "생명 : %d ", g_sBall.nHP );
				ScreenPrint( 45, 4, string );	

				sprintf( string, "점수 : %d ", g_nGrade );
				ScreenPrint( 45, 6, string );	

				sprintf( string, "남은 시간 : %d 분 %d 초", g_RemainTime/60, g_RemainTime%60 );
				ScreenPrint( 45, 8, string );	


				for( i = 0 ; i < g_StageInfo[ g_nStage ].nBlockCount ; i++ )
				{
					if( g_sBlock[i].nLife == 1 )
						ScreenPrint( g_sBlock[i].nX, g_sBlock[i].nY, "■" );
				}

				for( i = 0 ; i < 3 ; i++ )
				{
					ScreenPrint( g_sBar.nX[i], g_sBar.nY, "▣" );
				}

				ScreenPrint( g_sBall.nX, g_sBall.nY, "●");			
				break;
	case SUCCESS :				
				SuccessScreen();	
				sprintf( string, "%d Stage", g_nStage + 1 );
				ScreenPrint( 19, 12, string );
				sprintf( string, "총점: %d", g_nTotalGrade );
				ScreenPrint( 25, 16, string );				
				break;
	case FAILED :				
				FailureScreen();			
				sprintf( string, "%d Stage!", g_nStage + 1 );
				ScreenPrint( 16, 4, string );				
				break;

	case RESULT :
				ResultScreen();		
				sprintf( string, "%d", g_nTotalGrade );
				ScreenPrint( 25, 11, string );
				break;
	}

	ScreenFlipping();
}

void Release()
{
}


int main(void)
{
	int nKey, i, nX, nY;
	clock_t CurTime;

	ScreenInit();
	Init();

	while( 1 )
	{
		if( _kbhit() )
		{
			if( g_nGameState == RESULT )
				break ;

			nKey = _getch();

			switch( nKey )
			{
			  case ' ' :
						if( g_nGameState == INIT && g_nStage == 0 )
						{							
							g_nGameState = READY;
							g_UpdateOldTime = clock();  // ready를 일정시간 지속해 주기 위해 							
						}
						break;
 
			   case 'k' : //공 발사
						if( g_nGameState == RUNNING && g_sBall.nReady == 1 )
						{
							g_sBall.nReady = 0;  // 이동 상태
							g_sBall.OldTime = clock();
						}
						break;

			   case 'j' :
						if( g_nGameState == RUNNING )
						{

							 CurTime = clock();
							 if( CurTime - g_sBar.OldTime > g_sBar.MoveTime )
							 {
								 g_sBar.OldTime = CurTime;
								 if( g_sBar.nX[0] > 2 ) // 경계 영역 충돌 체크
								 {
									 g_sBar.nX[0]--;
									 g_sBar.nX[1]--;
									 g_sBar.nX[2]--;		

									if( g_sBall.nReady == 1 )
										g_sBall.nX--;
								 }
							 }
						}
						 break;
			   case 'l' : 
						if( g_nGameState == RUNNING )
						{
							CurTime = clock();
							if( CurTime - g_sBar.OldTime > g_sBar.MoveTime )
							{					
								g_sBar.OldTime = CurTime;
								if( g_sBar.nX[2] < 40 )  // 경계 영역 체크
								{
									g_sBar.nX[0]++;
									g_sBar.nX[1]++;
									g_sBar.nX[2]++;		

									if( g_sBall.nReady == 1 )
										g_sBall.nX++;														
								}
							}
						}
						break;

			   case 'i' : // 공 잡기
						 //막대기와 공 충돌
						if( g_nGameState == RUNNING && g_sBall.nReady == 0 )
						{
							nX = g_sBall.nX;
							nY = g_sBall.nY;

							for( i = 0 ; i < 3 ; i++ )
							{
								if( nY == (g_sBar.nY - 1) ) // 공과 막대기가 접한 경우
								{
									if( ( nX >= g_sBar.nX[0] && nX <= ( g_sBar.nX[2] + 1 ) ) || ( ( nX + 1 ) >= g_sBar.nX[0] && ( nX + 1 ) <= ( g_sBar.nX[2] + 1 ) ) )
									{
										g_sBall.nReady = 1;		
										break;				
									}
								}
							}
						}
						break;
			                    
			   case '1' : // 공을 잡았을 때 1, 2, 3 방향 선택가능	
						   if(  g_nGameState == RUNNING && g_sBall.nReady == 1 )
						   {							  
							   g_sBall.nDirect = RIGHT_TOP;
							   g_sBall.OldTime = clock();	
						   }
						   break;

			   case '2' :
						   if(  g_nGameState == RUNNING && g_sBall.nReady == 1 ) 
						   {
							   g_sBall.nDirect = TOP;
							   g_sBall.OldTime = clock();	
						   }
						   break;
						   
			   case '3' :		
						   if(  g_nGameState == RUNNING && g_sBall.nReady == 1 )
						   {						  
							   g_sBall.nDirect = LEFT_TOP;
							   g_sBall.OldTime = clock();
						   }
						   break;
			   case 'y' :
			   case 'Y' :
							if( g_nGameState == FAILED ) 
							{													
								Init();
								g_nGameState = READY;
								g_UpdateOldTime = clock();
 							}						
							break;
			   case 'n' :
			   case 'N' :
							if( g_nGameState == FAILED )
							{							
								g_nGameState = RESULT;												
							}
							break;				
			}
		}

		Update();
		Render();
	}

	Release();
	ScreenRelease();	
	return 0;
}
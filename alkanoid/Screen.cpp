#include <windows.h>

static int g_nScreenIndex;
static HANDLE g_hScreen[2];

void ScreenInit()
{
	system("mode con: cols=120 lines=30");

	CONSOLE_CURSOR_INFO cci;
	
	// 가상의 콘솔창 2개룰 만든다.
	g_hScreen[0] = CreateConsoleScreenBuffer( GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );
	g_hScreen[1] = CreateConsoleScreenBuffer( GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

	// 커서 숨기기
	cci.dwSize = 1;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo( g_hScreen[0], &cci );
	SetConsoleCursorInfo( g_hScreen[1], &cci );
}

void ScreenFlipping()
{		
	Sleep( 10 );
	SetConsoleActiveScreenBuffer( g_hScreen[g_nScreenIndex] );	
	g_nScreenIndex = !g_nScreenIndex;	
}

void ScreenClear()
{		
	COORD Coor = { 0, 0 };
	DWORD dw;
	FillConsoleOutputCharacter( g_hScreen[g_nScreenIndex], ' ', 80*25, Coor, &dw );
}

void ScreenRelease()
{
	CloseHandle( g_hScreen[0] );
	CloseHandle( g_hScreen[1] );
}


void ScreenPrint( int x, int y, const char *string )
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	CursorPosition.X = x * 2;
	//SetConsoleCursorPosition( g_hScreen[g_nScreenIndex], CursorPosition );	
	//WriteFile( g_hScreen[g_nScreenIndex], string, strlen( string ), &dw, NULL );

	WriteConsoleOutputCharacter(g_hScreen[g_nScreenIndex], string, strlen(string), CursorPosition, &dw);
}

void ScreenColorPrint(int x, int y, const char* string, WORD attr)
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	CursorPosition.X = x * 2;
	//SetConsoleCursorPosition( g_hScreen[g_nScreenIndex], CursorPosition );	
	//WriteFile( g_hScreen[g_nScreenIndex], string, strlen( string ), &dw, NULL );

	WriteConsoleOutputAttribute(g_hScreen[g_nScreenIndex], &attr, strlen(string), CursorPosition, &dw);
	WriteConsoleOutputCharacterA(g_hScreen[g_nScreenIndex], string, strlen(string), CursorPosition, &dw);
}

// 1 ~ 15 까지 색상 설정 가능
void SetColor(int x, int y, unsigned short color, const char *string )
{		
	//SetConsoleTextAttribute( g_hScreen[g_nScreenIndex], color );
	DWORD dw;
	COORD CursorPosition = { x, y };
	CursorPosition.X = x * 2;
	
}
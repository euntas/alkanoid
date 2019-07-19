void ScreenInit();
void ScreenFlipping();
void ScreenClear();
void ScreenRelease();
void ScreenPrint( int x, int y, const char *string );
void ScreenColorPrint(int x, int y, const char* string, WORD attr);
void SetColor( unsigned short color );
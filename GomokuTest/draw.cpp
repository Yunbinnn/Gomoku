#include "draw.h"

cDraw* cDraw::getInstance()
{
	static cDraw draw;
	return &draw;
}

cDraw::cDraw()
{
	nX = (40 - SIZE - 1) / 2;
	nY = 11 - SIZE / 2;
}

// 화면의 커서를 보이게 하거나 숨겨놓기
void cDraw::CursorView(char show)
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

// x, y 좌표로 커서를 움직이는 함수
void cDraw::gotoxy(int x, int y)
{
	COORD XY = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}

// 콘솔 색상을 지정함
void cDraw::SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 콘솔 바탕과 글자 색상을 지정
void cDraw::SetColor(int color, int bgcolor)
{
	color &= 0xf;
	bgcolor &= 0xf;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bgcolor << 4) | color);
}

// 상황별 메시지 출력 함수
void cDraw::errMsg(int msg)
{
	msg -= 10;
	const char* str[] = {
		"이미 돌이 놓여 있습니다.\n다른 곳에 착수하세요.",
		"삼삼 입니다.\n다른 곳에 착수하세요.",
		"사사 입니다.\n다른 곳에 착수하세요.",
		"흑은 육목을 둘 수 없습니다.\n다른 곳에 착수하세요.",
		"한 수만 물릴 수 있습니다"
	};
	MessageBox(NULL, str[msg], "Warning!", MB_OK);
}

void cDraw::showMsg(int msg)
{
	const char* str[] =
	{
		"흑이 둘 차례입니다.",
		"백이 둘 차례입니다.",
		"       ",
		"흑이 승리했습니다! ",
		"백이 승리했습니다! ",
	};

	SetColor(GRAY);
	gotoxy((nX + SIZE + 1) * 2, nY);
	printf("%s", str[msg]);
}

// 승패결정 이후 게임을
// 다시 할 것인지 물어봄
bool cDraw::endMsg(int stone)
{
	const char* winner[] =
	{
		"    흑이 승리했습니다!\n    한번 더 하시겠습니까?    ",
		"    백이 승리했습니다!\n    한번 더 하시겠습니까?    "
	};
	
	showMsg(stone + 3);
	if (MessageBox(NULL, winner[stone], "알 림", MB_YESNO) == IDYES) return true;
	else return false;
}

// 바둑판과 돌을 콘솔 창에 그림
void cDraw::printData(int& x, int &y, int type)
{
	if (x < 1) x = 1;
	else if (y < 1) y = 1;
	else if (x > SIZE) x = SIZE;
	else if (y > SIZE) y = SIZE;

	const int color[] = { BLACK, WHITE, BLACK, BLACK, BLACK, BLACK };
	const char* shape[] = { "┌", "┬", "┐", "├", "┼", "┤", "└", "┴", "┘", "●", "●", "□" };

	SetColor(color[type], DARK_YELLOW);
}

int cDraw::getLineNumber(int x, int y)
{
	// 왼쪽 세로줄
	if (x == 1)
	{
		// x, y가 전부 0이라면 왼쪽 상단 모서리
		if (y == 1) return 0;
		// 왼쪽 하단 모서리
		else if (y == SIZE) return 0;
		// 왼쪽 세로줄
		else return 3;
	}																											
	// 오른쪽 세로줄
	else if (x == SIZE)
	{
		// 오른쪽 상단 모서리
		if (y == 1) return 2;
		// 오른쪽 하단 모서리
		else if (y == SIZE) return 8;
		// 오른쪽 세로줄
		else return 5;
	}
	// 윗쪽 가로줄
	else if (y == 1) return 1;
	// 아래쪽 가로줄
	else if (y == SIZE) return 7;
	// 마지막 가운데 십자 모양
	else return 4;
}
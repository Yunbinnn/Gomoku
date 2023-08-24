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

// ȭ���� Ŀ���� ���̰� �ϰų� ���ܳ���
void cDraw::CursorView(char show)
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

// x, y ��ǥ�� Ŀ���� �����̴� �Լ�
void cDraw::gotoxy(int x, int y)
{
	COORD XY = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}

// �ܼ� ������ ������
void cDraw::SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// �ܼ� ������ ���� ������ ����
void cDraw::SetColor(int color, int bgcolor)
{
	color &= 0xf;
	bgcolor &= 0xf;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bgcolor << 4) | color);
}

// ��Ȳ�� �޽��� ��� �Լ�
void cDraw::errMsg(int msg)
{
	msg -= 10;
	const char* str[] = {
		"�̹� ���� ���� �ֽ��ϴ�.\n�ٸ� ���� �����ϼ���.",
		"��� �Դϴ�.\n�ٸ� ���� �����ϼ���.",
		"��� �Դϴ�.\n�ٸ� ���� �����ϼ���.",
		"���� ������ �� �� �����ϴ�.\n�ٸ� ���� �����ϼ���.",
		"�� ���� ���� �� �ֽ��ϴ�"
	};
	MessageBox(NULL, str[msg], "Warning!", MB_OK);
}

void cDraw::showMsg(int msg)
{
	const char* str[] =
	{
		"���� �� �����Դϴ�.",
		"���� �� �����Դϴ�.",
		"       ",
		"���� �¸��߽��ϴ�! ",
		"���� �¸��߽��ϴ�! ",
	};

	SetColor(GRAY);
	gotoxy((nX + SIZE + 1) * 2, nY);
	printf("%s", str[msg]);
}

// ���а��� ���� ������
// �ٽ� �� ������ ���
bool cDraw::endMsg(int stone)
{
	const char* winner[] =
	{
		"    ���� �¸��߽��ϴ�!\n    �ѹ� �� �Ͻðڽ��ϱ�?    ",
		"    ���� �¸��߽��ϴ�!\n    �ѹ� �� �Ͻðڽ��ϱ�?    "
	};
	
	showMsg(stone + 3);
	if (MessageBox(NULL, winner[stone], "�� ��", MB_YESNO) == IDYES) return true;
	else return false;
}

// �ٵ��ǰ� ���� �ܼ� â�� �׸�
void cDraw::printData(int& x, int &y, int type)
{
	if (x < 1) x = 1;
	else if (y < 1) y = 1;
	else if (x > SIZE) x = SIZE;
	else if (y > SIZE) y = SIZE;

	const int color[] = { BLACK, WHITE, BLACK, BLACK, BLACK, BLACK };
	const char* shape[] = { "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" };

	SetColor(color[type], DARK_YELLOW);
}

int cDraw::getLineNumber(int x, int y)
{
	// ���� ������
	if (x == 1)
	{
		// x, y�� ���� 0�̶�� ���� ��� �𼭸�
		if (y == 1) return 0;
		// ���� �ϴ� �𼭸�
		else if (y == SIZE) return 0;
		// ���� ������
		else return 3;
	}																											
	// ������ ������
	else if (x == SIZE)
	{
		// ������ ��� �𼭸�
		if (y == 1) return 2;
		// ������ �ϴ� �𼭸�
		else if (y == SIZE) return 8;
		// ������ ������
		else return 5;
	}
	// ���� ������
	else if (y == 1) return 1;
	// �Ʒ��� ������
	else if (y == SIZE) return 7;
	// ������ ��� ���� ���
	else return 4;
}
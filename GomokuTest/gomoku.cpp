#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#pragma region Ű�Է� ��ó�� ����

#define BACK_SPACE 8
#define	ENTER      13
#define	ESC	       27
#define UP         72
#define DOWN       80
#define LEFT       75
#define RIGHT      77
#define PAGE_UP    73
#define PAGE_DOWN  81
#define DEL        83

#pragma endregion

// ���� �����
enum {
	BLACK,			/*  0 : ���� */
	DARK_BLUE,		/*  1 : ��ο� �Ķ� */
	DARK_GREEN,		/*  2 : ��ο� �ʷ� */
	DARK_SKY_BLUE,	/*  3 : ��ο� �ϴ� */
	DARK_RED,		/*  4 : ��ο� ���� */
	DARK_VIOLET,	/*  5 : ��ο� ���� */
	DARK_YELLOW,	/*  6 : ��ο� ��� */
	GRAY,			/*  7 : ȸ�� */
	DARK_GRAY,		/*  8 : ��ο� ȸ�� */
	BLUE,			/*  9 : �Ķ� */
	GREEN,			/* 10 : �ʷ� */
	SKY_BLUE,		/* 11 : �ϴ� */
	RED,			/* 12 : ���� */
	VIOLET,			/* 13 : ���� */
	YELLOW,			/* 14 : ��� */
	WHITE,			/* 15 : �Ͼ� */
};

// �������� ũ��
#define SIZE   15
#define CENTER ((SIZE + 2) / 2)
#define FINISH -1
#define TIED   2

// �ٵ����� ����, �ٵϵ��� ����, ������ ���� ���
enum { BLACK_STONE, WHITE_STONE, CURSOR, BLACKWIN, WHITEWIN, TIE, LINE };
enum { OCCUPIED = 10, DOUBLETHREE, DOUBLEFOUR, SIXMOK, NOTUNDO, FIVEMOK, CHANGE, PASS };
enum { HIDE, SHOW };

/*-----------------------------------------ȭ���� �׷��� Draw class------------------------------------------*/
class cDraw
{
private:
	cDraw();
	~cDraw() { ; }
	// ���Ӻ����� ù ���� ���� ��ġ
	int nX, nY;

public:
	static cDraw* getInstance();
	int getLineNumber(int x, int y);
	void printData(int& x, int& y, int type);
	void printNum();
	void drawTime(time_t sec);
	void showMsg(int msg);
	void errMsg(int msg);
	bool endMsg(int stone);
	void CursorView(char show);
	void gotoxy(int x, int y);
	void SetColor(int color);
	void SetColor(int color, int bgcolor);
	void infoKey();
};

// singleton ����� ���� ��ũ��
#define pDraw cDraw::getInstance

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
	CONSOLE_CURSOR_INFO ConsoleCursor{};

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

// x, y ��ǥ�� Ŀ���� �����̴� �Լ�
void cDraw::gotoxy(int x, int y)
{
	COORD XY = { (short)x,(short)y };
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
	const char* str[] =
	{
		"�̹� ���� ���� �ֽ��ϴ�.\n�ٸ� ���� �����ϼ���.",
		"�ֻ� �Դϴ�.\n�ٸ� ���� �����ϼ���.",
		"�ֻ� �Դϴ�.\n�ٸ� ���� �����ϼ���.",
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
		"���º��Դϴ�.     ",
	};

	SetColor(GRAY);
	gotoxy((nX + SIZE + 1) * 2, nY);
	printf("%s", str[msg]);
}

// �Է� ������ Ű�� ���� UI�� ���
void cDraw::infoKey()
{
	const char* str[] =
	{
		"�����̽��� : ��  ��",
		"��  ��  Ű : ��  ��",
		"    U      : ������",
		"Delete Key : PASS      ",
		" ",
		"��� ���� ��������",
		"PASS�ϸ� ���º�"
	};

	SetColor(GRAY);

	for (int i = 0; i < 7; i++)
	{
		gotoxy((nX + SIZE + 1) * 2, nY + 3 + i);
		printf(str[i]);
	}
}

// ���а��� ���� ������
// �ٽ� �� ������ ���
bool cDraw::endMsg(int stone)
{
	const char* winner[] =
	{
		"    ���� �¸��߽��ϴ�!\n    �ѹ� �� �Ͻðڽ��ϱ�?    ",
		"    ���� �¸��߽��ϴ�!\n    �ѹ� �� �Ͻðڽ��ϱ�?    ",
		"    ���º��Դϴ�.\n    �ѹ� �� �Ͻðڽ��ϱ�?    "
	};

	showMsg(stone + 3);
	if (MessageBox(NULL, winner[stone], "�� ��", MB_YESNO) == IDYES) return true;
	else return false;
}

// �ٵ��ǰ� ���� �ܼ� â�� �׸�
void cDraw::printData(int& x, int& y, int type)
{
	if (x < 1) x = 1;
	else if (y < 1) y = 1;
	else if (x > SIZE) x = SIZE;
	else if (y > SIZE) y = SIZE;

	const int color[] = { BLACK, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK };
	const char* shape[] = { "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" };

	SetColor(color[type], DARK_YELLOW);

	// �� ��ǥ���� �� ���� ������ �ٸ��� ������
	// ��� �׷��� ���� ��ǥ�� �ְ� ����� �����´�.
	if (type == LINE) type = getLineNumber(x, y);
	// ���� 0-8���� �̰�, �� �ڿ� ���� ��ġ�Ǿ� ������ 9�� �����ش�.
	else type += 9;
	gotoxy((x + nX - 1) * 2, y + nY - 1);
	printf("%s", shape[type]);
	SetColor(GRAY);
}

// ���ο� �������� ��ǥ�� ���ڿ� ���ĺ����� ǥ���Ѵ�.
void cDraw::printNum()
{
	SetColor(BLACK, DARK_YELLOW);

	// x(����)���� ���
	for (int i = 0; i < SIZE; i++)
	{
		gotoxy((nX + i) * 2, nY + SIZE);
		printf("%2c", i + 'A');
	}

	// y(����)���� ���
	for (int i = 0, j = SIZE; i < SIZE; i++, j--)
	{
		gotoxy((nX - 1) * 2, nY + j - 1);
		printf("%2d", i + 1);
	}

	gotoxy((nX - 1) * 2, nY + SIZE);
	printf("  ");
	SetColor(GRAY);
}

int cDraw::getLineNumber(int x, int y)
{
	// ���� ������
	if (x == 1)
	{
		// x, y�� ���� 0�̶�� ���� ��� �𼭸�
		if (y == 1) return 0;
		// ���� �ϴ� �𼭸�
		else if (y == SIZE) return 6;
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

// ���� �ð� ȭ��ǥ��
void cDraw::drawTime(time_t sec)
{
	int hh, mm;

	hh = static_cast<unsigned int>(sec) / 3600;
	mm = (static_cast<unsigned int>(sec) % 3600) / 60;
	sec = sec %= 60;

	SetColor(SKY_BLUE);
	gotoxy(nX * 2 - 2, nY + SIZE + 1);
	printf("%02d : %02d : %02d", hh, mm, static_cast<unsigned int>(sec));
	SetColor(GRAY);
}

/*-----------------------------------------------���ַ� Class-----------------------------------------------*/
class cRenjuRule
{
private:
	int nBoard[SIZE + 2][SIZE + 2];

public:
	cRenjuRule();
	virtual ~cRenjuRule() { ; }

	void SetBoard(int(*arr)[SIZE + 2]);
	bool IsFive(int x, int y, int nStone);
	bool IsSix(int x, int y, int nStone);
	bool IsFour(int x, int y, int nStone, int nDir);
	int IsOpenFour(int x, int y, int nStone, int nDir);
	bool IsOpenThree(int x, int y, int nStone, int nDir);
	bool IsDoubleThree(int x, int y, int nStone);
	bool IsDoubleFour(int x, int y, int nStone);

private:
	void SetStone(int x, int y, int nStone);
	void GetDirTable(int& x, int& y, int nDir);
	bool IsFive(int x, int y, int nStone, int nDir);
	bool IsEmpty(int& x, int& y, int nStone, int i);
	bool IsForbidden(int x, int y, int nStone);
	int GetStoneCount(int x, int y, int nStone, int nDir);
};

cRenjuRule::cRenjuRule()
{
	for (int i = 0; i < SIZE + 2; i++)
	{
		// �ٵ��� ���� �迭�� ���� �ٸ� ���ڷ�
		// ��Ÿ�����ν� ��踦 ǥ���Ѵ�.
		nBoard[i][0] = nBoard[i][SIZE + 1] = -1;
		nBoard[0][i] = nBoard[SIZE + 1][i] = -1;
	}
}

// �����ϱ� ���� �ٵ��� ���¸� �̸� �����ص�
void cRenjuRule::SetBoard(int(*arr)[SIZE + 2])
{
	for (int i = 1; i <= SIZE; i++)
		for (int j = 1; j <= SIZE; j++)
			nBoard[i][j] = arr[i][j];
}

// x, y ��ǥ�� ���� ���ų� �����(���� �� �� �ڸ��� �� �ٽ� ����)
void cRenjuRule::SetStone(int x, int y, int nStone)
{
	nBoard[y][x] = nStone;
}

// 8���� ���ڴ� �� 8���� ������ ��Ÿ����.
// �迭�� ������� ��, ��, ��, ��, �ϼ�, ����, ����, �ϵ�
// 2�迭�� �ѽ��� �Ǿ� ���� ���� �밢���� ������ ǥ���Ѵ�.
void cRenjuRule::GetDirTable(int& x, int& y, int nDir)
{
	int dx[] = { -1,  1,  0,  0, -1,   1,  -1,   1 };
	int dy[] = { 0,  0, -1,  1, -1,   1,   1,  -1 };

	x = dx[nDir];
	y = dy[nDir];
}

// �־��� �������� ��ǥ�� �Ű� �� ���� ã��
// �װ��� ��ǥ�� �˷��ִ� ����
bool cRenjuRule::IsEmpty(int& x, int& y, int nStone, int nDir)
{
	int dx, dy;

	// ���� Table���� dx, dy ���� �����´�.
	// ���� nDir�� 0�̶�� dx = -1, dy = 0�̴�.
	GetDirTable(dx, dy, nDir);

	// x�� 1 y�� 0�̴ϱ� x+= dx, y+=dy �� ���Ŀ� ���ؼ�
	// x�� ���� �۾��� �������� ����, y�� ������ ������ ���ڸ��� �ӹ���.
	// �׷��� ���η� ���ʿ� nStone�� �ƴ� ������ ���߰� �ȴ�.
	for (; nBoard[y][x] == nStone; x += dx, y += dy);

	// nStone�� �ƴ϶� �ؼ� ���� LINE�̶� �� �� ����.
	// �ٸ� ���̰ų� �ٵ����� ��� �� �ϼ��� ������
	// �װ��� LINE�� ���� ���� ��ȯ�Ѵ�.
	if (nBoard[y][x] == LINE) return true;
	else return false;
}

int cRenjuRule::GetStoneCount(int x, int y, int nStone, int nDir)
{
	int dx, dy;
	int tx = x;
	int ty = y;
	int cnt = 0;

	// ������ ���� LINE�� �ƴ϶�� ������ �� ����.
	// 0(����)�� ��ȯ
	if (nBoard[y][x] != LINE) return 0;

	SetStone(x, y, nStone);

	// ���� Table���� dx, dy ���� ������
	GetDirTable(dx, dy, nDir);
	// ������ ������ ���� nDir�������� ���� ���� ������ ����.
	for (; nBoard[y][x] == nStone; x += dx, y += dy) cnt++;

	// ���� Table�� �ϳ��� �Ǿ��ְ�,
	// ������ �׻� (��, ��) �Ǵ� (��, ��)���� �Ǿ� �־�
	// nDir�� Ȧ���� ���� -1��ŭ �� ������ �˻��Ͽ��� �Ѵ�.
	// �׷��� �ʰ� ���� �����̶�� ������ +1�� �ع�����
	// ���� nDir�� ����(1)�̶�� ����(0)�� �˻��ؾ� �Ǵµ�
	// +1�ؼ� 2���Ǹ� ����(0)�� �ƴ� ����(2)�� �˻��ϰ� �ȴ�.
	// �̸� �����ϱ� ���� 3�� �����ڸ� �̿���
	// ���ǿ� �´� ������ �μ��� �ѱ�� �Ͽ���. 
	GetDirTable(dx, dy, nDir % 2 ? nDir - 1 : nDir + 1);

	// x, y�� ���� ���� ����鿡 ���� ù ��ǥ�� ����� ������
	// ù ��ǥ ���� ���� �������� ��ĭ �Űܼ� �װ���������
	// ���� ������ ���� �����Ѵ�.
	// ������ ���� ���Ҵ� ���� ������ ������ �ߺ�üũ�� �����ϱ� ���ؼ��̴�.
	x = tx + dx;
	y = ty + dy;
	for (; nBoard[y][x] == nStone; x += dx, y += dy)cnt++;

	// ���� ������ �� �����ٸ� ���� ����.
	SetStone(tx, ty, LINE);
	return cnt;
}

// ���� �Ͽ��� �� �������� üũ�Ѵ�.
bool cRenjuRule::IsFive(int x, int y, int nStone)
{
	// ���� Table�� ���� 8�̴ϱ� 8���� �˻��ϰ�
	// �������� +=2 �ΰ�
	// �Ʒ� GetStoneCount()�Լ� �ȿ��� �糡�� �˻��ϱ� ����
	for (int i = 0; i < 8; i += 2)
	{
		if (GetStoneCount(x, y, nStone, i) == 5) return true;
	}

	return false;
}

// �� �Լ��� ���� ���������� �˻�������,
// nDir���⿡ ���ؼ��� �˻��Ѵ�.
// �̴� Four�� OpenFour�� �Ǵ��� �˾ƺ��� ���ؼ�
// �� ���� ���� �ϳ��� ���ƺ��鼭
// ������ �Ǵ��� �˻��ϱ� ���ؼ��̴�.
bool cRenjuRule::IsFive(int x, int y, int nStone, int nDir)
{
	if (GetStoneCount(x, y, nStone, nDir) == 5) return true;

	return false;
}

// �������� �˻��ϴ� ��ó�� ���� �̻����� �˻��Ѵ�.
bool cRenjuRule::IsSix(int x, int y, int nStone)
{
	for (int i = 0; i < 8; i += 2)
	{
		if (GetStoneCount(x, y, nStone, i) >= 6) return true;
	}

	return false;
}

// ���� ���� �� 4���� �˻��Ѵ�.
bool cRenjuRule::IsFour(int x, int y, int nStone, int nDir)
{
	// ������ ��ǥ x, y�� ����ϱ� ���� ����
	int tx, ty;

	// �� ���⿡ ���Ͽ� �糡�� �˻��ϴµ�,
	// �� ���� ��ǥ �� ù��°���� �˻��ϱ� �����̴�.
	nDir % 2 ? nDir -= 1 : nDir;

	// �ش� ��ǥ�� �����Ѵ�.
	SetStone(x, y, nStone);

	// �������� �糡�� �˻�
	for (int i = 0; i < 2; i++)
	{
		// ������ ��ǥ�� �߽����� �־��� ����(nDir)�� ���Ͽ�
		// �������� �̵��ϸ鼭 ����� ã�� �װ��� ��ǥ�� ���;� �Ǳ⿡
		// ������ ��ǥ�� ������ �ȴ�. ���� ������ �˻��ϱ� ���ؼ�
		// ����� ������ �ʿ��ϰ�, �Ź� �Ʒ��� ����
		// ������ ��ǥ�� �޾Ƽ� �˻簡 ����Ǿ�� �Ѵ�.
		tx = x;
		ty = y;

		// �� ���� �ִٸ� �� ��ǥ�� �޾ƿ´�.
		if (IsEmpty(tx, ty, nStone, nDir + i))
		{
			if (IsFive(tx, ty, nStone, nDir + i))
			{
				// ������ �Ǿ��ٸ� 4�̱� ������
				// ���� ���� ���� �����Ѵ�.
				SetStone(x, y, LINE);
				return true;
			}
		}
	}

	// �˻縦 �غ����� 4�� �ƴ϶�� �ص� ���� ����.
	SetStone(x, y, LINE);
	return false;
}

// ���� �������� 4���̰� ���� ���� ������ �� ������ �Ǹ�
// ���� 4�� �ȴ�.
int cRenjuRule::IsOpenFour(int x, int y, int nStone, int nDir)
{
	// ���� 4�� �� ���� ���ʿ� ���� ������ ��
	// ���� ��� ������ �Ǿ�� �ϱ� ������
	// �� ��ǥ ���� 4�� �Ǵ��� �˻縦 ���� �����̴�.
	int tx, ty;

	int sum = 0;

	nDir % 2 ? nDir -= 1 : nDir;
	SetStone(x, y, nStone);

	for (int i = 0; i < 2; i++)
	{
		tx = x;
		ty = y;
		// ��������� Four()�Լ��� ����.
		if (IsEmpty(tx, ty, nStone, nDir + i))
		{
			if (IsFive(tx, ty, nStone, nDir + i)) sum++;
		}
	}

	SetStone(x, y, LINE);

	// �������� ��ΰ� ������ �Ǿ������� ���� 4 �̴�.
	if (sum == 2)
	{
		// ���� 4�� ���� Ư���ϰ� �� �ٿ� 44�� �� �� �ִ�.
		// 1234567    12345678
		// 0 0 0 0    00  0 00
		// ���� ���� ��Ȳ���� 4��ǥ�� ���� ���̰� �Ǹ�
		// ������ ��� 4�� �Ǿ� ���ٿ��� 44�� �߻��Ѵ�.
		// ������ 0000 �̿� ���� ������ ���� 4�� �ϳ��̱� ������
		// �˻縦 �Ͽ� ������ ��Ȯ�ϰ� ������ش�.
		if (GetStoneCount(x, y, nStone, nDir) == 4) sum = 1;
	}
	// sum�� 2�� �ƴ϶�� ���� 4�� �ƴϹǷ� 0���� �ʱ�ȭ
	else sum = 0;

	return sum;
}

// �ֻ� ��Ģ üũ
bool cRenjuRule::IsDoubleFour(int x, int y, int nStone)
{
	int cnt = 0;

	// ��ǥ�� �߽����� ������ �ٿ� ���Ͽ� 4�� �˻��Ѵ�.
	for (int i = 0; i < 8; i += 2)
	{
		// OpenFour()������ �� �ٿ� 44�� ���� �� ������ �˻��ϰ�
		if (IsOpenFour(x, y, nStone, i) == 2) return true;

		// ���� 4�� 4�̴� 4�� �˻��Ͽ� 2�� �̻��̸� �ֻ� �̴�.
		else if (IsFour(x, y, nStone, i)) cnt++;

		if (cnt >= 2) return true;
	}

	return false;
}

bool cRenjuRule::IsOpenThree(int x, int y, int nStone, int nDir)
{
	int tx, ty;

	SetStone(x, y, nStone);

	for (int i = 0; i < 2; i++)
	{
		tx = x;
		ty = y;

		if (IsEmpty(tx, ty, nStone, nDir += i))
		{
			// 33�� ���� �ϳ� �� ������ �� �ݵ�� ���� 4�̾�� �ϰ�,
			// �� �� ���� �ڸ��� 44, 6�� �� �ٸ� 33
			// �� �ݼ� �ڸ��� �ƴϾ�� �Ѵ�.
			if ((IsOpenFour(tx, ty, nStone, nDir) == 1) && (!IsForbidden(tx, ty, nStone)))
			{
				// �� ������ �����ϴ� �ڸ��̸� 3�̹Ƿ� ���� �����ϰ� ���� �����Ѵ�.
				SetStone(x, y, LINE);
				return true;
			}
		}
	}

	SetStone(x, y, LINE);
	return false;
}

// �ֻ� ��Ģ üũ
bool cRenjuRule::IsDoubleThree(int x, int y, int nStone)
{
	int cnt = 0;

	for (int i = 0; i < 8; i += 2)
	{
		if (IsOpenThree(x, y, nStone, i)) cnt++;

		if (cnt >= 2) return true;
	}

	return false;
}

// �ݼ��ڸ��� üũ�Ѵ�.
bool cRenjuRule::IsForbidden(int x, int y, int nStone)
{
	if (IsDoubleFour(x, y, nStone)) return true;
	else if (IsDoubleThree(x, y, nStone)) return true;
	else if (IsSix(x, y, nStone)) return true;
	else return false;
}

/*-------------------------------------���� ���� üũ�� ���� Gomoku Class-------------------------------------*/
class cGomoku
{
protected:
	static int arrBoard[SIZE + 2][SIZE + 2];
	static int x, y;

	// ������ ���� ����
	static int lx, ly;

	// �Ѽ� ������ ���� �÷��� ����
	static bool undoflag;

	// Ŀ���� �������� �����ϱ� ���� �ð�
	static time_t t1, t2;

	cRenjuRule renjurule;

	bool isOccupy();
	void undoSet();
	void drawBoard();
	void saveBoard(int stone);
	void setXY(int ax, int ay);

public:
	cGomoku();
	virtual ~cGomoku() { ; }

	void setBoard(int x, int y);
	void cursorView(int x, int y);
	void initGomoku();
	void initBoard();
	int undo();

	virtual int placement(int ax, int ay, int stone);
};

/*------------------------------------���� �� ���¸� class ������ ���� üũ------------------------------------*/
/*---------------------Renju Rule�� ���� ���� ���� �Ǳ� ������ ������ class�� �и� �Ͽ���.----------------------*/
class cBlackStone : public cGomoku
{
public:
	cBlackStone();
	~cBlackStone() { ; }

	virtual int placement(int ax, int ay, int stone);
};

int cGomoku::arrBoard[SIZE + 2][SIZE + 2] = { LINE, };
int cGomoku::lx = 0;
int cGomoku::ly = 0;
int cGomoku::x = CENTER;
int cGomoku::y = CENTER;
time_t cGomoku::t1 = clock() - 500;
time_t cGomoku::t2 = clock();
bool cGomoku::undoflag = 0;

cGomoku::cGomoku()
{
}

// ������ ���۵� �� �ʱ�ȭ
void cGomoku::initGomoku()
{
	drawBoard();
	initBoard();
	undoflag = 0;
	t2 = clock();

	// 1�ʿ� �ѹ��� �׸��µ� ������ �α� ���ؼ�
	// ��������� 500ms���� Ŀ��ó�� �����Ÿ��� �ȴ�.
	t1 = static_cast<time_t>(clock()) - 500;
	pDraw()->infoKey();
}

// Ŀ���� �̵��ϱ� �� ���� �������� �����Ѵ�.
void cGomoku::setBoard(int x, int y)
{
	pDraw()->printData(x, y, arrBoard[y][x]);

	// Ŀ���� �̵� �� �϶� Ŀ���� �����Ÿ��� �ʰ� �Ѵ�.
	// ������ �� �� �ð��� ������ �ʾ� �����̵��� �ð��� ����
	t2 = static_cast<time_t>(clock()) - 400;
	t1 = static_cast<time_t>(clock()) - 900;
}

// Ŀ���� ����� Ŀ��ó�� ���̰� �Ѵ�.
void cGomoku::cursorView(int x, int y)
{
	// Ŀ���� 1�� ������ �׷��ش�.
	if (clock() - t1 >= 1000)
	{
		pDraw()->printData(x, y, CURSOR);
		t1 = clock();
	}

	// Ŀ�� ��ġ�� �������� 1�� ������ �׷��ش�.
	if (clock() - t2 >= 1000)
	{
		pDraw()->printData(x, y, arrBoard[y][x]);
		t2 = clock();
	}
}

// �� �� ������ �����Լ�
int cGomoku::undo()
{
	if (!undoflag) return NOTUNDO;

	pDraw()->printData(lx, ly, LINE);
	arrBoard[ly][lx] = LINE;
	undoflag = false;

	return CHANGE;
}

void cGomoku::undoSet()
{
	lx = x;
	ly = y;
	undoflag = true;
}

// �ٵ����� �׸���.
void cGomoku::drawBoard()
{
	pDraw()->printNum();

	for (int i = 1; i <= SIZE; i++)
	{
		for (int j = 1; j <= SIZE; j++)
		{
			pDraw()->printData(j, i, LINE);
		}
	}
}

// ���� ���̸� �迭�� ��, �鵹�� �����Ѵ�.
// ó�� �浹�� �߾ӿ� �� �� ���� �����Ѵ�.
void cGomoku::initBoard()
{
	for (int i = 0; i < SIZE + 2; i++)
	{
		for (int j = 0; j <= SIZE + 2; j++)
		{
			arrBoard[i][j] = LINE;
		}
	}
}

// ����� ���� �ڸ��� üũ�Ѵ�.
bool cGomoku::isOccupy()
{
	return arrBoard[y][x] != LINE;
}

// ������� ���� �Ǹ� ���忡 ������� �׸���
// ���� �迭�� �����Ѵ�.
void cGomoku::saveBoard(int stone)
{
	pDraw()->printData(x, y, stone);
	arrBoard[y][x] = stone;
	undoSet();
}

// Game class���� �޾ƿ� ��ǥ�� �����Ѵ�.
void cGomoku::setXY(int ax, int ay)
{
	x = ax;
	y = ay;
}

// ������ ���Ѻ���
// ������ �������� üũ�ϰ�,
// ������ �Ǹ� ������ �Ѵ�.
// �������� �������� �˻��Ͽ� ������� ��ȯ�Ѵ�.
int cGomoku::placement(int ax, int ay, int nStone)
{
	setXY(ax, ay);
	if (isOccupy()) return OCCUPIED;

	int returnValue = CHANGE;
	renjurule.SetBoard(arrBoard);
	if (renjurule.IsFive(x, y, nStone) || renjurule.IsSix(x, y, nStone)) returnValue = FIVEMOK;
	saveBoard(nStone);

	return returnValue;
}

/*------------------------------------���� �� ���¸� class ������ ���� üũ------------------------------------*/
/*---------------------Renju Rule�� ���� ���� ���� �Ǳ� ������ ������ class�� �и� �Ͽ���.----------------------*/
cBlackStone::cBlackStone()
{
}

int cBlackStone::placement(int ax, int ay, int nStone)
{
	setXY(ax, ay);
	if (isOccupy()) return OCCUPIED;

	renjurule.SetBoard(arrBoard);
	if (renjurule.IsFive(x, y, nStone))
	{
		saveBoard(nStone);
		return FIVEMOK;
	}
	else if (renjurule.IsSix(x, y, nStone)) return SIXMOK;
	else if (renjurule.IsDoubleFour(x, y, nStone))  return DOUBLEFOUR;
	else if (renjurule.IsDoubleThree(x, y, nStone)) return DOUBLETHREE;

	saveBoard(nStone);
	return CHANGE;
}

/*--------------------------------��ü���� ������ ������ �����ϴ� Game Class-----------------------------------*/
class cGame
{
private:
	// t1 : 1�ʸ��� ȭ�鿡 �ð��� ǥ�����ֱ� ���� ����
	// ���ʸ��� �ʱ�ȭ�� ����
	// t2 : ������ ���� �� �� �ʱ�ȭ �Ͽ�
	// ������ ����� ������ �����ϸ鼭 ���ؽð��� �ȴ�.
	// t3 : �ð������� �ʿ��� �� ����� ����
	time_t t1 = clock(), t2 = clock(), t3 = clock();

	// ���� ���� �� ��
	int curStone = BLACK_STONE;

	// gomoku�� x, y�� ���� ��ǥ�� �����Ѵ�.
	int x = CENTER, y = CENTER;

	// ���ºθ� �˱� ���ؼ��� ������ ���� �ʿ��ϴ�.
	// ������ pass�� �ϸ� passTrigger�� On���°� �ȴ�.
	// �׷� passCounter�� ���۵ǰ�
	// �������� �鵹�� �浹�� �������� ��������,
	// �ƴϸ� ���� ���� �������� pass�� ������
	// �Ǵ��ϱ� ���Ͽ� stoneState�� �迭�� ����Ѵ�.
	bool passTriggerOn = false;
	int passCount = 0;
	int stoneState[2] = { 0, };

	cGomoku white;
	cBlackStone black;

	cGomoku* pGomoku[2];

	bool checkTie();
	void drawTime();
	void initGame();
	void changeStone();
	int getKey();
	int checkKey();

public:
	cGame();
	~cGame() { ; }

	bool playGame();
};

cGame::cGame()
{
	pGomoku[0] = &black;
	pGomoku[1] = &white;
}

// ������ ������ �� �ʿ��� �������� �ʱ�ȭ �ϰ�,
// ȭ�鿡 ǥ�����ش�.
void cGame::initGame()
{
	x = y = CENTER;
	t1 = t2 = clock();
	passTriggerOn = false;
	passCount = 0;
	stoneState[0] = 0;
	stoneState[1] = 0;
	drawTime();
	curStone = BLACK_STONE;
	pDraw()->printData(x, y, BLACK_STONE);
	pDraw()->showMsg(curStone);
	pGomoku[curStone]->initGomoku();
	pGomoku[curStone]->cursorView(x, y);
}

// main�Լ����� �� �Լ��� ȣ���ϸ� ������ ���۵ȴ�.
bool cGame::playGame()
{
	int result;

	system("cls");
	initGame();
	while (true)
	{
		// 1�ʸ��� ȭ�鿡 �ð��� ǥ���Ѵ�.
		if (clock() - t1 >= 1000) drawTime();

		result = checkKey();

		switch (result)
		{
			// Ű���� ESC�� ������ ������ �����Ѵ�.
		case FINISH: return false;
			// ������ �Ǿ����� ���ڸ� �˸���, �ѹ� �� �� ������ �����.
		case FIVEMOK: return pDraw()->endMsg(curStone);
			//������ �Ұ����� ���� �޽����� ��� return�Ѵ�.
		case OCCUPIED:
		case DOUBLETHREE:
		case DOUBLEFOUR:
		case SIXMOK:
		case NOTUNDO:
			pDraw()->errMsg(result);
			pGomoku[curStone]->setBoard(x, y);
			break;
			// ������ �ưų�. �Ѽ� ������ �� ���� �ٲ۴�.
		case PASS: if (!passTriggerOn) passTriggerOn = true;
			stoneState[curStone]++;
		case CHANGE: changeStone(); break;
		default: break;
		}

		pGomoku[curStone]->cursorView(x, y);
		if (checkTie()) return pDraw()->endMsg(TIED);
		Sleep(20);
	}
}

// ��� ���� ���ʸ� �ٲ۴�.
void cGame::changeStone()
{
	// ��� ���̴� ���ؼ� 2�� ������ ��ȯ�� �ȴ�.
	curStone = (curStone + 1) % 2;

	// ���� ���� ���ʸ� �˷��ش�.
	pDraw()->showMsg(curStone);
	if (passTriggerOn) passCount++;
}

bool cGame::checkTie()
{
	if (!passTriggerOn) return false;

	bool isTied = false;
	if (passCount == 2)
	{
		if (stoneState[0] == 1 && stoneState[1] == 1) isTied = true;
		passTriggerOn = false;
		passCount = 0;
		stoneState[0] = 0;
		stoneState[1] = 0;
	}
	return isTied;
}

// 1�ʸ��� ȭ�鿡 �ð��� ǥ���Ѵ�.
void cGame::drawTime()
{
	time_t sec = (clock() - t2) / 1000;
	pDraw()->drawTime(sec);
	t1 = clock();
}

int cGame::getKey()
{
	int ch = _getch();
	if (ch == 0 || ch == 0xE0) ch = _getch();

	return ch;
}

int cGame::checkKey()
{
	int ch, dx, dy;;

	if (_kbhit()) ch = getKey();
	else return 0;

	dx = dy = 0;
	switch (ch)
	{
	case UP: --dy; break;
	case DOWN: ++dy; break;
	case LEFT: --dx; break;
	case RIGHT: ++dx; break;

	case DEL: return PASS;
	case ESC: return FINISH;
	case ' ': return pGomoku[curStone]->placement(x, y, curStone);
	case 'u':
	case 'U': return pGomoku[curStone]->undo();
	default: break;
	}
	pGomoku[curStone]->setBoard(x, y);
	y += dy;
	x += dx;
	pDraw()->printData(x, y, CURSOR);

	return 0;
}

int main()
{
	pDraw()->CursorView(HIDE);

	cGame game;
	while (game.playGame());

	pDraw()->CursorView(SHOW);

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#pragma region 키입력 전처리 지정

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

// 각종 색깔들
enum {
	BLACK,			/*  0 : 검정 */
	DARK_BLUE,		/*  1 : 어두운 파랑 */
	DARK_GREEN,		/*  2 : 어두운 초록 */
	DARK_SKY_BLUE,	/*  3 : 어두운 하늘 */
	DARK_RED,		/*  4 : 어두운 빨강 */
	DARK_VIOLET,	/*  5 : 어두운 보라 */
	DARK_YELLOW,	/*  6 : 어두운 노랑 */
	GRAY,			/*  7 : 회색 */
	DARK_GRAY,		/*  8 : 어두운 회색 */
	BLUE,			/*  9 : 파랑 */
	GREEN,			/* 10 : 초록 */
	SKY_BLUE,		/* 11 : 하늘 */
	RED,			/* 12 : 빨강 */
	VIOLET,			/* 13 : 보라 */
	YELLOW,			/* 14 : 노랑 */
	WHITE,			/* 15 : 하양 */
};

// 오목판의 크기
#define SIZE   15
#define CENTER ((SIZE + 2) / 2)
#define FINISH -1
#define TIED   2

// 바둑판의 색상, 바둑돌의 색상, 게임의 상태 등등
enum { BLACK_STONE, WHITE_STONE, CURSOR, BLACKWIN, WHITEWIN, TIE, LINE };
enum { OCCUPIED = 10, DOUBLETHREE, DOUBLEFOUR, SIXMOK, NOTUNDO, FIVEMOK, CHANGE, PASS };
enum { HIDE, SHOW };

/*-----------------------------------------화면을 그려줄 Draw class------------------------------------------*/
class cDraw
{
private:
	cDraw();
	~cDraw() { ; }
	// 게임보드의 첫 생성 시작 위치
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

// singleton 사용을 위한 매크로
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

// 화면의 커서를 보이게 하거나 숨겨놓기
void cDraw::CursorView(char show)
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor{};

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

// x, y 좌표로 커서를 움직이는 함수
void cDraw::gotoxy(int x, int y)
{
	COORD XY = { (short)x,(short)y };
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
	const char* str[] =
	{
		"이미 돌이 놓여 있습니다.\n다른 곳에 착수하세요.",
		"쌍삼 입니다.\n다른 곳에 착수하세요.",
		"쌍사 입니다.\n다른 곳에 착수하세요.",
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
		"무승부입니다.     ",
	};

	SetColor(GRAY);
	gotoxy((nX + SIZE + 1) * 2, nY);
	printf("%s", str[msg]);
}

// 입력 가능한 키들 설명 UI를 띄움
void cDraw::infoKey()
{
	const char* str[] =
	{
		"스페이스바 : 착  수",
		"방  향  키 : 이  동",
		"    U      : 무르기",
		"Delete Key : PASS      ",
		" ",
		"흑과 백이 연속으로",
		"PASS하면 무승부"
	};

	SetColor(GRAY);

	for (int i = 0; i < 7; i++)
	{
		gotoxy((nX + SIZE + 1) * 2, nY + 3 + i);
		printf(str[i]);
	}
}

// 승패결정 이후 게임을
// 다시 할 것인지 물어봄
bool cDraw::endMsg(int stone)
{
	const char* winner[] =
	{
		"    흑이 승리했습니다!\n    한번 더 하시겠습니까?    ",
		"    백이 승리했습니다!\n    한번 더 하시겠습니까?    ",
		"    무승부입니다.\n    한번 더 하시겠습니까?    "
	};

	showMsg(stone + 3);
	if (MessageBox(NULL, winner[stone], "알 림", MB_YESNO) == IDYES) return true;
	else return false;
}

// 바둑판과 돌을 콘솔 창에 그림
void cDraw::printData(int& x, int& y, int type)
{
	if (x < 1) x = 1;
	else if (y < 1) y = 1;
	else if (x > SIZE) x = SIZE;
	else if (y > SIZE) y = SIZE;

	const int color[] = { BLACK, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK };
	const char* shape[] = { "┌", "┬", "┐", "├", "┼", "┤", "└", "┴", "┘", "●", "●", "□" };

	SetColor(color[type], DARK_YELLOW);

	// 각 좌표마다 들어갈 선의 종류가 다르기 때문에
	// 어떤걸 그려야 할지 좌표를 주고 모양을 가져온다.
	if (type == LINE) type = getLineNumber(x, y);
	// 선은 0-8까지 이고, 그 뒤에 돌이 배치되어 있으니 9를 더해준다.
	else type += 9;
	gotoxy((x + nX - 1) * 2, y + nY - 1);
	printf("%s", shape[type]);
	SetColor(GRAY);
}

// 가로와 세로줄의 좌표를 숫자와 알파벳으로 표시한다.
void cDraw::printNum()
{
	SetColor(BLACK, DARK_YELLOW);

	// x(가로)방향 출력
	for (int i = 0; i < SIZE; i++)
	{
		gotoxy((nX + i) * 2, nY + SIZE);
		printf("%2c", i + 'A');
	}

	// y(세로)방향 출력
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
	// 왼쪽 세로줄
	if (x == 1)
	{
		// x, y가 전부 0이라면 왼쪽 상단 모서리
		if (y == 1) return 0;
		// 왼쪽 하단 모서리
		else if (y == SIZE) return 6;
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

// 게임 시간 화면표시
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

/*-----------------------------------------------렌주룰 Class-----------------------------------------------*/
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
		// 바둑판 밖의 배열의 끝을 다른 숫자로
		// 나타냄으로써 경계를 표시한다.
		nBoard[i][0] = nBoard[i][SIZE + 1] = -1;
		nBoard[0][i] = nBoard[SIZE + 1][i] = -1;
	}
}

// 착수하기 전의 바둑판 상태를 미리 복사해둠
void cRenjuRule::SetBoard(int(*arr)[SIZE + 2])
{
	for (int i = 1; i <= SIZE; i++)
		for (int j = 1; j <= SIZE; j++)
			nBoard[i][j] = arr[i][j];
}

// x, y 좌표에 돌을 놓거나 지운다(지운 후 그 자리에 선 다시 생성)
void cRenjuRule::SetStone(int x, int y, int nStone)
{
	nBoard[y][x] = nStone;
}

// 8개의 숫자는 각 8개의 방향을 나타낸다.
// 배열의 순서대로 서, 동, 북, 남, 북서, 남동, 남서, 북동
// 2배열씩 한쌍이 되어 가로 세로 대각선의 양쪽을 표시한다.
void cRenjuRule::GetDirTable(int& x, int& y, int nDir)
{
	int dx[] = { -1,  1,  0,  0, -1,   1,  -1,   1 };
	int dy[] = { 0,  0, -1,  1, -1,   1,   1,  -1 };

	x = dx[nDir];
	y = dy[nDir];
}

// 주어진 방향으로 좌표를 옮겨 빈 곳을 찾아
// 그곳의 좌표를 알려주는 역할
bool cRenjuRule::IsEmpty(int& x, int& y, int nStone, int nDir)
{
	int dx, dy;

	// 방향 Table에서 dx, dy 값을 가져온다.
	// 만약 nDir이 0이라면 dx = -1, dy = 0이다.
	GetDirTable(dx, dy, nDir);

	// x는 1 y는 0이니까 x+= dx, y+=dy 이 수식에 의해서
	// x는 점점 작아져 왼쪽으로 가고, y는 변함이 없으니 한자리에 머문다.
	// 그래서 가로로 왼쪽에 nStone이 아닌 곳에서 멈추게 된다.
	for (; nBoard[y][x] == nStone; x += dx, y += dy);

	// nStone이 아니라 해서 전부 LINE이라 할 수 없다.
	// 다른 돌이거나 바둑판을 벗어난 곳 일수도 있으니
	// 그곳이 LINE일 때만 참을 반환한다.
	if (nBoard[y][x] == LINE) return true;
	else return false;
}

int cRenjuRule::GetStoneCount(int x, int y, int nStone, int nDir)
{
	int dx, dy;
	int tx = x;
	int ty = y;
	int cnt = 0;

	// 착수할 곳이 LINE이 아니라면 착수할 수 없다.
	// 0(거짓)을 반환
	if (nBoard[y][x] != LINE) return 0;

	SetStone(x, y, nStone);

	// 방향 Table에서 dx, dy 값을 가져와
	GetDirTable(dx, dy, nDir);
	// 착수한 곳에서 부터 nDir방향으로 같은 돌의 갯수를 센다.
	for (; nBoard[y][x] == nStone; x += dx, y += dy) cnt++;

	// 방향 Table이 하나로 되어있고,
	// 순서는 항상 (동, 서) 또는 (남, 북)으로 되어 있어
	// nDir이 홀수일 때는 -1만큼 뺀 방향을 검사하여야 한다.
	// 그렇지 않고 다음 방향이라고 무심코 +1을 해버리면
	// 만약 nDir이 동쪽(1)이라면 서쪽(0)을 검사해야 되는데
	// +1해서 2가되면 서쪽(0)이 아닌 북쪽(2)을 검사하게 된다.
	// 이를 방지하기 위해 3항 연산자를 이용해
	// 조건에 맞는 방향을 인수로 넘기게 하였다. 
	GetDirTable(dx, dy, nDir % 2 ? nDir - 1 : nDir + 1);

	// x, y의 값이 위의 연산들에 의해 첫 좌표를 벗어낫기 때문에
	// 첫 좌표 값에 다음 방향으로 한칸 옮겨서 그곳에서부터
	// 돌의 갯수를 세기 시작한다.
	// 위에서 돌을 놓았던 곳을 세었기 때문에 중복체크를 방지하기 위해서이다.
	x = tx + dx;
	y = ty + dy;
	for (; nBoard[y][x] == nStone; x += dx, y += dy)cnt++;

	// 돌의 갯수를 다 세었다면 돌을 들어낸다.
	SetStone(tx, ty, LINE);
	return cnt;
}

// 착수 하였을 때 오목인지 체크한다.
bool cRenjuRule::IsFive(int x, int y, int nStone)
{
	// 방향 Table의 값이 8이니까 8까지 검사하고
	// 증가분이 +=2 인건
	// 아래 GetStoneCount()함수 안에서 양끝을 검사하기 때문
	for (int i = 0; i < 8; i += 2)
	{
		if (GetStoneCount(x, y, nStone, i) == 5) return true;
	}

	return false;
}

// 위 함수와 같이 오목인지를 검사하지만,
// nDir방향에 대해서만 검사한다.
// 이는 Four나 OpenFour가 되는지 알아보기 위해서
// 빈 곳에 돌을 하나씩 놓아보면서
// 오목이 되는지 검사하기 위해서이다.
bool cRenjuRule::IsFive(int x, int y, int nStone, int nDir)
{
	if (GetStoneCount(x, y, nStone, nDir) == 5) return true;

	return false;
}

// 오목인지 검사하는 것처럼 육목 이상인지 검사한다.
bool cRenjuRule::IsSix(int x, int y, int nStone)
{
	for (int i = 0; i < 8; i += 2)
	{
		if (GetStoneCount(x, y, nStone, i) >= 6) return true;
	}

	return false;
}

// 착수 했을 때 4인지 검사한다.
bool cRenjuRule::IsFour(int x, int y, int nStone, int nDir)
{
	// 원래의 좌표 x, y를 대신하기 위한 변수
	int tx, ty;

	// 한 방향에 대하여 양끝을 검사하는데,
	// 한 쌍의 좌표 중 첫번째부터 검사하기 위함이다.
	nDir % 2 ? nDir -= 1 : nDir;

	// 해당 좌표에 착수한다.
	SetStone(x, y, nStone);

	// 다음으로 양끝을 검사
	for (int i = 0; i < 2; i++)
	{
		// 원래의 좌표를 중심으로 주어진 방향(nDir)에 대하여
		// 양쪽으로 이동하면서 빈곳을 찾아 그곳의 좌표를 얻어와야 되기에
		// 원래의 좌표가 변경이 된다. 따라서 양쪽을 검사하기 위해서
		// 대신할 변수가 필요하고, 매번 아래와 같이
		// 원래의 좌표를 받아서 검사가 진행되어야 한다.
		tx = x;
		ty = y;

		// 빈 곳이 있다면 그 좌표를 받아온다.
		if (IsEmpty(tx, ty, nStone, nDir + i))
		{
			if (IsFive(tx, ty, nStone, nDir + i))
			{
				// 오목이 되었다면 4이기 때문에
				// 돌을 들어내고 참을 리턴한다.
				SetStone(x, y, LINE);
				return true;
			}
		}
	}

	// 검사를 해봤지만 4가 아니라고 해도 돌을 들어낸다.
	SetStone(x, y, LINE);
	return false;
}

// 돌이 연속으로 4개이고 끝에 돌을 놓았을 때 오목이 되면
// 열린 4가 된다.
int cRenjuRule::IsOpenFour(int x, int y, int nStone, int nDir)
{
	// 열린 4가 될 때는 양쪽에 돌을 놓았을 때
	// 양쪽 모두 오목이 되어야 하기 때문에
	// 두 좌표 전부 4가 되는지 검사를 위한 변수이다.
	int tx, ty;

	int sum = 0;

	nDir % 2 ? nDir -= 1 : nDir;
	SetStone(x, y, nStone);

	for (int i = 0; i < 2; i++)
	{
		tx = x;
		ty = y;
		// 여기까지는 Four()함수와 같다.
		if (IsEmpty(tx, ty, nStone, nDir + i))
		{
			if (IsFive(tx, ty, nStone, nDir + i)) sum++;
		}
	}

	SetStone(x, y, LINE);

	// 양쪽으로 모두가 오목이 되었을때만 열린 4 이다.
	if (sum == 2)
	{
		// 열린 4일 때는 특이하게 한 줄에 44가 될 수 있다.
		// 1234567    12345678
		// 0 0 0 0    00  0 00
		// 위와 같은 상황에서 4좌표에 돌이 놓이게 되면
		// 양쪽이 모두 4가 되어 한줄에서 44가 발생한다.
		// 때문에 0000 이와 같이 연속일 때는 4가 하나이기 때문에
		// 검사를 하여 갯수를 정확하게 만들어준다.
		if (GetStoneCount(x, y, nStone, nDir) == 4) sum = 1;
	}
	// sum이 2가 아니라면 열린 4가 아니므로 0으로 초기화
	else sum = 0;

	return sum;
}

// 쌍사 규칙 체크
bool cRenjuRule::IsDoubleFour(int x, int y, int nStone)
{
	int cnt = 0;

	// 좌표를 중심으로 각각의 줄에 대하여 4를 검사한다.
	for (int i = 0; i < 8; i += 2)
	{
		// OpenFour()에서는 한 줄에 44가 나올 수 있으니 검사하고
		if (IsOpenFour(x, y, nStone, i) == 2) return true;

		// 열린 4도 4이니 4를 검사하여 2개 이상이면 쌍사 이다.
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
			// 33은 돌을 하나 더 놓았을 때 반드시 열린 4이어야 하고,
			// 한 점 놓을 자리가 44, 6목 또 다른 33
			// 즉 금수 자리가 아니어야 한다.
			if ((IsOpenFour(tx, ty, nStone, nDir) == 1) && (!IsForbidden(tx, ty, nStone)))
			{
				// 위 조건을 만족하는 자리이면 3이므로 돌을 제거하고 참을 리턴한다.
				SetStone(x, y, LINE);
				return true;
			}
		}
	}

	SetStone(x, y, LINE);
	return false;
}

// 쌍삼 규칙 체크
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

// 금수자리를 체크한다.
bool cRenjuRule::IsForbidden(int x, int y, int nStone)
{
	if (IsDoubleFour(x, y, nStone)) return true;
	else if (IsDoubleThree(x, y, nStone)) return true;
	else if (IsSix(x, y, nStone)) return true;
	else return false;
}

/*-------------------------------------게임 상태 체크를 위한 Gomoku Class-------------------------------------*/
class cGomoku
{
protected:
	static int arrBoard[SIZE + 2][SIZE + 2];
	static int x, y;

	// 마지막 착수 지점
	static int lx, ly;

	// 한수 물리기 위한 플래그 변수
	static bool undoflag;

	// 커서의 깜빡임을 구현하기 위한 시간
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

/*------------------------------------검은 돌 상태를 class 단위로 따로 체크------------------------------------*/
/*---------------------Renju Rule은 검은 돌만 적용 되기 때문에 별도의 class로 분리 하였다.----------------------*/
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

// 오목이 시작될 때 초기화
void cGomoku::initGomoku()
{
	drawBoard();
	initBoard();
	undoflag = 0;
	t2 = clock();

	// 1초에 한번씩 그리는데 시차를 두기 위해서
	// 결과적으로 500ms마다 커서처럼 깜빡거리게 된다.
	t1 = static_cast<time_t>(clock()) - 500;
	pDraw()->infoKey();
}

// 커서를 이동하기 전 원래 오목판을 복원한다.
void cGomoku::setBoard(int x, int y)
{
	pDraw()->printData(x, y, arrBoard[y][x]);

	// 커서가 이동 중 일땐 커서가 깜빡거리지 않게 한다.
	// 멈췄을 때 얼마 시간이 지나지 않아 깜빡이도록 시간을 조절
	t2 = static_cast<time_t>(clock()) - 400;
	t1 = static_cast<time_t>(clock()) - 900;
}

// 커서를 대신해 커서처럼 보이게 한다.
void cGomoku::cursorView(int x, int y)
{
	// 커서를 1초 단위로 그려준다.
	if (clock() - t1 >= 1000)
	{
		pDraw()->printData(x, y, CURSOR);
		t1 = clock();
	}

	// 커서 위치의 오목판을 1초 단위로 그려준다.
	if (clock() - t2 >= 1000)
	{
		pDraw()->printData(x, y, arrBoard[y][x]);
		t2 = clock();
	}
}

// 한 수 물리기 위한함수
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

// 바둑판을 그린다.
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

// 돌이 놓이면 배열에 흑, 백돌을 저장한다.
// 처음 흑돌을 중앙에 한 수 놓고 시작한다.
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

// 오목알 놓을 자리를 체크한다.
bool cGomoku::isOccupy()
{
	return arrBoard[y][x] != LINE;
}

// 오목알이 착수 되면 보드에 오목알을 그리고
// 보드 배열에 저장한다.
void cGomoku::saveBoard(int stone)
{
	pDraw()->printData(x, y, stone);
	arrBoard[y][x] = stone;
	undoSet();
}

// Game class에서 받아온 좌표를 저장한다.
void cGomoku::setXY(int ax, int ay)
{
	x = ax;
	y = ay;
}

// 착수를 위한변수
// 착수가 가능한지 체크하고,
// 착수가 되면 저장을 한다.
// 다음으로 오목인지 검사하여 결과값을 반환한다.
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

/*------------------------------------검은 돌 상태를 class 단위로 따로 체크------------------------------------*/
/*---------------------Renju Rule은 검은 돌만 적용 되기 때문에 별도의 class로 분리 하였다.----------------------*/
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

/*--------------------------------전체적인 오목의 진행을 전담하는 Game Class-----------------------------------*/
class cGame
{
private:
	// t1 : 1초마다 화면에 시간을 표시해주기 위한 변수
	// 매초마다 초기화를 해줌
	// t2 : 게임이 시작 될 떄 초기화 하여
	// 게임이 종료될 때까지 유지하면서 기준시간이 된다.
	// t3 : 시간제한이 필요할 때 사용할 변수
	time_t t1 = clock(), t2 = clock(), t3 = clock();

	// 현재 착수 할 돌
	int curStone = BLACK_STONE;

	// gomoku의 x, y와 같은 좌표를 유지한다.
	int x = CENTER, y = CENTER;

	// 무승부를 알기 위해서는 변수가 많이 필요하다.
	// 누군가 pass를 하면 passTrigger가 On상태가 된다.
	// 그럼 passCounter가 시작되고
	// 연속으로 백돌과 흑돌이 연속으로 눌렀는지,
	// 아니면 한쪽 돌만 연속으로 pass한 것인지
	// 판단하기 위하여 stoneState는 배열을 사용한다.
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

// 게임이 시작할 때 필요한 변수들을 초기화 하고,
// 화면에 표시해준다.
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

// main함수에서 이 함수를 호출하면 게임이 시작된다.
bool cGame::playGame()
{
	int result;

	system("cls");
	initGame();
	while (true)
	{
		// 1초마다 화면에 시간을 표시한다.
		if (clock() - t1 >= 1000) drawTime();

		result = checkKey();

		switch (result)
		{
			// 키보드 ESC가 눌리면 게임을 종료한다.
		case FINISH: return false;
			// 오목이 되었으면 승자를 알리고, 한번 더 할 것인지 물어본다.
		case FIVEMOK: return pDraw()->endMsg(curStone);
			//착수가 불가능한 곳은 메시지를 띄워 return한다.
		case OCCUPIED:
		case DOUBLETHREE:
		case DOUBLEFOUR:
		case SIXMOK:
		case NOTUNDO:
			pDraw()->errMsg(result);
			pGomoku[curStone]->setBoard(x, y);
			break;
			// 착수가 됐거나. 한수 물렸을 때 돌을 바꾼다.
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

// 흑과 백의 차례를 바꾼다.
void cGame::changeStone()
{
	// 흑백 둘이니 더해서 2로 나누면 교환이 된다.
	curStone = (curStone + 1) % 2;

	// 현재 돌의 차례를 알려준다.
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

// 1초마다 화면에 시간을 표시한다.
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
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

#define MAX_MAP 19 // 바둑판의 최대 크기
#define DRAW_BLACK printf("○") // 흑 돌을 그릴 때의 문자 정의
#define DRAW_WHITE printf("●") // 백 돌을 그릴 때의 문자 정의
#define BLACK 1 // 흑 돌을 1로 정의
#define WHITE 2	// 백 돌은 2로 정의

void gotoxy(int x, int y);

int board[MAX_MAP][MAX_MAP] = { 0 };

void setCursorView();
void drawInit();
int setStoneOnBoard(int y, int x, int stone);
void inputCode(int* y, int* x);
int checkFinish(int t);
void gotoInput();


int main()
{
	setCursorView();

	drawInit();

	int x, y, i = 0;

	while (1)
	{
		inputCode(&y, &x);

		i += setStoneOnBoard(y, x, (i) % 2 + 1);

		if (int c = checkFinish(0))
		{
			gotoInput();
			printf("게임 종료!\n");

			if (c == BLACK)
				printf("검은 돌 "), DRAW_BLACK;
			else if (c == WHITE)
				printf("흰 돌 "), DRAW_WHITE;

			printf("승리 !!!\n");
			break;
		}
	}

	_getch();

	return 0;
}

// 콘솔창 내에서 커서의 움직임 비활성화
void setCursorView()
{
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1; // 커서의 굵기 지정 (1 ~ 100)
	cursorInfo.bVisible = 0; // 커서의 시각화 유무 TRUE(1) 보임 / FALSE(0) 안보임
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// 게임 종료 구분짓는 역할
int checkFinish(int t)
{
	if (t == MAX_MAP * MAX_MAP)
		return 0;

	int x = t % MAX_MAP, y = t / MAX_MAP; // t 값을 각각 board에 들어갈 x,y 좌표로 변환
	int color = board[y][x];
	int dir[3] = { 0 };

	// 0,0 부터 MAX_MAP, MAX_MAP 좌표까지 이동하며 게임이 끝났는지 반복 확인.
	for (int i = 0; i < 5 && color != 0; i++)
	{
		// 가로 범위의 게임 승리 여부 확인
		if (x + i < MAX_MAP && color == board[y][x + i])
			dir[0]++;

		// 세로 범위의 게임 승리 여부 확인
		if (y + i < MAX_MAP && color == board[y + i][x])
			dir[1]++;

		// 대각선 범위의 게임 승리 여부 확인
		if (x + i < MAX_MAP && y + i < MAX_MAP && color == board[y + i][x + i])
			dir[2]++;
	}

	// 게임이 끝났다면(5개의 돌이 연속지어진 한 줄) 해당 줄의 color값을 반환함
	// 아니라면 0을 반환하여 다시 게임 재개
	for (int i = 0; i < 3; i++)
		if (dir[i] == 5)
			return color;

	return checkFinish(t + 1);
}

// x와 y를 call by reference로 받음
void inputCode(int* y, int* x)
{
	// 콘솔 커서를 입력할 위치로 이동
	gotoInput();

	printf("돌을 둘 좌표를 입력하세요. (A1 ~ S19): ");
	char s[10]; // 입력 받은 문자열을 저장할 변수
	gets_s(s, sizeof(s)); // 문자열 입력
	*x = s[0] - 'A'; // 문자열 첫 문자에 'A'를 뺀 값을 저장. (s[0]이A라면 x는0, B는1, .. 이 저장됨.)

	// 만약 문자열의 길이가 4이상이라면 return (4 이상이 입력되면 의미x A134라는건 없기 때문)
	if (strlen(s) > 3)
		return;
	if (strlen(s) == 3)	// 문자열의 길이가 3이라면 숫자 정보를 추출하여 y값에 저장
		*y = (s[1] - '0') * 10 + s[2] - '0' - 1;
	else if (strlen(s) == 2) // 문자열의 길이가 2일때 숫자 정보 추출
		*y = s[1] - '0' - 1;
	else // 길이가 1이하이면 이렇게 저장
		*y = *x -= 1;

	gotoInput(); printf("돌을 둘 좌표를 입력하세요. (A1 ~ S19): \t\t\t");

	return;
}

// 돌을 바둑판 위에 두는 함수
int setStoneOnBoard(int y, int x, int stone)
{
	// 돌을 둘 수 없는 상황이거나 stone이 흑, 백이 아닌 잘못된 값이면
	// 0으로 반환, 정상으로 작동시 1을 반환
	if (board[y][x] || x < 0 || x >= MAX_MAP || y < 0 || y >= MAX_MAP)
		return 0;

	gotoxy(x, y);

	if (stone == BLACK)
	{
		DRAW_BLACK;
		board[y][x] = BLACK;
	}
	else if (stone == WHITE)
	{
		DRAW_WHITE;
		board[y][x] = WHITE;
	}
	else
		return 0;

	return 1;
}

// 바둑판을 그려내는 함수 (19x19)
void drawInit()
{
	gotoxy(0, 0);
	for (int i = 0; i < MAX_MAP; i++)
	{
		for (int j = 0; j < MAX_MAP; j++)
		{
			gotoxy(j, i);
			if (i == 0)
			{
				if (j == 0)
					printf("┌");
				else if (j == MAX_MAP - 1)
					printf("┐");
				else
					printf("┬");
			}
			else if (j == 0) {
				if (i == MAX_MAP - 1)
					printf("└");
				else
					printf("├");
			}
			else if (j == MAX_MAP - 1)
			{
				if (i == MAX_MAP - 1)
					printf("┘");
				else
					printf("┤");
			}
			else if (i == MAX_MAP - 1)
			{
				printf("┴");
			}
			else
				printf("┼");
		}
		printf("\n");
	}

	for (int i = 0; i < MAX_MAP; i++)
	{
		gotoxy(MAX_MAP, i);
		printf("%d", i + 1);
		gotoxy(i, MAX_MAP);
		printf("%c", i + 'A');
	}

	gotoInput();
	printf("fin.");
}

// 좌표 지정후 커서이동
void gotoxy(int x, int y)
{
	// x, y 좌표 설정
	COORD pos = { x * 2,y };

	// 해당하는 좌표로 커서를 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void gotoInput()
{
	gotoxy(21, 20);
}
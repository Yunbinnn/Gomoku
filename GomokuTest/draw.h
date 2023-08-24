#pragma once
#include "console.h"

// 오목판의 크기
#define SIZE      15
#define CENTER 	  ((SIZE + 2) / 2)
#define FINISH    -1

// 바둑판의 색상, 바둑돌의 색상 
enum {BLACK_STONE, WHITE_STONE, CURSOR, BLACKWIN, WHITEWIN, LINE};
enum { OCCUPIED = 10, SAMSAM, SASA, SIXMOK, NOTUNDO, FIVEMOK, CHANGE };
enum { HIDE, SHOW };

// 화면을 그려줄 Draw class
class cDraw
{
private : 
	cDraw();
	~cDraw();
	// 게임보드의 첫 생성 시작 위치
	int nX, nY;

public : 
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
};

#define pDraw cDraw::getInstance
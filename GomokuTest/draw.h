#pragma once
#include "console.h"

// �������� ũ��
#define SIZE      15
#define CENTER 	  ((SIZE + 2) / 2)
#define FINISH    -1

// �ٵ����� ����, �ٵϵ��� ���� 
enum {BLACK_STONE, WHITE_STONE, CURSOR, BLACKWIN, WHITEWIN, LINE};
enum { OCCUPIED = 10, SAMSAM, SASA, SIXMOK, NOTUNDO, FIVEMOK, CHANGE };
enum { HIDE, SHOW };

// ȭ���� �׷��� Draw class
class cDraw
{
private : 
	cDraw();
	~cDraw();
	// ���Ӻ����� ù ���� ���� ��ġ
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
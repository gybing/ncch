#include<Windows.h>

#include "position.h"
#include "winform.h"
#include "resource.h"

const int MASK_COLOR = RGB(0, 0, 0);
const int BOARD_EDGE = 8;		//
const int TOOLBAR_SIZE = 41;		

const int SQUARE_SIZE = 56;		//����С������56
const int PIECE_SIZE  = 52;		//���Ӵ�С������52

const int LBrook_LEFT = 55;		//�������ϽǺڳ�λ��X���������ͼ�񣩣�����55
const int LBrook_TOP  = 70;		//�������ϽǺڳ�λ��Y���������ͼ�񣩣�����70

const int BOARD_WIDTH  = 560;
const int BOARD_HEIGHT = 645;
const int ScreenOffX =   BOARD_EDGE + LBrook_LEFT ;	//��߿հ�
const int ScreenOffY =   BOARD_EDGE + LBrook_TOP + TOOLBAR_SIZE;	//�ϱ߿հ�

// "DrawSquare"����
const bool DRAW_SELECTED = true;

static void DrawLine(HDC hdc,int x1,int y1,int x2,int y2);//����
static void DrawBoard1(HDC hdc);//������

// ���Ƹ���
void DrawSquare(int sq, BOOL bSelected)
{
	int sqFlipped, xx, yy, pc;
	sqFlipped = wforms.bFlipped ? SQUARE_FLIP(sq) : sq;
	xx = LBrook_LEFT - SQUARE_SIZE/2+ (FILE_X(sqFlipped) - FILE_LEFT) * SQUARE_SIZE ;		//��ʱ��X�����������λͼ���Ͻǵ�
	yy = LBrook_TOP  - SQUARE_SIZE/2+ (RANK_Y(sqFlipped) - RANK_TOP) * SQUARE_SIZE  ;
	SelectObject(wforms.hdcTmp, wforms.bmpBoard);   //λͼѡ�����ݵ��豸����ʱ�����Ͻ������ǣ�0��0��ע��ת����
	//BitBlt(hdc, BOARD_EDGE, BOARD_EDGE + TOOLBAR_SIZE, BOARD_WIDTH, BOARD_HEIGHT, hdcTmp, 0, 0, SRCCOPY);   //��������ʱ������
	BitBlt(wforms.hdc, BOARD_EDGE+xx, BOARD_EDGE+TOOLBAR_SIZE+yy, SQUARE_SIZE, SQUARE_SIZE, wforms.hdcTmp, xx, yy, SRCCOPY);			//�ָ�ԭ���̸ô�ͼ��
	pc = pos.ucpcSquares[sq];

	xx += BOARD_EDGE ;
	yy += BOARD_EDGE + TOOLBAR_SIZE ;
	if (pc != 0) 
	{
		DrawTransBmp(wforms.hdc, wforms.hdcTmp,xx, yy, wforms.bmpPieces[pc]);
	}
	if (bSelected)
	{
		DrawTransBmp(wforms.hdc, wforms.hdcTmp,  xx, yy,  wforms.bmpSelected);
	}
}

// ��������¼�����
void ClickSquare(int sq) {
	int pc, mv, vlRep;
	wforms.hdc = GetDC(wforms.hWnd);
	wforms.hdcTmp = CreateCompatibleDC(wforms.hdc);
	sq = wforms.bFlipped ? SQUARE_FLIP(sq) : sq;
	pc = pos.ucpcSquares[sq];

	if ((pc & SIDE_TAG(pos.sdPlayer)) != 0)
	{
		// �������Լ����ӣ���ôֱ��ѡ�и���
		if (wforms.sqSelected != 0) 
		{
			DrawSquare(wforms.sqSelected);
		}
		wforms.sqSelected = sq;
		DrawSquare(sq, DRAW_SELECTED);
		if (wforms.mvLast != 0)
		{
			DrawSquare(SRC(wforms.mvLast));
			DrawSquare(DST(wforms.mvLast));
		}
		PlayResWav(IDR_CLICK); // ���ŵ��������
	} 
	else if (wforms.sqSelected != 0 && !wforms.bGameOver)
	{
		// �������Ĳ����Լ����ӣ�������ѡ����(һ�����Լ�����)����ô�������
		mv = MOVE(wforms.sqSelected, sq);
		if (pos.LegalMove(mv)) 
		{
			if (pos.MakeMove(mv)) 
			{
				wforms.mvLast = mv;
				DrawSquare(wforms.sqSelected, DRAW_SELECTED);
				DrawSquare(sq, DRAW_SELECTED);
				wforms.sqSelected = 0;
				// ����ظ�����
				vlRep = pos.RepStatus(3);
				if (pos.IsMate()) {
					// ����ֳ�ʤ������ô����ʤ�������������ҵ���������������ʾ��
					PlayResWav(IDR_WIN);
					MessageBoxMute("ף����ȡ��ʤ����");
					wforms.bGameOver = TRUE;
				} 
				else if (vlRep > 0) 
				{
					vlRep = pos.RepValue(vlRep);
					// ע�⣺"vlRep"�ǶԵ�����˵�ķ�ֵ
					PlayResWav(vlRep > WIN_VALUE ? IDR_LOSS : vlRep < -WIN_VALUE ? IDR_WIN : IDR_DRAW);
					MessageBoxMute(vlRep > WIN_VALUE ? "�����������벻Ҫ���٣�" :
						vlRep < -WIN_VALUE ? "���Գ���������ף����ȡ��ʤ����" : "˫���������ͣ������ˣ�");
					wforms.bGameOver = TRUE;
				} 
				else if (pos.nMoveNum > 100) 
				{
					PlayResWav(IDR_DRAW);
					MessageBoxMute("������Ȼ�������ͣ������ˣ�");
					wforms.bGameOver = TRUE;
				} 
				else 
				{
					// ���û�зֳ�ʤ������ô���Ž��������ӻ�һ�����ӵ�����
					PlayResWav(pos.InCheck() ? IDR_CHECK : pos.Captured() ? IDR_CAPTURE : IDR_MOVE);
					if (pos.Captured()) 
					{
						pos.SetIrrev();
					}
					ResponseMove(); // �ֵ���������
				}
			}
			else 
			{
				PlayResWav(IDR_ILLEGAL); // ���ű�����������
			}
		}
		// ��������Ͳ������߷�(������������)����ô���������
	}
	DeleteDC(wforms.hdcTmp);
	ReleaseDC(wforms.hWnd, wforms.hdc);
}


// ����͸��ͼƬ
//TransparentBlt�ĵ�����2��3����������С��ͼƬʵ�ʴ�С�������Ϊ��BitBlt��ͬ����Ҫ����ע�⡣#pragma comment( lib, "msimg32.lib" )��TransparentBlt������Ҫ���������⡣
void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp) {
	SelectObject(hdcTmp, bmp);
	TransparentBlt(hdc, xx, yy, PIECE_SIZE, PIECE_SIZE, hdcTmp, 0, 0, PIECE_SIZE, PIECE_SIZE, MASK_COLOR);
}

void DrawLine(HDC hdc,int x1,int y1,int x2,int y2)//����
{
	MoveToEx (hdc, x1, y1, NULL) ;
	LineTo (hdc,x2, y2) ;
}

// ��������
void DrawBoard(HDC hdc) {
	int x, y, xx, yy, sq, pc;
	HDC hdcTmp;

	// ������
	hdcTmp = CreateCompatibleDC(hdc);
	SelectObject(hdcTmp, wforms.bmpBoard);
	BitBlt(hdc, BOARD_EDGE, BOARD_EDGE + TOOLBAR_SIZE, BOARD_WIDTH, BOARD_HEIGHT, hdcTmp, 0, 0, SRCCOPY);
	DrawBoard1(hdc);

	// ������
	for (x = FILE_LEFT; x <= FILE_RIGHT; x ++) {
		for (y = RANK_TOP; y <= RANK_BOTTOM; y ++) {
			if (wforms.bFlipped) 
			{
				xx = BOARD_EDGE + LBrook_LEFT -	SQUARE_SIZE/2 +	(FILE_FLIP(x) - FILE_LEFT) * SQUARE_SIZE;
				yy = BOARD_EDGE + LBrook_TOP + TOOLBAR_SIZE - SQUARE_SIZE/2 + (RANK_FLIP(y) - RANK_TOP) * SQUARE_SIZE;
			} 
			else 
			{
				xx = BOARD_EDGE + LBrook_LEFT- SQUARE_SIZE/2 +  (x - FILE_LEFT) * SQUARE_SIZE;
				yy = BOARD_EDGE  + LBrook_TOP + TOOLBAR_SIZE - SQUARE_SIZE/2 + (y - RANK_TOP) * SQUARE_SIZE;
			}

			sq = COORD_XY(x, y);
			pc = pos.ucpcSquares[sq];

			if (pc != 0) 
			{
				DrawTransBmp(hdc, hdcTmp, xx, yy, wforms.bmpPieces[pc]);
			}
			if (sq == wforms.sqSelected || sq == SRC(wforms.mvLast) || sq == DST(wforms.mvLast)) 
			{
				DrawTransBmp(hdc, hdcTmp, xx, yy, wforms.bmpSelected);
			}
		}
	}
	DeleteDC(hdcTmp);
}

void DrawBoard1(HDC hdc)//������
{
	char buffer[]="��       ��              ��      ��";
	char numberB[9][3]={"1","2","3","4","5","6","7","8","9"};
	char numberR[9][3]={"һ","��","��","��","��","��","��","��","��"};
	int i;
	
	//������
	for( i=0;i<=9;i++)
	{
		DrawLine(hdc,ScreenOffX, ScreenOffY+SQUARE_SIZE*i, ScreenOffX+SQUARE_SIZE*8, ScreenOffY+SQUARE_SIZE*i);
	}

	TextOut(hdc, ScreenOffX*3, int(ScreenOffY+SQUARE_SIZE*4.3), buffer, sizeof(buffer)-1) ;//���Ӻ���

	//������
	for( i=0;i<=8;i++)
	{
		TextOut(hdc, ScreenOffX+SQUARE_SIZE*i, ScreenOffY+SQUARE_SIZE*0-SQUARE_SIZE, numberB[i], sizeof(numberB[i])-2) ;//�ڷ����
		TextOut(hdc, ScreenOffX+SQUARE_SIZE*i, int(ScreenOffY+SQUARE_SIZE*9+SQUARE_SIZE*0.7), numberR[i], sizeof(numberR[i])-1) ;//�췽���

		if(i==0||i==8)
		{
			DrawLine(hdc,ScreenOffX+SQUARE_SIZE*i,ScreenOffY,ScreenOffX+SQUARE_SIZE*i,ScreenOffY+SQUARE_SIZE*9);
		}
		else
		{
			DrawLine(hdc,ScreenOffX+SQUARE_SIZE*i,ScreenOffY,ScreenOffX+SQUARE_SIZE*i,ScreenOffY+SQUARE_SIZE*4);
			DrawLine(hdc,ScreenOffX+SQUARE_SIZE*i,ScreenOffY+SQUARE_SIZE*5,ScreenOffX+SQUARE_SIZE*i,ScreenOffY+SQUARE_SIZE*9);
		}
	}

	////���Ź���
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*2);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*7,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*9);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*2,ScreenOffX+SQUARE_SIZE*5,ScreenOffY);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*9,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*7);

	////����λ
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*2);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*7,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*9);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*2,ScreenOffX+SQUARE_SIZE*5,ScreenOffY);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*9,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*7);
	//
	////����λ
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*2);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*7,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*9);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*2,ScreenOffX+SQUARE_SIZE*5,ScreenOffY);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*9,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*7);

	////����ߴֵķ���
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*2);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*7,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*9);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*2,ScreenOffX+SQUARE_SIZE*5,ScreenOffY);
	//DrawLine(hdc,ScreenOffX+SQUARE_SIZE*3,ScreenOffY+SQUARE_SIZE*9,ScreenOffX+SQUARE_SIZE*5,ScreenOffY+SQUARE_SIZE*7);
}


// ����������������ʾ��
void MessageBoxMute(LPCSTR lpszText) {
	MSGBOXPARAMS mbp;
	mbp.cbSize = sizeof(MSGBOXPARAMS);
	mbp.hwndOwner = wforms.hWnd;
	mbp.hInstance = NULL;
	mbp.lpszText = lpszText;
	mbp.lpszCaption = "������Ȥ";
	mbp.dwStyle = MB_USERICON;
	mbp.lpszIcon = MAKEINTRESOURCE(IDI_INFORMATION);
	mbp.dwContextHelpId = 0;
	mbp.lpfnMsgBoxCallback = NULL;
	mbp.dwLanguageId = 0;
	if (MessageBoxIndirect(&mbp) == 0) {
		// ϵͳͼ���� Windows 98 �»�ʧ�ܣ�����Ҫʹ��Ӧ�ó���ͼ��
		mbp.hInstance = wforms.hInst;
		mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
		MessageBoxIndirect(&mbp);
	}
}

// ���Ի�Ӧһ����
void ResponseMove(void) {
	int vlRep;
	// ������һ����
	SetCursor((HCURSOR) LoadImage(NULL, IDC_WAIT, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	int mv= SearchMain();
	SetCursor((HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	pos.MakeMove(mv);
	// �����һ�����ѡ����
	DrawSquare(SRC(wforms.mvLast));
	DrawSquare(DST(wforms.mvLast));
	// �ѵ����ߵ����ǳ���
	wforms.mvLast = mv;//Search.mvResult;
	DrawSquare(SRC(wforms.mvLast), DRAW_SELECTED);
	DrawSquare(DST(wforms.mvLast), DRAW_SELECTED);
	// ����ظ�����
	vlRep = pos.RepStatus(3);
	if (pos.IsMate())
	{
		// ����ֳ�ʤ������ô����ʤ�������������ҵ���������������ʾ��
		PlayResWav(IDR_LOSS);
		MessageBoxMute("���ٽ�������");
		wforms.bGameOver = TRUE;
	} 
	else if (vlRep > 0) 
	{
		vlRep = pos.RepValue(vlRep);
		// ע�⣺"vlRep"�Ƕ������˵�ķ�ֵ
		PlayResWav(vlRep < -WIN_VALUE ? IDR_LOSS : vlRep > WIN_VALUE ? IDR_WIN : IDR_DRAW);
		MessageBoxMute(vlRep < -WIN_VALUE ? "�����������벻Ҫ���٣�" :
			vlRep > WIN_VALUE ? "���Գ���������ף����ȡ��ʤ����" : "˫���������ͣ������ˣ�");
		wforms.bGameOver = TRUE;
	} 
	else if (pos.nMoveNum > 100) 
	{
		PlayResWav(IDR_DRAW);
		MessageBoxMute("������Ȼ�������ͣ������ˣ�");
		wforms.bGameOver = TRUE;
	} 
	else 
	{
		// ���û�зֳ�ʤ������ô���Ž��������ӻ�һ�����ӵ�����
		PlayResWav(pos.InCheck() ? IDR_CHECK2 : pos.Captured() ? IDR_CAPTURE2 : IDR_MOVE2);
		if (pos.Captured()) 
		{
			pos.SetIrrev();
		}
	}
}


// ��ʼ�����
void Startup(void) 
{
	pos.Startup();
	wforms.sqSelected = wforms.mvLast = 0;
	wforms.bGameOver = FALSE;
}

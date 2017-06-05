#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>  

#include "resource.h"
#include "positionstruct.h"
//#include "link\\toolsCV.h"

#pragma comment(lib,"comctl32.lib")  
#pragma comment(lib, "WINMM.LIB")
#pragma comment( lib, "msimg32.lib" )

using namespace std;
//using namespace cv;

extern PositionStruct pos;		// ����ʵ��

// �汾��
const LPCSTR cszAbout = "��Ȥ���壨ͼ���������� Ver 0530\n\n������ʿ";

// ���ںͻ�ͼ����
const int WINDOW_STYLES = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
//const int MASK_COLOR = RGB(0, 255, 0);
const int MASK_COLOR = RGB(0, 0, 0);
const int SQUARE_SIZE = 56;
const int BOARD_EDGE = 8*5;
const int BOARD_WIDTH = BOARD_EDGE + SQUARE_SIZE * 9 + BOARD_EDGE;
const int BOARD_HEIGHT = BOARD_EDGE + SQUARE_SIZE * 10 + BOARD_EDGE;

struct {
	HINSTANCE hInst;                              // Ӧ�ó�����ʵ��
	HWND hWnd;                                    // �����ھ��
	HDC hdc, hdcTmp;                              // �豸�����ֻ��"ClickSquare"��������Ч
	HBITMAP bmpBoard, bmpSelected, bmpPieces[24]; // ��ԴͼƬ���
	int sqSelected, mvLast;                       // ѡ�еĸ��ӣ���һ����
	BOOL bFlipped, bGameOver;                     // �Ƿ�ת���̣��Ƿ���Ϸ����(���ü�������ȥ)
	HICON hIcon;
	HCURSOR hCursor;
}Xqwl;					// ��ͼ�ν����йص�ȫ�ֱ���

// ����͸��ͼƬ
//TransparentBlt�ĵ�����2��3����������С��ͼƬʵ�ʴ�С�������Ϊ��BitBlt��ͬ����Ҫ����ע�⡣#pragma comment( lib, "msimg32.lib" )��TransparentBlt������Ҫ���������⡣
void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp) {
	SelectObject(hdcTmp, bmp);
	TransparentBlt(hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, hdcTmp, 0, 0, SQUARE_SIZE, SQUARE_SIZE, MASK_COLOR);
	//TransparentBlt(hdc, xx, yy, PIECE_SIZE, PIECE_SIZE, hdcTmp, 0, 0, PIECE_SIZE, PIECE_SIZE, MASK_COLOR);
}


#define GridSize SQUARE_SIZE		//���ӱ߳�
#define ScreenOffX SQUARE_SIZE/2+BOARD_EDGE	//��߿հ�
#define ScreenOffY SQUARE_SIZE/2+BOARD_EDGE	//�ϱ߿հ�

void DrawLine(HDC hdc,double x1,double y1,double x2,double y2)//����
{
	MoveToEx (hdc, x1, y1, NULL) ;
	LineTo (hdc,x2, y2) ;
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
		DrawLine(hdc,ScreenOffX,ScreenOffY+GridSize*i,ScreenOffX+GridSize*8,ScreenOffY+GridSize*i);
	}

	TextOut(hdc, ScreenOffX*5, ScreenOffY+GridSize*4.3, buffer, sizeof(buffer)-1) ;//���Ӻ���

	//������
	for( i=0;i<=8;i++)
	{
		TextOut(hdc, ScreenOffX+GridSize*i, ScreenOffY+GridSize*0-GridSize, numberB[i], sizeof(numberB[i])-2) ;//�ڷ����
		TextOut(hdc, ScreenOffX+GridSize*i, ScreenOffY+GridSize*9+GridSize*0.7, numberR[i], sizeof(numberR[i])-1) ;//�췽���

		if(i==0||i==8)
		{
			DrawLine(hdc,ScreenOffX+GridSize*i,ScreenOffY,ScreenOffX+GridSize*i,ScreenOffY+GridSize*9);
		}
		else
		{
			DrawLine(hdc,ScreenOffX+GridSize*i,ScreenOffY,ScreenOffX+GridSize*i,ScreenOffY+GridSize*4);
			DrawLine(hdc,ScreenOffX+GridSize*i,ScreenOffY+GridSize*5,ScreenOffX+GridSize*i,ScreenOffY+GridSize*9);
		}
	}

	//���Ź���
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY,ScreenOffX+GridSize*5,ScreenOffY+GridSize*2);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*7,ScreenOffX+GridSize*5,ScreenOffY+GridSize*9);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*2,ScreenOffX+GridSize*5,ScreenOffY);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*9,ScreenOffX+GridSize*5,ScreenOffY+GridSize*7);

	//����λ
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY,ScreenOffX+GridSize*5,ScreenOffY+GridSize*2);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*7,ScreenOffX+GridSize*5,ScreenOffY+GridSize*9);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*2,ScreenOffX+GridSize*5,ScreenOffY);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*9,ScreenOffX+GridSize*5,ScreenOffY+GridSize*7);
	
	//����λ
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY,ScreenOffX+GridSize*5,ScreenOffY+GridSize*2);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*7,ScreenOffX+GridSize*5,ScreenOffY+GridSize*9);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*2,ScreenOffX+GridSize*5,ScreenOffY);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*9,ScreenOffX+GridSize*5,ScreenOffY+GridSize*7);

	//����ߴֵķ���
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY,ScreenOffX+GridSize*5,ScreenOffY+GridSize*2);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*7,ScreenOffX+GridSize*5,ScreenOffY+GridSize*9);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*2,ScreenOffX+GridSize*5,ScreenOffY);
	DrawLine(hdc,ScreenOffX+GridSize*3,ScreenOffY+GridSize*9,ScreenOffX+GridSize*5,ScreenOffY+GridSize*7);
}

// ��������
static void DrawBoard(HDC hdc) {
	int x, y, xx, yy, sq, pc;
	HDC hdcTmp;

	// ������
	hdcTmp = CreateCompatibleDC(hdc);
	SelectObject(hdcTmp, Xqwl.bmpBoard);
	BitBlt(hdc, SQUARE_SIZE/2, SQUARE_SIZE/2, BOARD_WIDTH, BOARD_HEIGHT, hdcTmp, 0, 0, SRCCOPY);
	DrawBoard1(hdc);

	// ������
	for (x = FILE_LEFT; x <= FILE_RIGHT; x ++) {
		for (y = RANK_TOP; y <= RANK_BOTTOM; y ++) {
			if (Xqwl.bFlipped) {
				xx = BOARD_EDGE + (FILE_FLIP(x) - FILE_LEFT) * SQUARE_SIZE;
				yy = BOARD_EDGE + (RANK_FLIP(y) - RANK_TOP) * SQUARE_SIZE;
			} else {
				xx = BOARD_EDGE + (x - FILE_LEFT) * SQUARE_SIZE;
				yy = BOARD_EDGE + (y - RANK_TOP) * SQUARE_SIZE;
			}
			sq = COORD_XY(x, y);
			pc = pos.ucpcSquares[sq];
			if (pc != 0) {
				DrawTransBmp(hdc, hdcTmp, xx, yy, Xqwl.bmpPieces[pc]);
			}
			if (sq == Xqwl.sqSelected || sq == SRC(Xqwl.mvLast) || sq == DST(Xqwl.mvLast)) {
				DrawTransBmp(hdc, hdcTmp, xx, yy, Xqwl.bmpSelected);
			}
		}
	}
	DeleteDC(hdcTmp);
}

// "DrawSquare"����
const BOOL DRAW_SELECTED = TRUE;

// ���Ƹ���
void DrawSquare(int sq, BOOL bSelected = FALSE) {
	int sqFlipped, xx, yy, pc;

	sqFlipped = Xqwl.bFlipped ? SQUARE_FLIP(sq) : sq;
	xx = BOARD_EDGE + (FILE_X(sqFlipped) - FILE_LEFT) * SQUARE_SIZE;
	yy = BOARD_EDGE + (RANK_Y(sqFlipped) - RANK_TOP) * SQUARE_SIZE;
	SelectObject(Xqwl.hdcTmp, Xqwl.bmpBoard);
	BitBlt(Xqwl.hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, Xqwl.hdcTmp, xx, yy, SRCCOPY);
	pc = pos.ucpcSquares[sq];
	if (pc != 0) {
		DrawTransBmp(Xqwl.hdc, Xqwl.hdcTmp, xx, yy, Xqwl.bmpPieces[pc]);
	}
	if (bSelected) {
		DrawTransBmp(Xqwl.hdc, Xqwl.hdcTmp, xx, yy, Xqwl.bmpSelected);
	}
}

// ������Դ����
inline void PlayResWav(int nResId) {
	PlaySound(MAKEINTRESOURCE(nResId), Xqwl.hInst, SND_ASYNC | SND_NOWAIT | SND_RESOURCE);
}

// ����������������ʾ��
void MessageBoxMute(LPCSTR lpszText) {
	MSGBOXPARAMS mbp;
	mbp.cbSize = sizeof(MSGBOXPARAMS);
	mbp.hwndOwner = Xqwl.hWnd;
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
		mbp.hInstance = Xqwl.hInst;
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
	DrawSquare(SRC(Xqwl.mvLast));
	DrawSquare(DST(Xqwl.mvLast));
	// �ѵ����ߵ����ǳ���
	Xqwl.mvLast = mv;//Search.mvResult;
	DrawSquare(SRC(Xqwl.mvLast), DRAW_SELECTED);
	DrawSquare(DST(Xqwl.mvLast), DRAW_SELECTED);
	// ����ظ�����
	vlRep = pos.RepStatus(3);
	if (pos.IsMate()) {
		// ����ֳ�ʤ������ô����ʤ�������������ҵ���������������ʾ��
		PlayResWav(IDR_LOSS);
		MessageBoxMute("���ٽ�������");
		Xqwl.bGameOver = TRUE;
	} else if (vlRep > 0) {
		vlRep = pos.RepValue(vlRep);
		// ע�⣺"vlRep"�Ƕ������˵�ķ�ֵ
		PlayResWav(vlRep < -WIN_VALUE ? IDR_LOSS : vlRep > WIN_VALUE ? IDR_WIN : IDR_DRAW);
		MessageBoxMute(vlRep < -WIN_VALUE ? "�����������벻Ҫ���٣�" :
			vlRep > WIN_VALUE ? "���Գ���������ף����ȡ��ʤ����" : "˫���������ͣ������ˣ�");
		Xqwl.bGameOver = TRUE;
	} else if (pos.nMoveNum > 100) {
		PlayResWav(IDR_DRAW);
		MessageBoxMute("������Ȼ�������ͣ������ˣ�");
		Xqwl.bGameOver = TRUE;
	} else {
		// ���û�зֳ�ʤ������ô���Ž��������ӻ�һ�����ӵ�����
		PlayResWav(pos.InCheck() ? IDR_CHECK2 : pos.Captured() ? IDR_CAPTURE2 : IDR_MOVE2);
		if (pos.Captured()) {
			pos.SetIrrev();
		}
	}
}

// ��������¼�����
void ClickSquare(int sq) {
	int pc, mv, vlRep;
	Xqwl.hdc = GetDC(Xqwl.hWnd);
	Xqwl.hdcTmp = CreateCompatibleDC(Xqwl.hdc);
	sq = Xqwl.bFlipped ? SQUARE_FLIP(sq) : sq;
	pc = pos.ucpcSquares[sq];

	if ((pc & SIDE_TAG(pos.sdPlayer)) != 0) {
		// �������Լ����ӣ���ôֱ��ѡ�и���
		if (Xqwl.sqSelected != 0) {
			DrawSquare(Xqwl.sqSelected);
		}
		Xqwl.sqSelected = sq;
		DrawSquare(sq, DRAW_SELECTED);
		if (Xqwl.mvLast != 0) {
			DrawSquare(SRC(Xqwl.mvLast));
			DrawSquare(DST(Xqwl.mvLast));
		}
		PlayResWav(IDR_CLICK); // ���ŵ��������

	} else if (Xqwl.sqSelected != 0 && !Xqwl.bGameOver) {
		// �������Ĳ����Լ����ӣ�������ѡ����(һ�����Լ�����)����ô�������
		mv = MOVE(Xqwl.sqSelected, sq);
		if (pos.LegalMove(mv)) {
			if (pos.MakeMove(mv)) {
				Xqwl.mvLast = mv;
				DrawSquare(Xqwl.sqSelected, DRAW_SELECTED);
				DrawSquare(sq, DRAW_SELECTED);
				Xqwl.sqSelected = 0;
				// ����ظ�����
				vlRep = pos.RepStatus(3);
				if (pos.IsMate()) {
					// ����ֳ�ʤ������ô����ʤ�������������ҵ���������������ʾ��
					PlayResWav(IDR_WIN);
					MessageBoxMute("ף����ȡ��ʤ����");
					Xqwl.bGameOver = TRUE;
				} else if (vlRep > 0) {
					vlRep = pos.RepValue(vlRep);
					// ע�⣺"vlRep"�ǶԵ�����˵�ķ�ֵ
					PlayResWav(vlRep > WIN_VALUE ? IDR_LOSS : vlRep < -WIN_VALUE ? IDR_WIN : IDR_DRAW);
					MessageBoxMute(vlRep > WIN_VALUE ? "�����������벻Ҫ���٣�" :
						vlRep < -WIN_VALUE ? "���Գ���������ף����ȡ��ʤ����" : "˫���������ͣ������ˣ�");
					Xqwl.bGameOver = TRUE;
				} else if (pos.nMoveNum > 100) {
					PlayResWav(IDR_DRAW);
					MessageBoxMute("������Ȼ�������ͣ������ˣ�");
					Xqwl.bGameOver = TRUE;
				} else {
					// ���û�зֳ�ʤ������ô���Ž��������ӻ�һ�����ӵ�����
					PlayResWav(pos.InCheck() ? IDR_CHECK : pos.Captured() ? IDR_CAPTURE : IDR_MOVE);
					if (pos.Captured()) {
						pos.SetIrrev();
					}
					ResponseMove(); // �ֵ���������
				}
			} else {
				PlayResWav(IDR_ILLEGAL); // ���ű�����������
			}
		}
		// ��������Ͳ������߷�(������������)����ô���������
	}
	DeleteDC(Xqwl.hdcTmp);
	ReleaseDC(Xqwl.hWnd, Xqwl.hdc);
}

// ��ʼ�����
void Startup(void) {
	pos.Startup();
	Xqwl.sqSelected = Xqwl.mvLast = 0;
	Xqwl.bGameOver = FALSE;
}

// װ����ԴͼƬ
inline HBITMAP LoadResBmp(int nResId) {
	return (HBITMAP) LoadImage(Xqwl.hInst, MAKEINTRESOURCE(nResId), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
}

inline HCURSOR LoadResCur(int nResId) {
	return (HCURSOR) LoadImage(Xqwl.hInst, MAKEINTRESOURCE(nResId), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
}

struct CButton
{
	int     iStyle ;
	TCHAR * szText ;
}button[] = {
	BS_PUSHBUTTON,      TEXT ("���Ӳ���"),
	BS_DEFPUSHBUTTON,   TEXT ("��������"),
	BS_CHECKBOX,        TEXT ("CHECKBOX"), 
	BS_AUTOCHECKBOX,    TEXT ("AUTOCHECKBOX"),
	BS_RADIOBUTTON,     TEXT ("RADIOBUTTON"),
	BS_3STATE,          TEXT ("3STATE"),
	BS_AUTO3STATE,      TEXT ("AUTO3STATE"),
	BS_GROUPBOX,        TEXT ("GROUPBOX"),
	BS_AUTORADIOBUTTON, TEXT ("AUTORADIO"),
	BS_OWNERDRAW,       TEXT ("OWNERDRAW")
} ;
HWND hwndButton[10]; 


// �����¼���׽����
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int cxChar, cyChar;
	static int  cxClient, cyClient ;
    static HPEN hpen, holbpen;

	static HFONT hFont;  //�߼�����
	static HWND hLabUsername;  //��̬�ı���--�û���
	static HWND hLabPassword;  //��̬�ı���--����
	static HWND hEditUsername;  //�����ı������
	static HWND hEditPassword;  //���������
	static HWND hBtnLogin;  //��¼��ť

    TCHAR       ButtonName[]=TEXT("���ǰ�ť");  
    static  HWND  hWndStatus,hWndTool;  
    TCHAR    szBuf[MAX_PATH];  
    int nCount=3;  
    int array[3]={250,500,-1};  
    static TBBUTTON tbb[3];  
    static TBADDBITMAP tbab;  
    LPNMHDR lpnmhdr;  
    LPTOOLTIPTEXT lpttext;  

	//���建����
	TCHAR szUsername[100];
	TCHAR szPassword[100];
	TCHAR szUserInfo[200];

	static char buffer[255]="this is string!";
	int x, y;
	HDC hdc;
	static RECT rect;
	PAINTSTRUCT ps;
	// MSGBOXPARAMS mbp;
	//  BITMAP bmap;  
	HDC hdcmem;
	HANDLE hbitmap;
	HBITMAP hbmp;
	//  HDC hdcTmp ;

	switch (uMsg) {
		// �½�����
	case WM_CREATE:
		//toolbar and statusbar
		hWndStatus=CreateWindowEx(0,STATUSCLASSNAME,"",WS_CHILD|WS_BORDER|WS_VISIBLE,-100,-100,10,10,hWnd,(HMENU)100,Xqwl.hInst,NULL);  
        if(!hWndStatus)  
            MessageBox(hWnd,TEXT("can't create statusbar!"),TEXT("error_notify"),MB_OK);  
        SendMessage(hWndStatus,SB_SETPARTS,(WPARAM)nCount,(LPARAM)array);  
        SendMessage(hWndStatus,SB_SETTEXT,(LPARAM)1,(WPARAM)TEXT("I am status bar"));  
        SendMessage(hWndStatus,SB_SETTEXT,(LPARAM)2,(WPARAM)TEXT("by yiruirui"));  
        hWndTool=CreateWindowEx(0,TOOLBARCLASSNAME,NULL,WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS,0,0,0,0,hWnd,(HMENU)1111,Xqwl.hInst,NULL);  
        SendMessage(hWndTool,TB_BUTTONSTRUCTSIZE,(WPARAM)sizeof(TBBUTTON),0);  
  
        tbab.hInst=HINST_COMMCTRL;  
        tbab.nID=IDB_STD_SMALL_COLOR;  
        if(SendMessage(hWndTool,TB_ADDBITMAP,1,(LPARAM)&tbab)==-1)  
            MessageBox(hWnd,TEXT("ʧ��"),TEXT("����"),0);  
        ZeroMemory(tbb,sizeof(tbb));  
        tbb[0].iBitmap=STD_FILENEW;  
        tbb[0].fsState=TBSTATE_ENABLED;  
        tbb[0].fsStyle=TBSTYLE_BUTTON;  
        tbb[0].idCommand=ID_FILE_NEW;  
        tbb[0].iString=(INT_PTR)"yi";  
        tbb[1].iBitmap=STD_FILEOPEN;  
        tbb[1].fsState=TBSTATE_ENABLED;  
        tbb[1].fsStyle=TBSTYLE_BUTTON;  
        tbb[1].idCommand=ID_FILE_OPEN;  
        tbb[1].iString=(INT_PTR)"rui";  
        tbb[2].iBitmap=STD_FILESAVE;  
        tbb[2].fsState=TBSTATE_ENABLED;  
        tbb[2].fsStyle=TBSTYLE_BUTTON;  
        tbb[2].idCommand=ID_FILE_SAVEAS;  
        tbb[2].iString=(INT_PTR)"rui";  
  
        SendMessage(hWndTool,TB_ADDBUTTONS,sizeof(tbb)/sizeof(TBBUTTON),(LPARAM)&tbb);  
        SendMessage(hWndTool,TB_SETBUTTONSIZE,0,(LPARAM)MAKELONG(35,35));  
        SendMessage(hWndTool,TB_AUTOSIZE,0,0);  


		cxChar = LOWORD (GetDialogBaseUnits ()) ;
		cyChar = HIWORD (GetDialogBaseUnits ()) ;
		//�����߼�����
		hFont = CreateFont(-14/*��*/, -7/*��*/, 0, 0, 400 /*һ�����ֵ��Ϊ400*/,
			FALSE/*б��?*/, FALSE/*�»���?*/, FALSE/*ɾ����?*/,DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_DONTCARE, TEXT("΢���ź�")
			);

		//������̬�ı���ؼ�--�û���
		hLabUsername = CreateWindow(TEXT("static"), TEXT("�û�����"),
			WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE /*��ֱ����*/ | SS_RIGHT /*ˮƽ����*/,
			cxChar+BOARD_WIDTH /*x����*/, cyChar * (1 + 2 * 10) /*y����*/, 70 /*���*/, 26 /*�߶�*/,
			hWnd /*�����ھ��*/, (HMENU)1 /*�ؼ�ID*/, Xqwl.hInst /*��ǰ����ʵ�����*/, NULL	);

		//������̬�ı���ؼ�--����
		hLabPassword = CreateWindow(TEXT("static"), TEXT("���룺"),
			WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE /*��ֱ����*/ | SS_RIGHT /*ˮƽ����*/,
			cxChar+BOARD_WIDTH /*x����*/, cyChar * (1 + 2 * 11) /*y����*/, 70, 26,
			hWnd, (HMENU)2,  Xqwl.hInst, NULL);

		//���������ı���ؼ�
		hEditUsername = CreateWindow(TEXT("edit"), TEXT(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_AUTOHSCROLL /*ˮƽ����*/,
			cxChar+BOARD_WIDTH /*x����*/, cyChar * (1 + 2 * 12) /*y����*/, 200, 26,
			hWnd, (HMENU)3,  Xqwl.hInst, NULL
			);
		//�������������
		hEditPassword = CreateWindow(TEXT("edit"), TEXT(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD /*����*/ | ES_AUTOHSCROLL /*ˮƽ����*/,
			cxChar+BOARD_WIDTH /*x����*/, cyChar * (1 + 2 * 13) /*y����*/, 200, 26,
			hWnd, (HMENU)4,  Xqwl.hInst, NULL
			);
		//������ť�ؼ�
		hBtnLogin = CreateWindow(TEXT("button"), TEXT("��¼"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | BS_FLAT/*��ƽ��ʽ*/,
			cxChar+BOARD_WIDTH /*x����*/, cyChar * (1 + 2 * 14)/*y����*/,200, 30,
			hWnd, (HMENU)5,  Xqwl.hInst, NULL
			);
		//�������ÿؼ�������
		SendMessage(hLabUsername, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(hLabPassword, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(hEditUsername, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(hEditPassword, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(hBtnLogin, WM_SETFONT, (WPARAM)hFont, NULL);


		//���ɰ�ť���ӿؼ�
		for (int i = 0 ; i < 10 ; i++)
			hwndButton[i] = CreateWindow ( TEXT("button"), button[i].szText, WS_CHILD | WS_VISIBLE | button[i].iStyle, 
			cxChar+BOARD_WIDTH, cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4, hWnd, (HMENU) i,
			((LPCREATESTRUCT) lParam)->hInstance, NULL) ;

		// ��������λ�úͳߴ�
		GetWindowRect(hWnd, &rect);
		x = rect.left;
		y = rect.top;
		rect.right = rect.left + BOARD_WIDTH + BOARD_WIDTH/3;
		rect.bottom = rect.top + BOARD_HEIGHT + SQUARE_SIZE/2;
		AdjustWindowRect(&rect, WINDOW_STYLES, TRUE);
		MoveWindow(hWnd, x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		break;

	case WM_SIZE:
        {  
            HWND hTool;  
			hTool=GetDlgItem(Xqwl.hWnd,IDC_MAIN_TOOL);  
            SendMessage(hTool,TB_AUTOSIZE,0,0);  
            MoveWindow(hWndStatus,0,0,0,0,TRUE);  
        }  
		//cxChar = LOWORD(lParam);
		//cyChar = HIWORD(lParam);
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

		// �˵�����
	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
		case ID_FILE_NEW:  
			MessageBox(Xqwl.hWnd,TEXT("notify"),TEXT("file new"),0);  
			break;  
		case ID_FILE_OPEN:  
			MessageBox(Xqwl.hWnd,TEXT("notify"),TEXT("file open"),0);  
			break;  
		case ID_FILE_SAVEAS:  
			MessageBox(Xqwl.hWnd,TEXT("notify"),TEXT("file save as"),0);  
			break;  

		case 5:
			//��ȡ����������
			GetWindowText(hEditUsername, szUsername, 100);
			GetWindowText(hEditPassword, szPassword, 100);
			wsprintf(szUserInfo, TEXT("�����û��˺ţ�%s\r\n�����û����룺%s"), szUsername, szPassword);
			MessageBox(hWnd, szUserInfo, TEXT("��Ϣ��ʾ"), MB_ICONINFORMATION);
			break;
		case BS_PUSHBUTTON:
			strcpy(buffer, "BS_PUSHBUTTON");
			rect.left   = 0 ;
			rect.top    = 0 ;
			rect.right  = BOARD_WIDTH ;
			rect.bottom = 100;
			InvalidateRect (Xqwl.hWnd, &rect, TRUE) ;
			break;
		case BS_AUTO3STATE:
			strcpy(buffer, "BS_AUTO3STATE");
			rect.left   = 0 ;
			rect.top    = 0 ;
			rect.right  = BOARD_WIDTH ;
			rect.bottom = 100;
			InvalidateRect (Xqwl.hWnd, &rect, TRUE) ;
			break;
		case BS_RADIOBUTTON:
			strcpy(buffer, "BS_RADIOBUTTON");
			rect.left   = 0 ;
			rect.top    = 0 ;
			rect.right  = BOARD_WIDTH ;
			rect.bottom = 100;
			InvalidateRect (Xqwl.hWnd, &rect, TRUE) ;
			break;
		case IDM_FILE_RED:
		case IDM_FILE_BLACK:
			Xqwl.bFlipped = (LOWORD(wParam) == IDM_FILE_BLACK);
			Startup();
			hdc = GetDC(Xqwl.hWnd);
			DrawBoard(hdc);
			if (Xqwl.bFlipped) {
				Xqwl.hdc = hdc;
				Xqwl.hdcTmp = CreateCompatibleDC(Xqwl.hdc);
				ResponseMove();
				DeleteDC(Xqwl.hdcTmp);
			}
			ReleaseDC(Xqwl.hWnd, hdc);
			break;
		case IDM_FILE_EXIT:
			DestroyWindow(Xqwl.hWnd);
			break;
		case IDM_HELP_ABOUT:

			ShellAbout(hWnd,"������Ȥ","������Ȥ",Xqwl.hIcon);    

			//MessageBeep(MB_ICONINFORMATION);
			//   mbp.cbSize = sizeof(MSGBOXPARAMS);
			//   mbp.hwndOwner = hWnd;
			//   mbp.hInstance = Xqwl.hInst;
			//   mbp.lpszText = cszAbout;
			//   mbp.lpszCaption = "������Ȥ";
			//   mbp.dwStyle = MB_USERICON;
			//   mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
			//   mbp.dwContextHelpId = 0;
			//   mbp.lpfnMsgBoxCallback = NULL;
			//   mbp.dwLanguageId = 0;
			//   MessageBoxIndirect(&mbp);
			break;
		}
		break;
		// ��ͼ
	case WM_PAINT:
		hdc = BeginPaint(Xqwl.hWnd, &ps);

		DrawBoard(hdc);

		DrawText (hdc, TEXT ("Hello, Windows 98!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;  
		

		//�����¼�/////////////////////////////////////
		//   hbitmap = LoadImage(NULL,"..\\res\\rp.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);  
		//
		//	hdcmem = CreateCompatibleDC(hdc);  
		//hbmp= LoadResBmp(IDB_RP);
		//DrawTransBmp(Xqwl.hdc, hdcmem, 33, 33,hbmp);
		//DrawTransBmp(Xqwl.hdc, hdcmem, 0, 0, (HBITMAP)hbitmap);

		//GetObject(hbitmap,sizeof(BITMAP),&bmap);  
		//   SelectObject(hdcmem,hbitmap);  
		//   BitBlt(hdc,11,11,bmap.bmWidth,bmap.bmHeight,hdcmem,0,0,SRCCOPY); 

		//DeleteDC(hdcmem); 
		//DeleteObject(hbitmap); 
		////////////////////////////////////////////////////////////
		//Rectangle (hdc, cxClient/8, cyClient/8, 7*cxClient/8, 7*cyClient/8) ;
		//MoveToEx  (hdc,  0,  0, NULL) ;			LineTo(hdc, cxClient, cyClient) ;
		//MoveToEx  (hdc,  0, cyClient, NULL) ;	LineTo(hdc, cxClient,  0) ;
		//Ellipse   (hdc, cxClient/8, cyClient/8, 7*cxClient/8, 7*cyClient/8) ;
		//RoundRect (hdc, cxClient/4, cyClient/4, 3*cxClient/4, 3*cyClient/4, cxClient/4, cyClient/4 ) ;

		EndPaint(Xqwl.hWnd, &ps);
		break;
		// �����
	case WM_LBUTTONDOWN:
		x = FILE_LEFT + (LOWORD(lParam) - BOARD_EDGE) / SQUARE_SIZE;
		y = RANK_TOP + (HIWORD(lParam) - BOARD_EDGE) / SQUARE_SIZE;
		if (x >= FILE_LEFT && x <= FILE_RIGHT && y >= RANK_TOP && y <= RANK_BOTTOM) {
			ClickSquare(COORD_XY(x, y));
			SetCapture(Xqwl.hWnd);
			SetCursor(Xqwl.hCursor);
		}

		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		break;

	case    WM_MOUSEMOVE:  
        wsprintf(szBuf,"Mouse position:%d,%d",LOWORD(lParam),HIWORD(lParam));  
        SendMessage(hWndStatus,SB_SETTEXT,0,(LPARAM)(LPSTR)szBuf);  
        break;  

		// �˳�
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// �����¼�
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return FALSE;
}

void test(char* fn);

// ��ڹ���
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#ifdef _DEBUG
	//	test("E:\\link_vc2010\\RES\\3D��������(���)", "E:\\link_vc2010\\RES\\");
#endif
	int i;
	MSG msg;
	WNDCLASSEX wce;

	// ��ʼ��ȫ�ֱ���
	srand((DWORD) time(NULL));
	//InitZobrist();
	Xqwl.hInst = hInstance;
	LoadBook(hInstance);
	Xqwl.bFlipped = FALSE;
	Startup();

	// װ��ͼƬ
	Xqwl.bmpBoard = LoadResBmp(IDB_BOARD);
	Xqwl.bmpSelected = LoadResBmp(IDB_SELECTED);
	for (i = PIECE_KING; i <= PIECE_PAWN; i ++) {
		Xqwl.bmpPieces[SIDE_TAG(0) + i] = LoadResBmp(IDB_RK + i);
		Xqwl.bmpPieces[SIDE_TAG(1) + i] = LoadResBmp(IDB_BK + i);
	}

	// ���ô���
	wce.cbSize = sizeof(WNDCLASSEX);
	wce.style = 0;
	wce.lpfnWndProc = (WNDPROC) WndProc;
	wce.cbClsExtra = wce.cbWndExtra = 0;
	wce.hInstance = hInstance;
	wce.hIcon = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, LR_SHARED);
	wce.hCursor = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wce.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wce.lpszMenuName = MAKEINTRESOURCE(IDM_MAINMENU);
	wce.lpszClassName = "JQXQ";
	wce.hIconSm = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, LR_SHARED);
	RegisterClassEx(&wce);

	// �򿪴���
	Xqwl.hIcon = wce.hIcon;
	//Xqwl.hCursor=  SetCursor(LoadCursor(Xqwl.hInst, MAKEINTRESOURCE(IDC_CURSOR_HAND)));
	//Xqwl.hCursor = LoadCursorFromFile("hand.cur");
	Xqwl.hCursor = LoadResCur(IDC_CURSOR_HAND);

	Xqwl.hWnd = CreateWindow("JQXQ", "��Ȥ����", WINDOW_STYLES,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (Xqwl.hWnd == NULL) {
		return 0;
	}
	ShowWindow(Xqwl.hWnd, nCmdShow);
	UpdateWindow(Xqwl.hWnd);

	// ������Ϣ
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	}
	return msg.wParam;
}

#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>  

#include "resource.h"
#include "winform.h"
#include "position.h"
//#include "link\toolsCV.h"

#pragma comment(lib,"comctl32.lib")  
#pragma comment(lib, "WINMM.LIB")
#pragma comment( lib, "msimg32.lib" )

const LPCSTR cszAbout = "��Ȥ���壨ͼ���������� Ver 0530\n\n������ʿ";// �汾��
const int WINDOW_STYLES = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;// ���ںͻ�ͼ����

extern Position pos;		// ����ʵ��
WINFORMSTRUCT wforms;

void change_piece_size(char *path, char* dst,int size);//�����任��������ͼ���С
//LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ;


// �����¼���׽����
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{

	static struct CButton
	{
		int     iStyle ;
		TCHAR * szText ;
	}button[] = {
		BS_PUSHBUTTON,      TEXT ("���Ӳ���"),
		BS_PUSHBUTTON,      TEXT ("ȡ������"),
		BS_DEFPUSHBUTTON,   TEXT ("������"),
		BS_DEFPUSHBUTTON,   TEXT ("������"),
		BS_AUTOCHECKBOX,    TEXT ("���ֿ�"),
		BS_AUTOCHECKBOX,    TEXT ("�ƿ�"),
		//BS_AUTO3STATE,      TEXT ("����"),
		BS_AUTORADIOBUTTON, TEXT ("����"),
		BS_AUTORADIOBUTTON, TEXT ("����"),
		//BS_CHECKBOX,        TEXT ("CHECKBOX"), 
		//BS_RADIOBUTTON,     TEXT ("RADIOBUTTON"),
		//BS_3STATE,          TEXT ("3STATE"),
		//BS_GROUPBOX,        TEXT ("GROUPBOX"),
		//BS_OWNERDRAW,       TEXT ("OWNERDRAW")
	} ;
	static int NUMOFBUTTON =8;   //����ĿӦ������ʵ����ԣ��������
	static HWND hwndButton[10]; 

	static int cxChar, cyChar;
	static int  cxClient, cyClient ;
	static HPEN hpen, holbpen;

	static HFONT hFont;			//�߼�����
	static HWND hLabUsername;	//��̬�ı���--�û���
	static HWND hLabPassword;	 //��̬�ı���--����
	static HWND hEditUsername;  //�����ı������
	static HWND hEditPassword;  //���������
	static HWND hBtnLogin;		//��¼��ť
	static char buffer[255]="this is string!";
	static RECT rect;

    TCHAR   ButtonName[]=TEXT("��ť");  
    static  HWND  hWndStatus,hWndTool;  
    TCHAR   szBuf[MAX_PATH];  
    int nCount = 2;  
    int array[2]={150,-1};  
    static TBBUTTON tbb[3];  
    static TBADDBITMAP tbab;  
    //LPNMHDR lpnmhdr;  
    //LPTOOLTIPTEXT lpttext;  

	//���建����
//	TCHAR szUsername[100];
//	TCHAR szPassword[100];
//	TCHAR szUserInfo[200];
	HDC hdc;
	PAINTSTRUCT ps;
	int x, y;

	switch (uMsg) {
		// �½�����
	case WM_CREATE:
		//statusbar
		hWndStatus=CreateWindowEx(0,STATUSCLASSNAME,"",WS_CHILD|WS_BORDER|WS_VISIBLE,-100,-100,10,10,hWnd,(HMENU)100,wforms.hInst,NULL);  
        if(!hWndStatus)  
            MessageBox(hWnd,TEXT("can't create statusbar!"),TEXT("error_notify"),MB_OK);  
        SendMessage(hWndStatus,SB_SETPARTS,(WPARAM)nCount,(LPARAM)array);  
        SendMessage(hWndStatus,SB_SETTEXT,(LPARAM)1,(WPARAM)TEXT("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1"));  
        SendMessage(hWndStatus,SB_SETTEXT,(LPARAM)2,(WPARAM)TEXT("info message bestmove"));  
		
		//toolbar
        hWndTool=CreateWindowEx(0,TOOLBARCLASSNAME,NULL,WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS,0,0,0,0,hWnd,(HMENU)1111, wforms.hInst,NULL);  
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
        tbb[0].iString=(INT_PTR)"new";
        tbb[1].iBitmap=STD_FILEOPEN;  
        tbb[1].fsState=TBSTATE_ENABLED;  
        tbb[1].fsStyle=TBSTYLE_BUTTON;  
        tbb[1].idCommand=ID_FILE_OPEN;  
        tbb[1].iString=(INT_PTR)"open";  
		tbb[2].iBitmap=STD_FILESAVE;  
        tbb[2].fsState=TBSTATE_ENABLED;  
        tbb[2].fsStyle=TBSTYLE_BUTTON;  
        tbb[2].idCommand=ID_FILE_SAVEAS;  
        tbb[2].iString=(INT_PTR)"save";  
        SendMessage(hWndTool,TB_ADDBUTTONS,sizeof(tbb)/sizeof(TBBUTTON),(LPARAM)&tbb);  
        SendMessage(hWndTool,TB_SETBUTTONSIZE,0,(LPARAM)MAKELONG(35,35));  
        SendMessage(hWndTool,TB_AUTOSIZE,0,0);  

		cxChar = LOWORD (GetDialogBaseUnits ()) ;
		cyChar = HIWORD (GetDialogBaseUnits ()) ;

    	//���ɰ�ť���ӿؼ�
		for (int i = 0 ; i < NUMOFBUTTON ; i++)
			hwndButton[i] = CreateWindow ( TEXT("button"), button[i].szText, WS_CHILD | WS_VISIBLE | button[i].iStyle, 
				cxChar + BOARD_WIDTH + BOARD_EDGE, cyChar * (1 + 2 * i) + LBrook_TOP, 15 * cxChar, 7*cyChar/4, 
				hWnd, (HMENU) i, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;

		//������̬�ı���ؼ�
		hLabUsername = CreateWindow(TEXT("static"), TEXT("���ڱ�������"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE /*��ֱ����*/ |SS_RIGHT /*ˮƽ����*/, cxChar+BOARD_WIDTH+ BOARD_EDGE /*x����*/, cyChar * (1 + 2 * 11) /*y����*/, 100/*���*/, 26 /*�߶�*/, hWnd /*�����ھ��*/, (HMENU)1 /*�ؼ�ID*/, wforms.hInst /*��ǰ����ʵ�����*/, NULL	);

		//���������ı���ؼ�
		hEditUsername = CreateWindow(TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_AUTOHSCROLL /*ˮƽ����*/, cxChar+BOARD_WIDTH+ BOARD_EDGE /*x����*/, cyChar * (1 + 2 * 12) /*y����*/, 100, 26,	hWnd, (HMENU)3,  wforms.hInst, NULL	);


		// ��������λ�úͳߴ�
		GetWindowRect(hWnd, &rect);
		x = rect.left;
		y = rect.top;
		rect.right = rect.left + BOARD_WIDTH + BOARD_EDGE + 135;
		rect.bottom = rect.top + TOOLBAR_SIZE + BOARD_HEIGHT + BOARD_EDGE*5;
		AdjustWindowRect(&rect, WINDOW_STYLES, TRUE);
		MoveWindow(hWnd, x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		break;

	case WM_SIZE:
        {  
            HWND hTool;  
			hTool=GetDlgItem(wforms.hWnd,IDC_MAIN_TOOL);  
            SendMessage(hTool,TB_AUTOSIZE,0,0);  
            MoveWindow(hWndStatus,0,0,0,0,TRUE);  
        }  

		//cxChar = LOWORD(lParam);
		//cyChar = HIWORD(lParam);
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

		// �˳�
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// �˵�����
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case ID_FILE_NEW:  
			MessageBox(wforms.hWnd,TEXT("notify"),TEXT("file new"),0);  
			break;  
		case ID_FILE_OPEN:  
			MessageBox(wforms.hWnd,TEXT("notify"),TEXT("file open"),0);  
			break;  
		case ID_FILE_SAVEAS:  
			MessageBox(wforms.hWnd,TEXT("notify"),TEXT("file save as"),0);  
			break;  

		//case 5:
		//	//��ȡ����������
		//	GetWindowText(hEditUsername, szUsername, 100);
		//	GetWindowText(hEditPassword, szPassword, 100);
		//	wsprintf(szUserInfo, TEXT("�����û��˺ţ�%s\r\n�����û����룺%s"), szUsername, szPassword);
		//	MessageBox(hWnd, szUserInfo, TEXT("��Ϣ��ʾ"), MB_ICONINFORMATION);
		//	break;
		case BS_PUSHBUTTON:
			strcpy_s(buffer, "BS_PUSHBUTTON");
			rect.left   = 0 ;
			rect.top    = 0 ;
			rect.right  = BOARD_WIDTH ;
			rect.bottom = 100;
			InvalidateRect (wforms.hWnd, &rect, TRUE) ;
			break;
		case BS_AUTO3STATE:
			strcpy_s(buffer, "BS_AUTO3STATE");
			rect.left   = 0 ;
			rect.top    = 0 ;
			rect.right  = BOARD_WIDTH ;
			rect.bottom = 100;
			InvalidateRect (wforms.hWnd, &rect, TRUE) ;
			break;
		case BS_RADIOBUTTON:
			strcpy_s(buffer, "BS_RADIOBUTTON");
			rect.left   = 0 ;
			rect.top    = 0 ;
			rect.right  = BOARD_WIDTH ;
			rect.bottom = 100;
			InvalidateRect (wforms.hWnd, &rect, TRUE) ;
			break;
		case IDM_FILE_RED:
		case IDM_FILE_BLACK:
			wforms.bFlipped = (LOWORD(wParam) == IDM_FILE_BLACK);
			Startup();
			hdc = GetDC(wforms.hWnd);
			DrawBoard(hdc);
			if (wforms.bFlipped) {
				wforms.hdc = hdc;
				wforms.hdcTmp = CreateCompatibleDC(wforms.hdc);
				ResponseMove();
				DeleteDC(wforms.hdcTmp);
			}
			ReleaseDC(wforms.hWnd, hdc);
			break;
		case IDM_FILE_EXIT:
			DestroyWindow(wforms.hWnd);
			break;
		case IDM_HELP_ABOUT:

			ShellAbout(hWnd,"������Ȥ","������Ȥ",wforms.hIcon);    

			/*	MessageBeep(MB_ICONINFORMATION);
			   mbp.cbSize = sizeof(MSGBOXPARAMS);
			   mbp.hwndOwner = hWnd;
			   mbp.hInstance = wforms.hInst;
			   mbp.lpszText = cszAbout;
			   mbp.lpszCaption = "������Ȥ";
			   mbp.dwStyle = MB_USERICON;
			   mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
			   mbp.dwContextHelpId = 0;
			   mbp.lpfnMsgBoxCallback = NULL;
			   mbp.dwLanguageId = 0;
			   MessageBoxIndirect(&mbp);*/
			break;
		}
		break;
		// ��ͼ
	case WM_PAINT:
		hdc = BeginPaint(wforms.hWnd, &ps);
		
		DrawBoard(hdc);
		//DrawText (hdc, TEXT ("Hello, Windows 98!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

		//�����¼�/////////////////////////////////////
		//hbitmap = LoadImage(NULL,"..\\res\\rp.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);  
		//Hdcmem = CreateCompatibleDC(hdc);  
		//hbmp= LoadResBmp(IDB_RP);
		//DrawTransBmp(wforms.hdc, hdcmem, 33, 33,hbmp);
		//DrawTransBmp(wforms.hdc, hdcmem, 0, 0, (HBITMAP)hbitmap);

		EndPaint(wforms.hWnd, &ps);
		break;
		// �����
	case WM_LBUTTONDOWN:
		x = FILE_LEFT + (LOWORD(lParam) - (BOARD_EDGE + LBrook_LEFT - SQUARE_SIZE/2 )) / SQUARE_SIZE;
		y = RANK_TOP + (HIWORD(lParam) - (BOARD_EDGE  + LBrook_TOP + TOOLBAR_SIZE - SQUARE_SIZE/2)) / SQUARE_SIZE;
		if (x >= FILE_LEFT && x <= FILE_RIGHT && y >= RANK_TOP && y <= RANK_BOTTOM) 
		{
			ClickSquare(COORD_XY(x, y));
			SetCapture(wforms.hWnd);
			SetCursor(wforms.hCursor);
		}
		break;

	case    WM_MOUSEMOVE:  
        wsprintf(szBuf,"Mouse %d,%d",LOWORD(lParam),HIWORD(lParam));  
        SendMessage(hWndStatus,SB_SETTEXT,0,(LPARAM)(LPSTR)szBuf);  
        break;  
	case WM_LBUTTONUP:
		ReleaseCapture();
		break;
		// �����¼�
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
#ifdef _DEBUG
	//change_piece_size("E:\\private\\��������3.6\\������������\\���Ӵ�ȫ-�ػ����ṩ\\3D��������(���)", "C:\\deleted\\link_vc2010\\RES\\",PIECE_SIZE );
	//change_piece_size("E:\\��������360\\������������\\���Ӵ�ȫ-�ػ����ṩ\\3D��������(���)", "G:\\link_vc2010_office\\RES\\",PIECE_SIZE);		//home
#endif
	int i;
	MSG msg;
	WNDCLASSEX wce;

	// ��ʼ��ȫ�ֱ���
	srand((DWORD) time(NULL));
	//InitZobrist();
	wforms.hInst = hInstance;
	LoadBook(hInstance);
	wforms.bFlipped = FALSE;
	Startup();

	// װ��ͼƬ
	wforms.bmpBoard = LoadResBmp(IDB_BOARD);
	wforms.bmpSelected = LoadResBmp(IDB_SELECTED);
	for (i = PIECE_KING; i <= PIECE_PAWN; i ++) {
		wforms.bmpPieces[SIDE_TAG(0) + i] = LoadResBmp(IDB_RK + i);
		wforms.bmpPieces[SIDE_TAG(1) + i] = LoadResBmp(IDB_BK + i);
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
	wforms.hIcon = wce.hIcon;
	//wforms.hCursor=  SetCursor(LoadCursor(wforms.hInst, MAKEINTRESOURCE(IDC_CURSOR_HAND)));
	//wforms.hCursor = LoadCursorFromFile("hand.cur");
	wforms.hCursor = LoadResCur(IDC_CURSOR_HAND);

	wforms.hWnd = CreateWindow("JQXQ", "��Ȥ����", WINDOW_STYLES,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (wforms.hWnd == NULL) {
		return 0;
	}
	ShowWindow(wforms.hWnd, nCmdShow);
	UpdateWindow(wforms.hWnd);

	// ������Ϣ
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	}
	return msg.wParam;
}

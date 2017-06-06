#ifndef TOOLSYS_H_INCLUDE
#define TOOLSYS_H_INCLUDE

extern const int MASK_COLOR;
extern const int SQUARE_SIZE;
extern const int PIECE_SIZE;
extern const int LBrook_LEFT;		//�������ϽǺڳ�λ��X���������ͼ�񣩣�����55
extern const int LBrook_TOP;		//�������ϽǺڳ�λ��Y���������ͼ�񣩣�����70

extern const int BOARD_EDGE;
extern const int BOARD_WIDTH;
extern const int BOARD_HEIGHT;
extern const bool DRAW_SELECTED;
extern const int ScreenOffX;
extern const int ScreenOffY;
		
// ��ͼ�ν����йص�ȫ�ֱ���
struct WINFORMSTRUCT{
	HINSTANCE hInst;                              // Ӧ�ó�����ʵ��
	HWND hWnd;                                    // �����ھ��
	HDC hdc, hdcTmp;                              // �豸�����ֻ��"ClickSquare"��������Ч
	HBITMAP bmpBoard, bmpSelected, bmpPieces[24]; // ��ԴͼƬ���
	int sqSelected, mvLast;                       // ѡ�еĸ��ӣ���һ����
	BOOL bFlipped, bGameOver;                     // �Ƿ�ת���̣��Ƿ���Ϸ����(���ü�������ȥ)
	HICON hIcon;
	HCURSOR hCursor;
};			
extern WINFORMSTRUCT wforms;

void MessageBoxMute(LPCSTR lpszText);

void DrawBoard(HDC hdc);
void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp);
void DrawSquare(int sq, BOOL bSelected = FALSE);//���Ƹ���
void ResponseMove(void);
void ClickSquare(int sq);
void Startup(void);

// ������Դ����
inline void PlayResWav(int nResId) {
	PlaySound(MAKEINTRESOURCE(nResId), wforms.hInst, SND_ASYNC | SND_NOWAIT | SND_RESOURCE);
}

// װ����ԴͼƬ
inline HBITMAP LoadResBmp(int nResId) {
	return (HBITMAP) LoadImage(wforms.hInst, MAKEINTRESOURCE(nResId), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
}

inline HCURSOR LoadResCur(int nResId) {
	return (HCURSOR) LoadImage(wforms.hInst, MAKEINTRESOURCE(nResId), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
}

#endif
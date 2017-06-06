#ifndef POSITIONSTRUCT_H_INCLUDED
#define POSITIONSTRUCT_H_INCLUDED

// ���̷�Χ
const int RANK_TOP = 3;
const int RANK_BOTTOM = 12;
const int FILE_LEFT = 3;
const int FILE_RIGHT = 11;

// ���ӱ��
const int PIECE_KING = 0;
const int PIECE_ADVISOR = 1;
const int PIECE_BISHOP = 2;
const int PIECE_KNIGHT = 3;
const int PIECE_ROOK = 4;
const int PIECE_CANNON = 5;
const int PIECE_PAWN = 6;

// "GenerateMoves"����
const BOOL GEN_CAPTURE = TRUE;

// ��������
const int MAX_GEN_MOVES = 128; // ���������߷���
const int MAX_MOVES = 256;     // ������ʷ�߷���
const int LIMIT_DEPTH = 64;    // �����������
const int MATE_VALUE = 10000;  // ��߷�ֵ���������ķ�ֵ
const int BAN_VALUE = MATE_VALUE - 100; // �����и��ķ�ֵ�����ڸ�ֵ����д���û���
const int WIN_VALUE = MATE_VALUE - 200; // ������ʤ���ķ�ֵ���ޣ�������ֵ��˵���Ѿ�������ɱ����
const int DRAW_VALUE = 20;     // ����ʱ���صķ���(ȡ��ֵ)
const int ADVANCED_VALUE = 3;  // ����Ȩ��ֵ
const int RANDOM_MASK = 7;     // ����Է�ֵ
const int NULL_MARGIN = 400;   // �ղ��ü��������߽�
const int NULL_DEPTH = 2;      // �ղ��ü��Ĳü����
const int HASH_SIZE = 1 << 20; // �û����С
const int HASH_ALPHA = 1;      // ALPHA�ڵ���û�����
const int HASH_BETA = 2;       // BETA�ڵ���û�����
const int HASH_PV = 3;         // PV�ڵ���û�����
const int BOOK_SIZE = 16384;   // ���ֿ��С

extern const char ccInBoard[256];
extern const char ccInFort[256];
extern const char ccLegalSpan[512];
extern const char ccKnightPin[512];
extern const char ccKingDelta[4];
extern const char ccAdvisorDelta[4];
extern const char ccKnightDelta[4][2];
extern const char ccKnightCheckDelta[4][2];
extern const BYTE cucpcStartup[256];
extern const BYTE cucvlPiecePos[7][256];

/////////////////////////////////////////////////////////////////////////
// �ж������Ƿ���������
inline BOOL IN_BOARD(int sq) {
	return ccInBoard[sq] != 0;
}

// �ж������Ƿ��ھŹ���
inline BOOL IN_FORT(int sq) {
	return ccInFort[sq] != 0;
}

// ��ø��ӵĺ�����
inline int RANK_Y(int sq) {
	return sq >> 4;
}

// ��ø��ӵ�������
inline int FILE_X(int sq) {
	return sq & 15;
}

// ����������ͺ������ø���
inline int COORD_XY(int x, int y) {
	return x + (y << 4);
}

// ��ת����
inline int SQUARE_FLIP(int sq) {
	return 254 - sq;
}

// ������ˮƽ����
inline int FILE_FLIP(int x) {
	return 14 - x;
}

// �����괹ֱ����
inline int RANK_FLIP(int y) {
	return 15 - y;
}

// ����ˮƽ����
inline int MIRROR_SQUARE(int sq) {
	return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

// ����ˮƽ����
inline int SQUARE_FORWARD(int sq, int sd) {
	return sq - 16 + (sd << 5);
}

// �߷��Ƿ����˧(��)�Ĳ���
inline BOOL KING_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 1;
}

// �߷��Ƿ������(ʿ)�Ĳ���
inline BOOL ADVISOR_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 2;
}

// �߷��Ƿ������(��)�Ĳ���
inline BOOL BISHOP_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 3;
}

// ��(��)�۵�λ��
inline int BISHOP_PIN(int sqSrc, int sqDst) {
	return (sqSrc + sqDst) >> 1;
}

// ���ȵ�λ��
inline int KNIGHT_PIN(int sqSrc, int sqDst) {
	return sqSrc + ccKnightPin[sqDst - sqSrc + 256];
}

// �Ƿ�δ����
inline BOOL HOME_HALF(int sq, int sd) {
	return (sq & 0x80) != (sd << 7);
}

// �Ƿ��ѹ���
inline BOOL AWAY_HALF(int sq, int sd) {
	return (sq & 0x80) == (sd << 7);
}

// �Ƿ��ںӵ�ͬһ��
inline BOOL SAME_HALF(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x80) == 0;
}

// �Ƿ���ͬһ��
inline BOOL SAME_RANK(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0xf0) == 0;
}

// �Ƿ���ͬһ��
inline BOOL SAME_FILE(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x0f) == 0;
}

// ��ú�ڱ��(������8��������16)
inline int SIDE_TAG(int sd) {
	return 8 + (sd << 3);
}

// ��öԷ���ڱ��
inline int OPP_SIDE_TAG(int sd) {
	return 16 - (sd << 3);
}

// ����߷������
inline int SRC(int mv) {
	return mv & 255;
}

// ����߷����յ�
inline int DST(int mv) {
	return mv >> 8;
}

// ���������յ����߷�
inline int MOVE(int sqSrc, int sqDst) {
	return sqSrc + sqDst * 256;
}

// �߷�ˮƽ����
inline int MIRROR_MOVE(int mv) {
	return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
}

// RC4������������
struct RC4Struct {
	BYTE s[256];
	int x, y;

	void InitZero(void);   // �ÿ���Կ��ʼ��������������
	BYTE NextByte(void) {  // ��������������һ���ֽ�
		BYTE uc;
		x = (x + 1) & 255;
		y = (y + s[x]) & 255;
		uc = s[x];
		s[x] = s[y];
		s[y] = uc;
		return s[(s[x] + s[y]) & 255];
	}
	DWORD NextLong(void) { // ���������������ĸ��ֽ�
		BYTE uc0, uc1, uc2, uc3;
		uc0 = NextByte();
		uc1 = NextByte();
		uc2 = NextByte();
		uc3 = NextByte();
		return uc0 + (uc1 << 8) + (uc2 << 16) + (uc3 << 24);
	}
};


// �û�����ṹ
struct HashItem {
	BYTE ucDepth, ucFlag;
	short svl;
	WORD wmv, wReserved;
	DWORD dwLock0, dwLock1;
};

// ���ֿ���ṹ
struct BookItem {
	DWORD dwLock;
	WORD wmv, wvl;
};

struct SEARCH{
	int mvResult;                  // �����ߵ���
	int nHistoryTable[65536];      // ��ʷ��
	int mvKillers[LIMIT_DEPTH][2]; // ɱ���߷���
	HashItem HashTable[HASH_SIZE]; // �û���
	int nBookSize;                 // ���ֿ��С
	BookItem BookTable[BOOK_SIZE]; // ���ֿ�
} ;

// Zobrist�ṹ
struct ZobristStruct {
	DWORD dwKey, dwLock0, dwLock1;

	void InitZero(void) {                 // �������Zobrist
		dwKey = dwLock0 = dwLock1 = 0;
	}
	void InitRC4(RC4Struct &rc4) {        // �����������Zobrist
		dwKey = rc4.NextLong();
		dwLock0 = rc4.NextLong();
		dwLock1 = rc4.NextLong();
	}
	void Xor(const ZobristStruct &zobr) { // ִ��XOR����
		dwKey ^= zobr.dwKey;
		dwLock0 ^= zobr.dwLock0;
		dwLock1 ^= zobr.dwLock1;
	}
	void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2) {
		dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
		dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
		dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
	}
};

// Zobrist��
struct ZOBRIST{
	ZobristStruct Player;
	ZobristStruct Table[14][256];
};

extern ZOBRIST Zobrist;

// ��ʷ�߷���Ϣ(ռ4�ֽ�)
struct MoveStruct {
	WORD wmv;
	BYTE ucpcCaptured, ucbCheck;
	DWORD dwKey;

	void Set(int mv, int pcCaptured, BOOL bCheck, DWORD dwKey_) {
		wmv = mv;
		ucpcCaptured = pcCaptured;
		ucbCheck = bCheck;
		dwKey = dwKey_;
	}
}; // mvs

// ����ṹ
struct Position {
	int sdPlayer;                   // �ֵ�˭�ߣ�0=�췽��1=�ڷ�
	BYTE ucpcSquares[256];          // �����ϵ�����
	int vlWhite, vlBlack;           // �졢��˫����������ֵ
	int nDistance, nMoveNum;        // ������ڵ�Ĳ�������ʷ�߷���
	MoveStruct mvsList[MAX_MOVES];  // ��ʷ�߷���Ϣ�б�
	ZobristStruct zobr;             // Zobrist

	void ClearBoard(void) {         // �������
		sdPlayer = vlWhite = vlBlack = nDistance = 0;
		memset(ucpcSquares, 0, 256);
		zobr.InitZero();
	}
	void SetIrrev(void) {           // ���(��ʼ��)��ʷ�߷���Ϣ
		mvsList[0].Set(0, 0, Checked(), zobr.dwKey);
		nMoveNum = 1;
	}
	void Startup(void);             // ��ʼ������
	void ChangeSide(void) {         // �������ӷ�
		sdPlayer = 1 - sdPlayer;
		zobr.Xor(Zobrist.Player);
	}
	void AddPiece(int sq, int pc) { // �������Ϸ�һö����
		ucpcSquares[sq] = pc;
		// �췽�ӷ֣��ڷ�(ע��"cucvlPiecePos"ȡֵҪ�ߵ�)����
		if (pc < 16) {
			vlWhite += cucvlPiecePos[pc - 8][sq];
			zobr.Xor(Zobrist.Table[pc - 8][sq]);
		} else {
			vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
			zobr.Xor(Zobrist.Table[pc - 9][sq]);
		}
	}
	void DelPiece(int sq, int pc) { // ������������һö����
		ucpcSquares[sq] = 0;
		// �췽���֣��ڷ�(ע��"cucvlPiecePos"ȡֵҪ�ߵ�)�ӷ�
		if (pc < 16) {
			vlWhite -= cucvlPiecePos[pc - 8][sq];
			zobr.Xor(Zobrist.Table[pc - 8][sq]);
		} else {
			vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
			zobr.Xor(Zobrist.Table[pc - 9][sq]);
		}
	}
	int Evaluate(void) const {      // �������ۺ���
		return (sdPlayer == 0 ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
	}
	BOOL InCheck(void) const {      // �Ƿ񱻽���
		return mvsList[nMoveNum - 1].ucbCheck;
	}
	BOOL Captured(void) const {     // ��һ���Ƿ����
		return mvsList[nMoveNum - 1].ucpcCaptured != 0;
	}
	int MovePiece(int mv);                      // ��һ���������
	void UndoMovePiece(int mv, int pcCaptured); // ������һ���������
	BOOL MakeMove(int mv);                      // ��һ����
	void UndoMakeMove(void) {                   // ������һ����
		nDistance --;
		nMoveNum --;
		ChangeSide();
		UndoMovePiece(mvsList[nMoveNum].wmv, mvsList[nMoveNum].ucpcCaptured);
	}
	void NullMove(void) {                       // ��һ���ղ�
		DWORD dwKey;
		dwKey = zobr.dwKey;
		ChangeSide();
		mvsList[nMoveNum].Set(0, 0, FALSE, dwKey);
		nMoveNum ++;
		nDistance ++;
	}
	void UndoNullMove(void) {                   // ������һ���ղ�
		nDistance --;
		nMoveNum --;
		ChangeSide();
	}
	// ���������߷������"bCapture"Ϊ"TRUE"��ֻ���ɳ����߷�
	int GenerateMoves(int *mvs, BOOL bCapture = FALSE) const;
	BOOL LegalMove(int mv) const;               // �ж��߷��Ƿ����
	BOOL Checked(void) const;                   // �ж��Ƿ񱻽���
	BOOL IsMate(void);                          // �ж��Ƿ�ɱ
	int DrawValue(void) const {                 // �����ֵ
		return (nDistance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
	}
	int RepStatus(int nRecur = 1) const;        // ����ظ�����
	int RepValue(int nRepStatus) const {        // �ظ������ֵ
		int vlReturn;
		vlReturn = ((nRepStatus & 2) == 0 ? 0 : nDistance - BAN_VALUE) +
			((nRepStatus & 4) == 0 ? 0 : BAN_VALUE - nDistance);
		return vlReturn == 0 ? DrawValue() : vlReturn;
	}
	BOOL NullOkay(void) const {                 // �ж��Ƿ�����ղ��ü�
		return (sdPlayer == 0 ? vlWhite : vlBlack) > NULL_MARGIN;
	}
	void Mirror(Position &posMirror) const; // �Ծ��澵��
};

int 	SearchMain(void) ;
void	LoadBook(HINSTANCE hInst);

extern Position pos;		// ����ʵ��

#endif
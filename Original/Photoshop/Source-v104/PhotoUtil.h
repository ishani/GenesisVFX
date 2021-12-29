float TextToFloat(char *);
void PutFloatInEdit(HWND,float,float);
void PutIntInEdit(HWND,int);
void CenterWindow(HWND,HWND);
void StatusCreate(HWND);
void StatusHide();
void StatusProgress(int,int);
void StatusText(char *);
void StatusKill();
void SimpleStatus(int,int);
int AbortIt();
unsigned int HardwareLockID();

COLORREF ChooseColor(char *,COLORREF);


typedef struct {
	unsigned short	r;
	unsigned short	g;
	unsigned short  b;
	unsigned short  alpha;
} BMM_Color_64;

typedef struct {
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
} RGBColour;

typedef struct {
	int				havemask;
	unsigned char*	mask;
	int				x0,y0,x1,y1;
	int				masklinelen;
	int				ImageMode;
	int				FilterCase;
	int				Height;
	int				Width;
	unsigned char*	InData;
	unsigned char*	OutData;
	int				InRowBytes;
	int				OutRowBytes;
	int				InPixelSize;
	int				OutPixelSize;
} MaskData;
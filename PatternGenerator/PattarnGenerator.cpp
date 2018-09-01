#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include "resource.h"
#include "Display.h"
#include <string>
using namespace std;

//#define DEBUG

enum Pattern
{
	BLACK,
	GRAY_25,
	GRAY_50,
	GRAY_75,
	WHITE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	CYAN,
	MAGENTA,
	MONOGRAD_16,
	MONOGRAD_32,
	MONOGRAD_64,
	MONOGRAD_128,
	MONOGRAD_256,
	COLORGRAD_16,
	COLORGRAD_32,
	COLORGRAD_64,
	COLORGRAD_128,
	COLORGRAD_256
};

static const int FIRST_PATTERN = BLACK;
static const int LAST_PATTERN = COLORGRAD_256;

static const int NUM_PATTERNS = LAST_PATTERN + 1;	

static const int NUM_DISPLAY = 4;						
static const int NUM_COLOR = 6;

static const int divide_16 = 16;
static const int divide_32 = 32;
static const int divide_64 = 64;
static const int divide_128 = 128;
static const int divide_256 = 256;

HINSTANCE g_hInst;              
HWND g_hDlg;                    
int patternType=FIRST_PATTERN;	
RECT clientRect;				
int	monitorCount;				
Display *display[NUM_DISPLAY];

HWND Create(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MonitorEnumProc(
	HMONITOR ,
	HDC,
	LPRECT,
	LPARAM
	);
void Draw(HWND) ;
void drawSolidPattern(HDC,int,int,int);
void drawMonoGradPattern(HDC, RECT *, const int);
void drawColorGradPattern(HDC, RECT *, const int,const int);
void searchMonitor();
void DebugMessage(const char *pFormat, ...);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szSTR, int iCmdShow){

	HWND hWnd ;
	MSG msg ;

	for(int i = 0; i < NUM_DISPLAY; i++){
		display[i] = new Display();
	}

	g_hInst = hInst;

	hWnd = Create( hInst );

	ShowWindow (hWnd,SW_SHOW) ;
	UpdateWindow( hWnd );

	while( 1 )
	{
		BOOL ret = GetMessage( &msg, NULL, 0, 0 );	
		if( ret == 0 || ret == -1 )
		{
			break;
		}
		else
		{
			if( g_hDlg == NULL || !IsDialogMessage( g_hDlg, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
	}
	return 0;

}

HWND Create(HINSTANCE hInst)
{
	WNDCLASSEX  wndclass ;
	wndclass.cbSize = sizeof (wndclass) ;
	wndclass.style = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc = WndProc ;
	wndclass.cbClsExtra = 0 ;
	wndclass.cbWndExtra = 0 ;
	wndclass.hInstance = hInst ;
	wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW) ;				
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH) ;	
	wndclass.lpszMenuName = NULL ;									
	wndclass.lpszClassName = "Pattern Generator" ;
	wndclass.hIconSm = LoadIcon (NULL, IDI_APPLICATION) ;
	RegisterClassEx (&wndclass) ;

	int x = GetSystemMetrics(SM_CXSCREEN);							
	int y = GetSystemMetrics(SM_CYSCREEN);							

	searchMonitor();												
	return CreateWindow (wndclass.lpszClassName, wndclass.lpszClassName,WS_VISIBLE|WS_POPUP, 0, 0, x, y, NULL, NULL, hInst, NULL);
}


LRESULT CALLBACK WndProc( HWND hWnd, UINT iMsg, WPARAM wParam,
						 LPARAM lParam)
{	
	switch(iMsg)
	{
	case WM_CREATE:
		g_hDlg = CreateDialog( g_hInst,MAKEINTRESOURCE(IDD_DIALOG1), hWnd,DialogProc);		
		ShowWindow( g_hDlg, SW_SHOW );
		break;

	case WM_LBUTTONUP:		
		ShowWindow( g_hDlg, SW_SHOW );
		break;

	case WM_DESTROY:		
		DestroyWindow( g_hDlg );
		PostQuitMessage(0) ;
		break ;

	case WM_PAINT:
		Draw(hWnd) ;
		break ;
	case WM_KEYDOWN:			
		switch((CHAR)wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0) ;
			break ;
		}
		break;
	}
	return DefWindowProc (hWnd, iMsg, wParam, lParam) ;
}


void Draw(HWND hWnd)
{
	HDC hdc ;
	PAINTSTRUCT ps ;
	hdc = BeginPaint( hWnd, &ps ) ;

	GetClientRect(hWnd , &clientRect);

	#ifdef DEBUG
	DebugMessage("clientwidth＝%ld\n",clientRect.right);
	#endif

	RECT gradRect16[16];
	RECT gradRect32[32];
	RECT gradRect64[64];
	RECT gradRect128[128];
	RECT gradRect256[256];

	switch(patternType)
	{
	case BLACK:			
		SetDlgItemText(g_hDlg, IDC_STATIC1, "BLACK");
		drawSolidPattern(hdc,0,0,0);
		break;

	case GRAY_25:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "GRAY25%");
		drawSolidPattern(hdc,64,64,64);
		break;

	case GRAY_50:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "GRAY50%");
		drawSolidPattern(hdc,128,128,128);
		break;

	case GRAY_75:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "GRAY75%");
		drawSolidPattern(hdc,192,192,192);
		break;

	case WHITE:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "WHITE");
		drawSolidPattern(hdc,255,255,255);
		break;

	case RED:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "RED");
		drawSolidPattern(hdc,255,0,0);
		break;

	case GREEN:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "GREEN");
		drawSolidPattern(hdc,0,255,0);
		break;

	case BLUE:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "BLUE");
		drawSolidPattern(hdc,0,0,255);
		break;

	case YELLOW:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "YELLOW");
		drawSolidPattern(hdc,255,255,0);
		break;

	case CYAN:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "CYAN");
		drawSolidPattern(hdc,0,255,255);
		break;

	case MAGENTA:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "MAGENTA");
		drawSolidPattern(hdc,255,0,255);
		break;

	case MONOGRAD_16:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "MONOCHROME GRADATION 16STEPS");
		drawMonoGradPattern(hdc, gradRect16, divide_16);
		break;

	case MONOGRAD_32:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "MONOCHROME GRADATION 32STEPS");
		drawMonoGradPattern(hdc, gradRect32, divide_32);
		break;

	case MONOGRAD_64:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "MONOCHROME GRADATION 64STEPS");
		drawMonoGradPattern(hdc, gradRect64, divide_64);
		break;

	case MONOGRAD_128:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "MONOCHROME GRADATION 128STEPS");
		drawMonoGradPattern(hdc, gradRect128, divide_128);
		break;

	case MONOGRAD_256:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "MONOCHROME GRADATION 256STEPS");
		drawMonoGradPattern(hdc, gradRect256, divide_256);		
		break;

	case COLORGRAD_16:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "COLOR GRADATION 16STEPS");
		drawColorGradPattern(hdc, gradRect16, divide_16,6);
		break;

	case COLORGRAD_32:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "COLOR GRADATION 32STEPS");
		drawColorGradPattern(hdc, gradRect32, divide_32,6);
		break;

	case COLORGRAD_64:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "COLOR GRADATION 64STEPS");
		drawColorGradPattern(hdc, gradRect64, divide_64,6);
		break;

	case COLORGRAD_128:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "COLOR GRADATION 128STEPS");
		drawColorGradPattern(hdc, gradRect128, divide_128,6);
		break;

	case COLORGRAD_256:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "COLOR GRADATION 256STEPS");
		drawColorGradPattern(hdc, gradRect256, divide_256,6);
		break;

	default:
		SetDlgItemText(g_hDlg, IDC_STATIC1, "UNKNOWN PATTERN");
		drawSolidPattern(hdc,0,0,0);
		break;
	}
	EndPaint( hWnd, &ps);
	return ;
}

void drawSolidPattern(HDC hdc,int r,int g,int b){
	HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));
	RECT solidRect;		
	solidRect.left =  clientRect.left;
	solidRect.top =   clientRect.top;
	solidRect.right =  clientRect.right;
	solidRect.bottom = clientRect.bottom;
	FillRect(hdc, &solidRect,hBrush);
	DeleteObject(hBrush);
}

void drawMonoGradPattern(HDC hdc, RECT *gradRect, const int divide){
	double gradWidth = (clientRect.right)/(double)divide;																	//グラデーション１つ分の幅
	DebugMessage("gradWidth=%lf\n",gradWidth);
	for(int i = 0; i < divide; i++){
		HBRUSH hBrush = CreateSolidBrush(RGB(i*(255/((double)(divide-1))), i*(255/((divide-1))), i*(255/((divide-1)))));	//ブラシの生成と色決め
		gradRect[i].left = clientRect.left+(gradWidth*i);																	//グラデーション１つ分の左端
		gradRect[i].top = clientRect.top;
		gradRect[i].right = (clientRect.left+(gradWidth*i))+gradWidth;														//グラデーション１つ分の右端
		gradRect[i].bottom = clientRect.bottom;
		FillRect(hdc, &gradRect[i],hBrush);
		DeleteObject(hBrush);

		#ifdef DEBUG
		DebugMessage("loop=%d\n",i+1);
		DebugMessage("gradientright=%lf\n",(clientRect.left+(gradWidth*i))+gradWidth);
		DebugMessage("color=%lf\n",i*(255/(double)(divide-1)));
		#endif
	}
}

void drawColorGradPattern(HDC hdc, RECT *gradRect, const int divide, const int numcolor){
	double gradWidth = (clientRect.right)/(double)divide;
	double gradHeight = (clientRect.bottom)/(double)(numcolor*2);

	#ifdef DEBUG
	DebugMessage("gradWidth=%lf\n",gradWidth);
	#endif

	HBRUSH hBrush;

	for(int j = 0; j < numcolor*2; j++){
		for(int i = 0; i < divide; i++){
			switch(j){
			case 0://red->white
				hBrush = CreateSolidBrush(RGB(255, i*(255/((divide-1))), i*(255/((divide-1)))));
				break;
			case 1://green->white
				hBrush = CreateSolidBrush(RGB(i*(255/((divide-1))), 255, i*(255/((divide-1)))));
				break;
			case 2://blue->white
				hBrush = CreateSolidBrush(RGB(i*(255/((divide-1))), i*(255/((divide-1))), 255));
				break;
			case 3://yellow->white
				hBrush = CreateSolidBrush(RGB(255, 255, i*(255/((divide-1)))));
				break;
			case 4://cyan->white
				hBrush = CreateSolidBrush(RGB(i*(255/((divide-1))), 255, 255));
				break;
			case 5://magenta->white
				hBrush = CreateSolidBrush(RGB(255, i*(255/((divide-1))), 255));
				break;
			case 6://black->red
				hBrush = CreateSolidBrush(RGB(i*(255/((divide-1))), 0, 0));
				break;
			case 7://black->green
				hBrush = CreateSolidBrush(RGB(0, i*(255/((divide-1))), 0));
				break;
			case 8://black->blue
				hBrush = CreateSolidBrush(RGB(0, 0, i*(255/((divide-1)))));
				break;
			case 9://black->yellow
				hBrush = CreateSolidBrush(RGB(i*(255/((divide-1))), i*(255/((divide-1))), 0));
				break;
			case 10://black->cyan
				hBrush = CreateSolidBrush(RGB(0,i*(255/((divide-1))), i*(255/((divide-1)))));
				break;
			case 11://black->magenta
				hBrush = CreateSolidBrush(RGB(i*(255/((divide-1))), 0, i*(255/((divide-1)))));
				break;
			default:
				hBrush = CreateSolidBrush(RGB(0, 0, 0));
				break;
			}

			gradRect[i].left = clientRect.left+(gradWidth*i);
			gradRect[i].top = clientRect.top+(gradHeight*j);
			gradRect[i].right = (clientRect.left+(gradWidth*i))+gradWidth;
			gradRect[i].bottom = (clientRect.bottom+(gradHeight*i))+gradHeight;
			FillRect(hdc, &gradRect[i],hBrush);
			DeleteObject(hBrush);

			#ifdef DEBUG
			DebugMessage("loop=%d\n",i+1);
			DebugMessage("gradientright=%lf\n",(clientRect.left+(gradWidth*i))+gradWidth);
			DebugMessage("color=%lf\n",i*(255/(double)(divide-1)));
			#endif
		}
	}	
}

BOOL CALLBACK MonitorEnumProc(
	HMONITOR hMonitor,
	HDC hdcMonitor,
	LPRECT lprcMonitor,
	LPARAM dwData 
	) {
		//char	sStr[MAX_PATH];
		MONITORINFOEX monitorInfo;

		monitorInfo.cbSize = sizeof(MONITORINFOEX);		
		GetMonitorInfo(hMonitor,&monitorInfo); 

		if(monitorCount<=NUM_DISPLAY){
		display[monitorCount]->setDisplayArea(monitorInfo.rcMonitor.bottom,monitorInfo.rcMonitor.left,monitorInfo.rcMonitor.right,monitorInfo.rcMonitor.top);
		display[monitorCount]->setName(monitorInfo.szDevice);
		monitorCount++;
		}
		//MessageBox(NULL, sStr, "Info", MB_OK);
		return TRUE;
}

void searchMonitor() {
	monitorCount = 0;

	if(EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0)==FALSE){
		MessageBox(NULL, "Display was not found.", "Info", MB_OK| MB_ICONINFORMATION);
		PostQuitMessage(0) ; 
	}
	return;
}

BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT wmId;
	HWND hWnd = FindWindow("Pattern Generator", NULL);

	HWND hCombo;
	int comboId;
	char	itemName[MAX_PATH];
	//WPARAM index;
	hCombo = GetDlgItem(hDlg, IDC_COMBO1);

	switch (msg)
	{
	case WM_INITDIALOG:
		for (int i = 0 ; i < monitorCount ; i++)
		{
			sprintf_s(itemName, "%d: %s",i+1,display[i]->getName().c_str());
			SendMessage(hCombo , CB_ADDSTRING , 0 , (LPARAM)itemName);
		}
		SendMessage(hCombo, CB_SETCURSEL, 0, 0);
		return TRUE;

	case WM_CLOSE:		
		ShowWindow( g_hDlg, SW_HIDE );	
		DestroyWindow( g_hDlg );
		PostQuitMessage(0) ;
		return TRUE;
		/*case IDOK:
		EndDialog(g_hDlg, IDCANCEL);
		ShowWindow( g_hDlg, SW_HIDE);
		break;*/

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		switch(wmId)
		{
		case IDC_BUTTON1:
			if(patternType==FIRST_PATTERN)
				patternType = LAST_PATTERN;
			else
				patternType -= 1;
			InvalidateRect(hWnd , NULL , TRUE);
			break;
		case IDC_BUTTON2:
			if(patternType==LAST_PATTERN)
				patternType = FIRST_PATTERN;
			else
				patternType += 1;
			InvalidateRect(hWnd , NULL , TRUE);
			break;
		case IDC_BUTTON3:
			ShowWindow( g_hDlg, SW_HIDE );
			break;
		case IDC_COMBO1:
			if(HIWORD(wParam)==CBN_SELCHANGE){
				comboId = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
				SetWindowPos( hWnd, NULL, display[comboId]->getLeft(), display[comboId]->getTop(), display[comboId]->getRight()-display[comboId]->getLeft(), display[comboId]->getBottom()-display[comboId]->getTop(), (SWP_NOZORDER) );

			}
			break;
		}
		break;
	}
	return FALSE;
}


void DebugMessage(const char *pFormat, ...)
{
	char    szBuff[1024];
	va_list argptr;

	va_start(argptr, pFormat);
	vsprintf_s(szBuff, pFormat, argptr);
	va_end(argptr);

	OutputDebugString(szBuff);

	return;
}

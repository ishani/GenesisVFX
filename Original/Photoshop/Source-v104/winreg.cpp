#define STRICT
#include <windows.h>
#include <stdio.h>
#include "winres.h"

int CodeValid=FALSE;
char user[256];
char code[256];
char key[256];
char fname[256];

int type;

UINT incode,keycode,serial;


BOOL CheckCode()
{
	sscanf(code,"%d",&incode);

	return TRUE;
}

BOOL WriteEntry()
{
	FILE *out;
	
	out=fopen("serialPS.dat","a+");

	if( out==NULL ) {
		MessageBox(NULL,"Cannot open file!","Error",MB_OK);
		return TRUE;
	}

	fprintf(out,"Code:%d Key:%s Registered to:%s\n",incode,key,user);
	fclose(out);

	return TRUE;
}

BOOL CALLBACK WinRegProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message) {
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDC_USER:
		case IDC_INCODE:
			if(CodeValid) {
				CodeValid=FALSE;
				EnableWindow(GetDlgItem(hwnd,IDC_NUMTEXT),FALSE);
				EnableWindow(GetDlgItem(hwnd,IDC_KEY),FALSE);
				EnableWindow(GetDlgItem(hwnd,IDC_REGISTER),TRUE);
			}
			break;
		case IDC_REGISTER:
			GetDlgItemText(hwnd,IDC_INCODE,code,256);
			GetDlgItemText(hwnd,IDC_USER,user,256);
			if( CheckCode() ) {
					EnableWindow(GetDlgItem(hwnd,IDC_NUMTEXT),TRUE);
					EnableWindow(GetDlgItem(hwnd,IDC_KEY),TRUE);
					EnableWindow(GetDlgItem(hwnd,IDC_REGISTER),FALSE);

					keycode=incode;
					srand(keycode^0xafbabafa);
					
					for(int i=0;i!=10;i++)
						{ keycode^=0x12577345; keycode^=(unsigned long int)rand();
						keycode^=(unsigned long int)(rand()<<16);
						}
					sprintf(key,"%10.10u",keycode);

					SetDlgItemText(hwnd,IDC_KEY,key);
					CodeValid=TRUE;
					WriteEntry();
			}
			break;
		case IDCANCEL:
			EndDialog(hwnd,0);
			break;
		}
		return TRUE;
	}
	return FALSE;

}










int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR CmdLine,int Show)
{
	DialogBox(hInst,MAKEINTRESOURCE(IDD_DIALOG),NULL,WinRegProc);
	return TRUE;
}

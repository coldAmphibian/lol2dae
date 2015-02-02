// lol2dae.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "lol2dae.h"

#include <commdlg.h>

#include "FileLocation.h"
#include "SknVertex.h"
#include "SknImporter.h"
#include "SklImporter.h"
#include "ColladaWriter.h"
#define MAX_LOADSTRING 100

using namespace std;

// Global Variables:
FileLocation inLocation, outLocation, sklLocation;

SknImporter inputSkn;
ColladaWriter outputCollada;
SklImporter inputSkl;

HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

bool displayLogo(HDC& hWinDC, RECT& clientRect)
{
	HBITMAP hBitmap;
	hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_LOGO));

	HDC hLocalDC;
	hLocalDC = CreateCompatibleDC(hWinDC);

	BITMAP qBitmap;
	int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));

	HBITMAP hOldBmp = (HBITMAP)SelectObject(hLocalDC, hBitmap);
	int startX = ((clientRect.right) - (qBitmap.bmWidth)) / 2;
	BOOL qRetBlit = BitBlt(hWinDC, startX, 15, qBitmap.bmWidth, qBitmap.bmHeight, hLocalDC, 0, 0, SRCCOPY);

	SelectObject(hLocalDC, hOldBmp);
	DeleteDC(hLocalDC);
	DeleteObject(hBitmap);
	return true;
}

HFONT getFont() {
	HFONT defaultFont;

	HDC hdc = GetDC(NULL);
	int nHeight = -MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72);

	defaultFont = CreateFont(nHeight, 0, 0, 0, 400, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 0, _T("Sergoe UI"));
	if (defaultFont == NULL)
	{
		defaultFont = CreateFont(nHeight, 0, 0, 0, 400, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 0, _T("Lucida Sans Unicode"));
		if (defaultFont == NULL)
		{
			defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		}
	}
	return defaultFont;
}

void mbError(HWND& hWnd, TCHAR* errorMessage)
{
	MessageBox(hWnd, errorMessage, _T("Error"), MB_ICONERROR);
}

void mbInfo(HWND& hWnd, TCHAR* infoMessage)
{
	MessageBox(hWnd, infoMessage, _T("Information"), MB_ICONINFORMATION);
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LOL2DAE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LOL2DAE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOL2DAE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground =  CreateSolidBrush(RGB(128, 128, 128));
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LOL2DAE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_LOL2DAE));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, 0, 500, 325, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	HWND hEditInput;
	HWND hEditOutput;
	HWND hSelectInputButton;
	HWND hSelectOutputButton;
	HWND hConvertButton;
	HWND hExitButton;
	static HWND hCheckBox;

	RECT clientRect;
	RECT textRect;

	OPENFILENAME ofn;

	switch (message)
	{
	case WM_CTLCOLOREDIT:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)CreateSolidBrush(RGB(221, 221, 221));
		break;

	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)CreateSolidBrush(RGB(128, 128, 128));
		break;

	case WM_CREATE:
		hEditInput = CreateWindow(_T("edit"), inLocation.Path, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 115, 100, 275, 24, hWnd, (HMENU)135, NULL, NULL);
		SendMessage(hEditInput, WM_SETFONT, (WPARAM)getFont(), TRUE);

		hEditOutput = CreateWindow(_T("edit"), outLocation.Path, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 115, 150, 275, 24, hWnd, (HMENU)136, NULL, NULL);
		SendMessage(hEditOutput, WM_SETFONT, (WPARAM)getFont(), TRUE);

		hSelectInputButton = CreateWindow(_T("button"), _T("..."), WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | BS_CENTER | BS_VCENTER, 405, 100, 32, 24, hWnd, (HMENU)137, NULL, NULL);
		SendMessage(hSelectInputButton, WM_SETFONT, (WPARAM)getFont(), TRUE);

		hSelectOutputButton = CreateWindow(_T("button"), _T("..."), WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | BS_CENTER | BS_VCENTER, 405, 150, 32, 24, hWnd, (HMENU)138, NULL, NULL);
		SendMessage(hSelectOutputButton, WM_SETFONT, (WPARAM)getFont(), TRUE);

		hConvertButton = CreateWindow(_T("button"), _T("Convert"), WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | BS_CENTER | BS_VCENTER, 215, 240, 100, 32, hWnd, (HMENU)139, NULL, NULL);
		SendMessage(hConvertButton, WM_SETFONT, (WPARAM)getFont(), TRUE);

		hExitButton = CreateWindow(_T("button"), _T("Exit"), WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | BS_CENTER | BS_VCENTER, 335, 240, 100, 32, hWnd, (HMENU)140, NULL, NULL);
		SendMessage(hExitButton, WM_SETFONT, (WPARAM)getFont(), TRUE);

		hCheckBox = CreateWindow(_T("button"), NULL, WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_LEFT | BS_VCENTER | BS_AUTOCHECKBOX, 115, 195, 20, 24, hWnd, (HMENU)143, NULL, NULL);
		SendMessage(hCheckBox, WM_SETFONT, (WPARAM)getFont(), TRUE);
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDC_INPUTBUTTON:
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = _T("League of Legends SKN (*.skn)\0*.skn\0All Files (*.*)\0*.*\0");
			ofn.lpstrFile = inLocation.Path;
			ofn.nMaxFile = _MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = _T("skn");

			if (GetOpenFileName(&ofn))
			{
				SetDlgItemText(hWnd, IDC_INPUTEDIT, inLocation.Path);
				_wsplitpath_s(inLocation.Path, inLocation.Drive, inLocation.Directory, inLocation.Name, inLocation.Extension);
				_wmakepath_s(outLocation.Path, inLocation.Drive, inLocation.Directory, inLocation.Name, _T("dae"));
				SetDlgItemText(hWnd, IDC_OUTPUTEDIT, outLocation.Path);
			}
			break;

		case IDC_OUTPUTBUTTON:
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = _T("COLLADA (*.dae)\0*.dae\0All Files (*.*)\0*.*\0");
			ofn.lpstrFile = outLocation.Path;
			ofn.nMaxFile = _MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
			ofn.lpstrDefExt = _T("dae");

			if (GetSaveFileName(&ofn))
			{
				SetDlgItemText(hWnd, IDC_OUTPUTEDIT, outLocation.Path);
			}
			break;

		case IDC_CONVERTBUTTON:
			GetDlgItemText(hWnd, IDC_INPUTEDIT, inLocation.Path, _MAX_PATH);
			GetDlgItemText(hWnd, IDC_OUTPUTEDIT, outLocation.Path, _MAX_PATH);

			_wsplitpath_s(inLocation.Path, inLocation.Drive, inLocation.Directory, inLocation.Name, inLocation.Extension);
			_wmakepath_s(sklLocation.Path, inLocation.Drive, inLocation.Directory, inLocation.Name, _T("skl"));

			inputSkn.applyOffset = false;
			inputSkn.invertTextureV = true;

			SendMessage(hCheckBox, BM_GETCHECK, NULL, NULL) == BST_CHECKED ? outputCollada.includeSkl = true : outputCollada.includeSkl = false;

			try
			{
				if (inputSkn.readFile(inLocation.Path))
				{
					outputCollada.indices = &inputSkn.indices;
					outputCollada.vertices = &inputSkn.vertices;
					outputCollada.numVertices = &inputSkn.numVertices;
					outputCollada.numIndices = &inputSkn.numIndices;
					
					inputSkl.sknVersion = &inputSkn.fileVersion;

					if (outputCollada.includeSkl)
					{
						if (inputSkl.readFile(sklLocation.Path))
						{
							outputCollada.numBones = &inputSkl.numBones;
							outputCollada.sklBones = &inputSkl.bones;
							outputCollada.numBoneIndices = &inputSkl.numBoneIndices;
							outputCollada.boneIndices = &inputSkl.boneIndices;
						}
					}
					
					if (outputCollada.writeFile(outLocation.Path))
					{
						mbInfo(hWnd, _T("File was written successfully,"));
					}
				}

			}

			catch (TCHAR* Error)
			{
				mbError(hWnd, Error);
			}
			break;

		case IDC_EXITBUTTON:
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		GetClientRect(hWnd, &clientRect);
		displayLogo(hdc, clientRect);
		SetTextColor(hdc, RGB(234, 234, 234));
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, getFont());

		textRect.left = 0;
		textRect.top = 100;
		textRect.bottom = 124;
		textRect.right = 100;
		DrawText(hdc, _T("Input file:"), -1, &textRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		textRect.top = 150;
		textRect.bottom = 174;
		DrawText(hdc, _T("Output file:"), -1, &textRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		textRect.left = 135;
		textRect.top = 195;
		textRect.bottom = 219;
		textRect.right = 390;
		DrawText(hdc, _T("Import skeleton file"), -1, &textRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

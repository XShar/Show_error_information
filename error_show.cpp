#include <Windows.h>
#include <strsafe.h>
#include "stdafx.h"
#include "error_show.h"

HWND Dialog;

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (IsDlgButtonChecked(hwndDlg, IDC_CHECK1))
	{
		SetWindowPos(hwndDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(hwndDlg, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	switch (uMsg)
	{
		case WM_CLOSE: ExitProcess(0);
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDC_BUTTON1:
				{
					// получаем код ошибки
					DWORD dwError = GetDlgItemInt(hwndDlg, IDC_EDIT1, NULL, FALSE);
					HLOCAL hlocal = NULL;
					
					// Мы ищем сообщения Windows, поэтому используем системные 
					// региональные параметры по умолчанию 
					DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

					// получаем описание ошибки по коду
					BOOL fOk = FormatMessage(
						FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
						NULL,
						dwError,
						systemLocale,
						(PTSTR)& hlocal,
						0,
						NULL);

					if (!fOk)
					{
						// Это ошибка, связанная с сетью ? 
						HMODULE hDll = LoadLibraryEx(
							TEXT("netmsg.dll"),
							NULL,
							DONT_RESOLVE_DLL_REFERENCES);

						if (hDll != NULL)
						{
							FormatMessage(
								FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
								hDll,
								dwError,
								MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
								(PTSTR)& hlocal,
								0,
								NULL);
							FreeLibrary(hDll);
						}
					}

					if (hlocal != NULL)
					{
						SetDlgItemText(hwndDlg, IDC_EDIT3, (PCTSTR)LocalLock(hlocal));
						LocalFree(hlocal);
					}
					else
					{
						SetDlgItemText(hwndDlg, IDC_EDIT3, TEXT("Error number not found."));
					}
					break;
				}	
			}
			break;
		}
	}
	return FALSE;
}

#pragma comment(linker, "/SUBSYSTEM:Windows /ENTRY:WinMain")
BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdShow, INT nCmdShow)
{
	MSG uMsg;
	Dialog = CreateDialogParam(NULL, MAKEINTRESOURCE(IDD_FORMVIEW), HWND_DESKTOP, DialogProc, NULL);
	ShowWindow(Dialog, SW_SHOW);
	while (GetMessage(&uMsg, 0, 0, 0))
	{
		if (!IsDialogMessage(Dialog, &uMsg))
		{
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}
	}

    return (int)uMsg.wParam;
}
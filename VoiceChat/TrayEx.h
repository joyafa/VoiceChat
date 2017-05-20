#pragma once
#include "xcgui.h"
#include "Common.h"

class CTrayEx
{
public:
	int OnWndDestroy(BOOL *pbHandled)
	{
		DeleteTray();
		return 0;
	}
	int OnShowWindow(WPARAM wParam, LPARAM lParam, BOOL *pbHandled)
	{
		if (wParam == TRUE)
		{
			DeleteTray();
		}
		else
		{
			CreateTray();

		}
		return 0;
	}

	void Create(HWINDOW hWindow);
	void CreateTray();
	void DeleteTray();
public:
	int  OnWndMenuSelect(int nID, BOOL *pBool);
	int OnWndTray(WPARAM wParam, LPARAM lParam, BOOL *pbHandled);


private:
	HWINDOW m_hWindow;
};
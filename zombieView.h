// zombieView.h : interface of the CZombieView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

class CZombieView : public CWindowImpl<CZombieView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CZombieView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);

	CZombieView();
	void DrawGrid();
	void DrawMachines();
	void DrawStatus();
	void DrawMouseOverText();

	bool mouseover;
	int mouse_x; 
	int mouse_y;
	std::string memoryText;

	Gdiplus::Bitmap m_BitmapImage;
	Gdiplus::Graphics m_GraphicsImage;
};

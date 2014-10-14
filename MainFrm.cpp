// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "zombieView.h"
#include "ZombieWorld.h"
#include "MainFrm.h"
#include <stdlib.h>
#include <time.h>

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	CreateSimpleStatusBar();

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Check if this is the turn timer
	if (wParam == 1)
	{
		// For each of the zombies that exist in the sim
		for (auto zombie : ZombieWorld::zombies)
		{
			// If this zombie recently turned, we have to reset its 
			// state and bind it!
			if (zombie->turnToZombie == TRUE)
			{
				zombie->turnToZombie = FALSE;
				zombieMachine.BindState(*zombie);
				zombie->m_ProgramCounter = 1;
				zombie->m_ActionsTaken = 0;
				zombie->m_Test = false;
			}
			zombieMachine.TakeTurn(*zombie);
		}

		// For each of the zombies that exist in the sim
		for (auto human : ZombieWorld::humans)
		{
			humanMachine.TakeTurn(*human);
		}

		// Increment turns and redraw 
		ZombieWorld::turns++;
		m_view.RedrawWindow();

		// If the sim should be over, kill the timer and print who won
		if ((ZombieWorld::zombies.size() == 0 || ZombieWorld::humans.size() == 0) && Running)
		{
			KillTimer(1);
			Running = false;
			if (ZombieWorld::zombies.size() == 0){
				MessageBox("Humans win!");
			}
			else {
				MessageBox("Zombies win!");
			}

		}
	}

	return 0;
}

LRESULT CMainFrame::OnSimStart(WORD , WORD , HWND , BOOL& )
{
	if (Running){
		// Stop the timer
		KillTimer(1);
		Running = false;
	}
	else {
		// Add timer to run once per second
		SetTimer(1, 1000);
		Running = true;
	}
	return 0;
}

LRESULT CMainFrame::OnSimClear(WORD, WORD, HWND, BOOL&)
{
	// Stop the sim and reset the turns
	KillTimer(1);
	Running = false;
	ZombieWorld::turns = 0;

	// Clear ZombieWorld of all the states, update the locationMap
	for (auto zombie : ZombieWorld::zombies){
		delete zombie;
	}
	for (auto human : ZombieWorld::humans){
		delete human;
	}
	ZombieWorld::zombies.clear();
	ZombieWorld::humans.clear();
	ZombieWorld::UpdatePositions();

	m_view.RedrawWindow();

	return 0;
}

LRESULT CMainFrame::OnLoadZombie(WORD, WORD, HWND, BOOL&)
{
	// Open dialog for .zom files
	char filter[] = { "ZOM Files\0*.zom\0All Files\0*.*\0" };
	CFileDialog openDialog(TRUE, _T("zom"), _T(".zom"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, filter);

	if (openDialog.DoModal() == IDOK)
	{
		// Get the filename and load it up
		std::string filename = openDialog.m_szFileName;
		unsigned found = filename.find_last_of("/\\");
		zombieMachine.LoadMachine(filename.substr(found+1));
		ZombieWorld::zombieProgram = filename.substr(found + 1);
	}
	return 0;
}

LRESULT CMainFrame::OnLoadSurvivor(WORD, WORD, HWND, BOOL&)
{
	// Open dialog for .zom files
	char filter[] = { "ZOM Files\0*.zom\0All Files\0*.*\0" };
	CFileDialog openDialog(TRUE, _T("zom"), _T(".zom"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, filter);

	if (openDialog.DoModal() == IDOK)
	{
		// Get the filename and load it up
		std::string filename = openDialog.m_szFileName;
		unsigned found = filename.find_last_of("/\\");
		humanMachine.LoadMachine(filename.substr(found + 1));
		ZombieWorld::humanProgram = filename.substr(found + 1);
	}
	return 0;
}

LRESULT CMainFrame::OnSimRandomize(WORD, WORD, HWND, BOOL&)
{
	// Set random seed
	srand((unsigned int)time(NULL));

	try {
		if (ZombieWorld::zombieProgram == "" || ZombieWorld::humanProgram == ""){
			throw MachinesNotLoaded();
		}

		bool validSpot;
		int x, y;

		// Create zombies
		for (int i = 0; i < 20; i++)
		{
			validSpot = FALSE;
			MachineState *z = new MachineState;

			// Ensure that location hasn't already been taken
			while (!validSpot)
			{
				x = rand() % 19;
				y = rand() % 19;
				if (!ZombieWorld::IsOccupied(x, y)){
					validSpot = TRUE;
				}
			}

			z->x = x;
			z->y = y;

			// Randomize its direction
			switch (rand() % 4){
			case 0:
				z->m_Facing = MachineState::UP;
				break;
			case 1:
				z->m_Facing = MachineState::RIGHT;
				break;
			case 2:
				z->m_Facing = MachineState::DOWN;
				break;
			default:
			case 3:
				z->m_Facing = MachineState::LEFT;
				break;
			}

			zombieMachine.BindState(*z);
			ZombieWorld::AddZombie(z);
		}

		// Create humans
		for (int i = 0; i < 10; i++)
		{
			validSpot = FALSE;

			MachineState *h = new MachineState;

			// Ensure that location hasn't already been taken
			while (!validSpot)
			{
				x = rand() % 19;
				y = rand() % 19;
				if (!ZombieWorld::IsOccupied(x, y)){
					validSpot = TRUE;
				}
			}

			h->x = x;
			h->y = y;

			// Randomize its direction
			switch (rand() % 4){
			case 0:
				h->m_Facing = MachineState::UP;
				break;
			case 1:
				h->m_Facing = MachineState::RIGHT;
				break;
			case 2:
				h->m_Facing = MachineState::DOWN;
				break;
			default:
			case 3:
				h->m_Facing = MachineState::LEFT;
				break;
			}

			humanMachine.BindState(*h);
			ZombieWorld::AddHuman(h);
		}

		m_view.RedrawWindow();
	} 
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
	}

	return 0;
}
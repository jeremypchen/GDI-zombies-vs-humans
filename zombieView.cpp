// zombieView.cpp : implementation of the CZombieView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "machine.h"
#include "zombieView.h"
#include "ZombieWorld.h"
#include <sstream>

using namespace Gdiplus;

CZombieView::CZombieView()
	: m_BitmapImage(1024, 768)
	, m_GraphicsImage(&m_BitmapImage)
{

}

BOOL CZombieView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CZombieView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	DrawGrid();
	DrawMachines();
	DrawStatus();
	if (mouseover){
		DrawMouseOverText();
	}

	Graphics graphics(dc);
	graphics.DrawImage(&m_BitmapImage, 0, 0);

	return 0;
}

/* Used to draw memory status over humans */
LRESULT CZombieView::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	// Get the mouse coordinates 
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);

	MachineState* m = new MachineState;
	
	// If there is something at xPos, yPos
	if (ZombieWorld::GetMachineAt(xPos, yPos, m) && !m->GetInfect()){
		mouseover = true;
		// Save what the memory is of this machine
		std::stringstream stream;
		stream << m->GetMemory(0) << ", " << m->GetMemory(1);
		memoryText = stream.str();
		mouse_x = xPos;
		mouse_y = yPos;

		// Since mouseover = true, then DrawMouseOverText() will be called in OnPaint
		RedrawWindow();
	}
	else 
	{
		// Otherwise, don't draw text
		memoryText = "";
		//mouseover = false;
		RedrawWindow();
	}

	return 0;
}


void CZombieView::DrawMouseOverText()
{
	Font font(L"Times New Roman", 12);
	SolidBrush brushBlack(Color(0, 0, 0));

	PointF origin(static_cast<float>(mouse_x)+10, static_cast<float>(mouse_y));

	m_GraphicsImage.DrawString(CA2W(memoryText.c_str()), -1, &font,
		PointF(origin),
		&brushBlack);
}

void CZombieView::DrawGrid()
{
	m_GraphicsImage.Clear(Color(255, 255, 255));
	Pen pen(Color(0, 0, 0));
	// Outline of board
	m_GraphicsImage.DrawRectangle(&pen, 10, 10, 600, 600);

	// Draw 400 boxes, size 30x30
	for (int x = 0; x < 20; x++){
		for (int y = 0; y < 20; y++){
			m_GraphicsImage.DrawRectangle(&pen, 10 + x*30, 10 + y*30, 30, 30);
		}
	}
}

void CZombieView::DrawMachines()
{
	// Get all zombies and humans, draw them at their respective locations
	for (auto zombie : ZombieWorld::zombies)
	{
		REAL x = static_cast<float>((zombie->x)*30 + 10);
		REAL y = static_cast<float>((zombie->y)*30 + 10);

		// Use three points to draw a triangle shape
		PointF point1;
		PointF point2;
		PointF point3;
		PointF points[3];

		// Draw different facing triangles depending on their direction
		switch (zombie->m_Facing)
		{
		case(MachineState::UP) :
			point1.X = x; point1.Y = y + 30;
			point2.X = x + 30; point2.Y = y + 30;
			point3.X = (x + x + 30) / 2; point3.Y = y;

			points[0] = point1; points[1] = point2; points[2] = point3;
			break;
		case(MachineState::RIGHT) :
			point1.X = x; point1.Y = y;
			point2.X = x; point2.Y = y + 30;
			point3.X = x + 30; point3.Y = (y + y + 30)/2;

			points[0] = point1; points[1] = point2; points[2] = point3;
			break;
		case(MachineState::DOWN) :
			point1.X = x; point1.Y = y;
			point2.X = x + 30; point2.Y = y;
			point3.X = (x + x + 30) / 2; point3.Y = y + 30;

			points[0] = point1; points[1] = point2; points[2] = point3;
			break;
		default:
		case(MachineState::LEFT) :
			point1.X = x + 30; point1.Y = y;
			point2.X = x + 30; point2.Y = y + 30;
			point3.X = x; point3.Y = (y + y + 30) / 2;

			points[0] = point1; points[1] = point2; points[2] = point3;
			break;
		}

		// Zombies are GREEN!
		SolidBrush greenBrush(Color(255, 66, 111, 66));

		m_GraphicsImage.FillPolygon(&greenBrush, points, 3);
	}

	for (auto human : ZombieWorld::humans)
	{
		REAL x = static_cast<float>((human->x) * 30 + 10);
		REAL y = static_cast<float>((human->y) * 30 + 10);

		// Use three points to draw a triangle shape
		PointF point1;
		PointF point2;
		PointF point3;
		PointF points[3];

		// Draw different facing triangles depending on their direction
		switch (human->m_Facing)
		{
		case(MachineState::UP) :
			point1.X = x; point1.Y = y + 30;
			point2.X = x + 30; point2.Y = y + 30;
			point3.X = (x + x + 30) / 2; point3.Y = y;

			points[0] = point1; points[1] = point2; points[2] = point3;
			break;
		case(MachineState::RIGHT) :
			point1.X = x; point1.Y = y;
			point2.X = x; point2.Y = y + 30;
			point3.X = x + 30; point3.Y = (y + y + 30) / 2;

			points[0] = point1; points[1] = point2; points[2] = point3;
			break;
		case(MachineState::DOWN) :
			point1.X = x; point1.Y = y;
			point2.X = x + 30; point2.Y = y;
			point3.X = (x + x + 30) / 2; point3.Y = y + 30;

			points[0] = point1; points[1] = point2; points[2] = point3;
			break;
		default:
		case(MachineState::LEFT) :
			point1.X = x + 30; point1.Y = y;
			point2.X = x + 30; point2.Y = y + 30;
			point3.X = x; point3.Y = (y + y + 30) / 2;

			points[0] = point1; points[1] = point2; points[2] = point3;
			break;
		}

		// Humans are tan!!!
		SolidBrush tanBrush(Color(255, 210, 180, 140));

		m_GraphicsImage.FillPolygon(&tanBrush, points, 3);
	}
}

// Draw the status of the program on the right-hand side
void CZombieView::DrawStatus()
{
	Font font(L"Calibri", 12);
	SolidBrush brushBlack(Color(0, 0, 0));

	// Use same stringstream for each line
	PointF loc(615, 10);
	std::stringstream stream;
	stream << "Zombies Alive: " << ZombieWorld::zombies.size();

	m_GraphicsImage.DrawString(CA2W(stream.str().c_str()), -1, &font,
		PointF(loc),
		&brushBlack);

	// Move down, clear stream. 
	// stream.str(std::string()) is efficient way to clear
	loc.Y += 20;
	stream.str(std::string());
	stream << "Zombie Program: " << ZombieWorld::zombieProgram;

	m_GraphicsImage.DrawString(CA2W(stream.str().c_str()), -1, &font,
		PointF(loc),
		&brushBlack);

	// Rinse, repeat for each line
	loc.Y += 20;
	stream.str(std::string());
	stream << "Humans Alive: " << ZombieWorld::humans.size();

	m_GraphicsImage.DrawString(CA2W(stream.str().c_str()), -1, &font,
		PointF(loc),
		&brushBlack);

	loc.Y += 20;
	stream.str(std::string());
	stream << "Human Program: " << ZombieWorld::humanProgram;

	m_GraphicsImage.DrawString(CA2W(stream.str().c_str()), -1, &font,
		PointF(loc),
		&brushBlack);

	loc.Y += 20;
	stream.str(std::string());
	stream << "Month (turns): " << ZombieWorld::GetMonthAndTurn();

	m_GraphicsImage.DrawString(CA2W(stream.str().c_str()), -1, &font,
		PointF(loc),
		&brushBlack);
}
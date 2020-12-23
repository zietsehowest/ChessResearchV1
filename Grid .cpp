#include "pch.h"
#include "Grid.h"
#include "utils.h"

Grid::Grid(int rows, int cols, float windowWidth, float windowHeight)
	: m_Rows{rows},
	m_Cols{cols},
	m_WindowWidth{windowWidth},
	m_WindowHeight{windowHeight},
	m_CellWidth{windowWidth / cols},
	m_CellHeight{windowHeight / rows}
{
	// Initialize all the cells inside the grid
	for (int row{}; row < rows; ++row)
	{
		for (int col{}; col < cols; ++col)
		{
			m_Cells.push_back(Rectf{col * m_CellWidth, row * m_CellHeight, m_CellWidth, m_CellHeight});
		}
	}
}

void Grid::DrawGrid(bool isCheckered) const
{
	if (isCheckered)
	{
		bool isBlack = true;
		int rowCount = 0;
		for (const auto& cell : m_Cells)
		{
			if (rowCount == 8)
			{
				rowCount = 0;
				isBlack = !isBlack;
			}
			if (isBlack)
			{
				utils::SetColor(Color4f{ 50.f / 255.f, 125.f / 255.f, 0.f, 1.f });
			}
			else
			{
				utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 150.f / 255.f });
			}
			utils::FillRect(cell);
			isBlack = !isBlack;
			++rowCount;
		}
		return;
	}
	utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f});
	for (const auto& cell : m_Cells)
	{
		utils::DrawRect(cell, 3.f);
	}
}

int Grid::GetIndexFromPos(const Point2f& pos) const
{
	int idx{}, col{}, row{};
	col = int(pos.x / m_CellWidth);
	row = int((m_CellHeight*m_Rows - pos.y) / m_CellHeight);

	idx = col + (row * m_Cols);

	return idx;
}

const Point2f Grid::GetPosFromIdx(int idx) const
{
	return Point2f{ m_Cells[idx].left , m_Cells[idx].bottom };;
}

float Grid::GetCellWidth() const
{
	return m_CellWidth;
}

float Grid::GetSpaceHeight() const
{
	return m_WindowHeight;
}

std::pair<int, int> Grid::GetRowCol(const Point2f& pos) const
{
	int idx{}, col{}, row{};
	col = int(pos.x / m_CellWidth);
	row = int(pos.y / m_CellHeight);

	return std::pair<int, int>{row, col};
}

int Grid::GetRowFromIdx(size_t idx) const
{
	return int(GetPosFromIdx(idx).y / m_CellHeight);
}

int Grid::GetColFromIdx(size_t idx) const
{
	return int(GetPosFromIdx(idx).x / m_CellWidth);
}

Rectf Grid::GetCell(size_t idx) const
{
	return m_Cells[idx];
}

int Grid::GetCellAmount() const
{
	return (int)m_Cells.size();
}
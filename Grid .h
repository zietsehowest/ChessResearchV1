#pragma once
#include <vector>
#include "structs.h"

class Grid
{
public:
	Grid(int rows, int cols, float windowWidth, float windowHeight);

	void DrawGrid(bool isCheckered) const;

	int GetIndexFromPos(const Point2f& pos) const;

	const Point2f GetPosFromIdx(int idx) const;

	float GetCellWidth() const;
	float GetSpaceHeight() const;
	std::pair<int, int> GetRowCol(const Point2f& pos) const;
	int GetRowFromIdx(size_t idx) const;
	int GetColFromIdx(size_t idx) const;
	Rectf GetCell(size_t idx) const;

	int GetCellAmount() const;
private:
	int m_Rows, m_Cols;
	std::vector<Rectf> m_Cells;
	float m_WindowWidth, m_WindowHeight, m_CellWidth, m_CellHeight;
};
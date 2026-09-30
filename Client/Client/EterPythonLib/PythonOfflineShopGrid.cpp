#include "StdAfx.h"
#if defined(ENABLE_OFFLINE_SHOP_GRID)
#include <string.h>
#include <stdio.h>
#include "PythonOfflineShopGrid.h"
#include <algorithm>

/*
author_ Lightwork
work_ OfflineShop Grid
date_ 12.09.2022, 10:52 PM
*/

CGrid::CGrid(int32_t w, int32_t h) : m_iWidth(w), m_iHeight(h), l_gridLocked(false) 
{
	m_pGrid = new char[m_iWidth * m_iHeight];
	memset(m_pGrid, 0, sizeof(char) * m_iWidth * m_iHeight);
}

CGrid::~CGrid() 
{
	delete[] m_pGrid;
	l_unlocked = 0;
	m_iWidth = 0;
	m_iHeight = 0;
	l_gridLocked = false;
}

void CGrid::SetSize(int32_t w, int32_t h, int32_t l_availableSlots, int32_t visualWidth) 
{
	delete[] m_pGrid; // Deleting the old grid when set the size
	m_iWidth = w;
	m_iHeight = h;

	if (l_availableSlots > 0)
	{
		l_unlocked = l_availableSlots;
		l_gridLocked = true;
		m_iWidthVisual = visualWidth;
	}
	m_pGrid = new char[m_iWidth * m_iHeight];
	memset(m_pGrid, 0, sizeof(char) * m_iWidth * m_iHeight);
}

bool CGrid::isLocked(int32_t iPos, int32_t h) 
{
	for (int32_t y = 0; y < h; ++y)
	{
		int32_t iStart = iPos + (y * m_iWidthVisual);
		if (l_unlocked < iStart)
			return true; // this slot not available brocum cant put item
	}

	return false;
}

void CGrid::Unlock()
{
	if (l_unlocked > m_iWidth * m_iHeight) 
	{ // It does not let if the unlocked size is over the whole grid table
		return;
	}
	l_unlocked += 1;
}

void CGrid::Clear() 
{
	memset(m_pGrid, 0, sizeof(char) * m_iWidth * m_iHeight);
}

int CGrid::FindBlank(int32_t w, int32_t h) 
{
	if (w > m_iWidth || h > m_iHeight)
		return -1;

	int32_t iRow;

	for (iRow = 0; iRow < m_iHeight; ++iRow)
	{
		for (int32_t iCol = 0; iCol < m_iWidth; ++iCol)
		{
			int32_t iIndex = iRow * m_iWidth + iCol;

			if (IsEmpty(iIndex, w, h))
			{
				return iIndex;
			}
		}
	}

	return -1;
}

bool CGrid::Put(int32_t iPos, int32_t w, int32_t h) 
{
	if (!IsEmpty(iPos, w, h)) 
	{
		return false;
	}

	for (int32_t y = 0; y < h; ++y)
	{
		int32_t iStart = iPos + (y * m_iWidth);
		if (l_gridLocked == true)
		{
			iStart = iPos + (y * m_iWidthVisual); // If control is still active, it sets the pos to start pos
		}
		m_pGrid[iStart] = true; // fix false -> true

		int32_t x = 1;
		while (x < w) 
		{
			m_pGrid[iStart + x++] = true;
		}
	}

	return true;
}

void CGrid::Get(int32_t iPos, int32_t w, int32_t h)
{
	if (iPos < 0 || iPos >= m_iWidth * m_iHeight) 
	{
		return;
	}

	for (int32_t y = 0; y < h; ++y)
	{
		int32_t iStart = iPos + (y * m_iWidth);

		if (l_gridLocked == true) 
		{
			iStart = iPos + (y * m_iWidthVisual); // If control is still active, it sets the pos to start pos
		}
		m_pGrid[iStart] = false;

		int32_t x = 1;
		while (x < w)
		{
			m_pGrid[iStart + x++] = false;
		}
	}
}

bool CGrid::IsEmpty(int32_t iPos, int32_t w, int32_t h)
{
	if (iPos < 0) 
	{
		return false;
	}

	if (l_gridLocked == true && isLocked(iPos, h)) 
	{
		return false; // If lock check is active and the slot locked, it says it is not empty
	}

	int32_t iRow = iPos / m_iWidth;

	if (l_gridLocked) 
	{
		if (iRow + h > m_iWidthVisual)
			return false;
	}
	else 
	{
		if (iRow + h > m_iHeight)
			return false;
	}

	if (iPos + w > iRow * m_iWidth + m_iWidth) 
	{
		return false;
	}

	for (int32_t y = 0; y < h; ++y)
	{
		int32_t iStart = iPos + (y * m_iWidth);

		if (l_gridLocked == true) 
		{
			iStart = iPos + (y * m_iWidthVisual); // If control is still active, it sets the pos to start pos
		}

		if (m_pGrid[iStart]) 
		{
			return false;
		}

		int32_t x = 1;

		while (x < w) {
			if (m_pGrid[iStart + x++]) 
			{
				return false;
			}
		}
	}
	return true;
}

uint32_t CGrid::GetSize() 
{
	return m_iWidth * m_iHeight;
}

uint32_t CGrid::GetAvailableSlots() 
{
	return l_unlocked;
}
#endif
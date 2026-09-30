#ifndef __INC_METIN_II_GRID_H__
#define __INC_METIN_II_GRID_H__

#if defined(ENABLE_OFFLINE_SHOP_GRID)

/*
author_ Lightwork
work_ OfflineShop Grid
date_ 12.09.2022, 10:52 PM
*/

class CGrid
{
public:
	CGrid(int w, int h);
	virtual ~CGrid();

	void		Clear();
	int			FindBlank(int32_t w, int32_t h);
	bool		IsEmpty(int32_t iPos, int32_t w, int32_t h);
	bool		Put(int32_t iPos, int32_t w, int32_t h);
	void		Get(int32_t iPos, int32_t w, int32_t h);
	uint32_t	GetSize();
	void		SetSize(int32_t w, int32_t h, int32_t l_availableSlots, int32_t visualWidth);
	bool		isLocked(int32_t iPos, int32_t h);
	void		Unlock();
	uint32_t	GetAvailableSlots();

protected:
	int32_t		m_iWidth;
	int32_t		m_iHeight;
	int32_t		l_unlocked;
	int32_t		m_iWidthVisual;
	bool		l_gridLocked;
	char* m_pGrid;
};

#endif
#endif

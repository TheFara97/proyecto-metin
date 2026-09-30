#include "../../common/stl.h"

class CMapLocation : public singleton<CMapLocation>
{
	public:
		typedef struct SLocation
		{
			int32_t        addr;
			WORD        port;
		} TLocation;

		bool    Get(int32_t x, int32_t y, int32_t & lMapIndex, int32_t & lAddr, WORD & wPort);
		bool	Get(int iIndex, int32_t & lAddr, WORD & wPort);
		void    Insert(int32_t lIndex, const char * c_pszHost, WORD wPort);

	protected:
		std::map<int32_t, TLocation> m_map_address;
};
//martysama0134's ceqyqttoaf71vasf9t71218

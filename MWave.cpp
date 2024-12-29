#include "pch.h"
#include "MWave.h"
#include <cassert>

namespace mwave
{
	const char Pattern::Table[32][5]
	{
		{1,3,2,5,4},
		{1,4,2,5,3},
		{1,4,3,5,2},
		{1,5,2,4,3},
		{1,5,3,4,2},
		{2,1,4,3,5},
		{2,1,5,3,4},
		{2,3,1,5,4},
		{2,4,1,5,3},
		{2,4,3,5,1},
		{2,5,1,4,3},
		{2,5,3,4,1},
		{3,1,4,2,5},
		{3,1,5,2,4},
		{3,2,4,1,5},
		{3,2,5,1,4},
		{3,4,1,5,2},
		{3,4,2,5,1},
		{3,5,1,4,2},
		{3,5,2,4,1},
		{4,1,3,2,5},
		{4,1,5,2,3},
		{4,2,3,1,5},
		{4,2,5,1,3},
		{4,3,5,1,2},
		{4,5,1,3,2},
		{4,5,2,3,1},
		{5,1,3,2,4},
		{5,1,4,2,3},
		{5,2,3,1,4},
		{5,2,4,1,3},
		{5,3,4,1,2},
	};

	Pattern::Pattern()
		:Index{ -1 }
	{
	}

	Pattern::Pattern(const char index)
		:Index{ index }
	{
		//assert(index >= 0 && index <= 31);
	}

	Pattern Pattern::FromPrices(const double arr[])
	{
		auto iter{ arr };
		double peaks[5];

		for (auto& val : peaks)
			val = *iter++;

		auto peaks_max_value = [&peaks]()->int
			{
				auto iMax{ 0 };
				auto maxVal{ peaks[iMax] };

				for (int i = 1; i < 5; ++i)
					if (peaks[i] > maxVal)
						maxVal = peaks[i], iMax = i;

				return iMax;
			};

		int rank[5]{};

		for (int i = 1; i <= 5; i++)
		{
			const auto ind{ peaks_max_value() };
			rank[ind] = i, peaks[ind] = .0;
		}

		return GetPattern(rank[0], rank[1], rank[2]);
	}

	std::string Pattern::to_string() const
	{
		char ret[6];
		auto iter{ ret };

		for (auto val : Table[Index])
			*iter++ = '0' + val;
		*iter = '\0';
		return ret;
	}

	std::wstring Pattern::to_wstring() const
	{
		wchar_t ret[6];
		auto iter{ ret };

		for (auto val : Table[Index])
			*iter++ = L'0' + val;
		*iter = L'\0';
		return ret;
	}

	const char* Pattern::get_table_entry() const
	{
		return Table[Index];
	}

	char Pattern::get_id() const
	{
		return Index;
	}

	Pattern::operator char()const
	{
		return Index;
	}

	Pattern Pattern::GetPattern(const char a, const char b, const char c)
	{
		if (!is_valid(a, b, c))
			return {};

		switch (a)
		{
		case 1:
			switch (b)
			{
			case 3:return 0;
			case 4:return c == 2 ? 1 : 2;
			default:return c == 2 ? 3 : 4;
			}

		case 2:
			switch (b)
			{
			case 3:return 7;
			case 1:return c == 4 ? 5 : 6;
			case 4:return c == 1 ? 8 : 9;
			default:return c == 1 ? 10 : 11;
			}

		case 3:
			switch (b)
			{
			case 1:return c == 4 ? 12 : 13;
			case 2:return c == 4 ? 14 : 15;
			case 4:return c == 1 ? 16 : 17;
			default:return c == 1 ? 18 : 19;
			}

		case 4:
			switch (b)
			{
			case 1:return c == 3 ? 20 : 21;
			case 2:return c == 3 ? 22 : 23;
			case 5:return c == 1 ? 25 : 26;
			default:return 24;
			}

		default:
			switch (b)
			{
			case 1:return c == 3 ? 27 : 28;
			case 2:return c == 3 ? 29 : 30;
			default:return 31;
			}
		}
	}

	bool Pattern::operator==(const Pattern& oth)const
	{
		return Index == oth.Index;
	}

	bool Pattern::is_m() const
	{
		auto dots{ get_table_entry() };
		return dots[4] > dots[3];
	}
	
	bool Pattern::is_w() const
	{
		return !is_m();
	}
	
	bool Pattern::is_valid( const char a, const char b, const char c)
	{
		for (auto p : Table)
			if (p[0] == a && p[1] == b && p[2] == c)
				return true;
		
		return false;
	}
}
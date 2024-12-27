#include "pch.h"
#include "SReversal.h"
#include<cassert>

namespace mwave
{
	int SReversal::ArrayMinimum(const series& arr, const int iFrom, const int iCount)
	{
		int ret{ iFrom };
		double minVal{ arr[ret] };

		for (int i = iFrom + 1; i < iFrom + iCount; ++i)
			if (arr[i] < minVal)
				minVal = arr[i], ret = i;

		return ret;
	}

	int SReversal::ArrayMaximum(const series& arr, int iFrom, int iCount)
	{
		int ret{ iFrom };
		double maxVal{ arr[ret] };

		for (int i = iFrom + 1; i < iFrom + iCount; ++i)
			if (arr[i] > maxVal)
				maxVal = arr[i], ret = i;

		return ret;
	}

	SReversal::SReversal()
		:SReversal{ -1 }
	{
	}

	SReversal::SReversal(int reversal)
		:m_iLook4{ LOOK4::INIT }
		, m_iPeriod{ reversal }
		, m_iLastPeak{ -1 }
		, m_iLastLeg{ -1 }
		, m_EmptyValue{ DBL_MAX }
	{
	}

	void SReversal::Init(const size_t uSize, const double empty_value)
	{
		m_EmptyValue = empty_value;
		m_Buffers.resize(uSize, Data{ empty_value,empty_value,empty_value,empty_value, });
	}

	void SReversal::Apply(const series& open, const series& high, const series& low)
	{
		const auto count{ open.size() };
		assert(count == high.size());
		assert(low.size() == count);

		for (int index = 0; index < count; ++index)
			UpdateRecord(index, open, high, low);
	}

	const std::vector<SReversal::Data>& SReversal::GetBuffers() const
	{
		return m_Buffers;
	}

	void SReversal::UpdateRecord(const int index, const series& open, const series& high, const series& low)
	{
		assert(index < m_Buffers.size());
		double db;
		auto& buf{ m_Buffers[index] };
		switch (m_iLook4)
		{
		case  LOOK4::HIGH:
			if (ArrayMinimum(low, m_iLastLeg, index - m_iLastLeg + 1) == index)
			{
				m_Buffers[m_iLastPeak].Peaks = m_EmptyValue;
				buf.Lowest = buf.Peaks = low[index];

				m_iLastPeak = index;
			}
			else if (index > m_iLastLeg
				&& index > m_iLastPeak
				&& IsHighest(index, open[index], high, db))
			{
				buf.Leg = db;
				buf.Highest = buf.Peaks = high[index];

				m_iLook4 = LOOK4::LOW;
				m_iLastLeg = m_iLastPeak = index;
			}
			break;
		case LOOK4::LOW:
			if (ArrayMaximum(high, m_iLastLeg, index - m_iLastLeg + 1) == index)
			{
				m_Buffers[m_iLastPeak].Peaks = m_EmptyValue;
				buf.Highest = buf.Peaks = high[index];

				m_iLastPeak = index;
			}
			else if (index > m_iLastLeg
				&& index > m_iLastPeak
				&& IsLowest(index, open[index], low, db))
			{
				buf.Leg = db;
				buf.Lowest = buf.Peaks = low[index];

				m_iLook4 = LOOK4::HIGH;
				m_iLastLeg = m_iLastPeak = index;
			}

			break;
		default:
			if (IsHighest(index, open[index], high, db))
			{
				buf.Highest = buf.Peaks = high[index];
				m_iLook4 = LOOK4::LOW;
			}
			else if (IsLowest(index, open[index], low, db))
			{
				buf.Lowest = buf.Peaks = low[index];
				m_iLook4 = LOOK4::HIGH;
			}
			else break;

			buf.Leg = db;
			m_iLastLeg = m_iLastPeak = index;

			break;
		}
	}

	bool SReversal::IsHighest(const int index, const double open, const series& high, double& leg)const
	{
		const int  from{ index - m_iPeriod };
		if (from < 0)
			return false;

		const int iMax{ ArrayMaximum(high, from, m_iPeriod) };
		if (open >= high[iMax])
			leg = open;
		else if (high[index] >= high[iMax])
			leg = high[iMax];
		else return false;

		return true;
	}
	//+------------------------------------------------------------------+
	//|                                                                  |
	//+------------------------------------------------------------------+
	bool SReversal::IsLowest(const int index, const double open, const series& low, double& leg)const
	{
		const int  from{ index - m_iPeriod };
		if (from < 0)
			return false;

		const int iMin{ ArrayMinimum(low, from, m_iPeriod) };
		if (open <= low[iMin])
			leg = open;
		else if (low[index] <= low[iMin])
			leg = low[iMin];
		else return false;

		return true;

	}

	void SReversal::Serialize(std::ostream& os)
	{
		__int32 i{ __int32(m_iLook4) };
		os.write((const char*)&i, sizeof(__int32));

		i = m_iPeriod;
		os.write((const char*)&i, sizeof(__int32));

		i = m_iLastPeak;
		os.write((const char*)&i, sizeof(__int32));

		i = m_iLastLeg;
		os.write((const char*)&i, sizeof(__int32));

		os.write((const char*)&m_EmptyValue, sizeof m_EmptyValue);

		os << m_Buffers;
	}

	void SReversal::Serialize(std::istream& is)
	{
		__int32 i;
		is.read((char*)&i, sizeof(__int32));
		m_iLook4 = LOOK4(i);

		is.read((char*)&i, sizeof(__int32));
		m_iPeriod = i;

		is.read((char*)&i, sizeof(__int32));
		m_iLastPeak = i;

		is.read((char*)&i, sizeof(__int32));
		m_iLastLeg = 1;

		is.read((char*)&m_EmptyValue, sizeof m_EmptyValue);

		is >> m_Buffers;
	}

	double SReversal::GetEmptyValue() const
	{
		return m_EmptyValue;
	}
}
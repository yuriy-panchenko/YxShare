#pragma once
#include <vector>
#include "ISerial.h"

namespace mwave
{
	class SReversal
		:public ISerial
	{

	public:
		using series = std::vector<double>;
		struct Data { double Leg, Peaks, Highest, Lowest; };

	public:
		SReversal();
		SReversal(int reversal);

		void Init(size_t, double empty_value = DBL_MAX);
		void Apply(const series& open, const series& high, const series& low);
		const std::vector< Data >& GetBuffers()const;

	protected:
		void UpdateRecord(const int index, const series& open, const series& high, const series& low);
		virtual void Serialize(std::ostream&)override;
		virtual void Serialize(std::istream&)override;

	private:
		static int ArrayMinimum(const series&, int iFrom, int iCount);
		static int ArrayMaximum(const series&, int iFrom, int iCount);
		bool IsHighest(const int index, const double open, const series& high, double& leg)const;
		bool IsLowest(const int index, const double open, const series& low, double& leg)const;

	private:
		enum class LOOK4 { INIT, HIGH, LOW, } m_iLook4;
		int m_iPeriod, m_iLastPeak, m_iLastLeg;
		double m_EmptyValue;

		std::vector< Data > m_Buffers;
	};
}


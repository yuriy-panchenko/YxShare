#include "pch.h"
#include "csv_file.h"

#define BOM_WIN	0xFEFF
#define BOM_MAC	0xFFFE

namespace csv
{
	bool file::Read(const std::filesystem::path& filename)
	{
		Parse(Read(std::fstream{ filename }));
		return true;
	}

	const std::vector<std::wstring>& file::GetColumns() const
	{
		return m_Columns;
	}

	const std::vector<file::line>& file::GetLines() const
	{
		return m_Lines;
	}

	std::string file::Read(std::fstream&& s) const
	{
		if (s)
			return std::string{ std::istreambuf_iterator<char>(s), std::istreambuf_iterator<char>() };
		else
			return {};
	}

	bool file::Parse(std::string&& s)
	{
		m_Lines.clear();
		m_Columns.clear();

		if (s.length() < 2)
			return false;

		union con
		{
			wchar_t w;
			char c[2];
		}bom;

		auto to_ws = [](auto from, auto to)
			{
				con u;
				auto ind{ 0 };

				std::wstring ret;
				ret.reserve(std::distance(from, to));

				for (auto iter{ from }; iter < to; ++iter)
				{
					u.c[ind++] = *iter;
					if (!(ind %= 2))
						ret += u.w;
				}

				return ret;
			};

		auto is_number = [](const chunk& ch)->bool
			{
				static const std::wstring possible_chars{ L"0123456789.+-eE" };

				for (auto c : ch)
					if (possible_chars.find(c) == std::wstring::npos)
						return false;

				return true;
			};

		auto is_text_line = [is_number](const line& l)->bool
			{
				auto uCount{ 0ull };

				for (auto& ch : l)
					if (is_number(ch))
						++uCount;

				return uCount != l.size();
			};

		auto create_lines = [this, is_text_line](const wchar_t* eol, size_t eol_size)
			{
				size_t off{ 0ull };
				bool isFirstRecord{ true };
				m_Lines.clear();
				m_Lines.reserve(EstimateLineCount());

				for (size_t ind{ m_Buffer.find(eol, off) }; ind < std::wstring::npos; ind = m_Buffer.find(eol, off = ind + eol_size))
				{
					auto ln{ ProcessLine({ &m_Buffer[off], ind - off }) };
					if (m_Lines.empty() && isFirstRecord)
						if (is_text_line(ln))
						{
							for (auto& ch : ln)
								m_Columns.push_back({ ch.begin(), ch.end() });
							isFirstRecord = false;
							continue;
						}
						else
							for (size_t i = 0; i < ln.size(); i++)
								m_Columns.push_back(std::to_wstring(i + 1));

					m_Lines.push_back(std::move(ln));
					isFirstRecord = false;
				}
				m_Lines.shrink_to_fit();
			};

		auto itPos{ s.begin() };

		bom.c[0] = *itPos++;
		bom.c[1] = *itPos++;


		if (bom.w == BOM_WIN || bom.w == BOM_MAC)
		{
			const auto eol{ bom.w == BOM_WIN ? L"\r\n" : L"\n" };
			const auto eol_size{ bom.w == BOM_WIN ? 2 : 1 };
			m_Buffer = to_ws(itPos, s.end());
			create_lines(eol, eol_size);
		}
		else
		{
			m_Buffer = { s.begin(), s.end() };
			create_lines(L"\n", 1);
		}


		return true;
	}

	size_t file::EstimateLineCount()const
	{
		size_t iStart{}, ret{};

		do
		{
			const auto ind{ m_Buffer.find(L'\n',iStart) };
			if (ind == std::wstring::npos)
				break;
			iStart = ind + 1;
			++ret;
		} while (true);

		return ret;
	}

	file::line file::ProcessLine(std::wstring_view&& v) const
	{
		line ret;
		auto iter{ v.cbegin() }, itFrom{ iter };

		for (auto iEnd{ v.cend() }; iter != iEnd; ++iter)
			switch (*iter)
			{
			case L'\t':
				ret.push_back(v.substr(std::distance(v.begin(), itFrom), std::distance(itFrom, iter)));
				itFrom = iter + 1;
				break;
			case L'\n':
			case L'\r':
				break;
			}

		if (iter > itFrom)
			ret.push_back(v.substr(std::distance(v.begin(), itFrom), std::distance(itFrom, iter)));

		return ret;
	}
}
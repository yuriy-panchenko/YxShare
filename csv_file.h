#pragma once
#include <string>
#include <filesystem>
#include <fstream>

namespace csv
{
	class file
	{
		using chunk = std::wstring_view;
		using line = std::vector<chunk>;
		
		std::vector<std::wstring> m_Columns;
		std::vector<line> m_Lines;
		std::wstring m_Buffer;

	public:
		bool Read(const std::filesystem::path&);
		const std::vector<std::wstring>& GetColumns()const;
		const std::vector<line>& GetLines()const;

	protected:
		std::string Read(std::fstream&&)const;
		bool Parse(std::string&&);
		line ProcessLine(std::wstring_view&&)const;
		size_t EstimateLineCount()const;
	};
}


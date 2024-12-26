#pragma once
#include <string>

namespace mwave
{
	class Pattern
	{
	public:
		Pattern();
		Pattern(const Pattern&) = default;
		Pattern(char);

		static const char Table[32][5];

		bool operator==(const Pattern&)const;
		explicit operator char()const;
		
		static Pattern FromPrices(const double arr[]);

		std::string to_string()const;
		std::wstring to_wstring()const;

		const char* get_table_entry()const;
		char get_id()const;
		bool is_m()const;
		bool is_w()const;
		static bool is_valid(char,char,char);	//	first three dots of pattern
		
	protected:
		static Pattern GetPattern(char a, char b, char c);

	private:
		char Index;	//	pattern index (0..31)
	};
}
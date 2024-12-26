#pragma once
#include <vector>
#include <iostream>

template<typename T>std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);
template<typename T>std::istream& operator>>(std::istream& is, std::vector<T>& v);

class ISerial abstract
{
public:
	friend std::istream& operator>>(std::istream&, ISerial&);
	friend std::ostream& operator<<(std::ostream&, ISerial&);

protected:
	virtual void Serialize(std::ostream&) = 0;
	virtual void Serialize(std::istream&) = 0;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	const uint64_t size{ v.size() };
	os.write((const char*)&size, sizeof size);
	os.write((const char*)v.data(), size * sizeof(T));
	return os;
}

template<typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& v)
{
	uint64_t size;
	is.read((char*)&size, sizeof size);
	v.resize(size);
	is.read((char*)v.data(), size * sizeof(T));
	return is;
}
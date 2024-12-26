#include "pch.h"
#include "ISerial.h"

std::istream& operator>>(std::istream& is, ISerial& obj)
{
	try
	{
		obj.Serialize(is);
	}
	catch (const std::exception&)
	{

	}
	return is;
}

std::ostream& operator<<(std::ostream& os, ISerial& obj)
{
	try
	{
		obj.Serialize(os);
	}
	catch (const std::exception&)
	{

	}
	return os;
}

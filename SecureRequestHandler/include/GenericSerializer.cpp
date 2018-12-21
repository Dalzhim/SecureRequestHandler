#include "GenericSerializer.hpp"
#include <string>

template <>
std::string GenericSerialize<int>(const int& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<long>(const long& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<long long>(const long long& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<unsigned int>(const unsigned int& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<unsigned long>(const unsigned long& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<unsigned long long>(const unsigned long long& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<float>(const float& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<double>(const double& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<long double>(const long double& v)
{
	return std::to_string(v);
}

template <>
std::string GenericSerialize<std::string>(const std::string& v)
{
	return v;
}

template <>
std::string GenericSerialize<std::string_view>(const std::string_view& v)
{
	return std::string{v};
}

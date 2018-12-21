#include "GenericValidator.hpp"

template <>
std::optional<int> GenericValidate<int>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return std::stoi(str);
}

template <>
std::optional<long> GenericValidate<long>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return std::stol(str);
}

template <>
std::optional<long long> GenericValidate<long long>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return std::stoll(str);
}

template <>
std::optional<unsigned int> GenericValidate<unsigned int>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return static_cast<unsigned int>(std::stoul(str));
}

template <>
std::optional<unsigned long> GenericValidate<unsigned long>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return std::stoul(str);
}

template <>
std::optional<unsigned long long> GenericValidate<unsigned long long>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return std::stoull(str);
}

template <>
std::optional<float> GenericValidate<float>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return std::stof(str);
}

template <>
std::optional<double> GenericValidate<double>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return std::stod(str);
}

template <>
std::optional<long double> GenericValidate<long double>(std::string_view sv)
{
	std::string str{sv.data(), sv.size()};
	return std::stold(str);
}

template <>
std::optional<std::string> GenericValidate<std::string>(std::string_view sv)
{
	return std::string{sv.data(), sv.size()};
}

template <>
std::optional<std::string_view> GenericValidate<std::string_view>(std::string_view sv)
{
	return sv;
}

#ifndef GENERIC_SERIALIZER_HPP
#define GENERIC_SERIALIZER_HPP

#include <string>
#include <string_view>
#include <type_traits>

template <typename T>
std::string GenericSerialize(const T&)
{
	static_assert(!std::is_same_v<T, T>, "Specialization required.");
}

template <>
std::string GenericSerialize<int>(const int&);
template <>
std::string GenericSerialize<long>(const long&);
template <>
std::string GenericSerialize<long long>(const long long&);
template <>
std::string GenericSerialize<unsigned int>(const unsigned int&);
template <>
std::string GenericSerialize<unsigned long>(const unsigned long&);
template <>
std::string GenericSerialize<unsigned long long>(const unsigned long long&);
template <>
std::string GenericSerialize<float>(const float&);
template <>
std::string GenericSerialize<double>(const double&);
template <>
std::string GenericSerialize<long double>(const long double&);
template <>
std::string GenericSerialize<std::string>(const std::string&);
template <>
std::string GenericSerialize<std::string_view>(const std::string_view&);

template <typename T>
struct GenericSerializer
{
	using value_type = T;
	
	std::string operator()(const T& t) const
	{
		return GenericSerialize<T>(t);
	}
};

#endif

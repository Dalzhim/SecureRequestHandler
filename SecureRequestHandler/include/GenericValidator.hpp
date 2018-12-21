#ifndef GENERIC_VALIDATOR_H
#define GENERIC_VALIDATOR_H

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

template <typename T>
std::optional<T> GenericValidate(std::string_view sv)
{
	static_assert(!std::is_same_v<T, T>, "Specialization required.");
	return std::nullopt;
}

template <>
std::optional<int> GenericValidate<int>(std::string_view sv);
template <>
std::optional<long> GenericValidate<long>(std::string_view sv);
template <>
std::optional<long long> GenericValidate<long long>(std::string_view sv);
template <>
std::optional<unsigned int> GenericValidate<unsigned int>(std::string_view sv);
template <>
std::optional<unsigned long> GenericValidate<unsigned long>(std::string_view sv);
template <>
std::optional<unsigned long long> GenericValidate<unsigned long long>(std::string_view sv);
template <>
std::optional<float> GenericValidate<float>(std::string_view sv);
template <>
std::optional<double> GenericValidate<double>(std::string_view sv);
template <>
std::optional<long double> GenericValidate<long double>(std::string_view sv);
template <>
std::optional<std::string> GenericValidate<std::string>(std::string_view sv);
template <>
std::optional<std::string_view> GenericValidate<std::string_view>(std::string_view sv);

template <typename T>
struct GenericValidator
{
	std::optional<T> operator()(std::string_view sv)
	{
		return GenericValidate<T>(sv);
	}
};

#endif

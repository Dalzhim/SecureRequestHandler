#ifndef QUERY_STRING_VALIDATOR_H
#define QUERY_STRING_VALIDATOR_H

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include "GenericValidator.hpp"
#include "QueryString.hpp"
#include <vector>

std::optional<std::string> decodeURIComponent(std::string_view str);

template <typename T>
std::optional<T> ValidateQueryString(const QueryString& queryString)
{
	static_assert(!std::is_same_v<T, T>, "Specialization required.");
	return std::nullopt;
}

template <typename T, typename Validator = GenericValidator<T>>
std::optional<T> ValidateQueryString(const QueryString& queryString, std::string_view key, Validator validator = GenericValidator<T>{})
{
	const auto itEnd = queryString.end();
	auto itParam = std::find_if(queryString.begin(), itEnd, [key](const std::pair<std::string_view, std::string_view>& param) -> bool {
		return param.first == key;
	});
	if (itParam == itEnd) {
		return std::nullopt;
	} else {
		auto valeurDecodee = decodeURIComponent(itParam->second);
		if (valeurDecodee) {
			return validator(std::string_view{*valeurDecodee});
		} else {
			return std::nullopt;
		}
	}
}

struct QueryStringValidatorBase
{
	QueryString getQueryParams(std::string_view) const;
};

template <typename T>
struct QueryStringValidator : private QueryStringValidatorBase
{
	std::optional<T> operator()(std::string_view sv) const
	{
		return ValidateQueryString<T>(getQueryParams(sv));
	}
};

#endif

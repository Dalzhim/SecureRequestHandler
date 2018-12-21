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
#include <vector>

using QueryString = std::vector<std::pair<std::string_view, std::string_view>>;

struct QueryStringValidatorBase
{
	QueryString getQueryParams(std::string_view) const;
};

static std::optional<unsigned int> parseHexDigit(unsigned char c)
{
	if (c >= '0' && c <= '9') {
		return {c - '0'};
	} else if (c >= 'a' && c <= 'f') {
		return {c - 'a' + 10};
	} else if (c >= 'A' && c <= 'F') {
		return {c - 'A' + 10};
	}
	return std::nullopt;
}

inline std::optional<std::string> decodeURIComponent(std::string_view str)
{
	std::string result;
	const int maxPositionLastPercent = str.length() - 2;
	
	for (std::string::size_type i = 0, n = str.length(); i < n; i++) {
		char c = str[i];
		if (c != '%'){
			if (c == '+')
				result += ' ';
			else
				result += c;
		} else {
			if (i < maxPositionLastPercent) {
				const auto d1Opt = parseHexDigit(str[i+1]);
				const auto d2Opt = parseHexDigit(str[i+2]);
				if (d1Opt && d2Opt) {
					result += static_cast<char>(*d1Opt * 16 + *d2Opt);
					i += 2;
				} else {
					return std::nullopt;
				}
			} else {
				return std::nullopt;
			}
		}
	}
	return result;
}

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

template <typename T>
struct QueryStringValidator : private QueryStringValidatorBase
{
	std::optional<T> operator()(std::string_view sv) const
	{
		return ValidateQueryString<T>(getQueryParams(sv));
	}
};

#endif

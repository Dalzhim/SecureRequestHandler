#include "QueryStringSerializer.hpp"

#include <numeric>

std::string encodeURIComponent(std::string_view sv)
{
	std::string result;
	for (unsigned char c : sv) {
		if	(
					(c >= '0' && c <= '9')
					|| (c >= 'a' && c <= 'z')
					|| (c >= 'A' && c <= 'Z')
				 ) {
			result += c;
		} else {
			unsigned char upper = c >> 4;
			unsigned char lower = c & 0xF;
			result += '%';
			result += ((upper >= 10) ? ('a' - 10) : '0') + upper;
			result += ((lower >= 10) ? ('a' - 10) : '0') + lower;
		}
	}
	return result;
}

std::size_t estimateEncodedStringLength(const QueryStringBuffer& qs)
{
	auto rawLength = std::accumulate(qs.begin(), qs.end(), 0, [](int total, const std::pair<std::string_view, std::string_view>& element) -> int {
		return element.first.length() + (element.second.empty() ? 0 : element.second.length() + 1);
	});
	rawLength += qs.size() - 1;
	return rawLength * 2;
}

std::string makeIntoString(const QueryStringBuffer& qs)
{
	std::string result;
	result.reserve(estimateEncodedStringLength(qs));
	for (const auto& element : qs) {
		result += encodeURIComponent(element.first);
		if (!element.second.empty()) {
			result += '=';
			result += encodeURIComponent(element.second);
		}
		result += '&';
	}
	if (!qs.empty()) {
		result.pop_back();
	}
	return result;
}

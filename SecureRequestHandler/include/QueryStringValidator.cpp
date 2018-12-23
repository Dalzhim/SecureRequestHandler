/*
 * Copyright 2013-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file has a member function implementation coming from the Folly library
 * which was adaptated into the Validator concept.
 */

#include "QueryStringValidator.hpp"
#include <boost/regex.hpp>

std::vector<std::pair<std::string_view, std::string_view>>
QueryStringValidatorBase::getQueryParams(std::string_view str) const
{
	std::vector<std::pair<std::string_view, std::string_view>> result;
	if (!str.empty()) {
		// Parse query string
		static const boost::regex queryParamRegex(
																							"(^|&)" /*start of query or start of parameter "&"*/
																							"([^=&]*)=?" /*parameter name and "=" if value is expected*/
																							"([^=&]*)" /*parameter value*/
																							"(?=(&|$))" /*forward reference, next should be end of query or
																													 start of next parameter*/);
		const boost::cregex_iterator paramBeginItr(str.data(), str.data() + str.size(), queryParamRegex);
		boost::cregex_iterator paramEndItr;
		for (auto itr = paramBeginItr; itr != paramEndItr; ++itr) {
			if (itr->length(2) == 0) {
				// key is empty, ignore it
				continue;
			}
			result.emplace_back(
													std::string_view{(*itr)[2].first, static_cast<std::string_view::size_type>(std::distance((*itr)[2].first, (*itr)[2].second))},
													std::string_view{(*itr)[3].first, static_cast<std::string_view::size_type>(std::distance((*itr)[3].first, (*itr)[3].second))});
		}
	}
	return result;
}

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

std::optional<std::string> decodeURIComponent(std::string_view str)
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

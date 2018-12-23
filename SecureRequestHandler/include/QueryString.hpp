#ifndef QUERY_STRING_HPP
#define QUERY_STRING_HPP

#include <string>
#include <string_view>
#include <utility>
#include <vector>

using QueryString = std::vector<std::pair<std::string_view, std::string_view>>;
using QueryStringBuffer = std::vector<std::pair<std::string, std::string>>;

#endif

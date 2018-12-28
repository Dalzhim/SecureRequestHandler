#ifndef QUERY_STRING_SERIALIZER_HPP
#define QUERY_STRING_SERIALIZER_HPP

#include "GenericSerializer.hpp"
#include "QueryString.hpp"
#include <string>
#include <string_view>

std::string encodeURIComponent(std::string_view sv);

template <typename T>
QueryStringBuffer SerializeQueryString(const T&)
{
	static_assert(!std::is_same_v<T, T>, "Specialization required.");
}

std::string makeIntoString(const QueryStringBuffer&);

template <typename T>
struct QueryStringSerializer
{
	using value_type = T;
	
	std::string operator()(const T& t) const
	{
		return makeIntoString(SerializeQueryString<T>(t));
	}
};

#endif

#ifndef JSON_SERIALIZER_HPP
#define JSON_SERIALIZER_HPP

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <string>
#include <type_traits>

template <typename T>
rapidjson::Value SerializeJSON(const T&, rapidjson::Document::AllocatorType&)
{
	static_assert(!std::is_same_v<T, T>, "Specialization required.");
}

template <>
rapidjson::Value SerializeJSON<bool>(const bool&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<int>(const int&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<long>(const long&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<long long>(const long long&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<unsigned int>(const unsigned int&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<unsigned long>(const unsigned long&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<unsigned long long>(const unsigned long long&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<float>(const float&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<double>(const double&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<long double>(const long double&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<std::string>(const std::string&, rapidjson::Document::AllocatorType&);
template <>
rapidjson::Value SerializeJSON<std::string_view>(const std::string_view&, rapidjson::Document::AllocatorType&);

template <typename T>
struct JSONSerializer
{
	using value_type = T;
	
	std::string operator()(const T& t) const
	{
		rapidjson::Document doc{rapidjson::kObjectType};
		rapidjson::Value& val = doc;
		val = SerializeJSON<T>(t, doc.GetAllocator());
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};
		doc.Accept(writer);
		return std::string{buffer.GetString(), buffer.GetLength()};
	}
};

#endif

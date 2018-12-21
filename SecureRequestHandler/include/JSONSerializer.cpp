#include "JSONSerializer.hpp"

template <>
rapidjson::Value SerializeJSON<bool>(const bool& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(v);
}

template <>
rapidjson::Value SerializeJSON<int>(const int& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(v);
}

template <>
rapidjson::Value SerializeJSON<long>(const long& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(static_cast<int64_t>(v));
}

template <>
rapidjson::Value SerializeJSON<long long>(const long long& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(v);
}

template <>
rapidjson::Value SerializeJSON<unsigned int>(const unsigned int& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(v);
}

template <>
rapidjson::Value SerializeJSON<unsigned long>(const unsigned long& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(static_cast<uint64_t>(v));
}

template <>
rapidjson::Value SerializeJSON<unsigned long long>(const unsigned long long& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(v);
}

template <>
rapidjson::Value SerializeJSON<float>(const float& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(v);
}

template <>
rapidjson::Value SerializeJSON<double>(const double& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(v);
}

template <>
rapidjson::Value SerializeJSON<long double>(const long double& v, rapidjson::Document::AllocatorType&)
{
	return rapidjson::Value(static_cast<double>(v));
}

template <>
rapidjson::Value SerializeJSON<std::string>(const std::string& v, rapidjson::Document::AllocatorType& allocator)
{
	return rapidjson::Value(v.c_str(), v.size(), allocator);
}

template <>
rapidjson::Value SerializeJSON<std::string_view>(const std::string_view& v, rapidjson::Document::AllocatorType& allocator)
{
	return rapidjson::Value(v.data(), v.size(), allocator);
}

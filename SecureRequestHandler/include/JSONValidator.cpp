#include "JSONValidator.hpp"

template <>
std::optional<bool> ValidateJSON<bool>(const rapidjson::Value& json)
{
	if (json.IsBool()) {
		return json.GetBool();
	}
	return std::nullopt;
}

template <>
std::optional<int> ValidateJSON<int>(const rapidjson::Value& json)
{
	if (json.IsNumber()) {
		return json.GetInt();
	}
	return std::nullopt;
}

template <>
std::optional<int64_t> ValidateJSON<int64_t>(const rapidjson::Value& json)
{
	if (json.IsNumber()) {
		return json.GetInt64();
	}
	return std::nullopt;
}

template <>
std::optional<unsigned int> ValidateJSON<unsigned int>(const rapidjson::Value& json)
{
	if (json.IsNumber()) {
		return json.GetUint();
	}
	return std::nullopt;
}

template <>
std::optional<uint64_t> ValidateJSON<uint64_t>(const rapidjson::Value& json)
{
	if (json.IsNumber()) {
		return json.GetUint64();
	}
	return std::nullopt;
}

template <>
std::optional<float> ValidateJSON<float>(const rapidjson::Value& json)
{
	if (json.IsNumber()) {
		return json.GetFloat();
	}
	return std::nullopt;
}

template <>
std::optional<double> ValidateJSON<double>(const rapidjson::Value& json)
{
	if (json.IsNumber()) {
		return json.GetDouble();
	}
	return std::nullopt;
}

template <>
std::optional<std::string> ValidateJSON<std::string>(const rapidjson::Value& json)
{
	if (json.IsString()) {
		return std::string{json.GetString(), json.GetStringLength()};
	}
	return std::nullopt;
}

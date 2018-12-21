#ifndef JSON_VALIDATOR_HPP
#define JSON_VALIDATOR_HPP

#include <optional>
#include <rapidjson/document.h>
#include <string>
#include <string_view>
#include <type_traits>

template <typename T>
struct JSONValidator;

template <typename T>
std::optional<T> ValidateJSON(const rapidjson::Value&)
{
	static_assert(!std::is_same_v<T, T>, "Specialization required.");
	return std::nullopt;
}

template <>
std::optional<bool> ValidateJSON<bool>(const rapidjson::Value& json);
template <>
std::optional<int> ValidateJSON<int>(const rapidjson::Value& json);
template <>
std::optional<int64_t> ValidateJSON<int64_t>(const rapidjson::Value& json);
template <>
std::optional<unsigned int> ValidateJSON<unsigned int>(const rapidjson::Value& json);
template <>
std::optional<uint64_t> ValidateJSON<uint64_t>(const rapidjson::Value& json);
template <>
std::optional<float> ValidateJSON<float>(const rapidjson::Value& json);
template <>
std::optional<double> ValidateJSON<double>(const rapidjson::Value& json);
template <>
std::optional<std::string> ValidateJSON<std::string>(const rapidjson::Value& json);

template <typename T>
std::optional<T> ValidateJSON(const rapidjson::Value& json, std::string_view key)
{
	if (json.IsObject()) {
		const auto it = json.FindMember(key.data());
		const auto itEnd = json.MemberEnd();
		if (it != itEnd) {
			return ValidateJSON<T>(it->value);
		}
	}
	return std::nullopt;
}

template <typename T>
std::optional<std::vector<T>> ValidateJSONArray(const rapidjson::Value& json)
{
	if (json.IsArray()) {
		std::vector<T> elements;
		const auto size = json.Size();
		elements.reserve(size);
		for (rapidjson::SizeType i = 0; i < size; ++i) {
			auto elemOpt = ValidateJSON<T>(json[i]);
			if (elemOpt) {
				elements.push_back(std::move(*elemOpt));
			} else {
				return std::nullopt;
			}
		}
		return elements;
	}
	return std::nullopt;
}

template <typename T>
std::optional<std::vector<T>> ValidateJSONArray(const rapidjson::Value& json, std::string_view key)
{
	const auto it = json.FindMember(key.data());
	const auto itEnd = json.MemberEnd();
	if (it == itEnd) {
		return std::nullopt;
	} else {
		return ValidateJSONArray<T>(it->value);
	}
}

template <typename T>
struct JSONValidator
{
	std::optional<T> operator()(std::string_view sv) const
	{
		rapidjson::Document doc;
		if (!doc.Parse(sv.data(), sv.size()).HasParseError()) {
			return ValidateJSON<T>(doc);
		}
		return std::nullopt;
	}
};

#endif

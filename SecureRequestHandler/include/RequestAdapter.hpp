#ifndef REQUEST_ADAPTER_HPP
#define REQUEST_ADAPTER_HPP

#include <string_view>
#include <type_traits>

template <typename RequestType>
struct RequestAdapter
{
	using request_type = RequestType;
	using status_type = unsigned int;
	
	constexpr static status_type BadRequest = 400u;
	constexpr static status_type Ok = 200u;
	
	static std::string_view getHeader(const request_type&, std::string_view)
	{
		static_assert(!std::is_same_v<RequestType, RequestType>, "Specialize RequestAdapter for your http library.");
	}
	
	static std::string_view getBody(const request_type&)
	{
		static_assert(!std::is_same_v<RequestType, RequestType>, "Specialize RequestAdapter for your http library.");
	}
	
	static std::string_view getPath(const request_type&)
	{
		static_assert(!std::is_same_v<RequestType, RequestType>, "Specialize RequestAdapter for your http library.");
	}
	
	static std::string_view getQueryString(const request_type&)
	{
		static_assert(!std::is_same_v<RequestType, RequestType>, "Specialize RequestAdapter for your http library.");
	}
	
	static std::string_view getVerb(const request_type&)
	{
		static_assert(!std::is_same_v<RequestType, RequestType>, "Specialize RequestAdapter for your http library.");
	}
	
	template <typename SerializerType, typename Send>
	static auto makeSender(const RequestType&, const Send&)
	{
		static_assert(!std::is_same_v<RequestType, RequestType>, "Specialize RequestAdapter for your http library.");
	}
};

#endif

#ifndef REQUEST_ADAPTER_HPP
#define REQUEST_ADAPTER_HPP

#include <string_view>
#include <type_traits>

template <typename RequestType>
struct RequestAdapter
{
	using request_type = RequestType;
	using response_type = void;
	using status_type = unsigned int;
	
	template <typename SerializerType>
	using make_response_type = void;
	
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
};

#endif

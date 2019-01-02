#ifndef BEAST_REQUEST_ADAPTER_HPP
#define BEAST_REQUEST_ADAPTER_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include "RequestAdapter.hpp"
#include "SecureRequestHandler.hpp"
#include <string_view>
#include <type_traits>

template <typename RequestType, typename SerializerType>
struct BeastMakeResponse
{
	using request_type = RequestType;
	using response_body_type = boost::beast::http::string_body;
	using response_type = boost::beast::http::response<response_body_type>;
	
	BeastMakeResponse(const request_type& req) : req{req}
	{}
	
	template <typename ValueType>
	auto operator()(ValueType&& val)
	{
		response_type response{boost::beast::http::status::ok, req.version()};
		static_assert(!std::is_same_v<typename SerializerType::value_type, void>, "Can't provide a body for Output<void, /* … */>");
		if constexpr (!std::is_same_v<typename SerializerType::value_type, void>) {
			response.body() = SerializerType{}(val);
			response.prepare_payload();
		}
		return response;
	}
	
	auto operator()(boost::beast::http::status status)
	{
		return response_type{status, req.version()};
	}
	
	template <typename ValueType>
	auto operator()(boost::beast::http::status status, ValueType&& val)
	{
		response_type response{status, req.version()};
		static_assert(!std::is_same_v<typename SerializerType::value_type, void>, "Can't provide a body for Output<void, /* ... */>");
		if constexpr (!std::is_same_v<typename SerializerType::value_type, void>) {
			response.body() = SerializerType{}(val);
			response.prepare_payload();
		}
		return response;
	}
	
	const request_type& req;
};

template <>
struct RequestAdapter<boost::beast::http::request<boost::beast::http::string_body>>
{
	using request_body_type = boost::beast::http::string_body;
	using request_type = boost::beast::http::request<request_body_type>;
	using response_body_type = boost::beast::http::string_body;
	using response_type = boost::beast::http::response<response_body_type>;
	using status_type = boost::beast::http::status;
	
	constexpr static status_type BadRequest = status_type::bad_request;
	constexpr static status_type Ok = status_type::ok;
	
	static std::string_view getHeader(const request_type& req, std::string_view name)
	{
		auto itEnd = req.end();
		auto it = req.find(boost::string_view{name.data(), name.size()});
		if (it != itEnd) {
			const auto sv = it->value();
			return std::string_view{sv.data(), sv.size()};
		}
		return std::string_view{};
	}
	
	static std::string_view getBody(const request_type& req)
	{
		static_assert(!std::is_same_v<request_body_type, std::string>, "Boost::Beast request adapter only supports boost::beast::http::string_body and boost::beast::http::empty_body");
		const auto sv = std::string_view{req.body()};
		return std::string_view{sv.data(), sv.size()};
	}
	
	static std::string_view getPath(const request_type& req)
	{
		const auto sv = req.target();
		const auto itBegin = sv.begin();
		const auto itEnd = sv.end();
		auto itSeparateur = std::find(itBegin, itEnd, '?');
		const auto distance = std::distance(itBegin, itSeparateur);
		return std::string_view{sv.data(), static_cast<std::string_view::size_type>(distance)};
	}
	
	static std::string_view getQueryString(const request_type& req)
	{
		const auto sv = req.target();
		const auto itBegin = sv.begin();
		const auto itEnd = sv.end();
		auto itSeparateur = std::find(itBegin, itEnd, '?');
		if (itSeparateur != itEnd) {
			const auto distance = std::distance(itBegin, itSeparateur) + 1;
			return std::string_view{sv.data() + distance, static_cast<std::string_view::size_type>(sv.size() - distance)};
		} else {
			return std::string_view{sv.data() + sv.size(), 0};
		}
	}
	
	static std::string_view getVerb(const request_type& req)
	{
		const auto sv = req.method_string();
		return std::string_view{sv.data(), sv.size()};
	}
	
	template <typename SerializerType>
	using make_response_type = BeastMakeResponse<request_type, SerializerType>;
};

template <typename OutputDesc, typename ... InputDesc>
using BeastRequestHandler = RequestHandler<boost::beast::http::request<boost::beast::http::string_body>, OutputDesc, InputDesc...>;

#endif

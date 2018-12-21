#ifndef BEAST_REQUEST_ADAPTER_HPP
#define BEAST_REQUEST_ADAPTER_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include "RequestAdapter.hpp"
#include "SecureRequestHandler.hpp"
#include <string_view>
#include <type_traits>

// This is the C++11 equivalent of a generic lambda.
// The function object is used to send an HTTP message.
template<class Stream>
struct send_lambda
{
	Stream& stream_;
	bool& close_;
	boost::system::error_code& ec_;
	
	explicit
	send_lambda(
							Stream& stream,
							bool& close,
							boost::system::error_code& ec)
	: stream_(stream)
	, close_(close)
	, ec_(ec)
	{}
	
	template<bool isRequest, class Body, class Fields>
	void
	operator()(boost::beast::http::message<isRequest, Body, Fields>&& msg) const
	{
		// Determine if we should close the connection after
		close_ = msg.need_eof();
		
		// We need the serializer here because the serializer requires
		// a non-const file_body, and the message oriented version of
		// http::write only works with const messages.
		boost::beast::http::serializer<isRequest, Body, Fields> sr{msg};
		boost::beast::http::write(stream_, sr, ec_);
	}
};

template <typename RequestType, typename SerializerType, typename Send>
struct BeastSendResponse
{
	BeastSendResponse(const RequestType& req, const Send& send) : req{req}, send{send}
	{}
	
	template <typename ValueType>
	auto operator()(ValueType&& val)
	{
		boost::beast::http::response<boost::beast::http::string_body> response{boost::beast::http::status::ok, req.version()};
		response.body() = SerializerType{}(val);
		response.prepare_payload();
		return send(std::move(response));
	}
	
	auto operator()(boost::beast::http::status status)
	{
		return send(
			boost::beast::http::response<boost::beast::http::empty_body>{status, req.version()}
		);
	}
	
	template <typename ValueType>
	auto operator()(boost::beast::http::status status, ValueType&& val)
	{
		boost::beast::http::response<boost::beast::http::string_body> response{status, req.version()};
		response.body() = SerializerType{}(val);
		response.prepare_payload();
		return send(std::move(response));
	}
	
	const RequestType& req;
	const Send& send;
};

template <typename BodyType>
struct RequestAdapter<boost::beast::http::request<BodyType>>
{
	using request_type = boost::beast::http::request<BodyType>;
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
		static_assert(!std::is_same_v<BodyType, std::string>, "Boost::Beast request adapter only supports boost::beast::http::string_body and boost::beast::http::empty_body");
		const auto sv = std::string_view{req.body()};
		return std::string_view{sv.data(), sv.size()};
	}
	
	static std::string_view getPath(const request_type& req)
	{
		const auto sv = req.target();
		return std::string_view{sv.data(), sv.size()};
	}
	
	static std::string_view getVerb(const request_type& req)
	{
		const auto sv = req.method_string();
		return std::string_view{sv.data(), sv.size()};
	}
	
	template <typename SerializerType, typename Send>
	using sender_type = BeastSendResponse<request_type, SerializerType, Send>;
	
	template <typename SerializerType, typename Send>
	static auto makeSender(const request_type& req, const Send& send)
	{
		return BeastSendResponse<request_type, SerializerType, Send>{req, send};
	}
};

template <typename OutputDesc, typename ... InputDesc>
using BeastRequestHandler = RequestHandler<boost::beast::http::request<boost::beast::http::string_body>, send_lambda<boost::asio::ip::tcp::socket>, OutputDesc, InputDesc...>;

#endif

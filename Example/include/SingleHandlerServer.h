#ifndef SINGLE_HANDLER_SERVER_H
#define SINGLE_HANDLER_SERVER_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <tuple>
#include <utility>

using status_type = boost::beast::http::status;
using return_type = std::tuple<status_type, std::optional<std::string>>;
using body_type = boost::beast::http::string_body;
using request_type = boost::beast::http::request<body_type>;
using handler_type = std::function<return_type(request_type)>;

int handleRequests(handler_type);

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

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<
class HandlerType,
class Body, class Allocator,
class Send>
void
handle_request(
							 boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req,
							 Send&& send,
							 HandlerType handler)
{
	send(handler(req));
}

//------------------------------------------------------------------------------

// Report a failure
inline void
fail(boost::system::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

// Handles an HTTP server connection
template <typename HandlerType>
void
do_session(boost::asio::ip::tcp::socket& socket, HandlerType handler)
{
	bool close = false;
	boost::system::error_code ec;
	
	// This buffer is required to persist across reads
	boost::beast::flat_buffer buffer;
	
	// This lambda is used to send messages
	send_lambda<boost::asio::ip::tcp::socket> lambda{socket, close, ec};
	
	for(;;)
	{
		// Read a request
		boost::beast::http::request<boost::beast::http::string_body> req;
		boost::beast::http::read(socket, buffer, req, ec);
		if(ec == boost::beast::http::error::end_of_stream)
			break;
		if(ec)
			return fail(ec, "read");
		
		// Send the response
		handle_request(std::move(req), lambda, handler);
		if(ec)
			return fail(ec, "write");
		if(close)
		{
			// This means we should close the connection, usually because
			// the response indicated the "Connection: close" semantic.
			break;
		}
	}
	
	// Send a TCP shutdown
	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
	
	// At this point the connection is closed gracefully
}

template <typename HandlerType>
int handleRequests(const HandlerType& handler)
{
	try
	{
		auto const address = boost::asio::ip::make_address("0.0.0.0");
		auto const port = static_cast<unsigned short>(std::atoi("8080"));
		
		// The io_context is required for all I/O
		boost::asio::io_context ioc{1};
		
		// The acceptor receives incoming connections
		boost::asio::ip::tcp::acceptor acceptor{ioc, {address, port}};
		for(;;)
		{
			// This will receive the new connection
			boost::asio::ip::tcp::socket socket{ioc};
			
			// Block until we get a connection
			acceptor.accept(socket);
			
			// Launch the session, transferring ownership of the socket
			std::thread{std::bind(
														&do_session<HandlerType>,
														std::move(socket),
														handler)}.detach();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

#endif

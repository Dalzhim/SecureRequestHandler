#ifndef SECURE_REQUEST_HANDLER_HPP
#define SECURE_REQUEST_HANDLER_HPP

#include "GenericSerializer.hpp"
#include "GenericValidator.hpp"
#include "JSONSerializer.hpp"
#include "JSONValidator.hpp"
#include <optional>
#include "QueryStringSerializer.hpp"
#include "QueryStringValidator.hpp"
#include "RequestAdapter.hpp"
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include "typestring.h"

/**
 * Syntax summary :
 *   RequestHandler<RequestType, SendType, OutputDesc, InputDesc ...>
 *   RequestType is specific to your HTTP library,
 *               RequestAdapter must be specialized with RequestType
 *   SendType is a type that can be invoked to send a response, it is specific to your
 *            HTTP library and RequestAdapter must be specialized to use it
 *   OutputDesc<ContentType, GenericSerializer>
 *   InputDesc<ValueType, Source, GenericValidator>
 *   InputDesc<ValueType, Source, JSONValidator>
 *   InputDesc<ValueType, Source, QueryStringValidator>
 *   Source => HeaderParam<typestring_is("host")> | BodyParam | VerbParam | PathParam
 * Usage example :
 *
 
 RequestHandler<
	 OutputDesc<std::string>,
	 InputDesc<std::string_view, HeaderParam<typestring_is("host")>>,
	 InputDesc<CustomerInfo, HeaderParam<typestring_is("customer")>, JSONValidator>,
	 InputDesc<CustomerInfo, BodyParam, QueryStringValidator>,
	 InputDesc<std::string_view, VerbParam>,
	 InputDesc<std::string_view, PathParam>
 > reqHandler{
 	[](auto send, auto p1, auto p2, auto p3, auto p4, auto p5) -> void {
 		std::cout << p1 << '\n' << p2 << '\n' << p3 << '\n' << p4 << '\n' << p5 << '\n';
 		send(typename RequestAdapter<RequestType>::Ok, "TEST\n");
 	}
 };
 
 *
 */

template <typename Key>
struct HeaderParam
{
	static_assert(!std::is_same_v<Key, typestring_is("")>, "Reading from headers requires a name parameter.");
	
	template <typename T>
	using default_validator_type = GenericValidator<T>;
	
	template <typename RequestType>
	std::string_view operator()(const RequestType& req) const
	{
		Key key;
		const auto name = std::string_view{key.data(), key.size()};
		return RequestAdapter<RequestType>::getHeader(req, name);
	}
};
struct PathParam
{
	template <typename T>
	using default_validator_type = GenericValidator<T>;
	
	template <typename RequestType>
	std::string_view operator()(const RequestType& req) const
	{
		return RequestAdapter<RequestType>::getPath(req);
	}
};
struct QueryStringParam
{
	template <typename T>
	using default_validator_type = QueryStringValidator<T>;
	
	template <typename RequestType>
	std::string_view operator()(const RequestType& req) const
	{
		return RequestAdapter<RequestType>::getQueryString(req);
	}
};
struct VerbParam
{
	template <typename T>
	using default_validator_type = GenericValidator<T>;
	
	template <typename RequestType>
	std::string_view operator()(const RequestType& req) const
	{
		return RequestAdapter<RequestType>::getVerb(req);
	}
};
struct BodyParam
{
	template <typename T>
	using default_validator_type = GenericValidator<T>;
	
	template <typename RequestType>
	std::string_view operator()(const RequestType& req) const
	{
		return RequestAdapter<RequestType>::getBody(req);
	}
};

template <typename T, template <typename> typename Serializer = GenericSerializer>
struct OutputDesc
{
	using value_type = T;
	using serializer_type = Serializer<T>;
};

template <typename T, typename Source, template <typename> typename Validator = Source::template default_validator_type>
struct InputDesc
{
	using value_type = T;
	using opt_value_type = std::optional<T>;
	using source_type = Source;
	using validator_type = Validator<T>;
	
	template <typename RequestType>
	opt_value_type operator()(const RequestType& req) const
	{
		validator_type validator{};
		source_type readSource{};
		return validator(readSource(req));
	}
};

namespace detail
{
	template <typename Output, typename ... Inputs, typename RequestType, typename Handler, std::size_t ... Is>
	auto invokeHandlerImpl(const RequestType& req, Handler&& handler, std::index_sequence<Is...>) -> typename RequestAdapter<RequestType>::response_type
	{
		using serializer_type = typename Output::serializer_type;
		using make_response_type = typename RequestAdapter<RequestType>::template make_response_type<serializer_type>;
		static_assert(sizeof...(Inputs) == sizeof...(Is));
		
		std::tuple<std::optional<typename Inputs::value_type>...> params;
		if (((std::get<Is>(params) = Inputs{}(req)) && ...)) {
			return std::invoke(
				handler,
				make_response_type{req},
				(*std::get<Is>(params))...
			);
		} else {
			return make_response_type{req}(RequestAdapter<RequestType>::BadRequest);
		}
	}
	
	template <typename Output, typename ... Inputs, typename RequestType, typename Handler>
	auto invokeHandler(const RequestType& req, Handler&& handler) -> typename RequestAdapter<RequestType>::response_type
	{
		return invokeHandlerImpl<Output, Inputs...>(
			req,
			std::forward<Handler>(handler),
			std::index_sequence_for<Inputs...>{}
		);
	}
}

template <typename Output, typename ... Inputs, typename RequestType, typename Handler>
auto handleRequest(const RequestType& req, Handler&& handler) -> typename RequestAdapter<RequestType>::response_type
{
	return detail::invokeHandler<Output, Inputs...>(
		req,
		std::forward<Handler>(handler)
	);
}

template <typename RequestType, typename Output, typename ... Inputs>
struct RequestHandler
{
	using request_adapter = RequestAdapter<RequestType>;
	using serializer_type = typename Output::serializer_type;
	using response_type = typename request_adapter::response_type;
	using make_response_type = typename request_adapter::template make_response_type<serializer_type>;
	using handler_type = std::function<response_type(make_response_type, typename Inputs::value_type...)>;
	
	RequestHandler(handler_type&& handler) : handler(std::forward<handler_type>(handler)) {}
	
	response_type operator()(const RequestType& req) const
	{
		return detail::invokeHandler<Output, Inputs...>(
			req,
			handler
		);
	}
	
	handler_type handler;
};

#endif


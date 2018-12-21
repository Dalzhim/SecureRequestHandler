#ifndef REST_HPP
#define REST_HPP

#include <optional>
#include "EnsureSendWasInvoked.hpp"
#include "GenericSerializer.hpp"
#include "GenericValidator.hpp"
#include "JSONSerializer.hpp"
#include "JSONValidator.hpp"
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
	template <typename RequestType>
	std::string_view operator()(const RequestType& req) const
	{
		return RequestAdapter<RequestType>::getPath(req);
	}
};
struct VerbParam
{
	template <typename RequestType>
	std::string_view operator()(const RequestType& req) const
	{
		return RequestAdapter<RequestType>::getVerb(req);
	}
};
struct BodyParam
{
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

template <typename T, typename Source, template <typename> typename Validator = GenericValidator>
struct InputDesc
{
	using value_type = T;
	using source_type = Source;
	using validator_type = Validator<T>;
};

namespace detail
{
//	Replace LazyInput?
//	
//	template <typename Arg, typename... Ops, std::size_t... Is>
//	auto apply_impl(std::index_sequence<Is...>, Arg &&arg, Ops&&... ops) {
//		std::tuple<decltype(std::declval<Ops>()(std::forward<Arg>(arg)))...> result;
//		((std::get<Is>(result) = ops(arg)) && ...);
//		return result;
//	}
//
//	template <typename Arg, typename... Ops>
//	auto apply(Arg &&arg, Ops&&... ops) {
//		return apply_impl(std::index_sequence_for<Ops...>{},
//											std::forward<Arg>(arg), std::forward<Ops>(ops)...);
//	}
	template <typename Input>
	struct LazyInput
	{
		using value_type = typename Input::value_type;
		using validator_type = typename Input::validator_type;
		using source_type = typename Input::source_type;
		
		template <typename RequestType>
		bool initialize(const RequestType& req)
		{
			value = validator_type{}(source_type{}.template operator()(req));
			return static_cast<bool>(value);
		}
		
		std::optional<value_type> value;
	};
	
	template <typename Output, typename ... Inputs, typename RequestType, typename Send, typename Handler, std::size_t ... Is>
	auto invokeHandlerImpl(const RequestType& req, const Send& send, Handler&& handler, std::index_sequence<Is...>) -> SendWasInvoked
	{
		static_assert(sizeof...(Inputs) == sizeof...(Is));
		std::tuple<LazyInput<Inputs>...> params;
		if ((... && std::get<Is>(params).initialize(req))) {
			return std::invoke(
				handler,
				RequestAdapter<RequestType>::template makeSender<typename Output::serializer_type, EnsureSendWasInvoked<Send>>(
					req,
					EnsureSendWasInvoked<Send>{send}
				),
				(*std::get<Is>(params).value)...
			);
		} else {
			return RequestAdapter<RequestType>::template makeSender<typename Output::serializer_type, EnsureSendWasInvoked<Send>>(
				req,
				EnsureSendWasInvoked<Send>{send}
			)(RequestAdapter<RequestType>::BadRequest);
		}
	}
	
	template <typename Output, typename ... Inputs, typename RequestType, typename Send, typename Handler>
	auto invokeHandler(const RequestType& req, const Send& send, Handler&& handler) -> void
	{
		invokeHandlerImpl<Output, Inputs...>(
			req,
			send,
			std::forward<Handler>(handler),
			std::index_sequence_for<Inputs...>{}
		);
	}
}

template <typename Output, typename ... Inputs, typename RequestType, typename Send, typename Handler>
auto handleRequest(const RequestType& req, const Send& send, Handler&& handler) -> void
{
	return detail::invokeHandler<Output, Inputs...>(
		req,
		send,
		std::forward<Handler>(handler)
	);
}

template <typename RequestType, typename Send, typename Output, typename ... Inputs>
struct RequestHandler
{
	using serializer_type = typename Output::serializer_type;
	using sender_type = typename RequestAdapter<RequestType>::template sender_type<serializer_type, EnsureSendWasInvoked<Send>>;
	using handler_type = std::function<SendWasInvoked(sender_type, typename Inputs::value_type...)>;
	
	RequestHandler(handler_type&& handler) : handler(std::forward<handler_type>(handler)) {}
	
	void operator()(const RequestType& req, const Send& send) const
	{
		return detail::invokeHandler<Output, Inputs...>(
			req,
			send,
			handler
		);
	}
	
	handler_type handler;
};

#endif


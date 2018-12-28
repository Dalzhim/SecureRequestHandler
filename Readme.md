# Abstract

This library is meant to make it easier to write HTTP Request Handlers without forgetting to sanitize your inputs. These core principles are baked in the design :

1. Decouple input validation and request handling
1. Deprive the request handler direct access to the `request` object
1. Gain access to validated inputs through a descriptive interface
1. Promote the use of types to describe inputs

While this library doesn't solve insufficient validation, it empowers the programmer so as to never forget to apply some form of validation to every input.

# Motivation

This library is meant as a proof of concept to demonstrate we can write safer HTTP Request Handlers by leveraging C++'s type system and declarative programming. It does so by providing the `RequestHandler` type which wraps a user-defined handler and handles the input validation and output serialization so that user code can stick to well defined C++ types.
The example program demonstrates the flexibility of the library by implementing a single handler server.

There is an increasing amount of new C++ libraries geared towards hosting HTTP servers and there are many attempts at writing complete web frameworks that glue together the HTTP server, the REST Router and the JSON library.

This library is meant to be combinable with any solution you have already chosen. It is a means of generating an invocable wrapper (`void operator()(request, send)`) around your request handler (`void operator()(send, input1, input2, /* ... */, inputN)`). That wrapper handles the validation so that your request handler is only ever invoked with valid input. The `send` parameter is discussed later on.

# Quick glance

Here is some sample code which implements a simple request handler that expects a request body that contains a query string such as what browsers send when submitting a `<form>` with `Content-Type: application/x-www-form-urlencoded`. It is going to decode a user type name `MyType` so that the handler is only ever invoked with a valid instance of that type. In order to validate a user supplied type, there are templates which must be specialized, in this case : `ValidateQueryString`.

```
struct MyType
{
	int value;
};

template <>
std::optional<MyType> ValidateQueryString<MyType>(const QueryString& queryString)
{
	auto valueOpt = ValidateQueryString<int>(queryString, "value");
	if (valueOpt) {
		return MyType{*valueOpt};
	}
	return std::nullopt;
}

BeastRequestHandler<
	OutputDesc<std::string>,
	InputDesc<MyType, BodyParam, QueryStringValidator>
> reqHandler{
	[](auto send, auto myType) {
		std::cout << myType << '\n';
		return send(boost::beast::http::status::ok, "Success!");
	}
};
```

# Input descriptor

The `InputDesc` template is used to declare inputs, what type it is represented with, where to read from in the HTTP request and how to validate. In other words : `InputDesc<value_type, source_type, validator_type>`.

The `value_type` is going to be sent to the user-declared handler after all inputs have been validated.

The `source_type` describes the various locations in a HTTP request where we might want to read inputs, namely : `HeaderParam<typestring_is("key")>`, `BodyParam`, `VerbParam` and `PathParam`. The query string is currently part of `PathParam` because of how Boost::Beast handles HTTP requests, both will be separated in the future.

The `validator_type` is an invocable type with signature `std::optional<value_type> operator()(std::string_view)` that returns `nullopt` whenever validation fails.

# Output descriptor

The `OutputDesc` template is used to declare the output, what type it is represented with and how to serialize it. In other words : `OutputDesc<value_type, serializer_type>`.

The `value_type` is a user type that can be supplied as a parameter when invoking `send`.

The `serializer_type` is an invocable type with signature `std::string operator()(value_type)` so that a Handler can use the type system to describe its output.

Describing the output using the type system makes it possible to avoid problems where a handler produces different structures for different inputs. This kind of behavior is surprising and leads to mistakes. Hence it is best to describe the ouputs.

# Validators

There are three default validators provided with the library : `GenericValidator`, `JSONValidator` and `QueryStringValidator`.

`GenericValidator` expects the whole contents of an input to be deserializable to a single, unstructured type.

`JSONValidator` expects the input to be a valid JSON Object and provides a few default validators to extract primitive types and strings. In order to provide validators for user-defined types, one must specialize the `ValidateJSON` template function. Such specializations should always delegate the work to deserialize a sub-object to the appropriate specialization in order to prevent multiple levels of nesting in a single validator and also to apply the DRY principle.

`QueryStringValidator` expects the input to be valid `x-www-form-urlencoded` contents, handles percent-decoding of the values and provides a few default validators to extract primitive types and strings. In order to provide validators for user-defined types, one must specialize the `ValidateQueryString` template function using the same guidelines as those of `JSONValidator`.

# Serializers

There are two default validators provided with the library : `GenericSerializer` and `JSONSerializer`.

`GenericSerializer` generates a string in a single shot provided some certain type. Serialization of user-defined types is not encouraged, but possible by specializing `GenericSerialize`.

`JSONSerializer` generates a json object that can be serialized into the body of the response. In order to provide serializers for user-defined types, one must specialize the `SerializeJSON` template function. Such specializations should always delegate the work to serialize a sub-object to the appropriate specialization in order to apply the DRY principle.

# Dependencies

1. [Boost::Beast](https://github.com/boostorg/beast)
1. [rapidjson](https://github.com/Tencent/rapidjson/)
1. [irqus::typestring](https://github.com/irrequietus/typestring)

# Attributions

1. The typestring_is is a substitution for Non-Type Template Parameters coming in C++20. It comes from irrequietus's library : https://github.com/irrequietus/typestring/blob/8d52ee6ef21fcd353871661226d6f9e3312d1442/typestring.hh licensed under the Mozilla Public License 2.0.
1. The Query String validation code is taken from the Folly library : https://github.com/facebook/folly/blob/da78383fec6133dbda05e345919ebdfa939c65eb/folly/Uri.cpp#L132 licensed under the Apache License 2.0

# Limitations

1. Can't easily separate the Path and the Query String
1. Validation doesn't fail when a handler is invoked with unused inputs
1. Only provides an adapter for Boost::Beast
1. Compilation errors can be daunting
1. Sending responses is currently done synchronously when using the provided adapter for Beast

# Future work

1. Make use of `async_write` rather than `write` to send responses
1. Allow responses with custom headers
1. `OutputDesc<void>`
1. Leverage C++20 Concepts
1. Leverage C++20 Non-Type Template Parameters

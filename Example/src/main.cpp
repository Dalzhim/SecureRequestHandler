#include "Address.h"
#include "BeastRequestAdapter.hpp"
#include "CustomerInfo.h"
#include <iostream>
#include <optional>
#include "SecureRequestHandler.hpp"
#include "SingleHandlerServer.h"
#include <string>
#include <tuple>

template <>
std::optional<Address> ValidateQueryString<Address>(const QueryString& queryString)
{
	auto numberOpt = ValidateQueryString<int>(queryString, "number");
	auto streetOpt = ValidateQueryString<std::string>(queryString, "street");
	if (numberOpt && streetOpt) {
		return Address{*numberOpt, *streetOpt};
	}
	return std::nullopt;
}

template <>
std::optional<CustomerInfo> ValidateQueryString<CustomerInfo>(const QueryString& queryString)
{
	auto firstNameOpt = ValidateQueryString<std::string>(queryString, "firstName");
	auto lastNameOpt = ValidateQueryString<std::string>(queryString, "lastName");
	auto addressOpt = ValidateQueryString<Address>(queryString, "address", QueryStringValidator<Address>{});
	if (firstNameOpt && lastNameOpt && addressOpt) {
		return CustomerInfo{*firstNameOpt, *lastNameOpt, *addressOpt};
	}
	return std::nullopt;
}

template <>
std::optional<Address> ValidateJSON<Address>(const rapidjson::Value& json)
{
	auto numberOpt = ValidateJSON<int>(json, "number");
	auto streetOpt = ValidateJSON<std::string>(json, "street");
	if (numberOpt && streetOpt) {
		return Address{*numberOpt, *streetOpt};
	}
	return std::nullopt;
}

template <>
std::optional<CustomerInfo> ValidateJSON<CustomerInfo>(const rapidjson::Value& json)
{
	auto firstNameOpt = ValidateJSON<std::string>(json, "firstName");
	auto lastNameOpt = ValidateJSON<std::string>(json, "lastName");
	auto addressOpt = ValidateJSON<Address>(json, "address");
	if (firstNameOpt && lastNameOpt && addressOpt) {
		return CustomerInfo{*firstNameOpt, *lastNameOpt, *addressOpt};
	}
	return std::nullopt;
}

template <>
rapidjson::Value SerializeJSON<Address>(const Address& v, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value element{rapidjson::kObjectType};
	element.AddMember("number", SerializeJSON(v.number, allocator), allocator);
	element.AddMember("street", SerializeJSON(v.street, allocator), allocator);
	return element;
}

template <>
rapidjson::Value SerializeJSON<CustomerInfo>(const CustomerInfo& v, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value element{rapidjson::kObjectType};
	element.AddMember("firstName", SerializeJSON(v.firstName, allocator), allocator);
	element.AddMember("lastName", SerializeJSON(v.lastName, allocator), allocator);
	element.AddMember("address", SerializeJSON(v.address, allocator), allocator);
	return element;
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	BeastRequestHandler<
		OutputDesc<CustomerInfo, JSONSerializer>,
		InputDesc<std::string_view, HeaderParam<typestring_is("host")>>,
		InputDesc<CustomerInfo, HeaderParam<typestring_is("customer")>, JSONValidator>,
		InputDesc<CustomerInfo, BodyParam, QueryStringValidator>,
		InputDesc<std::string_view, VerbParam>,
		InputDesc<std::string_view, PathParam>
	> reqHandler{
		[](auto send, auto p1, auto p2, auto p3, auto p4, auto p5) {
			std::cout << p1 << '\n' << p2 << '\n' << p3 << '\n' << p4 << '\n' << p5 << '\n';
			return send(boost::beast::http::status::ok, p2);
		}
	};
	std::cout << "Server is starting up... Use the following command to try it out...\n"
						<< "curl -v \"http://127.0.0.1:8080/Exemple\" -d \"firstName=Gabriel&lastName=Aubut-Lussier&address=number%3D25%26street%3DC%252B%252B%2520Montr%25C3%25A9al\" -H \"customer: {\\\"firstName\\\":\\\"Gabriel\\\",\\\"lastName\\\":\\\"Aubut-Lussier\\\",\\\"address\\\":{\\\"number\\\":25,\\\"street\\\":\\\"C++ Montréal\\\"}}\"\n";
	handleRequests(reqHandler);
}

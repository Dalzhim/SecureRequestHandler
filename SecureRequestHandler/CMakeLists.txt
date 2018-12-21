cmake_minimum_required(VERSION 3.5)

find_package(Boost 1.68 COMPONENTS regex system)

add_library(SecureRequestHandler INTERFACE)
set_property(TARGET SecureRequestHandler PROPERTY INTERFACE_CXX_STANDARD 17)
target_include_directories(SecureRequestHandler INTERFACE
	./include
	/usr/local/include
	${Boost_INCLUDE_DIRS}
)
target_sources(SecureRequestHandler INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include/BeastRequestAdapter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/EnsureSendWasInvoked.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/GenericSerializer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/GenericSerializer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/GenericValidator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/GenericValidator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/JSONSerializer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/JSONSerializer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/JSONValidator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/JSONValidator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/QueryStringValidator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/QueryStringValidator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/RequestAdapter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/REST.hpp
)
target_link_libraries(SecureRequestHandler INTERFACE ${Boost_LIBRARIES})


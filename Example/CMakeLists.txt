cmake_minimum_required(VERSION 3.12)

find_package(Boost 1.68 COMPONENTS system)

add_executable(Example)
set_property(TARGET Example PROPERTY CXX_STANDARD 17)
target_include_directories(Example PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${Boost_INCLUDE_DIRS}
)
target_link_libraries(Example PRIVATE SecureRequestHandler typestring ${Boost_LIBRARIES})
target_sources(Example PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include/Address.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CustomerInfo.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/SingleHandlerServer.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/Address.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CustomerInfo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
if (APPLE)
	target_compile_options(Example PRIVATE "-mmacosx-version-min=10.14")
	set_target_properties(Example PROPERTIES LINK_FLAGS "-mmacosx-version-min=10.14")
endif ()


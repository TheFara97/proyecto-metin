cmake_minimum_required(VERSION 2.8)

project(libxml CXX)

set(ACH_HEADERS
	Document.hpp
	Iterable.hpp
	Types.hpp
	Util.hpp
	xml.hpp
	rapidxml.hpp
	rapidxml_iterators.hpp
	rapidxml_print.hpp
	rapidxml_utils.hpp
	deserializer.hpp
	io.hpp
	serialize.hpp
	serializer.hpp
	sizer.hpp
	stdafx.h
	stream.hpp
)

set(ACH_SRCS
	Document.cpp
	Util.cpp
)

find_package(Boost COMPONENTS system REQUIRED)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif (Boost_FOUND)

add_library(${PROJECT_NAME} ${ACH_HEADERS} ${ACH_SRCS})


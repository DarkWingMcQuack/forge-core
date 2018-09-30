include(ExternalProject)
include(GNUInstallDirs)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  set(JSONCPP_CMAKE_COMMAND emcmake cmake)
else()
  set(JSONCPP_CMAKE_COMMAND ${CMAKE_COMMAND})
endif()

# Disable implicit fallthrough warning in jsoncpp for gcc >= 7 until the upstream handles it properly
if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
  set(JSONCCP_EXTRA_FLAGS -Wno-implicit-fallthrough)
else()
  set(JSONCCP_EXTRA_FLAGS "")
endif()

ExternalProject_Add(jsoncpp-project
  PREFIX deps/jsoncpp
  DOWNLOAD_NAME jsoncpp-1.8.4.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/open-source-parsers/jsoncpp/archive/1.8.4.tar.gz
  CMAKE_COMMAND ${JSONCPP_CMAKE_COMMAND}
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  # Build static lib but suitable to be included in a shared lib.
  -DCMAKE_POSITION_INDEPENDENT_CODE=On
  -DJSONCPP_WITH_TESTS=Off
  -DJSONCPP_WITH_PKGCONFIG_SUPPORT=Off
  -DCMAKE_CXX_FLAGS=${JSONCCP_EXTRA_FLAGS}
  # Overwrite build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )

# Create jsoncpp imported library
ExternalProject_Get_Property(jsoncpp-project INSTALL_DIR)
add_library(jsoncpp STATIC IMPORTED)
set(JSONCPP_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}jsoncpp${CMAKE_STATIC_LIBRARY_SUFFIX})
set(JSONCPP_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${JSONCPP_INCLUDE_DIR})  # Must exist.
set_property(TARGET jsoncpp PROPERTY IMPORTED_LOCATION ${JSONCPP_LIBRARY})
set_property(TARGET jsoncpp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${JSONCPP_INCLUDE_DIR})
add_dependencies(jsoncpp jsoncpp-project)
unset(INSTALL_DIR)

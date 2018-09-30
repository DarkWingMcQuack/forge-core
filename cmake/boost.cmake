include(ExternalProject)
include(GNUInstallDirs)

set(BOOST_CXXFLAGS "")
if (WIN32)
  set(BOOST_BOOTSTRAP_COMMAND bootstrap.bat)
  set(BOOST_BUILD_TOOL b2.exe)
  set(BOOST_LIBRARY_SUFFIX -vc140-mt-1_68.lib)
else()
  set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
  set(BOOST_BUILD_TOOL ./b2)
  set(BOOST_LIBRARY_SUFFIX .a)
  if (${BUILD_SHARED_LIBS})
    set(BOOST_CXXFLAGS "cxxflags=-fPIC")
  endif()
endif()

ExternalProject_Add(boost-project
  PREFIX deps/boost
  URL https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz
  URL_HASH SHA256=da3411ea45622579d419bfda66f45cd0f8c32a181d84adfa936f5688388995cf
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  SOURCE_DIR ${CMAKE_BINARY_DIR}/deps/boost
  STAMP_DIR ${CMAKE_BINARY_DIR}/tmp/boost
  TMP_DIR ${CMAKE_BINARY_DIR}/tmp/boost
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_COMMAND}
  BUILD_COMMAND ${BOOST_BUILD_TOOL} stage
  ${BOOST_CXXFLAGS}
  threading=multi
  link=static
  variant=release
  address-model=64
  -d0
  --ignore-site-config
  --with-test
  --with-program_options
  INSTALL_COMMAND ""
  )

set(BOOST_INCLUDE_DIR ${CMAKE_BINARY_DIR}/boost)
set(BOOST_LIB_DIR ${CMAKE_BINARY_DIR}/deps/boost/stage/lib)

add_library(boost::unit_test_framework STATIC IMPORTED)
set_property(TARGET boost::unit_test_framework PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_unit_test_framework${BOOST_LIBRARY_SUFFIX})
add_dependencies(boost::unit_test_framework boost-project)

add_library(boost::program_options STATIC IMPORTED)
set_property(TARGET boost::program_options PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_program_options${BOOST_LIBRARY_SUFFIX})
add_dependencies(boost::program_options boost-project)

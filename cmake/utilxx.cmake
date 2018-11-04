include(ExternalProject)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=Release
  -DBUILD_STATIC_LIBS=ON
  -DUTILXX_BUILD_EXAMPLES=OFF
  -DBUILD_SHARED_LIBS=OFF)

ExternalProject_Add(utilxx-project
  PREFIX deps/utilxx
  GIT_REPOSITORY "https://github.com/DarkWingMcQuack/Utilxx"
  GIT_TAG master
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )

ExternalProject_Get_Property(utilxx-project INSTALL_DIR)
set(UTILXX_INCLUDE_DIR ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
unset(CMAKE_ARGS)

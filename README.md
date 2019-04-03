# cppFORGE
This is a C++17 implementation of FORGE.

## Building
currently only gcc is supported since clang does not allow to capture structured bindings in lambda functions.
Hopefully this will change with C++20.

### Dependencys
in order to build cppForge libmicrohttpd needs to be installed on the machine.
All other dependencies will be downloaded and compiled by the makefiles.

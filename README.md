# forge-core [![Build Status](https://travis-ci.com/DarkWingMcQuack/forge-core.svg?token=DspAxqiWFXesStxvvxdD&branch=master)](https://travis-ci.com/DarkWingMcQuack/forge-core)
This is a C++17 implementation of FORGE.

## Building
### Dependencys
Forge-core tries to download and build all its dependencies by itself. Unfortunately this is not always possible.
Currently Forge dependes on you installing `libmicrohttpd`. The following guides will help you building forge-core
on your system.

### OSX
### Windows
### Ubuntu

#### installing GCC 9 as C++17 ready compiler
In order to compile Forge you need to have compiler installed which supports C++17.
Here we use GCC 9. To install it and use it as default compiler, 
execute the following in the terminal.

```
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install g++-9
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 90
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90
```

#### installing dependencies
Now you need to install the dependencies of Forge. Forge tries to download all its dependencies
while building to avoid the need of installing a lot of packages. Unfortunately this is not always possible.
Currently Forge dependes on you installing `libmicrohttpd`. To do this in Ubuntu, run the following commands in
the terminal. 

```
sudo apt install libmicrohttpd-dev
```

#### cloning the forge-core repository
Now you are ready to actualy build Forge. First clone this repository and change your directory with the commands:
```
git clone https://github.com/DarkWingMcQuack/forge-core
cd forge-core/
```

#### creating a *build* directory
Next, you should create a `build` directory where all the files and dependencies will go while the building process.
To create one, execute the following command.
```
mkdir build
cd build/
```
This creates a directory called `build` and changes your current directory to it.

#### run *cmake*
In the next step, you need to execute the following:

```
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=1 ..
```
This creates a bunch of files which are needed for building Forge. `-DCMAKE_BUILD_TYPE=Release` 
specifies that you would like to build a fully optimized binary to get the best performance possible.
`-DBUILD_TESTS=1` tells the cmake to also build tests in order to check later if everything works as intended.
Finally the `..` at the end tells cmake that you are refering to the project which is in the parrent folder of your current (`build`) one.

#### compiling forge-core
After you now created all the files which are needed to build Forge, you can finally do so.
Use the follwing command to finally build Forge.
```
make -j4
```
The `-j4` tells `make` to use 4 CPU cores. If you have more than 4 you can increase this number to the number of cores you have available. If you have less than 4 cores, you can decrease that number. If you run into issues with your RAM, which looks something like the following:
```
cc: internal compiler error: Killed (...)
```
you should also decrease that number.

#### running tests
After you build Forge, you can test if all tests are passing and if everything works as intended with

```
make test
```
If any test fails, please submit a bug report and **DO NOT USE FORGE IN THIS CASE**.
If everything works as intended, you can now use Forge.
The binary files `forged` which is the Forge server and `forge-cli` which 
is a CLI tool to talk with the server should now be available in your `build` directory.

### Currently Tested Compilers
* gcc 8.3
* gcc 9.1
* clang 8
* clang 9
* clang 10



## FAQ
### What actualy is Forge?
### What Blockchains are supported?
Currently only [ODIN](https://odinblockchain.org/) is supported, but in the future i surely plan to add support for [bitcoin](https://bitcoin.org/en/) and [bitcoin cash](https://www.bitcoincash.org/). If you want your project to be supported, feel free
to add it with a pull request or talk to me.
#### How can I make Forge work with my Altcoin X?
Forge-core is build in a way that it is not hard to add support for other blockchains.
To add your own, have a look at the classes in the `forge::daemon` namespace and the `forge::core::Coin` class.
### Why did you build Forge?
For fun :)
### Why does Forge not support feature X?
Maybe because i did not even thought about it. Open an issue or pull request and we will see what we can do and if it is a good idea.
### Why does Forge currently not have any UI?
Because i did not want to integrate a UI into forge-core directly. I thought it would be a good idea to separate the logic part from the UI. Forge-core exposes a ton of [JSON-RPC](https://en.wikipedia.org/wiki/JSON-RPC) methods which make it easy to build a webview for Forge. Unfortunately I have never done any web programming or frontend development. If you are a web developer and want to help this project, feel free to start a webview talking to the `forged` server.

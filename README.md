# forge-core [![Build Status](https://travis-ci.com/DarkWingMcQuack/forge-core.svg?token=DspAxqiWFXesStxvvxdD&branch=master)](https://travis-ci.com/DarkWingMcQuack/forge-core) [![GPLv3 license](https://img.shields.io/badge/License-GPLv3-blue.svg)](http://perso.crans.org/besson/LICENSE.html) [![chatroom icon](https://patrolavia.github.io/telegram-badge/chat.png)](https://t.me/forgecore)
<p align="center">
 <img src="https://github.com/DarkWingMcQuack/forge-core/blob/master/graphics/logo.jpeg" width="400" height="300">
</p>


This is a C++17 implementation of Forge.
Forge is a protocol using OP_RETURN transactions in blockchains to encode commands.
Doing this, Forge is currently able to support **Utility Tokens** and **Unique Entrys**. 

## Utility Tokens
Utility tokens can be used to create new digital assets ontop of already existing blockchains.
Utility tokens consist of a *name* and a *supply*
#### Creation
A new type of utility token can only be created if its name is not used by any other type of token or entry at the time of creation.
When a user creates a new utility token, he specifies the *name* and the *supply* and then recievies the *supply* of his new tokens to his address.

#### Transfering Utility Tokens
Utility Tokens can be send to other addresses. The owner of a bunch of tokens of the same type can create a *ownership transfer* operation using Forge. Such an operation consists of the *name* of the token he wants to send, the *number* of tokens he wants to send and ofcourse a reciever address. If at the time of the transaction the owner has enought tokens, he the reciever will recieve specified number of tokens and the creator of the *ownership transfer* does not have access or controll over the send tokens anymore.

#### Burning Utility Tokens
Much like Bitcoin or any other Cryptocurrency, Utility Tokens can be burned. To do this a user needs to create *deletion* operation which consists of the *name* of the token he wants to burn and a *number* of tokens he/she wants to burn.
If the user has enought tokens at the time of the *deletion* operation, the *number* of tokens get burned and nobody has access to them anymore after that, which means the total *supply* of a token is decreased.
If a token has a *supply* of 0 at any time, the *name* of the token can be reused for another *creation* of tokens or entries within Forge.

## Unique Entries
Unique Entries are *key-value* pairs where the key is owned and controlled by exactly one owner at a time.
The *value* of such a key, which can currently be *nothing*, an *IPv4*, an *IPv6* or an arbitrary string can be choosen by the owner.

#### Lifetime and Refreshing
Once created such an unique entry it is valid for the next year. To make it longer valid the owner can **refresh** the entry at any given time which makes the entry valid for another year since the refreshing.

#### Ownership Transfering
Like tokens, unique entrys can be transfered to another owner. After such an ownership transfer the new owner has the responsibility of refreshing the entry.

#### Deletion
In order to give an entry free for others to use it, an entry can be deleted. After deletion, the entry is invalidated, cannot be found in lookup operations anymore and it can be created from any other user of forge with any assigned *value*.

Currently there are two types of unique entrys. *Modifiable Unique Entrys*, called *UMEntrys* and *Immutable Unique Entrys*, called *Unique Entrys* in the code.

### Modifiable Unique Entrys
Modifiable Unique Entrys are handy whenever the owner is unsure which *value* he wants to be paired with the entry, since modifiable entries support **update** operations which reasign a new *value* to the entry.

### Immutable Unique Entrys
Modifiable Unique Entrys do not support update operations, which means once created with a specific *value* it cannot be updated or changed.

### P2E
Since Forge knows the owner of unique entries and unique entries are as the name suggests unique, it is possible to use Forge as a DNS for payments. This means instead of paying somebody with an address, it is possible using forge to pay someone to its entry. Since entries can have arbitrary names users can create an entry with a nickname and then recieve payments to their nickname instead of publishing an complex address.

### DNS
Forge supports *IPv4* and *IPv6* as associated values with entries, which means Forge can operate as a decentralized DNS, where users lookup IP addresses through Forge Entries.
It is planed for the future that Forge provides scripts which can be started in order to run a local DNS-Server which utilizes Forge as backend.

### Keyserver
Not only IP addresses can be supported, but also public keys can be looked up in a decentralized manner, meaning Forge can be used as a decentralized Keyserver.

### File Hashes
Since Unique Entries support storing arbitrary byte-values, it is possible to store file hashes into the blockchain which are accosiated by an entry name. This makes the filehash easy to lookup in the future and the proof of knowledge of a given file at a given time can be done easily with Forge.

# Building
Forge-core tries to download and build all its dependencies by itself. Unfortunately this is not always possible.
Currently Forge dependes on you installing `libmicrohttpd`. The following guides will help you building forge-core
on your system.

## Docker
## OSX
## Windows
## Ubuntu
Currently Ubuntu Bionic Beaver (18.04) and newer are supported. If you want to compile forge-core on an older Ubuntu version,
it can happen that you run into linking problems due to old versions of `libmicrohttpd` in the official package sources. To avoid this you can try to install `libmicrohttpd` by yourself. 

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

## Currently Tested Compilers
* gcc 8.3
* gcc 9.1
* clang 8
* clang 9
* clang 10



# FAQ
### What actualy is Forge?
Forge is a protocol which uses OP_RETURN transactions provided by blockchains to save data and commands into the blockchain.
Doing so Forge can support operations which the underlying blockchain does not understand, such as the creation of tokens.
Forge-core is an implementation of this protocol which tries to make the use of Forge as simple as possible by supporting a large amount of commands.

Forge also supports the creation of unique entry-value pairs which then can be looked up. This enables a large amount of use cases, such as public key lookups. Decentralized DNS services and a DNS like system for payments where users do not need to know the address of an entity to pay them, but their username.

### How can I run Forge?
Forge-core consists of two binaries. **forged** which is a server talking to the underlying blockchain daemon(like bitcoind).
On the first run it creates a *.forge/* directory in *$HOME*. Inside that directory all the configuration files can be found and addapted to your needs.
The other binary **forge-cli** is an utility which can be used to talk with **forged** and to send commands.
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

# forge-core [![Build Status](https://travis-ci.com/DarkWingMcQuack/forge-core.svg?token=DspAxqiWFXesStxvvxdD&branch=master)](https://travis-ci.com/DarkWingMcQuack/forge-core) [![GPLv3 license](https://img.shields.io/badge/License-GPLv3-blue.svg)](http://perso.crans.org/besson/LICENSE.html) [![chatroom icon](https://patrolavia.github.io/telegram-badge/chat.png)](https://t.me/forgecore)
<p align="center">
 <img src="https://github.com/DarkWingMcQuack/forge-core/blob/master/graphics/logo.jpeg" width="400" height="300">
</p>


This implementation of FORGE uses the C++17 language.
Forge is a protocol using OP_RETURN transactions in blockchains to encode commands.
Doing this, Forge is currently able to support **Utility Tokens** and **Unique Entries**. 

## Utility Tokens
Utility tokens create new digital assets on top of already existing blockchains.
Utility tokens consist of a *name* and a *supply*
#### Creation
A new type of utility token can only be generated if its name is not currently in use by any other type of token or entry within FORGE at the time of the new token creation.
When a user creates a new utility token, they specify the *name* and the *supply* and then receive the *supply* of newly FORGED tokens to the provided address.

#### Transfering Utility Tokens
Utility Tokens may be sent to other addresses. The owner of a batch of tokens of the same type can create an *ownership transfer* operation using FORGE. Such an operation consists of the *name* of the token they want to send, the *number* of tokens to be sent and a receiver's address. If at the time of the transaction, the owner has enough tokens,  the receiver will receive the pre-determined number of tokens sent during the initial send by the prior owner. The initial creator (the sender) of the *ownership transfer* will no longer have control over the sent tokens. New ownership will then solely lie with the recipient (the receiver) of the *ownership transfer*

#### Burning Utility Tokens
Like Bitcoin or any other cryptocurrency, Utility Tokens can be burned by the owner of the tokens. To do this, a user needs to create *deletion* operation, which consists of the *name* of the token he wants to burn and a *number* of tokens he/she wants to burn.
If the user has enough tokens at the time of the *deletion* operation, the pre-determined *number* of tokens gets burned, and access to them after the burn is lost, resulting in the total *supply* of a token being decreased.
If a token at any time has a *supply* of 0, the *name* of the token can be reused to *create* new tokens or entries with the same name within FORGE.

## Unique Entries
Unique Entries are *key-value* pairs where the key is owned and controlled by precisely one owner at a time.
The *value* of such a key, which can currently be *nothing*, an *IPv4*, an *IPv6*, or an arbitrary string can be chosen by the owner.

#### Lifetime and Refreshing
Once a unique entry is generated, it remains valid for exactly one year.  To extend its lifetime, the owner can **refresh** the entry.  The entry's validity is then extended precisely one year from the time of **refresh**.

#### Ownership Transfering
Like tokens, unique entries can be transferred to another owner. After such an ownership transfer, the new owner is solely responsible for refreshing the entry.

#### Deletion
An entry, like tokens, can be deleted.  After deletion, an entry is now free for others to use.  Upon deletion, the entry is invalid, cannot be found in lookup operations, and is open for creation by other FORGE users with any assigned *value*.

Currently, there are two types of unique entries:

1.)*Modifiable Unique Entries* also known as *MUEntries*
2.)*Immutable Unique Entries* also known as *Unique Entries* in the code.

### Modifiable Unique Entries
Modifiable Unique Entries are useful when the owner is unsure which *value* they want to be associated with the entry. With modifiable entries, support **update** operations, allow for reassignment of *value* to the entry.

### Immutable Unique Entries
Modifiable Unique Entries do not support update operations. Once created with a specific *value*, the entry cannot be modified or changed.

### P2E
FORGE "knows" the owner of unique entries.  Since individual entries are unique, it is possible to use FORGE as a DNS for payments. Essentially, this means that instead of paying someone with an address, it is possible to pay someone via their entry.  Since entries can be created with arbitrary names, users can create an entry with a nickname and receive payments directly to their nickname rather than publishing a complicated and intimidating long-string random alphanumeric address.

### DNS
FORGE currently supports *IPv4* and *IPv6* as associated values for entries. *IPv4* and *IPv6* means FORGE can operate as a decentralized DNS, allowing for users to lookup IP address through FORGE Entries.
FORGE scripts allowing for local running of DNS-Servers is planned and will make use of FORGE as backend.

### Keyserver
Not only is IP address lookup currently supported, but it also allows for public key search in a decentralized form, meaning FORGE can function as a decentralized Keyserver.

### File Hashes
Unique Entries support the storage of arbitrary byte-values. Because of this storage support, it is possible to store file hashes within the blockchain with an associated entry name. This storage method allows for a more straightforward filehash lookup in future instances using FORGE.

# Building
FORGE-Core will initially try to download and build all dependencies itself. Unfortunately, this is not always possible.
In its current state, FORGE depends on the installation of `libmicrohttpd`. The following guides will help you in building FORGE-Core in different OS environments dependent on your system.

## Docker
## OSX
## Windows
## Ubuntu
Currently, Ubuntu Bionic Beaver (18.04) and newer are supported for FORGE. If you want to compile FORGE-Core on an older Ubuntu version, you may run into linking problems due to earlier versions of `libmicrohttpd` in the official package sources. To avoid this, you can try to install `libmicrohttpd` yourself. 

#### installing GCC 9 as C++17 ready compiler
To compile Forge, you need to have a compiler installed supporting C++17.
Here we use GCC 9. To install it and use it as the default compiler, 
execute the following in the terminal.

```
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install g++-9
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 90
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90
```

#### installing dependencies
Now you will need to install the FORGE dependencies.  Again, FORGE will try to download all of its dependencies automatically.  However, this is not always possible.  The current iteration of FORGE depends on the installation of `libmicrohttpd`. To do this in Ubuntu, run the following commands in terminal. 

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
### What is Forge?
Forge is a protocol that uses OP_RETURN transactions provided by blockchains to save data and commands into the blockchain.
Doing so Forge can support operations that the underlying blockchain does not understand, such as the creation of tokens.
Forge-core is an implementation of this protocol that tries to make the use of Forge as simple as possible by supporting a large number of commands.

Forge supports the creation of unique entry-value pairs that can be found in future lookups. Future lookups allow for vast use cases, amongst them public-key searches, decentralized DNS services, and DNS like systems.  These systems allow for payment transfers to an assigned nickname in place of a traditional public address made up of a long string form of alphanumeric characters.

### How can I run Forge?
Forge-core consists of two binaries. **forged**, which is a server talking to the underlying blockchain client(like bitcoind).
On the first run, it creates a *.forge/* directory in *$HOME*. Inside that directory, all the configuration files can be found and adapted to your needs.
The other binary **forge-cli** is a utility that can be used to talk with **forged** and to send commands.
### What Blockchains are supported?
Currently, only [ODIN](https://odinblockchain.org/) is supported, but in the future, support is planned for [bitcoin](https://bitcoin.org/en/) and [bitcoin cash](https://www.bitcoincash.org/). If you want your project to be supported, feel free to add it with a pull request.

#### How can I make Forge work with my Altcoin X?
Forge-core is built in a way that supports of other blockchains can be added.
To add your own, have a look at the classes in the `forge::client` namespace and the `forge::core::Coin` class.
### Why did you build Forge?
For fun :)
### Why does Forge not support feature X?
With the initial release of Forge, all possible features have not been exhausted. Should you like the support of additional features, open an issue or pull request and we will see what we can do and if it is a good idea.
### Why does Forge currently not have any UI?
With the initial build, I did not want to integrate a UI into forge-core directly. I thought it would be a good idea to separate the logic part from the UI. Forge-core exposes a ton of [JSON-RPC](https://en.wikipedia.org/wiki/JSON-RPC) methods, which make it easy to build a web view for Forge. My current skill set is most suited toward backend builds and support. If you are a web developer and want to help this project, feel free to start a webview by talking to the `forged` server.


Source code of the TinyEVM System (to be published) in <b>40th IEEE - ICDCS 2020</b> conference.

# TinyEVM: Off-Chain Smart Contracts on Low-Power IoT Devices
<b>Chistos Profentzas</b>, Magnus Almgren, and Olaf Landsiedel. In Proceedings of the 40th IEEE International Conference on Distributed Computing Systems (ICDCS), 2020.

[paper](https://research.chalmers.se/publication/516951/file/516951_Fulltext.pdf), [talk] ICDCS - December 2020



## Abstract
In this paper, we present TinyEVM, a novel system to generate and execute off-chain smart contracts based on sensor data.
TinyEVM's goal is to enable IoT devices to perform micropayments and, at the same time, address the IoT constraints.
We investigate the trade-offs to execute smart contracts in low-power IoT devices.
We test our system with 7,000 publicly verified smart contracts, where TinyEVM achieves to deploy 93 % of them without any modification.
Finally, we evaluate the execution of off-chain smart contracts in terms of run-time performance, energy, and memory requirements on IoT devices.
Notably, we find that low-power devices can deploy a smart contract in 215 ms on average.
The mote can complete an off-chain payment in 584 ms on average.




## Implementation

We implement the Etheruem Virtual Machine (EVM), in <b>C</b> for the <b>Contiki-NG OS</b>.

We support Texas Instruments CC2538 SoC.

## Code structure

### EVM code
Under [ContikiNG/Ethereum-vm-256](./ContikiNG/Ethereum-vm-256/).



### Execute Smart Contracts

### Configuring, Compiling, Flashing and Running on HW

#### Toolchain: Compiler
To compile, you need to install the GCC compiler toolchain and tell the Makefile where to find it.

<!-- We use [gcc-arm-none-eabi-7-2017-q4-major](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads) -->
For the compiler, we use [gcc-arm-none-eabi-7-2017-q4-major](https://github.com/gnu-mcu-eclipse/arm-none-eabi-gcc/releases).
Download the binaries of this version of the compiler suitable to your system, then extract the archive on your system.
Consult [GNU MCU Eclipse page](https://gnu-mcu-eclipse.github.io/toolchain/arm/install/) for help.

You need to modify the following variable in the Makefile in accordance to your installation:
```
GCC_INSTALL_ROOT	:= ~/opt/gnu-mcu-eclipse/arm-none-eabi-gcc/7.2.1-1.1-20180401-0515
GCC_VERSION		:= 7.2.1
GCC_PREFIX		:= arm-none-eabi
```

#### Make parameters
We have the following configuration parameters to specify in compile time, with the `make` command:
```
TARGET=openmote-cc2538 # Default supported platform
MAKE_NET = MAKE_NET_IPV6 # Default uIP low-power IPv6 stack, with 6LoWPAN and RPL.

```

#### Make command examples
Compile command example:
```
make clean && make  
```

To program the connected mote:
```
make -j4 TARGET=openmote-cc2538 PORT=/dev/[DEVICE PORT] Ethereum_App.upload
```


Source code of the TinyEVM System (to be published) in <b>40th IEEE - ICDCS 2020</b> conference.

# TinyEVM: Off-Chain Smart Contracts on Low-Power IoT Devices
<b>Chistos Profentzas</b>, Magnus Almgren, and Olaf Landsiedel. In Proceedings of the 40th IEEE International Conference on Distributed Computing Systems (ICDCS), 2020.



## Abstract
With the rise of the Internet of Things (IoT), billions of devices ranging from rudimentary sensors and actuators to smart-phones can participate in millions of micropayments originating from IoT-enabled services at a micro-level.
However, the current centralized solutions are unable to handle a massive number of micropayments from untrusted devices.

Blockchains are promising technologies suitable for solving some of the challenges. 
Particularly, permissionless blockchains such as Ethereum and Bitcoin have drawn the attention of the research community.
However, the increasingly large-scale deployments of blockchain reveal some of their scalability limitations.
Prominent proposals to scale the payment system include off-chain protocols such as payment channels.
However, the leading proposals assume powerful nodes with an always-on connection and frequent synchronization.
These assumptions require in practice significant communication, memory, and computation capacity, whereas IoT devices face substantial constraints in these areas.
Existing approaches also do not capture the logic and process of IoT, where applications need to process locally collected sensor data to allow for full use of IoT micropayments.

In this paper, we present TinyEVM, a novel system to generate and execute off-chain smart contracts based on sensor data.
TinyEVM's goal is to enable IoT devices to perform micropayments and, at the same time, address the IoT constraints.
We investigate the trade-offs to execute smart contracts in low-power IoT devices.
We test our system with 7,000 publicly verified smart contracts, where TinyEVM achieves to deploy 93 % of them without any modification.
Finally, we evaluate the execution of off-chain smart contracts in terms of run-time performance, energy, and memory requirements on IoT devices.
Notably, we find that low-power devices can deploy a smart contract in 215 ms on average.
The mote can complete an off-chain payment in 584 ms on average.


# Source code will be unlocked latest 7th of July 2020 

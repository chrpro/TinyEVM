#ifndef EVM_H
#define EVM_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "uint256.h"

#define MEMORY_SPACE 8089
#define MESSAGEDATASIZE 9
#define STACK_SPACE 96  
#define STORAGE_SPACE 64
#define GAS_LIMIT 16000000

// typedef uint8_t byte;
// typedef uint16_t word;
extern uint8_t deployed_contract[];
extern uint64_t DeployLength;

typedef struct Message_Ext {
  uint256_t call_value;

  uint256_t caller;

  
  uint256_t address;
  
  uint8_t data[MESSAGEDATASIZE];
  uint32_t datasize;

  uint32_t codesize;
}Message_Ext;

typedef struct machine {
	uint32_t PC;
	int SP;
	uint8_t MEM[MEMORY_SPACE];
	uint256_t STACK[STACK_SPACE];
  uint256_t STORAGE[STORAGE_SPACE];
	uint32_t GAS_Charge;
  Message_Ext message;
} Machine;


typedef struct SignedMessage {
   uint32_t ECDSA_R[8];
   uint32_t ECDSA_S[8];
} MachiSignedMessagene;

uint64_t swapLong(uint64_t *);

//keccak256
void get_keccak256(const uint8_t *data, uint16_t length, uint8_t *result);

void print256(uint256_t * number_1);
//VM functions
void init_machine(Machine *);
void shutdown_machine(Machine *);
int decode_instruction(Machine *, uint8_t, const uint8_t *);
void execute_contract(Machine *, const uint8_t *, uint32_t );

//stuck operations
void stack_push(Machine *, uint256_t  );
uint256_t stack_pop(Machine *);
uint256_t stack_peek(Machine *);
void stack_duplicate(Machine *);

//error reports
void stack_overflow_err();
void empty_stack_err(char *);
void size_err(char *);
void stack_print(Machine *);

int64_t ipow(int64_t , uint8_t );


struct GAS_price
{
    uint32_t 
    stepGas0 ,
    stepGas1 ,
    stepGas2  ,
    stepGas3  ,
    stepGas5  ,
    stepGas6  ,
    stepGas8  ,
    stepGas10,
    sha3Gas  ,
    sha3WordGas  ,
    sloadGas  ,
    sstoreSetGas   ,
    sstoreResetGas  ,
    sstoreUnchangedGas  ,
    jumpdestGas  ,
    logGas  ,
    logDataGas  ,
    logTopicGas  ,
    createGas  ,
    memoryGas  ,
    quadCoeffDiv  ,
    copyGas  ,
    valueTransferGas  ,
    callStipend ,
    callNewAccount ;
};


enum OP_CODE{
    STOP = 0x00,
    ADD,
    MUL,
    SUB,
    DIV,
    SDIV,
    MOD,
    SMOD,
    ADDMOD,
    MULMOD,
    EXP,
    SIGNEXTEND,
    //EXTENSION TO EVM FOR IOT APPs
    SENSOR = 0x0c,
    LED,
    TEMPERATURE,




    LT = 0x10,
    GT,
    SLT,
    SGT,
    EQ,
    ISZERO,
    AND,
    OR,
    XOR,
    NOT,
    BYTE,
    SHL,
    SHR,
    SAR,


    SHA3 = 0x20,
    

    ADDRESS = 0x30,
    BALANCE,
    ORIGIN,
    CALLER,
    CALLVALUE,
    CALLDATALOAD,
    CALLDATASIZE,
    CALLDATACOPY,
    CODESIZE,
    CODECOPY,
    GASPRICE,
    EXTCODESIZE,
    EXTCODECOPY,

    BLOCKHASH = 0x40,
    COINBASE,
    TIMESTAMP,
    NUMBER,
    DIFFICULTY,
    GASLIMIT,

    POP = 0x50,
    MLOAD,
    MSTORE,
    MSTORE8,
    SLOAD,
    SSTORE,
    JUMP,
    JUMPI,
    PC,
    MSIZE,
    GAS,
    JUMPDEST,

    PUSH1 = 0x60,
    PUSH2,
    PUSH3,
    PUSH4,
    PUSH5,
    PUSH6,
    PUSH7,
    PUSH8,
    PUSH9,
    PUSH10,
    PUSH11,
    PUSH12,
    PUSH13,
    PUSH14,
    PUSH15,
    PUSH16,
    PUSH17,
    PUSH18,
    PUSH19,
    PUSH20,
    PUSH21,
    PUSH22,
    PUSH23,
    PUSH24,
    PUSH25,
    PUSH26,
    PUSH27,
    PUSH28,
    PUSH29,
    PUSH30,
    PUSH31,
    PUSH32,

    DUP1 = 0x80,
    DUP2,
    DUP3,
    DUP4,
    DUP5,
    DUP6,
    DUP7,
    DUP8,
    DUP9,
    DUP10,
    DUP11,
    DUP12,
    DUP13,
    DUP14,
    DUP15,
    DUP16,

    SWAP1 = 0x90,
    SWAP2,
    SWAP3,
    SWAP4,
    SWAP5,
    SWAP6,
    SWAP7,
    SWAP8,
    SWAP9,
    SWAP10,
    SWAP11,
    SWAP12,
    SWAP13,
    SWAP14,
    SWAP15,
    SWAP16,

    LOG0 = 0xa0,
    LOG1,
    LOG2,
    LOG3,
    LOG4,

    CREATE = 0xf0,
    CALL,
    CALLCODE,
    RETURN,
    DELEGATECALL,

    STATICCALL = 0xfa,
    REVERT= 0xfd,
    INVALID,


    SELFDESTRUCT = 0xff,
  };
typedef enum OP_CODE OP_CODE;

#endif /* MY_HEADER_H */


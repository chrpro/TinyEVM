#include "contiki.h"
#include "uint256.h"
#include "evm.h"
#include "keccak256.h"
#include "bytecode.h"



//REV reversve the byte order
#define REV(X) ((X << 24) | ((X & 0xff00) << 8) | ((X >> 8) & 0xff00) | (X >> 24))

#define DEPLOY_LENGTH 3000


uint8_t deployed_contract[DEPLOY_LENGTH];
uint64_t DeployLength = 0;


static rtimer_clock_t total_time;

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Eth-VM");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
	PROCESS_BEGIN();
    static Machine MAIN_VM; 
    init_machine(&MAIN_VM);

    unsigned char funcation_name[] = "close(uint256,bytes)";

    // -1 to remove \0 of the string termination
    uint8_t keccak_fun_name[32];
    get_keccak256(funcation_name, sizeof(funcation_name) - 1, keccak_fun_name);

    // uint8_t call_value[396] = {0};
    uint64_t parameter1 = 0x01;
    uint64_t sizeOfparameter1 = 0x40;
	uint64_t sizeOfsignature = 0x14;
	uint8_t signature[32]  = {0xca,0x35,0xb7,0xd9,0x15,0x45,0x8e,0xf5,0x40,0xad,0xe6,0x06,0x8d,0xfe,0x2f,0x44,0xe8,0xfa,0x73,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
	
	
	memcpy(MAIN_VM.message.data, keccak_fun_name, sizeof(uint8_t) * 4 );
	memcpy(MAIN_VM.message.data + 3 + 32  , &parameter1, sizeof(parameter1)  );
	memcpy(MAIN_VM.message.data + 3 + 64  , &sizeOfparameter1, sizeof(sizeOfparameter1)  );
	memcpy(MAIN_VM.message.data + 3 + 96  , &sizeOfsignature, sizeof(sizeOfsignature)  );
	memcpy(MAIN_VM.message.data + 3 + 129 , &signature, sizeof(signature)  );
	MAIN_VM.message.datasize  = 0x84;


	
	UPPER(UPPER (MAIN_VM.message.caller)) =	0xca35b7d915;										
	UPPER(LOWER (MAIN_VM.message.caller)) =	0x00000000ca35b7d9;
	LOWER(UPPER (MAIN_VM.message.caller)) =	0x15458ef540ade606;
	LOWER(LOWER (MAIN_VM.message.caller)) =	0x8dfe2f44e8fa733c;


	LOWER(LOWER (MAIN_VM.message.call_value)) =	0x00;

	MAIN_VM.message.codesize  = sizeof(smart_contract);


	total_time = RTIMER_NOW();
	execute_contract( &MAIN_VM, smart_contract, sizeof(smart_contract)) ;
	total_time = RTIMER_NOW() - total_time;
	printf("Size of contract: %d\n", sizeof(smart_contract));
	printf("EVM time: %lu ms\n", (uint32_t)((uint64_t)total_time * 1000 / RTIMER_SECOND));	
	printf("deployed_contract : \n");
	printf("LENGTH : %llu\n",DeployLength);

	printf("Deployed_contract: \n");
	printf("-----------------------------\n");
    for (int i = 0; i < DeployLength;i++){
                printf("%X",deployed_contract[i]);
    }
 	printf("\n -----------------------------\n");

	init_machine(&MAIN_VM);	
	execute_contract( &MAIN_VM, deployed_contract, DEPLOY_LENGTH) ;

  
  PROCESS_END();
}

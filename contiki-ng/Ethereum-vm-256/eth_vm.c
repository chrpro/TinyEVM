#include "evm.h"
#include <math.h>
#include "keccak256.h"
#include "dev/leds.h"
#include "dev/cc2538-sensors.h"

// #define CC2538_CHIP 

void print256(uint256_t * number_1 ){
    printf("%016llX\n", UPPER(UPPER_P(number_1 )));
    printf("%016llX\n", UPPER(LOWER_P(number_1)));
    printf("%016llX\n", LOWER(UPPER_P(number_1 )));
    printf("%016llX\n", LOWER(LOWER_P(number_1)));
}

uint64_t swapLong(uint64_t *X) {
    uint64_t x =  *X;
    x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
    x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
    x = (x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8;
    return x;
}

static struct GAS_price GAS_TABLE = {  
    .stepGas0 = 0,
    .stepGas1 = 1,
    .stepGas2 = 2,
    .stepGas3 = 3,
    .stepGas5 = 5,
    .stepGas8 = 8,
    .stepGas10 = 10,
    .sha3Gas = 30,
    .sha3WordGas = 6,
    .sloadGas = 50,
    .sstoreSetGas = 20000,
    .sstoreResetGas = 5000,
    .sstoreUnchangedGas = 200,
    .jumpdestGas = 1,
    .logGas = 375,
    .logDataGas = 8,
    .logTopicGas = 375,
    .createGas = 32000,
    .memoryGas = 3,
    .quadCoeffDiv = 512,
    .copyGas = 3,
    .valueTransferGas = 9000,
    .callStipend = 2300,
    .callNewAccount = 25000,
};


static rtimer_clock_t time;


void init_machine(Machine * state) {
	state->PC = 0;
	state->SP = 0;
    state->GAS_Charge = 0;
}

int max_mem_offset = 0;
int max_storage_counter = 0;
int max_sp = 0;

void execute_contract(Machine *machine_state, const uint8_t *s_contract, uint32_t size) {

    
    //Execute smart contract till end of bytecode / exit or error 
    while(machine_state->PC  < size - 1 )
    {
        // GAS can be emmited for off-chain
        if(machine_state->GAS_Charge > GAS_LIMIT){
             printf("Run out of GAS!\n");
             break;
        }
        //decode the next instruction
        int status = decode_instruction(machine_state, s_contract[machine_state->PC] , s_contract );
        //check for stack pointer
        if (machine_state->SP > max_sp){
            max_sp = machine_state->SP; 
        }
        if (status == RETURN)
        {
            // printf("return!\n");
            break;
        }
        if (status < 0)
        {
            printf("ERROR!\n");
            break;
        }
        machine_state->PC ++;
        if(s_contract[machine_state->PC] == STOP){
            break;
        }
    }
    // End of smart contract execution print stats
    printf("Stack Pointer :  %u \n",max_sp);
    printf("Stack usage :  %u \n",max_sp * 256);
    printf("Memory usage :  %u \n",max_mem_offset) ;
    printf("Storage usage  :  %u \n",max_storage_counter * 256);
   
}
//each word-machine parse through the decode state
int decode_instruction(Machine *machine_state, u_int8_t op_code_exc, const u_int8_t *s_contract) { 
	switch (op_code_exc ) {
        //****<< IOT APP OPCODES >>****
        case SENSOR:{
            printf("sensor\n");
            break;

        }
        case TEMPERATURE :{
    
            // read and sgtore temperature sensor
            #ifdef CC2538_CHIP
                #define TMP_BUF_SZ 32
                char tmp_buf[TMP_BUF_SZ];
                SENSORS_ACTIVATE(cc2538_temp_sensor);
                memset(tmp_buf, 0, TMP_BUF_SZ);
                snprintf(tmp_buf, TMP_BUF_SZ, "\"On-Chip Temp (mC)\":%d",
                    cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED));
                puts (tmp_buf);
                uint256_t temperature;
                LOWER(LOWER(temperature)) = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
                stack_push(machine_state, temperature);
            #endif
        break;
        }

        case LED :{
            // printf ("LED sensor\n");
            leds_on(LEDS_YELLOW);
            break;             
        }

        //****<< ORIGINAL OPCODES >>****
		case STOP: { // STOP
			printf("STOP..\n");
            // printf("Stack output:  %llu \n", stack_peek(machine_state));
            printf("GAS spend:  %lu \n", machine_state->GAS_Charge);
			shutdown_machine(machine_state);
		} 
		case PUSH1:
        case PUSH2:
        case PUSH3:
        case PUSH4:
        case PUSH5:
        case PUSH6:
        case PUSH7:
        case PUSH8:{ // Push x bits into the stack 
            // printf( "PC:%li - PUSH \n",machine_state->PC); 
            if (op_code_exc < 0) {	
                    printf( "malformed op_code: {PUSH}\n");
                    break;
            }
            int numberOfBytes = (int)op_code_exc - (int)PUSH1 + 1;
            
            if (machine_state->SP + numberOfBytes >= STACK_SPACE){ 
                    stack_overflow_err();
                    return -1;
            }
            
            machine_state->SP++;

            uint64_t element_upp_upp = 0; 
            uint64_t element_upp_low = 0; 
            uint64_t element_low_upp = 0; 
            uint64_t element_low_low = 0; 

            int i;
            
            for (i =0 ; i < numberOfBytes; i++){
                    machine_state->PC ++;
                    element_low_low =  ( element_low_low << 8 ) | (uint64_t) s_contract[ machine_state->PC];
            }
            // printf("element upp upp %016llX\n", element_upp_upp);
            // printf("element upp low %016llX\n", element_upp_low);
            // printf("element low upp %016llX\n", element_low_upp);
            // printf("element low low %016llX\n", element_low_low);
            
            LOWER(LOWER(machine_state->STACK[machine_state->SP])) = element_low_low;
            LOWER(UPPER(machine_state->STACK[machine_state->SP])) = element_low_upp;
            UPPER(LOWER(machine_state->STACK[machine_state->SP])) = element_upp_low;
            UPPER(UPPER(machine_state->STACK[machine_state->SP])) = element_upp_upp;

            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;
			break;
		} 
        case PUSH9:
        case PUSH10:
        case PUSH11:
        case PUSH12:
        case PUSH13:
        case PUSH14:
        case PUSH15:
        case PUSH16:
        { 
            if (op_code_exc < 0) {	
                    printf( "malformed op_code: {PUSH}\n");
                    break;
            }
            int push_code = (int)op_code_exc - (int)PUSH1 ;  

            int numberOfBytes = push_code + 1;

            if (machine_state->SP + numberOfBytes >= STACK_SPACE){ 
                    stack_overflow_err();
                    return -1;
            }

            machine_state->SP++;

            uint64_t element_upp_upp = 0; 
            uint64_t element_upp_low = 0; 
            uint64_t element_low_upp = 0; 
            uint64_t element_low_low = 0; 
            
            
            int size_low_up = push_code - 8;
            int i;
            // printf("size_low_up = %d\n",  size_low_up); 
            for(i = 0; i <= size_low_up; i++)
            {
                machine_state->PC ++;
                element_low_upp = ( element_low_upp  << 8 ) | (uint64_t) s_contract[ machine_state->PC];
            }
            for(i = 0; i < 8; i++)
            {
                machine_state->PC ++;
                element_low_low = ( element_low_low  << 8 ) | (uint64_t) s_contract[ machine_state->PC];           
            }
            
            LOWER(LOWER(machine_state->STACK[machine_state->SP])) = element_low_low;
            LOWER(UPPER(machine_state->STACK[machine_state->SP])) = element_low_upp;
            UPPER(LOWER(machine_state->STACK[machine_state->SP])) = element_upp_low;
            UPPER(UPPER(machine_state->STACK[machine_state->SP])) = element_upp_upp;
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;
			break;
		} 
        case PUSH17:
        case PUSH18:
        case PUSH19:
        case PUSH20:
        case PUSH21:
        case PUSH22:
        case PUSH23:
        case PUSH24:        
        { 
            if (op_code_exc < 0) {	
                    printf( "malformed op_code: {PUSH}\n");
                    break;
            }
            int push_code = (int)op_code_exc - (int)PUSH1 ;  

            int numberOfBytes = push_code + 1;

            if (machine_state->SP + numberOfBytes >= STACK_SPACE){ 
                    stack_overflow_err();
                    return -1;
            }

            machine_state->SP++;

            uint64_t element_upp_upp = 0; 
            uint64_t element_upp_low = 0; 
            uint64_t element_low_upp = 0; 
            uint64_t element_low_low = 0; 
            
            
            int size_low_up = push_code - 16;
            int i;
            // printf("size_low_up = %d\n",  size_low_up); 
            for(i = 0; i <= size_low_up; i++)
            {
                machine_state->PC ++;
                 element_upp_low  = ( element_upp_low  << 8 ) | (uint64_t) s_contract[ machine_state->PC];
            }
            for(i = 0; i < 8; i++)
            {
                machine_state->PC ++;
                element_low_upp = ( element_low_upp  << 8 ) | (uint64_t) s_contract[ machine_state->PC];           
            }
            for(i = 0; i < 8; i++)
            {
                machine_state->PC ++;
                element_low_low = ( element_low_low  << 8 ) | (uint64_t) s_contract[ machine_state->PC];           
            }
                        
            // printf("element upp upp %016llX\n", element_upp_upp);
            // printf("element upp low %016llX\n", element_upp_low);
            // printf("element low upp %016llX\n", element_low_upp);
            // printf("element low low %016llX\n", element_low_low);
            
            LOWER(LOWER(machine_state->STACK[machine_state->SP])) = element_low_low;
            LOWER(UPPER(machine_state->STACK[machine_state->SP])) = element_low_upp;
            UPPER(LOWER(machine_state->STACK[machine_state->SP])) = element_upp_low;
            UPPER(UPPER(machine_state->STACK[machine_state->SP])) = element_upp_upp;

            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;
;
			break;
		} 
        case PUSH25:
        case PUSH26:
        case PUSH27:
        case PUSH28:
        case PUSH29:
        case PUSH30:
        case PUSH31:
        case PUSH32:        
        { 
            if (op_code_exc < 0) {	
                    printf( "malformed op_code: {PUSH}\n");
                    break;
            }
            int push_code = (int)op_code_exc - (int)PUSH1 ;  

            int numberOfBytes = push_code + 1;

            if (machine_state->SP + numberOfBytes >= STACK_SPACE){ 
                    stack_overflow_err();
                    return -1;
            }
            machine_state->SP++;

            uint64_t element_upp_upp = 0; 
            uint64_t element_upp_low = 0; 
            uint64_t element_low_upp = 0; 
            uint64_t element_low_low = 0; 
            
            
            int size_low_up = push_code - 24;
            int i;
            // printf("size_low_up = %d\n",  size_low_up); 
            for(i = 0; i <= size_low_up; i++)
            {
                machine_state->PC ++;
                 element_upp_upp  = ( element_upp_upp  << 8 ) | (uint64_t) s_contract[ machine_state->PC];
            }
            for(i = 0; i < 8; i++)
            {
                machine_state->PC ++;
                 element_upp_low  = ( element_upp_low  << 8 ) | (uint64_t) s_contract[ machine_state->PC];
            }
            for(i = 0; i < 8; i++)
            {
                machine_state->PC ++;
                element_low_upp = ( element_low_upp  << 8 ) | (uint64_t) s_contract[ machine_state->PC];           
            }
            for(i = 0; i < 8; i++)
            {
                machine_state->PC ++;
                element_low_low = ( element_low_low  << 8 ) | (uint64_t) s_contract[ machine_state->PC];           
            }
                        
            // printf("element upp upp %016llX\n", element_upp_upp);
            // printf("element upp low %016llX\n", element_upp_low);
            // printf("element low upp %016llX\n", element_low_upp);
            // printf("element low low %016llX\n", element_low_low);
            
            LOWER(LOWER(machine_state->STACK[machine_state->SP])) = element_low_low;
            LOWER(UPPER(machine_state->STACK[machine_state->SP])) = element_low_upp;
            UPPER(LOWER(machine_state->STACK[machine_state->SP])) = element_upp_low;
            UPPER(UPPER(machine_state->STACK[machine_state->SP])) = element_upp_upp;

            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;

			break;
		} 
		case ADD: { // Add top two values of the stack 
            uint256_t target = {0};
            uint256_t number_1 = stack_pop(machine_state);
            uint256_t number_2 = stack_pop(machine_state);

            add256( &number_1, &number_2, &target);
            stack_push(machine_state, target);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
			break;
		} 
		case MUL: { // Multiply top two values of the stack
            uint256_t target = {0};
            uint256_t number_1 = stack_pop(machine_state);
            uint256_t number_2 = stack_pop(machine_state);

            mul256( &number_1, &number_2, &target);        
            stack_push(machine_state,target);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas5;
            break;
        } 
		case SUB: { // Subtract top two values of the stack
            uint256_t target = {0};
            uint256_t number_1 = stack_pop(machine_state);
            uint256_t number_2 = stack_pop(machine_state);

            minus256( &number_1, &number_2, &target);            
            stack_push(machine_state, target);
            break;
        }
		case DIV: { // Divide (unsign) top two values of the stack 
            uint256_t target = {0};                      
            uint256_t number_1 = stack_pop(machine_state);
            uint256_t number_2 = stack_pop(machine_state);
            if( zero256(&number_2)){
                printf("divide by zero\n");
            }
            else{
                divmod256( &number_1, &number_2, &target, NULL);            
            }
            stack_push(machine_state, target);
            break;
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas5;
            break;
        }
		case SDIV: { // SDIV top two values of the stack
            uint256_t target = {0};
            uint256_t modulo = {0};                      
            uint256_t number_1 = stack_pop(machine_state);
            uint256_t number_2 = stack_pop(machine_state);

            divmod256( &number_1, &number_2, &target, &modulo);            
            stack_push(machine_state, target);
            break;
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas5;
            break;
        }
		case MOD: { // Modulo using top two of the stack
            uint256_t target = {0};
            uint256_t modulo = {0};                      
            uint256_t number_1 = stack_pop(machine_state);
            uint256_t number_2 = stack_pop(machine_state);

            divmod256( &number_1, &number_2, &target, &modulo);            
            stack_push(machine_state, modulo);
            break;
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas5;
            break;
        } 
		case SMOD: { // SMOD
            uint256_t target = {0};
            uint256_t modulo = {0};                      
            uint256_t number_1 = stack_pop(machine_state);
            uint256_t number_2 = stack_pop(machine_state);

            divmod256( &number_1, &number_2, &target, &modulo);            
            stack_push(machine_state, modulo);
            break;
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas5;
            break;
        } 
		case ADDMOD: {	//Add two values and modulo N (take the three values from strack)	
            uint256_t  div = {0};
            uint256_t  modulo = {0}; 
            uint256_t  sum = {0};                     
            uint256_t  number_1 = stack_pop(machine_state);
            uint256_t  number_2 = stack_pop(machine_state);
            uint256_t  modulo_N = stack_pop(machine_state);

            add256( &number_1, &number_2, &sum);
            divmod256( &sum, &modulo_N, &div, &modulo);         
            stack_push(machine_state, modulo);
            break;
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas5;
            break;
        } 
		case MULMOD: { //Multiply two values and modulo N (take the three values from strack)
            uint256_t number_1 = stack_pop(machine_state);
            uint256_t number_2 = stack_pop(machine_state);
            uint256_t modulo_N = stack_pop(machine_state);
            uint256_t mul_res = {0};
            uint256_t modulo = {0};
            uint256_t div = {0};

            mul256( &number_1, &number_2, &mul_res);
            divmod256( &mul_res, &modulo_N, &div, &modulo);     
            stack_push(machine_state,  modulo );
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas8;
            break;
        }
		case EXP: { // Exponentiation modulo top two values of the stack

            // very inefficient way to implement this
        	uint256_t base = stack_pop(machine_state);
            uint64_t exponent = LOWER(LOWER( stack_pop(machine_state)));
            uint256_t accumulator={0};
            uint256_t result ={0};
            uint64_t i;
            if (exponent == 0){
                LOWER(LOWER( result)) =0x01;
            }
            else{
                for (i = 0; i < exponent; i ++){
                    mul256( &base, &accumulator, &result);
                    accumulator = result;
                }
            }
            stack_push(machine_state, result);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas10;
            break;
        } 
        case SIGNEXTEND: { // Sign and extends using top two ...
            printf("SIGNEXTEND not supported\n");

            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas5;
            break;
        } 

		case LT: { // Less Than comparison top two ...
            uint256_t  top = stack_pop(machine_state);
            uint256_t  bot = stack_pop(machine_state);
            uint256_t zeroORone = {0};

            bool bot_greater = gt256(&bot, &top);
			if (bot_greater){
                LOWER(LOWER(zeroORone)) = 0x01;
				stack_push(machine_state, zeroORone);
            }
			else{
				stack_push(machine_state, zeroORone);
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
        } 
		case GT: { // Greater than comparion top two ...
            uint256_t  top = stack_pop(machine_state);
            uint256_t  bot = stack_pop(machine_state);
            uint256_t zeroORone = {0};

            bool top_greater = gt256(&top, &bot);
			if (top_greater){
                LOWER(LOWER(zeroORone)) = 0x01;
				stack_push(machine_state, zeroORone);
            }
			else{
				stack_push(machine_state, zeroORone);
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
        } 
		case SLT: { // Less Than comparison treat values by 2 compliment (note: in C values are 2complement by default)
            uint256_t  top = stack_pop(machine_state);
            uint256_t  bot = stack_pop(machine_state);
            uint256_t zeroORone = {0};

            bool bot_greater = gt256(&bot, &top);
			if (bot_greater){
                LOWER(LOWER(zeroORone)) = 0x01;
				stack_push(machine_state, zeroORone);
            }
			else{
				stack_push(machine_state, zeroORone);
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
        }
		case SGT: { //Greater Than comparison treat values by 2 compliment (note: in C values are 2complement by default)
            uint256_t  top = stack_pop(machine_state);
            uint256_t  bot = stack_pop(machine_state);
            uint256_t zeroORone = {0};

            bool top_greater = gt256(&top, &bot);
			if (top_greater){
                LOWER(LOWER(zeroORone)) = 0x01;
				stack_push(machine_state, zeroORone);
            }
			else{
				stack_push(machine_state, zeroORone);
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
		} 
		case EQ: { // Equal comparison 
            uint256_t  top = stack_pop(machine_state);
            uint256_t  bot = stack_pop(machine_state);
            uint256_t zeroORone = {0};

            bool TopBotEquals = equal256(&top, &bot);
			if (TopBotEquals){
                LOWER(LOWER(zeroORone)) = 0x01;
				stack_push(machine_state, zeroORone);
            }
			else{
				stack_push(machine_state, zeroORone);
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
		} 
		case ISZERO: { // Test if top is zero
            uint256_t  top = stack_pop(machine_state);
            uint256_t zeroORone = {0};

            bool TopZero = zero256(&top);
			if (TopZero){
                LOWER(LOWER(zeroORone)) = 0x01;
				stack_push(machine_state, zeroORone);
            }
			else{
				stack_push(machine_state, zeroORone);
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
		} 
		case AND: { // AND on top two values
            uint256_t top = stack_pop(machine_state);
            uint256_t bot = stack_pop(machine_state);
            uint256_t andRes = {0};
            and256(&top,&bot,&andRes);

            stack_push(machine_state, andRes);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;	
		} 
		case OR: { // OR on top two values
            uint256_t top = stack_pop(machine_state);
            uint256_t bot = stack_pop(machine_state);
            uint256_t orRes = {0};
            or256(&top,&bot,&orRes);

            stack_push(machine_state, orRes);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;		
		} 
		case XOR: { // XOR on top two values
            uint256_t top = stack_pop(machine_state);
            uint256_t bot = stack_pop(machine_state);
            uint256_t xorRes = {0};
            xor256(&top,&bot,&xorRes);


			stack_push(machine_state, xorRes);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;	
		} 
		case NOT: { // NOT on top value 
            uint256_t top = stack_pop(machine_state);
            not256(&top);
			stack_push(machine_state, top);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;	
		} 
		case BYTE: { // BYTE on top two values
            uint256_t i = stack_pop(machine_state);
            uint256_t x = stack_pop(machine_state);

            
            uint32_t shift_value = (uint32_t) LOWER(LOWER(i));
            shift_value = 248 - shift_value * 8 ;
            uint256_t target = {0};
            shiftr256(&x,shift_value, &target);
            uint256_t tmp0ff = {0};
            LOWER(LOWER(tmp0ff))= 0xFF;
            uint256_t y = {0};
            and256(&target,&tmp0ff,&y);
            stack_push(machine_state,y);
            
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;	
		}
        case SHL: { //shift left
            uint256_t shift = stack_pop(machine_state);
            uint256_t value = stack_pop(machine_state);
            uint32_t shift_v = (uint32_t) LOWER(LOWER(shift));
            uint256_t target = {0};
            shiftl256(&value,shift_v, &target);
 
            stack_push(machine_state,target);
            
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;	
		}
        case SHR: { // shift right
            uint256_t shift = stack_pop(machine_state);
            uint256_t value = stack_pop(machine_state);

            
            uint32_t shift_v = (uint32_t) LOWER(LOWER(shift));

            uint256_t target = {0};
            shiftr256(&value,shift_v, &target);
 
            stack_push(machine_state,target);
            
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;	
		}
        case SAR: { // shift int right
            uint256_t shift = stack_pop(machine_state);
            uint256_t value = stack_pop(machine_state);

            
            uint32_t shift_v = (uint32_t) LOWER(LOWER(shift));

            uint256_t target = {0};
            shiftr256(&value,shift_v, &target);
 
            stack_push(machine_state,target);
            
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;	
		}
        case SHA3:{
            uint256_t Offset_256 = stack_pop(machine_state);
            uint256_t Length_256 = stack_pop(machine_state);

            uint64_t offset = LOWER(LOWER(Offset_256));
            uint64_t length = LOWER(LOWER(Length_256));
            // printf("Offset: %llu\n",offset);
            // printf("length: %llu\n",length);
            if( length > 256){
                return -1;
            }
            uint8_t data[length];

            if (offset < 0 ||  offset > MEMORY_SPACE)
            {
                printf("MEM Offeset: 0x%llX is invalid\n" , offset);
            }
            else{
                memcpy(&data, &machine_state->MEM[offset], sizeof( uint8_t ) * length);
            }

            // ARM stores little endian -> Reverse as uint64
            uint64_t *ptr = (uint64_t *)& data;
            for (int i = 0; i < 4; i++){
                ptr[i] = swapLong(&ptr[i]);
            }
            uint8_t result[32];

            get_keccak256(data, sizeof( uint8_t ) * 32, result);

            uint64_t element_upp_upp = 0; 
            uint64_t element_upp_low = 0; 
            uint64_t element_low_upp = 0; 
            uint64_t element_low_low = 0;
            int i = 0;
            for(i = 0; i <8; i++)
            {
                 element_upp_upp  = ( element_upp_upp  << 8 ) | (uint64_t) result[i];
            }
            for(i = 8; i < 16; i++)
            {
                 element_upp_low  = ( element_upp_low  << 8 ) | (uint64_t) result[i];
            }
            for(i = 16; i < 24; i++)
            {
                element_low_upp = ( element_low_upp  << 8 ) | (uint64_t) result[i];           
            }
            for(i = 24; i < 32; i++)
            {
                element_low_low = ( element_low_low  << 8 ) | (uint64_t) result[i];           
            }

            uint256_t hashTOpush = {0};
            LOWER(LOWER(hashTOpush)) = element_low_low;
            LOWER(UPPER(hashTOpush)) = element_low_upp;
            UPPER(LOWER(hashTOpush)) = element_upp_low;
            UPPER(UPPER(hashTOpush)) = element_upp_upp;
            
            stack_push(machine_state, hashTOpush);

            break;
        }

        case ADDRESS:
        {
            stack_push(machine_state,  machine_state->message.address);            
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;
		    break;
        }
        case CALLER:{

            stack_push(machine_state,  machine_state->message.caller);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;
 
		    break;
        }
        case CALLVALUE:{

            stack_push(machine_state,  machine_state->message.call_value);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;
 
		    break;
        }
        case CALLDATALOAD:{
            // printf("CALLDATALOAD: CAREFULL WITH SIZE AND BYTE ORDER\n");
            // uint256_t index = stack_pop(machine_state);
            uint32_t offset = LOWER(LOWER(stack_pop(machine_state)));
            uint256_t dataFROMmessage ={0};

            if (offset < 0 ||  offset > MESSAGEDATASIZE)
            {
                printf("DATA Offeset: 0x%lX is invalid\n" , offset);
            }
            else if ( offset == 0){
                uint64_t function_name = 0;
                memcpy(&function_name, &machine_state->message.data[offset] , sizeof(uint8_t) *4 );
                // printf("fucnction nname %llX",swapLong (&function_name));
                UPPER(UPPER(dataFROMmessage)) = swapLong (&function_name);
                stack_push(machine_state, dataFROMmessage);

            }
            else
            {    
                memcpy(&dataFROMmessage, &machine_state->message.data[offset], sizeof( uint8_t ) * 32);

                UPPER(UPPER(dataFROMmessage)) = swapLong (&UPPER(UPPER(dataFROMmessage)));
                UPPER(LOWER(dataFROMmessage)) = swapLong (&UPPER(LOWER(dataFROMmessage)));
                LOWER(UPPER(dataFROMmessage)) = swapLong (&LOWER(UPPER(dataFROMmessage)));
                LOWER(LOWER(dataFROMmessage)) = swapLong (&LOWER(LOWER(dataFROMmessage)));

                stack_push(machine_state, dataFROMmessage);
            }
            break;

        }
        case CALLDATASIZE:
        {
            // printf("CALLDATASIZE: data size is limited \n");
            uint256_t sizeofdata = {0};
            LOWER(LOWER(sizeofdata)) = machine_state->message.datasize ; 

            stack_push(machine_state,sizeofdata);
		    break;
        }
        case CALLDATACOPY:
        {
            // printf("CALLDATACOPY: CARE ABOUT MEM SIZE AND BYTEORDER \n");
            
            uint64_t destOffset = LOWER(LOWER(stack_pop(machine_state)));
            uint64_t offset = LOWER(LOWER(stack_pop(machine_state)));
            uint64_t length = LOWER(LOWER(stack_pop(machine_state)));

            // printf("CALLDATACOPY: length(%llu)+ offdet(%llu) \n",length,destOffset);
            if( (destOffset + length) < 0 || (destOffset + length) > MEMORY_SPACE ){
               printf("CALLDATACOPY: length(%llu)+ offdet(%llu) out of memory bound\n",length,destOffset);
            }
            else if  ( (offset + length) < 0 || (offset + length) > MESSAGEDATASIZE ) {
               printf("CALLDATACOPY: length(%llu)+ msg.data [(%llu)] out of memory bound\n",length,offset);
            }
            else{
                memcpy(&machine_state->MEM[destOffset] , &s_contract[ offset], sizeof( uint8_t ) * length);
                 if (destOffset + length > max_mem_offset){
                    max_mem_offset = destOffset + length ; 
                }
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;

		    break;
        }
        
        case RETURN:{
            // printf("RETURN !\n");
            uint64_t offset = LOWER(LOWER (  stack_pop(machine_state)));
            uint64_t length = LOWER(LOWER (  stack_pop(machine_state)));
            if( (offset + length) < 0 || (offset + length) > MEMORY_SPACE ){
               printf("MEM: length(%llu) with offdet(%llX) out of memory bound\n",length,offset);
                length = MEMORY_SPACE - 1 ;
                offset = 0;
            }
            uint8_t return_value[ offset + length ];
            memcpy(&return_value, &machine_state->MEM[offset] , sizeof( uint8_t ) * length);
            DeployLength = offset +length;

            memcpy(&deployed_contract, &return_value , sizeof( uint8_t ) * length);
            return RETURN;
            break;
        }

        case BALANCE:
        {
            //  printf("BALANCE: BALANCE of contract not available on local exc...\n");
		    break;
        }
        case CODECOPY:{
            uint64_t MEMOffset = LOWER(LOWER (stack_pop(machine_state)));
            uint64_t Offset = LOWER(LOWER (stack_pop(machine_state)));
            uint64_t length =  LOWER(LOWER (stack_pop(machine_state)));


            // printf("CODECOPY:  MEMOffset 0x%llX ,Offset 0x%llX , length 0x%llu  \n", MEMOffset,Offset,length);

            if( (MEMOffset ) < 0 || (MEMOffset) > MEMORY_SPACE ){
               printf("CODECOPY: length(%llu)+ offdet(%llXF) out of memory bound\n",length,MEMOffset);
            }
            else if (length < 100){
                if (MEMOffset + length > max_mem_offset){
                    max_mem_offset = MEMOffset + length; 
                }
                int len = length ;
                while (len > 0 ) {
                    uint64_t element_upp_upp = 0;
                    uint64_t element_upp_low = 0;
                    uint64_t element_low_upp = 0;
                    uint64_t element_low_low = 0;
                    int i;
                    for(i = 0; i < 8; i++)
                    {
                        element_upp_upp  = ( element_upp_upp  << 8 ) | (uint64_t) s_contract[ Offset];
                        Offset++;
                        len --;
                    }

                    for(i = 0; i < 8; i++)
                    {
                        element_upp_low  = ( element_upp_low  << 8 ) | (uint64_t) s_contract[ Offset];
                        Offset++;
                         len --;
                    }
                    for(i = 0; i < 8; i++)
                    {
                        element_low_upp  = ( element_low_upp  << 8 ) | (uint64_t) s_contract[ Offset];
                        Offset++;
                         len --;
                    }
                    for(i = 0; i < 8; i++)
                    {
                        element_low_low  = ( element_low_low  << 8 ) | (uint64_t) s_contract[ Offset];
                        Offset++;
                        len --;
                    }

                    memcpy(&machine_state->MEM[MEMOffset] , &element_upp_upp,  sizeof(element_upp_upp));
                    memcpy(&machine_state->MEM[MEMOffset] , &element_upp_low,  sizeof(element_upp_upp));
                    memcpy(&machine_state->MEM[MEMOffset] , &element_low_upp,  sizeof(element_upp_upp));
                    memcpy(&machine_state->MEM[MEMOffset] , &element_low_low,  sizeof(element_upp_upp));

                }
            }
            else{
                memcpy(&machine_state->MEM[MEMOffset] , &s_contract[Offset],  length);
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;
            break;
        }
        case CODESIZE:{
            uint256_t code_sz = {0};
            LOWER(LOWER (code_sz) ) = machine_state -> message.codesize;
            stack_push(machine_state, code_sz);   

            break;
        }

        case POP: //POP the first element and discard it
        {
            // printf("[DEBUG]POP Opcode: discard the first element from the stack\n");
            stack_pop(machine_state);
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas2;
            break;
        }


        case MLOAD:
        {
            uint64_t offset = LOWER(LOWER ( stack_pop(machine_state)));
            // printf("MLOAD 0x%llX\n", offset);
            if (offset < 0 ||  offset > MEMORY_SPACE)
            {
                // printf("MEM Offeset: 0x%llX is invalid\n" , offset);
            }
            else
            {    
                uint256_t value= {0};
                memcpy(&value, &machine_state->MEM[offset], sizeof( uint8_t ) * 32);
                stack_push(machine_state, value);   
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
        }

		case MSTORE: { // Store at the memory using as offest and word top two values of the stack 
			// printf("MSTORE opcode\n");
            uint64_t offset = LOWER(LOWER (  stack_pop(machine_state)));
			uint256_t word = stack_pop(machine_state);
           
            if (offset < 0 ||  offset > MEMORY_SPACE){
                printf("MEM Offeset: 0x%llX is invalid\n" , offset);
            }
            else{
                
                
                memcpy(&machine_state->MEM[offset], &word,  sizeof( word ));  
                if (sizeof( word ) > max_mem_offset){
                    max_mem_offset = offset + sizeof( word ); 
                }          
            }

            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;	
		}
        case MSTORE8: {
             uint64_t offset = LOWER(LOWER (  stack_pop(machine_state)));
             uint8_t word =(uint8_t)  LOWER(LOWER (  stack_pop(machine_state)));
            if (offset < 0 ||  offset > MEMORY_SPACE)
            {
                printf("MEM Offeset: 0x%llX is invalid\n" , offset);
            }
            else
            {
                machine_state->MEM[offset] =  word & 0xFF;           
            }

            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
        }
        case SSTORE:{
            uint64_t key = LOWER(LOWER ( stack_pop(machine_state)) );
            uint256_t value = stack_pop(machine_state);
            // printf("STORAGE key: 0x%llX \n" , key);
            max_storage_counter ++;
            
            if (key < 0 ||  key > STORAGE_SPACE)
            {
                printf("STORAGE key: 0x%llX is invalid\n" , key);
            }
            else
            {
                memcpy(&machine_state->STORAGE[key], &value,  sizeof( uint8_t ) * 32); 
            }
            
            // print256(&machine_state->STORAGE[0]);
            // print256(&machine_state->STORAGE[1]);
            // print256(&machine_state->STORAGE[2]);
            // print256(&machine_state->STORAGE[3]);

            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.sstoreSetGas;
            break;
        }
        case SLOAD:{
            uint64_t key = LOWER(LOWER ( stack_pop(machine_state) ) ) ;
            // printf("SLOAD key: 0x%llX\n" , key);
            // print256(&machine_state->STORAGE[0]);
            // print256(&machine_state->STORAGE[1]);
            // print256(&machine_state->STORAGE[2]);
            // print256(&machine_state->STORAGE[3]);
            if (key < 0 ||  key > STORAGE_SPACE)
            {
                printf("STORAGE key: 0x%llX is invalid\n" , key);
            }
            else
            {
                uint256_t value ={0};
                memcpy( &value,&machine_state->STORAGE[key], sizeof( uint8_t ) * 32); 
                stack_push(machine_state, value);
            }
            break;
        }
        case JUMPI: {
            // printf("JUMPI:\n");
            // printf("Currnet PC:  %lu\n",machine_state->PC  );
            uint64_t destination = LOWER(LOWER ( stack_pop(machine_state)) );
            uint64_t cond = LOWER(LOWER ( stack_pop(machine_state) ) ) ;
            machine_state->PC = cond ? destination : machine_state->PC ;

            // printf("Jump PC:  %lu\n",machine_state->PC  );
            break;
        }
        case JUMPDEST:{
            //  printf("JUMP label Nothing to do...\n");
             break;
        }
        case JUMP:{
            uint64_t destination = LOWER(LOWER ( stack_pop(machine_state) ) );
            // printf("jump (%llu) destination\n", destination);
            if (destination < 0 ){
                // printf("jump (%llu) destination out of bound", destination);
            }
            else{
                machine_state->PC = destination ;
            }
            break;
        }

        case  DUP1:	
        case  DUP2:	
        case  DUP3:	
        case  DUP4:	
        case  DUP5:	
        case  DUP6:	
        case  DUP7:	
        case  DUP8:	
        case  DUP9:	
        case  DUP10:
        case  DUP11:
        case  DUP12:
        case  DUP13:
        case  DUP14:
        case  DUP15:
        case  DUP16:
        {
            
            uint64_t offset = (uint64_t)op_code_exc - (uint64_t)DUP1 ;
            // printf("DUP: to clone value at:%llu \n",offset);
            int index_to_clone = machine_state->SP - offset;
            // printf("DUP Index: %d , SP:%d \n",index_to_clone,  machine_state->SP);
          
            if (index_to_clone < 0){
                // printf("DUP Index non valid : %d , SP:%d \n",index_to_clone,  machine_state->SP);
            }
            else{
                uint256_t to_clone = machine_state->STACK[index_to_clone];
                stack_push(machine_state, to_clone);
            }
            break;
        }
        
        case SWAP1:
        case SWAP2:
        case SWAP3:
        case SWAP4:
        case SWAP5:
        case SWAP6:
        case SWAP7:
        case SWAP8:
        case SWAP9:
        case SWAP10:
        case SWAP11:
        case SWAP12:
        case SWAP13:
        case SWAP14:
        case SWAP15:
        case SWAP16:{
            int SwapOffest = machine_state->SP - ( (uint64_t)op_code_exc - (uint64_t)SWAP1 + 1 );

            // printf("[DEBUG] SWAP with offset(%d)\n",SwapOffest);
            // printf("SP : %d \n",machine_state->SP);
            // printf("SwapOffest %016llX\n", LOWER(LOWER(machine_state->STACK[SwapOffest] )));
            if (SwapOffest < 0 || SwapOffest > STACK_SPACE){
                printf("SWAP: offset(%d) is invalid\n",SwapOffest);
            }
            else{
                // printf("SWAP\n");
                uint256_t temp_store = machine_state->STACK[machine_state->SP];
                machine_state->STACK[machine_state->SP] = machine_state->STACK[SwapOffest];
                machine_state->STACK[SwapOffest] = temp_store;       
            }

            break;
        }
        
        case LOG0:
        case LOG1:
        case LOG2:
        case LOG3:
        case LOG4:
        {
            printf("LOG unsupported \n");
            break;
        }

        case TIMESTAMP:
        {
            uint256_t timestamp = {0};
            time = RTIMER_NOW();
            LOWER(LOWER(timestamp) ) = time;
            stack_push(machine_state, timestamp);

            printf("Unused opcode\n");
            break;
        }
        case REVERT:
        {
            uint64_t offset = LOWER(LOWER ( stack_pop(machine_state)));
            uint64_t length = LOWER(LOWER ( stack_pop(machine_state)));

            if (offset < 0 ||  offset > MEMORY_SPACE)
            {
                printf("MEM Offeset: %llX is invalid\n" , offset);
            }
            else
            {    
                uint8_t return_value[ offset + length ];
                memcpy(&return_value, &machine_state->MEM[offset], sizeof( uint8_t ) * length);

            return RETURN;
            }
            machine_state->GAS_Charge = machine_state->GAS_Charge  + GAS_TABLE.stepGas3;
            break;
        }
        case BLOCKHASH :{
            uint256_t block = {0};
            stack_push(machine_state, block);
            break;
        }
        case INVALID:
        {
            printf("Unused opcode\n");
            break;
        }
		default: {
			printf("Unsupported OP_CODE: %X\n",op_code_exc);
            return -1;
			break;
		}
	}
    return 0;
}

void shutdown_machine(Machine *machine_state) {
    machine_state->PC = 0;
	machine_state->SP = 0;
    machine_state->GAS_Charge = 0;
}

// Stack ops
void stack_push(Machine *machine_state, uint256_t item) {
	if (machine_state->SP >= STACK_SPACE){
		stack_overflow_err();
    }
    else{
        machine_state->SP++;
        machine_state->STACK[machine_state->SP] = item;
    }
        
}

uint256_t  stack_pop(Machine *machine_state) {
	uint256_t tmp = machine_state->STACK[machine_state->SP];
	machine_state->SP--;
	return tmp;
}
uint256_t  stack_peek(Machine *machine_state) {
	uint256_t tmp = machine_state->STACK[machine_state->SP];
	return tmp;
}

void stack_duplicate(Machine *machine_state) {
	if (machine_state->SP == -1)
                empty_stack_err("stack_duplicate");
	stack_push(machine_state, machine_state->STACK[machine_state->SP] );
}

void stack_print(Machine *machine_state) {
	int i;
	printf("[top]\n");
    uint256_t *strucPtr;


    unsigned char *charPtr;
	for (i = machine_state->SP; i >= 0; i--) {
        strucPtr = & machine_state->STACK[i];
        charPtr = (unsigned char *)strucPtr;
        for (i = 0; i < sizeof( uint256_t); i++){
            printf("%02x", charPtr[i]);
        }
		printf("\n");
	}
}

// Errors
void stack_overflow_err() {
        printf("[!!!] Fatal error: stack overflow.\n");
        // exit(-1);
}

void empty_stack_err(char *name) {
        printf("[!!!] Fatal error: %s from empty stack.\n", name);
        // exit(-1);
}

void size_err(char *name) {
        printf("[!!!] Fatal error: %s called with insufficient sized stack.\n", name);
        // exit(-1);
}
void get_keccak256(const uint8_t *data, uint16_t length, uint8_t *result) {

    SHA3_CTX context;
    keccak_init(&context);
    keccak_update(&context, (const unsigned char*)data, (size_t)length);
    keccak_final(&context, (unsigned char*)result);

    // Clear out the contents of what we hashed (in case it was secret)
    memset((char*)&context, 0, sizeof(SHA3_CTX));
}


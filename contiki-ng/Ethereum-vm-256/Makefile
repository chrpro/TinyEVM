CONTIKI_PROJECT =  Ethereum_App
all: $(CONTIKI_PROJECT)
TARGET=openmote-cc2538
# MAC_ROUTING=ROUTING_CONF_NULLROUTING
# MAKE_MAC = MAKE_MAC_OTHER
MAKE_NET = MAKE_NET_IPV6
# MAKE_ROUTING = MAKE_ROUTING_NULLROUTING
PROJECT_SOURCEFILES += eth_vm.c
PROJECT_SOURCEFILES += sha3.c
PROJECT_SOURCEFILES += keccak256.c
PROJECT_SOURCEFILES += uint256.c
#DEBUGFLAGS  = -O0 -D _DEBUG
#CFLAGS += -ggdb
#CFLAGS += -O0
#CFLAGS_OPT ?= -Os
#CFLAGS_DEBUG ?= -ggdb3 -fomit-frame-pointer
#CFLAGS  += -Os        # O2 for size
#CFLAGS  += -O0        # do not optimize
#CFLAGS  += -Og   #debugging friendly
#CFLAGS  += -fverbose-asm
#CFLAGS  += -ggdb    #info for the debugger
#CFLAGS += -O0 -mcpu=cortex-m3 -mthumb -mlittle-endian
#CFLAGS += -fshort-enums -fomit-frame-pointer -fno-strict-aliasing
#CFLAGS += -Wall
#CFLAGS += -gdwarf-2
#LDFLAGS += -mcpu=cortex-m3 -mthumb -nostartfiles
#LDFLAGS += -T $(LDSCRIPT)
#LDFLAGS += -Wl,-Map=$(@:.elf=-$(TARGET).map),--cref,--no-warn-mismatch
#OBJCOPY_FLAGS += -O binary --gap-fill 0xff
ThreePokemons[ThreePokemons['class']==name_dict[p]]['longitude'].values,
ThreePokemons[(ThreePokemons['class']==name_dict[p])]['latitude'].values,
marker='.')

.PHONY: renode
renode: all
ifneq ($(TARGET),cc2538dk)
	$(error Only the cc2538dk TARGET is supported for Renode demo scripts)
endif
ifndef SCRIPT
	$(warning SCRIPT not defined! Using "rpl-udp.resc" as default)
	renode hello-world.resc
else
ifeq ($(wildcard $(SCRIPT)),)
	$(error SCRIPT "$(SCRIPT)" does not exist!)
endif
	renode $(SCRIPT)
endif

CONTIKI=../
include $(CONTIKI)/Makefile.include

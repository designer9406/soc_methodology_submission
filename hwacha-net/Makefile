OBJDIR= ./obj/

CC=	riscv64-unknown-linux-gnu-gcc
OBJDUMP=riscv64-unknown-linux-gnu-objdump
AR=	riscv64-unknown-linux-gnu-ar

VPATH=./src

ISA ?= rv64g

ARFLAGS =  rcs
OPTS =	   -O3
LDFLAGS =  -lm
COMMON =   -Iinclude/
CFLAGS = -static -g \
	-march=$(ISA)xhwacha \
	-Wl,--gc-sections \
	-Wall \
	-Wno-comment \
	-Wno-unknown-pragmas \
	-Wno-misleading-indentation \
	-Wfatal-errors \
	-fPIC \
	-ffast-math \
	-fno-common \
	-ffunction-sections \
	-fdata-sections \

CFLAGS +=  $(OPTS)

ifdef SCALAR
CFLAGS += -DUSE_SCALAR
endif

OBJ = util.o layer.o util_asm.o convolutional_layer.o maxpool_layer.o gemm.o gemm_asm.o fc_layer.o fc_layer_asm.o
OBJS = $(addprefix $(OBJDIR), $(OBJ))

DEPS = $(wildcard include/*.h) Makefile obj

#EXECS = tiny_yolo_16 tiny_yolo_32 test squeezenet_32 squeezenet_encoded_32 squeezenet_encoded_compressed_32 alexnet_32 alexnet_encoded_32
EXECS = soc_project
EXECOBJS = $(addsuffix .o, $(addprefix $(OBJDIR), $(EXECS)))
DUMPS = $(addsuffix .dump, $(EXECS))

default : all
all : $(EXECS) $(DUMPS)

%.dump : %
	$(OBJDUMP) -d $^ > $@

$(EXECS): %: ./obj/%.o $(OBJS)
	$(CC) $(COMMON) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.o: %.S $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@
obj:
	mkdir -p obj

.PHONY: clean

clean:
	rm -rf $(OBJS) $(EXECS) $(DUMPS) $(EXECOBJS)


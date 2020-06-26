include ../buildenv.mk

TARGET = app 

RM = rm -f

INC = -I$(SGX_SDK)/include -I$(TOPDIR)/util
LIB := -lpthread -lsgx_urts -lsgx_uae_service -L$(SGX_SDK)/lib64 
CXXFLAGS += -ggdb $(INC) $(LIB)
CFLAGS += -ggdb $(INC) $(LIB)

####################### App Objects ##################
all: $(TARGET)
	
enclave_u.o: enclave_u.c
	@$(CC) $(CFLAGS) -c $< -o $@

enclave_u.c: ../enclave/enclave.edl
	$(SGX_EDGER8R) --use-prefix --untrusted $< --search-path ../enclave --search-path $(SGX_SDK)/include

SRC_CPP=$(wildcard *.cpp)
SRC_CPP+=$(wildcard ../util/*.cpp)
SRC_C=$(wildcard *.c)

SRC_OBJ += $(SRC_CPP:.cpp=.o)
SRC_OBJ += $(SRC_C:.c=.o)

$(TARGET): enclave_u.o $(SRC_OBJ)
	$(CXX) $^ $(CXXFLAGS) -o $@	
	@mv $@ $(TOPDIR)/$(OUTDIR)/
	rm ../util/*.o

clean:	
	$(RM) $(TARGET) *.o ../util/*.o *_u.c *_u.h

.PHONY = all clean

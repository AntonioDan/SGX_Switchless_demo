include ../buildenv.mk

ENCLAVE_NAME = libtestenclave.so
TARGET = libtestenclave.signed.so

Enclave_Cpp_Files := $(wildcard *.cpp)
Enclave_Cpp_Files += $(wildcard ../util/*.cpp)
Enclave_Cpp_Objects := $(Enclave_Cpp_Files:.cpp=.o)
Enclave_Config_File := enclave_config.xml

SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
SGX_TQVL_LIBRARY_PATH := $(TOPDIR)/lib

Enclave_Include_Paths = -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(TOPDIR)/util -I$(TOPDIR)/enclave 

ENCLAVE_CFLAGS += $(Enclave_Include_Paths) -DTRUSTED
ENCLAVE_CXXFLAGS += $(Enclave_Include_Paths) -DTRUSTED

Enclave_Link_Flags := -shared -O2 -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) -L$(SGX_TQVL_LIBRARY_PATH) \
        -Wl,--whole-archive -lsgx_trts -Wl,--no-whole-archive \
        -Wl,--start-group -lsgx_tstdc -lsgx_tcxx -lsgx_tcrypto -lsgx_tservice -Wl,--end-group \
        -Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
        -Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
        -Wl,--defsym,__ImageBase=0 \
        -Wl,--version-script=enclave.lds

all: $(TARGET)
       
$(TARGET): enclave_t.o $(Enclave_Cpp_Objects)
	$(CXX) $^ -MMD -o $(ENCLAVE_NAME) $(Enclave_Link_Flags)	
	$(SGX_ENCLAVE_SIGNER) sign -key enclave_private.pem -enclave $(ENCLAVE_NAME) -out $@ -config $(Enclave_Config_File)
	@cp $@ $(TOPDIR)/$(OUTDIR)/
	rm ../util/*.o

$(Enclave_Cpp_Objects): %.o: %.cpp
	$(CXX) $(ENCLAVE_CXXFLAGS) -c -MMD $< -o $@

enclave_t.o: enclave_t.c
	$(CC) $(ENCLAVE_CFLAGS) -c -MMD $< -o $@

enclave_t.c: $(SGX_EDGER8R) enclave.edl 
	$(SGX_EDGER8R) --use-prefix --trusted enclave.edl --search-path $(SGX_SDK)/include 

clean:
	$(RM) *.o *.d ../util/*.o enclave_t.* $(ENCLAVE_NAME) $(TARGET)

.PHONY: all clean

LINK    = @echo linking $@ && g++
CXX     = @echo compiling $@ && g++
AR      = @echo generating static library $@ && ar rc

# LINK    = g++
# CXX     = g++
# AR      = ar rc

CXXFLAGS   = -g -Wall -Wno-format -Wno-unused-variable -Wno-unused-but-set-parameter -Wno-unused-result -fPIC -std=c++11 -O3 -lpthread -fsanitize=address -fno-omit-frame-pointer -static-libasan
HEADER  = 
LIBS    = libsimulator.a 
LINKFLAGS = -fsanitize=address

OBJECT 	:= 	main.o bsw.o poa.o chain.o phmm.o

ifeq ($(debug),1)
CXXFLAGS+= -DDEBUG
endif

ifeq ($(profile),1)
CXXFLAGS+= -DPROFILE
endif

LIB_SIMULATOR_OBJECTS= \
			alu_32.o \
			compute_unit_32.o \
			crossbar.o \
			regfile.o \
			comp_decoder.o \
			data_buffer.o \
			pe.o \
			FIFO.o \
			pe_array.o

BIN_PATH = ./

TARGET = sim

$(TARGET) : $(OBJECT) $(LIBS)
	$(LINK) $(FLAGS) $(LINKFLAGS) -o $@ $^

.cpp.o:
	$(CXX) -c $(HEADER) $(CXXFLAGS) -fpermissive -o $@ $<

libsimulator.a:  $(LIB_SIMULATOR_OBJECTS)
	rm -f $@
	$(AR) $@ $(LIB_SIMULATOR_OBJECTS)
	ranlib $@

# libkernel.a: bsw.o poa.o chain.o
# 	rm -f $@
# 	$(AR) $@ bsw.o poa.o chain.o
# 	ranlib $@

install: $(TARGET)
	cp $(TARGET) $(BIN_PATH)

clean:
	rm -rf $(TARGET) *.o *.so *.a
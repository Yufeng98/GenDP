CXX=g++
AR=ar rc
CXXFLAGS=-g -Wall -Wextra -O3 -fopenmp -Wno-unused-parameter -Wno-unused-variable
CXXFLAGS+= -std=c++11
SIMULATOR_PATH=../../
INCLUDES= -I$(SIMULATOR_PATH)
LIBS=
CXXFLAGS+= $(INCLUDES)

OBJECTS=ksw.o

ifeq ($(power),1)
CXXFLAGS+= -DRAPL=1
INCLUDES+= -I../rapl-tools
OBJECTS+= ../rapl-tools/Rapl.o
endif

ifeq ($(mac),1)
CXX=/usr/local/Cellar/llvm/12.0.1/bin/clang++
INCLUDES+= -I/usr/local/include
LIBS+= -L/usr/local/lib -lomp
endif

LIBOBJECTS= \
	$(SIMULATOR_PATH)/comp_decoder.o \
	$(SIMULATOR_PATH)/alu_32.o \
	$(SIMULATOR_PATH)/compute_unit_32.o \

PROG=ksw-test

# .cpp.o:
# 		$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

all:$(PROG)

$(PROG):$(OBJECTS) libsim.a
		$(CXX) $(CXXFLAGS) $(LIBS) $^ -o $@

libsim.a: $(LIBOBJECTS)
	rm -f $@
	$(AR) $@ $(LIBOBJECTS)
	ranlib $@

power:$(PROG)
	cp ./$(PROG) /tmp/$(PROG)
	sudo setcap cap_sys_rawio+ep /tmp/$(PROG)

clean:
		rm $(PROG) $(LIBOBJECTS) $(OBJECTS)

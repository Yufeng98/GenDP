
CC = gcc
CXX = g++
CXXFLAGS= -g -DUSE_WEIGHTED_LINKS -DUSE_PROJECT_HEADER -fpermissive -static-libasan
CXXFLAGS+= $(INCLUDE)
CFLAGS= -g -DUSE_WEIGHTED_LINKS -DUSE_PROJECT_HEADER
INCLUDE= -I$(SIMULATOR_PATH)
LDFLAGS= -L. # -L.$(SIMULATOR_PATH)
LIBS= -llpo # -lsimulator
AR=ar rc
SIMULATOR_PATH=../../


TARGETS=poa liblpo.a poa_doc libbflag.a libsimulator.a

# align_score.c CAN BE USED TO ADD CUSTOMIZED SCORING FUNCTIONS
OBJECTS= \
	align_score.o \
	main.o

ifeq ($(profile),1)
CXXFLAGS+= -DPROFILE
endif

LIBOBJECTS= \
	$(SIMULATOR_PATH)/comp_decoder.o \
	$(SIMULATOR_PATH)/alu_32.o \
	$(SIMULATOR_PATH)/compute_unit_32.o \
	black_flag.o \
	seq_util.o \
	fasta_format.o \
	msa_format.o \
	align_lpo2.o \
	align_lpo_po2.o \
	buildup_lpo.o \
	lpo.o \
	heaviest_bundle.o \
	lpo_format.o \
	create_seq.o \
	remove_bundle.o \
	numeric_data.o \
	stringptr.o

all: poa


clean:
	rm -f $(OBJECTS) $(LIBOBJECTS) $(TARGETS)

liblpo.a: $(LIBOBJECTS)
	rm -f $@
	$(AR) $@ $(LIBOBJECTS)
	ranlib $@

# NB: LIBRARY MUST FOLLOW OBJECTS OR LINK FAILS WITH UNRESOLVED REFERENCES!!
poa: $(OBJECTS) liblpo.a # libsimulator.a
	$(CXX) -o $@ $(OBJECTS) $(LIBS) $(LDFLAGS) $(INCLUDE) 

what:
	@echo poa: partial-order based sequence alignment program
	@echo liblpo.a: partial-order alignment and utilities function library

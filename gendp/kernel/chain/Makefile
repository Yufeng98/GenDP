# CXX = icpc
CXX = g++

# path #
SRC_PATH = src
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin

# executable #
BIN_NAME = chain

# extensions #
SRC_EXT = cpp

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)


ARCH_FLAGS=	-msse4.1

ifeq ($(arch),sse41)
	ARCH_FLAGS=-msse4.1
else ifeq ($(arch),avx2)
	ifeq ($(CXX), icpc)
		ARCH_FLAGS=-march=core-avx2 #-xCORE-AVX2
	else	
		ARCH_FLAGS=-mavx2
	endif
else ifeq ($(arch),avx512)
	ifeq ($(CXX), icpc)
		ARCH_FLAGS=-xCORE-AVX512
	else	
		ARCH_FLAGS=-mavx512bw
	endif
else ifeq ($(arch),native)
		ARCH_FLAGS=-march=native
else ifneq ($(arch),)
	## To provide a different architecture flag like -march=core-avx2.
		ARCH_FLAGS=$(arch)
endif

# flags #
# COMPILE_FLAGS = -std=c++11 -Wall -Wextra -g -O3 -fopenmp -xAVX2 -axAVX2
# VTUNE_HOME= /opt/intel/oneapi/vtune/2021.1.1
COMPILE_FLAGS = -std=c++11 -Wall -Wextra -Wno-unused-result -g -O3 -fopenmp $(ARCH_FLAGS) -Wno-unused-parameter -Wno-unused-variable -static-libasan
INCLUDES = -I../tools/minimap2 -I$(SIMULATOR_PATH)
LDFLAGS = -L.
# Space-separated pkg-config libraries used by this project
LIBS = -ldl -lsim
# LIBS = -L../tools/minimap2 -lminimap2
SIMULATOR_PATH=../../


ifneq ($(VTUNE_HOME),)
COMPILE_FLAGS += -DVTUNE_ANALYSIS=1
INCLUDES += -I${VTUNE_HOME}/include
LIBS +=  -L${VTUNE_HOME}/lib64 -littnotify
endif

ifeq ($(print),1)
CXXFLAGS+= -DPRINT_OUTPUT=1
endif

ifeq ($(profile),1)
CXXFLAGS+= -DPROFILE
endif

ifeq ($(power),1)
CXXFLAGS+= -DRAPL=1
INCLUDES+= -I../rapl-tools
OBJECTS+= ../rapl-tools/Rapl.o
endif

ifeq ($(mac),1)
CXX=/usr/local/Cellar/llvm/12.0.1/bin/clang++
INCLUDE+= -I/usr/local/include
LIBS+= -L/usr/local/lib -lomp
endif

# OBJECTS+=$(SIMULATOR_PATH)/comp_decoder.o \
# 	$(SIMULATOR_PATH)/alu_32.o \
# 	$(SIMULATOR_PATH)/compute_unit_32.o

LIBOBJECTS= \
	$(SIMULATOR_PATH)/comp_decoder.o \
	$(SIMULATOR_PATH)/alu_32.o \
	$(SIMULATOR_PATH)/compute_unit_32.o \

.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	rm $(LIBOBJECTS)
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

libsim.a: $(LIBOBJECTS)
	rm -f $@
	ar rc $@ $(LIBOBJECTS)
	ranlib $@

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS) libsim.a
	@echo "Linking: $@"
	# $(CXX) -O3 -fopenmp -xAVX2 -axAVX2 $(OBJECTS) -o $@
	$(CXX) -O3 -fopenmp $(ARCH_FLAGS) $(OBJECTS) $(INCLUDES) $(LIBS) $(CXXFLAGS) $(LDFLAGS) -o $@
	
power: $(BIN_PATH)/$(BIN_NAME)
	cp $< /tmp/$(BIN_NAME)
	sudo setcap cap_sys_rawio+ep /tmp/$(BIN_NAME)

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -MP -MMD -c $< -o $@

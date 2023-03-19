#CXXFLAGS=-c -Wall -std=c++11 -g -mfpmath=sse -O4 -march=corei7 -pedantic
CXX = g++
CXXFLAGS=-Wall -Wno-unused-variable -Wno-unused-value -std=c++1y -g -msse -mfpmath=sse -ffast-math -O2 -pedantic -march=corei7-avx# -Wa,-q  -fsanitize=address -static-libasan -g
SIMULATOR_PATH=../../../
INCLUDE=-I$(SIMULATOR_PATH)
LDFLAGS=-L.
SOURCES= testcase.cpp testcase_iterator.cpp main.cpp diagonals.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=../pairhmm
LIBS = -lsim

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

diagonals.o: diagonals.cpp diagonals.h
main.o: main.cpp input_reader.h testcase_iterator.h testcase.h read.h simulation.h\
 utils.h haplotype.h pairhmm.h pairhmm_impl.h constants.h diagonals.h  pairhmm_scalarimpl.h pairhmm_vecimpl.h pairhmm_sseimpl.h pairhmm_avximpl.h
testcase.o: testcase.cpp testcase.h read.h utils.h haplotype.h
testcase_iterator.o: testcase_iterator.cpp testcase_iterator.h testcase.h \
 read.h utils.h haplotype.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(INCLUDES) -c $< -o $@

clean:
	rm *.o ../pairhmm 

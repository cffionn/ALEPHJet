#std-c++11
CXX = g++
CXXFLAGS = -Wall -O3 -Wextra -Wno-unused-local-typedefs -Wno-deprecated-declarations -std=c++11
ifeq "$(GCCVERSION)" "1"
  CXXFLAGS += -Wno-error=misleading-indentation
endif

ROOT=`root-config --cflags --glibs`
FASTJET=`fastjet-config --cxxflags --libs`
INCLUDES=-I/usr/include -I$(PWD) -I$(FASTJETPATH)

MKDIR_BIN = mkdir -p $(PWD)/bin
MKDIR_PDFDIR = mkdir -p $(PWD)/pdfDir
MKDIR_OUTPUT = mkdir -p $(PWD)/output

all: mkdirBin mkdirOutput mkdirPdfdir bin/processCleanedAleph.exe bin/createAlephJet.exe bin/exchange.exe

mkdirBin:
	$(MKDIR_BIN)

mkdirOutput:
	$(MKDIR_OUTPUT)

mkdirPdfdir:
	$(MKDIR_PDFDIR)


bin/processCleanedAleph.exe: src/processCleanedAleph.C
	$(CXX) $(CXXFLAGS) src/processCleanedAleph.C $(ROOT) $(FASTJET) $(INCLUDES) -lm -lpthread -lX11 -o bin/processCleanedAleph.exe

bin/createAlephJet.exe: src/createAlephJet.C
	$(CXX) $(CXXFLAGS) src/createAlephJet.C $(ROOT) $(FASTJET) $(INCLUDES) -lm -lpthread -lX11 -o bin/createAlephJet.exe

bin/exchange.exe: src/exchange.C
	$(CXX) $(CXXFLAGS) src/exchange.C -o bin/exchange.exe

clean:
	rm *~ || true
	rm *# || true
	rm include/*~ || true
	rm include/#*# || true
	rm src/*~ || true
	rm src/#*# || true
	rm bash/*~ || true
	rm bash/#*# || true
	rm bin/*.exe || true
	rmdir bin || true

SRCDIR = src
TGT_NAME = festi
UNAME_S := $(shell uname -s)
CFLAGS+=-g -pthread -lrt $(shell pkg-config libcurl --cflags)
CXXFLAGS+=-std=c++14  -Iinclude $(CFLAGS)
INSTALL_PREFIX=/usr/local
LDFLAGS+=-lpigpiod_if2 $(shell pkg-config libcurl --libs)
BUILDDIR=build
$(shell mkdir -p $(BUILDDIR) >/dev/null)
SOURCES = $(wildcard src/*.cpp)
OBJ = $(subst /src,,$(patsubst %.cpp,$(BUILDDIR)/%.o,$(SOURCES)))

ifeq ($(UNAME_S),Linux)
        SO_SUFFIX=so
endif
ifeq ($(UNAME_S),Darwin)
        SO_SUFFIX=dylib
endif

TARGET = $(BUILDDIR)/$(TGT_NAME)

$(TARGET): $(OBJ)
		@echo [LINK] $(TGT_NAME)
		@$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(CFLAGS) $(CXXFLAGS)

debug: CFLAGS+= -O0
debug: $(TARGET)

install: $(TARGET)
		@echo [INSTALL] 
		@mkdir -p $(INSTALL_PREFIX)/bin
		@cp $(TARGET) $(INSTALL_PREFIX)/bin
		
uninstall:
		@echo [UNINSTALL]
		@rm -f $(INSTALL_PREFIX)/bin/$(TGT_NAME)
clean:
		@echo [CLEAN]
		@rm -f $(OBJ)

.PHONY: clean test install uninstall

$(OBJ): $(BUILDDIR)/%.o : src/%.cpp
		@echo [C++] $<
		@$(COMPILE.cc) $(OUTPUT_OPTION) $<

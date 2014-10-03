
BIN_DIR = bin
BUILD_DIR = build
ASM_DIR = asm
SRC_DIR = test

CC = clang++ 
ARCH := -arch x86_64 -msse4
CFLAGS = -std=c++11 -O3 -g -D___OSX  -D___SSE -D___SSE4 $(ARCH) -Iinclude/math/ -Iinclude/collections -Iinclude/ -Wunused-value
LDFLAGS = -lstdc++

EXES = testunitcollections profilelinkedlist profilesort profilearray profiletreemap profiletreeset delaunay
EXES := $(EXES:%=$(BIN_DIR)/%)

.PHONY: all $(EXES)
all: $(EXES)

print:
	@echo "Headers:" $(EXES)
	@echo "Compile Flags:" $(CFLAGS)
	@echo "Link Flags:" $(LDFLAGS)
	
# Target directory rules

$(BUILD_DIR): 
	mkdir -p $(BUILD_DIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
$(ASM_DIR):
	mkdir -p $(ASM_DIR)

# Object rules

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Executable rules

$(BIN_DIR)/delaunay: $(BUILD_DIR)/Delaunay.o test/Delaunay.h test/PeriodicDelaunay.h test/Bounds.h | $(BIN_DIR)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

$(BIN_DIR)/testunitcollections: $(BUILD_DIR)/UnitTestCollections.o | $(BIN_DIR)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

$(BIN_DIR)/testunitmath: $(BUILD_DIR)/UnitTestMath.o | $(BIN_DIR)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

$(BIN_DIR)/profilelinkedlist: $(BUILD_DIR)/ProfileLinkedList.o | $(BIN_DIR)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

$(BIN_DIR)/profilesort: $(BUILD_DIR)/ProfileSort.o | $(BIN_DIR)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

$(BIN_DIR)/profilearray: $(BUILD_DIR)/ProfileArray.o | $(BIN_DIR)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

$(BIN_DIR)/profiletreemap: $(BUILD_DIR)/ProfileTreeMap.o | $(BIN_DIR)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

$(BIN_DIR)/profiletreeset: $(BUILD_DIR)/ProfileTreeSet.o | $(BIN_DIR)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<



clean:
	rm -rf $(BIN_DIR)
	rm -rf $(BUILD_DIR)

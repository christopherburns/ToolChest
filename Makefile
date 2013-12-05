
BIN_DIR = bin
BUILD_DIR = build
ASM_DIR = asm
SRC_DIR = test

CC = clang++ 

CFLAGS = -std=c++11 -O3 -g -D___OSX -Itoolchest/ -Wunused-value
LDFLAGS = -lstdc++

EXES = testunit profilelinkedlist profilesort profilearray profiletreemap profiletreeset delaunay
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

$(BIN_DIR)/testunit: $(BUILD_DIR)/UnitTests.o | $(BIN_DIR)
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

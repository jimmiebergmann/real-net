# Folders
BIN_FOLDER := bin
LIB_FOLDER := lib
OBJ_FOLDER := obj
INCLUDE_FOLDER := include

# Get real-net files
REALNET_CPP = $(wildcard source/*.cpp) $(wildcard source/core/*.cpp)
REALNET_RELEASE_OBJ := $(patsubst %.cpp,$(OBJ_FOLDER)/release/realnet/%.o, $(REALNET_CPP))
REALNET_DEBUG_OBJ := $(patsubst %.cpp,$(OBJ_FOLDER)/debug/realnet/%.o, $(REALNET_CPP))

all: release debug test

$(REALNET_RELEASE_OBJ): obj/release/realnet/%.o: %.cpp
	$(CXX) -std=c++11 -c -Iinclude $< -o $@

$(REALNET_DEBUG_OBJ): obj/debug/realnet/%.o: %.cpp
	$(CXX) -std=c++11 -g -c -Iinclude $< -o $@


release: release_folders archive_release
debug: debug_folders archive_debug

archive_release: $(REALNET_RELEASE_OBJ)
	ar rcs $(LIB_FOLDER)/realnet.a $(REALNET_RELEASE_OBJ)
archive_debug: $(REALNET_DEBUG_OBJ)
	ar rcs $(LIB_FOLDER)/realnet-d.a $(REALNET_DEBUG_OBJ)


folders:
	mkdir -p $(BIN_FOLDER)
	mkdir -p $(LIB_FOLDER)
release_folders: folders
	mkdir -p $(OBJ_FOLDER)/release/realnet/source/core
debug_folders: folders
	mkdir -p $(OBJ_FOLDER)/debug/realnet/source/core
test_release_folders: folders
	mkdir -p $(OBJ_FOLDER)/release/test
test_debug_folders: folders
	mkdir -p $(OBJ_FOLDER)/debug/test


.PHONY: clean clean_all clean_release clean_debug
clean_all:
	rm -r -f $(OBJ_FOLDER)
	rm -r -f $(LIB_FOLDER)
	rm -r -f $(BIN_FOLDER)
clean:
	rm -r -f $(OBJ_FOLDER)
clean_release:
	rm -r -f $(OBJ_FOLDER)/release/realnet
clean_debug:
	rm -r -f $(OBJ_FOLDER)/debug/realnet

clean_test: clean_test_release clean_test_debug
clean_test_release: clean_release
	rm -r -f $(OBJ_FOLDER)/release/test
clean_test_debug: clean_debug
	rm -r -f $(OBJ_FOLDER)/debug/test


$(OBJ_FOLDER)/release/test/test.o: test/test.cpp
	$(CXX) -std=c++11 -Itest/googletest/googletest/include -Iinclude -c $< -o $@

$(OBJ_FOLDER)/debug/test/test.o: test/test.cpp
	$(CXX) -std=c++11 -g -Itest/googletest/googletest/include -Iinclude -c $< -o $@


test_release: release test_release_folders $(OBJ_FOLDER)/release/test/test.o
	$(CXX) -o $(BIN_FOLDER)/test $(OBJ_FOLDER)/release/test/test.o -s test/googletest/googletest/make/gtest_main.a lib/realnet.a -lpthread

test_debug: debug test_debug_folders $(OBJ_FOLDER)/debug/test/test.o
	$(CXX) -g -o $(BIN_FOLDER)/test-d $(OBJ_FOLDER)/debug/test/test.o test/googletest/googletest/make/gtest_main.a lib/realnet-d.a -lpthread

test: test_release test_debug


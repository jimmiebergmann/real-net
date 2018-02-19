# Folders
BIN_FOLDER := bin
LIB_FOLDER := lib
OBJ_FOLDER := obj
INCLUDE_FOLDER := include

# Get real-net files
REALNET_CPP = $(wildcard source/*.cpp) $(wildcard source/core/*.cpp)
REALNET_RELEASE_OBJ := $(patsubst %.cpp,$(OBJ_FOLDER)/release/realnet/%.o,$(notdir $(REALNET_CPP)))
REALNET_DEBUG_OBJ := $(patsubst %.cpp,$(OBJ_FOLDER)/debug/realnet/%.o,$(notdir $(REALNET_CPP)))

all: release debug test

$(REALNET_RELEASE_OBJ): $(REALNET_CPP)  
	$(CXX) -c $< -o $@

$(REALNET_DEBUG_OBJ): $(REALNET_CPP)  
	$(CXX) -g -c $< -o $@


release: release_folders archive_release
debug: debug_folders archive_debug

archive_release: $(REALNET_RELEASE_OBJ)
	ar rcs $(LIB_FOLDER)/realnet.a $<
archive_debug: $(REALNET_DEBUG_OBJ)
	ar rcs $(LIB_FOLDER)/realnet-d.a $<


folders:
	mkdir -p $(BIN_FOLDER)
	mkdir -p $(LIB_FOLDER)
release_folders: folders
	mkdir -p $(OBJ_FOLDER)/release/realnet
debug_folders: folders
	mkdir -p $(OBJ_FOLDER)/debug/realnet


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



#test: folders ../obj/test/test.o ../obj/test/Yaml.o
#	$(CXX) -o ../bin/test ../obj/test/test.o ../obj/test/Yaml.o  -s  googletest/googletest/make/gtest_main.a -lpthread

#../obj/test/test.o: test.cpp
#	$(CXX) -std=c++11 -Igoogletest/googletest/include -I../yaml -c test.cpp -o ../obj/test/test.o

#../obj/test/Yaml.o: ../yaml/Yaml.cpp
#	$(CXX) -std=c++11 -I../yaml -c ../yaml/Yaml.cpp -o ../obj/test/Yaml.o

#folders:
#	mkdir -p ../bin
#	mkdir -p ../obj/test
	

#.PHONY: clean
#clean:
#	rm -r ../obj

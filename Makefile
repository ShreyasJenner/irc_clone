CXX=g++
CXXFLAGS=-Wall -Wextra -Iinclude
FLAGS=

SRC=src
OBJ=obj
BIN=bin
APP=app

# Source files
SERVER_SRC=$(APP)/server.cpp
CLIENT_SRC=$(APP)/client.cpp
CLIENT_2_SRC=$(APP)/client2.cpp

# Object files
SERVER_OBJ=$(OBJ)/server.o
CLIENT_OBJ=$(OBJ)/client.o
CLIENT_2_OBJ=$(OBJ)/client2.o

# Find all common source files in src/ excluding server.cpp and client.cpp
COMMON_SRCS=$(filter-out $(SERVER_SRC) $(CLIENT_SRC), $(shell find $(SRC) -name '*.cpp'))
COMMON_OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(COMMON_SRCS))

SERVER=$(BIN)/server
CLIENT=$(BIN)/client
CLIENT_2=$(BIN)/client2

# Build target
build: $(OBJ) $(BIN) $(SERVER) $(CLIENT) $(CLIENT_2)

# Ensure obj and bin directories exist
$(OBJ) $(BIN):
	@mkdir -p $(OBJ) $(BIN)

$(SERVER): $(SERVER_OBJ) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(FLAGS)

$(CLIENT): $(CLIENT_OBJ) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(FLAGS)

$(CLIENT_2): $(CLIENT_2_OBJ) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(FLAGS)

# Compile common source files from src/
$(OBJ)/%.o: $(SRC)/%.cpp | $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile app source files (server.cpp and client.cpp)
$(OBJ)/%.o: $(APP)/%.cpp | $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(BIN)

.PHONY: build clean


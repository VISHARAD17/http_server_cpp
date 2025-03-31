CC = g++
CFLAGS = -std=c++17 -Wall
TARGET = http_server
BUILD_DIR = build
SRC_DIR = .

SOURCES = main.cpp http_message.cpp http_server.cpp
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(OBJECTS)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/http_server.h $(SRC_DIR)/http_message.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/http_message.o: $(SRC_DIR)/http_message.cpp $(SRC_DIR)/http_message.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/http_message.cpp -o $(BUILD_DIR)/http_message.o

$(BUILD_DIR)/http_server.o: $(SRC_DIR)/http_server.cpp $(SRC_DIR)/http_server.h $(SRC_DIR)/http_message.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/http_server.cpp -o $(BUILD_DIR)/http_server.o

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

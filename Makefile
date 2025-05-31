CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -O3 -Iinclude
LDFLAGS = -lsndfile
SRC_DIR = src
BUILD_DIR = build
TARGET = detector.out

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET) $(AUDIO_FILE)

bear: clean
	bear -- make

.PHONY: all clean run test

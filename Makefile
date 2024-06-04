# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Directories
SRC_DIR = .
UTILS_DIR = Utils
NETWORK_DIR = $(UTILS_DIR)/Network
CLIPBOARD_DIR = $(UTILS_DIR)/Clipboard
CRYPTO_DIR = $(UTILS_DIR)/Crypto
LOGGING_DIR = $(UTILS_DIR)/Logging
NOTIFICATIONS_DIR = $(UTILS_DIR)/Notifications

# Sources and headers
SRC_FILES = $(SRC_DIR)/uniclip.cpp \
            $(NETWORK_DIR)/Network.cpp \
            $(CLIPBOARD_DIR)/Clipboard.cpp \
            $(CRYPTO_DIR)/Crypto.cpp \
            $(LOGGING_DIR)/Logging.cpp \
            $(NOTIFICATIONS_DIR)/Notify.cpp

# Object files
OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Target executable
TARGET = Uniclip

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ_FILES) $(TARGET)

.PHONY: all clean

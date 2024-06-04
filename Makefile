CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

SRC_DIR = .
UTILS_DIR = Utils
NETWORK_DIR = $(UTILS_DIR)/Network
CLIPBOARD_DIR = $(UTILS_DIR)/Clipboard
CRYPTO_DIR = $(UTILS_DIR)/Crypto
LOGGING_DIR = $(UTILS_DIR)/Logging
NOTIFICATIONS_DIR = $(UTILS_DIR)/Notifications

SRC_FILES = $(SRC_DIR)/uniclip.cpp \
            $(NETWORK_DIR)/Network.cpp \
            $(CLIPBOARD_DIR)/Clipboard.cpp \
            $(CRYPTO_DIR)/Crypto.cpp \
            $(LOGGING_DIR)/Logging.cpp \
            $(NOTIFICATIONS_DIR)/Notify.cpp

OBJ_FILES = $(SRC_FILES:.cpp=.o)

TARGET = Uniclip

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) $(TARGET)

.PHONY: all clean

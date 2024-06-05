CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

SRC_DIR = .
UTILS_DIR = Utils
NETWORK_DIR = $(UTILS_DIR)/Network
CLIPBOARD_DIR = $(UTILS_DIR)/Clipboard
CRYPTO_DIR = $(UTILS_DIR)/Crypto
LOGGING_DIR = $(UTILS_DIR)/Logging
NOTIFICATIONS_DIR = $(UTILS_DIR)/Notifications

BUILD_DIR = build

SRC_FILES = $(SRC_DIR)/uniclip.cpp \
            $(NETWORK_DIR)/Network.cpp \
            $(CLIPBOARD_DIR)/Clipboard.cpp \
            $(CRYPTO_DIR)/Crypto.cpp \
            $(LOGGING_DIR)/Logging.cpp \
            $(NOTIFICATIONS_DIR)/Notify.cpp

OBJ_FILES = $(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BUILD_DIR)/Uniclip

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR) $(BUILD_DIR)/$(UTILS_DIR) $(BUILD_DIR)/$(NETWORK_DIR) $(BUILD_DIR)/$(CLIPBOARD_DIR) $(BUILD_DIR)/$(CRYPTO_DIR) $(BUILD_DIR)/$(LOGGING_DIR) $(BUILD_DIR)/$(NOTIFICATIONS_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

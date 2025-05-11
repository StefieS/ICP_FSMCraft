.PHONY: all build run clean doc pack

BUILD_DIR := build
SRC_DIR := src
ZIP_NAME := xlesigm00-xnovakf00-xmarina00.zip

all: build

build:
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && $(MAKE)

run: build
	cd $(BUILD_DIR) && ./$(SRC_DIR)/fsmtool

clean:
	@rm -rf $(BUILD_DIR) $(ZIP_NAME) doc

doxygen:
	doxygen Doxyfile

pack: clean
	zip -r $(SRC_DIR) examples README.txt doc Makefile CMakeLists.txt
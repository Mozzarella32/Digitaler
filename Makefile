BUILD_DIR := build
TARGET_PATH := app/Digitaler

.PHONY: all debug release clean run

NUM_THREADS := $(shell nproc)

all: debug

debug:
	@mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR) -j$(NUM_THREADS)
	@mkdir -p app

release:
	@mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) -j$(NUM_THREADS)
	@mkdir -p app

run:
	$(TARGET_PATH)

clean:
	@rm -rf $(BUILD_DIR) $(TARGET_PATH)

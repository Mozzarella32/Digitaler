BUILD_DIR := build
TARGET_PATH := app/Digitaler
LOG_PATH := app/Error.log

.PHONY: all debug release clean run

NUM_THREADS := $(shell nproc)

all: debug

debug:
	@mkdir -p $(BUILD_DIR)
	-rm $(LOG_PATH)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR) -j$(NUM_THREADS)

release:
	@mkdir -p $(BUILD_DIR)
	-rm $(LOG_PATH)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) -j$(NUM_THREADS)

run:
	$(TARGET_PATH)

clean:
	-rm -rf $(BUILD_DIR) $(TARGET_PATH) $(LOG_PATH)

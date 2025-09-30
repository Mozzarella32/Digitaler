BUILD_DIR := build
TARGET_PATH := app/Digitaler
LOG_PATH := app/Error.log

.PHONY: all debug release clean run build

NUM_THREADS := $(shell nproc)

GENERATOR ?=

ifeq ($(GENERATOR),)
GENERATOR_FLAG :=
else
GENERATOR_FLAG := -G "$(GENERATOR)"
endif

all: release build run

debug: clean
	@mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) $(GENERATOR_FLAG) -DCMAKE_BUILD_TYPE=Debug

release: clean
	@mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) $(GENERATOR_FLAG) -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build $(BUILD_DIR) -j$(NUM_THREADS)

run:
	$(TARGET_PATH)

clean:
	-rm -rf $(BUILD_DIR) $(TARGET_PATH) $(LOG_PATH)

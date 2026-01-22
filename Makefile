# Makefile для video2ASCII
# Альтернатива сборки через CMake

# Компилятор
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic

# Директории
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

# Исходные файлы (явно перечисляем для правильного порядка)
SOURCES := $(SRC_DIR)/main.cpp \
           $(SRC_DIR)/ascii_cache.cpp \
           $(SRC_DIR)/video_downloader.cpp \
           $(SRC_DIR)/video_processor.cpp \
           $(SRC_DIR)/utils.cpp \
           $(SRC_DIR)/cli.cpp \
           $(SRC_DIR)/video_path_resolver.cpp \
           $(SRC_DIR)/logger.cpp \
           $(SRC_DIR)/signal_handler.cpp \
           $(SRC_DIR)/video_validator.cpp \
           $(SRC_DIR)/video_cache.cpp \
           $(SRC_DIR)/processing_stats.cpp \
           $(SRC_DIR)/interactive_cli.cpp
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Имя исполняемого файла
TARGET := video2ASCII

# OpenCV настройки (через pkg-config)
PKG_CONFIG := pkg-config
OPENCV_FLAGS := $(shell $(PKG_CONFIG) --cflags opencv4 2>/dev/null || $(PKG_CONFIG) --cflags opencv 2>/dev/null)
OPENCV_LIBS := $(shell $(PKG_CONFIG) --libs opencv4 2>/dev/null || $(PKG_CONFIG) --libs opencv 2>/dev/null)

# Флаги компиляции
INCLUDES := -I$(INCLUDE_DIR) $(OPENCV_FLAGS)
LDFLAGS := $(OPENCV_LIBS) -pthread

# Режим сборки (Debug/Release)
BUILD_TYPE ?= Release

ifeq ($(BUILD_TYPE),Debug)
    CXXFLAGS += -g -O0 -DDEBUG
else
    CXXFLAGS += -O3 -DNDEBUG
endif

# Цвета для вывода (опционально)
ifneq ($(NO_COLOR),1)
    COLOR_GREEN := \033[0;32m
    COLOR_YELLOW := \033[0;33m
    COLOR_RED := \033[0;31m
    COLOR_RESET := \033[0m
endif

.PHONY: all clean install uninstall help check-opencv

# Цель по умолчанию
all: check-opencv $(TARGET)
	@echo "$(COLOR_GREEN)✓ Сборка завершена успешно!$(COLOR_RESET)"

# Проверка наличия OpenCV
check-opencv:
	@echo "$(COLOR_YELLOW)Проверка OpenCV...$(COLOR_RESET)"
	@if [ -z "$(OPENCV_FLAGS)" ]; then \
		echo "$(COLOR_RED)✗ Ошибка: OpenCV не найден через pkg-config$(COLOR_RESET)"; \
		echo "Попробуйте установить: sudo apt install libopencv-dev"; \
		exit 1; \
	fi
	@echo "$(COLOR_GREEN)✓ OpenCV найден$(COLOR_RESET)"

# Сборка исполняемого файла
$(TARGET): $(OBJECTS)
	@echo "$(COLOR_YELLOW)Линковка...$(COLOR_RESET)"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJECTS) -o $(BUILD_DIR)/$(TARGET) $(LDFLAGS)
	@ln -sf $(BUILD_DIR)/$(TARGET) $(TARGET)

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "$(COLOR_YELLOW)Компиляция $<...$(COLOR_RESET)"
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Очистка
clean:
	@echo "$(COLOR_YELLOW)Очистка...$(COLOR_RESET)"
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
	@echo "$(COLOR_GREEN)✓ Очистка завершена$(COLOR_RESET)"

# Установка (опционально)
install: $(TARGET)
	@echo "$(COLOR_YELLOW)Установка...$(COLOR_RESET)"
	@mkdir -p $(DESTDIR)/usr/local/bin
	cp $(BUILD_DIR)/$(TARGET) $(DESTDIR)/usr/local/bin/$(TARGET)
	@echo "$(COLOR_GREEN)✓ Установлено в $(DESTDIR)/usr/local/bin/$(TARGET)$(COLOR_RESET)"

# Удаление установленного файла
uninstall:
	@echo "$(COLOR_YELLOW)Удаление...$(COLOR_RESET)"
	rm -f $(DESTDIR)/usr/local/bin/$(TARGET)
	@echo "$(COLOR_GREEN)✓ Удалено$(COLOR_RESET)"

# Справка
help:
	@echo "Использование: make [цель] [опции]"
	@echo ""
	@echo "Цели:"
	@echo "  all          - Сборка проекта (по умолчанию)"
	@echo "  clean        - Удаление собранных файлов"
	@echo "  install      - Установка в /usr/local/bin"
	@echo "  uninstall    - Удаление установленного файла"
	@echo "  help         - Показать эту справку"
	@echo ""
	@echo "Опции:"
	@echo "  BUILD_TYPE=Debug   - Сборка в режиме отладки (по умолчанию: Release)"
	@echo "  NO_COLOR=1         - Отключить цветной вывод"
	@echo ""
	@echo "Примеры:"
	@echo "  make                    # Сборка в Release режиме"
	@echo "  make BUILD_TYPE=Debug   # Сборка в Debug режиме"
	@echo "  make clean              # Очистка"
	@echo "  make install            # Установка"

# Информация о сборке
info:
	@echo "Информация о сборке:"
	@echo "  Компилятор: $(CXX)"
	@echo "  Режим: $(BUILD_TYPE)"
	@echo "  Флаги: $(CXXFLAGS)"
	@echo "  OpenCV флаги: $(OPENCV_FLAGS)"
	@echo "  OpenCV библиотеки: $(OPENCV_LIBS)"
	@echo "  Исходные файлы: $(SOURCES)"
	@echo "  Объектные файлы: $(OBJECTS)"

cmake_minimum_required(VERSION 3.10)
project(video2ASCII VERSION 1.0.0 LANGUAGES CXX)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Поиск OpenCV
find_package(OpenCV REQUIRED)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/ascii_cache.cpp
    src/video_downloader.cpp
    src/video_processor.cpp
    src/utils.cpp
    src/cli.cpp
    src/video_path_resolver.cpp
    src/logger.cpp
    src/signal_handler.cpp
    src/video_validator.cpp
    src/video_cache.cpp
    src/processing_stats.cpp
    src/interactive_cli.cpp
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES})

# Включаемые директории
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# Линковка библиотек
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OpenCV_LIBS}
    pthread
)

# Вывод информации о найденных библиотеках
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")

# Оптимизация для Release
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Предупреждения
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

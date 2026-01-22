# video2ASCII

Конвертер видео в ASCII-арт с поддержкой загрузки видео по ссылке.

![image](https://github.com/user-attachments/assets/a40991e3-d355-480f-a466-42f52007c85f)

## Возможности

- Конвертация видео в ASCII-арт в реальном времени
- Поддержка загрузки видео по HTTP/HTTPS ссылке
- Многопоточная обработка (обработка и воспроизведение в отдельных потоках)
- Настраиваемая ширина ASCII-изображения
- Кроссплатформенность (Windows/Linux)
- Обработка ошибок и валидация входных данных
- RAII и ООП архитектура

## Требования

- OpenCV (https://opencv.org/releases/)
- C++17 или выше
- curl (для загрузки видео по ссылке)
  - Windows: включен в Windows 10/11 или скачать с https://curl.se/windows/
  - Linux: sudo apt-get install curl (Ubuntu/Debian) или sudo yum install curl (CentOS/RHEL)

## Сборка

### Способ 1: Makefile (рекомендуется)

```bash
# Простая сборка
make

# Сборка в режиме отладки
make BUILD_TYPE=Debug

# Очистка
make clean

# Справка
make help
```

### Способ 2: CMake

```bash
# Создание директории сборки
mkdir build && cd build

# Конфигурация
cmake ..

# Сборка
cmake --build .

# Или одной командой
cmake -B build && cmake --build build
```

### Способ 3: Ручная компиляция

```bash
# Linux
g++ -std=c++17 src/*.cpp -Iinclude -o video2ASCII \
    $(pkg-config --cflags --libs opencv4) -pthread

# Windows (MinGW)
g++ -std=c++17 src/*.cpp -Iinclude -o video2ASCII.exe \
    -lopencv_world -pthread
```

## Использование

### Базовое использование

```bash
# Локальный файл
./video2ASCII video.mp4

# Видео по ссылке
./video2ASCII https://example.com/video.mp4

# С настройкой ширины
./video2ASCII -w 300 video.mp4
```

### Опции командной строки

- -w, --width <число> - Ширина ASCII-изображения (по умолчанию: 250)
- -h, --help - Показать справку

### Примеры

```bash
# Воспроизведение локального видео
./video2ASCII my_video.mp4

# Загрузка и воспроизведение видео по ссылке
./video2ASCII https://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4

# Увеличенная ширина для лучшего качества
./video2ASCII -w 400 video.mp4
```

## Архитектура

Проект использует принципы:
- RAII - автоматическое управление ресурсами через деструкторы
- ООП - классы VideoProcessor, AsciiCache, VideoDownloader
- DRY - переиспользование кода через классы
- KISS - простота реализации
- SOLID - разделение ответственности между классами

### Основные классы

- VideoProcessor - основной класс для обработки видео (RAII)
- AsciiCache - кеширование преобразования пикселей в ASCII
- VideoDownloader - загрузка видео по URL

## Особенности реализации

- Многопоточность: обработка кадров и их воспроизведение выполняются параллельно
- Кеширование: предварительный расчет индексов ASCII символов
- Оптимизация памяти: резервирование памяти для строк ASCII-кадров
- Автоматическая очистка: временные файлы удаляются после использования

## Примечания

- Высота ASCII-изображения рассчитывается автоматически с сохранением пропорций
- Коэффициент высоты: 0.5 (для лучшей читаемости в консоли)
- Для корректной работы требуется терминал с поддержкой ANSI escape-последовательностей

#include <opencv2/opencv.hpp>

#include <chrono>
#include <iostream>
#include <thread>

#include <mutex>
#include <condition_variable>

using namespace std;
using namespace cv;

const string ASCII_CHARS = "    ._-^*+:=?&$@#%"; // Набор символов для отрисовки картинки
//const string ASCII_CHARS = "%#@$=+*-_. ";
const int ASCII_CHARS_LENGTH = ASCII_CHARS.length();
const int MAX_PIXEL_INTENSITY = 256;

// Кеширование таблицы ASCII символов
int asciiIndexCache[MAX_PIXEL_INTENSITY];

void initAsciiIndexCache() {
    for (int i = 0; i < MAX_PIXEL_INTENSITY; i++) {
        asciiIndexCache[i] = (int)((i * ASCII_CHARS_LENGTH) / MAX_PIXEL_INTENSITY);
    }
}

// Преобразование пикселя в ASCII-символ
char pixelToASCII(int pixel_intensity) {
    return ASCII_CHARS[asciiIndexCache[pixel_intensity]];
}

int main() {
    initAsciiIndexCache(); // Инициализация таблицы символов

    string video_path = "/Project/video2ASCII/exp1.mp4"; // Выберите видео и укажите путь к нему тут
    VideoCapture cap(video_path);

    double fps = cap.get(CAP_PROP_FPS);

    int frame_duration_ms = 1000 / fps; // Частота смены кадра

    int width = 250;
    int height;

    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);

    height = (width * frame_height / frame_width) * 0.5;

    Mat frame, gray_frame, resized_frame;

    string ascii_frame;

    mutex mtx;
    condition_variable cv;
    bool frame_ready = false;

    // Поток с вычислением нужного символа ASCII относительно глубины цвета пикселя
    thread thread1([&](){
        while (true) {
            cap >> frame;
            if (frame.empty())
                break;

            cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

            resize(gray_frame, resized_frame, Size(width, height), 0, 0, INTER_LINEAR);

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    ascii_frame += pixelToASCII(resized_frame.at<uchar>(i, j));
                }
                ascii_frame += "\n";
            }
            {
                lock_guard<mutex> lock(mtx);
                frame_ready = true;
            }
            cv.notify_one();
        }
    });

    // Поток с выводом символьной "картинки"
    thread thread2([&]() {
        while (true) {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock);

            system("cls");
            cout << ascii_frame;
            ascii_frame.clear();
            this_thread::sleep_for(chrono::milliseconds(frame_duration_ms));

            frame_ready = false;
        }
    });

    thread1.join();
    thread2.join();

    return 0;
}
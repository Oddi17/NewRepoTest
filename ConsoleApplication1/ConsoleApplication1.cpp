#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <thread>
#include <string>

using namespace std;

void invertColors(vector<unsigned char>& imageData, int startIndex, int endIndex) {
    for (int i = startIndex; i < endIndex; ++i) {
        imageData[i] = 255 - imageData[i];
    }
}

int main() {

    const char* inputFileName = "D:/testing/test_task/in.bmp";
    const char* outputFileName = "D:/testing/test_task/out.bmp";

    ifstream inputFile(inputFileName, ios::binary);
    //ifstream inputFile(inputFileName);

    if (!inputFile) {
        cerr << "Could not open input file." << endl;
        system("pause");
        return 1;
    }

    char header[54];
    inputFile.read(header, 54);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageDataSize = width * height * 3;

    if (width != 512 || height != 512) {
        cout << "Wrong format, you need 512x512" << endl;
        system("pause");
        return 1;
    }
    string userAnswer;
    cout << "Use the threads (yes or no)?" << endl;
    cin >> userAnswer;

    vector<unsigned char> imageData(imageDataSize);
    inputFile.read(reinterpret_cast<char*>(imageData.data()), imageDataSize);

    inputFile.close();

    auto startTime = chrono::high_resolution_clock::now();

    if (userAnswer == "yes") {
        int numThreads = thread::hardware_concurrency();
        vector<thread> threads;
        int chunkSize = imageDataSize / numThreads;

        // Создаем потоки для обработки частей данных
        for (int i = 0; i < numThreads; ++i) {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? imageDataSize : (i + 1) * chunkSize;
            threads.emplace_back(invertColors, ref(imageData), start, end);
        }

        // Ожидаем завершения всех потоков
        for (auto& thread : threads) {
            thread.join();
        }
    }
    else if (userAnswer == "no") {
        // Инвертирование цветов
        for (auto& pixel : imageData) {
            pixel = 255 - pixel;
        }
    }
    else {
        cout << "Wrong command, only yes or no" << endl;
        system("pause");
        return 1;
    }

    auto endTime = chrono::high_resolution_clock::now(); // Запоминаем время окончания обработки
    auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime); // Вычисляем длительность

    ofstream outputFile(outputFileName, ios::binary);
    if (!outputFile) {
        cerr << "Could not create or open output file." << endl;
        return 1;
    }

    outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));
    outputFile.write(reinterpret_cast<const char*>(imageData.data()), imageDataSize);
    outputFile.close();

    cout << "Image inverted successfully." << endl;
    cout << "Time invert: " << duration.count() * pow(10, -6) << "s" << endl;

    system("pause");
    return 0;
}
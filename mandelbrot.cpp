#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <tuple>

const int WIDTH = 30720;
const int HEIGHT = 17280;
const int MAX_ITER = 800;

std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> colorTable(MAX_ITER + 1);

void initColorTable() {
    for (int iter = 0; iter <= MAX_ITER; ++iter) {
        double t = static_cast<double>(iter) / MAX_ITER;
        uint8_t r = static_cast<uint8_t>(9 * (1 - t) * t * t * t * 255);
        uint8_t g = static_cast<uint8_t>(15 * (1 - t) * (1 - t) * t * t * 255);
        uint8_t b = static_cast<uint8_t>(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
        colorTable[iter] = {r, g, b};
    }
}

int calculateMandelbrot(float cx, float cy, int max_iter) {
    float zx = 0.0f, zy = 0.0f;
    int iter = 0;

    float q = (cx - 0.25f) * (cx - 0.25f) + cy * cy;
    if (q * (q + (cx - 0.25f)) <= 0.25f * cy * cy || (cx + 1.0f) * (cx + 1.0f) + cy * cy <= 0.0625f) {
        return max_iter;
    }

    while (iter < max_iter && (zx * zx + zy * zy) <= 4.0f) {
        float tmp = zx * zx - zy * zy + cx;
        zy = 2.0f * zx * zy + cy;
        zx = tmp;
        iter++;
    }
    return iter;
}

void generateMandelbrot(std::vector<uint8_t>& image) {
    float scaleX = 3.5f / WIDTH;
    float scaleY = 2.0f / HEIGHT;

    #pragma omp parallel for collapse(2) schedule(static)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            float cx = x * scaleX - 2.5f;
            float cy = y * scaleY - 1.0f;
            int iter = calculateMandelbrot(cx, cy, MAX_ITER);

            auto [r, g, b] = colorTable[iter];
            int idx = (y * WIDTH + x) * 3;
            image[idx] = r;
            image[idx + 1] = g;
            image[idx + 2] = b;
        }
    }
}

int main() {
    initColorTable(); // Инициализация таблицы цветов
    std::cout << "Generating 32k mandelbrot image..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<uint8_t> image(WIDTH * HEIGHT * 3);
    generateMandelbrot(image);

    auto render_end = std::chrono::high_resolution_clock::now();
    std::cout << "Render time: " << std::chrono::duration<double>(render_end - start).count() << " seconds\n";

    stbi_write_jpg("mandelbrot_optimized.jpg", WIDTH, HEIGHT, 3, image.data(), WIDTH * 3);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Full time: " << std::chrono::duration<double>(end - start).count() << " seconds\n";

    return 0;
}
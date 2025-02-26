#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <chrono>

const int WIDTH = 30720;
const int HEIGHT = 17280;
const int MAX_ITER = 800;

int calculateMandelbrot(double cx, double cy, int max_iter) {
    double zx = 0.0, zy = 0.0, zx2 = 0.0, zy2 = 0.0;
    int iter = 0;

    double q = (cx - 0.25) * (cx - 0.25) + cy * cy;
    if (q * (q + (cx - 0.25)) <= 0.25 * cy * cy || (cx + 1) * (cx + 1) + cy * cy <= 0.0625) {
        return max_iter;
    }

    while (iter < max_iter && (zx2 + zy2) <= 4.0) {
        zy = 2.0 * zx * zy + cy;
        zx = zx2 - zy2 + cx;
        zx2 = zx * zx;
        zy2 = zy * zy;
        iter++;
    }
    return iter;
}

void getColor(int iter, int max_iter, uint8_t& r, uint8_t& g, uint8_t& b) {
    if (iter == max_iter) {
        r = g = b = 0;
    } else {
        double t = (double)iter / max_iter;
        r = static_cast<uint8_t>(9 * (1 - t) * t * t * t * 255);
        g = static_cast<uint8_t>(15 * (1 - t) * (1 - t) * t * t * 255);
        b = static_cast<uint8_t>(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
    }
}

void generateMandelbrot(std::vector<uint8_t>& image) {
    double scaleX = 3.5 / WIDTH;
    double scaleY = 2.0 / HEIGHT;

    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double cx = x * scaleX - 2.5;
            double cy = y * scaleY - 1.0;
            int iter = calculateMandelbrot(cx, cy, MAX_ITER);
            uint8_t r, g, b;
            getColor(iter, MAX_ITER, r, g, b);
            int idx = (y * WIDTH + x) * 3;
            image[idx] = r;
            image[idx + 1] = g;
            image[idx + 2] = b;
        }
    }
}

int main() {
    std::cout << "Generating 32k mandelbrot image..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<uint8_t> image(WIDTH * HEIGHT * 3);
    generateMandelbrot(image);
    auto render_end = std::chrono::high_resolution_clock::now();
    std::cout << "Render time: " << std::chrono::duration<double>(render_end - start).count() << " seconds\n";

    stbi_write_jpg("mandelbrot.jpg", WIDTH, HEIGHT, 3, image.data(), WIDTH * 3);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Full time: " << std::chrono::duration<double>(end - start).count() << " seconds\n";
    return 0;
}

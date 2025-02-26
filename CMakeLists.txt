cmake_minimum_required(VERSION 3.30)
project(mandelbrot)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 26)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math")

# Добавляем исходный файл
add_executable(mandelbrot mandelbrot.cpp)

find_package(OpenMP REQUIRED)

# Проверка на наличие OpenMP
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found. Adding OpenMP support.")

    # Добавление флагов компиляции и линковки для OpenMP
    target_compile_options(mandelbrot PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(mandelbrot PRIVATE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "OpenMP not found. Please install OpenMP.")
endif()

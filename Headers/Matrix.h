#pragma once

#include "pico/stdlib.h"
#include "string"

class Matrix {
    public:
        Matrix(uint8_t width, uint8_t height);
        ~Matrix();

        std::string toString();

        double* operator [](uint8_t index);
        Matrix operator *(const Matrix& m1);
        void operator =(const Matrix& m1);

        static double destructCount;

        uint8_t width, height;
    private:
        double **matrixValues;
};
#pragma once

#include "pico/stdlib.h"
#include "string"
#include "Vector.h"

class Matrix {
    public:
        Matrix(uint8_t width, uint8_t height);
        Matrix(const Vector3 v1);
        Matrix(const Matrix& m1);
        ~Matrix();

        std::string toString();

        double* operator [](uint8_t index) const;
        Matrix operator *(const Matrix& m1) const;
        void operator =(const Matrix& m1);
        void operator =(const Vector3& v1);

        uint8_t width, height;
    private:
        double **matrixValues;
};
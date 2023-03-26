#pragma once

#include "pico/stdlib.h"
#include "string"
#include "math.h"

class Matrix;

class Vector2 {
public:
    Vector2(double x, double y);
    Vector2();

    static const Vector2 zero;

    bool operator ==(const Vector2&) const;
    bool operator !=(const Vector2&) const;

    void operator =(const Vector2&);
    Vector2 operator *(const Vector2&) const;
    Vector2 operator /(const Vector2&) const;
    Vector2 operator +(const Vector2&) const;
    Vector2 operator -(const Vector2&) const;
    Vector2& operator *=(const Vector2&);
    Vector2& operator /=(const Vector2&);
    Vector2& operator +=(const Vector2&);
    Vector2& operator -=(const Vector2&);

    void operator =(const double&);
    Vector2 operator *(const double&) const;
    Vector2 operator /(const double&) const;
    Vector2 operator +(const double&) const;
    Vector2 operator -(const double&) const;
    Vector2& operator *=(const double&);
    Vector2& operator /=(const double&);
    Vector2& operator +=(const double&);
    Vector2& operator -=(const double&);

    std::string toString(bool rounded = false) const;
    void round();
    double magnitude() const;

    double X, Y;
};


class Vector3 {
public:
    Vector3(double x, double y, double z);
    Vector3(const Matrix);
    Vector3();

    static const Vector3 zero;

    bool operator ==(const Vector3&) const;
    bool operator !=(const Vector3&) const;

    void operator =(const Vector3&);
    Vector3 operator *(const Vector3&) const;
    Vector3 operator /(const Vector3&) const;
    Vector3 operator +(const Vector3&) const;
    Vector3 operator -(const Vector3&) const;
    Vector3& operator *=(const Vector3&);
    Vector3& operator /=(const Vector3&);
    Vector3& operator +=(const Vector3&);
    Vector3& operator -=(const Vector3&);

    void operator =(const double&);
    Vector3 operator *(const double&) const;
    Vector3 operator /(const double&) const;
    Vector3 operator +(const double&) const;
    Vector3 operator -(const double&) const;
    Vector3& operator *=(const double&);
    Vector3& operator /=(const double&);
    Vector3& operator +=(const double&);
    Vector3& operator -=(const double&);

    void operator =(const Matrix&);

    std::string toString(bool rounded = false) const;
    void round();
    double magnitude() const;

    double X, Y, Z;
};
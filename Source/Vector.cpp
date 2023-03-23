#include "Vector.h"
#include "Matrix.h" // include Matrix.h here, not in the header file, to prevent cyclical inclusions
                    // I declared the Matrix calss in Vector.h, and define it here

Vector2::Vector2(double x, double y) {
    X = x;
    Y = y;
}
Vector2::Vector2() {
    X = 0;
    Y = 0;
}

const Vector2 Vector2::zero = Vector2(0, 0);

bool Vector2::operator ==(const Vector2& v1) const {
    return (X == v1.X && Y == v1.Y);
}

bool Vector2::operator !=(const Vector2& v1) const {
    return (X != v1.X || Y != v1.Y);
}

void Vector2::operator =(const Vector2& v1) {
    X = v1.X;
    Y = v1.Y;
}
Vector2 Vector2::operator +(const Vector2& v1) const {
    return Vector2(X + v1.X, Y + v1.Y);
}
Vector2 Vector2::operator -(const Vector2& v1) const {
    return Vector2(X - v1.X, Y - v1.Y);
}
Vector2 Vector2::operator *(const Vector2& v1) const {
    return Vector2(X * v1.X, Y * v1.Y);
}
Vector2 Vector2::operator /(const Vector2& v1) const {
    return Vector2(X / v1.X, Y / v1.Y);
}
Vector2& Vector2::operator +=(const Vector2& v1) {
    X += v1.X;
    Y += v1.Y;

    return *this;
}
Vector2& Vector2::operator -=(const Vector2& v1) {
    X -= v1.X;
    Y -= v1.Y;

    return *this;
}
Vector2& Vector2::operator *=(const Vector2& v1) {
    X *= v1.X;
    Y *= v1.Y;

    return *this;
}
Vector2& Vector2::operator /=(const Vector2& v1) {
    X /= v1.X;
    Y /= v1.Y;

    return *this;
}

void Vector2::operator =(const double& v1) {
    X = v1;
    Y = v1;
}
Vector2 Vector2::operator +(const double& v1) const {
    return Vector2(X + v1, Y + v1);
}
Vector2 Vector2::operator -(const double& v1) const {
    return Vector2(X - v1, Y - v1);
}
Vector2 Vector2::operator *(const double& v1) const {
    return Vector2(X * v1, Y * v1);
}
Vector2 Vector2::operator /(const double& v1) const {
    return Vector2(X / v1, Y / v1);
}
Vector2& Vector2::operator +=(const double& v1) {
    X += v1;
    Y += v1;

    return *this;
}
Vector2& Vector2::operator -=(const double& v1) {
    X -= v1;
    Y -= v1;

    return *this;
}
Vector2& Vector2::operator *=(const double& v1) {
    X *= v1;
    Y *= v1;

    return *this;
}
Vector2& Vector2::operator /=(const double& v1) {
    X /= v1;
    Y /= v1;

    return *this;
}

std::string Vector2::toString() const {
    return std::to_string(X).append("x").append(std::to_string(Y));
}

void Vector2::round() {
    X = ::round(X);
    Y = ::round(Y);
}


Vector3::Vector3(double x, double y, double z) {
    X = x;
    Y = y;
    Z = z;
}
Vector3::Vector3(const Matrix m1) {
    if (m1.width == 1 && m1.height == 3) {
        X = m1[0][0];
        Y = m1[0][1];
        Z = m1[0][2];
    } else {// should error here
        X = 0;
        Y = 0;
        Z = 0;
    }
}
Vector3::Vector3() {
    X = 0;
    Y = 0;
    Z = 0;
}

const Vector3 Vector3::zero = Vector3(0, 0, 0);

bool Vector3::operator ==(const Vector3& v1) const {
    return (X == v1.X && Y == v1.Y && Z == v1.Z);
}

bool Vector3::operator !=(const Vector3& v1) const {
    return (X != v1.X || Y != v1.Y || Z != v1.Z);
}

void Vector3::operator =(const Vector3& v1) {
    X = v1.X;
    Y = v1.Y;
    Z = v1.Z;
}
Vector3 Vector3::operator +(const Vector3& v1) const {
    return Vector3(X + v1.X, Y + v1.Y, Z + v1.Z);
}
Vector3 Vector3::operator -(const Vector3& v1) const {
    return Vector3(X - v1.X, Y - v1.Y, Z - v1.Z);
}
Vector3 Vector3::operator *(const Vector3& v1) const {
    return Vector3(X * v1.X, Y * v1.Y, Z * v1.Z);
}
Vector3 Vector3::operator /(const Vector3& v1) const {
    return Vector3(X / v1.X, Y / v1.Y, Z / v1.Z);
}
Vector3& Vector3::operator +=(const Vector3& v1) {
    X += v1.X;
    Y += v1.Y;
    Z += v1.Z;

    return *this;
}
Vector3& Vector3::operator -=(const Vector3& v1) {
    X -= v1.X;
    Y -= v1.Y;
    Z -= v1.Z;

    return *this;
}
Vector3& Vector3::operator *=(const Vector3& v1) {
    X *= v1.X;
    Y *= v1.Y;
    Z *= v1.Z;

    return *this;
}
Vector3& Vector3::operator /=(const Vector3& v1) {
    X /= v1.X;
    Y /= v1.Y;
    Z /= v1.Z;

    return *this;
}

void Vector3::operator =(const double& v1) {
    X = v1;
    Y = v1;
    Z = v1;
}
Vector3 Vector3::operator +(const double& v1) const {
    return Vector3(X + v1, Y + v1, Z + v1);
}
Vector3 Vector3::operator -(const double& v1) const {
    return Vector3(X - v1, Y - v1, Z - v1);
}
Vector3 Vector3::operator *(const double& v1) const {
    return Vector3(X * v1, Y * v1, Z * v1);
}
Vector3 Vector3::operator /(const double& v1) const {
    return Vector3(X / v1, Y / v1, Z / v1);
}
Vector3& Vector3::operator +=(const double& v1) {
    X += v1;
    Y += v1;
    Z += v1;

    return *this;
}
Vector3& Vector3::operator -=(const double& v1) {
    X -= v1;
    Y -= v1;
    Z -= v1;

    return *this;
}
Vector3& Vector3::operator *=(const double& v1) {
    X *= v1;
    Y *= v1;
    Z *= v1;

    return *this;
}
Vector3& Vector3::operator /=(const double& v1) {
    X /= v1;
    Y /= v1;
    Z /= v1;

    return *this;
}

void Vector3::operator =(const Matrix& m1) {
    if (m1.width == 1 && m1.height == 3) {
        X = m1[0][0];
        Y = m1[0][1];
        Z = m1[0][2];
    }
}

std::string Vector3::toString() const {
    return std::to_string(X).append("x").append(std::to_string(Y)).append("x").append(std::to_string(Z));
}

void Vector3::round() {
    X = ::round(X);
    Y = ::round(Y);
    Z = ::round(Z);
}

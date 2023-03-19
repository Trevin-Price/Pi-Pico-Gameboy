#include "Vector.h"

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



Vector3::Vector3(double x, double y, double z) {
    X = x;
    Y = y;
    Z = z;
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
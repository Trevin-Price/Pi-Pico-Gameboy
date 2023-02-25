#include "Vector.h"

Vector2::Vector2(double x, double y) {
    Vector2::x = x;
    Vector2::y = y;
}
Vector2::Vector2() {
    Vector2::x = 0;
    Vector2::y = 0;
}

const Vector2 Vector2::zero = Vector2(0, 0);

bool Vector2::operator ==(const Vector2& v1) const {
    return (this->x == v1.x && this->y == v1.y);
}

void Vector2::operator =(const Vector2& v1) {
    this->x = v1.x;
    this->y = v1.y;
}
Vector2 Vector2::operator +(const Vector2& v1) const {
    return Vector2(this->x + v1.x, this->y + v1.y);
}
Vector2 Vector2::operator -(const Vector2& v1) const {
    return Vector2(this->x - v1.x, this->y - v1.y);
}
Vector2 Vector2::operator *(const Vector2& v1) const {
    return Vector2(this->x * v1.x, this->y * v1.y);
}
Vector2 Vector2::operator /(const Vector2& v1) const {
    return Vector2(this->x / v1.x, this->y / v1.y);
}
Vector2& Vector2::operator +=(const Vector2& v1) {
    this->x += v1.x;
    this->y += v1.y;

    return *this;
}
Vector2& Vector2::operator -=(const Vector2& v1) {
    this->x -= v1.x;
    this->y -= v1.y;

    return *this;
}
Vector2& Vector2::operator *=(const Vector2& v1) {
    this->x *= v1.x;
    this->y *= v1.y;

    return *this;
}
Vector2& Vector2::operator /=(const Vector2& v1) {
    this->x /= v1.x;
    this->y /= v1.y;

    return *this;
}

void Vector2::operator =(const double& v1) {
    this->x = v1;
    this->y = v1;
}
Vector2 Vector2::operator +(const double& v1) const {
    return Vector2(this->x + v1, this->y + v1);
}
Vector2 Vector2::operator -(const double& v1) const {
    return Vector2(this->x - v1, this->y - v1);
}
Vector2 Vector2::operator *(const double& v1) const {
    return Vector2(this->x * v1, this->y * v1);
}
Vector2 Vector2::operator /(const double& v1) const {
    return Vector2(this->x / v1, this->y / v1);
}
Vector2& Vector2::operator +=(const double& v1) {
    this->x += v1;
    this->y += v1;

    return *this;
}
Vector2& Vector2::operator -=(const double& v1) {
    this->x -= v1;
    this->y -= v1;

    return *this;
}
Vector2& Vector2::operator *=(const double& v1) {
    this->x *= v1;
    this->y *= v1;

    return *this;
}
Vector2& Vector2::operator /=(const double& v1) {
    this->x /= v1;
    this->y /= v1;

    return *this;
}



Vector3::Vector3(double x, double y, double z) {
    Vector3::x = x;
    Vector3::y = y;
    Vector3::z = z;
}
Vector3::Vector3() {
    Vector3::x = 0;
    Vector3::y = 0;
    Vector3::z = 0;
}

const Vector3 Vector3::zero = Vector3(0, 0, 0);

bool Vector3::operator ==(const Vector3& v1) const {
    return (this->x == v1.x && this->y == v1.y && this->z == v1.z);
}

void Vector3::operator =(const Vector3& v1) {
    this->x = v1.x;
    this->y = v1.y;
    this->z = v1.z;
}
Vector3 Vector3::operator +(const Vector3& v1) const {
    return Vector3(this->x + v1.x, this->y + v1.y, this->z + v1.z);
}
Vector3 Vector3::operator -(const Vector3& v1) const {
    return Vector3(this->x - v1.x, this->y - v1.y, this->z - v1.z);
}
Vector3 Vector3::operator *(const Vector3& v1) const {
    return Vector3(this->x * v1.x, this->y * v1.y, this->z * v1.z);
}
Vector3 Vector3::operator /(const Vector3& v1) const {
    return Vector3(this->x / v1.x, this->y / v1.y, this->z / v1.z);
}
Vector3& Vector3::operator +=(const Vector3& v1) {
    this->x += v1.x;
    this->y += v1.y;
    this->z += v1.z;

    return *this;
}
Vector3& Vector3::operator -=(const Vector3& v1) {
    this->x -= v1.x;
    this->y -= v1.y;
    this->z -= v1.z;

    return *this;
}
Vector3& Vector3::operator *=(const Vector3& v1) {
    this->x *= v1.x;
    this->y *= v1.y;
    this->z *= v1.z;

    return *this;
}
Vector3& Vector3::operator /=(const Vector3& v1) {
    this->x /= v1.x;
    this->y /= v1.y;
    this->z /= v1.z;

    return *this;
}

void Vector3::operator =(const double& v1) {
    this->x = v1;
    this->y = v1;
    this->z = v1;
}
Vector3 Vector3::operator +(const double& v1) const {
    return Vector3(this->x + v1, this->y + v1, this->z + v1);
}
Vector3 Vector3::operator -(const double& v1) const {
    return Vector3(this->x - v1, this->y - v1, this->z - v1);
}
Vector3 Vector3::operator *(const double& v1) const {
    return Vector3(this->x * v1, this->y * v1, this->z * v1);
}
Vector3 Vector3::operator /(const double& v1) const {
    return Vector3(this->x / v1, this->y / v1, this->z / v1);
}
Vector3& Vector3::operator +=(const double& v1) {
    this->x += v1;
    this->y += v1;
    this->z += v1;

    return *this;
}
Vector3& Vector3::operator -=(const double& v1) {
    this->x -= v1;
    this->y -= v1;
    this->z -= v1;

    return *this;
}
Vector3& Vector3::operator *=(const double& v1) {
    this->x *= v1;
    this->y *= v1;
    this->z *= v1;

    return *this;
}
Vector3& Vector3::operator /=(const double& v1) {
    this->x /= v1;
    this->y /= v1;
    this->z /= v1;

    return *this;
}
#include "Vector3D.h"

#include <QtGlobal>
#include <math.h>

Vector3D::Vector3D()
{
    this->x = this->y = this->z = 0;
}

Vector3D::Vector3D(const float &x, const float &y, const float &z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

float Vector3D::dotProduct(const Vector3D &v1, const Vector3D &v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

Vector3D Vector3D::crossProduct(const Vector3D &v1, const Vector3D &v2)
{
    Vector3D result;

    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;

    return result;
}

Vector3D& Vector3D::operator=(const Vector3D &other)
{
    if(this != &other)
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }

    return *this;
}

Vector3D& Vector3D::operator=(const float &other)
{
    this->x = other;
    this->y = other;
    this->z = other;

    return *this;
}

Vector3D& Vector3D::operator+=(const Vector3D &other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

    return *this;
}

Vector3D& Vector3D::operator+=(const float &other)
{
    this->x += other;
    this->y += other;
    this->z += other;

    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D &other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;

    return *this;
}

Vector3D& Vector3D::operator-=(const float &other)
{
    this->x -= other;
    this->y -= other;
    this->z -= other;

    return *this;
}

Vector3D& Vector3D::operator*=(const float &other)
{
    this->x *= other;
    this->y *= other;
    this->z *= other;

    return *this;
}

Vector3D& Vector3D::operator/=(const float &other)
{
    this->x /= other;
    this->y /= other;
    this->z /= other;

    return *this;
}

const Vector3D Vector3D::operator*(const float &other) const
{
    Vector3D result = *this;

    result.x *= other;
    result.y *= other;
    result.z *= other;

    return result;
}

const Vector3D Vector3D::operator/(const float &other) const
{
    Vector3D result = *this;

    result.x /= other;
    result.y /= other;
    result.z /= other;

    return result;
}

const Vector3D Vector3D::operator+(const Vector3D &other) const
{
    Vector3D result = *this;

    result.x += other.x;
    result.y += other.y;
    result.z += other.z;

    return result;
}

const Vector3D Vector3D::operator+(const float &other) const
{
    Vector3D result = *this;

    result.x += other;
    result.y += other;
    result.z += other;

    return result;
}

const Vector3D Vector3D::operator-(const Vector3D &other) const
{
    Vector3D result = *this;

    result.x -= other.x;
    result.y -= other.y;
    result.z -= other.z;

    return result;
}

const Vector3D Vector3D::operator-(const float &other) const
{
    Vector3D result = *this;

    result.x -= other;
    result.y -= other;
    result.z -= other;

    return result;
}

bool Vector3D::operator==(const Vector3D &other) const
{
    bool value;

    value = qFuzzyCompare(this->x + 1, other.x + 1) & qFuzzyCompare(this->y + 1, other.y + 1) & qFuzzyCompare(this->z + 1, other.z + 1);

    return value;
}

float Vector3D::getModuleSquared() const
{
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

float Vector3D::getModule() const
{
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

Vector3D Vector3D::getUnitary() const
{
    float module;
    Vector3D unitary;

    module = this->getModule();

    unitary.x = this->x / module;
    unitary.y = this->y / module;
    unitary.z = this->z / module;

    return unitary;
}

const Vector3D operator*(const float &num, const Vector3D &other)
{
    Vector3D result;

    result.setX(other.getX() * num);
    result.setY(other.getY() * num);
    result.setZ(other.getZ() * num);

    return result;
}

const Vector3D operator+(const float &num, const Vector3D &other)
{
    Vector3D result;

    result.setX(other.getX() + num);
    result.setY(other.getY() + num);
    result.setZ(other.getZ() + num);

    return result;
}

const Vector3D operator-(const float &num, const Vector3D &other)
{
    Vector3D result;

    result.setX(num - other.getX());
    result.setY(num - other.getY());
    result.setZ(num - other.getZ());

    return result;
}

const Vector3D operator-(const Vector3D &other)
{
    Vector3D result;

    result.setX(-other.getX());
    result.setY(-other.getY());
    result.setZ(-other.getZ());

    return result;
}

#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{
    protected:
        float x, y, z;

    public:
        Vector3D();
        Vector3D(const float &x, const float &y, const float &z);

        virtual void setX(const float &x) { this->x = x; }
        virtual void setY(const float &y) { this->y = y; }
        virtual void setZ(const float &z) { this->z = z; }

        const float &getX() const { return this->x; }
        const float &getY() const { return this->y; }
        const float &getZ() const { return this->z; }

        static float   dotProduct(const Vector3D &v1, const Vector3D &v2);
        static Vector3D crossProduct(const Vector3D &v1, const Vector3D &v2);

        virtual Vector3D& operator=(const Vector3D &other);
        virtual Vector3D& operator=(const float &other);

        virtual Vector3D& operator+=(const Vector3D &other);
        virtual Vector3D& operator+=(const float &other);

        virtual Vector3D& operator-=(const Vector3D &other);
        virtual Vector3D& operator-=(const float &other);

        virtual Vector3D& operator*=(const float &other);
        virtual Vector3D& operator/=(const float &other);

        virtual const Vector3D operator*(const float &other) const;
        virtual const Vector3D operator/(const float &other) const;

        virtual const Vector3D operator+(const Vector3D &other) const;
        virtual const Vector3D operator+(const float &other)   const;

        virtual const Vector3D operator-(const Vector3D &other) const;
        virtual const Vector3D operator-(const float &other)   const;

        virtual bool operator==(const Vector3D &other) const;

        virtual float   getModuleSquared() const;
        virtual float   getModule()        const;
        virtual Vector3D getUnitary()       const;
};

const Vector3D operator*(const float& num, const Vector3D& other);
const Vector3D operator+(const float& num, const Vector3D& other);
const Vector3D operator-(const float& num, const Vector3D& other);
const Vector3D operator-(const Vector3D &other);

#endif // VECTOR3D_H

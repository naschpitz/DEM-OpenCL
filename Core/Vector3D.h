#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{
    protected:
        double x, y, z;

    public:
        Vector3D();
        Vector3D(const double &x, const double &y, const double &z);

        virtual void setX(const double &x) { this->x = x; }
        virtual void setY(const double &y) { this->y = y; }
        virtual void setZ(const double &z) { this->z = z; }

        const double &getX() const { return this->x; }
        const double &getY() const { return this->y; }
        const double &getZ() const { return this->z; }

        static double   dotProduct(const Vector3D &v1, const Vector3D &v2);
        static Vector3D crossProduct(const Vector3D &v1, const Vector3D &v2);

        virtual Vector3D& operator=(const Vector3D &other);
        virtual Vector3D& operator=(const double &other);

        virtual Vector3D& operator+=(const Vector3D &other);
        virtual Vector3D& operator+=(const double &other);

        virtual Vector3D& operator-=(const Vector3D &other);
        virtual Vector3D& operator-=(const double &other);

        virtual Vector3D& operator*=(const double &other);
        virtual Vector3D& operator/=(const double &other);

        virtual const Vector3D operator*(const double &other) const;
        virtual const Vector3D operator/(const double &other) const;

        virtual const Vector3D operator+(const Vector3D &other) const;
        virtual const Vector3D operator+(const double &other)   const;

        virtual const Vector3D operator-(const Vector3D &other) const;
        virtual const Vector3D operator-(const double &other)   const;

        virtual bool operator==(const Vector3D &other) const;

        virtual double   getModuleSquared() const;
        virtual double   getModule()        const;
        virtual Vector3D getUnitary()       const;
};

const Vector3D operator*(const double& num, const Vector3D& other);
const Vector3D operator+(const double& num, const Vector3D& other);
const Vector3D operator-(const double& num, const Vector3D& other);
const Vector3D operator-(const Vector3D &other);

#endif // VECTOR3D_H

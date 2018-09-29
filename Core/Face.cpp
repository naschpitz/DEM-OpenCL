#include "Face.h"

Face::Face()
{

}

Face::Face(const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;

    this->calculateArea();
}

Vector3D Face::getNormal() const
{
    Vector3D e1 = v2 - v1;
    Vector3D e2 = v3 - v1;

    return Vector3D::crossProduct(e1, e2);
}

double Face::getArea() const
{
    return this->getNormal().getModule() / 2.0;
}

void Face::calculateArea()
{
    this->area = this->getArea();
}

#include <Material.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <cstring>

Material::Material()
{

}

Material::Material(const QJsonObject& jsonObject)
{
    QString id = jsonObject["_id"].toString();

    QString material1 = jsonObject["material1"].toString();
    QString material2 = jsonObject["material2"].toString();

    double distanceThreshold = jsonObject["distanceThreshold"].toDouble();

    QString forceType     = jsonObject["forceType"].toString();
    QString dragForceType = jsonObject["dragForceType"].toString();

    this->setId(id);
    this->setParents(material1, material2);
    this->setDistanceThreshold(distanceThreshold);
    this->setForceType(forceType);
    this->setDragForceType(dragForceType);

    QJsonValue coefficients = jsonObject["coefficients"];
    foreach(QJsonValue value, coefficients.toArray())
        this->addCoefficient(value.toDouble());

    QJsonValue dragCoefficients = jsonObject["dragCoefficients"];
    foreach(QJsonValue value, dragCoefficients.toArray())
        this->addDragCoefficient(value.toDouble());
}

MaterialCL Material::getCL(const QMap<QString, int>& namesMap) const
{
    MaterialCL materialCL;
    materialCL.materialIndex1 = namesMap.contains(this->material1) ? namesMap[this->material1] : -1;
    materialCL.materialIndex2 = namesMap.contains(this->material2) ? namesMap[this->material2] : -1;
    materialCL.distanceThreshold = this->distanceThreshold;
    materialCL.forceType = this->fromQStringToForceType(this->forceType);
    materialCL.dragForceType = this->fromQStringToDragForceType(this->dragForceType);

    for(int i = 0; i < this->coefficients.count() && i < 10; i++) {
        materialCL.coefficients[i] = this->coefficients[i];
    }

    for(int i = 0; i < this->dragCoefficients.count() && i < 10; i++) {
        materialCL.dragCoefficients[i] = this->dragCoefficients[i];
    }

    return materialCL;
}

const QString& Material::getId() const
{
    return this->id;
}

const QString& Material::getMaterial1() const
{
    return this->material1;
}

const QString& Material::getMaterial2() const
{
    return this->material2;
}

const double& Material::getDistanceThreshold() const
{
    return this->distanceThreshold;
}

const QString& Material::getForceType() const
{
    return this->forceType;
}

const QString& Material::getDragForceType() const
{
    return this->dragForceType;
}

const QVector<double>& Material::getCoefficients() const
{
    return this->coefficients;
}

const QVector<double>& Material::getDragCoefficients() const
{
    return this->dragCoefficients;
}

void Material::setId(const QString& id)
{
    this->id = id;
}

void Material::setParents(const QString& material1, const QString& material2)
{
    this->material1 = material1;
    this->material2 = material2;
}

void Material::setDistanceThreshold(const double& distanceThreshold)
{
    this->distanceThreshold = distanceThreshold;
}

void Material::setForceType(const QString& forceType)
{
    this->forceType = forceType;
}

void Material::setDragForceType(const QString& dragForceType)
{
    this->dragForceType = dragForceType;
}

void Material::addCoefficient(const double& coefficient)
{
    this->coefficients.push_back(coefficient);
}

void Material::addDragCoefficient(const double& dragCoefficient)
{
    this->dragCoefficients.push_back(dragCoefficient);
}

int Material::fromQStringToForceType(const QString& forceTypeString) const
{
    if(forceTypeString == "hooks_law")
        return hooks_law;

    else if(forceTypeString == "inverse_linear")
        return inverse_linear;

    else if(forceTypeString == "inverse_quadratic")
        return inverse_quadratic;

    else if(forceTypeString == "inverse_cubic")
        return inverse_cubic;

    else if(forceTypeString == "adiabatic_compression")
        return adiabatic_compression;

    else if(forceTypeString == "realistic_material")
        return realistic_material;

    else
        return -1;
}

int Material::fromQStringToDragForceType(const QString& dragForceTypeString) const
{
    if(dragForceTypeString == "linear")
        return linear;

    else if(dragForceTypeString == "quadratic")
        return quadratic;

    else if(dragForceTypeString == "cubic")
        return cubic;

    else
        return -1;
}

bool Material::operator==(const Material &other) const
{
    return this->id == other.id;
}

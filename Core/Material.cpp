#include <Material.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <cstring>

Material::Material()
{

}

Material::Material(const QJsonValue& jsonValue)
{
    QString _id = jsonValue["_id"].toString();
    QString name = jsonValue["name"].toString();

    QString materialId1 = jsonValue["materialId1"].toString();
    QString materialId2 = jsonValue["materialId2"].toString();

    double distanceThreshold = jsonValue["distanceThreshold"].toDouble();

    QString forceType     = jsonValue["forceType"].toString();
    QString dragForceType = jsonValue["dragForceType"].toString();

    this->setId(_id);
    this->setName(name);
    this->setParentsIds(materialId1, materialId2);
    this->setDistanceThreshold(distanceThreshold);
    this->setForceType(forceType);
    this->setDragForceType(dragForceType);

    QJsonValue coefficients = jsonValue["coefficients"];
    foreach(QJsonValue value, coefficients.toArray())
        this->addCoefficient(value.toDouble());

    QJsonValue dragCoefficients = jsonValue["dragCoefficients"];
    foreach(QJsonValue value, dragCoefficients.toArray())
        this->addDragCoefficient(value.toDouble());
}

MaterialCL Material::getCL(const QMap<QString, int>& idsMap) const
{
    MaterialCL materialCL;
    materialCL.index = idsMap[this->_id];
    materialCL.materialIndex1 = idsMap.contains(this->materialId1) ? idsMap[this->materialId1] : -1;
    materialCL.materialIndex2 = idsMap.contains(this->materialId2) ? idsMap[this->materialId2] : -1;
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
    return this->_id;
}

const QString& Material::getName() const
{
    return this->name;
}

const QString& Material::getMaterialId1() const
{
    return this->materialId1;
}

const QString& Material::getMaterialId2() const
{
    return this->materialId2;
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

void Material::setId(const QString& _id)
{
    this->_id = _id;
}

void Material::setName(const QString& name)
{
    this->name = name;
}

void Material::setParentsIds(const QString& materialId1, const QString& materialId2)
{
    this->materialId1 = materialId1;
    this->materialId2 = materialId2;
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
    if(forceTypeString == "Hook's Law")
        return hooks_law;

    else if(forceTypeString == "Inverse Linear")
        return inverse_linear;

    else if(forceTypeString == "Inverse Quadratic")
        return inverse_quadratic;

    else if(forceTypeString == "Inverse Cubic")
        return inverse_cubic;

    else if(forceTypeString == "Adiabatic Compression")
        return adiabatic_compression;

    else if(forceTypeString == "Realistic Material")
        return realistic_material;

    else
        return -1;
}

int Material::fromQStringToDragForceType(const QString& dragForceTypeString) const
{
    if(dragForceTypeString == "Linear")
        return linear;

    else if(dragForceTypeString == "Quadratic")
        return quadratic;

    else if(dragForceTypeString == "Cubic")
        return cubic;

    else
        return -1;
}

bool Material::operator==(const Material &other) const
{
    return this->_id == other._id;
}

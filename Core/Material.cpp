#include <Material.h>

#include <cstring>
#include <iostream>

Material::Material()
{

}

Material::Material(const nlohmann::json& jsonObject)
{
    try {
        this->id = QString::fromStdString(jsonObject.at("_id").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing '_id' field in Material");
    }

    try {
        this->material1 = QString::fromStdString(jsonObject.at("material1").get<std::string>());
        this->material2 = QString::fromStdString(jsonObject.at("material2").get<std::string>());
    }

    catch (...) { }

    try {
        this->distanceThreshold = jsonObject.at("distanceThreshold").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'distanceThreshold' field in Material");
    }

    try {
        this->forceType = QString::fromStdString(jsonObject.at("forceType").get<std::string>());
    }

    catch (...) { }

    try {
        this->dragForceType = QString::fromStdString(jsonObject.at("dragForceType").get<std::string>());
    }

    catch (...) { }

    try {
        const nlohmann::json& coefficients = jsonObject.at("coefficients");

        foreach(nlohmann::json value, coefficients)
            this->addCoefficient(value.get<double>());
    }

    catch (...) { }

    try {
        const nlohmann::json& dragCoefficients = jsonObject.at("dragCoefficients");

        foreach(nlohmann::json value, dragCoefficients)
            this->addDragCoefficient(value.get<double>());
    }

    catch (...) { }
}

MaterialCL Material::getCL(const QMap<QString, int>& idsMap) const
{
    MaterialCL materialCL;
    materialCL.materialIndex1 = idsMap.contains(this->material1) ? idsMap[this->material1] : -1;
    materialCL.materialIndex2 = idsMap.contains(this->material2) ? idsMap[this->material2] : -1;
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
    if(forceTypeString == "adiabatic_compression")
        return adiabatic_compression;

    else if(forceTypeString == "hooks_law")
        return hooks_law;

    else if(forceTypeString == "inverse_linear")
        return inverse_linear;

    else if(forceTypeString == "inverse_quadratic")
        return inverse_quadratic;

    else if(forceTypeString == "inverse_cubic")
        return inverse_cubic;

    else if(forceTypeString == "lennard_jones")
        return lennard_jones;

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

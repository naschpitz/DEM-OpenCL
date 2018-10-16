#include "Color.h"

Color::Color()
{

}

Color::Color(const QJsonObject& jsonObject)
{
    this->red   = jsonObject["red"].toInt();
    this->green = jsonObject["green"].toInt();
    this->blue  = jsonObject["blue"].toInt();
    this->alpha = jsonObject["alpha"].toDouble();
}

QJsonObject Color::getJson() const
{
    QJsonObject jsonObject;

    jsonObject["red"]   = this->red;
    jsonObject["green"] = this->green;
    jsonObject["blue"]  = this->blue;
    jsonObject["alpha"] = this->alpha;

    return jsonObject;
}

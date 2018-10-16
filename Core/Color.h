#ifndef COLOR_H
#define COLOR_H

#include <QJsonObject>
#include <QString>

class Color
{
    private:
        int    red;
        int    green;
        int    blue;
        double alpha;

    public:
        Color();
        Color(const QJsonObject& jsonObject);

        QJsonObject getJson() const;
};

#endif // COLOR_H

#ifndef TESTVERTEX_H
#define TESTVERTEX_H

#include <QObject>

#include "../Vertex.h"

class TestVertex : public QObject
{
    Q_OBJECT

    public:
        TestVertex();

    private slots:
        void defaultConstructor();
        void xyzConstructor();
        void vectorConstructor();
        void copyConstructor();
        void assignmentOperator();
        void setFixed();
        void setPosition();
        void setVelocity();
        void displaceBy();
        void getCLsetCLRoundtrip();
        void getJson();
};

#endif // TESTVERTEX_H

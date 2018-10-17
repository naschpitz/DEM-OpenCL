#-------------------------------------------------
#
# Project created by QtCreator 2018-08-22T13:51:50
#
#-------------------------------------------------

QT       += testlib network

TARGET = Test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    TestOpenCL.cpp \
    TestMaterial.cpp \
    main.cpp \
    TestVector3D.cpp \
    TestMaterialsManager.cpp \
    TestNonSolidObject.cpp \
    TestObjectsManager.cpp \
    TestParticle.cpp \
    TestScenery.cpp \
    TestSolidObject.cpp

LIBS += -lOpenCL

## TetGen Library
unix:!macx: LIBS += -L$$PWD/../libs/tetgen1.5.0/ -ltet

INCLUDEPATH += $$PWD/../libs/tetgen1.5.0
DEPENDPATH += $$PWD/../libs/tetgen1.5.0

unix:!macx: PRE_TARGETDEPS += $$PWD/../libs/tetgen1.5.0/libtet.a

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc

INCLUDEPATH += ../Core

DISTFILES += \
    Test.cl \
    Material.json \
    MaterialsManager.json \
    NonSolidObjectWireframe.json \
    ObjectsManager.json \
    Particle.json \
    Scenery.json \
    SolidObject.json \
    Simulation.json

HEADERS += \
    TestMaterial.h \
    TestOpenCL.h \
    TestVector3D.h \
    TestMaterialsManager.h \
    TestNonSolidObject.h \
    TestObjectsManager.h \
    TestParticle.h \
    TestScenery.h \
    TestSolidObject.h

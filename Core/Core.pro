QT += core network
QT -= gui

CONFIG += c++1z console
CONFIG -= app_bundle

TARGET = Core

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc

HEADERS += \
    OpenCL.h \
    NonSolidObject.h \
    SolidObject.h \
    Material.h \
    Vertex.h \
    Simulation.h \
    Scenery.h \
    MaterialsManager.h \
    ObjectsManager.h \
    Vector3D.h \
    Particle.h \
    Face.h \
    RequestMapper.h \
    RequestSender.h

SOURCES += \
    main.cpp \
    OpenCL.cpp \
    Vertex.cpp \
    Particle.cpp \
    NonSolidObject.cpp \
    SolidObject.cpp \
    Material.cpp \
    Simulation.cpp \
    Scenery.cpp \
    MaterialsManager.cpp \
    ObjectsManager.cpp \
    Vector3D.cpp \
    Face.cpp \
    RequestMapper.cpp \
    RequestSender.cpp

OTHER_FILES +=

DISTFILES += \
    Vertex.cl \
    Particle.cl \
    Simulation.cl \
    Vector.cl \
    ParticleWorker.cl \
    Material.cl \
    MaterialsManager.cl \
    Face.cl \
    FaceWorker.cl \
    Edge.cl \
    Scenery.cl \
    Simulation2.cl

LIBS+= -lcurl -lOpenCL -L/usr/local/lib -lrestclient-cpp

## TetGen Library
unix:!macx: LIBS += -L$$PWD/../libs/tetgen1.5.0/ -ltet

INCLUDEPATH += $$PWD/../libs/tetgen1.5.0
DEPENDPATH += $$PWD/../libs/tetgen1.5.0

unix:!macx: PRE_TARGETDEPS += $$PWD/../libs/tetgen1.5.0/libtet.a

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#---------------------------------------------------------------------------------------
# The following lines include the sources of the QtWebAppLib library
#---------------------------------------------------------------------------------------

include(../libs/QtWebApp/httpserver/httpserver.pri)

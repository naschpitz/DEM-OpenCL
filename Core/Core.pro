QT += core network
QT -= gui

CONFIG += c++1z console
CONFIG -= app_bundle

TARGET = Core

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc

HEADERS += \
    Deflater.h \
    Error.h \
    NonSolidObject.h \
    Pigz.h \
    SolidObject.h \
    Material.h \
    Vertex.h \
    Simulation.h \
    Scenery.h \
    MaterialsManager.h \
    Neighborhood.h \
    ObjectsManager.h \
    Vector3D.h \
    Particle.h \
    Face.h \
    RequestMapper.h \
    RequestSender.h \
    LogSender.h \
    FrameSender.h \
    EasyZLib.h \
    Sender.h \
    Common.h

SOURCES += \
    Deflater.cpp \
    Error.cpp \
    Pigz.cpp \
    main.cpp \
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
    RequestSender.cpp \
    LogSender.cpp \
    FrameSender.cpp \
    EasyZLib.cpp \
    Sender.cpp \
    Common.cpp

OTHER_FILES +=

DISTFILES += \
    Edge.cpp.cl \
    Edge.h.cl \
    Face.cpp.cl \
    Face.h.cl \
    FaceWorker.cpp.cl \
    LICENSE.md \
    Material.cpp.cl \
    Material.h.cl \
    MaterialsManager.cpp.cl \
    MaterialsManager.h.cl \
    Neighborhood.cpp.cl \
    Neighborhood.h.cl \
    Particle.cpp.cl \
    Particle.h.cl \
    ParticleWorker.cpp.cl \
    Scenery.h.cl \
    Simulation.cpp.cl \
    Simulation.h.cl \
    TestBox.cpp.cl \
    Vector.cpp.cl \
    Vertex.cpp.cl \
    Vertex.h.cl

LIBS+= -lcurl -lOpenCL -L/usr/local/lib -lrestclient-cpp

INCLUDEPATH += $$PWD/../libs
DEPENDPATH += $$PWD/../libs

## OpenCLWrapper Library
unix:!macx: LIBS += -L$$PWD/../libs/OpenCLWrapper/ -lOpenCLWrapper

unix:!macx: PRE_TARGETDEPS += $$PWD/../libs/OpenCLWrapper/libOpenCLWrapper.a


## TetGen Library
unix:!macx: LIBS += -L$$PWD/../libs/tetgen1.5.1/ -ltet

unix:!macx: PRE_TARGETDEPS += $$PWD/../libs/tetgen1.5.1/libtet.a


## EasyZLib Library
unix:!macx: LIBS += -L$$PWD/../libs/easyzlib/ -leasyzlib

unix:!macx: PRE_TARGETDEPS += $$PWD/../libs/easyzlib/libeasyzlib.a

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#---------------------------------------------------------------------------------------
# The following lines include the sources of the QtWebAppLib library
#---------------------------------------------------------------------------------------

include(../libs/QtWebApp/httpserver/httpserver.pri)

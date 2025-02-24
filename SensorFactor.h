#ifndef SENSORFACTOR_H
#define SENSORFACTOR_H

#include "device.h"

#include "compass.h"
#include "depth.h"
#include "posture.h"

class SensorFactory{
public:
    static Device* createSessnor(const QString & className);
};

#endif // SENSORFACTOR_H

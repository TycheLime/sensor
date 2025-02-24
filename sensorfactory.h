#ifndef SENSORFACTORY_H
#define SENSORFACTORY_H

#include "device.h"

#include "compass.h"
#include "depth.h"
#include "posture.h"

class SensorFactory
{
public:
    SensorFactory();
    static Device* createSensor(const QString & className);
};



#endif // SENSORFACTORY_H

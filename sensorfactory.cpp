#include "sensorfactory.h"

SensorFactory::SensorFactory() {}

Device *SensorFactory::createSensor(const QString &className)
{
    if(className == "compass"){
        return new Compass();
    }else if(className == "depth"){
        return new Depth();
    }else if(className == "posture"){
        return new Posture();
    }
    return nullptr;
}

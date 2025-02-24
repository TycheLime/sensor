#ifndef SENSORRELEASETASK_H
#define SENSORRELEASETASK_H

#include <QThreadPool>
#include <QRunnable>
#include <QDebug>

#include "device.h"

// 定义一个用于释放传感器资源的任务类
class SensorReleaseTask : public QRunnable {
public:
    explicit SensorReleaseTask(Device* sensor) : m_sensor(sensor) {}

    void run() override {
        if (m_sensor) {
            delete m_sensor;  // 直接删除 Device* 指针
            qDebug() << "Sensor released asynchronously.";
        }
    }

private:
    Device* m_sensor;
};

#endif // SENSORRELEASETASK_H

#ifndef DEVICE_H
#define DEVICE_H
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDateTime>

// Device.h
class Device : public QObject {
    Q_OBJECT

public:
    explicit Device(QObject *parent = nullptr);  // 添加构造函数
    virtual QString getDeviceInfo() const = 0;  // 获取设备信息的纯虚函数
    virtual void configureSerialPort() = 0;
    virtual bool openSerialPort() = 0;
    virtual ~Device() {}  // 添加虚析构函数
protected:
    QSerialPort serialPort;
    QMutex mtx;
    QString path;
    QFile file;
    QTextStream out;
    QDateTime lastUpdate;  // 记录最后一次更新的时间
    QVariantMap latestData;  // 存储最新的有效数据
};



#endif // DEVICE_H

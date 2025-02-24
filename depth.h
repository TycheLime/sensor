#ifndef DEPTH_H
#define DEPTH_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QRegularExpression>
#include <QDebug>
#include <qsettings.h>
#include <QDir>

#include "device.h"

class Depth : public Device {
    Q_OBJECT

private:
    int baudRate;
    double pressure = 0.0;
    int samplerate;
    QString portName;
    QString location;
    QTimer timer; // 定时器作为成员变量
    QSettings settings; // 设置对象
    bool status = false;
    int total = 0; // 总计报文
    int error = 0; // 错误报文

public:
    explicit Depth(QObject *parent = nullptr)
        : Device(parent),
        settings("D:/qt/config.ini", QSettings::IniFormat) { // 初始化QSettings

        loadSettings(); // 加载设置

        configureSerialPort();

        // 尝试打开串口
        if (!openSerialPort()) {
            qWarning() << "Failed to open serial port.";
        }

        saveDataCsv();

        setupTimer();
        connect(&timer, &QTimer::timeout, this, &Depth::query);
        connect(&timer, &QTimer::timeout, this, &Depth::handleTimerOut);
    }

    ~Depth() override{
        if(serialPort.isOpen())
            serialPort.close();
        if (file.isOpen()) {
            file.close();
        }
    }

    bool openSerialPort() override{
        QMutexLocker locker(&mtx);
        return serialPort.open(QIODevice::ReadWrite);
    }

    void configureSerialPort() override{
        QMutexLocker locker(&mtx);
        serialPort.setPortName(portName); // 使用从配置文件读取的端口名
        serialPort.setBaudRate(baudRate);
        serialPort.setDataBits(QSerialPort::Data8);
        serialPort.setParity(QSerialPort::NoParity);
        serialPort.setStopBits(QSerialPort::OneStop);
        serialPort.setFlowControl(QSerialPort::NoFlowControl);
    }

    // 实现 getDeviceInfo 方法
    QString getDeviceInfo() const override;

    QString getLocation(){
        return location;
    }

public slots:
    void sendDeviceInformation();
    void handleTimerOut();
    void query();

private:
    void sendOnce();
    void receiveOnce();
    void loadSettings(); // 新增方法，用于加载配置文件中的设置
    void setupTimer(); // 新增方法，用于设置或更新定时器
    void setCycle(); // 新增方法，用于设置采样周期
    int calculateSleepTime(int commandSize); // 新增方法，用于计算休眠时间
    void saveDataCsv();

signals:
    void depthReceived(double pressure);  // 用于通知 QML 更新压力数据
    void depthDeciveReceived(const QString& device);
    void errorThresholdExceeded(int errorCount);
public:
    void closeFile();
    void closePort();
};
#endif // DEPTH_H

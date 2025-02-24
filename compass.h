#ifndef COMPASS_H
#define COMPASS_H

// Compass.h
#include <QRegularExpression>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <qsettings.h>
#include <QDir>
#include <QVector>
#include <QQueue>

#include "device.h"

class Compass : public Device {
    Q_OBJECT

private:
    int baudRate;
    double pitch = 0.0;
    double roll = 0.0;
    double heading = 0.0;
    int samplerate;
    QString portName;
    QString location;
    QTimer timer; // 定时器作为成员变量
    QSettings settings; // 设置对象
    bool status = false; // 状态
    QQueue<QVector<uint8_t>> waiters;
    int total = 0; // 总计报文
    int error = 0; // 错误报文

public:
    explicit Compass(QObject *parent = nullptr)
        : Device(parent),

        settings("D:/qt/config.ini", QSettings::IniFormat) { // 初始化QSettings

        loadSettings(); // 加载设置

        configureSerialPort();

        // 尝试打开串口
        if (!openSerialPort()) {
            qWarning() << "Failed to open serial port.";
        }

        saveDataToCsv();

        setupTimer();
        connect(&timer, &QTimer::timeout, this, &Compass::query);
        connect(&timer, &QTimer::timeout, this, &Compass::handleTimerOut);
    }

    ~Compass() override {
        if(serialPort.isOpen())
            serialPort.close();
        if (file.isOpen()) {
            file.close();
        }
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

    bool openSerialPort() override{
        QMutexLocker locker(&mtx);
        return serialPort.open(QIODevice::ReadWrite);
    }

    // 实现 getDeviceInfo 方法
    QString getDeviceInfo() const override;

public slots:
    void sendDeviceInformation();
    void handleTimerOut();
    void query();

private:
    void sendOnce();
    void receiveOnce();
    void loadSettings(); // 新增方法，用于加载配置文件中的设置
    void setupTimer(); // 新增方法，用于设置或更新定时器
    double parseAngle(const QVector<uint8_t> &rawData, int startIndex);
    void saveDataToCsv();

    // 处理不完整帧头的情况
    void handleInvalidHeader(const QVector<uint8_t>& rawData, QVector<uint8_t>& message);

    // 处理校验和
    bool checkChecksum(const QVector<uint8_t>& mess);

    // 解析数据并发送信号
    void parseAndEmit(const QVector<uint8_t>& rawData);

    // 处理数据长度小于有效长度的情况
    void handleDataLengthLess(const QVector<uint8_t>& message, int dataLength);

    // 处理数据长度大于等于有效长度的情况
    void handleDataLengthGreater(const QVector<uint8_t>& message);

    // 校准
    // void calibration(); 暂时不写

signals:
    void compassReceived(double pitch, double roll, double heading);  // 用于通知更新数据
    void compassDeciveReceived(const QString& device);
    void errorThresholdExceeded(int errorCount);

public:
    void closeFile();
    void closePort();
};



#endif // COMPASS_H

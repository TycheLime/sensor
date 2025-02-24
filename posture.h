#ifndef POSTURE_H
#define POSTURE_H

#include <QRegularExpression>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <qsettings.h>
#include <QDir>
#include <QVector>
#include <QQueue>

#include "device.h"

class Posture: public Device
{
    Q_OBJECT

private:
    int baudRate;

    double temperature = 0.0;
    struct {double x; double y; double z;} accelerate = {0.0, 0.0, 0.0};// 加速度
    struct {double x; double y; double z;} gyroscope = {0.0, 0.0, 0.0}; //角速度
    struct {double x; double y; double z;} norm = {0.0, 0.0, 0.0}; // 磁场归一化值
    struct {double x; double y; double z;} strength = {0.0, 0.0, 0.0}; // 磁场强度
    struct {double pitch; double roll; double yaw;} eluer = {0.0, 0.0, 0.0}; // 欧拉角度
    struct {double a; double b; double c; double d;} quaternion = {0.0, 0.0, 0.0, 0.0}; // 四元数 a+bi+cj+dk
    struct {double latitude; double longitude; double altitude;} coordination = {0.0, 0.0, 0.0}; // 位置
    struct {double east; double north; double up;} speed = {0.0, 0.0, 0.0}; // 速度

    int samplerate;
    QString portName;
    QString location;
    QTimer timer; // 定时器作为成员变量
    QSettings settings; // 设置对象
    bool status = false;
    QQueue<QVector<uint8_t>> waiters;
    // Timestamp samplingTimestamp; // Sampling timestamp
    // Timestamp syncOutTimestamp; // Sync output timestamp
    int total = 0; // 总计报文
    int error = 0; // 错误报文

public:
    explicit Posture(QObject *parent = nullptr)
        : Device(parent), //samplingTimestamp(0, 0, 0, 0), syncOutTimestamp(0, 0, 0, 0),

        settings("D:/qt/config.ini", QSettings::IniFormat){ // 初始化QSettings

        loadSettings(); // 加载设置

        configureSerialPort();

        // 尝试打开串口
        if (!openSerialPort()) {
            qWarning() << "Failed to open serial port.";
        }

        saveDataToCsv();

        setupTimer();
        connect(&timer, &QTimer::timeout, this, &Posture::query);
        connect(&timer, &QTimer::timeout, this, &Posture::handleTimerOut);
    }

    ~Posture() override {
        if (serialPort.isOpen()) {
            serialPort.close();
        }
        if (file.isOpen()) {
            file.close();
        }
    }

    void configureSerialPort() override{
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
    void setCycle(); // 新增方法，用于设置采样周期
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

    double parse1D(const QVector<uint8_t> &rawData, int index, int dataValidLength);

    std::tuple<double, double, double> parse3D(const QVector<uint8_t> &rawData, int index, int dataValidLength, int left = 4, int right = 8);

    std::tuple<double, double, double, double>parse4D(const QVector<uint8_t> &rawData, int index, int dataValidLength);

signals:
    void postureReceived(double temperature, double acc_x, double acc_y, double acc_z, double gyr_x, double gyr_y,
                         double gyr_z, double norm_x, double norm_y, double norm_z,
                         double str_x, double str_y, double str_z, double elu_pitch, double elu_roll, double elu_yaw,
                         double qua_a, double qua_b, double qua_c, double qua_d, double coo_a, double coo_o, double coo_l, double spe_e, double spe_n, double spe_u);  // 用于通知更新数据
    void postureDeviceReceived(const QString& device);
    void errorThresholdExceeded(int errorCount);

public:
    void closeFile();
    void closePort();
};



#endif // POSTURE_H

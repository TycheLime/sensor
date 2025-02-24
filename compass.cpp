#include "compass.h"

QString Compass::getDeviceInfo() const
{
    QString message;
    // 添加端口名称
    message += "端口号: " + portName + "\n";

    QString flag  = (status==true)? "成功" : "失败";
    message +="连接:" + flag + "\n";
    // 添加波特率
    message += "波特率: " + QString::number(baudRate) + "\n";
    // 添加采样频率
    message += "采样频率: " + QString::number(samplerate) + " Hz\n";
    // 添加保存路径
    message += "保存路径: " + location + "\n";
    qInfo() << message;
    return message;
}

void Compass::sendOnce() {
    // 创建一个十六进制表示的 QByteArray
    auto command = QByteArray::fromHex("6804000408");

    try {
        // 直接将原始的十六进制字节数据写入串口
        serialPort.write(command);
        if (serialPort.waitForBytesWritten(10)) {
            qDebug() << "Command sent successfully.";
        }
    } catch (...) {
        qCritical() << "Error sending data.";
    }
}

// 68 0D 00 84 10 26 80 00 33 65 03 13 71 66
void Compass::receiveOnce() {
    QThread::msleep(4);

    QByteArray buf = serialPort.readAll();

    if(!buf.isEmpty()){
        QVector<uint8_t> rawData;
        QVector<uint8_t> message;
        auto rawInit = [&](auto &&self,int current, int target){
            if (current >= target){
                return;
            }
            rawData.append(static_cast<uint8_t> (buf.at(current)));
            return self(self, current + 1, target);
        };

        rawInit(rawInit, 0, buf.size());


        size_t rawLength = rawData.size();

        if (rawLength > 0 && rawData[0] != 0x68) {
            handleInvalidHeader(rawData, message);
        } else {
            message = rawData;
        }

        if (message.size() > 1) {
            int dataLength = message[1];
            size_t validLength = message.size() - 1; // 减去帧头和校验和字节
            total++;
            if (dataLength < validLength) {
                handleDataLengthLess(message, dataLength);
            } else {
                handleDataLengthGreater(message);
            }
        }
    }
}

void Compass::sendDeviceInformation()
{
    QString deviceInfo = getDeviceInfo();
    emit compassDeciveReceived(deviceInfo);
}

void Compass::handleTimerOut()
{
    QMutexLocker locker(&mtx);

    // 假设错误率阈值为 5%
    const double errorThreshold = 0.05;
    int errorCount = error;
    int totalCount = total;

    // 计算错误率
    double errorRate = (totalCount > 0) ? static_cast<double>(errorCount) / totalCount : 0.0;

    // 判断是否超过阈值
    if (errorRate > errorThreshold) {
        emit errorThresholdExceeded(errorCount);
    }

    // 重置计数器
    error = 0;
    total = 0;
}

void Compass::query()
{
    QMutexLocker locker(&mtx);
    sendOnce();
    receiveOnce();
}

void Compass::loadSettings()
{
    QDateTime now = QDateTime::currentDateTime();
    settings.beginGroup("compass"); // 开始读取[compass]组
    baudRate = settings.value("baudRate", 57600).toInt(); // 默认值为9600
    samplerate = settings.value("frequency", 50).toInt(); // 默认值为50
    portName = settings.value("port", "COM3").toString(); // 默认值为"COM3"
    location = settings.value("location", "D:\\qt\\sensor\\").toString().append(now.toString(Qt::ISODate).replace(":", "-")); // 默认值为"D:\\qt\\sensor"
    settings.endGroup(); // 结束读取[compass]组
    status = true;

    qDebug() << "Loaded settings - Port:" << portName
             << ", Baud Rate:" << baudRate
             << ", Sample Rate:" << samplerate
             << ", Location:" << location;

    setupTimer();
}

void Compass::setupTimer()
{
    if(samplerate > 50){
        samplerate = 50;
    }

    if(samplerate < 1){
        samplerate = 1;
    }
    timer.stop(); // 如果定时器正在运行，则先停止它
    int interval = static_cast<int>(1000.0 / samplerate + 0.5); // 计算定时器间隔时间（毫秒）
    timer.start(interval); // 设置并启动定时器
}

double Compass::parseAngle(const QVector<uint8_t> &rawData, int startIndex)
{
    if (startIndex + 2 >= rawData.size()) {
        qInfo() << "Insufficient data for angle parsing.";
        return 0.0;
    }

    int sign = (rawData[startIndex] & 0x10) ? -1 : 1;

    int intPart = rawData[startIndex + 1] % 16 + rawData[startIndex + 1] / 16 * 10 + rawData[startIndex] % 16 * 100;
    int decPart = rawData[startIndex + 2] % 16 + rawData[startIndex + 2] / 16 * 10;

    return sign * (intPart + decPart * 1.0 / 100);
}

void Compass::saveDataToCsv()
{
    QDir dir(location);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create the directory:" << location;
            return;
        }
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString(Qt::ISODate).replace(":", "-").append("-compass.csv");

    // 使用 QDir::separator() 或者直接用 / 来拼接路径
    file.setFileName(location + QDir::separator() + name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open the file.";
        return;
    }

    out.setDevice(&file);
    out << "time, " << "pitch" << ", " << "roll" << ", " << "heading" << Qt::endl;
}

void Compass::handleInvalidHeader(const QVector<uint8_t> &rawData, QVector<uint8_t> &message)
{
    qInfo() << rawData;
    qInfo() << "Invalid frame header";
    if (!waiters.isEmpty()) {
        auto header = waiters.front();
        waiters.pop_front();
        message = header;

        for (auto raw : rawData) {
            message.push_back(raw);
        }
        qInfo() << "fulfill this message";

        // 验证补全后的消息是否以 0x68 开头
        if (message.size() > 0 && message[0] != 0x68) {
            qInfo() << "Even after fulfillment, the message still doesn't start with 0x68";
            return;
        }
    }
}

bool Compass::checkChecksum(const QVector<uint8_t> &mess)
{
    if (mess.size() > 1) {
        uint8_t ck = 0;
        uint8_t receivedCk = mess[mess.size() - 1];

        auto checkInit = [&](auto &&self, int current, int target) -> void {
            if (current >= target) {
                return;
            }
            ck += mess[current];
            self(self, current + 1, target);
        };
        checkInit(checkInit, 1, mess.size() - 1);

        if (ck != receivedCk) {
            qInfo() << "Checksum error: Expected (" << static_cast<int>(ck)
            << "), but received (" << static_cast<int>(receivedCk) << ")";
            error++;
            return false;
        } else {
            qInfo() << "Checksum verified successfully.";
        }
    }
    return true;
}

void Compass::parseAndEmit(const QVector<uint8_t> &rawData)
{
    if (rawData.size() >= 11) {
        // 解析俯仰角
        pitch = parseAngle(rawData, 4);

        // 解析横滚角
        roll = parseAngle(rawData, 7);

        // 解析航向角
        heading = parseAngle(rawData, 10);

        latestData["pitch"] = pitch;
        latestData["roll"] = roll;
        latestData["heading"] = heading;
        lastUpdate = QDateTime::currentDateTime();
        qint64 tick = lastUpdate.toMSecsSinceEpoch();
        qInfo() << pitch << ", " << roll << ", " << heading;

        out << tick << ", " << pitch << ", " << roll << ", " << heading << Qt::endl;

        emit compassReceived(pitch, roll, heading);
    }
}

void Compass::handleDataLengthLess(const QVector<uint8_t> &message, int dataLength)
{
    if (message.size() >= dataLength) {
        QVector<uint8_t> mess(message.begin(), message.begin() + dataLength + 1);
        qInfo() << "will miss a message";
        qInfo() << mess;

        if (message.size() > dataLength) {
            QVector<uint8_t> waiter(message.begin() + dataLength + 1, message.end());
            waiters.emplace_back(waiter);
        }

        if (checkChecksum(mess)) {
            parseAndEmit(message);
        }
    }
}

void Compass::handleDataLengthGreater(const QVector<uint8_t> &message)
{
    qInfo() << "This situation should rarely happen, indicating fragmented messages.";
    if (checkChecksum(message)) {
        parseAndEmit(message);
    }
}

void Compass::closeFile()
{
    if(file.isOpen()){
        file.close();
    }
}

void Compass::closePort()
{
    if(serialPort.isOpen()){
        serialPort.close();
    }
}




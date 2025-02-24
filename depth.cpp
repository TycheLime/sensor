#include "depth.h"

QString Depth::getDeviceInfo() const
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

void Depth::sendOnce() {
    qInfo() << "Sending command...";
    // 发送一次命令
    QByteArray cmd = "run\r\n";
    qint64 bytesWritten = serialPort.write(cmd);
    if (bytesWritten == -1) {
        qWarning() << "Error sending data.";
    } else {
        qInfo() << "Command sent.";
    }
}

void Depth::receiveOnce() {
    QByteArray buf;
    QThread::msleep(3); // 等待3毫秒确保传感器完成转换

    buf = serialPort.readAll();
    if (!buf.isEmpty()) {
        QString strData(buf);
        qInfo() << "Raw data:" << strData;

        // 去除可能存在的 \r\n 结尾
        strData = strData.remove(QChar('\r')).remove(QChar('\n'));

        // 使用正则表达式提取数字部分
        QRegularExpression regex(R"(\d+(\.\d+)?)");
        QRegularExpressionMatch match = regex.match(strData);

        if (match.hasMatch()) {
            QString pressureStr = match.captured(0);
            bool ok;
            pressure = pressureStr.toDouble(&ok);
            total++;

            if (ok) {
                // 更新最新数据
                latestData["pressure"] = pressure;
                lastUpdate = QDateTime::currentDateTime();
                qint64 tick = lastUpdate.toMSecsSinceEpoch();

                qInfo() << "Parsed pressure:" << pressure;
                out << tick << ", " << pressure << Qt::endl;

                emit depthReceived(pressure);
            } else {
                qWarning() << "Failed to parse pressure from:" << strData;
                error++;
            }

            emit depthReceived(pressure);
        } else {
            qWarning() << "No valid pressure data found in:" << strData;
        }
    }
}

void Depth::sendDeviceInformation()
{
    QString deviceInfo = getDeviceInfo();
    emit depthDeciveReceived(deviceInfo);
}

void Depth::handleTimerOut()
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

void Depth::query()
{
    QMutexLocker locker(&mtx);
    sendOnce();
    receiveOnce();
}

void Depth::setupTimer()
{
    if(samplerate > 16){
        samplerate = 16;
    }

    if(samplerate < 0){
        samplerate = 2;
    }
    timer.stop(); // 如果定时器正在运行，则先停止它
    int interval = static_cast<int>(1000.0 / samplerate + 0.5); // 计算定时器间隔时间（毫秒）
    timer.start(interval); // 设置并启动定时器
}

void Depth::setCycle()
{
    if (samplerate >= 2 && samplerate <= 16) {
        QString cmd = "setcycle " + QString::number(samplerate) + " H\r\n";
        QByteArray data = cmd.toUtf8();

        if (serialPort.isOpen()) {
            qint64 bytesWritten = serialPort.write(data);
            if (bytesWritten == data.size()) {
                qDebug() << "Command sent successfully: " << cmd;
                // 自动休眠一段时间
                int sleepTime = calculateSleepTime(data.size());
                QThread::msleep(sleepTime);
            } else {
                qDebug() << "Failed to send command.";
            }
        } else {
            qDebug() << "Serial port is not open.";
        }
    } else {
        qDebug() << "Invalid samplerate. Must be between 2 and 16.";
    }
}

int Depth::calculateSleepTime(int commandSize)
{
    // 假设每个字符传输时间为 1 / baudRate 秒
    // 3.5 字符时间 + 假设返回信息最大长度为 20 字符时间
    float charTime = (1000.0f * 9) / serialPort.baudRate(); // 每个字符大约传输时间（毫秒）
    return static_cast<int>((3.5 + commandSize) * charTime + 0.5f); // 四舍五入
}

void Depth::saveDataCsv()
{
    QDir dir(location);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create the directory:" << location;
            return;
        }
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString(Qt::ISODate).replace(":", "-").append("-depth.csv")
        ;
    // 使用 QDir::separator() 或者直接用 / 来拼接路径
    file.setFileName(location + QDir::separator() + name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open the file.";
        return;
    }

    out.setDevice(&file);
    out << "time, "<< "pressure" << Qt::endl;
}

void Depth::closeFile()
{
    if(file.isOpen()){
        file.close();
    }
}

void Depth::closePort()
{
    if(serialPort.isOpen()){
        serialPort.close();
    }
}

void Depth::loadSettings()
{
    QDateTime now = QDateTime::currentDateTime();
    settings.beginGroup("depth"); // 开始读取[depth]组
    baudRate = settings.value("baudRate", 57600).toInt(); // 默认值为9600
    samplerate = settings.value("frequency", 16).toInt(); // 默认值为16
    portName = settings.value("port", "COM1").toString(); // 默认值为"COM3"
    location = settings.value("location", "D:\\qt\\sensor\\").toString().append(now.toString(Qt::ISODate).replace(":", "-")); // 默认值为"D:\\qt\\sensor"
    settings.endGroup(); // 结束读取[depth]组
    status = true;

    setCycle();

    qDebug() << "Loaded settings - Port:" << portName
             << ", Baud Rate:" << baudRate
             << ", Sample Rate:" << samplerate
             << ", Location:" << location;

    setupTimer();
}

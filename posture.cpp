#include "posture.h"


QString Posture::getDeviceInfo() const
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

void Posture::sendOnce()
{

}

void Posture::receiveOnce()
{
    // QThread::msleep(4); // Waiting for sensor data

    QByteArray buf = serialPort.readAll();

    QVector<uint8_t> rawData;
    QVector<uint8_t> message;

    auto rawInit = [&](auto &&self, int current, int target){
        if(current >= target){
            return;
        }
        rawData.append(static_cast<uint8_t>(buf.at(current)));
        return self(self, current + 1, target);
    };

    rawInit(rawInit, 0, buf.size());

    size_t rawLength = rawData.size();

    if (rawLength > 0 && rawData[0] != 0x59) {
        handleInvalidHeader(rawData, message);
    } else {
        message = rawData;
    }

    if (message.size() > 1) {
        int dataLength = message[4];
        size_t validLength = message.size() - 7; // 减去帧头和校验和字节
        qInfo() << "dataLength = " << dataLength <<"," << "validLength = " << validLength;
        if (dataLength < validLength) {
            handleDataLengthLess(message, dataLength);
        } else {
            handleDataLengthGreater(message);
        }
    }
}

void Posture::sendDeviceInformation()
{
    QString deviceInfo = getDeviceInfo();
    emit postureDeviceReceived(deviceInfo);
}

void Posture::handleTimerOut()
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

void Posture::query()
{
    QMutexLocker locker(&mtx);
    sendOnce();
    receiveOnce();
}

void Posture::loadSettings()
{
    QDateTime now = QDateTime::currentDateTime();
    settings.beginGroup("posture"); // 开始读取[posture]组
    baudRate = settings.value("baudRate", 460800).toInt(); // 默认值为230400
    samplerate = settings.value("frequency", 200).toInt(); // 默认值为200
    portName = settings.value("port", "COM5").toString(); // 默认值为"COM5"
    location = settings.value("location", "D:\\qt\\sensor\\").toString().append(now.toString(Qt::ISODate).replace(":", "-")); // 默认值为"D:\\qt\\sensor"
    settings.endGroup(); // 结束读取[posture]组
    status = true;

    setCycle();

    qDebug() << "Loaded settings - Port:" << portName
             << ", Baud Rate:" << baudRate
             << ", Sample Rate:" << samplerate
             << ", Location:" << location;

    setupTimer();
}

void Posture::setupTimer()
{
    if(samplerate > 200){
        samplerate = 200;
    }

    if(samplerate < 1){
        samplerate = 1;
    }
    timer.stop(); // 如果定时器正在运行，则先停止它
    int interval = static_cast<int>(1000.0 / samplerate + 0.5); // 计算定时器间隔时间（毫秒）
    timer.start(interval); // 设置并启动定时器
}

void Posture::setCycle()
{
    QMutexLocker locker(&mtx);
    QByteArray command;

    if(baudRate == 1){
        command = QByteArray::fromHex("5953030003010F02");
    }else if(baudRate == 2){
        command = QByteArray::fromHex("5953030003020F03");
    }else if(baudRate == 5){
        command = QByteArray::fromHex("5953030003030F04");
    }else if(baudRate == 10){
        command = QByteArray::fromHex("5953030003040F05");
    }else if(baudRate == 20){
        command = QByteArray::fromHex("5953030003050F06");
    }else if(baudRate == 25){
        command = QByteArray::fromHex("5953030003060F08");
    }else if(baudRate == 50){
        command = QByteArray::fromHex("5953030003070F09");
    }else if(baudRate == 100){
        command = QByteArray::fromHex("5953030003080F1A");
    }else if(baudRate == 200){
        command = QByteArray::fromHex("5953030003090F1B");
    }else if(baudRate >= 250 && baudRate < 400){
        command = QByteArray::fromHex("59530300030A0F1C");
    }else if(baudRate >= 400 && baudRate < 800){
        command = QByteArray::fromHex("59530300030B0F1D");
    }else if(baudRate >= 100 && baudRate < 1600){
        command = QByteArray::fromHex("59530300030C0F1E");
    }else if(baudRate >= 2000 && baudRate < 3200){
        command = QByteArray::fromHex("59530300030D0F1F");
    }

    QByteArray hexString = command.toHex(' ');
    qDebug() << hexString;
    try {
        serialPort.write(hexString);
        if (serialPort.waitForBytesWritten(1000)) {
            qDebug() << "Command sent successfully.";
        }
    } catch (...) {
        qCritical() << "Error sending data.";
    }
}


void Posture::saveDataToCsv()
{   QDir dir(location);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create the directory:" << location;
            return;
        }
    }

    qInfo() << "location = " << location;

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString(Qt::ISODate).replace(":", "-").append("-posture.csv");

    // 使用 QDir::separator() 或者直接用 / 来拼接路径
    file.setFileName(location + QDir::separator() + name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open the file.";
        return;
    }

    out.setDevice(&file);
    out << "time" <<", " << "temperature" << ", "
        << "accelerate_x" << ", " << "accelerate_y" << ", " << "accelerate_z" << ", "
        << "gyroscope_x" << ", " <<"gyroscope_y" << ", " << "gyroscope_z" << ", "
        << "norm_x" << ", " << "norm_y" << ", " << "norm_z" << ", "
        << "strength_x" << ", " <<"strength_y" << ", "  << "strength_z" << ", "
        << "eluer_pitch" << ", " << "eluer_roll" << ", "<< "eluer_yaw" << ", "
        << "quaternion_a" << ", " << "quaternion_b" << ", " << "quaternion_c" << ", " << "quaternion_d" << ", "
        << "coordination_latitude" << ", " << "coordination_longitude" << ", " << "coordination_altitude" << ", "
        << "speed_east" << ", "<< "speed_north" << ", " << "speed_np" << Qt::endl;
}

void Posture::handleInvalidHeader(const QVector<uint8_t> &rawData, QVector<uint8_t> &message)
{
    qInfo() << rawData;
    qInfo() << "Invalid frame header";
    qInfo() << rawData.size();
    if (!waiters.isEmpty()) {
        auto header = waiters.front();
        waiters.pop_front();
        message = header;

        for (auto raw : rawData) {
            message.push_back(raw);
        }
        qInfo() << "fulfill this message";

        // 验证补全后的消息是否以 0x59 开头
        if (message.size() > 0 && (message[0] != 0x59 || message[1] != 0x53)) {
            qInfo() << "Even after fulfillment, the message still doesn't start with 0x68";
            return;
        }
    }
}

bool Posture::checkChecksum(const QVector<uint8_t> &mess)
{
    if (mess.size() > 1) {
        // 计算校验码
        uint8_t CK1 = 0;
        uint8_t CK2 = 0;
        // 假设从第5个字节开始是有效数据（跳过帧头和帧编号）
        for(size_t i = 2; i < mess.size() - 2; i++){ // 不包括最后的两个校验码字节
            CK1 += mess[i];
            CK2 += CK1;
        }

        // 获取接收到的校验码
        uint8_t receivedCK1 = mess[mess.size() - 2];
        uint8_t receivedCK2 = mess[mess.size() - 1];

        // 验证校验码是否匹配
        if(CK1 != receivedCK1 || CK2 != receivedCK2){
            qInfo() << "Checksum error: Expected (" << static_cast<int>(CK1) << "," << static_cast<int>(CK2)
            << "), but received (" << static_cast<int>(receivedCK1) << "," << static_cast<int>(receivedCK2) << ")";
            return false;
        } else {
            qInfo() << "Checksum verified successfully.";
        }
    }
    return true;
}

void Posture::parseAndEmit(const QVector<uint8_t> &rawData)
{
    size_t rawLength = rawData.size();
    size_t validLength = rawLength - 7;

    int dataId = 0;
    int index = 5;

    while(index < validLength){
        qInfo() << "index location = " << index;
        dataId = rawData[index];
        switch (dataId) {
        case 0x01:{
            index++;
            int dataValidLength = rawData[index];
            double value_t = parse1D(rawData, index, dataValidLength);
            index += dataValidLength + 1;

            temperature = {value_t * 0.01};
            qDebug() << "Temperature: " << temperature;
            break;
        }
        case 0x10:{
            index++;
            int dataValidLength = rawData[index];
            auto [value_x, value_y, value_z] = parse3D(rawData, index, dataValidLength);
            index += dataValidLength + 1;

            accelerate = {value_x * 0.000001 , value_y * 0.000001, value_z * 0.000001};
            qDebug() << "ax: " << accelerate.x  << ", ay: " << accelerate.y << ", az: " << accelerate.z;
            break;
        }
        case 0x20:{
            index++;
            int dataValidLength = rawData[index];
            auto [value_x, value_y, value_z] = parse3D(rawData, index, dataValidLength);
            index += dataValidLength + 1;

            gyroscope = {value_x * 0.000001 , value_y * 0.000001, value_z * 0.000001};
            qDebug() << "gx: " << gyroscope.x << ", gy: " << gyroscope.y << ", gz: " << gyroscope.z;
            break;
        }
        case 0x30:{
            index++;
            int dataValidLength = rawData[index];
            auto [value_x, value_y, value_z] = parse3D(rawData, index, dataValidLength);
            index += dataValidLength + 1;

            norm = {value_x * 0.000001 , value_y * 0.000001, value_z * 0.000001};
            qDebug() << "normx: " << norm.x << ", normy: " << norm.y << ", normz: " << norm.z;
            break;
        }
        case 0x31:{
            index++;
            int dataValidLength = rawData[index];
            auto [value_x, value_y, value_z] = parse3D(rawData, index, dataValidLength);
            index += dataValidLength + 1;

            strength = {value_x * 0.001 , value_y * 0.001, value_z * 0.001};
            qDebug() << "strengthx: " << strength.x << ", strengthy: " << strength.y << ", strengthz: " << strength.z;
            break;
        }
        case 0x40:{
            index++;
            int dataValidLength = rawData[index];
            auto [value_x, value_y, value_z] = parse3D(rawData, index, dataValidLength);
            index += dataValidLength + 1;

            eluer = {value_x * 0.000001 , value_y * 0.000001, value_z * 0.000001};
            qDebug() << "Pitch: " << eluer.pitch << ", Roll: " << eluer.roll << ", Yaw: " << eluer.yaw;
            break;
        }
        case 0x41:{
            index++;
            int dataValidLength = rawData[index];
            auto [value_a, value_b, value_c, value_d] = parse4D(rawData, index, dataValidLength);
            index += dataValidLength + 1;

            quaternion = {value_a * 0.000001 , value_b * 0.000001, value_c * 0.000001, value_d * 0.000001};
            qDebug() << "a: " << quaternion.a << ", b: " << quaternion.b << ", c: " << quaternion.c << ", d:" << quaternion.d;
            break;
        }
        case 0x68:{
            qInfo() << "???why will ?";
            index++;
            int dataValidLength = rawData[index];
            auto [latitude, longitude, altitude] = parse3D(rawData, index, dataValidLength, 8, 16);
            index += dataValidLength + 1;

            coordination = {latitude * 0.0000000001, longitude * 0.0000000001, altitude * 0.001};
            qDebug() << "lat: " << coordination.latitude << ", long: " << coordination.longitude << ", alt: " << coordination.altitude;
            break;
        }
        case 0x70:{
            index++;
            int dataValidLength = rawData[index];
            auto [value_e, value_n, value_u] = parse3D(rawData, index, dataValidLength);
            index += dataValidLength + 1;

            speed = {value_e * 0.001 , value_n * 0.001, value_u * 0.001};
            qDebug() << "East: " << speed.east << ", North: " << speed.north << ", Up: " << speed.up;
            break;
        }
        default:
            break;
        }
    }

    latestData["temperature"] = temperature;
    latestData["acclerate.x"] = accelerate.x;
    latestData["acclerate.y"] = accelerate.y;
    latestData["acclerate.z"] = accelerate.z;
    latestData["gyroscope.x"] = gyroscope.x;
    latestData["gyroscope.y"] = gyroscope.y;
    latestData["gyroscope.z"] = gyroscope.z;
    latestData["norm.x"] = norm.x;
    latestData["norm.y"] = norm.y;
    latestData["norm.z"] = norm.z;
    latestData["strength.x"] = strength.x;
    latestData["strength.y"] = strength.y;
    latestData["strength.z"] = strength.z;
    latestData["eluer.pitch"] = eluer.pitch;
    latestData["eluer.roll"] = eluer.roll;
    latestData["eluer.yaw"] = eluer.yaw;
    latestData["quaternion.a"] = quaternion.a;
    latestData["quaternion.b"] = quaternion.b;
    latestData["quaternion.c"] = quaternion.c;
    latestData["quaternion.d"] = quaternion.d;
    latestData["coordination.latitude"] = coordination.latitude;
    latestData["coordination.longitude"] = coordination.longitude;
    latestData["coordination.altitude"] = coordination.altitude;
    latestData["speed.east"] = speed.east;
    latestData["speed.northt"] = speed.north;
    latestData["peed.up"] = speed.up;

    lastUpdate = QDateTime::currentDateTime();
    qint64 tick = lastUpdate.toMSecsSinceEpoch();

    out << tick <<", " << temperature << ", "
        << accelerate.x << ", " << accelerate.y << ",  " << accelerate.z << ", "
        << gyroscope.x << ", " << gyroscope.y << ", " << gyroscope.z << ", "
        << norm.x << ", " << norm.y << ", " << norm.z << ", "
        << strength.x << ", " << strength.y << ", " << strength.z << ","
        << eluer.pitch << ", " << eluer.roll << ", " << eluer.yaw << ", "
        << quaternion.a << ", " << quaternion.b << ", " << quaternion.c << ", " << ", "
        << coordination.altitude << ", " << coordination.latitude <<", " << coordination.longitude << ", "
        << speed.east << ", " << speed.north << ", " << speed.up << Qt::endl;


    emit postureReceived(temperature, accelerate.x, accelerate.y, accelerate.z, gyroscope.x, gyroscope.y, gyroscope.z,
                         norm.x, norm.y, norm.z, strength.x, strength.y,
                         strength.z, eluer.pitch, eluer.roll, eluer.yaw,
                         quaternion.a, quaternion.b, quaternion.c, quaternion.d,
                         coordination.latitude, coordination.longitude, coordination.altitude,
                         speed.east, speed.north, speed.up);
}

void Posture::handleDataLengthLess(const QVector<uint8_t> &message, int dataLength)
{
    if (message.size() >= dataLength){
        QVector<uint8_t> mess(message.begin(), message.begin() + dataLength + 8);
        qInfo() << "original" << message;
        qInfo() << "will miss a message";
        qInfo() << mess;

        if (message.size() > dataLength) {
            QVector<uint8_t> waiter(message.begin() + dataLength + 8, message.end());
            waiters.emplace_back(waiter);
        }

        if (checkChecksum(mess)) {
            parseAndEmit(message);
        }
    }
}

void Posture::handleDataLengthGreater(const QVector<uint8_t> &message)
{
    qInfo() << "This situation should rarely happen, indicating fragmented messages.";
    if (checkChecksum(message)) {
        parseAndEmit(message);
    }
}

double Posture::parse1D(const QVector<uint8_t> &rawData, int index, int dataValidLength)
{
    double value = 0.0;
    for(int i = 0; i < dataValidLength; i++){
        value += static_cast<int64_t>(rawData[index + i + 1] << (i  * 8));
    }
    return value;

}

std::tuple<double, double, double> Posture::parse3D(const QVector<uint8_t> &rawData, int index, int dataValidLength, int left, int right)
{
    double value_x, value_y, value_z;
    value_x = value_y = value_z = 0.0;
    for(int i = 0; i < dataValidLength; i++){
        if(i < left){
            value_x += static_cast<int64_t>(rawData[index + i + 1] << (i % 4) * 8);
        }else if(i < right ){
            value_y += static_cast<int64_t>(rawData[index + i + 1] << (i % 4) * 8);
        }else{
            value_z += static_cast<int64_t>(rawData[index + i + 1] << (i % 4) * 8);
        }
    }
    return {value_x, value_y, value_z};
}

std::tuple<double, double, double, double> Posture::parse4D(const QVector<uint8_t> &rawData, int index, int dataValidLength)
{
    double value_a = 0.0;
    double value_b = 0.0;
    double value_c = 0.0;
    double value_d = 0.0;
    for(int i = 0; i < dataValidLength; i++){

        if(i < 4){
            value_a += static_cast<int64_t>(rawData[index + i + 1] << (i % 4) * 8);
        }else if(i < 8 ){
            value_b += static_cast<int64_t>(rawData[index + i + 1] << (i % 4) * 8);
        }else if(i < 12){
            value_c += static_cast<int64_t>(rawData[index + i + 1] << (i % 4) * 8);
        }else {
            value_d += static_cast<int64_t>(rawData[index + i + 1] << (i % 4) * 8);
        }
    }
    return {value_a, value_b, value_c, value_d};
}

void Posture::closeFile()
{
    if(file.isOpen()){
        file.close();
    }
}

void Posture::closePort()
{
    if(serialPort.isOpen()){
        serialPort.close();
    }
}

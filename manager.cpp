#include "manager.h"

void Manager::startOperation() {
    initSensor();

    // 连接 Nut 和 Compass 的数据接收信号到 Manager 的槽函数
    connect(depth, &Depth::depthReceived, this, &Manager::handleDeptheReceived);
    connect(compass, &Compass::compassReceived, this, &Manager::handleCompassReceived);
    connect(posture, &Posture::postureReceived, this, &Manager::handlePostureReceived);

    // 启动 Nut 和 Compass 的线程
    depthThread->start();
    compassThread->start();
    postureThread->start();
    location = depth->getLocation();

    // 等待线程启动，设置超时时间为 0.5 秒
    if (!depthThread->wait(500)) {
        qDebug() << "Depth thread failed to start within 5 seconds.";
    }
    if (!compassThread->wait(500)) {
        qDebug() << "Compass thread failed to start within 5 seconds.";
    }
    if (!postureThread->wait(500)) {
        qDebug() << "Posture thread failed to start within 5 seconds.";
    }

    startTimers();

    qDebug() << "Operation started.";
}

void Manager::delayUpdates() {
    // 如果 updateDelayTimer 已经存在，先停止它
    if (updateDelayTimer) {
        updateDelayTimer->stop();
    } else {
        updateDelayTimer = new QTimer(this);
    }

    updateDelayTimer->setSingleShot(true); // 单次触发
    updateDelayTimer->start(1); // 设置为 1 毫秒延迟
    qDebug() << "Updates delayed for 1 milliseconds.";

    // 连接定时器超时信号到相应的槽函数
    connect(updateDelayTimer, &QTimer::timeout, this, &Manager::onUpdateDelayTimeout);
}

void Manager::initSensor()
{
    depth = qobject_cast<Depth*>(SensorFactory::createSensor("depth"));
    compass = qobject_cast<Compass*>(SensorFactory::createSensor("compass"));
    posture = qobject_cast<Posture*>(SensorFactory::createSensor("posture"));

    depthThread = new QThread(this);
    compassThread = new QThread(this);
    postureThread = new QThread(this);

    depth->moveToThread(depthThread);
    compass->moveToThread(compassThread);
    posture->moveToThread(postureThread);
}

void Manager::shutSensor()
{
    // 停止定时器
    if (depthTimer) {
        depthTimer->stop();
        qDebug() << "Depth timer stopped.";
    }
    if (compassTimer) {
        compassTimer->stop();
        qDebug() << "Compass timer stopped.";
    }
    if (postureTimer) {
        postureTimer->stop();
        qDebug() << "Posture timer stopped.";
    }

    // 请求中断线程
    if (depthThread) {
        depthThread->requestInterruption();
        if (depthThread->isRunning()) {
            depthThread->quit();
            depthThread->wait(500);
            qDebug() << "Depth thread stopped.";
        }
        delete depthThread;
        depthThread = nullptr;
    }

    if (compassThread) {
        compassThread->requestInterruption();
        if (compassThread->isRunning()) {
            compassThread->quit();
            compassThread->wait(500);
            qDebug() << "Compass thread stopped.";
        }
        delete compassThread;
        compassThread = nullptr;
    }

    if (postureThread) {
        postureThread->requestInterruption();
        if (postureThread->isRunning()) {
            postureThread->quit();
            postureThread->wait(500);
            qDebug() << "Posture thread stopped.";
        }
        delete postureThread;
        postureThread = nullptr;
    }

    depth->closeFile();
    compass->closeFile();
    posture->closeFile();

    depth->closePort();
    compass->closePort();
    posture->closePort();

    delete depth;
    delete compass;
    delete posture;

    depth = nullptr;
    compass = nullptr;
    posture = nullptr;
}

void Manager::selfInspection()
{
    QMutexLocker locker(&mtx);
    depth = qobject_cast<Depth*>(SensorFactory::createSensor("depth"));
    compass = qobject_cast<Compass*>(SensorFactory::createSensor("compass"));
    posture = qobject_cast<Posture*>(SensorFactory::createSensor("posture"));

    connect(depth, &Depth::depthDeciveReceived, this, &Manager::handleDepthDeviceReceived);
    connect(compass, &Compass::compassDeciveReceived, this, &Manager::handleCompassDeviceReceived);
    connect(posture, &Posture::postureDeviceReceived, this, &Manager::handlePostureDeviceReceived);

    depth->sendDeviceInformation();
    compass->sendDeviceInformation();
    posture->sendDeviceInformation();
    location = depth->getLocation();

    delete depth;
    depth = nullptr;
    delete compass;
    compass = nullptr;
    delete posture;
    posture = nullptr;
}

void Manager::swapCondition()
{
    QMutexLocker locker(&mtx);
    if(running){
        startOperation();
    }else{
        stopOperation();
    }
    running = not running;
}

void Manager::handleDeptheReceived(double pressure) {
    emit depthReceived(pressure);
    delayUpdates();
}

void Manager::handleCompassReceived(double pitch, double roll, double heading) {
    emit compassReceived(pitch, roll, heading);
    delayUpdates();
}

void Manager::handlePostureReceived(double temperature, double acc_x, double acc_y, double acc_z, double gyr_x, double gyr_y, double gyr_z,
                                    double norm_x, double norm_y, double norm_z, double str_x, double str_y, double str_z,
                                    double elu_pitch, double elu_roll, double elu_yaw, double qua_a, double qua_b, double qua_c, double qua_d,
                                    double coo_a, double coo_o, double coo_l, double spe_e, double spe_n, double spe_u)
{
    emit postureReceived(temperature, acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, norm_x, norm_y, norm_z,
                         str_x, str_y, str_z, elu_pitch, elu_roll, elu_yaw,
                         qua_a, qua_b, qua_c, qua_d, coo_a, coo_o, coo_l, spe_e, spe_n, spe_u);
    delayUpdates();
}

void Manager::handleCompassDeviceReceived(const QString &devices)
{
    qDebug() << "Received device info from Compass:" << devices;
    // 触发 Manager 的 deviceStatusReceived 信号，通知 QML
    emit deviceCompassStatusReceived(devices);
}

void Manager::handleDepthDeviceReceived(const QString &devices)
{
    emit deviceDepthStatusReceived(devices);
}

void Manager::handlePostureDeviceReceived(const QString &devices)
{
    emit devicePostureStatusReceived(devices);
}

void Manager::stopOperation() {
    shutSensor();

    qDebug() << "Operation stopped.";
}

void Manager::startTimers()
{
    depthTimer = new QTimer(this);
    compassTimer = new QTimer(this);
    postureTimer = new QTimer(this);

    depthTimer->setTimerType(Qt::PreciseTimer);
    compassTimer->setTimerType(Qt::PreciseTimer);
    postureTimer->setTimerType(Qt::PreciseTimer);

    depthTimer->setTimerType(Qt::PreciseTimer);
    compassTimer->setTimerType(Qt::PreciseTimer);
    postureTimer->setTimerType(Qt::PreciseTimer);
}

void Manager::loadConfigFromQml(const QString &path)
{
    emit configLoaded(path);
}

void Manager::checkOperation()
{
    selfInspection();
}






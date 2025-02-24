#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QVariant>
#include <QMutex>
#include <QThreadPool>
#include <QRunnable>

#include "sensorfactory.h"

class Manager : public QObject {
    Q_OBJECT

private:
    Depth *depth;
    Compass *compass;
    Posture *posture;
    QThread *depthThread;
    QThread *compassThread;
    QThread *postureThread;
    QTimer *updateDelayTimer; // 用于延迟更新的计时器
    QMutex mtx;
    QTimer *depthTimer;
    QTimer *compassTimer;
    QTimer *postureTimer;
    bool running = true;
    QString location;

public:
    explicit Manager(QObject *parent = nullptr)
        : QObject(parent)
    {

    }

    ~Manager() {
        if(depth){
            delete depth;
            depth = nullptr;
        }

        if(compass){
            delete compass;
            compass = nullptr;
        }

        if(posture){
            delete posture;
            posture = nullptr;
        }

        if(depthThread){
            delete depthThread;
            depthThread = nullptr;
        }

        if(compassThread){
            delete compassThread;
            compassThread = nullptr;
        }

        if(postureThread){
            delete postureThread;
            postureThread = nullptr;
        }

        if(updateDelayTimer){
            delete updateDelayTimer;
            updateDelayTimer = nullptr;
        }

        if(depthTimer){
            delete depthTimer;
            depthTimer = nullptr;
        }

        if(compassTimer){
            delete compassTimer;
            compassTimer = nullptr;
        }

        if(postureTimer){
            delete postureTimer;
            postureTimer = nullptr;
        }
    }

signals:
    void depthReceived(double pressure);
    void compassReceived(double pitch, double roll, double heading);
    void postureReceived(double temperature, double acc_x, double acc_y, double acc_z, double gyr_x, double gyr_y, double gyr_z,
                         double norm_x, double norm_y, double norm_z, double str_x, double str_y, double str_z,
                         double elu_pitch, double elu_roll, double elu_yaw, double qua_a, double qua_b, double qua_c, double qua_d,
                         double coo_a, double coo_o, double coo_l, double spe_e, double spe_n, double spe_u);
    void configLoaded(const QString& path);

    void deviceCompassStatusReceived(const QString& devices);
    void deviceDepthStatusReceived(const QString& devices);
    void devicePostureStatusReceived(const QString& deives);

public slots:
    void handleDeptheReceived(double pressure);
    void handleCompassReceived(double pitch, double roll, double heading);
    void handlePostureReceived(double temperature, double acc_x, double acc_y, double acc_z, double gyr_x, double gyr_y, double gyr_z,
                               double norm_x, double norm_y, double norm_z, double str_x, double str_y, double str_z,
                               double elu_pitch, double elu_roll, double elu_yaw, double qua_a, double qua_b, double qua_c, double qua_ds,
                            double coo_a, double coo_o, double coo_l, double spe_e, double spe_n, double spe_u);

    void handleCompassDeviceReceived(const QString& devices);
    void handleDepthDeviceReceived(const QString& devices);
    void handlePostureDeviceReceived(const QString& devices);

    void loadConfigFromQml(const QString &path);
    void checkOperation();
    void swapCondition();

private slots:
    void onUpdateDelayTimeout() {
        // 定时器超时后恢复正常的更新频率
        // qDebug() << "Resuming updates...";
    }

private:
    void delayUpdates();
    void initSensor();
    void shutSensor();
    void selfInspection();
    void startOperation();
    void stopOperation();
    void startTimers();


public:
    Q_INVOKABLE Device* getSensor(const QString& name){
        if(name == "depth"){
            return depth;
        }else if(name == "compass"){
            return compass;
        }else if(name == "posture"){
            return posture;
        }
        return nullptr;
    }

    Q_INVOKABLE QString getLocation(){
        return location;
    }
};


#endif // MANAGER_H

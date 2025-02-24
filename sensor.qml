import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Fusion

Rectangle {
    width: 1152
    height: 700
    color: "transparent"

    property var runDepthInfo: []
    property var runCompassInfo: []
    property var runPostureInfo: []
    property var runLocationInfo: []
    property var runVialogInfo: []
    property var rungGidanceInfo: []

    // 定义需要的变量
    property double depthPressure: 0
    property double compassPitch: 0
    property double compassRoll: 0
    property double compassHeading: 0
    property double postureTemperate: 0
    property double postureAccX: 0
    property double postureAccY: 0
    property double postureAccZ: 0
    property double postureGyrX: 0
    property double postureGyrY: 0
    property double postureGyrZ: 0
    property double postureNorX: 0
    property double postureNorY: 0
    property double postureNorZ: 0
    property double postureStrX: 0
    property double postureStrY: 0
    property double postureStrZ: 0
    property double postureEluPitch: 0
    property double postureEluRoll: 0
    property double postureEluYaw: 0
    property double postureQuaA: 0
    property double postureQuaB: 0
    property double postureQuaC: 0
    property double postureQuaD: 0
    property string name: ""

    Component.onCompleted: {
        manager.depthReceived.connect(function(pressure) {
            depthPressure = parseFloat(pressure.toFixed(2))
            runDepthInfo = [
                { label: "压力", value: depthPressure + " Pa" }
            ]
            // 可根据需求添加显示逻辑，比如显示矩形
        });

        manager.compassReceived.connect(function(pitch, roll, heading) {
            compassPitch = parseFloat(pitch.toFixed(2))
            compassRoll = parseFloat(roll.toFixed(2))
            compassHeading = parseFloat(heading.toFixed(2))
            runCompassInfo = [
                { label: "倾斜角", value: compassPitch + "°" },
                { label: "横滚角重力工具面角", value: compassRoll + "°" },
                { label: "方位角", value: compassHeading + "°" }
            ]
            // 可根据需求添加显示逻辑，比如显示矩形
        });

        manager.postureReceived.connect(function(temperature, acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, norm_x, norm_y, norm_z,
                                                 str_x, str_y, str_z, elu_pitch, elu_roll, elu_yaw, qua_a, qua_b, qua_c, qua_d) {
            // 更新 posture 对象的属性
            postureTemperate = parseFloat(temperature.toFixed(2))
            postureAccX = parseFloat(acc_x.toFixed(2))
            postureAccY = parseFloat(acc_y.toFixed(2))
            postureAccZ = parseFloat(acc_z.toFixed(2))

            postureGyrX = parseFloat(gyr_x.toFixed(2))
            postureGyrY = parseFloat(gyr_y.toFixed(2))
            postureGyrZ = parseFloat(gyr_z.toFixed(2))

            postureNorX = parseFloat(norm_x.toFixed(2))
            postureNorY = parseFloat(norm_y.toFixed(2))
            postureNorZ = parseFloat(norm_z.toFixed(2))

            postureStrX = parseFloat(str_x.toFixed(2))
            postureStrY = parseFloat(str_y.toFixed(2))
            postureStrZ = parseFloat(str_z.toFixed(2))

            postureEluPitch = parseFloat(elu_pitch.toFixed(2))
            postureEluRoll = parseFloat(elu_roll.toFixed(2))
            postureEluYaw = parseFloat(elu_yaw.toFixed(2))

            postureQuaA = parseFloat(qua_a.toFixed(2))
            postureQuaB = parseFloat(qua_b.toFixed(2))
            postureQuaC = parseFloat(qua_c.toFixed(2))
            postureQuaD = parseFloat(qua_d.toFixed(2))

            runPostureInfo = [
                { label: "温度", value: postureTemperate + " °C" },
                { label: "加速度 X", value: postureAccX + " m/s²" },
                { label: "加速度 Y", value: postureAccY + " m/s²" },
                { label: "加速度 Z", value: postureAccZ + " m/s²" },
                { label: "角速度 X", value: postureGyrX + " rad/s" },
                { label: "角速度 Y", value: postureGyrY + " rad/s" },
                { label: "角速度 Z", value: postureGyrZ + " rad/s" },
                { label: "法线 X", value: postureNorX },
                { label: "法线 Y", value: postureNorY },
                { label: "法线 Z", value: postureNorZ },
                { label: "应力 X", value: postureStrX },
                { label: "应力 Y", value: postureStrY },
                { label: "应力 Z", value: postureStrZ },
                { label: "欧拉角 俯仰", value: postureEluPitch + "°" },
                { label: "欧拉角 横滚", value: postureEluRoll + "°" },
                { label: "欧拉角 偏航", value: postureEluYaw + "°" },
                { label: "四元数 A", value: postureQuaA },
                { label: "四元数 B", value: postureQuaB },
                { label: "四元数 C", value: postureQuaC },
                { label: "四元数 D", value: postureQuaD }
            ]
        });
    }

    Component.onDestruction: {
        if(!isRunning){
            manager.swapCondition()
            isRunning = !isRunning
        }
    }

    Rectangle {
        id: frame
        color: "#C3F7FA"
        radius: 8
        width: parent.width * 0.95
        height: parent.height * 0.95
        anchors.centerIn: parent

        Item {
            anchors.centerIn: parent

            Column {
                anchors.centerIn: parent
                spacing: 10

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "传感器运行信息"
                    font.family: "华文楷体"
                    font.pixelSize: 18
                    font.bold: true
                }
                Rectangle {
                    id: serial
                    width: frame.width * 0.92
                    height: frame.height * 0.92
                    color: "#FFF"
                    border.color: "#657AFF"
                    radius: 16
                    // 让 serial 垂直居中
                    anchors.horizontalCenter: parent.horizontalCenter

                    Column {
                        id: mainColumn
                        anchors.margins: 10
                        anchors.fill: parent
                        spacing: 10
                        anchors.centerIn: parent
                        anchors.topMargin: 70

                        Row{
                            spacing: 10
                            width: parent.width // 设置 Row 的宽度为父容器的宽度
                            anchors.horizontalCenter: parent.horizontalCenter

                            // 罗盘设备信息
                            SensorInfo {
                                title: "电子罗盘"
                                infoModel: runCompassInfo
                                color: "#F2F2F2"
                            }

                            // 深度设备信息
                            SensorInfo {
                                title: "深度传感器"
                                infoModel: runDepthInfo
                                color: "#E0F7FA"
                            }
                        }

                        Row{
                            spacing: 10
                            width: parent.width // 设置 Row 的宽度为父容器的宽度
                            anchors.horizontalCenter: parent.horizontalCenter

                            // 姿态设备信息
                            SensorInfo {
                                title: "姿态仪"
                                infoModel: runPostureInfo.slice(0, 3)
                                color: "#FFF0F5"
                            }

                            // 水声定位设备信息
                            SensorInfo {
                                title: "水声定位"
                                infoModel: runLocationInfo
                                color: "#FFFACD"
                            }
                        }

                        Row{
                            spacing: 10
                            width: parent.width // 设置 Row 的宽度为父容器的宽度
                            anchors.horizontalCenter: parent.horizontalCenter

                            // 计程仪设备信息
                            SensorInfo {
                                title: "计程仪"
                                infoModel: runVialogInfo
                                color: "#FFE4E1"
                            }

                            // 惯性导航设备信息
                            SensorInfo {
                                title: "惯性导航"
                                infoModel: rungGidanceInfo
                                color: "#F0FFF0"
                            }
                        }

                        // 操作按钮
                        Button {
                            width: 100
                            height: 24
                            font.family: "华文楷体"
                            font.pixelSize: 12
                            text: isRunning ? "运行" :"停止"
                            anchors.horizontalCenter: parent.horizontalCenter // 让按钮水平居中
                            onClicked: {
                                // 清空所有传感器信息数组
                                runDepthInfo = []
                                runCompassInfo = []
                                runPostureInfo = []
                                runLocationInfo = []
                                runVialogInfo = []
                                rungGidanceInfo = []

                                // 重置所有传感器信息变量
                                depthPressure = 0
                                compassPitch = 0
                                compassRoll = 0
                                compassHeading = 0
                                postureTemperate = 0
                                postureAccX = 0
                                postureAccY = 0
                                postureAccZ = 0
                                postureGyrX = 0
                                postureGyrY = 0
                                postureGyrZ = 0
                                postureNorX = 0
                                postureNorY = 0
                                postureNorZ = 0
                                postureStrX = 0
                                postureStrY = 0
                                postureStrZ = 0
                                postureEluPitch = 0
                                postureEluRoll = 0
                                postureEluYaw = 0
                                postureQuaA = 0
                                postureQuaB = 0
                                postureQuaC = 0
                                postureQuaD = 0

                                manager.swapCondition()
                                isRunning = !isRunning
                            }
                            background: Rectangle {
                                anchors.fill: parent
                                radius: 24
                                color: "#98FB98" // 修改按钮颜色
                                border.color: "#F74545"
                            }
                        }
                    }
                }
            }
        }
    }

    // 显示倒计时的文本
    Text {
        id: countdownText
        text: "当前保存路径: " + manager.getLocation()
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: frame.bottom
        }
    }
}

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import com.sensor

Window {
    visible: true
    width: 1152
    height: 700
    title: qsTr("数据采集")
    id: root

    minimumHeight: 700
    minimumWidth: 1152
    maximumHeight: 700
    maximumWidth: 1152
    color: "transparent"

    property var depthInfo: []
    property var compassInfo: []
    property var postureInfo: []
    property var locationInfo: []
    property var vialogInfo: []
    property var guidanceInfo: []
    property bool isRunning: false
    property string name: ""
    property real compassPitch: 0.0
    property real compassRoll: 0.0
    property real compassHeading: 0.0
    property real depthPressure: 0.0
    property real postureTemperate: 0.0
    property real postureAccX: 0.0
    property real postureAccY: 0.0
    property real postureAccZ: 0.0
    property real postureGyrX: 0.0
    property real postureGyrY: 0.0
    property real postureGyrZ: 0.0
    property real postureNorX: 0.0
    property real postureNorY: 0.0
    property real postureNorZ: 0.0
    property real postureStrX: 0.0
    property real postureStrY: 0.0
    property real postureStrZ: 0.0
    property real postureEluPitch: 0.0
    property real postureEluRoll:0.0
    property real postureEluYaw: 0.0
    property real postureQuaA: 0.0
    property real postureQuaB: 0.0
    property real postureQuaC: 0.0
    property real postureQuaD:0.0

    Manager{
        id: manager
    }

    Component.onCompleted: {
        // 连接信号到对应的更新函数
        manager.deviceCompassStatusReceived.connect(updateCompassInfo);
        manager.deviceDepthStatusReceived.connect(updateDepthInfo);
        manager.devicePostureStatusReceived.connect(updatePostureInfo);
    }

    Component.onDestruction: {
        manager.stopOperation();
    }

    function updateCompassInfo(infoString) {
        var lines = infoString.split("\n");
        var port = "";
        var connectionStatus = "";
        var baudRate = "";
        var sampleRate = "";
        var savePath = "";

        for (var i = 0; i < lines.length; i++) {
            var line = lines[i];
            if (line.indexOf("端口号:") !== -1) {
                port = line.split(":")[1].trim();
            } else if (line.indexOf("连接:") !== -1) {
                connectionStatus = line.split(":")[1].trim();
            } else if (line.indexOf("波特率:") !== -1) {
                baudRate = line.split(":")[1].trim();
            } else if (line.indexOf("采样频率:") !== -1) {
                sampleRate = line.split(":")[1].trim();
            } else if (line.indexOf("保存路径:") !== -1) {
                savePath = line
            }
        }

        compassInfo = [
            { label: qsTr("串口号:"), value: port },
            { label: qsTr("连接状态:"), value: connectionStatus },
            { label: qsTr("波特率:"), value: baudRate },
            { label: qsTr("采样频率:"), value: sampleRate },
            { label: qsTr("保存路径:"), value: savePath }
        ];
    }

    function updateDepthInfo(infoString) {
        var lines = infoString.split("\n");
        var port = "";
        var connectionStatus = "";
        var baudRate = "";
        var sampleRate = "";
        var savePath = "";

        for (var i = 0; i < lines.length; i++) {
            var line = lines[i];
            if (line.indexOf("端口号:") !== -1) {
                port = line.split(":")[1].trim();
            } else if (line.indexOf("连接:") !== -1) {
                connectionStatus = line.split(":")[1].trim();
            } else if (line.indexOf("波特率:") !== -1) {
                baudRate = line.split(":")[1].trim();
            } else if (line.indexOf("采样频率:") !== -1) {
                sampleRate = line.split(":")[1].trim();
            } else if (line.indexOf("保存路径:") !== -1) {
                savePath = line
            }
        }

        depthInfo = [
            { label: qsTr("串口号:"), value: port },
            { label: qsTr("连接状态:"), value: connectionStatus },
            { label: qsTr("波特率:"), value: baudRate },
            { label: qsTr("采样频率:"), value: sampleRate },
            { label: qsTr("保存路径:"), value: savePath }
        ];
    }

    function updatePostureInfo(infoString) {
        var lines = infoString.split("\n");
        var port = "";
        var connectionStatus = "";
        var baudRate = "";
        var sampleRate = "";
        var savePath = "";

        for (var i = 0; i < lines.length; i++) {
            var line = lines[i];
            if (line.indexOf("端口号:") !== -1) {
                port = line.split(":")[1].trim();
            } else if (line.indexOf("连接:") !== -1) {
                connectionStatus = line.split(":")[1].trim();
            } else if (line.indexOf("波特率:") !== -1) {
                baudRate = line.split(":")[1].trim();
            } else if (line.indexOf("采样频率:") !== -1) {
                sampleRate = line.split(":")[1].trim();
            } else if (line.indexOf("保存路径:") !== -1) {
                savePath = line;
            }
        }

        postureInfo = [
            { label: qsTr("串口号:"), value: port },
            { label: qsTr("连接状态:"), value: connectionStatus },
            { label: qsTr("波特率:"), value: baudRate },
            { label: qsTr("采样频率:"), value: sampleRate },
            { label: qsTr("保存路径:"), value: savePath }
        ];
    }

    Rectangle {
        id: frame
        color: "#C3F7FA"
        radius: 8
        width: root.width * 0.95
        height: root.height * 0.95
        anchors.centerIn: parent

        Item {
            anchors.centerIn: parent

            Column {
                anchors.centerIn: parent
                spacing: 10

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "传感器数据采集"
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
                                infoModel: compassInfo
                                color: "#F2F2F2"
                            }

                            // 深度设备信息
                            SensorInfo {
                                title: "深度传感器"
                                infoModel: depthInfo
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
                                infoModel: postureInfo
                                color: "#FFF0F5"
                            }

                            // 水声定位设备信息
                            SensorInfo {
                                title: "水声定位"
                                infoModel: locationInfo
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
                                infoModel: vialogInfo
                                color: "#FFE4E1"
                            }

                            // 惯性导航设备信息
                            SensorInfo {
                                title: "惯性导航"
                                infoModel: guidanceInfo
                                color: "#F0FFF0"
                            }
                        }

                        Row{
                            spacing: 10
                            width: parent.width // 设置 Row 的宽度为父容器的宽度
                            anchors.horizontalCenter: parent.horizontalCenter
                            // 操作按钮
                            Button {
                                width: 100
                                height: 24
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                text: "上电自检"
                                onClicked: {
                                    console.log("上电自检")
                                    manager.checkOperation()
                                    background.color = "#FF0000"
                                }
                                background: Rectangle {
                                    anchors.fill: parent
                                    radius: 24
                                    color: "#F2DFDF"
                                    border.color: "#F74545"
                                }
                            }

                            // 操作按钮
                            Button {
                                width: 100
                                height: 24
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                text: isRunning ? "运行" :"停止"
                                onClicked: {
                                    manager.startOperation()
                                    pageLoader.source = "sensor.qml"
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
    }
    Loader{
        id: pageLoader
    }
}

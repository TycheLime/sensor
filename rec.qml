import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Controls.Fusion
import com.sensor

Window {
    id: root
    visible: true
    width: 1152
    height: 1030
    title: qsTr("数据采集")

    minimumHeight: 1030
    minimumWidth: 1152
    maximumHeight: 1030
    maximumWidth: 1152

    // 创建全局的 Manager 对象
    Manager {
        id: manager
    }

    property var depthInfo: []
    property var compassInfo: []
    property var postureInfo: []

    Component.onCompleted: {
        // 连接信号到对应的更新函数
        manager.deviceCompassStatusReceived.connect(updateCompassInfo);
        manager.deviceDepthStatusReceived.connect(updateDepthInfo);
        manager.devicePostureStatusReceived.connect(updatePostureInfo);
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

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: Component {
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

                            Column {
                                id: mainColumn
                                anchors.margins: 10
                                anchors.fill: parent
                                spacing: 10
                                anchors.centerIn: parent

                                Row{
                                    spacing: 10
                                    width: parent.width // 设置 Row 的宽度为父容器的宽度
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    // 罗盘设备信息
                                    Rectangle {
                                        width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#F2F2F2"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "电子罗盘"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: compassInfo.length > 0 ? compassInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // 深度设备信息
                                    Rectangle {
                                         width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#E0F7FA"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "深度传感器"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: depthInfo.length > 0 ? depthInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }

                                }


                                Row{
                                    spacing: 10
                                    width: parent.width // 设置 Row 的宽度为父容器的宽度
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    // 姿态设备信息
                                    Rectangle {
                                        width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#FFEBEE"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "姿态仪"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: postureInfo.length > 0 ? postureInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // 水声定位设备信息
                                    Rectangle {
                                        width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#FFEBEE"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "水声定位"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: postureInfo.length > 0 ? postureInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                Row{
                                    spacing: 10
                                    width: parent.width // 设置 Row 的宽度为父容器的宽度
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    // 计程仪设备信息
                                    Rectangle {
                                        width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#F2F2F2"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "计程仪"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: compassInfo.length > 0 ? compassInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // 惯性导航设备信息
                                    Rectangle {
                                         width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#E0F7FA"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "惯性导航"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: depthInfo.length > 0 ? depthInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }


                                Row{
                                    spacing: 10
                                    width: parent.width // 设置 Row 的宽度为父容器的宽度
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    // 罗盘设备信息
                                    Rectangle {
                                        width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#F2F2F2"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "电子罗盘"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: compassInfo.length > 0 ? compassInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // 深度设备信息
                                    Rectangle {
                                         width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#E0F7FA"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "深度传感器"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: depthInfo.length > 0 ? depthInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }

                                }


                                Row{
                                    spacing: 10
                                    width: parent.width // 设置 Row 的宽度为父容器的宽度
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    // 姿态设备信息
                                    Rectangle {
                                        width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#FFEBEE"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "姿态仪"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: postureInfo.length > 0 ? postureInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // 水声定位设备信息
                                    Rectangle {
                                        width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#FFEBEE"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "水声定位"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: postureInfo.length > 0 ? postureInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                Row{
                                    spacing: 10
                                    width: parent.width // 设置 Row 的宽度为父容器的宽度
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    // 计程仪设备信息
                                    Rectangle {
                                        width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#F2F2F2"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "计程仪"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: compassInfo.length > 0 ? compassInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // 惯性导航设备信息
                                    Rectangle {
                                         width: (parent.width - parent.spacing) / 2
                                        height: 140
                                        color: "#E0F7FA"
                                        radius: 10
                                        border.color: "#9E9E9E"

                                        Column {
                                            anchors.fill: parent
                                            spacing: 8
                                            padding: 10

                                            Text {
                                                text: "惯性导航"
                                                font.family: "华文楷体"
                                                font.pixelSize: 14
                                                color: "#2F4F4F"
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: depthInfo.length > 0 ? depthInfo : [{ label: "", value: "" }]
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        text: modelData.label
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#333333"
                                                    }
                                                    Text {
                                                        text: modelData.value
                                                        font.family: "华文楷体"
                                                        font.pixelSize: 12
                                                        color: "#555555"
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                // 操作按钮
                                Button {
                                    width: 100
                                    height: 24
                                    font.family: "华文楷体"
                                    font.pixelSize: 12
                                    text: "上电自检"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    onClicked: {
                                        manager.checkOperation()
                                    }
                                    background: Rectangle {
                                        anchors.fill: parent
                                        radius: 24
                                        color: "#F2DFDF"
                                        border.color: "#F74545"
                                    }
                                }
                            }
                        }
                    }

                    Button {
                        id: next
                        width: 48
                        height: 48
                        x: 400
                        y: 20
                        text: ">"
                        font.family: "华文楷体"
                        font.pixelSize: 24
                        background: Rectangle {
                            anchors.fill: parent
                            radius: 24
                            color: "#F2DFE9"
                            border.color: "#F74545"
                        }
                        onClicked: {
                            stackView.push(nextPage)
                        }
                    }
                }
            }
        }

        transitions: Transition {
            // 禁用默认动画
        }
    }

    Component {
        id: nextPage
        Rectangle {
            color: "#C3F7FA"
            radius: 8
            width: root.width * 0.95
            height: root.height * 0.95
            anchors.centerIn: parent

            ButtonGroup {
                buttons: column.children
            }
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

            Component.onCompleted: {
                manager.depthReceived.connect(function(pressure) {
                    depthPressure = parseFloat(pressure.toFixed(2))
                    if(isRunning){
                        depthInfoRect.visible = true
                    } else {
                        messageInfoRect.visible = true
                    }
                });

                manager.compassReceived.connect(function(pitch, roll, heading) {
                    compassPitch = parseFloat(pitch.toFixed(2))
                    compassRoll = parseFloat(roll.toFixed(2))
                    compassHeading = parseFloat(heading.toFixed(2))
                    if(isRunning){
                        compassInfoRect.visible = true
                    } else {
                        messageInfoRect.visible = true
                    }
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
                    postureAccZ = parseFloat(str_z.toFixed(2))

                    postureEluPitch = parseFloat(elu_pitch.toFixed(2))
                    postureEluRoll = parseFloat(elu_roll.toFixed(2))
                    postureEluYaw = parseFloat(elu_yaw.toFixed(2))

                    postureQuaA = parseFloat(qua_a.toFixed(2))
                    postureQuaB = parseFloat(qua_b.toFixed(2))
                    postureQuaC = parseFloat(qua_c.toFixed(2))
                    postureQuaD = parseFloat(qua_d.toFixed(2))

                    if (isRunning) {
                        postureInfoRect.visible = true;
                    } else {
                        messageInfoRect.visible = true;
                    }
                });

                // 获取 depth 传感器对象
                var depthSensor = manager.getSensor("depth");
                if(isRunning){
                    if (depthSensor.handleTimerOut()){
                        name = "深度传感器"
                        errorInfoRect.visible = true
                    }
                }

                var compassSensor = manager.getSensor("compass");
                if(isRunning){
                    if (compassSensor.handleTimerOut()){
                        name = "电子罗盘"
                        errorInfoRect.visible = true
                    }
                }

                var postureSensor = manager.getSensor("posture");
                if(isRunning){
                    if (compassSensor.handleTimerOut()){
                        name = "姿势仪"
                        errorInfoRect.visible = true
                    }
                }
            }

            Component.onDestruction: {
                if(isRunning){
                    manager.stopOperation()
                }
            }

            Column {
                id: column
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10

                Rectangle {
                    width: parent.width
                    height: 10
                    color: "transparent"  // 透明矩形作为占位元素
                }

                Row{
                    spacing: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    Button {
                        width: 100
                        height:24
                        font.family: "华文楷体"
                        font.pixelSize: 12
                        checked: true
                        text: isRunning ? "停止" : "运行"

                        onClicked: {
                            isRunning = !isRunning;
                            if (!isRunning) {
                                manager.stopOperation();
                            } else {
                                manager.startOperation()
                            }
                        }
                        background: Rectangle {
                            anchors.fill: parent
                            radius: 12
                            color: "#FFDBDB"
                            opacity: 0.6
                            border.color: "#FFF"
                        }
                    }

                    Button {
                        width: 100
                        height:24
                        font.family: "华文楷体"
                        font.pixelSize: 12
                        text: "电子罗盘"
                        onClicked: {
                            if (!isRunning) {
                                messageInfoRect.visible = true
                            } else {
                                compassInfoRect.visible = true
                            }
                        }
                        background: Rectangle {
                            anchors.fill: parent
                            radius: 24
                            color: "#FFDBDB"
                            opacity: 0.7
                            border.color: "#FFF"
                        }
                    }

                    Button {
                        width: 100
                        height:24
                        font.family: "华文楷体"
                        font.pixelSize: 12
                        text: "深度传感器"
                        onClicked: {
                            if (!isRunning) {
                                messageInfoRect.visible = true
                            } else {
                                depthInfoRect.visible = true
                            }
                        }
                        background: Rectangle {
                            anchors.fill: parent
                            radius: 24
                            color: "#FFDBDB"
                            opacity: 0.6
                            border.color: "#FFF"
                        }
                    }

                    Button {
                        width: 100
                        height:24
                        font.family: "华文楷体"
                        font.pixelSize: 12
                        text: "姿态仪"
                        onClicked: {
                            if (!isRunning) {
                                messageInfoRect.visible = true
                            } else {
                                postureInfoRect.visible = true
                            }
                        }
                        background: Rectangle {
                            anchors.fill: parent
                            radius: 24
                            color: "#FFDBDB"
                            opacity: 0.6
                            border.color: "#FFF"
                        }
                    }
                }

            }

            Rectangle{
                width: 220
                height: 120
                color: "transparent"

                // 提示信息矩形
                Rectangle {
                    id: messageInfoRect
                    visible: false
                    width: 200
                    height: 100
                    color: "#FFFFFF"
                    radius: 8
                    border.color: "#9E9E9E"
                    border.width: 1
                    anchors.fill: parent
                    anchors.margins: 10
                    anchors.topMargin: 20

                    Column {
                        anchors.centerIn: parent
                        spacing: 10
                        Text {
                            text: "提示"
                            font.family: "华文楷体"
                            font.pixelSize: 14
                            color: "#2F4F4F"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            text: "请先运行传感器！"
                            font.family: "华文楷体"
                            font.pixelSize: 12
                            color: "#333333"
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }

            Column{
                spacing: 10
                anchors.fill: parent
                anchors.centerIn: parent
                Rectangle {
                    width: parent.width
                    height:40
                    color: "transparent"  // 透明矩形作为占位元素
                }

                Row{
                    spacing: 10
                    anchors.horizontalCenter: parent.horizontalCenter

                    Rectangle{
                        width: 220
                        height: 120
                        color: "transparent"

                    // 错误提醒矩形
                        Rectangle{
                            id: errorInfoRect
                            visible: false
                            // x: 200
                            // y: 200
                            width: 250
                            height: 150
                            color: "#FFFFFF"
                            radius: 8
                            border.color: "#9E9E9E"
                            border.width: 1
                            anchors.fill: parent
                            anchors.margins: 10
                            anchors.topMargin: 20

                            Image{
                                id: sleep
                                source: "sleep.svg"
                                width: 36
                                height: 36
                                horizontalAlignment: Text.AlignHCenter
                            }
                            Text{
                                text: "请检查当前" + name + "连接状态，错误率超过5%"
                                font.family: "华文楷体"
                                font.pixelSize: 14
                                color: "#FFC300"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }

                    // 电子罗盘信息矩形
                    Rectangle {
                        id: compassInfoRect
                        visible: false
                        width: 250
                        height: 150
                        color: "#FFFFFF"
                        radius: 8
                        border.color: "#9E9E9E"
                        border.width: 1

                        Column {
                            anchors.centerIn: parent
                            spacing: 10
                            Text {
                                text: "电子罗盘信息"
                                font.family: "华文楷体"
                                font.pixelSize: 14
                                color: "#2F4F4F"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }
                            Text {
                                text: "倾斜角: " + compassPitch + "°"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                            Text {
                                text: "横滚角重力工具面角: " + compassRoll + "°"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                            Text {
                                text: "方位角: " + compassHeading + "°"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                        }
                    }

                    // 深度传感器信息矩形
                    Rectangle {
                        id: depthInfoRect
                        visible: false
                        width: 280
                        height: 200
                        color: "#FFFFFF"
                        radius: 8
                        border.color: "#9E9E9E"
                        border.width: 1

                        Column {
                            anchors.centerIn: parent
                            spacing: 10
                            Text {
                                text: "深度传感器信息"
                                font.family: "华文楷体"
                                font.pixelSize: 14
                                color: "#2F4F4F"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }
                            Text {
                                text: "压力: " + depthPressure + " 帕"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                        }
                    }

                    // 姿态仪信息矩形
                    Rectangle {
                        id: postureInfoRect
                        visible: false
                        width: 280
                        height: 200
                        color: "#FFFFFF"
                        radius: 8
                        border.color: "#9E9E9E"
                        border.width: 1

                        Column {
                            anchors.centerIn: parent
                            spacing: 10
                            Text {
                                text: "姿势仪信息"
                                font.family: "华文楷体"
                                font.pixelSize: 14
                                color: "#2F4F4F"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }
                            Text {
                                text: "温度: " + postureTemperate + "度"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                            Text {
                                text: "加速度: " + "(" + postureAccX + "x, " + postureAccY + "y," + postureAccZ + "z） " + "米每平方秒"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                            Text {
                                text: "角速度: " +"(" + postureGyrX + "z, " + postureGyrY + "y, " + postureGyrZ + "z） " + "弧度每秒"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                            Text {
                                text: "磁场归一化: " + "(" + postureNorX + "x, " + postureNorY + "z, " + postureNorZ + "z） " + "磁密度"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                            Text {
                                text: "磁场强度: " + "(" + postureStrX + "x, " + postureStrY + "z, " + postureStrZ + "z） " + "磁密度"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                            Text {
                                text: "欧拉角: " + "(" + postureEluPitch + "x, " + postureEluRoll + "y, " + postureEluYaw + "z） " + "度"
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                            Text {
                                text: "四元数: " + "(" + postureQuaA + ", " + postureQuaB + "i, " + postureQuaC + "j, " + postureQuaD + "k) "
                                font.family: "华文楷体"
                                font.pixelSize: 12
                                color: "#333333"
                            }
                        }
                    }
                }
            }
        }
    }
}

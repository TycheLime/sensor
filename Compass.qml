import QtQuick
import QtQuick.Controls

Rectangle {
    objectName: "compassView"
    width: 400
    height: 290
    color: "#00AAAA"
    opacity: 0.25
    radius: 18
    Label {
        id: titleLabel
        text: "Compass 数据"
        font.pixelSize: 16
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: 10
        }
    }
    Text {
        id: compassDataText
        text: "倾斜角: " + compassPitch + "°\n横滚角重力工具面角: " + compassRoll + "°\n方位角: " + compassHeading + "°"
        font.pixelSize: 12
        anchors {
            top: titleLabel.bottom
            left: parent.left
            leftMargin: 20
            topMargin: 10
        }
    }
    property real compassPitch: 0.0
    property real compassRoll: 0.0
    property real compassHeading: 0.0

    function setSerial(pitch, roll, heading) {
        compassPitch = pitch
        compassRoll = roll
        compassHeading = heading
        compassDataText.text = "倾斜角: " + compassPitch + "°\n横滚角重力工具面角: " + compassRoll + "°\n方位角: " + compassHeading + "°"
    }
}

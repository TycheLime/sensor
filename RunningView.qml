import QtQuick
import QtQuick.Controls

Rectangle {
    objectName: "runningView"
    width: 400
    height: 290
    color: "#00AAAA"
    opacity: 0.25
    radius: 18

    Label {
        id: titleLabel
        text: "基本信息"
        font.pixelSize: 16
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: 10
        }
    }

    Text{

    }
}

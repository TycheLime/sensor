// SensorInfo.qml
import QtQuick
import QtQuick.Controls

Rectangle {
    width: (parent.width - parent.spacing) / 2
    height: 140
    color: "#F2F2F2"
    radius: 10
    border.color: "#9E9E9E"

    // 定义 title 和 infoModel 属性
    property string title: ""
    property var infoModel: []

    Column {
        id: titleColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: 8
        padding: 10

        Text {
            text: title
            font.family: "华文楷体"
            font.pixelSize: 14
            color: "#2F4F4F"
            font.bold: true
        }
    }

    Flickable {
        id: contentFlickable
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: titleColumn.bottom
        anchors.bottom: parent.bottom
        clip: true
        flickableDirection: Flickable.VerticalFlick  // 允许垂直滚动
        contentHeight: contentColumn.height  // 确保内容足够高度来允许滚动

        Column {
            id: contentColumn
            anchors.fill: parent
            spacing: 8
            leftPadding: 10

            Repeater {
                model: infoModel.length > 0 ? infoModel : [{ label: "", value: "" }]
                Row {
                    spacing: 5
                    Text {
                        text: modelData && modelData.label ? modelData.label : ""
                        font.family: "华文楷体"
                        font.pixelSize: 12
                        color: "#333333"
                    }
                    Text {
                        text: modelData && modelData.value ? modelData.value : ""
                        font.family: "华文楷体"
                        font.pixelSize: 12
                        color: "#555555"
                        wrapMode: Text.WordWrap  // 允许文本换行
                    }
                }
            }
        }
    }
}

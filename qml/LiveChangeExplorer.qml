import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    color: "#252526"
    implicitWidth: 900
    implicitHeight: 500

    Text {
        anchors.centerIn: parent

        text: "Live Change Explorer\nQML smoke test"
        color: "#E8E8E8"

        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 22
    }
}

import QtQuick 2.0

Item {
    width: 320
    height: 480

    Text {
        anchors.centerIn: parent
        font.family: "Arial"
        font.pixelSize: 10
        font.overline: true
        textFormat: Text.RichText
        text: "Lorem ipsum <font size=16>dolor sit amet</font>, consectetur."
    }
}

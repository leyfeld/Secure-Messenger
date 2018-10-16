import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.4

Window {
    id:mainWindow
    objectName: "mainWindow"
    visible: true
    width: 640
    height: 480
    property int formNumber: 1;
    Loader {
          id: ldid
          objectName: "ldid"
          anchors.fill: parent
          source: "EnterWindow.qml"

      }
}

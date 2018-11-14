import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.4

Item {
    id: enterwin
    objectName: "enterwin"
    visible: true
    Material.theme: Material.Light
    width: 640
    height: 480
    Connections{
        target: qmlConnection;
        onToMessanger:{
            ldid.source= "MainWindow.qml"
        }
    }
    Image {
        id: iReturn
        anchors.left: parent.left
        anchors.leftMargin: 50
        anchors.top: parent.top
        anchors.topMargin: 30
        height:70
        width:70
        source: "pics/arrow_pic_2.png"
        MouseArea{
            id: maReturn
            anchors.fill: parent
            hoverEnabled: true
            onClicked:
            {
                ldid.source= "EnterWindow.qml"
            }
            onEntered:
            {
                iReturn.source= "pics/arrow_pic_1.png"
            }
            onExited:
            {
                iReturn.source= "pics/arrow_pic_2.png"
            }
        }
      }
    Column
    {
        id:mainColumn
        width: parent.width
        topPadding: Math.max(100, (parent.height)*0.27)
        Text {
            id: txtError
            objectName: "txtError"
            color: "#F44336"
            text:qsTr(" ")
            font.underline: false
            font.bold: false
            font.pixelSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }
        TextField {
                id: ipRegField
                objectName: "ipRegField"
                anchors.horizontalCenter: parent.horizontalCenter
                Material.accent: "#B0BEC5"
                placeholderText: "IP-адрес сервера"
                width: Math.max(100, (parent.width)*0.5)

            }

        TextField {
                id: nameRegField
                objectName: "nameRegField"
                anchors.horizontalCenter: parent.horizontalCenter
                //topPadding: 50
                Material.accent: "#B0BEC5"
                placeholderText: "Ваше имя"
                width: Math.max(100, (parent.width)*0.5)
            }

        TextField {
                id: logRegField
                objectName: "logRegField"
                anchors.horizontalCenter: parent.horizontalCenter
                Material.accent: "#B0BEC5"
                placeholderText: "Придумайте логин"
                width: Math.max(100, (parent.width)*0.5)
            }
        TextField {
                id: pswRegField
                objectName: "pswRegField"
                anchors.horizontalCenter: parent.horizontalCenter
                Material.accent: "#B0BEC5"
                placeholderText: "Придумайте пароль"
                width: Math.max(100, (parent.width)*0.5)

            }

        Button {
                id:btRegEntry
                objectName: "btEntry"
                text: qsTr("Зарегистрироваться")
                width: Math.max(100, (parent.width)*0.3)
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Material.background: "#FFF59D"
                onClicked: {
                  qmlConnection.registrationForm()
                  //ldid.source= "MainWindow.qml" // Вызываем сигнал
                }
            }
    }
}

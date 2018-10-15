
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
       // property int formNumber: 1;
        signal signalExit
        Connections{
            target: qmlConnection;
            onToMessanger:{
                ldid.source= "MainWindow.qml"
            }
        }

        Column
        {
            id:mainColumn
            width: parent.width
            topPadding: Math.max(100, (parent.height)*0.33)
            Item{
                id: item1
                anchors.horizontalCenter: parent.horizontalCenter
                width:Math.max(100, (parent.width)*0.5)
                anchors.top: parent.top
                anchors.topMargin: 50

            Image {
                source: "pics/logo_3.png"
                width: Math.max(100, (parent.width)*0.5)
                height:Math.max(100, (parent.width)*0.5)
                anchors.horizontalCenter: parent.horizontalCenter

            }

            }
            Text {
                id: txtError
                objectName: "txtError"
                color: "#F44336"
                text:qsTr(" ")
                font.underline: false
                font.bold: false
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
                topPadding: 50
            }
            TextField {
                    id: logField
                    objectName: "logField"
                    anchors.horizontalCenter: parent.horizontalCenter
                    topPadding: 10
                    Material.accent: "#B0BEC5"
                    placeholderText: "Введите логин"
                    width: Math.max(100, (parent.width)*0.5)
                }
            TextField {
                    id: pswField
                    objectName: "pswField"
                    anchors.horizontalCenter: parent.horizontalCenter
                    Material.accent: "#B0BEC5"
                    placeholderText: "Введите пароль"
                    width: Math.max(100, (parent.width)*0.5)

                }

            Button {
                    id:btEntry
                    objectName: "btEntry"
                    text: qsTr("Вход")
                    width: Math.max(100, (parent.width)*0.3)
                    height: 50
                    anchors.horizontalCenter: parent.horizontalCenter
                    Material.background: "#FFF59D"

                    onClicked: {
                      mainWindow.formNumber=3
                      qmlConnection.enterForm()
                    }



                }
            Text {
                id: txReg
                property alias txReg: txReg
                color: "#9E9E9E"
                text: qsTr("Регистрация")
                font.underline: false
                font.bold: false
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
                topPadding: 10
                MouseArea {
                    id: mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked:
                    {

                        ldid.source= "RegisterWindow.qml"

                    }

                    onEntered:
                    {
                        txReg.color="#fbd04f"
                    }
                    onExited:
                    {
                        txReg.color= "#9E9E9E"
                    }
                }
            }

        }



    }



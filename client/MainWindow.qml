import QtQuick 2.2
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.0
import QtQuick.Dialogs 1.3


Item {
    id: messageWindow
    visible: true
    width: 640
    height: 480
    property alias lvMessage: lvMessage
    property alias scrollView: scrollView
    //title: qsTr("Messanger")


    Connections{
            target: qmlConnection;
            onToChatList:{

                listModel.append({itemText:log, clrItem:(offOn==true)? "#FFC107" : "#414037"})

            }
            onToMessageList:
            {
                lmMessage.append({txtMessage:mes,
                                     lmMessage:20})
            }
            onToPrevMessageList:{
                lmMessage.append({txtMessage:mes,
                                     lmMessage:(direction=="to")? 400 : 20})
            }
        }

    Component {
        id: highlightBar
        Rectangle {
            width: 200; height: 50
            color: "#EEEEEE"
        }
    }

    TabBar
    {
        id: bar
        width: parent.width
        currentIndex: 0
        Material.background: "#FFF59D"
        Material.accent: "#414037"
        contentWidth: parent.width

         TabButton
         {
                id:btnChatList
                text: qsTr("Chat List")
                width: Math.max(100, bar.width / 2)
                Image{
                    id: iRefresh
                    source: "pics/Refresh_pic_2.png"
                    width: Math.max((parent.height)*0.6)
                    height:Math.max((parent.height)*0.6)
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right:parent.right
                    anchors.rightMargin: 20
                    MouseArea {
                        id: maRefresh
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked:
                        {
                            listModel.clear()
                            qmlConnection.refreshChatList();

                        }

                        onEntered:
                        {
                            iRefresh.source="pics/Refresh_pic_1.png"
                        }
                        onExited:
                        {
                            iRefresh.source= "pics/Refresh_pic_2.png"
                        }
                    }
                }

                onClicked: {
                    lmMessage.clear();
                    btnMessageList.text="Messages List"
                }

         }

         TabButton
         {
                  id: btnMessageList
                  objectName: "btnMessageList"
                  text: qsTr("Messages List")
                  width: Math.max(100, bar.width / 2)
                  onClicked: {
                      bar.currentIndex=0
                  }
         }
   }
    StackLayout
    {
       anchors.top: bar.bottom
       width: parent.width
       currentIndex: bar.currentIndex
       Item
       {
           id: chatList

         ScrollView
         {
           id: scrollView
           anchors.top: parent.top
           width: 640
           height: 438
           ListView
           {
               id:clientListView
               width: 640
               clip: true
               anchors.top: parent.top
               highlight: highlightBar
               highlightFollowsCurrentItem: true
               model: ListModel{
                   id: listModel
               }

               delegate: Item
             {
                 id:listClientItem
                 objectName: "listClientItem"
                 anchors.left: parent.left
                 anchors.right: parent.right
                 height: 40
                Text
                {
                 id: txtItem
                 color: clrItem
                 text: itemText
                 font.underline: false
                 font.bold: false
                 font.pixelSize: 16
                 leftPadding: 20
                 topPadding: 10
                }
                 MouseArea {
                     id: maClientItem
                     anchors.fill: parent
                     hoverEnabled: true
                     onClicked:
                     {

                         bar.currentIndex=1;
                         btnMessageList.text=txtItem.text
                         qmlConnection.messageList(txtItem.text)

                     }
                     onEntered:
                     {
                         clientListView.currentIndex=index
                     }

            
             }

           }


         }
        }
}
       Item
       {
           id: item1
           width: 640
           height: 433

           ScrollView
           {
             id: svMessage
             anchors.top: parent.top
             width: 640
             height: 250
             ListView
             {
                 id:lvMessage
                 width: 640
                 clip: true
                 anchors.top: parent.top
                 anchors.topMargin: 10
                 model: ListModel{
                     id: lmMessage
                 }

                 delegate: Item
               {
                   id:iMessage
                   objectName: "iMessage"
                   anchors.left: parent.left
                   anchors.right: parent.right
                   height: Math.max(txtIMessage.contentHeight * 1.5)
                   //clip:true
                  Rectangle{
                      anchors.left: parent.left;
                      anchors.leftMargin: lmMessage
                      width:220
                      height:Math.max(parent.height*0.85)
                      color:"#EEEEEE"
                      border.color: "#FFF59D"
                      border.width:1
                      radius:10
                    Text
                    {
                        id: txtIMessage
                        width:200
                        wrapMode: Text.Wrap
                        //property alias txtIMessage: txtIMessage
                        color: "#414037"
                        text: txtMessage
                        font.underline: false
                        font.bold: false
                        font.pixelSize: 16
                        topPadding: Math.max((parent.height-contentHeight)/2)
                        leftPadding: 10
                    }
                  }
             }

           }
           }

                   TextField {
                               id: field1
                               objectName: "field1"
                               anchors.left: parent.left
                               anchors.bottom: parent.bottom
                               Material.accent: "#B0BEC5"
                               anchors.bottomMargin: 10
                               anchors.leftMargin: 30
                               placeholderText: "Введите текст"


                anchors.right: parent.right
                anchors.rightMargin: 141
            }

            Text {
                id: filename
                x: 61
                y: 357
                width: 217
                height: 22
                objectName: "filename"
                font.pixelSize: 12
            }

            CheckBox {
                id: checkBox
                y: 342
                height: 42
                text: qsTr("Check Box")
                anchors.right: parent.right
                anchors.rightMargin: 577
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 49
                anchors.left: parent.left
                anchors.leftMargin: 20
                onClicked: {
                    checkBox.checked = false
                    qmlConnection.cancelFile()
                }
            }

            Button {
                id: attachBtn
                y: 385
                height: 40
                anchors.left: parent.left
                anchors.leftMargin: 498
                anchors.right: parent.right
                anchors.rightMargin: 101
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 8
                Material.background: "#FFFFFF"
                Image {
                    id: attachBtnImage
                    width: parent.width*0.7
                    height: parent.height*0.5
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "pics/duck.png"
                }
                onClicked:
                {
                    fileDialog.open()
                }
            }

            FileDialog
            {
                id: fileDialog
                title: "Please choose a file"
                folder: shortcuts.home
                onAccepted:
                {
                    checkBox.checked = true
                    qmlConnection.chooseFile(this.fileUrl)
                }
            }
            Button {
                id: button
                y: 385
                height: 40
                objectName: "button"
                text: qsTr("Send")
                anchors.left: parent.left
                anchors.leftMargin: 539
                Material.background: "#FFF59D"
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 8
                anchors.rightMargin: 8
                onClicked: {
                    checkBox.checked = false
                    qmlConnection.messageForm()
                    lmMessage.append({txtMessage:field1.text,
                                         lmMessage:400})


                }

            }
        }
    }
}


/*##^## Designer {
    D{i:46;anchors_width:60;anchors_x:0}
}
 ##^##*/

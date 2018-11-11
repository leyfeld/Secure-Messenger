
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
                lmMessage.append({txtMessage:mes,txtTime:time,
                                     lmMessage:20})
                lvMessage.positionViewAtEnd()
            }
            onToPrevMessageList:{
                lmMessage.append({txtMessage:mes,txtTime:time,
                                     lmMessage:(direction=="to")? 400 : 20})
                lvMessage.positionViewAtEnd()
            }
            onToGetFile :
            {
                //console.log("emit file sig")
                fDialog.open()
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
        objectName: "bar"
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

         Dialog{
            id: fDialog
            title: "Would you like add new file?"
            standardButtons: Dialog.Ok | Dialog.Cancel
            Label {
                    id: textLabel
                    text: qsTr("Would you like add the new file?")
                    color: "#34aadc"
                    anchors.centerIn: parent // Помещаем сообщение в центре области для сообщения
                  }
                onAccepted:
                {
                    console.log("Ok clicked")
                    qmlConnection.okSendFile()
                }
                onRejected:
                {
                    console.log("Cancel clicked")
                    qmlConnection.cancelFile()
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
             height: 350
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
                   height: Math.max((txtIMessage.contentHeight+txtITime.contentHeight) * 1.5)
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
                        topPadding: Math.max((parent.height-contentHeight-txtITime.contentHeight)/2)
                        leftPadding: 10
                    }
                    Text{
                        id:txtITime
                        width: txtIMessage.width
                        anchors.top:txtIMessage.bottom
                        wrapMode: Text.Wrap
                        color: "#9E9E9E"
                        text: txtTime
                        font.underline: false
                        font.bold: false
                        font.pixelSize: 12
                        leftPadding: 10
                    }
                  }
             }

           }
           }

           ScrollView {
               id: svText
               anchors.left: parent.left
               anchors.bottom: parent.bottom
               anchors.bottomMargin: 8
               rightPadding: 15
               anchors.leftMargin: 30
               width:440
               height:50
               TextArea
              {
                id: field1
                clip: true
                height: 50
                objectName: "field1"
                Material.accent: "#414037"
                wrapMode: Text.Wrap
                placeholderText: "Введите текст"
            }
           }
                Image {
                    id: attachBtnImage
                    anchors.left: svText.right
                    anchors.leftMargin: 10
                    anchors.right: button.left
                    anchors.rightMargin: 10
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    height:44
                    source: "pics/duck.png"
                    MouseArea{
                        id: maAttach
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked:
                        {
                            fileDialog.open()
                        }
                        onEntered:
                        {
                            attachBtnImage.source= "pics/duck_1.png"
                        }
                        onExited:
                        {
                            attachBtnImage.source= "pics/duck.png"
                        }
                    }
                  }
            Text {
                id: filename
                x: 85
                y: 347
                width: 237
                height: 22
                objectName: "filename"
                font.pixelSize: 12
            }

            CheckBox {
                id: checkBox
                y: 327
                Material.accent: "#414037"
                height: 42
                anchors.bottomMargin: 6
                anchors.leftMargin: 0
                anchors.bottom: svText.top
                anchors.left: svText.left
                visible:false;
                onClicked: {
                    qmlConnection.cancelFile()
                    checkBox.visible=false
                    filename.visible=false
                    svMessage.height=350
                }
            }
            FileDialog
            {
                id: fileDialog
                title: "Please choose a file"
                folder: shortcuts.home
                onAccepted:
                {
                    checkBox.visible=true
                    checkBox.checked = true
                    filename.visible=true
                    svMessage.height=325
                    qmlConnection.chooseFile(this.fileUrl)
                }
            }

            Button {
                id: button
                y: 385
                height: 50
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
                    checkBox.visible = false
                    qmlConnection.messageForm()
                    lmMessage.append({txtMessage:field1.text,
                                         lmMessage:400})
                    lvMessage.positionViewAtEnd()
                    field1.text = ""
                    filename.text = ""
                }
            }
            Button {
                id: keybutton
                x: 0
                y: 0
                height: 40
                objectName: "keybutton"
                text: qsTr("key")
                anchors.left: parent.left
                anchors.leftMargin: 598
                Material.background: "#FFF59D"
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 434
                anchors.rightMargin: 0
                onClicked: {
                    keyMessage.open()
                    qmlConnection.getKey()
                }
            }
            Dialog  {
                id: keyMessage
                width: 300
                height: 100
                title: qsTr("This is your key!")
                standardButtons: MessageDialog.Ok
                Label {
                    id: keyLabel
                    color: "#34aadc"
                    text: qsTr("If you see this key on two devices, end-to-end encryption is guaranteed!")
                    anchors.top: parent.top
                    height: parent.height
                    width: parent.width
                    wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
                }
                onAccepted: {
                 keyMessage.close()
                }
                Text {
                    id: key
                    objectName: "key"
                    anchors.centerIn: parent
                    anchors.left: parent.left
                    height: parent.height
                    width: parent.width
                    wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
                    color: "white"
                    verticalAlignment: Text.AlignVCenter

                }

        }
        }
    }
    Rectangle{
        id: rctError
        objectName: "rctError"
        width:messageWindow.width
        height:50
        color:"#FFF59D"
        anchors.top:bar.bottom
        visible:false
        opacity: 0.5
        Text {
            id: txtError
            objectName: "txtError"
            color: "#F44336"
            text:qsTr(" ")
            font.underline: false
            font.bold: false
            font.pixelSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            //topPadding: 50
        }
    }
}


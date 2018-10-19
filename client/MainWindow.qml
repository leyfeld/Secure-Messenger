
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.0



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
                                 lmMessage:(num==true)? 20 : 400})
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
                onClicked: {
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
//                         qmlConnection.messageList(txtItem.text)

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
                 model: ListModel{
                     id: lmMessage
                 }

                 delegate: Item
               {
                   id:iMessage
                   objectName: "iMessage"
                   anchors.left: parent.left
                   anchors.right: parent.right
                   height: 40
                   //clip:true
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
                   anchors.left: parent.left;
                   anchors.leftMargin: lmMessage
                   topPadding: 10
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


                               width: Math.max(100, (parent.width - button.width)*0.9)
                           }
                   Button {
                               id: button
                               objectName: "button"
                               text: qsTr("Send")
                               Material.background: "#FFF59D"
                               anchors.right: parent.right
                               anchors.bottom: parent.bottom
                               anchors.bottomMargin: 10
                               anchors.rightMargin: 10
                               width:Math.max(100, parent.width / 4)
                               onClicked: {
                                   qmlConnection.messageForm()
                                   lmMessage.append({txtMessage:field1.text,
                                                        lmMessage:400})

                               }
                           }

       }
 }



}

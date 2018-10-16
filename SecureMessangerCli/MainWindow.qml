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
    property alias scrollView: scrollView
    //title: qsTr("Messanger")


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
               width: 640
               clip: true
               anchors.top: parent.top

               model: 40
               delegate: ItemDelegate
             {
                 id:listClient
                 objectName: "listClient"
                 anchors.top:Item.top
               //  text: "Item " + (index + 1)
                 width: bar.width
//                 onClicked:{
//                     bar.currentIndex=1
//                     btnMessageList.text="Item " + (index + 1)
//                 }
             }

           }


         }
        }

       Item
       {
           id: item1
           width: 640
           height: 433

                   TextArea {
                               id: textArea
                               objectName: "textArea"
                               anchors.right: parent.right
                               anchors.top: parent.top
                               Material.accent: "#B0BEC5"
                               anchors.topMargin: 10
                               anchors.rightMargin: 10
                               wrapMode: TextArea.Wrap
                               readOnly: true
                               width: 250
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
                               text: qsTr("Отправить")
                               Material.background: "#FFF59D"
                               anchors.right: parent.right
                               anchors.bottom: parent.bottom
                               anchors.bottomMargin: 10
                               anchors.rightMargin: 10
                               width:Math.max(100, parent.width / 4)
                               onClicked: {
                                   qmlConnection.messageForm()
                               }
                           }

       }
 }



}

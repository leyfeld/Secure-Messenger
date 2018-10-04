import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    property alias scrollView: scrollView
    title: qsTr("Messanger")

    Material.theme: Material.Light
    Material.primary: Material.BlueGray
    Material.accent: Material.Teal

    TabBar
    {
         id: bar
         width: parent.width
         currentIndex: 1
         contentWidth: parent.width

         TabButton
         {
                id:home
                text: qsTr("Chat List")
                width: Math.max(100, bar.width / 2)

         }

         TabButton
         {
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
           width: 640
           height: 433
           ListView
           {
               anchors.top: parent.top
               anchors.topMargin: 0
               snapMode: ListView.SnapToItem
               anchors.fill: parent

             model: 20
             delegate: ItemDelegate
             {
                 anchors.top:Item.top
                 text: "Item " + (index + 1)
                 width: bar.width
             }

           }


         }
        }

       Item
       {
           id: item1
           width: 640
           height: 433
//           Column
//           {
//                   spacing: 10
//                   anchors.centerIn: parent

                   TextArea {
                               id: textArea
                               objectName: "textArea"
                               anchors.right: parent.right
                               anchors.top: parent.top
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
                               anchors.bottomMargin: 10
                               anchors.leftMargin: 30
                               placeholderText: "Введите текст"
                               width: Math.max(100, (parent.width - button.width)*0.9)
                           }
                   Button {
                               id: button
                               objectName: "button"
                               text: qsTr("Отправить")
                               anchors.right: parent.right
                               anchors.bottom: parent.bottom
                               anchors.bottomMargin: 10
                               anchors.rightMargin: 10
                               width:Math.max(100, parent.width / 4)
                           }
//           }

       }
 }


}

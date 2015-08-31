import QtQuick 2.0
import QtQuick.Controls 1.3
import TelegramQml 1.0

Item {
    id: stickerlist

    property Telegram telegram
    property alias currentCategory: sticker_model.category

    StickersModel {
        id: sticker_model
        telegram: stickerlist.telegram
    }

    ListView {
        id: catList
        height: parent.height
        width: 200
        model: sticker_model.categories
        delegate: sticker_cat_component
    }

    ScrollView {
        height: parent.height
        anchors.left: catList.right
        anchors.right: parent.right

        GridView {
            anchors.fill: parent
            model: sticker_model
            delegate: sticker_item_component
            cellWidth: 100
            cellHeight: 100
        }
    }



    Component {
        id: sticker_cat_component
        Rectangle {
            id: catItem
            width: 200
            height: 26
            color: currentCategory==category? "#330d80ec" : "#00000000"

            property string category: sticker_model.categories[index]
            property StickerSet stickerSet: sticker_model.categoryItem(category)
            property Document document: sticker_model.categoryThumbnailDocument(category)

            Row {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    width: height
                    sourceSize: Qt.size(width, height)
                    source: handler.downloaded? handler.filePath : handler.thumbPath
                }

                Text {
                    text: catItem.stickerSet.title
                }
            }

            FileHandler {
                id: handler
                target: catItem.document
                telegram: stickerlist.telegram
                Component.onCompleted: download()
            }

            MouseArea {
                id: marea
                anchors.fill: parent
                onClicked: currentCategory = catItem.category
            }
        }
    }

    Component {
        id: sticker_item_component
        Item {
            id: item
            width: 100
            height: 100

            property Document document: model.document

            FileHandler {
                id: handler
                target: item.document
                telegram: stickerlist.telegram
                Component.onCompleted: download()
            }

            Image {
                anchors.fill: parent
                anchors.margins: 4
                height: parent.height
                width: height
                sourceSize: Qt.size(width, height)
                source: handler.downloaded? handler.filePath : handler.thumbPath
            }
        }
    }
}


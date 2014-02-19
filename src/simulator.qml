import QtQuick 2.0
import QtQuick.Controls 1.0
import ProgramViewModel 1.0
import MemoryViewModel 1.0

Rectangle {
  id: mainWindow
  width: 1366
  height: 768
  color: "#1c1c1c"

  Rectangle {
    id: stopRectangle
    x: 8
    y: 8
    width: 100
    height: 100
    color: "#1c1c1c"

    MouseArea {
      id: stopArea
      x: 0
      y: 0
      width: 100
      height: 100

      onClicked: {
        programViewModel.stop()
      }

      Image {
        id: stopImage
        x: 15
        y: 15
        width: 70
        height: 70
        source: "icons/Stop.png"
      }

      onEntered: {
        stopRectangle.color = "#403e41"
      }

      onExited: {
        stopRectangle.color = "#1c1c1c"
      }
    }
  }

  Rectangle {
    id: stepRectangle
    x: 114
    y: 8
    width: 100
    height: 100
    color: "#1c1c1c"

    MouseArea {
      id: stepArea
      x: 0
      y: 0
      width: 100
      height: 100

      onClicked: {
        programViewModel.step()
      }

      Image {
        id: stepImage
        x: 5
        y: 5
        width: 90
        height: 90
        source: "icons/Step.png"
      }

      onEntered: {
        stepRectangle.color = "#403e41"
      }

      onExited: {
        stepRectangle.color = "#1c1c1c"
      }
    }
  }

  Rectangle {
    id: continueRectangle
    x: 220
    y: 8
    width: 100
    height: 100
    color: "#1c1c1c"

    MouseArea {
      id: continueArea
      x: 0
      y: 0
      width: 100
      height: 100

      onClicked: {
        programViewModel.continueExecution()
      }

      Image {
        id: continueImage
        x: 5
        y: 5
        width: 90
        height: 90
        source: "icons/Continue.png"
      }

      onEntered: {
        continueRectangle.color = "#403e41"
      }

      onExited: {
        continueRectangle.color = "#1c1c1c"
      }
    }
  }

  Rectangle {
    id: runRectangle
    x: 326
    y: 8
    width: 100
    height: 100
    color: "#1c1c1c"

    MouseArea {
      id: runArea
      x: 0
      y: 0
      width: 100
      height: 100

      onClicked: {
        programViewModel.run()
      }

      Image {
        id: runImage
        x: 15
        y: 20
        width: 60
        height: 60
        source: "icons/Run.png"
      }

      onEntered: {
        runRectangle.color = "#403e41"
      }

      onExited: {
        runRectangle.color = "#1c1c1c"
      }
    }
  }

  TableView {
    id: instructionTable
    x: 8
    y: 160
    width: 800
    height: 600

    model: instructionModel

    //TableViewColumn {
      //id: breakColumn
      //width: 60
      //title: "Break"
      //resizable: false
      //movable: false
    //}

    //TableViewColumn {
      //id: positionColumn
      //width: 70
      //title: "Position"
      //resizable: false
      //movable: false
    //}

    TableViewColumn {
      id: instructionColumn
      width: 999
      horizontalAlignment: 1
      title: "Instruction"
      resizable: false
      movable: false
    }
  }

  TableView {
    id: memoryTable
    x: 958
    y: 465
    width: 400
    height: 295

    model: memoryModel

    TableViewColumn {
      id: lsbColumn
      horizontalAlignment: 1
      title: "LSB"
      resizable: false
      movable: false
    }

    TableViewColumn {
      id: msbColumn
      horizontalAlignment: 1
      title: "MSB"
      resizable: false
      movable: false
    }
  }

  TableView {
    id: stateTable
    x: 958
    y: 160
    width: 400
    height: 250

    TableViewColumn {
      id: stateListColumn
      horizontalAlignment: 1
      title: "Registers"
      resizable: false
      movable: false
    }

    TableViewColumn {
      id: stateValuesColumn
      horizontalAlignment: 1
      title: "Values"
      resizable: false
      movable: false
    }
  }

  TextEdit {
    id: searchMemoryInput
    x: 958
    y: 439
    width: 160
    height: 20
    color: "#739685"
    text: qsTr("Enter memory location")
    transformOrigin: Item.Center
    font.family: "Verdana"
    visible: true
    font.pixelSize: 12
  }

  Rectangle {
    id: searchMemoryArea
    x: 958
    y: 439
    width: 160
    height: 20
    color: "#333338"
    visible: true
    z: -1
  }

  Rectangle {
    id: displayMemoryRectangle
    x: 1198
    y: 439
    width: 160
    height: 20
    color: "#349dfe"

    MouseArea {
      id: displayMemoryArea
      x: 0
      y: 0
      width: 160
      height: 20
      onClicked: {
        memoryViewModel.displayMemory(searchMemoryInput.text.toString())
      }
    }
  }

  Text {
    id: displayMemoryText
    x: 1198
    y: 439
    width: 160
    height: 20
    text: qsTr("Display")
    font.family: "Verdana"
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    font.pixelSize: 12
  }

  Rectangle {
    id: closeRectangle
    x: 1338
    y: 8
    width: 20
    height: 20
    z: 3
    color: "#1c1c1c"

    MouseArea {
      id: closeArea
      width: 20
      height: 20

      onClicked: {
        Qt.quit()
      }

      Image {
        id: closeImage
        width: 20
        height: 20
        source: "icons/Close.png"
      }
    }
  }
}

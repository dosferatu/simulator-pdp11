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
    model: instructionModel
    x: 8
    y: 160
    width: 800
    height: 600
    alternatingRowColors: false
    backgroundVisible: false
    headerVisible: false
    frameVisible: false

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
    model: memoryModel
    x: 958
    y: 465
    width: 400
    height: 295
    activeFocusOnTab: false
    frameVisible: false
    highlightOnFocus: false
    sortIndicatorVisible: false
    sortIndicatorColumn: 0
    headerVisible: false
    alternatingRowColors: true
    backgroundVisible: false

    TableViewColumn {
      id: dataColumn
      horizontalAlignment: 1
      title: "Value"
      resizable: false
      movable: false
    }
  }

  TextInput {
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

    MouseArea {
      id: searchMemoryArea

      onClicked: {
        searchMemoryInput.text = ""
      }
    }
  }

  Rectangle {
    id: searchMemoryRectangle
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

  // Register fields//{{{
  TextInput {
      id: reg0
      x: 998
      y: 160
      width: 120
      height: 20
      color: "#739685"
      text: memoryViewModel._R0
      MouseArea {
          id: searchMemoryArea1
      }
      font.family: "Verdana"
      font.pixelSize: 12
      transformOrigin: Item.Center
      visible: true
  }

  TextInput {
      id: reg2
      x: 998
      y: 186
      width: 120
      height: 20
      color: "#739685"
      text: memoryViewModel._R2
      MouseArea {
          id: searchMemoryArea2
      }
      font.family: "Verdana"
      font.pixelSize: 12
      visible: true
      transformOrigin: Item.Center
  }

  TextInput {
      id: reg4
      x: 998
      y: 212
      width: 120
      height: 20
      color: "#739685"
      text: memoryViewModel._R4
      MouseArea {
          id: searchMemoryArea3
      }
      font.family: "Verdana"
      font.pixelSize: 12
      transformOrigin: Item.Center
      visible: true
  }

  TextInput {
      id: reg6
      x: 998
      y: 238
      width: 120
      height: 20
      color: "#739685"
      text: memoryViewModel._SP
      MouseArea {
          id: searchMemoryArea4
      }
      font.family: "Verdana"
      font.pixelSize: 12
      transformOrigin: Item.Center
      visible: true
  }

  TextInput {
      id: reg1
      x: 1238
      y: 160
      width: 120
      height: 20
      color: "#739685"
      text: memoryViewModel._R1
      MouseArea {
          id: searchMemoryArea6
      }
      font.family: "Verdana"
      font.pixelSize: 12
      transformOrigin: Item.Center
      visible: true
  }

  TextInput {
      id: reg3
      x: 1238
      y: 186
      width: 120
      height: 20
      color: "#739685"
      text: memoryViewModel._R3
      MouseArea {
          id: searchMemoryArea7
      }
      font.family: "Verdana"
      font.pixelSize: 12
      visible: true
      transformOrigin: Item.Center
  }

  TextInput {
      id: reg5
      x: 1238
      y: 212
      width: 120
      height: 20
      color: "#739685"
      text: memoryViewModel._R5
      MouseArea {
          id: searchMemoryArea8
      }
      font.family: "Verdana"
      font.pixelSize: 12
      visible: true
      transformOrigin: Item.Center
  }

  TextInput {
      id: reg7
      x: 1238
      y: 238
      width: 120
      height: 20
      color: "#739685"
      text: memoryViewModel._PC
      MouseArea {
          id: searchMemoryArea9
      }
      font.family: "Verdana"
      font.pixelSize: 12
      visible: true
      transformOrigin: Item.Center
  }//}}}

  // Register labels//{{{
  Label {
      id: r0Label
      x: 959
      y: 160
      width: 40
      height: 20
      color: "#739685"
      text: qsTr("R0")
  }

  Label {
      id: r2Label
      x: 959
      y: 186
      width: 40
      height: 20
      color: "#739685"
      text: qsTr("R2")
  }

  Label {
      id: r4Label
      x: 959
      y: 212
      width: 40
      height: 20
      color: "#739685"
      text: qsTr("R4")
  }

  Label {
      id: spLabel
      x: 959
      y: 238
      width: 40
      height: 20
      color: "#739685"
      text: qsTr("SP")
  }

  Label {
      id: r1Label
      x: 1198
      y: 160
      width: 40
      height: 20
      color: "#739685"
      text: qsTr("R1")
  }

  Label {
      id: r3Label
      x: 1198
      y: 186
      width: 40
      height: 20
      color: "#739685"
      text: qsTr("R3")
  }

  Label {
      id: r5Label
      x: 1198
      y: 212
      width: 40
      height: 20
      color: "#739685"
      text: qsTr("R5")
  }

  Label {
      id: pcLabel
      x: 1198
      y: 238
      width: 40
      height: 20
      color: "#739685"
      text: qsTr("PC")
  }

  Text {
      id: pcDisplayText
      x: 648
      y: 134
      width: 160
      height: 20
      color: "#739685"
      text: memoryViewModel._PC
      font.family: "Verdana"
      font.pixelSize: 12
  }

  Label {
      id: pcDisplayLabel
      x: 482
      y: 134
      width: 160
      height: 20
      text: qsTr("current instruction:")
      font.family: "Verdana"
  }

  TextInput {
      id: cFlag
      x: 1338
      y: 290
      width: 20
      height: 20
      color: "#739685"
      text: memoryViewModel._C
      transformOrigin: Item.Center
      MouseArea {
          id: searchMemoryArea5
      }
      font.pixelSize: 12
      visible: true
      font.family: "Verdana"
  }

  TextInput {
      id: vFlag
      x: 1312
      y: 290
      width: 20
      height: 20
      color: "#739685"
      text: memoryViewModel._V
      transformOrigin: Item.Center
      MouseArea {
          id: searchMemoryArea10
      }
      font.pixelSize: 12
      visible: true
      font.family: "Verdana"
  }

  TextInput {
      id: zFlag
      x: 1286
      y: 290
      width: 20
      height: 20
      color: "#739685"
      text: memoryViewModel._Z
      transformOrigin: Item.Center
      MouseArea {
          id: searchMemoryArea11
      }
      font.pixelSize: 12
      visible: true
      font.family: "Verdana"
  }

  TextInput {
      id: nFlag
      x: 1260
      y: 290
      width: 20
      height: 20
      color: "#739685"
      text: memoryViewModel._N
      transformOrigin: Item.Center
      MouseArea {
          id: searchMemoryArea12
      }
      font.pixelSize: 12
      visible: true
      font.family: "Verdana"
  }

  Label {
      id: nFlagLabel
      x: 1260
      y: 264
      width: 20
      height: 20
      color: "#739685"
      text: qsTr("N")
      font.family: "Verdana"
  }

  Label {
      id: cFlagLabel
      x: 1338
      y: 264
      width: 20
      height: 20
      color: "#739685"
      text: qsTr("C")
  }

  Label {
      id: vFlagLabel
      x: 1312
      y: 264
      width: 20
      height: 20
      color: "#739685"
      text: qsTr("V")
  }

  Label {
      id: zFlagLabel
      x: 1286
      y: 264
      width: 20
      height: 20
      color: "#739685"
      text: qsTr("Z")
  }//}}}
}

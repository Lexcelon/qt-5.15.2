import QtQuick 2.0
import QtQml.Models 2.12
import tst_qquickvisualdatamodel 1.0

ListView {
    width: 100
    height: 100
    model: visualModel.parts.package
    DelegateModel {
        id: visualModel
        objectName: "visualModel"

        groups: [
            DelegateModelGroup { id: visibleItems; objectName: "visibleItems"; name: "visible"; includeByDefault: true },
            DelegateModelGroup { id: selectedItems; objectName: "selectedItems"; name: "selected" }
        ]

        model: [
            "one",
            "two",
            "three",
            "four"
        ]

        delegate: Package {
            id: delegate

            property variant test1: index
            property variant test2: model.index
            property variant test3: modelData
            property variant test4: model.modelData

            function setTest3(arg) { modelData = arg }
            function setTest4(arg) { model.modelData = arg }

            Item {
                objectName: "delegate"

                width: 100
                height: 2

                Package.name: "package"
            }
        }
    }
}

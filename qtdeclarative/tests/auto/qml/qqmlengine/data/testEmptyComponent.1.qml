import QtQuick 2.0

Item {
    property bool success: false
    required property QtObject componentCache

    function reportError(s) { console.warn(s) }

    Component.onCompleted: {
        componentCache.trim()
        if (!componentCache.isTypeLoaded('testEmptyComponent.1.qml')) return reportError('Test component not loaded')
        if (componentCache.isTypeLoaded('EmptyComponent.qml')) return reportError('Empty component already loaded')

        var comp = Qt.createComponent('EmptyComponent.qml')
        componentCache.trim()
        if (!componentCache.isTypeLoaded('testEmptyComponent.1.qml')) return reportError('Test component not loaded 2')
        if (!componentCache.isTypeLoaded('EmptyComponent.qml')) return reportError('Empty component not loaded')

        var obj = comp.createObject()
        if (!obj) return reportError('Invalid object')
        if (obj.x == undefined) return reportError('Invalid object 2')

        obj.destroy()
        componentCache.trim()
        if (!componentCache.isTypeLoaded('testEmptyComponent.1.qml')) return reportError('Test component not loaded 3')
        if (!componentCache.isTypeLoaded('EmptyComponent.qml')) return reportError('Empty component already unloaded')

        comp.destroy()
        componentCache.trim()
        if (!componentCache.isTypeLoaded('testEmptyComponent.1.qml')) return reportError('Test component not loaded 4')
        if (componentCache.isTypeLoaded('EmptyComponent.qml')) return reportError('Empty component not unloaded')

        success = true
    }
}

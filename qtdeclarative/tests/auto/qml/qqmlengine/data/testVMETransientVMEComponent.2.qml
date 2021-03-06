import QtQuick 2.0

Item {
    property bool success: false
    required property QtObject componentCache

    function reportError(s) { console.warn(s) }

    Component.onCompleted: {
        componentCache.trim()
        if (!componentCache.isTypeLoaded('testVMETransientVMEComponent.2.qml')) return reportError('Test component not loaded')
        if (componentCache.isTypeLoaded('VMETransientVMEComponent.qml')) return reportError('Transient component already loaded')
        if (componentCache.isTypeLoaded('VMEComponent.qml')) return reportError('VME component already loaded')

        var comp = Qt.createComponent('VMETransientVMEComponent.qml')
        componentCache.trim()
        if (!componentCache.isTypeLoaded('testVMETransientVMEComponent.2.qml')) return reportError('Test component not loaded 2')
        if (!componentCache.isTypeLoaded('VMETransientVMEComponent.qml')) return reportError('Transient component not loaded')
        if (componentCache.isTypeLoaded('VMEComponent.qml')) return reportError('VME component already loaded')

        var obj = comp.createObject()
        if (!obj) return reportError('Invalid object')
        if (obj.x == undefined) return reportError('Invalid object 2')
        if (!obj.p) return reportError('Invalid object 3')
        if (obj.p.foo != 'bar') return reportError('Invalid object 4')
        if (!componentCache.isTypeLoaded('VMEComponent.qml')) return reportError('VME component not loaded')

        comp.destroy()
        componentCache.trim()
        if (!componentCache.isTypeLoaded('testVMETransientVMEComponent.2.qml')) return reportError('Test component not loaded 3')
        if (!componentCache.isTypeLoaded('VMETransientVMEComponent.qml')) return reportError('Transient component already unloaded')
        if (!componentCache.isTypeLoaded('VMEComponent.qml')) return reportError('VME component already unloaded')
        if (!obj) return reportError('Invalid object 5')
        if (obj.x == undefined) return reportError('Invalid object 6')
        if (!obj.p) return reportError('Invalid object 7')
        if (obj.p.foo != 'bar') return reportError('Invalid object 8')

        obj.destroy()
        componentCache.trim()
        if (!componentCache.isTypeLoaded('testVMETransientVMEComponent.2.qml')) return reportError('Test component not loaded 4')
        if (componentCache.isTypeLoaded('VMETransientVMEComponent.qml')) return reportError('Transient component not unloaded')
        if (componentCache.isTypeLoaded('VMEComponent.qml')) return reportError('VME component not unloaded')

        success = true
    }
}

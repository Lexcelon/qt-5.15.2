import QtQuick 2.0

Rectangle {
    property bool condition1: false
    property bool condition2: false
    property bool condition3: false

    states: [
        State { name: "state1"; when: condition1 },
        State { name: "state2"; when: condition2 },
        State { name: "state1"; when: condition3 }
    ]
}

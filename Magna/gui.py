from PyQt5 import QtGui, QtWidgets, uic
import sys
import serial_manager


def estop(self):
    serial_manager.send()
    print("estop function")


class Ui(QtWidgets.QDialog):
    def __init__(self):
        super(Ui, self).__init__()
        uic.loadUi('MainWindow.ui', self)

        self.auto_button_estop = self.findChild(QtWidgets.QPushButton, 'auto_button_arm')
        self.auto_button_estop.clicked.connect(estop)

        self.hardware_listview_ports = self.findChild(QtWidgets.QListView, 'hardware_listview_ports')
        #self.hardware_listview_ports.selectionModel().selectionChanged.connect(self.hardware_listview_ports())

        self.hardware_listview_portInfo = self.findChild(QtWidgets.QListView, 'hardware_listview_portInfo')

        self.show()

    def update_ports_list(self):
        model_portsList = QtGui.QStandardItemModel()
        self.hardware_listview_ports.setModel(model_portsList)

        #entries = ['one', 'two', 'three']
        ports_list = serial_manager.get_serial_ports()

        for i in ports_list:
            port_string = i[0] + " - " + i[1]
            item = QtGui.QStandardItem(port_string)
            model_portsList.appendRow(item)

    def view_port_info(self):
        model_portsInfo = QtGui.QStandardItemModel()
        self.hardware_listview_portInfo.setModel(model_portsInfo)

        # entries = ['one', 'two', 'three']
        port_info = serial_manager.get_serial_ports()

        for i in port_info:
            port_string = i[0] + " - " + i[1]
            item = QtGui.QStandardItem(port_string)
            model_portsInfo.appendRow(item)

app = QtWidgets.QApplication(sys.argv)
window = Ui()
Ui.update_ports_list(window)
app.exec_()

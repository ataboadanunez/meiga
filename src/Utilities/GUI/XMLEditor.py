import xml.etree.ElementTree as ET
from PyQt5.QtWidgets import QHBoxLayout, QVBoxLayout, QApplication, QMainWindow, QLabel, QLineEdit, QPushButton, QFileDialog, QComboBox, QListWidget, QWidget, QListView, QInputDialog, QMessageBox, QTableWidget, QTableWidgetItem, QSpacerItem, QSizePolicy, QCheckBox
from PyQt5.QtCore import QRect, Qt
from PyQt5.QtGui import *
import warnings
from Utils import *

from IPython import embed

warnings.warn("CHANGE PATH TO INCLUDE DIRECTORY")
DETECTOR_H_PATH = '/home/alvaro/projects/dev_meiga/install/include/Detector.h'
DETECTOR_ENUM = extract_enum_elements(DETECTOR_H_PATH, 'DetectorType')
SIMDATA_H_PATH = '/home/alvaro/projects/dev_meiga/install/include/SimData.h'
INJECTION_ENUM = extract_enum_elements(SIMDATA_H_PATH, 'InjectionMode')


class DetectorListEditorWindow(QMainWindow):
    def __init__(self, file_path):
        super().__init__()

        self.file_path = file_path
        self.DO_NOT_SHOW_AGAIN = False
        
        self.setWindowTitle("DetectorList Editor")
        self.setGeometry(100, 100, 600, 800)

        # create central widget and main layout
        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)

        # Create top layout for Injection settings
        top_layout = QHBoxLayout()
        
        # Injection Mode Settings
        # -----------------------
        self.injection_label = QLabel("----- Injection Settings -----", self)
        self.injection_label.setFont(QFont('Times', 14))

        self.injection_label.setGeometry(50, 10, 500, 40)
        self.injection_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.injection_label.setAlignment(Qt.AlignCenter)

        self.injection_mode_label = QLabel("Injection mode", self)
        self.injection_mode_label.setGeometry(50, 50, 150, 30)
        self.injection_mode_edit = QComboBox(self)
        self.injection_mode_edit.addItems(INJECTION_ENUM)
        self.injection_mode_edit.setGeometry(150, 50, 200, 30)
        top_layout.addWidget(self.injection_mode_label)
        top_layout.addWidget(self.injection_mode_edit)
        # Origin of Coordinates label
        self.injection_position_label = QLabel("Origin Coord. (X, Y, Z)", self)
        self.injection_position_label.setGeometry(50, 100, 150, 30)
        #  Origin of Coordinates values
        self.injection_posX_edit = QLineEdit(self)
        self.injection_posX_edit.setGeometry(200, 100, 30, 30)

        self.injection_posY_edit = QLineEdit(self)
        self.injection_posY_edit.setGeometry(250, 100, 30, 30)

        self.injection_posZ_edit = QLineEdit(self)
        self.injection_posZ_edit.setGeometry(300, 100, 30, 30)
        top_layout.addWidget(self.injection_position_label)
        top_layout.addWidget(self.injection_posX_edit)
        top_layout.addWidget(self.injection_posY_edit)
        top_layout.addWidget(self.injection_posZ_edit)

        #  Origin of Coordinates unit
        self.injection_pos_unit_label = QLabel("Unit", self)
        self.injection_pos_unit_label.setGeometry(350, 100, 50, 30)
        top_layout.addWidget(self.injection_pos_unit_label)
        self.injection_pos_unit_edit = QComboBox(self)
        self.injection_pos_unit_edit.addItems(["m", "cm", "mm"])
        self.injection_pos_unit_edit.setGeometry(390, 100, 60, 30)
        top_layout.addWidget(self.injection_pos_unit_edit)

        # Radius and Height (for Circle / HalfSphere injection)
        self.injection_radius_label = QLabel("Radius", self)
        self.injection_radius_label.setGeometry(50, 150, 60, 30)
        top_layout.addWidget(self.injection_radius_label)
        self.injection_radius_edit = QLineEdit(self)
        self.injection_radius_edit.setGeometry(100, 150, 50, 30)
        top_layout.addWidget(self.injection_radius_edit)
        self.injection_radius_unit_label = QLabel("Unit", self)
        self.injection_radius_unit_label.setGeometry(160, 150, 50, 30)
        top_layout.addWidget(self.injection_radius_unit_label)
        self.injection_radius_unit_edit = QComboBox(self)
        self.injection_radius_unit_edit.addItems(["m", "cm", "mm"])
        self.injection_radius_unit_edit.setGeometry(200, 150, 50, 30)
        top_layout.addWidget(self.injection_radius_unit_edit)

        self.injection_height_label = QLabel("Height", self)
        self.injection_height_label.setGeometry(50, 200, 60, 30)
        top_layout.addWidget(self.injection_height_label)
        self.injection_height_edit = QLineEdit(self)
        self.injection_height_edit.setGeometry(100, 200, 50, 30)
        top_layout.addWidget(self.injection_height_edit)
        self.injection_height_unit_label = QLabel("Unit", self)
        self.injection_height_unit_label.setGeometry(160, 200, 50, 30)
        top_layout.addWidget(self.injection_height_unit_label)
        self.injection_height_unit_edit = QComboBox(self)
        self.injection_height_unit_edit.addItems(["m", "cm", "mm"])
        self.injection_height_unit_edit.setGeometry(200, 200, 50, 30)
        top_layout.addWidget(self.injection_height_unit_edit)

        # create bottom layout for detector settings
        bottom_layout = QHBoxLayout()

        # Detector Settings
        # -----------------
        self.detector_label = QLabel("----- Detector Settings -----", self)
        self.detector_label.setGeometry(50, 250, 500, 40)
        self.detector_label.setFont(QFont('Times', 14))

        self.detector_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.detector_label.setAlignment(Qt.AlignCenter)
        

        self.detector_list = self.create_detector_list()
        #bottom_layout.addWidget(self.detector_list)
        # self.detector_list.setGeometry(50, 250, 300, 300)
        self.load_xml_data()
        
        self.save_button = QPushButton("Save changes", self)
        self.save_button.setGeometry(150, 700, 200, 30)
        self.save_button.clicked.connect(self.save_changes)

        #bottom_layout.addWidget(self.save_button)
        #main_layout.addLayout(top_layout)
        #main_layout.addStretch(1)
        #main_layout.addLayout(bottom_layout)

    def handle_label_selection(self, text): 
        # Handle the selected label
        print(f'Selected Label: {text}')

    def detector_editor(self):
        tree = ET.parse(self.file_path)
        root = tree.getroot()

        currentIndex = self.listwidget.currentRow()
        item = self.listwidget.item(currentIndex)
        if item is not None:
            # open detector editor
            detectorsTag = root.findall("detector")

            # get detector ID and Type to find data in XML
            s = item.text()
            open_index = s.index('(')
            close_index = s.index(')')
            det_type = s[:open_index].strip()
            det_id   = int(s[open_index+1:close_index].strip())
            # move to that detector tag and load properties
            detectorTag = detectorsTag[det_id]
            # list of properties in file
            properties = detectorTag.getchildren()

            # Create window for Detector Editor
            self.window = QMainWindow()
            self.window.setWindowTitle('%s (%i) Editor' %(det_type, det_id))
            self.window.setGeometry(350, 350, 400, 600)

            detector_widget = QWidget(self.window)
            self.window.setCentralWidget(detector_widget)

            # Create a horizontal layout for the main widget
            main_layout = QVBoxLayout(detector_widget)

            # Create horizontal layout for the position widget
            top_layout = QHBoxLayout()
            self.detector_pos_label = QLabel("Position (X, Y, Z)", self)
            self.detector_pos_label.setGeometry(50, 100, 150, 30)
            
            #  Position coordinates values
            self.detector_posX_edit = QLineEdit(self)
            self.detector_posX_edit.setGeometry(200, 100, 30, 30)

            self.detector_posY_edit = QLineEdit(self)
            self.detector_posY_edit.setGeometry(250, 100, 30, 30)

            self.detector_posZ_edit = QLineEdit(self)
            self.detector_posZ_edit.setGeometry(300, 100, 30, 30)

            # Load default values
            def_posX = detectorTag.find("x").text
            if def_posX is not None:
                self.detector_posX_edit.setText(def_posX)

            def_posY = detectorTag.find("y").text
            if def_posY is not None:
                self.detector_posY_edit.setText(def_posY)

            def_posZ = detectorTag.find("z").text
            if def_posZ is not None:
                self.detector_posZ_edit.setText(def_posZ)

            # Add labels and edits to top layout
            top_layout.addWidget(self.detector_pos_label)
            top_layout.addWidget(self.detector_posX_edit)
            top_layout.addWidget(self.detector_posY_edit)
            top_layout.addWidget(self.detector_posZ_edit)

            allowed_units = ["mm", "cm", "m"]

            def_pos_unit = detectorTag.find('x').get('unit')
            if def_pos_unit is not None:
                # sort units such that default appears first
                sorted_units = [def_pos_unit]
                for un in allowed_units:
                    if un != def_pos_unit:
                        sorted_units.append(un)
            else:
                sorted_units = allowed_units
            
            #  Position Coordinates unit
            self.detector_pos_unit_label = QLabel("Unit", self)
            self.detector_pos_unit_label.setGeometry(350, 100, 50, 30)
            top_layout.addWidget(self.detector_pos_unit_label)
            self.detector_pos_unit_edit = QComboBox(self)
            self.detector_pos_unit_edit.addItems(sorted_units)
            self.detector_pos_unit_edit.setGeometry(390, 100, 60, 30)
            top_layout.addWidget(self.detector_pos_unit_edit)

            # Create a vertical layout for the table widget
            left_layout = QVBoxLayout()

            # Create table Widget for detector properties
            table_widget = QTableWidget(self.window)
            table_widget.setColumnCount(3)
            table_widget.setHorizontalHeaderLabels(['Property', 'Unit', 'Value'])
            table_widget.setSelectionBehavior(QTableWidget.SelectRows)
            
            detector_pos_values = {
                                    'x' : self.detector_posX_edit, 
                                    'y' : self.detector_posY_edit,
                                    'z' : self.detector_posZ_edit}

            table_widget.setRowCount(len(properties) - len(detector_pos_values))
            # the following loop loads properties given in the XML file
            for p, prop in enumerate(properties):
                
                # treat position parameters separately
                if prop.tag in detector_pos_values.keys():
                    continue
                else:
                    
                    prop_label = QTableWidgetItem(prop.tag)
                    prop_unit = QTableWidgetItem(prop.get('unit'))
                    prop_value = QTableWidgetItem(prop.text)

                    table_widget.setItem(p, 0, prop_label)
                    table_widget.setItem(p, 1, prop_unit)
                    table_widget.setItem(p, 2, prop_value)
            
            left_layout.addWidget(table_widget)

            # Create horizontal layout for adding new properties button
            new_label_layout = QVBoxLayout()
            # new_label_layout.setGeometry(QRect(50, 50, 300, 30))
            # Create a combo box for selecting a new label
            # NOTE: this must be read from 'DetectorProperties.xml'
            warnings.warn("READ PROPERTIES FROM DetectorProperties.xml FILE")

            newProperties = ['numberOfBars', 'numberOfPanels', 'barLength', 'barWidth', 'barThickness', 'coatingThickness', 'casingThickness', 'rotationAngle', 'fiberLength','fiberRadius', 'claddingThickness', 'length', 'width', 'thickness', 'distancePanels']

            combo_box = QComboBox(self.window)
            combo_box.addItems(newProperties)
            #combo_box.setGeometry(400, 100, 200, 30)
            new_label_layout.addWidget(combo_box)

            # Create a button for adding the selected label
            add_button = QPushButton('Add property', self.window)
            add_button.clicked.connect(lambda: self.add_label(table_widget, combo_box.currentText()))
            new_label_layout.addWidget(add_button)

            # Create a button for clearing the last added row
            clear_button = QPushButton('Clear property', self.window)
            clear_button.clicked.connect(lambda: self.clear_selected_row(table_widget))
            new_label_layout.addWidget(clear_button)

            # Add a save changes button
            save_button = QPushButton('Save changes', self.window)
            save_button.clicked.connect(lambda: self.save_detector_changes(table_widget, detectorTag, det_id))
            new_label_layout.addWidget(save_button)

            # Spacer between layouts
            spacer = QSpacerItem(0, 0, QSizePolicy.Expanding, QSizePolicy.Expanding)
            new_label_layout.addItem(spacer)

            # Add the new label widget layout to the main layout
            main_layout.addLayout(top_layout)
            left_layout.addLayout(new_label_layout)
            main_layout.addLayout(left_layout)
            

            self.window.show()
            
        else:
            QMessageBox.warning(None, "Warning", "Select a detector from the list.")

    def add_label(self, table_widget, new_label):
        
        if not self.DO_NOT_SHOW_AGAIN:
            msg_box = QMessageBox()
            msg_box.setWindowTitle('Warning')
            msg_box.setText('This property might not be implemented in the current detector description.')
            msg_box.setIcon(QMessageBox.Warning)

            # Add a checkbox to the message box
            check_box = QCheckBox('Do not show this message again')
            msg_box.setCheckBox(check_box)

            # Executing the message box and checking the checkbox state
            result = msg_box.exec_()
            if check_box.isChecked():
                self.DO_NOT_SHOW_AGAIN = True
        
        # Get the current row count
        current_row_count = table_widget.rowCount()
        # Insert a new row at the end of the table
        table_widget.insertRow(current_row_count)
        # Create a new item with the new label
        item = QTableWidgetItem(new_label) 
        # Insert to the the table
        table_widget.setItem(current_row_count, 0, item)
    
    def clear_selected_row(self, table_widget):
        selected_indexes = table_widget.selectionModel().selectedRows()
        if len(selected_indexes) == 0:
            QMessageBox.warning(
                        self,
                        'No property selected',
                        'Please select a property from the table to be removed.')
        else:
            button = QMessageBox.question(
                                self,
                                'Confirmation',
                                'Are you sure that you want to remove this property?',
                                QMessageBox.StandardButton.Yes | 
                                QMessageBox.StandardButton.No)
            
            if button == QMessageBox.StandardButton.Yes:            
                for index in sorted(selected_indexes, reverse=True):
                    table_widget.removeRow(index.row())

    def insert_detector(self, text):
        """
            Inserts a new detector in the detector list
        """
        currentItems = self.listwidget.count()
        # insert the new detector at row = currentItems
        self.listwidget.insertItem(currentItems, "%s (%s)" %(text, currentItems))
        # add the new (empty) detector to the XML for editing
        tree = ET.parse(self.file_path)
        root = tree.getroot()
        new_element = ET.Element("detector")
        new_element.set('id', str(currentItems))
        new_element.set('type', text)

        # add position coordinates
        x_item = ET.SubElement(new_element, "x")
        x_item.set('unit', 'cm')
        x_item.text = '0.0'
        y_item = ET.SubElement(new_element, "y")
        y_item.set('unit', 'cm')
        y_item.text = '0.0'
        z_item = ET.SubElement(new_element, "z")
        z_item.set('unit', 'cm')
        z_item.text = '0.0'
        
        # NOTE: add pretty indentation
        root.append(new_element)
        tree.write(self.file_path)
        self.statusBar().showMessage("Detector added successfully.")

    def remove_detector(self, listwidget):
        """
            Remove detector(s) from list
        """

        selected_indexes = listwidget.selectionModel().selectedRows()

        if len(selected_indexes) == 0:
            QMessageBox.warning(
                        self,
                        'No detector selected',
                        'Please select a detector from the list to be removed.')
        else:
            
            button = QMessageBox.question(
                                self,
                                'Confirmation',
                                'Are you sure that you want to remove this detector?',
                                QMessageBox.StandardButton.Yes | 
                                QMessageBox.StandardButton.No)
            
            if button == QMessageBox.StandardButton.Yes:            
                for index in sorted(selected_indexes, reverse=True):
                    currentIndex = listwidget.currentRow()
                    
                    item = listwidget.takeItem(currentIndex)
                    if item is not None:
                        
                        # enter XML and delete detector tag
                        tree = ET.parse(self.file_path)
                        root = tree.getroot()

                        detectorsTag = root.findall("detector")

                        # get detector ID and Type to find data in XML
                        s = item.text()
                        open_index = s.index('(')
                        close_index = s.index(')')
                        det_type = s[:open_index].strip()
                        det_id   = s[open_index+1:close_index].strip()
                        
                        for element in root.iter("detector"):
                            if element.attrib.get("id") == det_id and element.attrib.get("type") == det_type:
                                root.remove(element)
                        tree.write(self.file_path)
                        del item
                        self.statusBar().showMessage("Detector removed successfully.")

                    

    def create_detector_list(self):
        """
            Creates widget with the detector list
            containing current detectors in XML file.
            Allows editing, adding and deleting detectors
            from the list.
        """
        tree = ET.parse(self.file_path)
        root = tree.getroot()
        
        if root.tag != 'detectorList':
            # raise message box instead
            QMessageBox.warning(None, "Invalid file", "File must contain a `detectorList` branch. Got `%s` instead." %root.tag)

        # get all detector tags in the file
        detectorsTag = root.findall("detector")
        # create list widget
        self.listwidget = QListWidget(self)
        for detector in detectorsTag:
            det_id = detector.get("id")
            det_type = detector.get("type")
            self.listwidget.insertItem(int(det_id), "%s (%s)" %(det_type, det_id))

        self.listwidget.setGeometry(50, 300, 200, 150)
        self.listwidget.setLayoutMode(QListView.Batched)
            
        # add buttons 
        self.edit_button = QPushButton("Edit detector", self)
        self.edit_button.setGeometry(280, 300, 100, 30)
        self.edit_button.setObjectName("edit_button")
        self.edit_button.clicked.connect(self.detector_editor)

        self.add_button = QPushButton("Add detector", self)
        self.add_button.setGeometry(280, 340, 100, 30)
        self.add_combo_box = QComboBox(self)
        self.add_combo_box.addItems(DETECTOR_ENUM)
        self.add_combo_box.setGeometry(390, 340, 100, 30)
        self.add_button.clicked.connect(lambda: self.insert_detector(self.add_combo_box.currentText()))
        
        self.remove_button = QPushButton("Remove detector", self)
        self.remove_button.setGeometry(280, 380, 130, 30)
        self.remove_button.clicked.connect(lambda: self.remove_detector(self.listwidget))

        return self.listwidget

    
    def save_detector_changes(self, table_widget, detectorTag, det_id):
        """
            Function to save current detector changes
        """

        # Save position coordinates
        new_detector_posX = self.detector_posX_edit.text()
        new_detector_posY = self.detector_posY_edit.text()
        new_detector_posZ = self.detector_posZ_edit.text()
        new_detector_pos_unit = self.detector_pos_unit_edit.currentText()
        
        detectorTag.find('x').text = new_detector_posX
        detectorTag.find('y').text = new_detector_posY
        detectorTag.find('z').text = new_detector_posZ

        detectorTag.find('x').set('unit', new_detector_pos_unit)
        detectorTag.find('y').set('unit', new_detector_pos_unit)
        detectorTag.find('z').set('unit', new_detector_pos_unit)

        # Get current table data
        table_data = []
        for row in range(table_widget.rowCount()):
            row_data = []
            for column in range(table_widget.columnCount()):
                item = table_widget.item(row, column)
                if item is not None:
                    row_data.append(item.text())
                else:
                    row_data.append("")
            table_data.append(row_data)
        
        for row in table_data:
            row_label = row[0]
            row_unit  = row[1]
            row_value = row[2]
            
            # check if current row was already in file
            if detectorTag.find(row_label) is not None:
                element = detectorTag.find(row_label)
                # replace unit and value
                if row_unit != '':
                    element.set('unit', row_unit)

                element.text = row_value
            # Add new entry to the detector branch
            else:
                new_element = ET.Element(row_label)
                if row_unit != '':
                    new_element.set('unit', row_unit)
                new_element.text = row_value
                detectorTag.append(new_element)

        # save changes to the corresponding detector branch in the XML file
        tree = ET.parse(self.file_path)
        root = tree.getroot()
        # append modified detector element to the root element
        root.append(detectorTag)
        # search the original detector element (by its id) and remove it
        root.remove(root.findall('detector')[det_id])
        tree.write(self.file_path)

        # Show a message box to indicate the successful save
        msg_box = QMessageBox()
        msg_box.setWindowTitle("Save Changes")
        msg_box.setText("Changes saved successfully.")
        msg_box.exec_()


    def load_xml_data(self):
        
        tree = ET.parse(self.file_path)
        root = tree.getroot() # <detectorList>

        if root.tag != 'detectorList':
            # raise message box instead
            raise NameError("Invalid file. File must contain a 'detectorList' branch. Got %s instead." %root.tag)
        
        # Injection Mode branch
        injectionMode = root.find("injectionMode") # <injectionMode>
        # add exceptions / pop-up messages if tags are not in file
        inj_posX = injectionMode.find("x").text
        if inj_posX is not None:
            self.injection_posX_edit.setText(inj_posX)

        inj_posY = injectionMode.find("y").text
        if inj_posY is not None:
            self.injection_posY_edit.setText(inj_posY)
        
        inj_posZ = injectionMode.find("z").text
        if inj_posZ is not None:
            self.injection_posZ_edit.setText(inj_posZ)

        inj_radius = injectionMode.find('radius').text
        if inj_radius is not None:
            self.injection_radius_edit.setText(inj_radius)

        inj_height = injectionMode.find('height').text
        if inj_height is not None:
            self.injection_height_edit.setText(inj_height)



    def save_changes(self):
        
        # Injection Settings
        new_injection_mode = self.injection_mode_edit.currentText()
        new_injection_posX = self.injection_posX_edit.text()
        new_injection_posY = self.injection_posY_edit.text()
        new_injection_posZ = self.injection_posZ_edit.text()
        new_injection_pos_unit = self.injection_pos_unit_edit.currentText()
        new_injection_radius = self.injection_radius_edit.text()
        new_injection_height = self.injection_height_edit.text()
        new_injection_radius_unit = self.injection_radius_unit_edit.currentText()
        new_injection_height_unit = self.injection_height_unit_edit.currentText()

        if self.file_path:
            tree = ET.parse(self.file_path)
            root = tree.getroot()

            # Modifications on injectionMode branch
            # -------------------------------------
            injectionMode = root.find('injectionMode')
            
            # injection type
            injectionMode.set('type', new_injection_mode)
            #injection position coordinates
            injectionMode.find('x').text = new_injection_posX
            injectionMode.find('y').text = new_injection_posY
            injectionMode.find('z').text = new_injection_posZ
            # All coordinates with same unit
            injectionMode.find('x').set('unit', new_injection_pos_unit)
            injectionMode.find('y').set('unit', new_injection_pos_unit)
            injectionMode.find('z').set('unit', new_injection_pos_unit)

            # Injection Radius and Height
            injectionMode.find('radius').text = new_injection_radius
            injectionMode.find('height').text = new_injection_height
            injectionMode.find('radius').set('unit', new_injection_radius_unit)
            injectionMode.find('height').set('unit', new_injection_height_unit)



            tree.write(self.file_path)
            self.statusBar().showMessage("Changes saved successfully.")
    
# GUI for Meiga Applications
# Author: a. taboada
# Date: April 2023

import json
import xml.etree.ElementTree as ET

from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QLineEdit, QPushButton, QComboBox, QFileDialog

import os
import subprocess

from XMLEditor import DetectorListEditorWindow

from IPython import embed

def word_to_bool(word):
  return (word == "Yes")


class ConfigEditorWindow(QMainWindow):

    def __init__(self):
        super().__init__()
        self.setWindowTitle("Meiga - GUI")
        self.setGeometry(100, 300, 800, 600)

        ################################################
        # Configuration settings  
        ################################################
        
        # Input Settings
        # --------------
        
        # Input File
        self.input_file_label = QLabel("Input file")
        self.input_file_label.setGeometry(50, 50, 100, 30)
        self.input_file_label.setParent(self)
        self.input_file_edit = QLineEdit(self)
        self.input_file_edit.setGeometry(150, 50, 200, 30)

        # Input Mode
        self.input_mode_label = QLabel("Input mode")
        self.input_mode_label.setGeometry(50, 90, 200, 30)
        self.input_mode_label.setParent(self)
        self.input_mode_edit = QComboBox(self)
        self.input_mode_edit.addItems(["UseARTI", "UseEcoMug"])
        self.input_mode_edit.setGeometry(150, 90, 200, 30)
        

        # Input Number of particles
        self.input_npart_label = QLabel("Number of particles")
        self.input_npart_label.setGeometry(50, 130, 140, 30)
        self.input_npart_label.setParent(self)
        self.input_npart_edit = QLineEdit(self)
        self.input_npart_edit.setGeometry(190, 130, 100, 30)

        # Output Settings
        # --------------

        # Output file
        self.output_file_label = QLabel("Output file")
        self.output_file_label.setGeometry(50, 170, 100, 30)
        self.output_file_label.setParent(self)
        self.output_file_edit = QLineEdit(self)
        self.output_file_edit.setGeometry(150, 170, 200, 30)

        # Compress output
        self.output_comp_label = QLabel("Compress output")
        self.output_comp_label.setGeometry(370, 170, 130, 30)
        self.output_comp_label.setParent(self)
        self.output_comp_edit = QComboBox(self)
        self.output_comp_edit.addItems(["Yes", "No"])
        self.output_comp_edit.setGeometry(500, 170, 70, 30)

        # PETimeDistribution
        self.output_petime_label = QLabel("Save PE time distribution")
        self.output_petime_label.setGeometry(50, 210, 170, 30)
        self.output_petime_label.setParent(self)
        self.output_petime_edit = QComboBox(self)
        self.output_petime_edit.addItems(["Yes", "No"])
        self.output_petime_edit.setGeometry(220, 210, 70, 30)

        # Components PETimeDistribution
        self.output_petime_comp_label = QLabel("Save Components")
        self.output_petime_comp_label.setGeometry(310, 210, 130, 30)
        self.output_petime_comp_label.setParent(self)
        self.output_petime_comp_edit = QComboBox(self)
        self.output_petime_comp_edit.addItems(["No", "Yes"])
        self.output_petime_comp_edit.setGeometry(440, 210, 70, 30)

        # Deposited Energy 
        self.output_energy_label = QLabel("Save deposited energy")
        self.output_energy_label.setGeometry(50, 250, 170, 30)
        self.output_energy_label.setParent(self)
        self.output_energy_edit = QComboBox(self)
        self.output_energy_edit.addItems(["Yes", "No"])
        self.output_energy_edit.setGeometry(220, 250, 70, 30)

        # Components Deposited Energy
        self.output_energy_comp_label = QLabel("Save Components")
        self.output_energy_comp_label.setGeometry(310, 250, 130, 30)
        self.output_energy_comp_label.setParent(self)
        self.output_energy_comp_edit = QComboBox(self)
        self.output_energy_comp_edit.addItems(["No", "Yes"])
        self.output_energy_comp_edit.setGeometry(440, 250, 70, 30)


        # Detector Settings
        # --------------
        # Detector List
        self.detector_list_label = QLabel("DetectorList")
        self.detector_list_label.setGeometry(50, 290, 100, 30)
        self.detector_list_label.setParent(self)
        self.detector_list_edit = QLineEdit(self)
        self.detector_list_edit.setGeometry(200, 290, 200, 30)

        # Edit DetectorList
        self.detector_list_button = QPushButton("Edit", self)
        self.detector_list_button.setGeometry(440, 290, 70, 30)
        self.detector_list_button.clicked.connect(self.open_xml_editor)
        self.xml_editor = None

        # Detector Properties
        self.detector_prop_label = QLabel("DetectorProperties")
        self.detector_prop_label.setGeometry(50, 330, 150, 30)
        self.detector_prop_label.setParent(self)
        self.detector_prop_edit = QLineEdit(self)
        self.detector_prop_edit.setGeometry(200, 330, 200, 30)

        # Simulation Settings
        # --------------------

        # Simulation Mode
        self.sim_mode_label = QLabel("Simulatiom mode")
        self.sim_mode_label.setGeometry(50, 370, 150, 30)
        self.sim_mode_label.setParent(self)
        self.sim_mode_edit = QComboBox(self)
        self.sim_mode_edit.addItems(["eFast", "eFull"])
        self.sim_mode_edit.setGeometry(200, 370, 80, 30)
        
        # Visualization
        self.vis_label = QLabel("Visualization")
        self.vis_label.setGeometry(50, 410, 150, 30)
        self.vis_label.setParent(self)
        self.vis_edit = QComboBox(self)
        self.vis_edit.addItems(["None", "Geometry", "Trajectories"])
        self.vis_edit.setGeometry(200, 410, 120, 30)



        # Run button
        # ---------------
        self.save_button = QPushButton("Run Application", self)
        self.save_button.setGeometry(50, 500, 200, 30)
        self.save_button.clicked.connect(self.save_config)

        self.load_config()
    
    def open_xml_editor(self):
        file_dialog = QFileDialog()
        file_path, _ = file_dialog.getOpenFileName(self, "Select XML File", os.getcwd(), "XML Files (*.xml)")

        if file_path:
            if self.xml_editor is None:
                self.xml_editor = DetectorListEditorWindow(file_path)
            else:
                self.xml_editor.file_path = file_path
                self.xml_editor.load_xml_data()

            self.xml_editor.show()


    def load_config(self):

        # open original 
        with open("G4ExSimulator.json", "r") as config_file:
            self.config = json.load(config_file)

        # Input Settings
        self.input_file_edit.setText(self.config["Input"]["InputFileName"])
        self.input_npart_edit.setText(str(self.config["Input"]["InputNParticles"]))
        
        # Output Settings
        self.output_file_edit.setText(self.config["Output"]["OutputFile"])

        # Detector
        self.detector_list_edit.setText(self.config["DetectorList"])
        self.detector_prop_edit.setText(self.config["DetectorProperties"])


    def save_config(self):

        # Input Settings
        self.config["Input"]["InputFileName"] = self.input_file_edit.text()
        self.config["Input"]["Mode"] = self.input_mode_edit.currentText()
        self.config["Input"]["InputNParticles"] = int(self.input_npart_edit.text())

        # Output Settings
        self.config["Output"]["OutputFile"] = self.output_file_edit.text()
        self.config["Output"]["CompressOuput"] = word_to_bool(self.output_comp_edit.currentText())
        self.config["Output"]["SavePETimeDistribution"] = word_to_bool(self.output_petime_edit.currentText())
        self.config["Output"]["SaveComponentsPETimeDistribution"] = word_to_bool(self.output_petime_comp_edit.currentText())
        self.config["Output"]["SaveEnergy"] = word_to_bool(self.output_energy_edit.currentText())
        self.config["Output"]["SaveComponentsEnergy"] = word_to_bool(self.output_energy_comp_edit.currentText())

        # Detector Settings
        self.config["DetectorList"] = self.detector_list_edit.text()
        self.config["DetectorProperties"] = self.detector_prop_edit.text()

        # Simulation Settings
        self.config["Simulation"]["SimulationMode"] = self.sim_mode_edit.currentText()
        self.config["Simulation"]["GeoVisOn"] = (self.vis_edit.currentText() != "None")
        self.config["Simulation"]["TrajVisOn"] = (self.vis_edit.currentText() == "Trajectories")

        # dump new configuration into file and execute
        with open("G4ExSimulator.json", "w") as config_file:
            json.dump(self.config, config_file, indent=4)
        self.execute_script()
      

    def execute_script(self):
        script_path = "run.py"  # Reemplaza con la ruta a tu script
        subprocess.call(["python3", script_path])

    

if __name__ == "__main__":
    app = QApplication([])
    window = ConfigEditorWindow()
    window.show()
    app.exec_()

## Ready-to-go version of LELEC210x project

This repository contains all the modules needed to run the LELEC210x project:

- `fpga/`: Quartus project for the LimeSDR Mini FPGA
- `mcu/`: STM32CubeIDE project for the Nucleo board
- `pylibs/`: Python librairies for the classification and authentification blocks
- `scripts/`: High-level Python scripts (model training, authentification and classification, ...)
- `telecom/`: Simulation framework and GNU Radio modules

## Compiling the gr-fsk module  

Open a terminal, go into the  `telecom/gr-fsk` folder and execute the following commands:
```
mkdir build
cd build/
cmake ..
sudo make install
```

Note that when modifying the core python implementation of the gr-fsk module, e.g., demodulation or syncrhonization, you have to redo an installation to propagate your changes. To do so, execute: 

```
cd build/
sudo make install
```

## Running the project 

An initial training of the classification model needs to be done at least once. Download first the ESC-50 dataset from https://dataverse.harvard.edu/file.xhtml?persistentId=doi:10.7910/DVN/YDEPUT/WTRZ0T&version=2.0 and unzip it to a local folder.

After unzipping the dataset, launch the training:
```
cd scripts/
source env.sh
python train_knn.py ../path/to/ESC-50/
```
The training produces a pickle file KNN.pickle that contains the trained model.

Once the model has been trained, follow these instructions to launch the set-up:

1. Compile the embedded project inside `mcu/`, and flash it on the Nucleo board.
2. Open Lime Suite GUI to flash the FPGA with the provided binary in `fpga/`.
3. Open the GNU Radio Companion with the application `telecom/gr-fsk/apps/main_app.grc`, generate the flow graph and execute it.
4. Launch the classifier in the `scripts/` directory: `python auth_and_classify.py --display --authenticate --classifier KNN.pickle`. The options of the tool can be retrieved using the flag `-h`.

Note that sourcing `env.sh` is mandatory to execute any Python script of the `scripts/` folder.
Please refer to the hands-on to know how to flash the MCU and the FPGA, and to install the GNU Radio out-of-tree modules.

# C dependencies
apt install liblimesuite-dev swig4.0 liborc-0.4-dev

# Python dependencies
To install via pip    

- pyserial
- numpy
- librosa
- sklearn


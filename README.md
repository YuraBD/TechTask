## Requirements
python libraries:
serial

## Used kit
PSoC 6 (CY8CKIT-062S2-43012)

## Installation
1. Copy repository
2. Open project in ModusToolbox workspace
3. Run `make getlibs` in terminal or update via Library Manager

Install PySerial for running python menu:
`python -m pip install pyserial`

## Usage
Run TechTask Program from Launches in ModusToolbox

## Python script usage
`python script_menu.py [-h] [-p VALUE] [-br VALUE]`

options:  
  -h, --help &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; &emsp;&nbsp;&nbsp; show this help message and exit  
  -p VALUE, --port VALUE &emsp; &emsp; &nbsp;&nbsp;Change serial port (Default COM4)  
  -br VALUE, --baudrate VALUE &nbsp;&nbsp;Change baud rate (Default 115200)  

Then choose options from menu

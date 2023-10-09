Based on https://github.com/KuleRucket/cm2024
The CRC-16-MODBUS implementation is copied from https://github.com/lammertb/libcrc/blob/master/src/crc16.c

Added:
  1) Charge battery to a percentage of the full capacity of the battery.
  2) Command line arguments.

This README has been tested on Ubuntu 20.04 and/or Ubuntu 22.04.


**************************************************
* Command Line Arguments                         *
**************************************************
The Command Line Arguments are: ./cm2024 <file-name> <format> <slot>
  1) The file name argument, either:
      '-usb'           : Show GUI.         Reads from the USB-connection, '/dev/ttyUSB0'
      '-usb_dump'      : Show GUI.         Reads from the USB-connection, '/dev/ttyUSB0'. Write the input to the 'cm2024_dump.bin' file. The file is overwritten if it exists.
      otherwise <file> : Do NOT show GUI.  Reads from file <file>. The format argument must be specified, see below.
  2) The format argument, either: (Can be specified for any file name argument. Must be specified if the file name argument is neither '-usb' nor '-usb_dump')
      'default'   : The existing and default print format. The slot argument must NOT be specified.
      'csv'       : Output CSV data for the given slot. The slot argument must be specified, see below.
  3) The slot argument, either: (Can only be specified if the format argument is 'csv')
      '1' .. '8'  : Output CSV data for this slot only
      'A' .. 'B'  : Output CSV data for this slot only


**************************************************
* Functionality                                  *
**************************************************
Added functionality:
  1) LCD Brightness can be changed from the GUI.
  2) The SD Card can be released or unmounted from the GUI.
  3) Charge a battery to a percentage of the full capacity of the battery.
       - Set the "Max charge %" in the GUI to the percentage of the capacity of the battery.
       - Important, select the "Cycle" program for the battery.
       - The "Max charge %" turns green in the GUI when the percentage input value is valid.
       - The slot turns green in the GUI when the "Max charge %" is valid and the slot is running the program "Cycle".
       - During the "Cycle" program after the battery has been discharged then an estimate for the remaining charge time is shown.
       - When the percentage is reached the "Cycle" program is cancelled and the charging stops.
       - Note, the estimate for the remaining charge time is changed when the charging current is changed.
       - Note, the discharge capacity is used when calculating the percentage capacity. The discharge capacity is normally slightly less than the charge capacity, which would be more accurate to use.


**************************************************
* CRC-16-MODBUS                                  *
**************************************************
The CRC-16-MODBUS implementation is copied from https://github.com/lammertb/libcrc/blob/master/src/crc16.c

Some online CRC-16-MODBUS calculators:
https://crccalc.com/
https://www.lammertbies.nl/comm/info/crc-calculation

The CRC for sending a command is as follows.
For details see UsbWorker::cm2024CrcModbus
  1) Calculate the CRC-16-MODBUS for the bytes from (and including) offset 0 to (and including) offset 10. A total of 11 bytes. All bytes are 0x00 except the first 2 bytes and the last byte is 0x01.
  2) Get the high byte and also the low byte of the CRC-16-MODBUS.
  3) Add the high byte to the CRC-16-MODBUS.
  4) If the sum of the high byte and the low byte exceeds 0xFF then add 0x01 to the CRC-16-MODBUS.

The above is used for the following programs:
  1) Cancel Slot, 0x02
  2) LCD Brightness, 0x03
  3) Release or unmount the SD Card, 0x04

This CRC has not been verified with the "Start" program.


**************************************************
* MESSAGE FORMAT                                 *
**************************************************
https://github.com/KuleRucket/cm2024/wiki#dat-message

DAT Message:
Offset 05: Program Running. Not-Running(0), Running(1). Or Charging/Discharging active/not active. At least related: When pressing "START PROGRAM" it will be Not-Running(0) for the first minute then afterwards it will be Running(1)
Offset 23: Voltage Index.
               Error Slot: 0x09.
               Empty Slot: 0x20.
               Else, seen values from 0x02 to 0x07.
               When charging, the voltage index relates to the voltage as follows:
                   0.000V  <= 0x02 <= 1.375V
                   1.376V  <= 0x03 <= 1.390V
                   1.391V  <= 0x04 <= 1.405V
                   1.406V  <= 0x05 <= 1.420V
                   1.421V  <= 0x06 <= 1.510V
                   0x07: Finished
               When discharging, the voltage index relates to the voltage as follows:
                   0x02: Finished
                   0.000V  <= 0x03 <=  1.250V
                   1.251V  <= 0x04 <=  1.270V
                   1.271V  <= 0x05 <=  1.285V
                   1.286V  <= 0x06 <=  1.300V
                   1.301V  <= 0x07 <=  1.510V
Offset 24: Trickle State. Not-Trickle(0), Trickle(1). Matches Program-state (offset 6) with value Trickle(8).
Offset 26: Slot Status.
                  0x00: Empty Slot.
                  0x01: Program running.
                  0x02: Determinating. Program just started within 5 seconds.
                  0x04: Program finished
offset 27: Number of "START PROGRAM" for the slot since power up. The number increments on every "START PROGRAM". Counting starts from 0.
Offset 32: Maximum Discharge Rate as specified at "START PROGRAM" (see discharge rate table). Offset 31 is the Actual Discharge Rate
Offset 33: Step Index. The "Charge" program has one step. The "Cycle" program has three steps. The step index is the index of the steps of the program.
           When the program is complete the step index is "max". That is when the "Cycle" program has finished the 3 steps then the step index is 3 (but 4 values: 0-3).
           Counting starts from 0.
           Step index 1 has value 0x00:
              00: Step 1
              01: Step 2
              02: Step 3
              etc


**************************************************
* CHARGING GRAPH                                 *
**************************************************
The charging data can be shown using a spreadsheet.
Use the template "cm2024-template.ods" and open it in LibreOffice.

While charging the battery use the command line argument '-usb_dump'.
./cm2024 -usb_dump

When the charging is done then do the following for slot 1:
  1) Run: ./cm2024 cm2024_dump.bin csv 1
     The "cm2024_dump.bin" file was created while running ./cm2024 -usb_dump
     The arguments "csv" "1" get CSV data for slot 1.
  2) Open cm2024-template.ods - only useful for few lines of CSV data from 1), otherwise see 3)
     - Select cell A12
     - Select cells below A12 to contain the CSV data.
     - Insert rows ( ctrl-+  -> "Entire Row" )  -- This will update the data ranges in the chart
     - Copy/paste CSV data from 1) into cell A10 (or A9, if the header is included). Select Language: "English(USA)", separator: "Space", mark: "Merge delimiters".
     - Copy/paste the cells G10..M10 downwards from G10 to match the copy/pasted CSV data from 1)
  2) Open cm2024-template.ods - handles large CSV data from 1)
     - Select A10 (or A9)
     - Copy/paste CSV data from 1) into cell A10 (or A9, if the header is included). Select Language: "English(USA)", separator: "Space", mark: "Merge delimiters".
     - Select cell A12
     - Select to end of column A: ctrl-shift-down
     - Unselect the very last row: shift-up
     - Insert rows ( ctrl-+  -> "Entire Row" )  -- This will update the data ranges in the chart
     - Go to the first non-empty row below A12: ctrl-down ctrl-down
     - Go to the row below: down
     - Select all data from this row and downwards and delete it: ctrl-shift-down ctrl-shift-right delete
     - Select A12: ctrl-up up
     - Select A10 (or A9)
     - Copy/paste CSV data from 1) into cell A10 (or A9, if the header is included). Select Language: "English(USA)", separator: "Space", mark: "Merge delimiters".
     - Select G10
     - Select G10..M10: ctrl-shift-right
     - Copy: ctrl-c
     - Select G10: left right
     - Select to the end of column G: ctrl-shift-down ctrl-shift-down
     - Paste: ctrl-v
     - Select G10: ctrl-up
     - The graph is at cell O5

The charging time minutes are shown on the x-axis.


**************************************************
* CONVERT CHARGING DATA FORMATS                  *
**************************************************
The following data formats can be converted to the CSV data format for the CHARGING GRAPH, see above:
  1) 'csv': The data format is output from: ./cm2024 -usb csv 1 > csv_slot1.txt
     - The data is in the file "csv_slot1.txt"
     - Normally, 46-47 rows pr minute are generated. Mostly, for each minute all of the rows are identical.

  2) 'default': The data format is output from: ./cm2024 -usb default > cm2024_dump.txt
     - The data is in the file "cm2024_dump.txt"
     - Normally, 46-47 rows pr minute are generated. Mostly, for each minute all of the rows are identical.
     - For slot 1 run: SLOT='1'; grep -C10 -a 'slot:         Slot '"${SLOT}"'$' cm2024_dump.txt  | grep -a '^\(minutes\|current\|voltage\|chargeCap\|dischargeCap\|step\):' | sed -z 's/\nvoltage: */ /g' | sed -z 's/\ncurrent: */ /g' | sed -z 's/\nchargeCap: */ /g' | sed -z 's/\ndischargeCap: */ /g' | sed -z 's/\nminutes: */ /g'  | sed -z 's/step: *#[0-9]*, //g' | sed 's/^\([^ ]*\) \([^ ]*\) /\2 \1 /'
     - Script Explanation:
       - Set the slot number.
       - Grep lines for the given slot and the next 10 lines.
       - Grep lines for minutes, current, voltage, chargeCap, dischargeCap and step.
       - Clear the names: minutes, current, voltage, chargeCap, dischargeCap and step.
       - Swap the column 1 and 2, that is the "minutes" and "step" columns.

  3) SD-card-format
     - The data is in the file "S1_LOG01.CSV" for slot 1 from the SD Card.
     - Normally, 1 row pr minute is generated.
     - Run: grep '^[0-9]*/[0-9]*:[0-9]*;' S1_LOG01.CSV | while read LINE; do DAY="$(sed 's,^\([0-9]*\).*,\1,' <<< "${LINE}")"; HOUR="$(sed 's,^[0-9]*/\([0-9]*\):.*,\1,' <<< "${LINE}")"; MINUTE="$(sed 's,^[0-9]*/[0-9]*:\([0-9]*\);.*,\1,' <<< "${LINE}")"; echo "$(( 10#${DAY} * 24 * 60 + 10#${HOUR} * 60 + 10#${MINUTE} ))"';'"$(sed 's,^[0-9]*/[0-9]*:[0-9]*;,,' <<< "${LINE}")"; done | sed 's/^\([0-9]*\);IDL;/\1;Idle;/' | sed 's/^\([0-9]*\);RDY;/\1;Ready;/' | sed 's/;\([0-9]\{3\}\);/;0.\1;/g' | sed 's/;\([0-9]\{3\}\);/;0.\1;/g' | sed 's/;\([0-9]\+\)\([0-9]\{3\}\);/;\1.\2;/g' | sed 's/;\([0-9]\+\)\([0-9]\{3\}\);/;\1.\2;/g' | sed 's/;\([0-9]*\),\([0-9]*\);/;\1.\2;/g' | sed 's/;\([0-9]*\),\([0-9]*\);/;\1.\2;/g' | sed 's/;/ /g'
     - Script Explanation:
       - Grep the lines starting with the "Minutes" time format.
       - while-part: Extract the days, hours and minutes part from the "Minutes" format and calculate the "Minutes" value.
       - Replace Step 'IDL' to 'Idle'.
       - Replace Step 'RDY' to 'Ready'.
       - Replace <3-digit-number> with semicolon around to 0.<3-digit-number>. That is the number is divived by 1000. That is from V to mV and A to mA.
       - Duplicated - handles overlapping semicolon-separators.
       - Replace <1-or-more-digit-number><3-digit-number> with semicolon around to <1-or-more-digit-number>.<3-digit-number>. That is the number is divived by 1000. That is from V to mV and A to mA.
       - Duplicated - handles overlapping semicolon-separators.
       - Replace <digit-number>,<digit-number> with semicolon around to <digit-number>.<digit-number>. That is the decimal mark changed from ',' to '.'.
       - Replace the semicolon separator to space.


**************************************************
* DEBUG CM2024 LOGGER FORMAT USING VIRTUALBOX    *
**************************************************
The Windows program "CM2024 Logger", Version 1.1.0.0, from Voltcraft can be downloaded here.
https://asset.conrad.com/media10/add/160267/c1/-/en/002002024DL01/download-2002024-oplader-til-runde-celler-voltcraft-charge-manager-cm2024-3000-ma.zip

Install the "CM2024 Logger" in Windows in VirtualBox.
Shutdown Windows in VirtualBox
In VirtualBox set the "Serial port", COM1 to use the "Path/Address" to "localhost:3017"

Run the script:
(stty 57600 raw; sudo echo Start Windows in VirtualBox within 20 seconds 1>&2; sleep 20; echo Windows in VirtualBox should have been started 1>&2; tee cm2024_input.bin) < /dev/ttyUSB0 | nc -l 3017 | tee cm2024_output.bin | sudo tee /dev/ttyUSB0
Within 20 seconds after the script is started then start Windows in VirtualBox
After 20 seconds, in Windows in VirtualBox start the "CM2024 Logger".

Data received from the "VOLTCRAFT Charge Manager CM2024 3000 mA" is found in "cm2024_input.bin"
Data sent to the "VOLTCRAFT Charge Manager CM2024 3000 mA" is found in "cm2024_output.bin"

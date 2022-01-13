# cm2024
Application for monitoring and control of Voltcraft CM 2024 Charge Manager from conrad. This device has a serial interface and Windows software but nothing for Linux so it is my intention to reverse engineer the interace and create some sort of open source command line tool to interact with it.

# Serial Interface
TBW

# Monitoring
There are two kinds of monitoring messages recieved from the device. There is a "CM2024 SUP" message every second which seems to be an overall status and is used to indicate that a battery is ready to charge (and possibly other things I am yet to figure out). The second type of message returns the status of each slot independently.

First a message like this will arrive
```
43 4D 32 30 32 34 20 53 55 50                      CM2024 SUP
```
Followed by another fixed length message like this:
```
02 00 7B 01 EE 02 0D 00 82 78 78 78 78 78 78 78
78 78 78 78 78 78 78 78 78 78 78 78 78 78 78 78
78 98 16 0D 0A
```
Then when a battery is inserted it will switch to something like:
```
01 00 7B 01 ED 02 09 00 13 00 01 01 00 02 00 00
00 3C 05 05 00 08 78 78 78 78 78 78 78 78 78 78
78 73 26 0D 0A
```

So far I have figured out the following:

| Offset | Description |
| -- | -- |
| 01 | Unknown |
| 02 | Unknown |
| 03 | Unknown |
| 04 | Unknown |
| 05 | Unknown |
| 06 | Unknown |
| 07 - 08 | Sequence Counter going from 0x0000 to 0x0257 (0-599) in increments of 13 |
| 09 | Number of the slot awaiting setup Slot 1=0x00 to Slot B=0x09 |
| 10 | Unknown |





# Control






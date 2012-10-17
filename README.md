external_axioctl
================

AXIS AX88179 USB 2.0/3.0 Giga Ethernet programming tools
The tools need driver support the feature.

AX88179/AX88178A Linux Command Line EEPROM/eFuse Programming Tool

This tool can be used to read/write the EEPROM/EFUSE of AX88179/AX88178A. 

================
Getting Start
================

1. Extract the compressed driver source file to your template directory by the
   following command:

	[root@localhost template]# tar -xf DRIVER_SOURCE_PACKAGE.tar.bz2

2. Now, the driver source files should be extracted under the current directory.
   Executing the following command to compile the driver:
 
	[root@localhost template]# make
			
3. If the compilation is well, the ioctl will be created under the current
   directory.
 
================
Usage
================

1. If you want to read out values of the EEPROM/EFUSE to a file, go to the driver directory and
   execute the following command:

	[root@localhost driver_dir]# ./axioctl reeprom type file_name size

2. If you want to write values of a file to the EEPROM/EFUSE,  go to the driver directory and 
   execute the following command:

	[root@localhost driver_dir]# ./axioctl weeprom type file_name _size

3. If you need more information about the instructions, go to the driver directory and
   execute the following commands:

	[root@localhost driver_dir]# ./axioctl reeprom help

or

	[root@localhost driver_dir]# ./axioctl weeprom help


command Note:
[root@localhost driver_dir]# ./axioctl reeprom type file_name size
        [type]         - 0: EEPROM,  1: eFuse.
        [file_name]    - Output file.
        [size]         - EEPROM/eFuse SIZE (bytes). EEPROM maximum 512 bytes, eFuse maximum 64 bytes


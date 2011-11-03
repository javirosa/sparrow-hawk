These utilities are used to download (thermodl) and 
mission (thermoms) a DS1921G Thermochron iButton.  The
DS1921Z/DS1921H are not supported with this application.

THERMODL:

usage: thermodl 1wire_net_name <output_filename> </Fahrenheit>
  - Thermochron download on the 1-Wire Net port
  - 1-wire_net_port required port name
    example: "COM1" (Win32 DS2480B),"/dev/cua0"
    (Linux DS2480B),"1" (Win32 TMEX)
  - <output_filename> optional output filename
  - </Fahrenheit> optional Fahrenheit mode (default Celsius)
  - version 1.03

THERMOMS:

usage: thermoms 1wire_net_name </Fahrenheit>
  - Thermochron configuration on the 1-Wire Net port
  - 1-wire_net_port required port name
    example: "COM1" (Win32 DS2480B),"/dev/cua0"
    (Linux DS2480B),"1" (Win32 TMEX)
  - </Fahrenheit> optional Fahrenheit mode (default Celsius)
  - version 1.03


The Thermochron has the following features:

   -Temperature range -40°C to 85°C (-40°F to 185°F) 
   -Selectable starting offset (0 minutes to 46 days) 
   -Sample interval (1 to 255 minutes) 
   -Alarms and capture registers triggered when outside specified range 
   -2048 time-stamped temperature readings with optional data wrap 
   -Long-term temperature histogram with 2°C resolution in 56 bins 
   -4096 bits of general-purpose read/write nonvolatile memory 

Required on the command line is the 1-Wire port name:

example:  "COM1"                (Win32 DS2480B)
          "/dev/cua0"           (Linux DS2480B)
          "1"                   (Win32 TMEX)
          "\\.\DS2490-1"        (Win32 USB DS2490)
          "{1,5}"               (Win32 DS2480B multi build)
          "{1,6}"               (Win32 USB DS2490 multi build)
          "{1,2}"               (Win32 DS1410E multi build)

This application uses the 1-Wire Public Domain API. 
Implementations of this API can be found in the '\lib' folder.
The libraries are divided into three categories: 'general', 
'userial' and 'other'. Under each category are sample platform
link files. The 'general' and 'userial' libraries have 'todo' 
templates for creating new platform specific implementations.  

This application also uses utility and header files found
in the '\common' folder. 

Application File(s):		'\apps\thermo'
thermodl.c   -	 this utility uses to download the results of the
                 current mission of a DS1921 Thermochron iButton
thermoms.c   -   mission/configure the DS1921 Thermochron iButton 
	     	

Common Module File(s):		'\common'
thermo21.c   - 	Thermochron iButton utility functions
thermo21.h   - 	include file for Thermochron demo
ownet.h    -   	include file for 1-Wire Net library
crcutil.c  -    keeps track of the CRC for 8 and 16 
                bit operations 
ioutil.c   - 	I/O utility functions

#########################################################

Result like below:

/---------------------------------------------
  Find and mission DS1921 Thermochron iButton(s)
  Version 2.00


Read status of Thermochron: 983B200001A61221
Setup to read the mission status --> Ready to read status page 16
Read the status page --> Pages read from Thermochron
Finished --> Operation complete
End script normally

Mission State
-------------
Serial Number of DS1921: 983B200001A61221
Mission is ended
Sample rate: 0 minute(s)
Roll-Over Enabled: no
Roll-Over Occurred: no
Mission Start time: not started yet
Mission Start delay: 0 minute(s)
Mission Samples: 0
Device total samples: 0
Temp displayed in: (Celsius)
High Threshold:   87.5
Low Threshold:   87.5
Current Real-Time Clock from DS1921: 01/01/1970  00:00:00
Current PC Time: 08/25/2011  15:54:13

Erase current mission
  (0) yes
  (1) no
Answer: (1):

Input abort

Read status of Thermochron: 2B4F200001247A21
Setup to read the mission status --> Ready to read status page 16
Read the status page --> Pages read from Thermochron
Finished --> Operation complete
End script normally

Mission State
-------------
Serial Number of DS1921: 2B4F200001247A21
Mission is ended
Sample rate: 0 minute(s)
Roll-Over Enabled: no
Roll-Over Occurred: no
Mission Start time: not started yet
Mission Start delay: 0 minute(s)
Mission Samples: 0
Device total samples: 0
Temp displayed in: (Celsius)
High Threshold:   87.5
Low Threshold:   87.5
Current Real-Time Clock from DS1921: 01/01/2000  00:00:01
Current PC Time: 08/25/2011  15:55:30

Erase current mission
  (0) yes
  (1) no
Answer: (1):

Input abort






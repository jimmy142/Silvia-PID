# Silvia-PID
Open-source project to create a PID controller for the Rancilio Silvia espresso machine. 

Code composed in Arduino IDE to be uploaded shortly.

Files uploaded so far:
 - Silvia-PID-Schematic.pdf - circuit diagram crerated in EastEDA.
 - Silvia-PID-PCB.zip - Gerber files for PCB design.
 - Silvia-PID-BOM.csv - Bill of materials. 
 - Silvia-PID.ino - Arduino sketch for ESP32
 - index.html - htmpl page for webserver, needs to be in \index folder and is uploaded in Ardunio IDE using sketch data upload.

Key components required include ESP32 Dev-kit-C, MAX6675 and a few capacitors, the rest are optional. Not shown on the schematic is the solid state relay and the 5V supply.  

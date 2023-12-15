## magloop-antenna project
### RC Servo-Based Capacitor Driver Design Files

Design files for my 20M magnetic loop antenna servo motor capacitor driver<br>
Design files are contained in folder "pwmServo-1". This folder is an Arduino project folder.<br>

This repo contains hardware construction details as well as software source code (Arduino) for the RC servo motor driver which rotates the antenna's variable tuning capacitor.

<p> NOTE: 12/15/23: Due to problems with servo jitter causing amplitude modulation of the received and transmitted signals, the servo and the Arduino software are being redesigned to add PID control of the servo. An Arduino Nano microcontroller board will be added to the servo to implement the PID control functions. Those changes, once tested, will be added to this repository.
<p>

Software source files are:<br>
<ul>
    <li>pwmServo-1.ino<br>
    <li>pwmServo-1.h
</ul>
    
Connections to the Arduino Uno:<br>
<ul>
   <li>Docs/Cap_driver_pinouts.txt
   
</ul>

Controller Schematic:<br>
<ul>
<li>Docs/controller_schematic.svg
<li>Docs/controller_schematic.pdf
</ul>

Images of the prototype:<br>
<ul>
<li>img/controller_interior.jpg
<li>img/controller_exterior.jpg
<li>img/controller_back_panel.jpg
</ul>

Project history and design notes:<br>
<ul>
<li>Docs/servo_design_notes.md
</ul>

See the pwmServo-1.ino source file for further notes.

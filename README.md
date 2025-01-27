# Beagle Copter

Beagle-copter is a Quadcopter project in c++ using Beaglebone black as a Flight controller. In this project I will be working on Beaglebone Black running on Angstrom OS with linux 3.8.13 kernel. 


# Projects That I Refered for Beagle-Copter

* Xu Shirui - Beagle-fly Project
* Build your own Quadcopter - Ghowen
* PRU Assembly Instructions - Texas Instruments


# [Final Presentation for the Project](http://antodominic.com/beagle-copter/)


# How to run this Code on BBB

1) Clone Beagle-copter into your BBB 
$ git clone https://github.com/antodoms/beagle-copter.git

2) Now navigate to beagle-copter folder and make an executive file "controls"
$ make CROSS_COMPILE=""

3) Now Move am335x-bone.dtb & am335x-boneblack.dtb present in the beagle-copter folder to /boot/  folder

4) Restart your BBB and navigate to beagle-copter folder and run the below lines in shell
$ ./controls




## Project Guide
Cyriac M Odackal


## Project Team
Anto Dominic
Dalbin Jose
Vishak Baby

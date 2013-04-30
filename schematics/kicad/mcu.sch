EESchema Schematic File Version 2  date Tue 30 Apr 2013 06:08:21 PM EEST
LIBS:stellaris_launchpad
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:noname-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date "30 apr 2013"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L STELLARIS_LAUNCHPAD U4
U 1 1 517FDBD9
P 4650 4450
F 0 "U4" H 4650 4350 60  0000 C CNN
F 1 "STELLARIS_LAUNCHPAD" H 4650 4500 60  0000 C CNN
F 2 "~" H 4650 4450 60  0000 C CNN
F 3 "~" H 4650 4450 60  0000 C CNN
	1    4650 4450
	1    0    0    -1  
$EndComp
Text HLabel 3750 2950 0    60   Output ~ 0
MOTOR_EN
Text HLabel 5550 3650 2    60   Output ~ 0
MOTOR_CLK
Text HLabel 3750 2550 0    60   Input ~ 0
LED_DETECTOR
Wire Wire Line
	3750 2650 2600 2650
Wire Wire Line
	2600 2650 2600 2750
$Comp
L GND #PWR011
U 1 1 517FDBED
P 2600 2750
F 0 "#PWR011" H 2600 2750 30  0001 C CNN
F 1 "GND" H 2600 2680 30  0001 C CNN
F 2 "" H 2600 2750 60  0000 C CNN
F 3 "" H 2600 2750 60  0000 C CNN
	1    2600 2750
	1    0    0    -1  
$EndComp
Text HLabel 3750 3750 0    60   Output ~ 0
LASER_PULSE_OUT
Text HLabel 3750 2750 0    60   Output ~ 0
LASER_PWM_OUT
Text HLabel 3750 2850 0    60   Output ~ 0
STEP_A1
Text HLabel 3750 3050 0    60   Output ~ 0
STEP_A2
Text HLabel 3750 3250 0    60   Output ~ 0
STEP_B1
Text HLabel 3750 3450 0    60   Output ~ 0
STEP_B2
Text HLabel 3750 3150 0    60   Input ~ 0
STEP_HOME_POS
$EndSCHEMATC

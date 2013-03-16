== Project skeleton for Atmel AVR MCUs with avr-gcc ==

System requirements:
* avrdude
* avr-gcc
* scons

== Usage ==

Edit SConstruct with your AVR MCU model and clock frequency. Change programmer in AVRDUDE_BASE to programmer you use.

Available programmers:
`avrdude -c -` 
Available AVR part numbers:
`avrdude -c usbasp -p -`


Program sources are in `src/` folder.

Build:
`scons`

Upload:
`scons upload`

Read fuses:
`scons read-fuses`

Write fuses (configured in SConstruct file):
`scons write-fuses`

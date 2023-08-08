# rocketbeacon (Work in progress)
Configurable sub-GHz radio beacon for amateur rocketry.

**Warning: make sure to comply with your local RF regulations. Some frequency bands require an amateur rocketry license, others have power or duty cycle limitations.**

## Hardware
The hardware is designed in KiCad for assembly with JLCPCB. Two versions of the RF front end are available, one optimized for the 220 MHz band and one for the 440 MHz band. Note that the 220 version is still untested.

The hardware features:

* STM32WLE5 wireless microcontroller
* Max 22 dBm output power
* CR2032 battery holder
* Can be powered externally by 3.3-5V

## Assembly
Other than the assembled PCB, an antenna is required. For a basic quarter wave copper antenna, you can use `length = 0.95 * 0.25 * speed of light / frequency`, where 0.95 is the velocity factor of copper. E.g.

* For 433 MHz, use a 165mm
* For 220 MHz band, Use 324mm

If powered by a button cell battery, solder on a [MY-2032-12](https://www.lcsc.com/product-detail/Battery-Connectors_MYOUNG-MY-2032-12_C964833.html) battery connector. Other similar CR2032 holders work as well, but may require trimming the solder leads to fit the footprint.

If powered by an external battery (e.g. 1s lipo), connect to the V+ and GND pins. Note that it doesn't have reverse polarity protection, so double check the polarity!


## Compile and program
The firmware can be modified and compiled with [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html).

The main code is located in `Firmware\Core\Src\main.c`

Programming the firmware can be done with [STM32CubeProg](https://www.st.com/en/development-tools/stm32cubeprog.html) and a cheap UART to USB dongle (Search "ftdi adaptor" and get one of the red dongles 6 pins)

* Disconnect power
* Hold down the button
* Connect the FTDI adaptor to the beacon (make sure Rx goes to Tx and Tx goes to Rx). If unsoldered, hold them at an angle to ensure proper contact.
* LED should be continuously ON, indicating it has entered bootloader mode.
* In STM32CubeProg, select the UART option and port. Then connect to the MCU.
* In the Erasing & Programming tab, pick the right firmware `Firmware\Debug\rocketbeacon.elf`, and program
* Cycle power to execute the code.


## Configuration
TODO: Make a little UART configuration interface to set frequency, callsign etc.
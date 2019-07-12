# Arduino PDU Relay Controller

Turns your Arduino into a relay controller. A typical use-case for this is a PDU (Power Distribution Unit).

## Hardware

This project requires the following hardware:
* an Arduino board (this was developed on a Leonardo, but should work with any other Arduino board)
* one Arduino digital pin for each relay, by default pins 2 to 8 are used
* 1 to 8 relays which can be controlled with the voltage from your Arduino's IO pins
  * check your board documentation: IO voltage may be 3.3V or 5V
  * there are so-called "relay modules" out there which operate at 5V or 12V

## Protocol

The communication with the relay controller uses a simple 1 (command only) or 2 (command and argument) byte protocol.
It aims to be compatible with the "USB-OPTO-RLY88" protocol from [Robot Electronics](https://robot-electronics.co.uk/).

Reference documentation:
* [Robot Electronics USB-OPTO-RLY88, 8 optically Isolated Inputs, 8 Relay Outputs (Technical Documentation)](https://robot-electronics.co.uk/htm/usb_opto_rly88tech.htm)
* [Robot Electronics USB-RLY82 - 2 relay outputs and 8 analogue or digital inputs
Technical Documentation](https://robot-electronics.co.uk/files/usb-rly82.pdf)

## Compatible Software

* [PDUDaemon](https://github.com/pdudaemon/pdudaemon)
  * use one of the following driver names `devantech_USB-OPTO-RLY88` or `devantech_USB-RLY08B`

## License

see the `LICENSE` file.

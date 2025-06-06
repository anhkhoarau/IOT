# IOT
IOT system in agriculture

This code performs the following tasks:

Reads data from the DHT11 temperature sensor, then sends a signal to a Panasonic air conditioner: if the temperature exceeds 31°C, it turns the AC on; if it falls below 27°C, it turns the AC off.

Reads data from a soil moisture sensor (analog pin) and controls a relay accordingly (the relay can be connected to a water pump, fan, or other switching device).

Displays information on an OLED screen (using I2C with SDA/SCL connected to D21/D22).


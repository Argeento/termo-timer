# Temperature-Controlled Relay System

This Arduino project controls a relay based on temperature readings and a timer. It is designed for applications requiring temperature-based switching, such as controlling a heating element.

## Features

- **Temperature Monitoring**: Uses a DallasTemperature sensor for precise temperature readings.
- **Timer Functionality**: Set a countdown timer for the relay to switch off automatically.
- **LCD Display**: Shows current temperature, target temperature, and remaining time.
- **Button Control**: Adjust target temperature and timer settings with physical buttons.

## Components

- Arduino (Uno, Mega, etc.)
- 1x LiquidCrystal_I2C LCD Display
- 1x DallasTemperature Sensor (e.g., DS18B20)
- 1x Relay Module
- 3x Push Buttons
- Various connecting wires

## Setup

1. **Connect the Components**: Follow the pin definitions in the code to connect all components to your Arduino board.
2. **Load the Code**: Upload the provided Arduino sketch to the board.
3. **Set Parameters**: Use the buttons to set the desired temperature and timer.

## Usage

- **Increase Button**: Raises the target temperature or increases timer duration.
- **Decrease Button**: Lowers the target temperature or decreases timer duration.
- **Start Button**: Starts the timer with the set parameters.

The LCD will display the current and target temperatures, as well as the remaining time if the timer is active. The relay will turn on or off based on the current temperature and the timer status.

## Code Structure

- `setup()`: Initializes all components.
- `loop()`: Main program loop, handling sensor reading, button inputs, LCD updates, and relay control.

## Customization

You can modify the sketch to suit different requirements, such as changing temperature thresholds or timer settings.

## License

This project is open-source and available under the MIT License.
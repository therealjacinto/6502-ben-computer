This project utilizes vscode on WSL2, and therefore there are a few setup requirements in order to flash an arduino from WSL2. Assuming you have already setup `usbipd-win` (see [top-level README](../README.md)), you now need to download the Arduino CLI and save it to the `bin` direcotry in the top-level of this workspace. The vscode settings files are already setup to look in that directory for this executable.

You will also need to download the [arduino vscode extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino).

Now that you have arduino properly setup on your computer, you will need to perform the following steps (take from [here](https://github.com/microsoft/vscode-arduino/issues/1163#issuecomment-997955776)):
```
./bin/arduino-cli config init  # May not be necessary
./bin/arduino-cli board list
./bin/arduino-cli core install [INSERT-CORE-VERSION-HERE]  # For example -> arduino:avr
```

The current settings for this repo use the Arduino Mega 2560 board. If you use a different board you will need to update the board using the arduino vscode extension.

You can now flash code onto the arduino and use the serial monitor (given you have passed the USB through to WSL2).
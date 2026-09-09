# ESP32 Environment Monitor

ESP32-based environmental monitoring project using a DHT11 temperature/humidity sensor and an MQ-135 gas sensor module. The ESP32 connects to Wi-Fi and exposes the latest readings as JSON over HTTP.

## Features

- Reads temperature and humidity from a DHT11 sensor.
- Reads gas status from the MQ-135 module digital comparator output.
- Preserves active LOW MQ-135 logic: `LOW = gas detected`, `HIGH = air OK`.
- Serves a simple HTTP status page at `/`.
- Serves latest environmental readings as JSON at `/latest`.
- Keeps Wi-Fi credentials outside version-controlled source code.

## Hardware

| Component | Notes |
| --- | --- |
| ESP32 development board | Main controller and Wi-Fi server |
| DHT11 sensor | Temperature and humidity |
| MQ-135 gas sensor module | Digital D0 output is currently used |
| Jumper wires | For sensor connections |
| USB cable | For power, serial monitor, and upload |

## Wiring

| Sensor | Pin | ESP32 connection |
| --- | --- | --- |
| DHT11 | VCC | 3V3 |
| DHT11 | GND | GND |
| DHT11 | DATA | GPIO27 |
| MQ-135 | VCC | 5V |
| MQ-135 | GND | GND |
| MQ-135 | D0 | GPIO33 |
| MQ-135 | A0 | Not used |

## MQ-135 Digital Output

This project currently uses the MQ-135 module's digital `D0` comparator output, not the analog `A0` output.

The digital output threshold is set with the small potentiometer on the MQ-135 module:

- `LOW` means gas detected.
- `HIGH` means air OK.

Adjust the potentiometer on the module to tune when the comparator changes state.

## Software Requirements

- Arduino IDE or `arduino-cli`
- ESP32 board package
- Libraries:
  - `DHT sensor library for ESPx` for `DHTesp.h`
  - `ArduinoJson`

The ESP32 `WiFi.h` and `WebServer.h` headers are provided by the ESP32 board package.

## Configure Wi-Fi

Copy the example secrets file:

```sh
cp secrets.example.h secrets.h
```

Edit `secrets.h` and replace the placeholders:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
```

`secrets.h` is ignored by Git so local Wi-Fi credentials are not committed.

## Upload to ESP32

1. Open `esp32-environment-monitor.ino` in Arduino IDE.
2. Select an ESP32 board from the board menu.
3. Select the ESP32 serial port.
4. Install the required libraries if they are not already installed.
5. Make sure `secrets.h` exists.
6. Compile and upload.

With `arduino-cli`, a typical compile command is:

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 .
```

Upload command example:

```sh
arduino-cli upload -p /dev/cu.usbserial-0001 --fqbn esp32:esp32:esp32 .
```

Replace the port with the port shown for your ESP32.

## Serial Monitor

Set Serial Monitor to `115200` baud. Example output:

```text
Connecting to WiFi....
WiFi connected!
ESP32 IP: 192.168.1.123
DHT + MQ-135 (D0) started. HTTP server started.
T=23.50 C | H=48.00 % | MQ=OK
T=23.60 C | H=48.00 % | MQ=GAZ
```

## HTTP API

After the ESP32 connects to Wi-Fi, the Serial Monitor prints its local IP address.

Open the root endpoint to confirm the server is running:

```text
http://<esp32-ip>/
```

Response:

```text
ESP32 OK. Open /latest for JSON.
```

Open `/latest` to get the latest readings:

```text
http://<esp32-ip>/latest
```

Example JSON response:

```json
{
  "temperature": 23.5,
  "humidity": 48,
  "gas": false
}
```

The `gas` field is `true` when the MQ-135 D0 pin reads `LOW`, meaning gas is detected by the module comparator.

# M5Paper WiFi & BLE Scanner Project

This project scans for WiFi and BLE devices using the M5Paper, logs the data to an SD card, and displays it on the M5Paper's e-paper screen. It also integrates GPS data for each scan, showing the time, location, altitude, and accuracy of each scan.

## Prerequisites

Before starting, make sure you have the following installed in your Arduino IDE:

1. **M5Paper Unit**
2. **M5 GPS i2c Unit & Cable**
3. **SD Card**
4. **Arduino IDE** (latest version)
5. **ESP32 Board Package** - Add via the Boards Manager:
   - Go to `Tools` > `Board` > `Boards Manager` and search for `ESP32`.
   - Install **ESP32 by Espressif Systems**.

## Libraries Needed

Some of the libraries are bundled with the ESP32 board package and don't require manual installation, while a few need to be installed from the Library Manager.

### Bundled Libraries (Included with the ESP32 Core)
1. **WiFi** - For scanning and connecting to WiFi networks
2. **SD** - For handling SD card operations
3. **SPI** - For SPI communication
4. **ESP32 BLE Arduino** - For BLE functionality
   
### External Libraries (Needs to be installed)
1. **M5EPD** - For M5Paper functionality 
2. **TinyGPS++** - For handling GPS data

### Installing Libraries
1. For **TinyGPS++**: Open **Arduino IDE**, go to **Sketch** > **Include Library** > **Manage Libraries**, search for **TinyGPS++**, and click **Install**.
2. For **M5EPD**: Open **Arduino IDE**, go to **Sketch** > **Include Library** > **Manage Libraries**, search for **M5EPD**, and click **Install**.


## Board Setup

1. In the **Arduino IDE**, go to **Tools** > **Board** and select **M5Paper**.
2. Make sure the correct port is selected under **Tools** > **Port**.

## Project Setup

### Step 1: Clone or download the `.ino` file

Download or clone this repository and open the `.ino` file in the Arduino IDE.

### Step 2: Initialize SD Card

The SD card is used to log data about detected WiFi and BLE devices along with GPS data. Ensure you have an SD card formatted and ready to use.

### Step 3: Upload Code

After configuring your board and libraries, upload the code to your M5Paper by clicking the **Upload** button in the Arduino IDE.

### Step 4: View Output

The M5Paper will begin scanning for WiFi and BLE devices, displaying the results sorted by RSSI on its e-paper screen. The GPS data, along with WiFi and BLE scan results, will be logged to a CSV file on the SD card.

## CSV Log Format

The following data is logged to the SD card in CSV format:

1. **MAC** - Device MAC address
2. **SSID** - WiFi network name (if applicable)
3. **AuthMode** - Authentication type (for WiFi)
4. **FirstSeen** - Timestamp when the device was first detected
5. **Channel** - WiFi channel
6. **RSSI** - Signal strength
7. **Latitude** - GPS latitude
8. **Longitude** - GPS longitude
9. **AltitudeMeters** - Altitude from GPS
10. **AccuracyMeters** - Accuracy of GPS data
11. **Type** - WiFi or BLE

## Disclaimer

This project is provided "as-is" without any express or implied warranty. In no event shall the author or contributors be held liable for any damages arising from the use of this software. By using this code, you agree to take full responsibility for any consequences, direct or indirect, resulting from its use. 

Use at your own risk. This software is open-source and intended for educational and experimental purposes only. The author disclaims any responsibility for any issues that may arise from the use of this project, including, but not limited to, loss of data, hardware damage, or violations of legal regulations.

For further information, please review the terms of the [MIT License](./LICENSE).


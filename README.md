# M5Paper Wardriver

This project scans for WiFi and BLE devices using the M5Paper, logs the data to an SD card, and displays it on the M5Paper's e-paper screen. It also integrates GPS and logs all seen devices to a wigle.net compatible CSV file. 

## Prerequisites

Before starting, ensure you have the following components and software set up:

1. **M5Paper Unit**
2. **M5 GPS i2c Unit & Cable**
3. **SD Card** (formatted and ready for use)
4. **Arduino IDE** (latest version)
5. **ESP32 Board Package** - Installed via the **Boards Manager**:
   - Open Arduino IDE and go to **File** > **Preferences**.
   - In the "Additional Board Manager URLs" field, add:  
     `https://dl.espressif.com/dl/package_esp32_index.json`.
   - Go to the **Sidebar** > **Boards Manager**, search for `ESP32`, and click **Install** next to **ESP32 by Espressif Systems**.

## Libraries Needed

The ESP32 board package includes some essential libraries, while others must be manually installed through the **Library Manager**.

### Bundled Libraries (Included with ESP32 Core)
1. **WiFi** - For scanning and connecting to WiFi networks.
2. **SD** - For handling SD card operations.
3. **SPI** - For SPI communication.
4. **ESP32 BLE Arduino** - For Bluetooth Low Energy (BLE) functionality.

### External Libraries (Requires Installation)
1. **M5EPD** - For M5Paper-specific functionality.
2. **TinyGPS++** - For handling GPS data.

### Installing External Libraries
1. For **TinyGPS++**:
   - In Arduino IDE, open the **Library Manager** via the **Sidebar** or **Sketch** > **Include Library** > **Manage Libraries**.
   - Search for **TinyGPS++** and click **Install**.
2. For **M5EPD**:
   - Similarly, search for **M5EPD** in the **Library Manager** and click **Install**.

## Board Setup

1. In **Arduino IDE**, go to **Tools** > **Board** and select **M5Paper** from the ESP32 list.
2. Ensure the correct port is selected under **Tools** > **Port** (e.g., COM3 on Windows or /dev/cu.SLAB_USBtoUART on macOS).

## Project Setup

### Step 1: Clone or Download the `.ino` File

Download or clone the required code repository. Open the `.ino` file in Arduino IDE to prepare for uploading.

### Step 2: Initialize SD Card

The SD card will be used to log detected WiFi networks, BLE devices, and GPS data. Make sure the card is formatted and ready for use in the M5Paper.

### Step 3: Upload Code

After verifying that your board, port, and libraries are properly configured, click the **Upload** button in Arduino IDE to flash the code onto your M5Paper.

Once uploaded, your M5Paper will start logging WiFi, BLE, and GPS data to the SD card.



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


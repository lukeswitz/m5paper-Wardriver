#include <M5EPD.h>
#include <SD.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

M5EPD_Canvas canvas(&M5.EPD);
#define SD_CS_PIN 4

const String BUILD = "1.0.1";
const String VERSION = "1.0";

HardwareSerial GPS_Serial(1);
TinyGPSPlus gps;

struct GPSData {
  String time;
  double latitude;
  double longitude;
  double altitude;
  double accuracy;
};

BLEScan* pBLEScan;
int scanTime = 5;

File logFile;
String logFileName = "/M5Paper-Wireless-Scan-.csv";

struct Device {
  char type[6];
  char ssid[33];
  char mac[18];
  int rssi;
  char info[128];
};

Device deviceList[150];  // Maximum 150 devices
int deviceIndex = 0;     // Current device count
int startIndex = 0;      // Index of the first device to display
int devicesPerPage = 0;  // Number of devices that fit on one screen


volatile bool needDisplayUpdate = false;

// Helper to get GPS data
GPSData getGPSData() {
  GPSData gpsData;
  char utc[21];
  sprintf(utc, "%04d-%02d-%02d %02d:%02d:%02d", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
  gpsData.time = String(utc);
  gpsData.latitude = gps.location.lat();
  gpsData.longitude = gps.location.lng();
  gpsData.altitude = gps.altitude.meters();
  gpsData.accuracy = gps.hdop.hdop();
  return gpsData;
}

// Write CSV header
void writeCSVHeader() {
  if (logFile) {
    logFile.println("WigleWifi-1.4,appRelease=" + BUILD + ",model=M5Paper,release=" + VERSION + ",device=M5Paper,display=ePaper,board=ESP32,brand=M5");
    logFile.println("MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,Type");
    logFile.flush();
  }
}

// Initialize SD card
bool initSDCard() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    return false;
  }
  logFile = SD.open(logFileName, FILE_WRITE);
  if (!logFile) {
    Serial.println("Error opening log file!");
    return false;
  }
  writeCSVHeader();
  return true;
}

// Log device data to CSV
void logToCSV(const char* netid, const char* ssid, const char* authType, const char* time, int channel, int signal, double lat, double lon, double altitude, double accuracy, const char* type) {
  if (logFile) {
    logFile.printf("%s,\"%s\",%s,%s,%d,%d,%.6f,%.6f,%.2f,%.2f,%s\n",
                   netid, ssid, authType, time, channel, signal, lat, lon, altitude, accuracy, type);
    logFile.flush();
  }
}

// Check if device with the same MAC address already exists
bool isDuplicate(const char* mac) {
  for (int i = 0; i < deviceIndex; i++) {
    if (strcmp(deviceList[i].mac, mac) == 0) {
      return true;  // Duplicate device
    }
  }
  return false;
}

// Function to find the index of the device with the lowest RSSI
int findLowestRSSIIndex() {
  int minIndex = 0;
  int minRSSI = deviceList[0].rssi;
  for (int i = 1; i < deviceIndex; i++) {
    if (deviceList[i].rssi < minRSSI) {
      minRSSI = deviceList[i].rssi;
      minIndex = i;
    }
  }
  return minIndex;
}

// Function to remove a device from the list
void removeDevice(int index) {
  if (index < 0 || index >= deviceIndex) {
    return;  // Invalid index
  }
  // Shift devices to fill the gap
  for (int i = index; i < deviceIndex - 1; i++) {
    deviceList[i] = deviceList[i + 1];
  }
  deviceIndex--;
}

// Function to add a device at a specific index
void addDeviceAtIndex(int index, const char* type, const char* ssid, const char* mac, int rssi, int channel = 0, const char* encryption = "") {
  strncpy(deviceList[index].type, type, sizeof(deviceList[index].type));
  strncpy(deviceList[index].ssid, ssid, sizeof(deviceList[index].ssid));
  strncpy(deviceList[index].mac, mac, sizeof(deviceList[index].mac));
  deviceList[index].rssi = rssi;
  if (strcmp(type, "WiFi") == 0) {
    snprintf(deviceList[index].info, sizeof(deviceList[index].info),
             "WiFi: %s [%d] (%s) %d dBm", ssid, channel, encryption, rssi);
  } else {
    snprintf(deviceList[index].info, sizeof(deviceList[index].info),
             "BLE: %s (%s) %d dBm", ssid, mac, rssi);
  }
}

// Function to add or update a device
void addOrUpdateDevice(const char* type, const char* ssid, const char* mac, int rssi, int channel = 0, const char* encryption = "") {
  if (deviceIndex < 150) {
    // There is space in the list, add the new device
    addDeviceAtIndex(deviceIndex, type, ssid, mac, rssi, channel, encryption);
    deviceIndex++;
  } else {
    // The list is full, find the device with the lowest RSSI
    int minIndex = findLowestRSSIIndex();
    if (rssi > deviceList[minIndex].rssi) {
      // Replace the device with the lowest RSSI
      removeDevice(minIndex);
      addDeviceAtIndex(minIndex, type, ssid, mac, rssi, channel, encryption);
    }
  }
  needDisplayUpdate = true;  // Set flag to update display
}

void displayDevices() {
  for (int i = 0; i < deviceIndex - 1; i++) {
    for (int j = i + 1; j < deviceIndex; j++) {
      if (deviceList[i].rssi < deviceList[j].rssi) {
        Device temp = deviceList[i];
        deviceList[i] = deviceList[j];
        deviceList[j] = temp;
      }
    }
  }

  canvas.createCanvas(540, 960);  // Correct full vertical canvas size
  canvas.fillCanvas(0);
  canvas.setTextSize(2);
  canvas.drawString("Discovered Devices (sorted by RSSI):", 10, 10);

  int y = 40;
  for (int i = 0; i < deviceIndex; i++) {
    canvas.drawString(String(i+1) + ": " + deviceList[i].info, 10, y);
    y += 30;
    if (y > canvas.height() - 20) {
      canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
      delay(2000);
      canvas.fillCanvas(0);
      y = 40;
    }
  }

  canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
}

void scrollDevices(bool forward) {
  if (forward) {
    if (startIndex + devicesPerPage < deviceIndex) {
      startIndex += devicesPerPage;
      needDisplayUpdate = true;
    }
  } else {
    if (startIndex - devicesPerPage >= 0) {
      startIndex -= devicesPerPage;
    } else {
      startIndex = 0;
      needDisplayUpdate = true;
    }
  }
}


// BLE callback to handle results
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String macStr = advertisedDevice.getAddress().toString().c_str();
    String ssidStr = advertisedDevice.getName().c_str();
    int rssi = advertisedDevice.getRSSI();

    const char* mac = macStr.c_str();
    const char* ssid = ssidStr.c_str();

    if (isDuplicate(mac)) {
      return;  // Skip duplicate devices
    }

    GPSData gpsData = getGPSData();
    logToCSV(mac, ssid, "", gpsData.time.c_str(), 0, rssi, gpsData.latitude, gpsData.longitude, gpsData.altitude, gpsData.accuracy, "BLE");

    addOrUpdateDevice("BLE", ssid, mac, rssi);
  }
};

// WiFi scan handler
void handleWiFiScan() {
  int n = WiFi.scanNetworks();

  if (n > 0) {
    GPSData gpsData = getGPSData();
    for (int i = 0; i < n; ++i) {
      String ssidStr = WiFi.SSID(i);
      String bssidStr = WiFi.BSSIDstr(i);
      int rssi = WiFi.RSSI(i);
      String encryptionStr = getAuthType(WiFi.encryptionType(i));
      int channel = WiFi.channel(i);

      const char* ssid = ssidStr.c_str();
      const char* bssid = bssidStr.c_str();
      const char* encryption = encryptionStr.c_str();

      if (isDuplicate(bssid)) {
        continue;  // Skip duplicates
      }

      logToCSV(bssid, ssid, encryption, gpsData.time.c_str(), channel, rssi, gpsData.latitude, gpsData.longitude, gpsData.altitude, gpsData.accuracy, "WiFi");

      addOrUpdateDevice("WiFi", ssid, bssid, rssi, channel, encryption);

      yield();
    }
  }
}

// Initialize BLE and WiFi scanning
void initializeScanning() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

// Setup function
void setup() {
  Serial.begin(115200);  // Initialize serial communication for debugging

  M5.begin();
  M5.EPD.SetRotation(1);
  M5.EPD.Clear(true);
  canvas.createCanvas(540, 960);
  canvas.setTextSize(2);

  if (!initSDCard()) {
    Serial.println("Failed to initialize SD card. Halting...");
    while (true) delay(1000);
  }

  GPS_Serial.begin(9600, SERIAL_8N1, 19, 18);
  canvas.drawString("GPS initialized.", 10, 40);
  canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
  delay(2000);

  canvas.createCanvas(540, 960);
  canvas.setTextSize(2);

  // Calculate devices per page based on canvas height and text spacing
  devicesPerPage = (canvas.height() - 60) / 30;  // Adjust 60 and 30 if needed

  initializeScanning();
}

// Main loop
unsigned long previousMillis = 0;
const unsigned long interval = 2000;  // 2 seconds

void loop() {
  // Update button states
  M5.update();

  if (M5.BtnL.wasPressed()) {
    scrollDevices(false);  // Scroll up
  }

  if (M5.BtnR.wasPressed()) {
    scrollDevices(true);  // Scroll down
  }

  while (GPS_Serial.available() > 0) {
    char c = GPS_Serial.read();
    gps.encode(c);
  }

  if (!gps.location.isValid()) {
    Serial.println("Waiting for valid GPS data...");
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    handleWiFiScan();
    pBLEScan->start(scanTime, false);  // Start BLE scan
    pBLEScan->clearResults();          // Clear BLE scan results after logging
  }

  if (needDisplayUpdate) {
    displayDevices();
    needDisplayUpdate = false;
  }

  // Allow background tasks to run
  delay(1);
}

const char* getAuthType(uint8_t wifiAuth) {
  switch (wifiAuth) {
    case WIFI_AUTH_OPEN:
      return "[OPEN]";
    case WIFI_AUTH_WEP:
      return "[WEP]";
    case WIFI_AUTH_WPA_PSK:
      return "[WPA_PSK]";
    case WIFI_AUTH_WPA2_PSK:
      return "[WPA2_PSK]";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "[WPA_WPA2_PSK]";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "[WPA2_ENTERPRISE]";
    case WIFI_AUTH_WPA3_PSK:
      return "[WPA3_PSK]";
    case WIFI_AUTH_WPA2_WPA3_PSK:
      return "[WPA2_WPA3_PSK]";
    case WIFI_AUTH_WAPI_PSK:
      return "[WAPI_PSK]";
    default:
      return "[UNKNOWN]";
  }
}

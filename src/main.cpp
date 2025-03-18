#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_TMP117.h>
#include <Adafruit_Sensor.h>

#define BLINK_INTERVAL 500
#define STREAM_INTERVAL 200

#define STREAM_TIMOUT 60000

#define LED_BUILTIN_PIN 2

BLEService myService("fff0");
BLEIntCharacteristic dataCharacteristic("fff2", BLERead | BLENotify); // "Channel" explicitely for publishing measurement stream
BLEStringCharacteristic commandCharacteristic("fff1", BLEWrite, 32);  // "Channel" explicitely for retrieving commands from central
String deviceName;
bool streaming = false;

Adafruit_TMP117 tmp117;

unsigned long streamingStartTimestamp = 0;

unsigned long lastBlinkTimestamp = 0;

String getMACSuffix()
{
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8)
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return String(chipId);
}

void setupCharacterics()
{
  deviceName = "ovulize-" + getMACSuffix();
  BLE.setLocalName(deviceName.c_str());
  BLE.setDeviceName(deviceName.c_str());
  myService.addCharacteristic(dataCharacteristic);
  myService.addCharacteristic(commandCharacteristic);
}

void setStream(bool pStreaming)
{
  if (streaming == pStreaming)
    return;
  streaming = pStreaming;
  if (streaming)
  {
    streamingStartTimestamp = millis();
    Serial.println("Starting temperature stream.");
  }
  else
  {
    Serial.println("Stopping temperature stream.");
  }
}

void generateValueStream()
{

  sensors_event_t lTemp;   // create an empty event to be filled
  tmp117.getEvent(&lTemp); // fill the empty event object with the current measurements
  dataCharacteristic.writeValue((int)(lTemp.temperature * 100));
  if (millis() - streamingStartTimestamp > STREAM_TIMOUT)
  {
    setStream(false);
  }
}

void initializeBLE()
{
  if (!BLE.begin())
  {
    Serial.println("Failed to initialize BLE!");
    while (1)
      ;
  }
  setupCharacterics();
  BLE.addService(myService);
  BLE.setAdvertisedService(myService);

  BLE.advertise();
  Serial.println("Waiting for connection. Pair for communication: " + deviceName);
}

void showWaitingAnimation()
{
  if (millis() - lastBlinkTimestamp > BLINK_INTERVAL)
  {
    lastBlinkTimestamp = millis();
    digitalWrite(LED_BUILTIN_PIN, !digitalRead(LED_BUILTIN_PIN));
  }
}

void handleConnection(BLEDevice pCentral)
{
  Serial.print("Connected to app: ");
  Serial.println(pCentral.address());

  while (pCentral.connected())
  {
    BLE.poll();

    if (commandCharacteristic.written())
    {
      String command = commandCharacteristic.value();
      if (command == "startTemperatureStream")
      {
        setStream(true);
      }
      else if (command == "stopTemperatureStream")
      {
        setStream(false);
      }
    }

    if (streaming)
    {
      generateValueStream();
    }
  }
  setStream(false);
  Serial.print("Disconnected from app: ");
  Serial.println(pCentral.address());
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  while (!Serial)
    ;

  if (!tmp117.begin())
  {
    Serial.println("Failed to find TMP117 chip");
    while (1)
    {
      delay(10);
    }
  }

  initializeBLE();
}

void loop()
{

  BLEDevice lCentral = BLE.central();

  if (lCentral)
  {
    handleConnection(lCentral);
  }
  else
  {
    showWaitingAnimation();
  }

  BLE.poll();
}

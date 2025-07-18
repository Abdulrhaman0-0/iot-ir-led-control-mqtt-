# iot-ir-led-control-mqtt

This IoT project uses an **ESP8266** (or **ESP32**) microcontroller to monitor an **IR obstacle sensor** and control a **relay or LED** remotely via **MQTT protocol**. Built as part of the IoT workshop led by **Eng. Omar Barakat**.

---

## 📌 Features

- IR sensor detection: CLEAR or OBSTACLE
- Control relay or LED via MQTT commands
- Local MQTT broker hosted on Raspberry Pi
- Dashboard on Android phone using IoT MQTT Panel
- Compatible with both ESP8266 and ESP32

---

## 📱 Compatibility

| Feature           | ESP8266             | ESP32                   |
|------------------|---------------------|--------------------------|
| Wi-Fi Library     | `<ESP8266WiFi.h>`   | `<WiFi.h>`               |
| Pin Naming        | D5, D6              | GPIO numbers (e.g. 14)   |
| Voltage           | 3.3V                | 3.3V                     |

---

## 🧰 Hardware Components

| Component         | Description                                |
|------------------|--------------------------------------------|
| ESP8266 / ESP32  | Microcontroller board                      |
| IR Sensor         | Digital obstacle detection sensor          |
| Relay or LED      | Output controlled via MQTT                 |
| Raspberry Pi      | Hosts Mosquitto MQTT broker                |
| Jumper Wires      | Circuit connections                        |
| Breadboard        | Assembly base                              |

---

## 🔌 Circuit Connections

| Device         | ESP8266 Pin | ESP32 Pin (example) |
|----------------|-------------|----------------------|
| IR Sensor OUT  | D5          | GPIO14               |
| Relay / LED IN | D6          | GPIO12               |
| VCC / GND      | 3.3V / GND  | 3.3V / GND           |

---

## 💬 MQTT Topics

| Topic                      | Direction  | Description                  |
|---------------------------|------------|------------------------------|
| `/sectorb5/ABDO/led`      | Subscribe  | Receive On/Off commands      |
| `/sectorb5/ABDO/irsensor` | Publish    | Send IR sensor status        |

---

## ⚙️ System Behavior

1. ESP connects to Wi-Fi and to the MQTT broker on Raspberry Pi.
2. Publishes IR sensor status every second as `"CLEAR"` or `"OBSTACLE"`.
3. Listens for `"On"` / `"Off"` messages to control the relay/LED.
4. Reconnects to broker if connection is lost.

---

## 📱 IoT MQTT Panel (Android App Setup)

App: **IoT MQTT Panel** – free from Google Play

### Widgets Added:

| Widget Type   | Topic                      | Function                      |
|---------------|----------------------------|-------------------------------|
| Switch        | `/sectorb5/ABDO/led`       | Toggle relay / LED            |
| LED Indicator | `/sectorb5/ABDO/irsensor`  | Green for "CLEAR", Red for "OBSTACLE" |
| Text Log      | `/sectorb5/ABDO/irsensor`  | Shows status history & debug  |

### Connection:

- Broker IP: your Raspberry Pi IP (e.g. `10.252.0.156`)
- Port: `1883`

---

## 🔧 Raspberry Pi as MQTT Broker (Mosquitto Setup)

### 🛠️ Installation

```bash
sudo apt update
sudo apt install mosquitto mosquitto-clients
```

---

### ⚙️ Quick Configuration for LAN Access

Create a config file to allow MQTT clients on the same network:

```bash
sudo nano /etc/mosquitto/conf.d/default.conf
```

Add the following lines:

```conf
listener 1883
allow_anonymous true
```

Save and exit:
- `Ctrl + O` → `Enter`
- `Ctrl + X`

Restart the service:

```bash
sudo systemctl restart mosquitto
```

---

### 🔥 If MQTT client can't connect (Error: ECONNREFUSED)

1. Make sure Mosquitto is listening to LAN:
   - Double-check config above.
2. Open port 1883 on firewall:
   ```bash
   sudo ufw allow 1883
   sudo ufw reload
   ```
   Or, if UFW not enabled:
   ```bash
   sudo ufw enable
   sudo ufw allow 1883
   ```

---

### ✅ How to Test the Broker

From any device:

```bash
mosquitto_sub -h <raspberry_ip> -t test/topic
mosquitto_pub -h <raspberry_ip> -t test/topic -m "hello"
```

If you see the message arrive → your broker is working 🎉

---

## 📷 Images


📸 IoT MQTT Panel Dashboard:  
`dashboard_screenshot.jpg` 

---

## ✅ Project Checklist

- [x] Code tested on ESP8266
- [x] Code works with ESP32
- [x] MQTT broker running locally
- [x] IR sensor publishing data
- [x] Mobile app dashboard created
- [x] README written and formatted

---

## 👨‍💻 Author

**Abdulrahman Ahmed Elsaid**  (https://www.linkedin.com/in/abdulrahman-ahmed-a90418249/)
IoT Final Project | Sector B5  
Instructor: [Eng. Omar Barakat](https://www.linkedin.com/in/omarbarakota/)

---

## 📁 Included Files

- `main.ino` – Arduino source code (with ESP32 support)
- `README.md` – Full documentation
- `broker.jpg` – Raspberry Pi MQTT broker image
- `dashboard_screenshot.jpg` –  phone dashboard screenshot

---

## 📃 License

This project is open-source and available under the [MIT License](https://opensource.org/licenses/MIT) for educational use.

---

> _"Built with code, curiosity, and a little MQTT magic."_

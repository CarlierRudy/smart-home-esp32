# 🏠 Smart Home ESP32

Système domotique embarqué sur ESP32 : surveillance environnementale (température, humidité, gaz, eau, lumière) et contrôle à distance (éclairage, serrure, portail, RFID) via protocole MQTT over WiFi.

---

## 📋 Table des Matières

- [Aperçu](#aperçu)
- [Matériel requis](#matériel-requis)
- [Brochage (Pinout)](#brochage-pinout)
- [Dépendances](#dépendances)
- [Installation & Configuration](#installation--configuration)
- [Topics MQTT](#topics-mqtt)
- [Structure du projet](#structure-du-projet)
- [Bugs connus](#bugs-connus)

---

## Aperçu

Le firmware tourne sur un **ESP32** avec le framework **Arduino** via **PlatformIO**. Il se connecte à un réseau WiFi et communique avec un broker MQTT public (`broker.emqx.io`). Trois modules principaux composent l'architecture :

| Module | Fichiers | Rôle |
|---|---|---|
| `Communication` | `communication.cpp/.h` | WiFi, connexion MQTT, publish/subscribe |
| `Environment` | `environment.cpp/.h` | Lecture périodique des 5 capteurs |
| `Controller` | `controller.cpp/.h` | Pilotage des actionneurs, RFID, LED |

---

## Matériel Requis

- ESP32 (board `esp32dev`)
- Capteur température/humidité **DHT11**
- Capteur de gaz analogique (série **MQ**)
- Capteur de lumière (LDR numérique)
- Capteur de niveau d'eau (analogique)
- Capteur de mouvement **PIR** *(déclaré, non utilisé dans le code actuel)*
- Module RFID **MFRC522** (SPI)
- Servo-moteur (portail)
- 2 relays (éclairage + serrure de porte)
- LED de statut intégrée (GPIO 2)

---

## Brochage (Pinout)

| Composant | GPIO ESP32 |
|---|---|
| LED de statut | 2 |
| Buzzer *(non utilisé)* | 17 |
| Capteur de lumière (digital) | 16 |
| DHT11 (température / humidité) | 4 |
| Capteur de gaz (ADC) | 33 |
| Capteur de mouvement PIR | 27 |
| Capteur niveau d'eau (ADC) | 32 |
| Relay éclairage | 25 |
| Relay serrure de porte | 26 |
| Servo portail (PWM) | 21 |
| MFRC522 SS/SDA | 5 |
| MFRC522 RST | 22 |

> Le bus SPI utilise les pins hardware par défaut de l'ESP32 (MOSI: 23, MISO: 19, SCK: 18).

---

## Dépendances

Gérées automatiquement par PlatformIO via `platformio.ini` :

```ini
lib_deps =
    adafruit/DHT sensor library@^1.4.6
    hideakitai/MQTTPubSubClient@^0.3.2
    miguelbalboa/MFRC522@^1.4.12
    adafruit/Adafruit Unified Sensor@^1.1.15
    madhephaestus/ESP32Servo@3.1.2
```

---

## Installation & Configuration

### 1. Cloner le dépôt

```bash
git clone https://github.com/<votre-username>/smart-home-esp32.git
cd smart-home-esp32
```

### 2. Modifier `src/config.h`

> ⚠️ **Ne pas committer ce fichier avec vos vraies credentials** — il est listé dans `.gitignore`.

```cpp
// WiFi
#define WIFI_SSID "VOTRE_SSID"
#define WIFI_PASS "VOTRE_MOT_DE_PASSE"

// MQTT — doit être unique par nœud
#define MQTT_CLIENT_ID    "smart-home-XX"
#define MQTT_BROKER_HOST  "broker.emqx.io"
#define MQTT_BROKER_PORT  1883
#define MQTT_TOPIC_PREFIX "smart-XX"

// Intervalles & seuils
#define ENVIRONMENT_DATA_UPDATE_INTERVAL 3000  // ms
#define PROPANE_MIN_VALUE 1000
#define PROPANE_MAX_VALUE 3000
#define DOOR_UNLOCK_TIMEOUT 3000  // ms
```

### 3. Compiler et flasher

```bash
# Avec PlatformIO CLI
pio run --target upload

# Ou via VS Code : bouton → dans la barre PlatformIO
```

### 4. Surveiller le port série

```bash
pio device monitor --baud 115200
```

Vous devriez voir :
```
Booting...
Connecting to WiFi: VOTRE_SSID ....
Connected! IP: 192.168.x.x
Connecting to MQTT(TCP) broker...
MQTT connection established !
```

---

## Topics MQTT

Tous les topics sont préfixés par la valeur de `MQTT_TOPIC_PREFIX` (ex. `smart-14`).

### 📤 Publications (ESP32 → Broker)

| Topic | Payload | Rétention |
|---|---|---|
| `{prefix}/state` | `connected` / `disconnected` | Will MQTT |
| `{prefix}/light/level` | `0` ou `100` | Non |
| `{prefix}/temperature` | valeur °C (entier) | Oui |
| `{prefix}/humidity` | valeur % (entier) | Oui |
| `{prefix}/water/level` | `0` à `100` % | Oui |
| `{prefix}/light/state` | `true` / `false` | Non |
| `{prefix}/door/locked` | `true` / `false` | Non |
| `{prefix}/gate/opened` | `true` / `false` | Non |
| `{prefix}/rfid/uid` | UID de la carte (String) | Non |

### 📥 Souscriptions (Broker → ESP32)

| Topic | Payload attendu | Effet |
|---|---|---|
| `{prefix}/light/switch` | `true` / `false` | Active / éteint l'éclairage |
| `{prefix}/door/lock` | `true` / `false` | Verrouille / déverrouille la porte |
| `{prefix}/gate/open` | `true` / `false` | Ouvre / ferme le portail |
| `{prefix}/env/update` | Entier (secondes) | Modifie l'intervalle de refresh des capteurs |

> 💡 La porte se reverrouille automatiquement après `DOOR_UNLOCK_TIMEOUT` ms (3 s par défaut).

---

## Structure du Projet

```
smart-home-esp32/
├── src/
│   ├── main.cpp              # Boucle principale, orchestration
│   ├── communication.cpp/.h  # WiFi + client MQTT
│   ├── controller.cpp/.h     # Actionneurs + RFID
│   ├── environment.cpp/.h    # Capteurs environnementaux
│   ├── config.h              # ⚠️ Credentials & constantes (à ne pas committer)
│   └── pins.h                # Mapping GPIO
├── .vscode/
│   ├── extensions.json
│   └── launch.json
├── platformio.ini
└── README.md
```

---

## Bugs Connus

| Fichier | Ligne | Description | Correction |
|---|---|---|---|
| `environment.cpp` | détection propane | Opérateur `>=` au lieu de `<=` pour `PROPANE_MAX_VALUE` | `gasReading >= PROPANE_MIN_VALUE && gasReading <= PROPANE_MAX_VALUE` |
| `environment.cpp` | flag propane | `m_propaneChanged = m_propaneChanged` (auto-assignation, flag jamais mis à `true`) | `m_propaneChanged = true` |
| `communication.cpp` | `isConnected()` | Retourne toujours `false` | Implémenter : `return WiFi.status() == WL_CONNECTED && m_mqtt.isConnected()` |
| `main.cpp` | loop | Données propane jamais publiées via MQTT | Ajouter le bloc `if (Env.propaneHasChanged())` |

---

## Licence

Ce projet est distribué à des fins pédagogiques.

# Home Assistant Dashboard for Project Aura

This folder contains a ready-to-use YAML configuration to visualize your Aura data.
It uses only standard Home Assistant cards. No HACS or external dependencies required.
The example view includes dedicated `CO2` and `AQI` gauge cards at the top, plus a
standard Home Assistant ventilation section with explicit `Auto` / `Manual` / `Stop`
mode switches, a manual speed slider in percent, a timer selector, and a timer
remaining indicator for the optional DAC-based exhaust output.

![Dashboard Preview](../assets/preview.png)

## How to Add
You can add this as a new view (tab) to your existing dashboard.

1. Open your Home Assistant Dashboard.
2. Click the Pencil icon (Edit Dashboard) in the top right.
3. Click the 3 dots menu (top right) -> Raw configuration editor.
4. Scroll to the place where you want to insert the code (or replace existing code if starting fresh).
5. Paste the contents of `dashboard.yaml`.
6. Click Save.

## MQTT Setup & Connection
Based on user findings, follow these steps to ensure the device communicates with Home Assistant:

### 1. Install the Broker
*   In Home Assistant, go to **Settings > Add-ons / Apps**.
*   Search for and install the **Mosquitto Broker**.
*   Start the Add-on (standard settings are usually sufficient).

### 2. Create the Credentials
*   Go to **Settings > People > Users**.
*   Click **Add User** (Create a dedicated user, e.g., `aura_mqtt`).
*   **Note:** This user is created in the main Home Assistant settings, not within the Mosquitto app settings.

### 3. Configure the Aura Device
*   Open **Settings > MQTT** on Aura device
*   Access the Aura Web UI at `http://[DEVICE_IP]/mqtt`
*   **Broker Address:** Set to **YOUT HOME ASSISTANT IP ADDRESS**.
*   **Broker Port:** Set to **1883**. (Do not use 8123).
*   **Username/Password:** Use the credentials for the user created in Step 2.
*   **Discovery:** Check **Enable Home Assistant Discovery**
*   Click **Save**. The device should now handshake with the broker.

## Troubleshooting
If sensors show as "Unavailable" or logs show "Not Authorised":
*   **Logs:** Check the Mosquitto Broker logs in HA. If you see "Connection closed by client," verify the **Client ID** in the Aura UI is unique.
*   **IP Address:** Ensure the Broker IP in the Aura settings is the internal IP of your Home Assistant instance.
*   **Prefix:** Ensure the discovery prefix in both Aura and HA is set to the default `homeassistant`.
*   Be patient, it might take several minutes to connect.

## Entity Configuration
Entity IDs are derived from your `MQTT base topic`.

If your base topic is the default `project_aura`, the example YAML should match directly:
- `sensor.project_aura_temperature`
- `switch.project_aura_fan_auto`
- `select.project_aura_fan_timer`

If you use a custom base topic, replace the `project_aura_` prefix in the YAML with the
slugged base topic. For example:
- base topic `project_aura_kitchen` -> `sensor.project_aura_kitchen_temperature`
- base topic `project_aura/bedroom` -> `sensor.project_aura_bedroom_temperature`

If you see "Entity not found" warnings:
1. Go to Settings -> Devices & Services -> Entities.
2. Search for your MQTT base topic or `Aura`.
3. Open the dashboard code and use Find & Replace to swap the prefix.

## Multiple Aura Devices
Two Aura devices must use different `MQTT base topic` values.

This is important for two reasons:
1. State and command topics are built from the base topic.
2. Home Assistant entity IDs are also derived from the base topic.

Recommended examples:
- `project_aura_kitchen`
- `project_aura_bedroom`
- `project_aura_office`

## Events Sensor

Aura now mirrors the same event stream shown in the built-in web dashboard `Events` tab to Home Assistant over MQTT.

Discovery creates one additional entity derived from your MQTT base topic, for example:
- `sensor.project_aura_room1_events`

Behavior:
- Sensor state is the human-readable event message.
- Attributes include `ts_ms`, `level`, `severity`, and `type`.
- The entity uses `force_update`, so repeated identical messages are still recorded as new state changes when they are not deduplicated by firmware.
- This entity is intended for Home Assistant `Activity` / `logbook` views.
- Long-term event history in Home Assistant is controlled by `recorder`, not by the firmware queue size.
- The final `entity_id` depends on your configured MQTT base topic, so names in `dashboard.yaml` may need to be adjusted to match the entities discovered in your own Home Assistant instance.

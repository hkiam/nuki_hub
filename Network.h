#pragma once

#include <Preferences.h>
#include <vector>
#include <map>
#include "networkDevices/NetworkDevice.h"
#include "MqttReceiver.h"
#include "networkDevices/IPConfiguration.h"
#include "MqttTopics.h"

enum class NetworkDeviceType
{
    WiFi,
    W5500,
    Olimex_LAN8720,
    WT32_LAN8720,
    M5STACK_PoESP32_Unit,
    LilyGO_T_ETH_POE
};

#define JSON_BUFFER_SIZE 1024

class Network
{
public:
    explicit Network(Preferences* preferences, const String& maintenancePathPrefix, char* buffer, size_t bufferSize);

    void initialize();
    bool update();
    void registerMqttReceiver(MqttReceiver* receiver);
    void reconfigureDevice();
    void setMqttPresencePath(char* path);
    void disableAutoRestarts(); // disable on OTA start
    void disableMqtt();

    void subscribe(const char* prefix, const char* path);
    void initTopic(const char* prefix, const char* path, const char* value);
    void publishFloat(const char* prefix, const char* topic, const float value, const uint8_t precision = 2);
    void publishInt(const char* prefix, const char* topic, const int value);
    void publishUInt(const char* prefix, const char* topic, const unsigned int value);
    void publishULong(const char* prefix, const char* topic, const unsigned long value);
    void publishBool(const char* prefix, const char* topic, const bool value);
    bool publishString(const char* prefix, const char* topic, const char* value);

    void publishHASSConfig(char* deviceType, const char* baseTopic, char* name, char* uidString, const bool& hasKeypad, char* lockAction, char* unlockAction, char* openAction, char* lockedState, char* unlockedState);
    void publishHASSConfigBatLevel(char* deviceType, const char* baseTopic, char* name, char* uidString);
    void publishHASSConfigDoorSensor(char* deviceType, const char* baseTopic, char* name, char* uidString, char* lockAction, char* unlockAction, char* openAction, char* lockedState, char* unlockedState);
    void publishHASSConfigRingDetect(char* deviceType, const char* baseTopic, char* name, char* uidString);
    void publishHASSConfigLedBrightness(char* deviceType, const char* baseTopic, char* name, char* uidString);
    void publishHASSConfigSoundLevel(char* deviceType, const char* baseTopic, char* name, char* uidString);
    void publishHASSConfigAccessLog(char* deviceType, const char* baseTopic, char* name, char* uidString);
    void publishHASSConfigKeypadAttemptInfo(char* deviceType, const char* baseTopic, char* name, char* uidString);
    void publishHASSWifiRssiConfig(char* deviceType, const char* baseTopic, char* name, char* uidString);
    void publishHASSBleRssiConfig(char* deviceType, const char* baseTopic, char* name, char* uidString);
    void removeHASSConfig(char* uidString);
    void removeHASSConfigTopic(char* deviceType, char* name, char* uidString);

    void clearWifiFallback();

    void publishPresenceDetection(char* csv);

    int mqttConnectionState(); // 0 = not connected; 1 = connected; 2 = connected and mqtt processed
    bool encryptionSupported();
    const String networkDeviceName() const;

    const NetworkDeviceType networkDeviceType();

    uint16_t subscribe(const char* topic, uint8_t qos);

    void setKeepAliveCallback(std::function<void()> reconnectTick);
    void addReconnectedCallback(std::function<void()> reconnectedCallback);

private:
    static void onMqttDataReceivedCallback(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total);
    void onMqttDataReceived(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total);
    void setupDevice();
    bool reconnect();

    void publishHassTopic(const String& mqttDeviceType,
                          const String& mattDeviceName,
                          const String& uidString,
                          const String& uidStringPostfix,
                          const String& displayName,
                          const String& name,
                          const String& baseTopic,
                          const String& stateTopic,
                          const String& deviceType,
                          const String& deviceClass,
                          const String& stateClass = "",
                          const String& entityCat = "",
                          const String& commandTopic = "",
                          std::vector<std::pair<char*, char*>> additionalEntries = {}
                          );

    void removeHassTopic(const String& mqttDeviceType, const String& mattDeviceName, const String& uidString);

    void onMqttConnect(const bool& sessionPresent);
    void onMqttDisconnect(const espMqttClientTypes::DisconnectReason& reason);

    void buildMqttPath(const char* prefix, const char* path, char* outPath);

    static Network* _inst;

    const char* _lastWillPayload = "offline";
    char _mqttConnectionStateTopic[211] = {0};

    Preferences* _preferences;
    IPConfiguration* _ipConfiguration = nullptr;
    String _hostname;
    char _hostnameArr[101] = {0};
    NetworkDevice* _device = nullptr;
    int _mqttConnectionState = 0;
    bool _connectReplyReceived = false;

    unsigned long _nextReconnect = 0;
    char _mqttBrokerAddr[101] = {0};
    char _mqttUser[31] = {0};
    char _mqttPass[31] = {0};
    char _mqttPresencePrefix[181] = {0};
    char _maintenancePathPrefix[181] = {0};
    int _networkTimeout = 0;
    std::vector<MqttReceiver*> _mqttReceivers;
    char* _presenceCsv = nullptr;
    bool _restartOnDisconnect = false;
    bool _firstConnect = true;
    bool _publishDebugInfo = false;
    std::vector<String> _subscribedTopics;
    std::map<String, String> _initTopics;

    unsigned long _lastConnectedTs = 0;
    unsigned long _lastMaintenanceTs = 0;
    unsigned long _lastRssiTs = 0;
    bool _mqttEnabled = true;
    static unsigned long _ignoreSubscriptionsTs;
    long _rssiPublishInterval = 0;

    char* _buffer;
    const size_t _bufferSize;

    std::function<void()> _keepAliveCallback = nullptr;
    std::vector<std::function<void()>> _reconnectedCallbacks;

    NetworkDeviceType _networkDeviceType  = (NetworkDeviceType)-1;

    int8_t _lastRssi = 127;
};

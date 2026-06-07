#pragma once

#include <memory>
#include <espMqttClient.h>
#include <battery/Provider.h>
#include <battery/mqtt/Stats.h>

namespace Batteries::Mqtt {

class Provider : public ::Batteries::Provider {
public:
    Provider() = default;

    bool init() final;
    void deinit() final;
    void loop() final;
    std::shared_ptr<::Batteries::Stats> getStats() const final { return _stats; }
    std::shared_ptr<HassIntegration> getHassIntegration() final { return nullptr; }

private:
    String _socTopic;
    String _socBackupTopic;
    String _voltageTopic;
    String _currentTopic;
    String _dischargeCurrentLimitTopic;
    String _chargeCurrentLimitTopic;
    String _solarInputPowerTopic;
    String _solarInputPowerBackupTopic;
    String _lowestCellVoltageTopic;
    String _lowestCellVoltageBackupTopic;
    std::shared_ptr<Stats> _stats = std::make_shared<Stats>();
    uint8_t _socPrecision = 0;
    uint8_t _currentPrecision = 0;

    void onMqttMessageSoC(espMqttClientTypes::MessageProperties const& properties,
            char const* topic, uint8_t const* payload, size_t len,
            char const* jsonPath, bool backup);
    void onMqttMessageVoltage(espMqttClientTypes::MessageProperties const& properties,
            char const* topic, uint8_t const* payload, size_t len,
            char const* jsonPath);
    void onMqttMessageCurrent(espMqttClientTypes::MessageProperties const& properties,
            char const* topic, uint8_t const* payload, size_t len,
            char const* jsonPath);
    void onMqttMessageDischargeCurrentLimit(espMqttClientTypes::MessageProperties const& properties,
            char const* topic, uint8_t const* payload, size_t len,
            char const* jsonPath);
    void onMqttMessageChargeCurrentLimit(espMqttClientTypes::MessageProperties const& properties,
            char const* topic, uint8_t const* payload, size_t len,
            char const* jsonPath);
    void onMqttMessageSolarInputPower(espMqttClientTypes::MessageProperties const& properties,
            char const* topic, uint8_t const* payload, size_t len,
            char const* jsonPath, bool backup);
    void onMqttMessageLowestCellVoltage(espMqttClientTypes::MessageProperties const& properties,
            char const* topic, uint8_t const* payload, size_t len,
            char const* jsonPath, bool backup);
    uint8_t calculatePrecision(float value);
};

} // namespace Batteries::Mqtt

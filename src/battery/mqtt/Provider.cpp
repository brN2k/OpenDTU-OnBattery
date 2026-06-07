#include <functional>
#include <Configuration.h>
#include <battery/mqtt/Provider.h>
#include <MqttSettings.h>
#include <Utils.h>
#include <LogHelper.h>

#undef TAG
static const char* TAG = "battery";
static const char* SUBTAG = "MQTT";

namespace Batteries::Mqtt {

bool Provider::init()
{
    _stats->setManufacturer("MQTT");

    auto const& config = Configuration.get();

    _socTopic = config.Battery.Mqtt.SocTopic;
    if (!_socTopic.isEmpty()) {
        MqttSettings.subscribe(_socTopic, 0/*QoS*/,
                std::bind(&Provider::onMqttMessageSoC,
                    this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3, std::placeholders::_4,
                    config.Battery.Mqtt.SocJsonPath, false)
                );

        DTU_LOGD("Subscribed to '%s' for SoC readings", _socTopic.c_str());
    }

    _socBackupTopic = config.Battery.Mqtt.SocBackupTopic;
    if (!_socBackupTopic.isEmpty()) {
        MqttSettings.subscribe(_socBackupTopic, 0/*QoS*/,
                std::bind(&Provider::onMqttMessageSoC,
                    this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3, std::placeholders::_4,
                    config.Battery.Mqtt.SocBackupJsonPath, true)
                );

        DTU_LOGD("Subscribed to '%s' for backup SoC readings", _socBackupTopic.c_str());
    }

    _voltageTopic = config.Battery.Mqtt.VoltageTopic;
    if (!_voltageTopic.isEmpty()) {
        MqttSettings.subscribe(_voltageTopic, 0/*QoS*/,
                std::bind(&Provider::onMqttMessageVoltage,
                    this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3, std::placeholders::_4,
                    config.Battery.Mqtt.VoltageJsonPath)
                );

        DTU_LOGD("Subscribed to '%s' for voltage readings", _voltageTopic.c_str());
    }

    _currentTopic = config.Battery.Mqtt.CurrentTopic;
    if (!_currentTopic.isEmpty()) {
        MqttSettings.subscribe(_currentTopic, 0/*QoS*/,
                std::bind(&Provider::onMqttMessageCurrent,
                    this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3, std::placeholders::_4,
                    config.Battery.Mqtt.CurrentJsonPath)
                );

        DTU_LOGD("Subscribed to '%s' for current readings", _currentTopic.c_str());
    }

    if (config.Battery.EnableDischargeCurrentLimit && config.Battery.UseBatteryReportedDischargeCurrentLimit) {
        _dischargeCurrentLimitTopic = config.Battery.Mqtt.DischargeCurrentLimitTopic;

        if (!_dischargeCurrentLimitTopic.isEmpty()) {
            MqttSettings.subscribe(_dischargeCurrentLimitTopic, 0/*QoS*/,
                    std::bind(&Provider::onMqttMessageDischargeCurrentLimit,
                        this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4,
                        config.Battery.Mqtt.DischargeCurrentLimitJsonPath)
                    );

            DTU_LOGD("Subscribed to '%s' for discharge current limit readings",
                _dischargeCurrentLimitTopic.c_str());
        }
    }

    if (config.Battery.EnableChargeCurrentLimit && config.Battery.UseBatteryReportedChargeCurrentLimit) {
        _chargeCurrentLimitTopic = config.Battery.Mqtt.ChargeCurrentLimitTopic;

        if (!_chargeCurrentLimitTopic.isEmpty()) {
            MqttSettings.subscribe(_chargeCurrentLimitTopic, 0/*QoS*/,
                    std::bind(&Provider::onMqttMessageChargeCurrentLimit,
                        this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4,
                        config.Battery.Mqtt.ChargeCurrentLimitJsonPath)
                    );

            DTU_LOGD("Subscribed to '%s' for charge current limit readings",
                _chargeCurrentLimitTopic.c_str());
        }
    }

    _solarInputPowerTopic = config.Battery.Mqtt.SolarInputPowerTopic;
    if (!_solarInputPowerTopic.isEmpty()) {
        MqttSettings.subscribe(_solarInputPowerTopic, 0/*QoS*/,
                std::bind(&Provider::onMqttMessageSolarInputPower,
                    this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3, std::placeholders::_4,
                    config.Battery.Mqtt.SolarInputPowerJsonPath, false)
                );

        DTU_LOGD("Subscribed to '%s' for solar input power readings",
            _solarInputPowerTopic.c_str());
    }

    _solarInputPowerBackupTopic = config.Battery.Mqtt.SolarInputPowerBackupTopic;
    if (!_solarInputPowerBackupTopic.isEmpty()) {
        MqttSettings.subscribe(_solarInputPowerBackupTopic, 0/*QoS*/,
                std::bind(&Provider::onMqttMessageSolarInputPower,
                    this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3, std::placeholders::_4,
                    config.Battery.Mqtt.SolarInputPowerBackupJsonPath, true)
                );

        DTU_LOGD("Subscribed to '%s' for backup solar input power readings",
            _solarInputPowerBackupTopic.c_str());
    }

    _lowestCellVoltageTopic = config.Battery.Mqtt.LowestCellVoltageTopic;
    if (!_lowestCellVoltageTopic.isEmpty()) {
        MqttSettings.subscribe(_lowestCellVoltageTopic, 0/*QoS*/,
                std::bind(&Provider::onMqttMessageLowestCellVoltage,
                    this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3, std::placeholders::_4,
                    config.Battery.Mqtt.LowestCellVoltageJsonPath, false)
                );

        DTU_LOGD("Subscribed to '%s' for lowest cell voltage readings",
            _lowestCellVoltageTopic.c_str());
    }

    _lowestCellVoltageBackupTopic = config.Battery.Mqtt.LowestCellVoltageBackupTopic;
    if (!_lowestCellVoltageBackupTopic.isEmpty()) {
        MqttSettings.subscribe(_lowestCellVoltageBackupTopic, 0/*QoS*/,
                std::bind(&Provider::onMqttMessageLowestCellVoltage,
                    this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3, std::placeholders::_4,
                    config.Battery.Mqtt.LowestCellVoltageBackupJsonPath, true)
                );

        DTU_LOGD("Subscribed to '%s' for backup lowest cell voltage readings",
            _lowestCellVoltageBackupTopic.c_str());
    }

    return true;
}

void Provider::deinit()
{
    if (!_voltageTopic.isEmpty()) {
        MqttSettings.unsubscribe(_voltageTopic);
    }

    if (!_socTopic.isEmpty()) {
        MqttSettings.unsubscribe(_socTopic);
    }

    if (!_socBackupTopic.isEmpty()) {
        MqttSettings.unsubscribe(_socBackupTopic);
    }

    if (!_currentTopic.isEmpty()) {
        MqttSettings.unsubscribe(_currentTopic);
    }

    if (!_dischargeCurrentLimitTopic.isEmpty()) {
        MqttSettings.unsubscribe(_dischargeCurrentLimitTopic);
    }

    if (!_chargeCurrentLimitTopic.isEmpty()) {
        MqttSettings.unsubscribe(_chargeCurrentLimitTopic);
    }

    if (!_solarInputPowerTopic.isEmpty()) {
        MqttSettings.unsubscribe(_solarInputPowerTopic);
    }

    if (!_solarInputPowerBackupTopic.isEmpty()) {
        MqttSettings.unsubscribe(_solarInputPowerBackupTopic);
    }

    if (!_lowestCellVoltageTopic.isEmpty()) {
        MqttSettings.unsubscribe(_lowestCellVoltageTopic);
    }

    if (!_lowestCellVoltageBackupTopic.isEmpty()) {
        MqttSettings.unsubscribe(_lowestCellVoltageBackupTopic);
    }
}

void Provider::loop()
{
    _stats->refreshActiveValues();
}

void Provider::onMqttMessageSoC(espMqttClientTypes::MessageProperties const& properties,
        char const* topic, uint8_t const* payload, size_t len,
        char const* jsonPath, bool backup)
{
    auto soc = Utils::getNumericValueFromMqttPayload<float>("MqttBattery",
            std::string(reinterpret_cast<const char*>(payload), len), topic,
            jsonPath);

    if (!soc.has_value()) { return; }

    if (*soc < 0 || *soc > 100) {
        DTU_LOGW("Implausible SoC '%.2f' in topic '%s'", *soc, topic);
        return;
    }

    _socPrecision = std::max(_socPrecision, calculatePrecision(*soc));

    if (backup) {
        _stats->setBackupSoC(*soc, _socPrecision, millis());
    } else {
        _stats->setPrimarySoC(*soc, _socPrecision, millis());
    }

    DTU_LOGD("Updated %sSoC to %.*f from '%s'",
            backup ? "backup " : "", _socPrecision, *soc, topic);
}

void Provider::onMqttMessageVoltage(espMqttClientTypes::MessageProperties const& properties,
        char const* topic, uint8_t const* payload, size_t len,
        char const* jsonPath)
{
    auto voltage = Utils::getNumericValueFromMqttPayload<float>("MqttBattery",
            std::string(reinterpret_cast<const char*>(payload), len), topic,
            jsonPath);


    if (!voltage.has_value()) { return; }

    auto const& config = Configuration.get();
    using Unit_t = BatteryVoltageUnit;
    switch (config.Battery.Mqtt.VoltageUnit) {
        case Unit_t::DeciVolts:
            *voltage /= 10;
            break;
        case Unit_t::CentiVolts:
            *voltage /= 100;
            break;
        case Unit_t::MilliVolts:
            *voltage /= 1000;
            break;
        default:
            break;
    }

    // since this project is revolving around Hoymiles microinverters, which can
    // only handle up to 65V of input voltage at best, it is safe to assume that
    // an even higher voltage is implausible.
    if (*voltage < 0 || *voltage > 65) {
        DTU_LOGW("Implausible voltage '%.2f' in topic '%s'", *voltage, topic);
        return;
    }

    _stats->setVoltage(*voltage, millis());

    DTU_LOGD("Updated voltage to %.2f from '%s'", *voltage, topic);
}

void Provider::onMqttMessageCurrent(espMqttClientTypes::MessageProperties const& properties,
        char const* topic, uint8_t const* payload, size_t len,
        char const* jsonPath)
{
    auto amperage = Utils::getNumericValueFromMqttPayload<float>("MqttBattery",
            std::string(reinterpret_cast<const char*>(payload), len), topic,
            jsonPath);


    if (!amperage.has_value()) { return; }

    auto const& config = Configuration.get();
    using Unit_t = BatteryAmperageUnit;
    switch (config.Battery.Mqtt.CurrentUnit) {
        case Unit_t::MilliAmps:
            *amperage /= 1000;
            break;
        default:
            break;
    }

    _currentPrecision = std::max(_currentPrecision, calculatePrecision(*amperage));

    _stats->setCurrent(*amperage, _currentPrecision, millis());

    DTU_LOGD("Updated current to %.*f from '%s'", _currentPrecision, *amperage, topic);
}

void Provider::onMqttMessageDischargeCurrentLimit(espMqttClientTypes::MessageProperties const& properties,
        char const* topic, uint8_t const* payload, size_t len,
        char const* jsonPath)
{
    auto amperage = Utils::getNumericValueFromMqttPayload<float>("MqttBattery",
            std::string(reinterpret_cast<const char*>(payload), len), topic,
            jsonPath);


    if (!amperage.has_value()) { return; }

    auto const& config = Configuration.get();
    using Unit_t = BatteryAmperageUnit;
    switch (config.Battery.Mqtt.DischargeCurrentLimitUnit) {
        case Unit_t::MilliAmps:
            *amperage /= 1000;
            break;
        default:
            break;
    }

    if (*amperage < 0) {
        DTU_LOGW("Implausible discharge current limit '%.2f' in topic '%s'", *amperage, topic);
        return;
    }

    _stats->setDischargeCurrentLimit(*amperage, millis());

    DTU_LOGD("Updated discharge current limit to %.2f from '%s'", *amperage, topic);
}

void Provider::onMqttMessageChargeCurrentLimit(espMqttClientTypes::MessageProperties const& properties,
        char const* topic, uint8_t const* payload, size_t len,
        char const* jsonPath)
{
    auto amperage = Utils::getNumericValueFromMqttPayload<float>("MqttBattery",
            std::string(reinterpret_cast<const char*>(payload), len), topic,
            jsonPath);

    if (!amperage.has_value()) { return; }

    auto const& config = Configuration.get();
    using Unit_t = BatteryAmperageUnit;
    switch (config.Battery.Mqtt.ChargeCurrentLimitUnit) {
        case Unit_t::MilliAmps:
            *amperage /= 1000;
            break;
        default:
            break;
    }

    if (*amperage < 0) {
        DTU_LOGW("Implausible charge current limit '%.2f' in topic '%s'", *amperage, topic);
        return;
    }
    _stats->setChargeCurrentLimit(*amperage, millis());
}

void Provider::onMqttMessageSolarInputPower(espMqttClientTypes::MessageProperties const& properties,
        char const* topic, uint8_t const* payload, size_t len,
        char const* jsonPath, bool backup)
{
    auto power = Utils::getNumericValueFromMqttPayload<float>("MqttBattery",
            std::string(reinterpret_cast<const char*>(payload), len), topic,
            jsonPath);

    if (!power.has_value()) { return; }

    if (*power < 0 || *power > 10000) {
        DTU_LOGW("Implausible solar input power '%.2f' in topic '%s'", *power, topic);
        return;
    }

    _stats->setSolarInputPowerWatts(*power, millis(), backup);

    DTU_LOGD("Updated %ssolar input power to %.1f W from '%s'",
            backup ? "backup " : "", *power, topic);
}

void Provider::onMqttMessageLowestCellVoltage(espMqttClientTypes::MessageProperties const& properties,
        char const* topic, uint8_t const* payload, size_t len,
        char const* jsonPath, bool backup)
{
    auto voltage = Utils::getNumericValueFromMqttPayload<float>("MqttBattery",
            std::string(reinterpret_cast<const char*>(payload), len), topic,
            jsonPath);

    if (!voltage.has_value()) { return; }

    if (*voltage < 0 || *voltage > 5) {
        DTU_LOGW("Implausible lowest cell voltage '%.3f' in topic '%s'", *voltage, topic);
        return;
    }

    _stats->setLowestCellVoltage(*voltage, millis(), backup);

    DTU_LOGD("Updated %slowest cell voltage to %.3f V from '%s'",
            backup ? "backup " : "", *voltage, topic);
}

uint8_t Provider::calculatePrecision(float value) {
    unsigned factor = 1;
    uint8_t precision = 0;
    while (precision < 2) {
        if (std::floor(value * factor) == value * factor) {
            break;
        }
        ++precision;
        factor *= 10;
    }
    return precision;
}

} // namespace Batteries::Mqtt

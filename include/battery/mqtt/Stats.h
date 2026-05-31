// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <battery/Stats.h>

namespace Batteries::Mqtt {

class Stats : public ::Batteries::Stats {
friend class Provider;

public:
    bool updateAvailable(uint32_t since) const final;

    void getLiveViewData(JsonVariant& root) const final;

    // since the source of information was MQTT in the first place,
    // we do NOT publish the same data under a different topic.
    void mqttPublish() const final { }

    bool supportsAlarmsAndWarnings() const final { return false; }

    std::optional<float> getSolarInputPowerWatts() const final;
    bool isSolarInputPowerStale() const final;

    std::optional<float> getLowestCellVoltage() const final;
    bool isLowestCellVoltageStale() const final;

    bool isSoCStale() const final;

private:
    void setSolarInputPowerWatts(float power, uint32_t timestamp) {
        _solarInputPowerWatts = power;
        _lastUpdateSolarInputPowerWatts = _lastUpdate = timestamp;
    }

    void setLowestCellVoltage(float voltage, uint32_t timestamp) {
        _lowestCellVoltage = voltage;
        _lastUpdateLowestCellVoltage = _lastUpdate = timestamp;
    }

    bool isTimedOut(uint32_t lastUpdate) const;
    std::optional<float> getValueIfAvailable(uint32_t lastUpdate, float value) const;

    float _solarInputPowerWatts = 0;
    uint32_t _lastUpdateSolarInputPowerWatts = 0;

    float _lowestCellVoltage = 0;
    uint32_t _lastUpdateLowestCellVoltage = 0;
};

} // namespace Batteries::Mqtt

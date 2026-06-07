// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <battery/Stats.h>

namespace Batteries::Mqtt {

class Stats : public ::Batteries::Stats {
friend class Provider;

public:
    bool updateAvailable(uint32_t since) const final;

    void getLiveViewData(JsonVariant& root) const final;
    void refreshActiveValues();

    // since the source of information was MQTT in the first place,
    // we do NOT publish the same data under a different topic.
    void mqttPublish() const final { }

    bool supportsAlarmsAndWarnings() const final { return false; }

    std::optional<float> getSolarInputPowerWatts() const final;
    bool isSolarInputPowerStale() const final;
    DataStatus getSolarInputPowerDataStatus() const final;

    std::optional<float> getLowestCellVoltage() const final;
    bool isLowestCellVoltageStale() const final;
    DataStatus getLowestCellVoltageDataStatus() const final;

    bool isSoCStale() const final;
    DataStatus getSoCDataStatus() const final;

private:
    enum class Source : uint8_t {
        None,
        Primary,
        Backup,
    };

    struct SourceValue {
        float Value = 0;
        uint8_t Precision = 0;
        uint32_t LastUpdate = 0;
    };

    void setPrimarySoC(float soc, uint8_t precision, uint32_t timestamp) {
        _primarySoC = { soc, precision, timestamp };
        refreshActiveSoC();
    }

    void setBackupSoC(float soc, uint8_t precision, uint32_t timestamp) {
        _backupSoC = { soc, precision, timestamp };
        refreshActiveSoC();
    }

    void setSolarInputPowerWatts(float power, uint32_t timestamp, bool backup) {
        auto& source = backup ? _backupSolarInputPowerWatts : _primarySolarInputPowerWatts;
        source = { power, 1, timestamp };
        _lastUpdate = timestamp;
    }

    void setLowestCellVoltage(float voltage, uint32_t timestamp, bool backup) {
        auto& source = backup ? _backupLowestCellVoltage : _primaryLowestCellVoltage;
        source = { voltage, 3, timestamp };
        _lastUpdate = timestamp;
    }

    void refreshActiveSoC();
    bool isFresh(uint32_t lastUpdate) const;
    DataStatus getDataStatus(uint32_t primaryLastUpdate,
            uint32_t backupLastUpdate, char const* backupTopic) const;
    std::optional<float> getValue(SourceValue const& primary,
            SourceValue const& backup, char const* backupTopic) const;
    Source getSelectedSource(SourceValue const& primary,
            SourceValue const& backup, char const* backupTopic) const;

    SourceValue _primarySoC;
    SourceValue _backupSoC;
    Source _activeSoCSource = Source::None;
    uint32_t _activeSoCLastUpdate = 0;

    SourceValue _primarySolarInputPowerWatts;
    SourceValue _backupSolarInputPowerWatts;

    SourceValue _primaryLowestCellVoltage;
    SourceValue _backupLowestCellVoltage;
};

} // namespace Batteries::Mqtt

// SPDX-License-Identifier: GPL-2.0-or-later
#include <algorithm>
#include <battery/mqtt/Stats.h>
#include <Configuration.h>

namespace Batteries::Mqtt {

bool Stats::updateAvailable(uint32_t since) const
{
    if (::Batteries::Stats::updateAvailable(since)) { return true; }

    bool const hasTimedValues = _primarySoC.LastUpdate > 0
            || _backupSoC.LastUpdate > 0
            || _primarySolarInputPowerWatts.LastUpdate > 0
            || _backupSolarInputPowerWatts.LastUpdate > 0
            || _primaryLowestCellVoltage.LastUpdate > 0
            || _backupLowestCellVoltage.LastUpdate > 0;

    return hasTimedValues && millis() - since >= 1000;
}

void Stats::refreshActiveValues()
{
    refreshActiveSoC();
}

void Stats::getLiveViewData(JsonVariant& root) const
{
    ::Batteries::Stats::getLiveViewData(root);

    if (isSoCValid()) {
        addLiveViewDataStatus(root, "status", "SoC", getSoCDataStatus());
    }

    auto solarInputPower = getSolarInputPowerWatts();
    if (solarInputPower.has_value()) {
        addLiveViewValue(root, "solarInputPower", *solarInputPower, "W", 1);
        addLiveViewDataStatus(root, "status", "solarInputPower", getSolarInputPowerDataStatus());
    }

    auto lowestCellVoltage = getLowestCellVoltage();
    if (lowestCellVoltage.has_value()) {
        addLiveViewInSection(root, "cells", "cellMinVoltage", *lowestCellVoltage, "V", 3);
        addLiveViewDataStatus(root, "cells", "cellMinVoltage", getLowestCellVoltageDataStatus());
    }
}

std::optional<float> Stats::getSolarInputPowerWatts() const
{
    auto const& config = Configuration.get();
    return getValue(_primarySolarInputPowerWatts,
            _backupSolarInputPowerWatts, config.Battery.Mqtt.SolarInputPowerBackupTopic);
}

bool Stats::isSolarInputPowerStale() const
{
    return DataStatus::Error == getSolarInputPowerDataStatus();
}

DataStatus Stats::getSolarInputPowerDataStatus() const
{
    auto const& config = Configuration.get();
    return getDataStatus(_primarySolarInputPowerWatts.LastUpdate,
            _backupSolarInputPowerWatts.LastUpdate,
            config.Battery.Mqtt.SolarInputPowerBackupTopic);
}

std::optional<float> Stats::getLowestCellVoltage() const
{
    auto const& config = Configuration.get();
    return getValue(_primaryLowestCellVoltage,
            _backupLowestCellVoltage, config.Battery.Mqtt.LowestCellVoltageBackupTopic);
}

bool Stats::isLowestCellVoltageStale() const
{
    return DataStatus::Error == getLowestCellVoltageDataStatus();
}

DataStatus Stats::getLowestCellVoltageDataStatus() const
{
    auto const& config = Configuration.get();
    return getDataStatus(_primaryLowestCellVoltage.LastUpdate,
            _backupLowestCellVoltage.LastUpdate,
            config.Battery.Mqtt.LowestCellVoltageBackupTopic);
}

bool Stats::isSoCStale() const
{
    return DataStatus::Error == getSoCDataStatus();
}

DataStatus Stats::getSoCDataStatus() const
{
    auto const& config = Configuration.get();
    return getDataStatus(_primarySoC.LastUpdate,
            _backupSoC.LastUpdate, config.Battery.Mqtt.SocBackupTopic);
}

void Stats::refreshActiveSoC()
{
    auto const& config = Configuration.get();
    auto source = getSelectedSource(_primarySoC, _backupSoC, config.Battery.Mqtt.SocBackupTopic);

    if (Source::None == source) {
        if (_primarySoC.LastUpdate == 0 && _backupSoC.LastUpdate == 0) { return; }

        source = (_primarySoC.LastUpdate >= _backupSoC.LastUpdate)
                ? Source::Primary
                : Source::Backup;
    }

    SourceValue const& value = (Source::Backup == source) ? _backupSoC : _primarySoC;
    if (value.LastUpdate == 0) { return; }

    if (_activeSoCSource == source && _activeSoCLastUpdate == value.LastUpdate) { return; }

    setSoC(value.Value, value.Precision, value.LastUpdate);
    _activeSoCSource = source;
    _activeSoCLastUpdate = value.LastUpdate;
}

bool Stats::isFresh(uint32_t lastUpdate) const
{
    if (lastUpdate == 0) { return false; }

    uint32_t const timeout = std::max<uint32_t>(1, Configuration.get().Battery.Mqtt.TopicTimeout);
    return (millis() - lastUpdate) <= timeout * 1000;
}

DataStatus Stats::getDataStatus(uint32_t primaryLastUpdate,
        uint32_t backupLastUpdate, char const* backupTopic) const
{
    if (isFresh(primaryLastUpdate)) { return DataStatus::Ok; }

    bool const backupConfigured = backupTopic != nullptr && backupTopic[0] != '\0';
    if (backupConfigured && isFresh(backupLastUpdate)) { return DataStatus::Backup; }

    return DataStatus::Error;
}

std::optional<float> Stats::getValue(SourceValue const& primary,
        SourceValue const& backup, char const* backupTopic) const
{
    auto source = getSelectedSource(primary, backup, backupTopic);

    if (Source::Primary == source) { return primary.Value; }
    if (Source::Backup == source) { return backup.Value; }

    if (primary.LastUpdate == 0 && backup.LastUpdate == 0) { return std::nullopt; }

    return (primary.LastUpdate >= backup.LastUpdate) ? primary.Value : backup.Value;
}

Stats::Source Stats::getSelectedSource(SourceValue const& primary,
        SourceValue const& backup, char const* backupTopic) const
{
    if (isFresh(primary.LastUpdate)) { return Source::Primary; }

    bool const backupConfigured = backupTopic != nullptr && backupTopic[0] != '\0';
    if (backupConfigured && isFresh(backup.LastUpdate)) { return Source::Backup; }

    return Source::None;
}

} // namespace Batteries::Mqtt

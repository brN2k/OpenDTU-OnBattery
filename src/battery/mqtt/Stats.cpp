// SPDX-License-Identifier: GPL-2.0-or-later
#include <algorithm>
#include <battery/mqtt/Stats.h>
#include <Configuration.h>

namespace Batteries::Mqtt {

bool Stats::updateAvailable(uint32_t since) const
{
    if (::Batteries::Stats::updateAvailable(since)) { return true; }

    bool const hasTimedValues = isSoCValid()
            || _lastUpdateSolarInputPowerWatts > 0
            || _lastUpdateLowestCellVoltage > 0;

    return hasTimedValues && millis() - since >= 1000;
}

void Stats::getLiveViewData(JsonVariant& root) const
{
    ::Batteries::Stats::getLiveViewData(root);

    if (isSoCValid()) {
        addLiveViewStale(root, "status", "SoC", isSoCStale());
    }

    auto solarInputPower = getSolarInputPowerWatts();
    if (solarInputPower.has_value()) {
        addLiveViewValue(root, "solarInputPower", *solarInputPower, "W", 1);
        addLiveViewStale(root, "status", "solarInputPower", isSolarInputPowerStale());
    }

    auto lowestCellVoltage = getLowestCellVoltage();
    if (lowestCellVoltage.has_value()) {
        addLiveViewInSection(root, "cells", "cellMinVoltage", *lowestCellVoltage, "V", 3);
        addLiveViewStale(root, "cells", "cellMinVoltage", isLowestCellVoltageStale());
    }
}

std::optional<float> Stats::getSolarInputPowerWatts() const
{
    return getValueIfAvailable(_lastUpdateSolarInputPowerWatts, _solarInputPowerWatts);
}

bool Stats::isSolarInputPowerStale() const
{
    return isTimedOut(_lastUpdateSolarInputPowerWatts);
}

std::optional<float> Stats::getLowestCellVoltage() const
{
    return getValueIfAvailable(_lastUpdateLowestCellVoltage, _lowestCellVoltage);
}

bool Stats::isLowestCellVoltageStale() const
{
    return isTimedOut(_lastUpdateLowestCellVoltage);
}

bool Stats::isSoCStale() const
{
    if (!isSoCValid()) { return true; }

    uint32_t const timeout = std::max<uint32_t>(1, Configuration.get().Battery.Mqtt.TopicTimeout);
    return getSoCAgeSeconds() > timeout;
}

bool Stats::isTimedOut(uint32_t lastUpdate) const
{
    if (lastUpdate == 0) { return true; }

    uint32_t const timeout = std::max<uint32_t>(1, Configuration.get().Battery.Mqtt.TopicTimeout);
    return (millis() - lastUpdate) > timeout * 1000;
}

std::optional<float> Stats::getValueIfAvailable(uint32_t lastUpdate, float value) const
{
    if (lastUpdate == 0) { return std::nullopt; }
    return value;
}

} // namespace Batteries::Mqtt

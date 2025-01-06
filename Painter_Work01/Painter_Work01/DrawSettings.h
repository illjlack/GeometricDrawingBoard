#pragma once

#include <unordered_map>
#include <variant>
#include <string>
#include <stdexcept>
#include <QColor>
#include "Enums.h"

using SettingValue = std::variant<int, QRgb, float>;

class DrawSettings {
public:
    static DrawSettings& getInstance();

    void setSetting(DrawSettingKey key, const SettingValue& value);

    SettingValue getSetting(DrawSettingKey key) const;

    void reset();

private:
    DrawSettings();

    std::unordered_map<DrawSettingKey, SettingValue> settings;
};

inline void setSetting(DrawSettingKey key, const SettingValue& value)
{
    DrawSettings::getInstance().setSetting(key, value);
}

inline int getSettingInt(DrawSettingKey key)
{
   return std::get<int>(DrawSettings::getInstance().getSetting(key));
}

inline QRgb getSettingQRgb(DrawSettingKey key)
{
    return std::get<QRgb>(DrawSettings::getInstance().getSetting(key));
}

inline float getSettingFloat(DrawSettingKey key)
{
    return std::get<float>(DrawSettings::getInstance().getSetting(key));
}
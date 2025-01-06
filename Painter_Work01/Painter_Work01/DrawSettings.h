#pragma once

#include <unordered_map>
#include <variant>
#include <string>
#include <stdexcept>
#include <QColor>
#include "Enums.h"

// 枚举类型（int），颜色，粗细
using SettingValue = std::variant<int, QRgb, float>;

// 全局
class GlobalDrawSettings {
public:
    static GlobalDrawSettings& getInstance();

    void setSetting(DrawSettingKey key, const SettingValue& value);

    SettingValue getSetting(DrawSettingKey key) const;

    void reset();

private:
    GlobalDrawSettings();

    std::unordered_map<DrawSettingKey, SettingValue> settings;
};

// 对选中组件的设置
class GroupDrawSettings
{
    GroupDrawSettings();
};

inline void setSetting(DrawSettingKey key, const SettingValue& value)
{
    GlobalDrawSettings::getInstance().setSetting(key, value);
}

template<class T>
T getSetting(DrawSettingKey key) {
    // 如果 T 是枚举类型，检查其底层类型是否为 int
    if constexpr (std::is_enum_v<T>) {
        static_assert(std::is_same_v<std::underlying_type_t<T>, int>, "Enum underlying type must be int.");
        return static_cast<T>(std::get<int>(GlobalDrawSettings::getInstance().getSetting(key)));
    }
    // 如果 T 是非枚举类型，直接获取
    else {
        return std::get<T>(GlobalDrawSettings::getInstance().getSetting(key));
    }
}


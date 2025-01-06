#pragma once

#include <unordered_map>
#include <variant>
#include <string>
#include <stdexcept>
#include <QColor>
#include "Enums.h"

// ö�����ͣ�int������ɫ����ϸ
using SettingValue = std::variant<int, QRgb, float>;

// ȫ��
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

// ��ѡ�����������
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
    // ��� T ��ö�����ͣ������ײ������Ƿ�Ϊ int
    if constexpr (std::is_enum_v<T>) {
        static_assert(std::is_same_v<std::underlying_type_t<T>, int>, "Enum underlying type must be int.");
        return static_cast<T>(std::get<int>(GlobalDrawSettings::getInstance().getSetting(key)));
    }
    // ��� T �Ƿ�ö�����ͣ�ֱ�ӻ�ȡ
    else {
        return std::get<T>(GlobalDrawSettings::getInstance().getSetting(key));
    }
}


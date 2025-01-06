#include "DrawSettings.h"
#include <QColor>

DrawSettings& DrawSettings::getInstance() {
    static DrawSettings instance;
    return instance;
}

DrawSettings::DrawSettings() {
    reset();
}

void DrawSettings::setSetting(DrawSettingKey key, const SettingValue& value) {
    settings[key] = value;
}

SettingValue DrawSettings::getSetting(DrawSettingKey key) const {
    auto it = settings.find(key);
    if (it != settings.end()) {
        return it->second;
    }
    throw std::runtime_error("Setting not found and no default value available.");
}

void DrawSettings::reset() {
    // 初始化一些默认设置
    settings[DrawSettingKey::Key_DrawMode] = DrawMode::None;

    settings[DrawSettingKey::Key_LineWidth] = 1;
    settings[DrawSettingKey::Key_LineColor] = QColor(Qt::black).rgba();
    //settings[DrawSettingKey::FillColor] = "white";
    settings[DrawSettingKey::Key_Opacity] = 1.0f;
}
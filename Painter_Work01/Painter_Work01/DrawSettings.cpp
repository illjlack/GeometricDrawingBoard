#include "DrawSettings.h"
#include <QColor>

GlobalDrawSettings& GlobalDrawSettings::getInstance() {
    static GlobalDrawSettings instance;
    return instance;
}

GlobalDrawSettings::GlobalDrawSettings() {
    reset();
}

void GlobalDrawSettings::setSetting(DrawSettingKey key, const SettingValue& value) {
    settings[key] = value;
}

SettingValue GlobalDrawSettings::getSetting(DrawSettingKey key) const {
    auto it = settings.find(key);
    if (it != settings.end()) {
        return it->second;
    }
    throw std::runtime_error("Setting not found and no default value available.");
}

void GlobalDrawSettings::reset() {
    // 初始化一些默认设置
    settings[Key_DrawMode] = DrawMode::None;

    settings[Key_PointShape] = PointShape::Circle;
    settings[Key_PointColor] = QColor(Qt::black).rgba();

    settings[Key_LineStyle] = LineStyle::Solid;
    settings[Key_LineWidth] = 1.0f;
    settings[Key_LineColor] = QColor(Qt::black).rgba();
    settings[Key_LineDashPattern] = 5.0f;
    settings[Key_PgLineMode] = DrawMode::DrawPolyline;
    settings[Key_PgFillColor] = QColor(Qt::blue).rgba();
    settings[Key_PgLineWidth] = 1.0f;
    settings[Key_PgLineColor] = QColor(Qt::black).rgba();
    settings[Key_PgLineStyle] = LineStyle::Solid;
    settings[Key_PgLineDashPattern] = 5.0f;

    settings[Key_SplineOrder] = 3;
    settings[Key_SplineNodeCount] = 1000;
}





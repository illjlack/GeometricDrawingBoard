#include "DrawSettings.h"
#include <QSettings>
#include <QColor>
#include <QFont>

DrawSettings& DrawSettings::instance() {
    static DrawSettings instance;
    return instance;
}

void DrawSettings::setLineColor(const QColor& color) {
    lineColor = color;
}

QColor DrawSettings::getLineColor() const {
    return lineColor;
}

void DrawSettings::setLineWidth(float width) {
    lineWidth = width;
}

float DrawSettings::getLineWidth() const {
    return lineWidth;
}

void DrawSettings::setFont(const QFont& font) {
    currentFont = font;
}

QFont DrawSettings::getFont() const {
    return currentFont;
}

void DrawSettings::setDashPattern(float pattern) {
    dashPattern = pattern;
}

float DrawSettings::getDashPattern() const {
    return dashPattern;
}

void saveSettingsToFile() {
    // 存储到 JSON 格式的文件
    QSettings settings("settings.json", QSettings::JsonFormat);

    // 存储设置
    settings.setValue("Window/Width", 800);
    settings.setValue("Window/Height", 600);
    settings.setValue("Window/Title", "My Application");
    settings.setValue("Appearance/Font", QFont("Arial", 12));
    settings.setValue("Appearance/Color", QColor(Qt::blue));
}

void loadSettingsFromFile() {
    // 从 JSON 文件加载设置
    QSettings settings("settings.json", QSettings::JsonFormat);

    // 读取设置
    int width = settings.value("Window/Width", 1024).toInt();
    int height = settings.value("Window/Height", 768).toInt();
    QString title = settings.value("Window/Title", "Untitled").toString();
    QFont font = settings.value("Appearance/Font", QFont("Arial", 10)).value<QFont>();
    QColor color = settings.value("Appearance/Color", QColor(Qt::black)).value<QColor>();

    qDebug() << "Width:" << width;
    qDebug() << "Height:" << height;
    qDebug() << "Title:" << title;
    qDebug() << "Font:" << font;
    qDebug() << "Color:" << color;
}
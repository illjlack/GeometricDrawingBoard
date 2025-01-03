#pragma once

// 获取全局绘图设置
#include <QColor>
#include <QFont>

class DrawSettings {
public:
    static DrawSettings& instance();  // 获取全局绘图设置实例

    // 设置和获取线条颜色
    void setLineColor(const QColor& color);
    QColor getLineColor() const;

    // 设置和获取线条宽度
    void setLineWidth(float width);
    float getLineWidth() const;

    // 设置和获取其他配置（例如字体、样式等）
    void setFont(const QFont& font);
    QFont getFont() const;

    // 设置和获取虚线段长度
    void setDashPattern(float pattern);
    float getDashPattern() const;

    // 保存配置到文件
    void saveToFile(const QString& filename);

    // 从文件加载配置
    void loadFromFile(const QString& filename);

private:
    DrawSettings();  // 私有化构造函数，确保是单例
    ~DrawSettings();

    // 禁用拷贝构造和赋值运算符
    DrawSettings(const DrawSettings&) = delete;
    DrawSettings& operator=(const DrawSettings&) = delete;

    QColor lineColor = Qt::black;  // 默认线条颜色
    float lineWidth = 1.0f;        // 默认线条宽度
    QFont currentFont = QFont("Arial", 10);  // 默认字体
    float dashPattern = 1.0f;      // 默认虚线段长度
};
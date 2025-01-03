#pragma once

// ��ȡȫ�ֻ�ͼ����
#include <QColor>
#include <QFont>

class DrawSettings {
public:
    static DrawSettings& instance();  // ��ȡȫ�ֻ�ͼ����ʵ��

    // ���úͻ�ȡ������ɫ
    void setLineColor(const QColor& color);
    QColor getLineColor() const;

    // ���úͻ�ȡ�������
    void setLineWidth(float width);
    float getLineWidth() const;

    // ���úͻ�ȡ�������ã��������塢��ʽ�ȣ�
    void setFont(const QFont& font);
    QFont getFont() const;

    // ���úͻ�ȡ���߶γ���
    void setDashPattern(float pattern);
    float getDashPattern() const;

    // �������õ��ļ�
    void saveToFile(const QString& filename);

    // ���ļ���������
    void loadFromFile(const QString& filename);

private:
    DrawSettings();  // ˽�л����캯����ȷ���ǵ���
    ~DrawSettings();

    // ���ÿ�������͸�ֵ�����
    DrawSettings(const DrawSettings&) = delete;
    DrawSettings& operator=(const DrawSettings&) = delete;

    QColor lineColor = Qt::black;  // Ĭ��������ɫ
    float lineWidth = 1.0f;        // Ĭ���������
    QFont currentFont = QFont("Arial", 10);  // Ĭ������
    float dashPattern = 1.0f;      // Ĭ�����߶γ���
};
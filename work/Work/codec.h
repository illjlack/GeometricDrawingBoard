#pragma once

#include <QTextCodec>

QTextCodec* codec = QTextCodec::codecForName("GBK");

#define L(x) (codec->toUnicode(x))
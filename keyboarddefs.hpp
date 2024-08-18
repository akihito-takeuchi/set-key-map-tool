#pragma once

#include <cstdint>

#include <QStringList>

enum class KeyboardType {
  kUS,
  kJP
};

KeyboardType getKeyboardTypeFromString(const QString& keyboard_type_str);
QString getStringOfKeyboardType(KeyboardType keyboard_type);

QStringList getKeyNames(KeyboardType keyboard);
QString getKeyNameOf(KeyboardType keyboard, uint16_t scan_code);
uint16_t getScanCodeOf(KeyboardType keyboard, const QString& key_name);

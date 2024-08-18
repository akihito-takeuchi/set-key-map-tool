#include "keyboarddefs.hpp"

namespace {

struct KeyCode {
  QString us_key_name;
  QString jp_key_name;
  uint16_t scan_code;
};

const QList<KeyCode> kKeyCodes = {
  {"ESC",                "ESC",                0x0001},
	{"TAB",								 "TAB",								 0x000F},
	{"CapsLock",					 "CapsLock",					 0x003A},
	{"Left Shift",				 "Left Shift",				 0x002A},
	{"Right Shift",				 "Right Shift",				 0x0036},
	{"Left Alt",					 "Left Alt",					 0x0038},
	{"Right Alt",					 "Right Alt",					 0xE038},
	{"Left Ctrl",					 "Left Ctrl",					 0x001D},
	{"Right Ctrl",				 "Right Ctrl",				 0xE01D},
	{"PrintScreen",				 "PrintScreen",				 0xE0A2},
	{"Up",								 "Up",								 0xE048},
	{"Down",							 "Down",							 0xE050},
	{"Right",							 "Right",							 0xE04D},
	{"Left",							 "Left",							 0xE04B},
	{"Insert",						 "Insert",						 0xE052},
	{"Delete",						 "Delete",						 0xE053},
	{"Home",							 "Home",							 0xE047},
	{"End",								 "End",								 0xE04F},
	{"PageUp",						 "PageUp",						 0xE049},
	{"PageDown",					 "PageDown",					 0xE051},
	{"Left Windows key",	 "Left Windows key",	 0xE05B},
	{"Right Windows key",	 "Right Windows key",	 0xE05C},
	{"App",								 "App",								 0xE05D},
	{"PAUSE",							 "PAUSE",							 0x0045},
	{"ScrollLock",         "ScrollLock",         0x0046},
  {"~ `",                "半角/全角",          0x0029},
  {"! 1",                "! 1",                0x0002},
  {"@ 2",                "\" 2",               0x0003},
  {"# 3",                "# 3",                0x0004},
  {"$ 4",                "$ 4",                0x0005},
  {"% 5",                "% 5",                0x0006},
  {"^ 6",                "& 6",                0x0007},
  {"& 7",                "' 7",                0x0008},
  {"* 8",                "( 8",                0x0009},
  {"( 9",                ") 9",                0x000A},
  {") 0",                "0",                  0x000B},
	{"_ -",                "= -",                0x000C},
  {"+ =",                "~ ^",                0x000D},
  {"Backspace",          "Backspace",          0x000E},
  {"Tab",                "Tab",                0x000F},
  {"Q",                  "Q",                  0x0010},
  {"W",                  "W",                  0x0011},
  {"E",                  "E",                  0x0012},
  {"R",                  "R",                  0x0013},
  {"T",                  "T",                  0x0014},
  {"Y",                  "Y",                  0x0015},
  {"U",                  "U",                  0x0016},
  {"I",                  "I",                  0x0017},
  {"O",                  "O",                  0x0018},
  {"P",                  "P",                  0x0019},
  {"{ [",                "` @",                0x001A},
  {"} ]",                "{ [",                0x001B},
  {"",                   "| \\",               0x007D},
  {"A",                  "A",                  0x001E},
  {"S",                  "S",                  0x001F},
  {"D",                  "D",                  0x0020},
  {"F",                  "F",                  0x0021},
  {"G",                  "G",                  0x0022},
  {"H",                  "H",                  0x0023},
  {"J",                  "J",                  0x0024},
  {"K",                  "K",                  0x0025},
  {"L",                  "L",                  0x0026},
  {": ;",                "+ ;",                0x0027},
  {"\" '",               "* :",                0x0028},
  {"| \\",               "} ]",                0x002B},
  {"Enter",              "Enter",              0x001C},
  {"Z",                  "Z",                  0x002C},
  {"X",                  "X",                  0x002D},
  {"C",                  "C",                  0x002E},
  {"V",                  "V",                  0x002F},
  {"B",                  "B",                  0x0030},
  {"N",                  "N",                  0x0031},
  {"M",                  "M",                  0x0032},
  {"< ,",                "< ,",                0x0033},
  {"> .",                "> .",                0x0034},
  {"? /",                "? /",                0x0035},
  {"",                   "\\ _",               0x0073},
  {"Space Bar",          "Space Bar",          0x0039},
  {"Num Lock",           "Num Lock",           0x0045},
  {"Numeric 7",          "Numeric 7",          0x0047},
  {"Numeric 4",          "Numeric 4",          0x004B},
  {"Numeric 1",          "Numeric 1",          0x004F},
  {"Numeric /",          "Numeric /",          0xE035},
  {"Numeric 8",          "Numeric 8",          0x0048},
	{"Numeric 5",          "Numeric 5",          0x004C},
	{"Numeric 2",          "Numeric 2",          0x0050},
	{"Numeric 0",          "Numeric 0",          0x0052},
	{"Numeric *",          "Numeric *",          0x0037},
	{"Numeric 9",          "Numeric 9",          0x0049},
	{"Numeric 6",          "Numeric 6",          0x004D},
	{"Numeric 3",          "Numeric 3",          0x0051},
	{"Numeric .",          "Numeric .",          0x0053},
	{"Numeric -",          "Numeric -",          0x004A},
	{"Numeric +",          "Numeric +",          0x004E},
  {"Numeric Enter",      "Numeric Enter",      0xE01C},
  {"F1",                 "F1",                 0x003B},
	{"F2",                 "F2",                 0x003C},
	{"F3",                 "F3",                 0x003D},
	{"F4",                 "F4",                 0x003E},
	{"F5",                 "F5",                 0x003F},
	{"F6",                 "F6",                 0x0040},
	{"F7",                 "F7",                 0x0041},
	{"F8",                 "F8",                 0x0042},
	{"F9",                 "F9",                 0x0043},
	{"F10",                "F10",                0x0044},
	{"F11",                "F11",                0x0057},
	{"F12",                "F12",                0x0058},
};

QString getKeyNameOf(KeyboardType keyboard, KeyCode key_code) {
  QString key_name;
  switch (keyboard) {
    case KeyboardType::kUS:
      key_name = key_code.us_key_name;
      break;
    case KeyboardType::kJP:
      key_name = key_code.jp_key_name;
      break;
  }
  return key_name;
}

}  // namespace

KeyboardType getKeyboardTypeFromString(const QString& keyboard_type_str) {
  KeyboardType keyboard_type = KeyboardType::kUS;
  if (keyboard_type_str == "US") {
    keyboard_type = KeyboardType::kUS;
  } else if (keyboard_type_str == "JP") {
    keyboard_type = KeyboardType::kJP;
  }
  return keyboard_type;
}

QString getStringOfKeyboardType(KeyboardType keyboard_type) {
  QString keyboard_type_str = "US";
  switch (keyboard_type) {
    case KeyboardType::kUS:
      keyboard_type_str = "US";
      break;
    case KeyboardType::kJP:
      keyboard_type_str = "JP";
      break;
  }
  return keyboard_type_str;
}

QStringList getKeyNames(KeyboardType keyboard) {
  QStringList key_names;
  for (auto& key_code : kKeyCodes) {
    auto key_name = getKeyNameOf(keyboard, key_code);
    if (!key_name.isEmpty()) {
      key_names.append(key_name);
    }
  }
  return key_names;
}

QString getKeyNameOf(KeyboardType keyboard, uint16_t scan_code) {
  QString key_name;
  for (auto& key_code : kKeyCodes) {
    if (key_code.scan_code == scan_code) {
      key_name = getKeyNameOf(keyboard, key_code);
      break;
    }
  }
  return key_name;
}

uint16_t getScanCodeOf(KeyboardType keyboard, const QString& key_name) {
  uint16_t scan_code = 0;
  for (auto& key_code : kKeyCodes) {
    if (key_name == getKeyNameOf(keyboard, key_code)) {
      scan_code = key_code.scan_code;
      break;
    }
  }
  return scan_code;
}

#include "winutil.hpp"

#include <vector>
#include <windows.h>

#include <QMessageBox>
#include <QCoreApplication>

namespace {

const QString kScancodePath = R"(SYSTEM\CurrentControlSet\Control\Keyboard Layout\Scancode Map)";

struct KeyInfo {
  QString sub_key;
  QString leaf_key;
};

KeyInfo getKeyInfo(const QString& key) {
  auto key_elems = key.split("\\");
  KeyInfo result;
  result.leaf_key = key_elems.back();
  key_elems.pop_back();
  result.sub_key = key_elems.join("\\");
  return result;
}

LPCWSTR toWinStr(const QString& str) {
  return reinterpret_cast<LPCWSTR>(str.utf16());
}

bool hasAdminPrivilege() {
  bool has_admin = false;
  HANDLE h_token;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &h_token)) {
    TOKEN_ELEVATION elevation;
    DWORD size = sizeof(TOKEN_ELEVATION);
    if (GetTokenInformation(h_token, TokenElevation, &elevation, sizeof(elevation), &size)) {
      has_admin = elevation.TokenIsElevated;
    }
  }
  if (h_token) {
    CloseHandle(h_token);
  }
  return has_admin;
}

}  // namespace

QByteArray loadBinaryFromRegistry(const QString& key) {
  const unsigned int max_length = 256;
  QByteArray result;
  HKEY h_key;
  DWORD dw_type;
  TCHAR lp_data[max_length];
  DWORD dw_data_size;
  LONG ret;

  auto key_info = getKeyInfo(key);

  ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, toWinStr(key_info.sub_key),
                      0, KEY_QUERY_VALUE, &h_key);
  if (ret == ERROR_SUCCESS) {
    dw_data_size = max_length;
    ret = RegQueryValueEx(h_key, toWinStr(key_info.leaf_key),
                           0, &dw_type, (LPBYTE)lp_data, &dw_data_size);
    if (ret == ERROR_SUCCESS && dw_type == REG_BINARY) {
      result = QByteArray(reinterpret_cast<const char*>(&lp_data[0]), dw_data_size);
    }
    RegCloseKey(h_key);
  }
  return result;
}

QList<KeyMapEntry> loadKeyMap() {
  auto scan_code = loadBinaryFromRegistry(kScancodePath);
  QList<KeyMapEntry> key_map;
  if (scan_code.count() <= 16) {
    return key_map;
  }
  int key_map_count =
        (scan_code[8]  << 0)
      + (scan_code[9]  << 8)
      + (scan_code[10] << 16)
      + (scan_code[11] << 24) - 1;
  const int kOffset = 12;
  for (int idx = kOffset; idx < key_map_count * 4 + kOffset && idx < scan_code.count(); idx += 4) {
    KeyMapEntry entry;
    entry.map_to_key = scan_code[idx+0] + (scan_code[idx+1] << 8);
    entry.actual_key = scan_code[idx+2] + (scan_code[idx+3] << 8);
    key_map.append(entry);
  }
  return key_map;
}

QString setBinaryToRegistry(const QString& key, const QByteArray& data) {
  HKEY h_key;
  DWORD dw_data_size;
  LONG ret;

  auto key_info = getKeyInfo(key);

  ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, toWinStr(key_info.sub_key),
                      0, KEY_WRITE, &h_key);
  if (ret == ERROR_SUCCESS) {
    dw_data_size = data.count();
    ret = RegSetValueEx(h_key, toWinStr(key_info.leaf_key), 0, REG_BINARY,
                         reinterpret_cast<const uint8_t*>(data.data()), dw_data_size);
    if (ret != ERROR_SUCCESS)
      return "Set key failed";
    RegCloseKey(h_key);
  } else {
    return "Open key failed";
  }
  return "";
}

QString setKeyMap(const QList<KeyMapEntry>& key_map) {
  // Create QByteArray object with header bytes
  QByteArray scan_code(8, '\0');
  int entry_count = key_map.count() + 1;
  scan_code.append((entry_count >>  0) & 0xFF);
  scan_code.append((entry_count >>  8) & 0xFF);
  scan_code.append((entry_count >> 16) & 0xFF);
  scan_code.append((entry_count >> 24) & 0xFF);
  for (auto& entry : key_map) {
    scan_code.append((entry.map_to_key >> 0) & 0xFF);
    scan_code.append((entry.map_to_key >> 8) & 0xFF);
    scan_code.append((entry.actual_key >> 0) & 0xFF);
    scan_code.append((entry.actual_key >> 8) & 0xFF);
  }
  // Add footer bytes
  scan_code.append('\0');
  scan_code.append('\0');
  scan_code.append('\0');
  scan_code.append('\0');
  return setBinaryToRegistry(kScancodePath, scan_code);
}

bool ensureAdminPrivilege() {
  if (!hasAdminPrivilege()) {
    // restart
    auto progPath = QCoreApplication::applicationFilePath();
    std::vector<wchar_t> progPathStr;
    progPathStr.resize(progPath.size() + 1);
    progPath.toWCharArray(progPathStr.data());
    ShellExecute(NULL,
                 L"runas",
                 progPathStr.data(),
                 NULL,
                 NULL,
                 SW_SHOWNORMAL);
    return false;
  }
  return true;
}

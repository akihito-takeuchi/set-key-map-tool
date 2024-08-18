#pragma once

#include <cstdint>

#include <QList>
#include <QString>

struct KeyMapEntry {
  uint16_t actual_key;
  uint16_t map_to_key;
  bool operator==(const KeyMapEntry& rhs) const {
    return actual_key == rhs.actual_key && map_to_key == rhs.map_to_key;
  }
};

QList<KeyMapEntry> loadKeyMap();

// Returns error messsage
QString setKeyMap(const QList<KeyMapEntry>& key_map);

// This function should be called at startup of the program.
// If the process doesn't have admin privilege, it starts the program
// again with admin privilege and returns false.
// Returns true if current process has admin privilege
bool ensureAdminPrivilege();

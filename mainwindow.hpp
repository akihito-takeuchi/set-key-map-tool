#pragma once

#include <QMainWindow>
#include <QAction>
#include <QLabel>
#include <QListWidget>
#include <QDialogButtonBox>

#include "winutil.hpp"
#include "keyboarddefs.hpp"

struct KeyMap {
  QString name;
  KeyboardType keyboard_type;
  QList<KeyMapEntry> key_map;
  bool operator==(const KeyMap& rhs) const {
    return name == rhs.name
      && keyboard_type == rhs.keyboard_type
      && key_map == rhs.key_map;
  }
};

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow(QWidget* parent = nullptr);

 private slots:
  void applyScanCodeMap_();
  void updateButtonState_();
  void showContextMenu_(const QPoint& pos);
  void addKeyMap_();
  void editKeyMap_();
  void deleteKeyMap_();

 private:
  void createActions_();
  void createWidgets_();
  void initWidgetValues_();
  void createMenus_();
  void createConnections_();

  QAction* add_key_map_action_;
  QAction* edit_key_map_action_;
  QAction* delete_key_map_action_;
  QLabel* current_key_map_name_;
  QListWidget* key_map_select_;
  QDialogButtonBox* buttons_;
  QList<KeyMap> key_maps_;
};

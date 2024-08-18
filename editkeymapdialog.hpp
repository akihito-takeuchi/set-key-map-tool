#pragma once

#include <QDialog>

#include <QLineEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QPushButton>

#include "winutil.hpp"
#include "keyboarddefs.hpp"

class EditKeyMapDialog : public QDialog {
  Q_OBJECT
 public:
  EditKeyMapDialog(QWidget* parent,
                   const QStringList& existing_names,
                   const QString& current_name=QString(),
                   KeyboardType current_keyboard_type = KeyboardType::kUS,
                   const QList<KeyMapEntry>& key_map=QList<KeyMapEntry>());
  QString getName() const;
  KeyboardType getKeyboardType() const;
  QList<KeyMapEntry> getKeyMap() const;

 private slots:
  void addMapEntry_();
  void deleteChecked_();
  void loadCurrentScancodeMap_();
  void updateWindowState_();

 private:
  void createWidgets_();
  void initWidgetValues_();
  void setKeyMapTable_(KeyboardType keyboard_type, const QList<KeyMapEntry>& key_map);
  void createConnections_();

  QStringList existing_names_;
  QString current_name_;
  KeyboardType current_keyboard_type_;
  QList<KeyMapEntry> current_key_map_;
  QLineEdit* name_input_;
  QComboBox* keyboard_type_select_;
  QPushButton* add_entry_button_;
  QPushButton* delete_checked_button_;
  QPushButton* load_current_scan_code_map_button_;
  QTableWidget* key_map_table_;
  QTextEdit* scan_code_display_;
  QDialogButtonBox* buttons_;
};

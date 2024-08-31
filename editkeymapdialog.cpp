#include "editkeymapdialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QSignalBlocker>

namespace {

QComboBox* createKeySelect(KeyboardType keyboard_type) {
  auto key_select = new QComboBox;
  for (auto& key_name : getKeyNames(keyboard_type)) {
    uint32_t scan_code = getScanCodeOf(keyboard_type, key_name);
    key_select->addItem(key_name, scan_code);
  }
  return key_select;
}

QLabel* createHeaderWidget(const QString& text) {
  auto header = new QLabel(QString("<h3>%1</h3>").arg(text));
  header->setAutoFillBackground(true);
  auto pal = header->palette();
  pal.setColor(QPalette::Base, QColor("navy"));
  pal.setColor(QPalette::WindowText, QColor("silver"));
  header->setPalette(pal);
  return header;
}

QString encodeToString(uint16_t value) {
  auto lower = QString::number((value >> 0) & 0xFF, 16).toUpper();
  if (lower.count() == 1) {
    lower = "0" + lower;
  }
  auto upper = QString::number((value >> 8) & 0xFF, 16).toUpper();
  if (upper.count() == 1) {
    upper = "0" + upper;
  }
  return QString("%1 %2").arg(lower).arg(upper);
}

QString encodeToString(uint32_t value) {
  return QString("%1 %2")
      .arg(encodeToString(static_cast<uint16_t>((value >> 0) & 0xFFFF)))
      .arg(encodeToString(static_cast<uint16_t>((value >> 16) & 0xFFFF)));
}

}  // namespace

EditKeyMapDialog::EditKeyMapDialog(QWidget* parent,
                                   const QStringList& existing_names,
                                   const QString& current_name,
                                   KeyboardType current_keyboard_type,
                                   const QList<KeyMapEntry>& key_map)
    : QDialog(parent),
      existing_names_(existing_names),
      current_name_(current_name),
      current_keyboard_type_(current_keyboard_type),
      current_key_map_(key_map) {
  existing_names_.removeOne(current_name_);
  createWidgets_();
  initWidgetValues_();
  createConnections_();
  updateWindowState_();
  resize(700, 600);
}

void EditKeyMapDialog::createWidgets_() {
  name_input_ = new QLineEdit;
  keyboard_type_select_ = new QComboBox;
  keyboard_type_select_->addItems({
      getStringOfKeyboardType(KeyboardType::kUS),
      getStringOfKeyboardType(KeyboardType::kJP)});
  add_entry_button_ = new QPushButton("Add entry");
  delete_checked_button_ = new QPushButton("Delete checked");
  load_current_scan_code_map_button_ = new QPushButton("Load current scancode map");
  key_map_table_ = new QTableWidget;
  key_map_table_->setColumnCount(3);
  key_map_table_->setColumnWidth(0, 30);
  key_map_table_->setColumnWidth(1, 200);
  key_map_table_->setHorizontalHeaderLabels({"", "Actual key", "Map to key"});
  key_map_table_->horizontalHeader()->setStretchLastSection(true);
  scan_code_display_ = new QTextEdit;
  scan_code_display_->setReadOnly(true);
  auto font = scan_code_display_->font();
  font.setFamily("BIZ UDゴシック");
  scan_code_display_->setFont(font);
  auto pal = scan_code_display_->palette();
  pal.setColor(QPalette::Text, Qt::gray);
  scan_code_display_->setPalette(pal);
  buttons_ = new  QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  auto layout = new QVBoxLayout;
  auto button_layout = new QHBoxLayout;
  button_layout->addStretch(1);
  button_layout->addWidget(add_entry_button_);
  button_layout->addWidget(delete_checked_button_);
  button_layout->addWidget(load_current_scan_code_map_button_);
  layout->addWidget(createHeaderWidget("Setup name"));
  layout->addWidget(name_input_);
  layout->addWidget(createHeaderWidget("Keyboard type"));
  layout->addWidget(keyboard_type_select_);
  layout->addWidget(createHeaderWidget("Key map table"));
  layout->addLayout(button_layout);
  layout->addWidget(key_map_table_);
  layout->addWidget(createHeaderWidget("Scan code"));
  layout->addWidget(scan_code_display_);
  layout->addWidget(buttons_);
  setLayout(layout);
}

void EditKeyMapDialog::initWidgetValues_() {
  if (!current_name_.isEmpty()) {
    name_input_->setText(current_name_);
    name_input_->setReadOnly(true);
    auto pal = name_input_->palette();
    pal.setColor(QPalette::Base, QColor("silver"));
    name_input_->setPalette(pal);
  }
  setKeyMapTable_(current_keyboard_type_, current_key_map_);
}

QList<KeyMapEntry> EditKeyMapDialog::setKeyMapTable_(KeyboardType keyboard_type,
                                                     const QList<KeyMapEntry>& key_map) {
  QList<KeyMapEntry> updated_key_map;
  auto keyboard_type_name = getStringOfKeyboardType(keyboard_type);
  keyboard_type_select_->setCurrentIndex(keyboard_type_select_->findText(keyboard_type_name));
  for (auto& key_map_entry : key_map) {
    addMapEntry_();
    auto row = key_map_table_->rowCount() - 1;

    auto actual_key_name = getKeyNameOf(keyboard_type, key_map_entry.actual_key);
    auto map_to_key_name = getKeyNameOf(keyboard_type, key_map_entry.map_to_key);
    if (actual_key_name.isEmpty() || map_to_key_name.isEmpty()) {
      // The key code is not supported in the specified keyboard type
      // Ignore this entry
      continue;
    }
    updated_key_map.append(key_map_entry);
    auto actual_key_select = qobject_cast<QComboBox*>(key_map_table_->cellWidget(row, 1));
    actual_key_select->setCurrentIndex(actual_key_select->findText(actual_key_name));

    auto map_to_key_select = qobject_cast<QComboBox*>(key_map_table_->cellWidget(row, 2));
    map_to_key_select->setCurrentIndex(map_to_key_select->findText(map_to_key_name));
  }
  return updated_key_map;
}

void EditKeyMapDialog::addMapEntry_() {
  QSignalBlocker blocker(key_map_table_);
  auto keyboard_type = getKeyboardType();
  auto row = key_map_table_->rowCount();
  key_map_table_->insertRow(row);
  auto check_item = new QTableWidgetItem();
  check_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
  check_item->setCheckState(Qt::Unchecked);
  key_map_table_->setItem(row, 0, check_item);

  auto actual_key_item = new QTableWidgetItem();
  actual_key_item->setFlags(Qt::NoItemFlags);
  key_map_table_->setItem(row, 1, actual_key_item);
  auto actual_key_select = createKeySelect(keyboard_type);
  key_map_table_->setCellWidget(row, 1, actual_key_select);
  connect(actual_key_select, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &EditKeyMapDialog::updateWindowState_);

  auto map_to_key_item = new QTableWidgetItem();
  map_to_key_item->setFlags(Qt::NoItemFlags);
  key_map_table_->setItem(row, 2, map_to_key_item);
  auto map_to_key_select = createKeySelect(keyboard_type);
  key_map_table_->setCellWidget(row, 2, map_to_key_select);
  connect(map_to_key_select, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &EditKeyMapDialog::updateWindowState_);

  updateWindowState_();
}

void EditKeyMapDialog::deleteChecked_() {
  QSignalBlocker blocker(key_map_table_);

  QList<int> rows_to_be_removed;
  for (int row = key_map_table_->rowCount() - 1; row >= 0; -- row) {
    if (key_map_table_->item(row, 0)->checkState() == Qt::Checked) {
      rows_to_be_removed.append(row);
    }
  }
  for (auto row : rows_to_be_removed) {
    key_map_table_->removeCellWidget(row, 1);
    key_map_table_->removeCellWidget(row, 2);
    key_map_table_->removeRow(row);
  }

  updateWindowState_();
}

void EditKeyMapDialog::loadCurrentScancodeMap_() {
  QSignalBlocker blocker(key_map_table_);
  key_map_table_->setRowCount(0);
  setKeyMapTable_(getKeyboardType(), loadKeyMap());
  updateWindowState_();
}

void EditKeyMapDialog::updateKeyboardType_() {
  auto key_map = getKeyMap();
  key_map_table_->setRowCount(0);
  setKeyMapTable_(getKeyboardType(), key_map);
  updateWindowState_();
}

void EditKeyMapDialog::updateWindowState_() {
  // Delete button state
  bool any_checked = false;
  for (int row = 0; row < key_map_table_->rowCount(); ++ row) {
    if (key_map_table_->item(row, 0)->checkState() == Qt::Checked) {
      any_checked = true;
      break;
    }
  }
  delete_checked_button_->setEnabled(any_checked);

  // OK button state
  auto keyboard_type = getKeyboardType();
  QList<KeyMapEntry> key_map;
  for (int row = 0; row < key_map_table_->rowCount(); ++ row) {
    KeyMapEntry entry;
    auto actual_key_select = qobject_cast<QComboBox*>(key_map_table_->cellWidget(row, 1));
    entry.actual_key = actual_key_select->currentData().toUInt();
    auto map_to_key_select = qobject_cast<QComboBox*>(key_map_table_->cellWidget(row, 2));
    entry.map_to_key = map_to_key_select->currentData().toUInt();
    key_map.append(entry);
  }
  auto ok_button = buttons_->button(QDialogButtonBox::Ok);
  ok_button->setEnabled(!name_input_->text().isEmpty()
                        && !existing_names_.contains(name_input_->text())
                        && (current_key_map_ != key_map
                            || current_keyboard_type_ != keyboard_type));

  // Scan code text view
  if (key_map.isEmpty()) {
    scan_code_display_->clear();
  } else {
    QString scan_code_str = "00 00 00 00  00 00 00 00\n";
    scan_code_str += encodeToString(static_cast<uint32_t>(key_map.count() + 1));
    int key_idx = 1;
    auto updateCurrentLineEnd = [&key_idx, &scan_code_str] () {
      if (key_idx == 1) {
        scan_code_str += "  ";
        key_idx ++;
      } else if (key_idx == 2) {
        scan_code_str += "\n";
        key_idx = 1;
      }
    };
    for (auto& key_map_entry : key_map) {
      updateCurrentLineEnd();
      scan_code_str += encodeToString(key_map_entry.map_to_key);
      scan_code_str += " ";
      scan_code_str += encodeToString(key_map_entry.actual_key);
    }
    updateCurrentLineEnd();
    scan_code_str += "00 00 00 00"; // Footer
    scan_code_display_->setPlainText(scan_code_str);
  }
}

void EditKeyMapDialog::createConnections_() {
  connect(name_input_, &QLineEdit::textChanged,
          this, &EditKeyMapDialog::updateWindowState_);
  connect(keyboard_type_select_, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &EditKeyMapDialog::updateKeyboardType_);
  connect(key_map_table_, &QTableWidget::itemChanged,
          this, &updateWindowState_);
  connect(add_entry_button_, &QPushButton::clicked,
          this, &EditKeyMapDialog::addMapEntry_);
  connect(delete_checked_button_, &QPushButton::clicked,
          this, &EditKeyMapDialog::deleteChecked_);
  connect(load_current_scan_code_map_button_, &QPushButton::clicked,
          this, &EditKeyMapDialog::loadCurrentScancodeMap_);
  connect(buttons_, &QDialogButtonBox::accepted,
          this, &EditKeyMapDialog::accept);
  connect(buttons_, &QDialogButtonBox::rejected,
          this, &EditKeyMapDialog::reject);
}

QString EditKeyMapDialog::getName() const {
  return name_input_->text();
}

KeyboardType EditKeyMapDialog::getKeyboardType() const {
  return getKeyboardTypeFromString(keyboard_type_select_->currentText());
}

QList<KeyMapEntry> EditKeyMapDialog::getKeyMap() const {
  QList<KeyMapEntry> key_map;
  for (int row = 0; row < key_map_table_->rowCount(); ++ row) {
    KeyMapEntry entry;
    auto actual_key_select = qobject_cast<QComboBox*>(key_map_table_->cellWidget(row, 1));
    entry.actual_key = actual_key_select->currentData().toUInt();
    auto map_to_key_select = qobject_cast<QComboBox*>(key_map_table_->cellWidget(row, 2));
    entry.map_to_key = map_to_key_select->currentData().toUInt();
    key_map.append(entry);
  }
  return key_map;
}

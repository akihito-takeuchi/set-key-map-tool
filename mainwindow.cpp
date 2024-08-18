#include "mainwindow.hpp"

#include <QFormLayout>
#include <QSettings>
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <QPushButton>
#include <QMessageBox>
#include <QDir>

#include "editkeymapdialog.hpp"

namespace {

class Settings : public QSettings {
 public:
  Settings()
      : QSettings(QDir(QCoreApplication::applicationDirPath()).filePath("keysetup.ini"),
                  QSettings::IniFormat) {
  }
  ~Settings() {
    sync();
  }
};

void setKeyMapToSettings(const KeyMap& key_map) {
  QByteArray key_code;
  for (auto& key_map_entry : key_map.key_map) {
    key_code.append((key_map_entry.map_to_key >> 0) & 0xFF);
    key_code.append((key_map_entry.map_to_key >> 8) & 0xFF);
    key_code.append((key_map_entry.actual_key >> 0) & 0xFF);
    key_code.append((key_map_entry.actual_key >> 8) & 0xFF);
  }
  Settings settings;
  settings.beginGroup(key_map.name);
  settings.setValue("kb_type", getStringOfKeyboardType(key_map.keyboard_type));
  settings.setValue("code", key_code);
}

}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
  createActions_();
  createWidgets_();
  initWidgetValues_();
  createMenus_();
  createConnections_();
  updateButtonState_();
}

void MainWindow::createActions_() {
  add_key_map_action_ = new QAction("Add key map");
  edit_key_map_action_ = new QAction("Edit key map");
  delete_key_map_action_ = new QAction("Delete key map");
}

void MainWindow::createWidgets_() {
  current_key_map_name_ = new QLabel();
  key_map_select_ = new QListWidget();
  key_map_select_->setContextMenuPolicy(Qt::CustomContextMenu);
  buttons_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  auto layout = new QFormLayout;
  layout->addRow("Current key map", current_key_map_name_);
  layout->addRow("Key map to apply", key_map_select_);
  layout->addRow(buttons_);
  auto w = new QWidget;
  w->setLayout(layout);
  setCentralWidget(w);
}

void MainWindow::initWidgetValues_() {
  Settings settings;
  for (auto& key_map_name : settings.childGroups()) {
    auto keyboard_type_str = settings.value(key_map_name + "/kb_type", QString()).toString();
    auto keyboard_type = getKeyboardTypeFromString(keyboard_type_str);
    auto key_codes = settings.value(key_map_name + "/code", QByteArray()).toByteArray();
    QList<KeyMapEntry> key_map;
    for (int idx = 0; idx < key_codes.count() / 4; ++ idx) {
      KeyMapEntry entry;
      entry.map_to_key = key_codes[idx*4+0] + (key_codes[idx*4+1] << 8);
      entry.actual_key = key_codes[idx*4+2] + (key_codes[idx*4+3] << 8);
      key_map.append(entry);
    }
    key_maps_.append({key_map_name, keyboard_type, key_map});
    key_map_select_->addItem(key_map_name);
  }

  QString current_name = "Unknown";
  auto current_key_map = loadKeyMap();
  if (current_key_map.count() == 0) {
    current_name = "No key map";
  }
  for (auto& key_map : key_maps_) {
    if (key_map.key_map == current_key_map) {
      current_name = key_map.name;
      break;
    }
  }
  current_key_map_name_->setText(current_name);
}

void MainWindow::createMenus_() {
  auto edit_menu = menuBar()->addMenu("&Edit");
  edit_menu->addAction(add_key_map_action_);
  edit_menu->addAction(edit_key_map_action_);
  edit_menu->addAction(delete_key_map_action_);
}

void MainWindow::createConnections_() {
  connect(key_map_select_, &QListWidget::itemClicked,
          this, &MainWindow::updateButtonState_);
  connect(key_map_select_, &QListWidget::customContextMenuRequested,
          this, &MainWindow::showContextMenu_);
  connect(buttons_, &QDialogButtonBox::accepted,
          this, &MainWindow::applyScanCodeMap_);
  connect(buttons_, &QDialogButtonBox::rejected,
          qApp, &QApplication::quit);
  connect(add_key_map_action_, &QAction::triggered,
          this, &MainWindow::addKeyMap_);
  connect(edit_key_map_action_, &QAction::triggered,
          this, &MainWindow::editKeyMap_);
  connect(delete_key_map_action_, &QAction::triggered,
          this, &MainWindow::deleteKeyMap_);
}

void MainWindow::updateButtonState_() {
  auto ok_button = buttons_->button(QDialogButtonBox::Ok);
  auto items = key_map_select_->selectedItems();
  if (items.isEmpty()) {
    ok_button->setEnabled(false);
  } else {
    ok_button->setEnabled(
        current_key_map_name_->text() != items.front()->text());
  }
}

void MainWindow::applyScanCodeMap_() {
  auto map_name = key_map_select_->selectedItems()[0]->text();
  for (auto& key_map : key_maps_) {
    if (key_map.name == map_name) {
      auto err_msg = setKeyMap(key_map.key_map);
      if (!err_msg.isEmpty()) {
        QMessageBox::warning(this, "Registry update error", err_msg);
        return;
      }
      break;
    }
  }
  QMessageBox::information(this, "Key map has been update",
                           QString("Key ap has been updated to '%1'").arg(map_name));
  qApp->quit();
}

void MainWindow::showContextMenu_(const QPoint&) {
  QMenu menu;
  menu.addAction(add_key_map_action_);
  menu.addAction(edit_key_map_action_);
  menu.addAction(delete_key_map_action_);
  auto selection = key_map_select_->selectionModel();
  if (selection->hasSelection()) {
    auto selected = selection->selectedIndexes()[0];
    edit_key_map_action_->setText(QString("Edit '%1'")
                                  .arg(selected.data(Qt::DisplayRole).toString()));
    edit_key_map_action_->setEnabled(true);
    delete_key_map_action_->setText(QString("Delete '%1'")
                                    .arg(selected.data(Qt::DisplayRole).toString()));
    delete_key_map_action_->setEnabled(true);
  } else {
    edit_key_map_action_->setText("Edit key map");
    edit_key_map_action_->setEnabled(false);
    delete_key_map_action_->setText("Delete key map");
    delete_key_map_action_->setEnabled(false);
  }
  menu.exec(QCursor::pos());
}

void MainWindow::addKeyMap_() {
  QStringList existing_names;
  for (int i = 0; i < key_map_select_->count(); ++ i) {
    existing_names.append(key_map_select_->item(i)->text());
  }
  EditKeyMapDialog dialog(this, existing_names);
  if (dialog.exec() == QDialog::Accepted) {
    KeyMap key_map;
    key_map.name = dialog.getName();
    key_map.keyboard_type = dialog.getKeyboardType();
    key_map.key_map = dialog.getKeyMap();
    key_maps_.append(key_map);
    key_map_select_->addItem(key_map.name);
    setKeyMapToSettings(key_map);
  }
}

void MainWindow::editKeyMap_() {
  QStringList existing_names;
  for (int i = 0; i < key_map_select_->count(); ++ i) {
    existing_names.append(key_map_select_->item(i)->text());
  }
  auto row = key_map_select_->selectionModel()->selectedIndexes()[0].row();
  auto key_map = key_maps_[row];
  EditKeyMapDialog dialog(this, existing_names,
                          key_map.name, key_map.keyboard_type, key_map.key_map);
  if (dialog.exec() == QDialog::Accepted) {
    key_maps_[row].keyboard_type = dialog.getKeyboardType();
    key_maps_[row].key_map = dialog.getKeyMap();
    setKeyMapToSettings(key_maps_[row]);
  }
}

void MainWindow::deleteKeyMap_() {
  auto row = key_map_select_->selectionModel()->selectedIndexes()[0].row();
  auto key_map = key_maps_[row];
  auto ans = QMessageBox::question(this, "Delete key map",
                                   QString("Are you sure to delete '%1'").arg(key_map.name),
                                   QMessageBox::Yes | QMessageBox::No);
  if (ans == QMessageBox::Yes) {
    key_maps_.removeOne(key_map);
    auto item = key_map_select_->takeItem(row);
    delete item;
    Settings settings;
    settings.remove(key_map.name);
  }
}

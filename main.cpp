//
// Main program of SetKeyMap
//

#include <QApplication>

#include "winutil.hpp"
#include "mainwindow.hpp"

int main(int argc, char* argv[]) {
  QCoreApplication::setOrganizationName("SetKeyMap");
  QCoreApplication::setApplicationName("SetKeyMap");
  QApplication a(argc, argv);
  if (ensureAdminPrivilege()) {
    MainWindow w;
    w.show();
    return a.exec();
  } else {
    return 0;
  }
}

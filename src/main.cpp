#include "main_window.hpp"
#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QPixmap>
#include <QStyleHints>
#include <Qt>

int main(int argc, char **argv)
{
  const QApplication app(argc, argv);

  QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
  QApplication::setWindowIcon(QIcon(QPixmap(":/images/SRSG_icon.svg")));

  MainWindow main_window;
  main_window.show();

  return QApplication::exec();
}

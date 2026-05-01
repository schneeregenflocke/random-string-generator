#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "configuration_widget.hpp"
#include "generator.hpp"
#include "license_info_dialog.hpp"
#include <QAction>
#include <QChar>
#include <QDialog>
#include <QIcon>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCore/qtmetamacros.h>
#include <memory>
#include <string>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr)
      : QMainWindow(parent), generator(std::make_unique<Generator>()),
        configuration_widget(new ConfigurationWidget(this)),
        indices_field(new QLineEdit(this)), string_field(new QTextEdit(this)),
        license_info_dialog(new LicenseInfoDialog(this))
  {

    constexpr int min_window_width = 400;
    constexpr int min_window_height = 400;
    setMinimumSize(min_window_width, min_window_height);

    setCentralWidget(new QWidget(this));
    const QPointer<QVBoxLayout> vbox_layout = new QVBoxLayout(centralWidget());
    vbox_layout->addWidget(configuration_widget);
    vbox_layout->addWidget(indices_field);
    vbox_layout->addWidget(string_field);

    const QPointer<QPushButton> generate_button = new QPushButton(tr("Generate"), this);
    generate_button->setAutoDefault(true);
    vbox_layout->addWidget(generate_button);

    connect(generate_button, &QPushButton::clicked, this, &MainWindow::GenerateString);

    InitMenu();
  }

private slots:

  void GenerateString()
  {
    const std::string generated_string =
        generator->GenerateString(configuration_widget->GetRandomStringConfiguration());

    QString indices_string = tr("random indices: ");
    for (const auto &index : generator->GetRandomIndices()) {
      indices_string += QString::number(index) + QChar(' ');
    }
    indices_field->setText(indices_string);
    string_field->setText(QString::fromStdString(generated_string));
  }

private:
  void InitMenu()
  {
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));

    const QAction *action_license_info =
        file_menu->addAction(QIcon::fromTheme("help-about"), tr("&Open Source License Info"));
    file_menu->addSeparator();
    const QAction *action_exit =
        file_menu->addAction(QIcon::fromTheme("application-exit"), tr("&Exit"));

    connect(action_exit, &QAction::triggered, this, &QMainWindow::close);
    connect(action_license_info, &QAction::triggered, license_info_dialog, &QDialog::show);
  }

  std::unique_ptr<Generator> generator;
  QPointer<ConfigurationWidget> configuration_widget;
  QPointer<QLineEdit> indices_field;
  QPointer<QTextEdit> string_field;
  QPointer<LicenseInfoDialog> license_info_dialog;
};

#endif // MAIN_WINDOW_HPP

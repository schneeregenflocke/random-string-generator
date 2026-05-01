
#ifndef CONFIGURATION_WIDGET_HPP
#define CONFIGURATION_WIDGET_HPP

#include "checkable_spinbox_widget.hpp"
#include "configuration.hpp"
#include <QLabel>
#include <QObject>
#include <QtCore/qobjectdefs.h>
#include <QPointer>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtCore/QMetaObject>
#include <QtCore/QObject>
#include <array>
#include <cstddef>
#include <vector>

class ConfigurationWidget : public QWidget {
  Q_OBJECT

public:
  explicit ConfigurationWidget(QWidget *parent)
      : QWidget(parent), lenght_widget(new QSpinBox(this)),
        character_configuration_widgets{nullptr, nullptr, nullptr, nullptr}
  {
    const QPointer<QVBoxLayout> vbox_layout = new QVBoxLayout(this);
    setLayout(vbox_layout);
    vbox_layout->setContentsMargins(0, 0, 0, 0);

    lenght_widget->setRange(0, max_password_lenght);
    lenght_widget->setValue(inital_password_lenght);
    vbox_layout->addWidget(lenght_widget);

    vbox_layout->addWidget(new QLabel(tr("String lenght:"), this));

    connect(lenght_widget, &QSpinBox::valueChanged, this,
            &ConfigurationWidget::UpdatePasswordConfiguration);

    for (auto &character_configuration_widget : character_configuration_widgets) {
      const QPointer<CheckableSpinBox> temp_pointer = new CheckableSpinBox(this);
      character_configuration_widget = temp_pointer;
      vbox_layout->addWidget(character_configuration_widget);
    }

    character_configuration_widgets.at(0)->SetText(tr("Minimum number of uppercase letters"));
    character_configuration_widgets.at(0)->SetValue(initial_minimum_number_uppercase_letters);

    character_configuration_widgets.at(1)->SetText(tr("Minimum number of lowercase letters"));
    character_configuration_widgets.at(1)->SetValue(initial_minimum_number_lowercase_letters);

    character_configuration_widgets.at(2)->SetText(tr("Minimum number of digits"));
    character_configuration_widgets.at(2)->SetValue(initial_minimum_number_digits);

    character_configuration_widgets.at(3)->SetText(tr("Minimum number of special characters"));
    character_configuration_widgets.at(3)->SetValue(initial_minimum_special_characters);

    SetCharacterConfigurationWidgetsRanges(max_password_lenght);

    for (auto &character_configuration_widget : character_configuration_widgets) {
      connect(character_configuration_widget.get(), &CheckableSpinBox::OptionalSpinBoxChanged, this,
              &ConfigurationWidget::UpdatePasswordConfiguration);
    }
  }

  [[nodiscard]] StringConfiguration GetRandomStringConfiguration() const
  {
    StringConfiguration configuration(static_cast<size_t>(lenght_widget->value()),
                                      GetCharacterConfigurations());
    return configuration;
  }

  [[nodiscard]] std::vector<CharacterConfiguration> GetCharacterConfigurations() const
  {
    const auto checked_state_indices = GetCheckedStateIndices();
    std::vector<CharacterConfiguration> configurations;
    for (const auto &checked_state_index : checked_state_indices) {
      auto type = static_cast<CharacterConfiguration::CharacterType>(checked_state_index);
      const CharacterConfiguration character_configuration(
          type,
          static_cast<size_t>(character_configuration_widgets.at(checked_state_index)->Value()));
      configurations.push_back(character_configuration);
    }
    return configurations;
  }

private slots:

  void UpdatePasswordConfiguration()
  {
    // prevent that there is no choice
    const auto checked_state_indices = GetCheckedStateIndices();
    if (checked_state_indices.empty()) {
      for (auto &character_configuration_widget : character_configuration_widgets) {
        if (sender() == character_configuration_widget) {
          character_configuration_widget->SetCheckState(Qt::CheckState::Checked);
        }
      }
    }

    // prevent the password configuration from exceeding the string length
    while (ConfigurationExceedingSize() > 0) {
      if (sender() == lenght_widget) {
        if (GetCheckedValuesSum() < max_password_lenght) {
          const int configuration_sum = GetCheckedValuesSum();
          lenght_widget->setValue(configuration_sum);
        } else {
          lenght_widget->setValue(max_password_lenght);
        }
      }

      for (auto &character_configuration_widget : character_configuration_widgets) {
        if (sender() == character_configuration_widget) {
          if (GetCheckedValuesSum() <= max_password_lenght) {
            lenght_widget->setValue(GetCheckedValuesSum());
          } else {
            character_configuration_widget->SetValue(character_configuration_widget->Value() -
                                                     ConfigurationExceedingSize());
          }
        }
      }
    }
  }

private:
  [[nodiscard]] int ConfigurationExceedingSize() const
  {
    const int password_lenght = lenght_widget->value();
    const int checked_values_sum = GetCheckedValuesSum();
    const int exceeding_size = checked_values_sum - password_lenght;
    return exceeding_size;
  }

  [[nodiscard]] std::vector<size_t> GetCheckedStateIndices() const
  {
    std::vector<size_t> checked_state_indices;
    for (size_t index = 0; index < character_configuration_widgets.size(); ++index) {
      if (character_configuration_widgets.at(index)->CheckState() == Qt::CheckState::Checked) {
        checked_state_indices.push_back(index);
      }
    }
    return checked_state_indices;
  }

  [[nodiscard]] int GetCheckedValuesSum() const
  {
    const auto checked_state_indices = GetCheckedStateIndices();
    int sum = 0;
    for (const auto &checked_state_index : checked_state_indices) {
      sum += character_configuration_widgets.at(checked_state_index)->Value();
    }
    return sum;
  }

  void SetCharacterConfigurationWidgetsRanges(int max_range)
  {
    for (auto &minimum_number_widget : character_configuration_widgets) {
      minimum_number_widget->SetRange(0, max_range);
    }
  }

  static constexpr int max_password_lenght = 1024;
  static constexpr int inital_password_lenght = 32;
  static constexpr int initial_minimum_number_uppercase_letters = 1;
  static constexpr int initial_minimum_number_lowercase_letters = 1;
  static constexpr int initial_minimum_number_digits = 1;
  static constexpr int initial_minimum_special_characters = 1;

  QPointer<QSpinBox> lenght_widget;
  std::array<QPointer<CheckableSpinBox>, 4> character_configuration_widgets;
};

#endif // CONFIGURATION_WIDGET_HPP
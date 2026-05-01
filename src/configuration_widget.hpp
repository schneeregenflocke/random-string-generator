#ifndef CONFIGURATION_WIDGET_HPP
#define CONFIGURATION_WIDGET_HPP

#include "checkable_spinbox_widget.hpp"
#include "configuration.hpp"
#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtCore/qtmetamacros.h>
#include <QtCore/qttranslation.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <vector>

class ConfigurationWidget : public QWidget {
  Q_OBJECT

public:
  explicit ConfigurationWidget(QWidget *parent)
      : QWidget(parent), length_widget(new QSpinBox(this)),
        character_configuration_widgets{nullptr, nullptr, nullptr, nullptr}
  {
    const QPointer<QVBoxLayout> vbox_layout = new QVBoxLayout(this);
    vbox_layout->setContentsMargins(0, 0, 0, 0);

    length_widget->setRange(0, max_password_length);
    length_widget->setValue(initial_password_length);
    vbox_layout->addWidget(length_widget);

    vbox_layout->addWidget(new QLabel(tr("String length:"), this));

    connect(length_widget, &QSpinBox::valueChanged, this,
            &ConfigurationWidget::UpdatePasswordConfiguration);

    static constexpr std::array<const char *, character_type_count> labels{
        QT_TR_NOOP("Minimum number of uppercase letters"),
        QT_TR_NOOP("Minimum number of lowercase letters"),
        QT_TR_NOOP("Minimum number of digits"),
        QT_TR_NOOP("Minimum number of special characters")};
    static constexpr std::array<int, character_type_count> initial_values{
        initial_minimum_uppercase, initial_minimum_lowercase, initial_minimum_digits,
        initial_minimum_special};

    for (size_t index = 0; index < character_configuration_widgets.size(); ++index) {
      const QPointer<CheckableSpinBox> widget = new CheckableSpinBox(this);
      widget->SetText(tr(labels.at(index)));
      widget->SetValue(initial_values.at(index));
      widget->SetRange(0, max_password_length);
      character_configuration_widgets.at(index) = widget;
      vbox_layout->addWidget(widget);

      connect(widget, &CheckableSpinBox::OptionalSpinBoxChanged, this,
              &ConfigurationWidget::UpdatePasswordConfiguration);
    }
  }

  [[nodiscard]] StringConfiguration GetRandomStringConfiguration() const
  {
    return {static_cast<size_t>(length_widget->value()), GetCharacterConfigurations()};
  }

  [[nodiscard]] std::vector<CharacterConfiguration> GetCharacterConfigurations() const
  {
    std::vector<CharacterConfiguration> configurations;
    for (const auto checked_index : GetCheckedStateIndices()) {
      configurations.emplace_back(
          static_cast<CharacterConfiguration::CharacterType>(checked_index),
          static_cast<size_t>(character_configuration_widgets.at(checked_index)->Value()));
    }
    return configurations;
  }

private slots:

  void UpdatePasswordConfiguration()
  {
    // prevent that there is no choice
    if (GetCheckedStateIndices().empty()) {
      for (auto &widget : character_configuration_widgets) {
        if (sender() == widget) {
          widget->SetCheckState(Qt::CheckState::Checked);
        }
      }
    }

    // prevent the password configuration from exceeding the string length
    while (ConfigurationExceedingSize() > 0) {
      if (sender() == length_widget) {
        length_widget->setValue(std::min(GetCheckedValuesSum(), max_password_length));
        continue;
      }

      for (auto &widget : character_configuration_widgets) {
        if (sender() != widget) {
          continue;
        }
        if (GetCheckedValuesSum() <= max_password_length) {
          length_widget->setValue(GetCheckedValuesSum());
        } else {
          widget->SetValue(widget->Value() - ConfigurationExceedingSize());
        }
      }
    }
  }

private:
  [[nodiscard]] int ConfigurationExceedingSize() const
  {
    return GetCheckedValuesSum() - length_widget->value();
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
    int sum = 0;
    for (const auto checked_index : GetCheckedStateIndices()) {
      sum += character_configuration_widgets.at(checked_index)->Value();
    }
    return sum;
  }

  static constexpr size_t character_type_count = 4;
  static constexpr int max_password_length = 1024;
  static constexpr int initial_password_length = 32;
  static constexpr int initial_minimum_uppercase = 1;
  static constexpr int initial_minimum_lowercase = 1;
  static constexpr int initial_minimum_digits = 1;
  static constexpr int initial_minimum_special = 1;

  QPointer<QSpinBox> length_widget;
  std::array<QPointer<CheckableSpinBox>, character_type_count> character_configuration_widgets;
};

#endif // CONFIGURATION_WIDGET_HPP

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

class CharacterConfiguration {
public:
  enum class CharacterType : std::uint8_t { uppercase, lowercase, digit, special_character };

  CharacterConfiguration(CharacterType character_type, size_t character_number)
      : type(character_type), number(character_number)
  {
  }

  [[nodiscard]] CharacterType Type() const { return type; }

  [[nodiscard]] size_t Number() const { return number; }

private:
  CharacterType type;
  size_t number;
};

class StringConfiguration {
public:
  StringConfiguration(size_t length, std::vector<CharacterConfiguration> configurations)
      : string_length(length), character_configurations(std::move(configurations))
  {
  }

  [[nodiscard]] size_t StringLength() const { return string_length; }

  [[nodiscard]] size_t NumberCharacterTypes() const { return character_configurations.size(); }

  [[nodiscard]] const CharacterConfiguration &GetCharacterConfiguration(size_t index) const
  {
    return character_configurations.at(index);
  }

private:
  size_t string_length;
  std::vector<CharacterConfiguration> character_configurations;
};

#endif // CONFIGURATION_HPP
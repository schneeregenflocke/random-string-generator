#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "configuration.hpp"
#include <algorithm>
#include <cstddef>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <vector>

class Generator {
public:
  Generator() : engine(std::random_device{}()) {}

  [[nodiscard]] std::string GenerateString(const StringConfiguration &configuration)
  {
    random_indices = GenerateRandomIndices(configuration.StringLength());

    std::string random_string(configuration.StringLength(), '\0');

    size_t random_indices_position = 0;
    for (size_t index = 0; index < configuration.NumberCharacterTypes(); ++index) {
      const auto &character_configuration = configuration.GetCharacterConfiguration(index);
      for (size_t subindex = 0; subindex < character_configuration.Number(); ++subindex) {
        const auto string_position = random_indices.at(random_indices_position);
        random_string.at(string_position) = GenerateRandomChar(character_configuration.Type());
        ++random_indices_position;
      }
    }

    while (random_indices_position < configuration.StringLength()) {
      const auto random_select =
          GenerateRandomInteger(0, configuration.NumberCharacterTypes() - 1);
      const auto string_position = random_indices.at(random_indices_position);
      random_string.at(string_position) =
          GenerateRandomChar(configuration.GetCharacterConfiguration(random_select).Type());
      ++random_indices_position;
    }

    return random_string;
  }

  [[nodiscard]] const std::vector<size_t> &GetRandomIndices() const { return random_indices; }

private:
  [[nodiscard]] std::vector<size_t> GenerateRandomIndices(const size_t number)
  {
    std::vector<size_t> indices(number);
    std::ranges::iota(indices, size_t{0});
    std::ranges::shuffle(indices, engine);
    return indices;
  }

  [[nodiscard]] size_t GenerateRandomInteger(const size_t min, const size_t max)
  {
    std::uniform_int_distribution<size_t> uniform_int(min, max);
    return uniform_int(engine);
  }

  [[nodiscard]] char GenerateRandomChar(CharacterConfiguration::CharacterType type)
  {
    switch (type) {
    case CharacterConfiguration::CharacterType::uppercase:
      return static_cast<char>(GenerateRandomInteger('A', 'Z'));
    case CharacterConfiguration::CharacterType::lowercase:
      return static_cast<char>(GenerateRandomInteger('a', 'z'));
    case CharacterConfiguration::CharacterType::digit:
      return static_cast<char>(GenerateRandomInteger('0', '9'));
    case CharacterConfiguration::CharacterType::special_character:
      return special_characters.at(GenerateRandomInteger(0, special_characters.size() - 1));
    }
    return 0;
  }

  // All printable ASCII non-alphanumerics: !"#$%&'()*+,-./ :;<=>?@ [\]^_` {|}~
  static constexpr std::string_view special_characters =
      R"(!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~)";

  std::mt19937 engine;
  std::vector<size_t> random_indices;
};

#endif // GENERATOR_HPP

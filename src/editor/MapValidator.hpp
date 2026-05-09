#pragma once
#include <string>
#include <vector>

struct ValidationResult {
  bool valid = true;
  std::vector<std::string> warnings;
  std::vector<std::string> errors;

  bool hasErrors() const { return !errors.empty(); }
  bool hasWarnings() const { return !warnings.empty(); }
};

class MapValidator {
public:
  static ValidationResult validate(const std::vector<std::string> &layout);

private:
  static void checkDimensions(const std::vector<std::string> &layout,
                              ValidationResult &result);
  static void checkBorder(const std::vector<std::string> &layout,
                          ValidationResult &result);
  static void checkSpawns(const std::vector<std::string> &layout,
                          ValidationResult &result);
  static void checkDots(const std::vector<std::string> &layout,
                        ValidationResult &result);
};

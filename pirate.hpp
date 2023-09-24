#ifndef PIRATE_ARGUMENTS_HPP
#define PIRATE_ARGUMENTS_HPP

#include <map>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "types/arg_types.hpp"

namespace pirate {
class Args {
 private:
  using ArgsMap = std::map<std::string, std::string, std::less<>>;
  using ArgsSet = std::set<std::string, std::less<>>;

 public:
  explicit Args(int argc, const char** argv) { parse(format(argc, argv)); }

  /**
   * @brief Adds an argument flag that can be accepted
   * 
   * @param key : argument key to add
   */
  static void register_arg(const std::string& key) { get_arg_map()[key] = ArgType::OPTIONAL; }

  /**
   * @brief Adds an argument flag that can be accepted, and marks it required
   * 
   * @param key : argument key to add
   * @param type : argument type
   */
  static void register_arg(const std::string& key, ArgType type) { get_arg_map()[key] = type; }

  /**
   * @brief Returns true if the argument is present
   * 
   * @param key : argument key
   * @return true 
   * @return false 
   */
  auto has(std::string_view key) -> bool { return _args.find(key) != _args.end(); }

  /**
   * @brief Returns the argument with the given key
   * 
   * @param key : argument key
   * @return const std::string& : argument value
   */
  auto get(std::string_view key) -> const std::string& {
    auto iter = _args.find(key);
    if (iter == _args.end())
      throw std::out_of_range("Attempt To Access Missing Argument:" + std::string(key));
    return iter->second;
  }

  /**
   * @brief Returns the number of arguments
   * 
   * @return size_t : argument count
   */
  auto count() -> size_t { return _args.size(); }

  static void reset() {
    get_arg_map().clear();
    get_req_set().clear();
  }

 private:
  ArgsMap _args{};

  /**
   * @brief Splits an argument into its key/value
   * 
   * @param arg : argument to split
   * @return std::pair<std::string, std::string> : arguments key/value
   */
  static auto split_arg(std::string_view arg) -> std::pair<std::string, std::string> {
    auto loc = arg.find('=');
    if (loc == std::string::npos) return {std::string(arg.begin() + 1), ""};

    return {std::string(arg.begin() + 1, loc - 1),
            std::string(arg.begin() + loc + 1, arg.length() - (loc + 1))};
  }

  static auto split_single_letter_args(std::string_view arg) -> std::vector<std::string> {
    std::vector<std::string> args;
    for (auto chr : arg) {
      args.emplace_back(1, chr);
    }
    return args;
  }

  /**
   * @brief Parses out the arguments passed into the program
   * 
   * @param argv 
   */
  void parse(const std::vector<std::string>& argv) {
    for (auto arg = std::next(argv.begin()); arg != argv.end(); ++arg) {
      if (arg->length() < 2) throw std::runtime_error("Unknown Flag: " + *arg);
      if ((*arg)[0] != '-') throw std::runtime_error("Unknown Flag: " + *arg);

      auto [flag, val] = split_arg(*arg);

      auto [fullValid, fullError] = validate_flag(flag, val);
      if (fullValid) {
        _args[flag] = val;
        continue;
      }

      auto [compositeValid, compositeError] = validate_composite_flag(flag);
      if (compositeValid) {
        continue;
      }

      throw std::runtime_error(fullError + ": " += flag);
    }

    check_required(_args);
  }

  auto validate_composite_flag(const std::string& arg) -> std::pair<bool, std::string> {
    auto flags = split_single_letter_args(arg);
    for (const auto& flag : flags) {
      auto valid = validate_flag(flag, "");
      if (!valid.first) return valid;
      _args[flag] = "";
    }

    return {true, ""};
  }

  /**
   * @brief Checks if a flag is valid
   * 
   * @param flag : flag to check
   */
  static auto validate_flag(std::string_view flag, std::string_view value) -> std::pair<bool, std::string> {
    auto& argSet = get_arg_map();
    auto  iter = argSet.find(flag);
    if (iter == argSet.end()) return {false, "Unknown Flag"};

    auto type = iter->second;

    if ((type & ArgType::VALUE_REQUIRED) == ArgType::VALUE_REQUIRED) {
      if (value.empty()) return {false, "Required Value Missing"};
    }

    return {true, ""};
  }

  /**
   * @brief Checks that all required flags are set
   * 
   * @param args : arguments map
   */
  static void check_required(const ArgsMap& args) {
    for (const auto& arg : get_req_set()) {
      if (args.find(arg) == args.end()) throw std::runtime_error("Missing Flag: " + arg);
    }
  }

  /**
   * @brief Converts argc/argv into a vector of strings
   * 
   * @param argc : command line argument count
   * @param argv : command line arguemnts
   * @return std::vector<std::string> 
   */
  static auto format(int argc, const char** argv) -> std::vector<std::string> {
    return std::vector<std::string>{argv, std::next(argv, static_cast<std::ptrdiff_t>(argc))};
  }

  static inline auto get_arg_map() -> std::map<std::string, ArgType, std::less<>>& {
    static std::map<std::string, ArgType, std::less<>> argSet;
    return argSet;
  }

  static inline auto get_req_set() -> ArgsSet& {
    static ArgsSet reqSet;
    return reqSet;
  }
};
}  // namespace pirate

#endif
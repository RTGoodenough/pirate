#ifndef PIRATE_ARGUMENTS_HPP
#define PIRATE_ARGUMENTS_HPP

#include <map>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace pirate {
class Args {
 public:
  explicit Args(int argc, const char** argv) { parse(format(argc, argv)); }

  /**
   * @brief Adds an argument flag that can be accepted, and marks it required
   * 
   * @param key : argument key to add
   * @param required : true if the argument is required
   */
  static void register_arg(std::string_view key, bool required, bool valueRequired) {
    get_arg_set().emplace(key);
    if (required) get_req_set().emplace(key);
    if (valueRequired) get_val_set().emplace(key);
  }

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
    get_arg_set().clear();
    get_req_set().clear();
  }

 private:
  std::map<std::string, std::string, std::less<>> _args{};

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
      if (validate_flag(flag, val)) {
        _args[flag] = val;
        continue;
      }

      validate_composite_flag(flag);
    }

    check_required(_args);
  }

  void validate_composite_flag(const std::string& arg) {
    auto flags = split_single_letter_args(arg);
    for (const auto& flag : flags) {
      if (!validate_flag(flag, "")) throw std::runtime_error("Unknown Flag: " + flag);
      _args[flag] = "";
    }
  }

  /**
   * @brief Checks if a flag is valid
   * 
   * @param flag : flag to check
   */
  static auto validate_flag(std::string_view flag, std::string_view value) -> bool {
    auto& argSet = get_arg_set();
    auto  iter = argSet.find(flag);
    if (iter == argSet.end()) return false;

    auto& valSet = get_val_set();
    auto  valIter = valSet.find(flag);
    if (valIter == valSet.end()) return true;

    return !value.empty();
  }

  /**
   * @brief Checks that all required flags are set
   * 
   * @param args : arguments map
   */
  static void check_required(const std::map<std::string, std::string, std::less<>>& args) {
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

  static inline auto get_arg_set() -> std::set<std::string, std::less<>>& {
    static std::set<std::string, std::less<>> argSet;
    return argSet;
  }

  static inline auto get_req_set() -> std::set<std::string, std::less<>>& {
    static std::set<std::string, std::less<>> reqSet;
    return reqSet;
  }

  static inline auto get_val_set() -> std::set<std::string, std::less<>>& {
    static std::set<std::string, std::less<>> valSet;
    return valSet;
  }
};
}  // namespace pirate

#endif
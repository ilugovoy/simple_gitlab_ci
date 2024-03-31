#include "s21_grep_func.h"

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "ru_RU.utf8");

  Flags flags;
  flags_init(&flags);
  bool any_flags = find_flags(argc, argv, &flags);
  check_input(argc, &flags, any_flags);

  output_s21_grep(&flags, argc, argv);

  // освобождаем память
  clear_memory(&flags);
  // if (flags.regex_patterns) regfree(&flags.regex_patterns);

  return EXIT_SUCCESS;
}

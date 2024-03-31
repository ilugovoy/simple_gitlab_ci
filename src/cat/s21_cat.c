#include "s21_cat_func.h"

int main(int arg_count, char* files[]) {
  setlocale(LC_ALL, "ru_RU.utf8");

  if (arg_count < 2) {
    fprintf(stderr, RED_COLOR
            "Не указан файл, пример: ./s21_cat test.txt\n" RESET_COLOR);
    return EXIT_FAILURE;
  }

  Flags flags;
  flags_init(&flags);

  find_flags(arg_count, files, &flags);

  for (int i = 1; i < arg_count; i++) {
    if (is_file(files[i])) cat(files[i], &flags);
  }

  return EXIT_SUCCESS;
}

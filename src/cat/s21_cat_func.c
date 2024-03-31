#include "s21_cat_func.h"

void cat(const char* filename, Flags* flags) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, RED_COLOR "%s: ", filename);
    perror("");
    fprintf(stderr, RESET_COLOR);
    return;
  }

  // буфер для файла с блоками размером 4096 байт
  if (setvbuf(file, NULL, _IOFBF, 4096) != 0) {
    fprintf(stderr, RED_COLOR "%s: ", filename);
    perror("Error calling setvbuf()");
    fprintf(stderr, RESET_COLOR);
    fclose(file);
    return;
  }

  print_file(file, flags);

  if (fclose(file) != 0) {
    fprintf(stderr, RED_COLOR "%s: ", filename);
    perror("");
    fprintf(stderr, RESET_COLOR);
    return;
  }
}

void find_flags(int arg_count, char* files[], Flags* flags) {
  for (int i = 1; i < arg_count; i++) {
    if (!is_file(files[i])) {
      if (IS_HELP(files[i])) {
        print_help();
        exit(0);
      }

      bool flag_or_not = false;

      if (strlen(files[i]) > 2 && files[i][0] == '-') {
        flag_or_not = is_combined_flag(files[i], flags);
      } else {
        flag_or_not = is_flag(files[i], flags);
      }

      if (files[i][0] == '-' && !flag_or_not) {
        fprintf(stderr,
                RED_COLOR "s21_cat_by_fungusgr_1.0: invalid option -- \'%c\'\n",
                files[i][1]);
        fprintf(stderr,
                RESET_COLOR "Try './s21_cat --help' for more information.\n");
        exit(1);
      }

      if (!flag_or_not) {
        fprintf(stderr, RED_COLOR "s21_cat_by_fungusgr_1.0: \'%s\'\n",
                files[i]);
        perror("");
        fprintf(stderr, RESET_COLOR);
        exit(1);
      }
    }
  }
}

void print_file(FILE* filename, Flags* flags) {
  int line_number = 1;
  int ch, prev_ch = '\0';
  bool is_first_line = true;
  int printed_empty_line_number = 1;

  if (flags->str_num_flag && !flags->number_nonblank_flag) {
    int next_ch = fgetc(filename);
    if (next_ch == EOF)
      return;
    else
      ungetc(next_ch, filename);
    printf("%6d\t", line_number++);
  }

  while ((ch = fgetc(filename)) != EOF) {
    if (flags->squeeze_flag && ch == '\n' && prev_ch == '\n') {
      if (!printed_empty_line_number) {
        printed_empty_line_number = 1;
      } else
        continue;
    }

    if (flags->number_nonblank_flag) {
      if (is_first_line && !end_of_line(ch)) {
        printf("%6d\t", line_number++);
        is_first_line = false;
      } else if (prev_ch == '\n' && !end_of_line(ch)) {
        printf("%6d\t", line_number++);
      }
    }

    if (flags->end_of_line_flag && end_of_line(ch)) {
      puts("$");
    } else if (flags->tab_flag && transform_tab(ch)) {
      ;
    } else if (flags->show_control_chars_flag &&
               transform_nonprinting_sym(ch)) {
      ;
    } else
      putchar(ch);

    int next_ch = fgetc(filename);
    if (next_ch == EOF)
      return;
    else
      ungetc(next_ch, filename);

    if (flags->str_num_flag && end_of_line(ch) &&
        !flags->number_nonblank_flag) {
      printf("%6d\t", line_number++);
    }

    if (ch != '\n') printed_empty_line_number = 0;
    prev_ch = ch;
  }
}

bool is_combined_flag(const char* arg, Flags* flags) {
  for (size_t i = 1; i < strlen(arg); i++) {  // первый символ дефис, поэтому 1
    int ch = arg[i];
    if (ch == 'e' || ch == 'E') flags->end_of_line_flag = true;
    if (ch == 'v') flags->show_control_chars_flag = true;
    if (ch == 't' || ch == 'E') flags->tab_flag = true;
    if (ch == 'b') flags->number_nonblank_flag = true;
    if (ch == 's') flags->squeeze_flag = true;
    if (ch == 'n') flags->str_num_flag = true;
  }

  if (flags->number_nonblank_flag || flags->end_of_line_flag ||
      flags->str_num_flag || flags->squeeze_flag || flags->tab_flag ||
      flags->show_control_chars_flag)
    return true;

  return false;
}

bool is_flag(const char* arg, Flags* flags) {
  if (SHOW_CONTROL_CHAR(arg)) flags->show_control_chars_flag = true;
  if (NUMBER_NONBLANK(arg)) flags->number_nonblank_flag = true;
  if (END_OF_LINE(arg)) flags->end_of_line_flag = true;
  if (STR_NUM(arg)) flags->str_num_flag = true;
  if (SQUEEZE_BLANK(arg)) flags->squeeze_flag = true;
  if (TAB(arg)) flags->tab_flag = true;

  if (flags->number_nonblank_flag || flags->end_of_line_flag ||
      flags->str_num_flag || flags->squeeze_flag || flags->tab_flag ||
      flags->show_control_chars_flag)
    return true;

  return false;
}

void flags_init(Flags* flags) {
  flags->show_control_chars_flag = false;
  flags->number_nonblank_flag = false;
  flags->end_of_line_flag = false;
  flags->str_num_flag = false;
  flags->squeeze_flag = false;
  flags->tab_flag = false;
}

bool is_file(const char* arg) {
  int fd = open(arg, O_RDONLY);
  if (fd != -1) {
    close(fd);
    return true;
  } else {
    return false;
  }
}

int end_of_line(int ch) { return ch == '\n'; }

int transform_tab(int ch) {
  if (ch == '\t') {
    putchar('^');
    putchar('I');
    return 1;
  }
  return 0;
}

int transform_nonprinting_sym(int ch) {
  if (ch == '\n') {
    printf("\n");
    return 1;
  } else if (ch == '\t') {
    printf("\t");
    return 1;
  } else if (isprint(ch)) {
    putchar(ch);
    return 1;
  } else if (ch == 127) {
    printf("^?");
    return 1;
  } else {
    printf("^%c", ch + 64);
    return 1;
  }
  return 0;
}

void print_help() {
  printf(
      "Добро пожаловать, бро! Ты в справке по программе s21_cat_by_fungusgr\n");
  printf(
      "Это утилита командной строки, которая используется для вывода "
      "содержимого файлов на экран.\n\n");
  printf("Вот список флагов, которые вы можете использовать с программой:\n\n");
  printf(
      "\"-n\" или \"--number\": отображает строки с номерами. \"cat -n "
      "filename\" выведет содержимое файла с номерами строк.\n");
  printf(
      "\"-s\" или \"--squeeze-blank\": удаляет повторяющиеся пустые строки. "
      "\"cat -s filename\" выведет содержимое файла без повторяющихся пустых "
      "строк.\n");
  printf(
      "\"-E\" или --show-ends: выводит символ '$' в конце каждой строки. \"cat "
      "-E filename\" выведет содержимое файла с символом '$' в конце каждой "
      "строки.\n");
  printf(
      "\"-b\" или \"--number-nonblank\": пронумерует только непустые строки. "
      "\"cat -b filename\" выведет только непустые строки с номерами "
      "строк.\n\n");
  printf("Примеры использования флагов:\n\n");
  printf("1. Вывод содержимого файла с номерами строк:\n");
  printf("   Команда: cat -n filename.txt\n");
  printf("   Результат:\n");
  printf("   1   Строка 1\n");
  printf("   2   Строка 2\n");
  printf("   3   Строка 3\n\n");
  printf("2. Вывод содержимого файла без повторяющихся пустых строк:\n");
  printf("   Далее 3 пустые строки \n\n\n");
  printf("   Команда: cat -s filename.txt\n");
  printf("   Результат:\n");
  printf("   Строка 1\n\n");
  printf("   Строка 2\n\n");
  printf("   Строка 3\n\n");
  printf("3. Вывод содержимого файла с символами '$' в конце каждой строки:\n");
  printf("   Команда: cat -E filename.txt\n");
  printf("   Результат:\n");
  printf("   Строка 1$\n");
  printf("   Строка 2$\n");
  printf("   Строка 3$\n\n");
  printf("Есть вопрос? Пиши fungusgr.21-school@yandex.ru\n");
}

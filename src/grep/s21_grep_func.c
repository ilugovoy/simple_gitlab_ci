#include "s21_grep_func.h"

void output_s21_grep(Flags* flags, int argc, char* argv[]) {
  pattern_mem_alloc(flags, argv, argc);

  int pattern_index = -1;
  find_patterns(argc, argv, flags, &pattern_index);

#ifdef DEBUG
  for (int i = 0; i < flags->pattern_count; i++) {
    printf("flags.patterns[i] = ");
    puts(flags->patterns[i]);
  }
  printf("flags.pattern_count = %d\n", flags->pattern_count);
  printf("pattern_index = %d\n", pattern_index);
#endif

  for (int i = argc - 1; i > pattern_index; i--) {
    if (!is_flag(argv[i])) flags->file_num++;  //считаем файлы
  }
#ifdef DEBUG
  printf("files_count = %d\n", flags->file_num);
  printf(
      "___________________________________________________________________"
      "\n\n");
#endif

  // ищем таргет в каждом файле и выводим в соответствии с флагами
  for (int i = pattern_index + 1; i < argc; i++) {
    if (!is_flag(argv[i])) grep(argv[i], flags);
  }
}

void grep(const char* filename, Flags* flags) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    error_message(flags, filename);
    return;
  }

  // буфер для файла с блоками размером 4096 байт
  if (setvbuf(file, NULL, _IOFBF, 4096) != 0) {
    error_message(flags, filename);
    fclose(file);
    return;
  }

  print_file_grep(file, flags, filename);

  if ((fclose(file) != 0)) {
    error_message(flags, filename);
    return;
  }
}

void print_file_grep(FILE* filename, Flags* flags, const char* name_file) {
  char line[1024];
  int match_count = 0;  // для флага -c
  int line_number = 0;  // для флага -n

  while (fgets(line, sizeof(line), filename) != NULL) {
    line_number++;
    bool line_has_match = false;  // для отслеживания совпадений в строке
    find_matches_in_line(flags, &line_has_match, line, name_file);
    if (line_has_match == true && !flags->inverted_res_flag) {
      if (flags->files_with_matches_flag) {
        printf("%s\n", name_file);
        return;
      }

      if (flags->match_count_flag) {
        match_count++;
        continue;
      }

      print_line(flags, line_number, line, name_file);
    } else if (line_has_match == false && flags->inverted_res_flag) {
      if (flags->files_with_matches_flag) {
        printf("%s\n", name_file);
        return;
      }
      if (flags->match_count_flag) {
        match_count++;
      } else
        print_line(flags, line_number, line, name_file);
    }
  }
  if (flags->match_count_flag) {
    if (flags->file_num > 1 && !flags->no_filename_flag &&
        !flags->only_matching_flag) {
      printf("%s:", name_file);
    }
    printf("%d\n", match_count);
    return;
  }
}

void print_line(Flags* flags, int line_number, char line[],
                const char* name_file) {
  if (flags->file_num > 1 && !flags->no_filename_flag &&
      !flags->only_matching_flag) {
    printf("%s:", name_file);  // если более 1 файла
  }
  if (flags->line_number_flag) {
    printf("%d:", line_number);
  }
  int length = strlen(line);
  if (!flags->only_matching_flag) {
    if (line[length - 1] != '\n')
      printf("%s\n", line);
    else
      printf("%s", line);
  }
}

void pattern_mem_alloc(Flags* flags, char* argv[], int argc) {
  flags->patterns = calloc(argc - 1, sizeof(char*));
  if (flags->patterns == NULL) {
    if (!flags->no_filename_flag) {
      error_message(flags, (const char*)argv);
    }
    exit(EXIT_FAILURE);
  }
}

void clear_memory(Flags* flags) {
  for (int i = 0; i < flags->pattern_count; i++) {
    regfree(flags->regex_patterns[i]);
    free(flags->regex_patterns[i]);
  }  // освобождаем выделенную память для regex

  for (int i = 0; i < flags->pattern_count; i++) {
    free(flags->patterns[i]);
  }  // освобождаем выделенную strdup-ом память
  free(flags->patterns);
  free(flags->pattern_file_name);  // и всё остальное
}

void check_input(int argc, Flags* flags, bool any_flags) {
  if ((argc < 3 || (argc == 3 && any_flags))) {
    if (!flags->silent_flag) {
      fprintf(stderr, RED_COLOR
              "s21_grep_by_fungusgr: для работы программы нужно передавать "
              "минимум два аргумента - паттерн и файл\n");
      fprintf(stderr, RESET_COLOR "Например: ./s21_grep test test.txt \n");
    }
    exit(EXIT_FAILURE);
  }
}

void error_message(Flags* flags, const char* argv) {
  if (!flags->silent_flag) {
    fprintf(stderr, RED_COLOR "s21_grep_by_fungusgr: %s: ", argv);
    perror("");
    fprintf(stderr, RESET_COLOR);
  }
}

void find_matches_in_line(Flags* flags, bool* line_has_match, char line[],
                          const char* name_file) {
  for (int i = 0; i < flags->pattern_count; i++) {
    regmatch_t matches[10];  // Изменим размер массива matches на достаточное
                             // количество
    int offset = 0;  // для отслеживания смещения строки
    while (regexec(flags->regex_patterns[i], line + offset, 10, matches, 0) ==
           0) {
      *line_has_match = true;
      if (flags->only_matching_flag) {
        for (int j = 0; j < 10; j++) {
          if (matches[j].rm_so == -1) {  // Найдены все совпадения
            break;
          }
          if (flags->file_num > 1 && !flags->no_filename_flag) {
            printf("%s:", name_file);  // если более 1 файла
          }
          // printf("%.*s\n", matches[j].rm_eo - matches[j].rm_so,
          //        &line[matches[j].rm_so + offset]);
          printf("%.*lld\n", (int)(matches[j].rm_eo - matches[j].rm_so),
                 (long long int)matches[j].rm_eo - matches[j].rm_so);
        }
      }
      offset += matches[0].rm_eo;  // Увеличиваем смещение для следующего поиска
    }
  }
}

bool find_flags(int arg_count, char* files[], Flags* flags) {
  bool flag_or_not = false;
  for (int i = 1; i < arg_count; i++) {
    if (files[i][0] == '-') {
      flag_or_not = is_combined_flag_grep(files[i], flags);
      if (!flag_or_not) {
        if (!flags->no_filename_flag) {
          fprintf(stderr,
                  RED_COLOR
                  "s21_grep_by_fungusgr_1.0: invalid option -- \'%c\'\n",
                  files[i][1]);
          fprintf(stderr, RESET_COLOR
                  "Try './s21_grep --help' for more information.\n");
        }
        exit(1);
      }
    }
  }
  return flag_or_not;
}

bool is_combined_flag_grep(const char* arg, Flags* flags) {
  for (size_t i = 1; i < strlen(arg);
       i++) {  // первый символ '-' или '\', поэтому 1
    int ch = arg[i];
    if (ch == 'l') flags->files_with_matches_flag = true;
    if (ch == 'i') flags->ignoring_the_case_flag = true;
    if (ch == 'o') flags->only_matching_flag = true;
    if (ch == 'e') flags->many_patterns_flag = true;
    if (ch == 'v') flags->inverted_res_flag = true;
    if (ch == 'c') flags->match_count_flag = true;
    if (ch == 'n') flags->line_number_flag = true;
    if (ch == 'h') flags->no_filename_flag = true;
    if (ch == 's') flags->silent_flag = true;
    if (ch == 'f') flags->file_target_flag = true;
  }
  if (flags->files_with_matches_flag || flags->ignoring_the_case_flag ||
      flags->only_matching_flag || flags->many_patterns_flag ||
      flags->inverted_res_flag || flags->match_count_flag ||
      flags->line_number_flag || flags->no_filename_flag ||
      flags->silent_flag || flags->file_target_flag)
    return true;
  return false;
}

bool is_flag(const char* arg) {
  if (arg[0] == '-') return true;
  return false;
}

void flags_init(Flags* flags) {
  flags->files_with_matches_flag = false;
  flags->ignoring_the_case_flag = false;
  flags->only_matching_flag = false;
  flags->many_patterns_flag = false;
  flags->inverted_res_flag = false;
  flags->match_count_flag = false;
  flags->line_number_flag = false;
  flags->file_target_flag = false;
  flags->no_filename_flag = false;
  flags->silent_flag = false;

  flags->pattern_file_name = NULL;
  flags->patterns = NULL;

  flags->pattern_count = 0;
  flags->file_num = 0;
}

char* set_target(const char* argv, Flags* flags) {
  char* target = strdup(argv);
  if (target == NULL) {
    if (!flags->no_filename_flag) {
      error_message(flags, argv);
    }
    exit(EXIT_FAILURE);
  }
  return target;
}

void create_regex_pattern(Flags* flags, char* argv) {
  flags->regex_patterns[flags->pattern_count - 1] =
      (regex_t*)malloc(sizeof(regex_t));
  if (flags->regex_patterns[flags->pattern_count - 1] == NULL) {
    if (!flags->no_filename_flag) {
      error_message(flags, argv);
    }
    exit(EXIT_FAILURE);
  }

  int reg_mode = REG_EXTENDED, ret;
  if (flags->ignoring_the_case_flag) reg_mode = REG_EXTENDED | REG_ICASE;
  ret = regcomp(flags->regex_patterns[flags->pattern_count - 1],
                flags->patterns[flags->pattern_count - 1], reg_mode);
  if (ret != 0) {
    char bug_error_message[1024];
    regerror(ret, flags->regex_patterns[flags->pattern_count - 1],
             bug_error_message, sizeof(bug_error_message));
    fprintf(stderr, "Ошибка компиляции регулярного выражения: %s\n",
            bug_error_message);
    regfree(flags->regex_patterns[flags->pattern_count - 1]);
    free(flags->regex_patterns[flags->pattern_count - 1]);
    exit(EXIT_FAILURE);
  }
}

void find_patterns(int argc, char* const argv[], Flags* flags,
                   int* pattern_index) {
  for (int i = 1; i < argc - 1; i++) {
    if (flags->file_target_flag && argv[i][0] != '-' &&
        (strcmp(argv[i - 1], "-f") == 0)) {
      flags->pattern_file_name = strdup(argv[i]);
      FILE* pattern_file = fopen(flags->pattern_file_name, "r");
      if (pattern_file == NULL) {
        if (!flags->no_filename_flag) {
          error_message(flags, argv[i]);
        }
        exit(EXIT_FAILURE);
      }
      char buffer[256];
      while (fgets(buffer, sizeof(buffer), pattern_file) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
          buffer[len - 1] = '\0';  // Удаляем символ новой строки
        }
        flags->patterns[flags->pattern_count++] =
            set_target(buffer, flags);  // Сохраняем паттерн из файла
        create_regex_pattern(flags, argv[i]);
        *pattern_index = i;
      }
      fclose(pattern_file);
    } else {
      if (!flags->many_patterns_flag && argv[i][0] != '-') {
        flags->patterns[flags->pattern_count++] = set_target(argv[i], flags);
        create_regex_pattern(flags, argv[i]);
        *pattern_index = i;
        break;
      } else if (flags->many_patterns_flag && argv[i][0] != '-' &&
                 (strcmp(argv[i - 1], "-e") == 0)) {
        flags->patterns[flags->pattern_count++] = set_target(argv[i], flags);
        create_regex_pattern(flags, argv[i]);
        *pattern_index = i;
      }
    }
  }
}

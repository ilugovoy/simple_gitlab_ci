#ifndef S21_CAT
#define S21_CAT

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ANSI макросы для изменения цвета символов
#define RED_COLOR "\033[0;31m"
#define RESET_COLOR "\033[0m"

// макросы для опознания флагов в аргументах
#define END_OF_LINE(arg)                               \
  (strcmp(arg, "-e") == 0 || strcmp(arg, "-E") == 0 || \
   strcmp(arg, "--show-ends") == 0)
#define TAB(arg)                                                \
  (strcmp(arg, "-t") == 0 || strcmp(arg, "--show-tabs") == 0 || \
   strcmp(arg, "-T") == 0)
#define NUMBER_NONBLANK(arg) \
  (strcmp(arg, "-b") == 0 || strcmp(arg, "--number-nonblank") == 0)
#define SQUEEZE_BLANK(arg) \
  (strcmp(arg, "-s") == 0 || strcmp(arg, "--squeeze-blank") == 0)
#define STR_NUM(arg) (strcmp(arg, "-n") == 0 || strcmp(arg, "--number") == 0)
#define SHOW_CONTROL_CHAR(arg) \
  (strcmp(arg, "-v") == 0 || strcmp(arg, "-e") == 0 || strcmp(arg, "-t") == 0)

// макрос и функция для поддержки флага --help
#define IS_HELP(arg) (strcmp(arg, "--help") == 0)
void print_help();

// структура для хранения найденных флагов
typedef struct {
  bool show_control_chars_flag;
  bool number_nonblank_flag;
  bool end_of_line_flag;
  bool str_num_flag;
  bool squeeze_flag;
  bool tab_flag;
} Flags;

// основные функции для определения флагов и вывода
bool is_combined_flag(const char* arg, Flags* flags);
bool is_flag(const char* arg, Flags* flags);
bool is_file(const char* arg);

void find_flags(int arg_count, char* files[], Flags* flags);
void print_file(FILE* filename, Flags* flags);
void cat(const char* filename, Flags* flags);
void flags_init(Flags* flags);

// вспомогательные функции для применения флагов
int transform_nonprinting_sym(int ch);
int transform_tab(int ch);
int end_of_line(int ch);

#endif
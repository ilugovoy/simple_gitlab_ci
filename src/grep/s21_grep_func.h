#ifndef S21_GREP
#define S21_GREP

#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <pcre.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#define DEBUG 0

// ANSI макросы для изменения цвета bug_error
#define GREEN_COLOR "\033[032m"
#define RED_COLOR "\033[031m"
#define RESET_COLOR "\033[0m"

char* strdup(const char* str);

typedef struct {
  regex_t* regex_patterns[1024];
  bool ignoring_the_case_flag;
  bool files_with_matches_flag;
  bool extended_regexp_flag;
  bool only_matching_flag;
  bool many_patterns_flag;
  bool inverted_res_flag;
  bool line_number_flag;
  bool match_count_flag;
  bool no_filename_flag;
  bool file_target_flag;
  bool silent_flag;

  char* pattern_file_name;
  char** patterns;

  int pattern_count;
  int file_num;

} Flags;

// основные функции
void find_patterns(int argc, char* const argv[], Flags* flags,
                   int* pattern_index);
bool find_flags(int arg_count, char* files[], Flags* flags);
void output_s21_grep(Flags* flags, int argc, char* argv[]);
void create_regex_pattern(Flags* flags, char* argv);
void grep(const char* filename, Flags* flags);
void flags_init(Flags* flags);

// функции для форматирования вывода
bool is_combined_flag_grep(const char* arg, Flags* flags);
bool is_flag(const char* arg);
bool is_file(const char* arg);

void find_matches_in_line(Flags* flags, bool* line_has_match, char line[],
                          const char* name_file);
void print_line(Flags* flags, int line_number, char line[],
                const char* name_file);
void print_file_grep(FILE* filename, Flags* flags, const char* name_file);

// вспомогательные функции
void pattern_mem_alloc(Flags* flags, char* argv[], int argc);
void check_input(int argc, Flags* flags, bool any_flags);
void error_message(Flags* flags, const char* argv);
char* set_target(const char* argv, Flags* flags);
void clear_memory(Flags* flags);

#endif
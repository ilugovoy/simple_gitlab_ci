LFLAGS=-g
CFLAGS=-Wall -Werror -Wextra -c -std=c11 -pedantic
CC = gcc
TARGET = s21_grep
SRC = s21_grep.c s21_grep_func.c s21_grep_func.h
OBJDIR = obj
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC))

#  цели и сценарии
all: brew_check $(TARGET) clean_o 

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

$(OBJDIR)/%.o: %.c | $(OBJDIR) 
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

test: $(TARGET) clean_o unit_test cpp style_test

unit_test:
	@cd unit_tests && bash ./unit_tests_grep.sh || (echo "Unit tests failed" && exit 1)

style_test:
	@cd unit_tests && bash ./style_test.sh || (echo "Style tests failed" && exit 1)

cpp:
	@cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

brew_check:
	@cd unit_tests && bash ./install_brew.sh || (echo "install_brew.sh failed" && exit 1)


#  очистка
clean: clean_o clean_res clean_obj clean_test_log

clean_test_log:
	@cd unit_tests && rm -f tests_result.log
	@cd unit_tests && rm -f s21_grep.log
	@cd unit_tests && rm -f sys_grep.log
	@cd unit_tests && rm -f leak_test.log

clean_obj:
	rm -rf $(OBJDIR)

clean_o:
	rm -f $(OBJDIR)/*.o

clean_res:
	rm -f $(TARGET)

#  прочее
format:
	clang-format -n *.c *.h
	clang-format -i *.c *.h
	

.PHONY: clean all clean_o cpp format clean_res


# $< : Это автоматическая переменная в GNU make, которая содержит первую зависимость (то есть исходный файл)
# $@ : Это автоматическая переменная в GNU make, которая содержит имя цели (то есть имя объектного файла)
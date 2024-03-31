CC = gcc
LFLAGS= -g
CFLAGS=-Wall -Werror -Wextra -c -std=c11 -pedantic -D_GNU_SOURCE

SRC = s21_cat.c s21_cat_func.c s21_cat_func.h
TARGET = s21_cat
OBJDIR = obj
OBJECTS = $(SRC:%.c=$(OBJDIR)/%.o)

#  цели и сценарии
all: $(TARGET) clean_o

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

$(OBJDIR)/%.o: %.c | $(OBJDIR) 
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

test: $(TARGET) brew_check clean_o unit_test cpp style_test


brew_check:
	@cd tests && bash ./install_brew.sh || (echo "install_brew.sh failed" && exit 1)


unit_test:
	@cd tests && bash ./unit_tests_cat.sh || (echo "Unit tests failed" && exit 1)


style_test:
	@cd tests && bash ./style_test.sh || (echo "Style tests failed" && exit 1)


cpp:
	@cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

	
#  очистка
clean: clean_o clean_res clean_obj clean_test_log
	
clean_test_log:
	@cd tests && rm -f tests_result.log
	@cd tests && rm -f s21_cat_log.log
	@cd tests && rm -f cat_log.log
	@cd tests && rm -f leak_test.log

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
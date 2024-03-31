#!/bin/bash

LEAK_TEST_FLAG=1 # установить 0, чтобы пропускать проверку на утечки
DONT_STOP_FLAG=1 # установить 0, чтобы продолжать работу даже при неудачных тестах

TEST_FILES_DIR="files_for_tests"
TEST_PROGRAM="../s21_grep"
MEMORY_LEAK_TEST_TOOL=""
LEAK_TEST_NUM=1
DIFF_RES=""
SYS_GREP=""
TEST_NUM=1
SUCCESS=0
FAIL=0

ANIMATION_DELAY=0.1
ANIMATION_FRAMES=("⠋" "⠙" "⠹" "⠸" "⠼" "⠴" "⠦" "⠧" "⠇" "⠏")

# Флаги для тестирования в цикле
declare -a options=(
"i"
"v"
"c"
"l"
"n"
"h"
"s"
# "o" # включить для тестирования флага -o
)

# Параметры тестирования: флаги, паттерны и файлы
declare -a multy_testing=(
"in $TEST_FILES_DIR/test1.txt"
"-e in -e 90 -e 21 $TEST_FILES_DIR/test2.txt $TEST_FILES_DIR/test3.txt"
"-e in -e \"i.*n\" $TEST_FILES_DIR/test1.txt $TEST_FILES_DIR/test2.txt $TEST_FILES_DIR/test3.txt"
"-e in -e \"^int\" -f $TEST_FILES_DIR/patterns1.txt $TEST_FILES_DIR/test1.txt $TEST_FILES_DIR/test2.txt $TEST_FILES_DIR/test3.txt"
"-e in -e \"txt$\" -f $TEST_FILES_DIR/patterns1.txt -f $TEST_FILES_DIR/patterns2.txt -f $TEST_FILES_DIR/patterns3.txt $TEST_FILES_DIR/test1.txt $TEST_FILES_DIR/test2.txt $TEST_FILES_DIR/test3.txt"
)

# functions_____________________________________________________________________________________________________________________________

# Определение ОС
if [ "$(uname)" == "Darwin" ]; 
	then 
		MEMORY_LEAK_TEST_TOOL="leaks -atExit --" 
		SYS_GREP="ggrep"
	else 
		MEMORY_LEAK_TEST_TOOL="valgrind --tool=memcheck --leak-check=yes"  
		SYS_GREP="grep"
fi

# Тест на утечки
function memory_leak_testing() {

    if [ $LEAK_TEST_NUM -lt 10 ] && [ $(($TEST_NUM % 10)) -eq 0 ]; 
    then
	    local t="$1"
		echo -e "LEAK TEST $LEAK_TEST_NUM: $TEST_PROGRAM $t -->\tSTARTED" >> leak_test.log
		$MEMORY_LEAK_TEST_TOOL $TEST_PROGRAM $TEST_FILES_DIR/$t 2>&1 | grep -E "LEAK TEST|Command:|SUCCESS|ERROR SUMMARY|in use|total heap|allocated|no leaks are possible" >> leak_test.log
		local return_code=$?
	
        if [ $return_code -eq 0 ]; 
        then 
            echo -e "LEAK TEST $LEAK_TEST_NUM: $TEST_PROGRAM $t -->\tOK" >> tests_result.log
			echo -e "______________________________________________________________________________________________________________________________________________\n" >> leak_test.log
			((LEAK_TEST_NUM++))
        else 
            echo -e "LEAK TEST $LEAK_TEST_NUM: $TEST_PROGRAM $t -->\tFAIL" >> tests_result.log
			echo -e "LEAK TEST $LEAK_TEST_NUM: $TEST_PROGRAM $t -->\tFAIL"
			echo -e "______________________________________________________________________________________________________________________________________________\n" >> leak_test.log
            exit_on_error
        fi
    fi
}

# Логика тестирования
function testing() {
    t=$@
    command="$TEST_PROGRAM $t"
	if timeout 10s $command > s21_grep.log 2>&1; then
		$SYS_GREP $t > sys_grep.log
	else
        echo "Тест $TEST_NUM завершен с ошибкой - программа s21_grep попала в бесконечный цикл" >> tests_result.log
        exit_on_error
    fi

	DIFF_RES="$(diff -s s21_grep.log sys_grep.log)"
    if [ "$DIFF_RES" == "Files s21_grep.log and sys_grep.log are identical" ]
    then
      (( SUCCESS++ ))
      echo -e "TEST $TEST_NUM: $command ------>\tOK" >> tests_result.log
    else
      (( FAIL++ ))
      echo -e "TEST $TEST_NUM: $command ------>\tFAIL" >> tests_result.log
      TEST_FAILED=true
	if [ $DONT_STOP_FLAG -eq 1 ]; 
		then 
			exit_on_error
	fi
    fi
	((TEST_NUM++))
	if [ "$DIFF_RES" != "Files s21_grep.log and sys_grep.log are identical" ]; then
		break
	fi
	if [ $LEAK_TEST_FLAG -eq 1 ]; 
		then 
			memory_leak_testing "$t"
	fi
}

function run_small_tests(){
	for test_file in "${multy_testing[@]}"
	do
		for option1 in "${options[@]}"
		do
			testing "-$option1 $test_file"
		done
	done
}

function run_single_tests(){
	for test_file in "${multy_testing[@]}"
	do
		for option1 in "${options[@]}"
		do
			for option2 in "${options[@]}"
			do
				testing "-$option1 -$option2 $test_file"
			done
		done
	done

	for option1 in "${options[@]}"
	do
		for option2 in "${options[@]}"
		do
			for test_file in "${multy_testing[@]}"
			do
			testing "-$option1 -$option2 $test_file"
			done
		done
	done

}

# Удалить логи, если тесты успешны
function delete_logs() {
	if [ $FAIL -eq 0 ]; then
		rm s21_grep.log sys_grep.log
		echo -e "\nAll tests done! sys_grep.log & s21_grep.log deleted."
		echo -e "All tests done! sys_grep.log & s21_grep.log deleted." >> tests_result.log
		echo -e "\033[32mTESTING COMPLETED SUCCESFULLY!!-------------------->\t" 
		echo -e "TESTING COMPLETED SUCCESFULLY!!------------>\t" >> tests_result.log
		echo -e "LEAK TEST SUCCESFULLY : $LEAK_TEST_NUM" >> tests_result.log
		echo -e "LEAK TEST SUCCESFULLY : $LEAK_TEST_NUM"
		echo -e "\033[32mSUCCESS: $SUCCESS\033[0m" 
		echo -e "\033[31mFAIL: $FAIL\033[0m"
		echo -e "SUCCESS: $SUCCESS" >> tests_result.log
		echo -e "FAIL: $FAIL" >> tests_result.log
	fi
}

# Выход при нахождении ошибки
function exit_on_error() {
	kill $animation_pid
	wait $animation_pid 2>/dev/null
	echo -e "\n\033[31mTESTING COMPLETED WITH ERRORS!!---------------------->\t\033[0m" 
	echo -e "LEAK TEST SUCCESFULLY : $LEAK_TEST_NUM" >> tests_result.log
	echo -e "LEAK TEST SUCCESFULLY : $LEAK_TEST_NUM"
	echo -e "\033[32mSUCCESS: $SUCCESS\033[0m" 
	echo -e "\033[31mFAIL: $FAIL\033[0m"
	echo -e "SUCCESS: $SUCCESS" >> tests_result.log
	echo -e "FAIL: $FAIL" >> tests_result.log
	echo -e "\033[0mFor details, see tests_result.log, grep.log, and s21_grep.log\033[0m"
    exit 1
}

# Для отображения анимации загрузки
function show_loading_animation() {
    local frame_index=0
    while true; do
        printf "${ANIMATION_FRAMES[frame_index]}"
        ((frame_index = (frame_index + 1) % ${#ANIMATION_FRAMES[@]}))
        sleep $ANIMATION_DELAY
    done
}


# main__________________________________________________________________________________________________________________________________

show_loading_animation &  # Запуск анимации в фоновом режиме
animation_pid=$!

echo -e "START TESTING s21_grep----------------------------->\t"

run_small_tests # первая партия тестов

kill $animation_pid
wait $animation_pid 2>/dev/null

# Удалить логи, если тесты успешны
delete_logs

# Проверка, запустить ли вторую партию тестов
read -p "Хочешь запустить вторую партию тестов для составных флагов? (y/n):" choice
if [[ "$choice" == "y" || "$choice" == "Y" ]]; then
	echo -e "Да у тебя железные яйца!"
	echo -e "START COMPOSITE FLAGS TESTING s21_grep------------->\t"

	show_loading_animation &  # Запуск анимации в фоновом режиме
	animation_pid=$!

	run_single_tests # Вторая партия тестов

	kill $animation_pid
	wait $animation_pid 2>/dev/null

	# Удалить логи, если тесты успешны
	delete_logs
else
	echo "Ты отказался от запуска второй партии тестов..."
fi

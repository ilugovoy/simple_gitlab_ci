#!/bin/bash

LEAK_TEST_FLAG=1 # установить 0, чтобы пропускать проверку на утечки
DONT_STOP_FLAG=1 # установить 0, чтобы продолжать работу даже при неудачных тестах

TEST_FILES_DIR="files_for_tests"
TEST_PROGRAM=".././s21_cat"
LOG_FILE="tests_result.log" 
MEMORY_LEAK_TEST_TOOL=""
LEAK_TEST_NUM=1
TEST_NUM=1
SYS_CAT=""
SUCCESS=0
FAIL=0

ANIMATION_FRAMES=("⠋" "⠙" "⠹" "⠸" "⠼" "⠴" "⠦" "⠧" "⠇" "⠏")
ANIMATION_DELAY=0.1

# functions_________________________________________________________________________________________

# Выход при нахождении ошибки
function exit_on_error() {
	kill $animation_pid
	wait $animation_pid 2>/dev/null
    echo -e "\033[32mTESTING COMPLETED SUCCESFULLY!!----------------------->\t"
	echo -e "LEAK TEST SUCCESFULLY : $LEAK_TEST_NUM" >> leak_test.log
	echo -e "LEAK TEST SUCCESFULLY : $LEAK_TEST_NUM"
	echo -e "\033[32mSUCCESS: $SUCCESS" 
	echo -e "\033[31mFAIL: $FAIL"
	echo -e "SUCCESS: $SUCCESS" >>$LOG_FILE
	echo -e "FAIL: $FAIL" >> $LOG_FILE
	echo -e "\033[0mFor details, see tests_result.log, cat.log, and s21_cat.log"
    exit 1
}

# Отображение анимации загрузки
function show_loading_animation() {
    local frame_index=0
    while true; do
        printf "${ANIMATION_FRAMES[frame_index]}"
        ((frame_index = (frame_index + 1) % ${#ANIMATION_FRAMES[@]}))
        sleep $ANIMATION_DELAY
    done
}

# Определение ОС
if [ "$(uname)" == "Darwin" ]; 
	then 
		MEMORY_LEAK_TEST_TOOL="leaks -atExit --" 
		SYS_CAT="gcat"
	else 
		MEMORY_LEAK_TEST_TOOL="valgrind --tool=memcheck --leak-check=yes"  
		SYS_CAT="cat"
fi

# Тест на утечки
function memory_leak_testing() {

    if [ $LEAK_TEST_NUM -lt 10 ] || [ $(($TEST_NUM % 2)) -eq 0 ]; 
    then
		local t="$1"
		echo -e "LEAK TEST $LEAK_TEST_NUM: $TEST_PROGRAM $t -->\tSTARTED" >> leak_test.log
		$MEMORY_LEAK_TEST_TOOL $TEST_PROGRAM $TEST_FILES_DIR/$t 2>&1 | grep -E "LEAK TEST|Command:|SUCCESS|ERROR SUMMARY|in use|total heap|allocated|no leaks are possible" >> leak_test.log
		local return_code=$?

        if [ $return_code -eq 0 ]; 
        then 
            echo -e "LEAK TEST $LEAK_TEST_NUM: $TEST_PROGRAM $t -->\tSUCCESS" >> leak_test.log
			echo -e "______________________________________________________________________________________________________________________________________________\n" >> leak_test.log
			((LEAK_TEST_NUM++))
        else 
            echo -e "LEAK TEST $LEAK_TEST_NUM: $TEST_PROGRAM $t -->\tFAIL" >> leak_test.log
			echo -e "LEAK TEST $LEAK_TEST_NUM: $TEST_PROGRAM $t -->\tFAIL"
			echo -e "______________________________________________________________________________________________________________________________________________\n" >> leak_test.log
            exit_on_error
        fi
    fi
}

# Удаление логов при удачном завершении тестов
function delete_logs() {
    rm s21_cat_log.log cat_log.log
    echo -e "\nAll tests done! cat.log & s21_cat.log deleted."
    echo -e "All tests done! cat.log & s21_cat.log deleted." >> $LOG_FILE
    echo -e "\033[32mTESTING COMPLETED SUCCESFULLY!!----------------------->\t" 
    echo -e "TESTING COMPLETED SUCCESFULLY!!----------------------->\t" >> $LOG_FILE
	echo -e "LEAK TEST SUCCESFULLY : $LEAK_TEST_NUM" >> leak_test.log
	echo -e "LEAK TEST SUCCESFULLY : $LEAK_TEST_NUM"
	echo -e "\033[32mSUCCESS: $SUCCESS\033[0m" 
	echo -e "\033[31mFAIL: $FAIL\033[0m"
	echo -e "SUCCESS: $SUCCESS" >> $LOG_FILE
	echo -e "FAIL: $FAIL" >> $LOG_FILE
}

# Для предотвращения зацикливания в случае зависания тестируемой программы
function infinite_loop_check() {
	if timeout 10s $TEST_PROGRAM $t3 > s21_cat_log.log 2>&1; then
		cat $t3 > cat_log.log
	else
		echo "Тест $TEST_NUM завершен с ошибкой - программа s21_cat $t3 $var1 $var2 $var3 попала в бесконечный цикл" >> $LOG_FILE
		exit_on_error
	fi
}

function multi_flag_testing() {
    for filename in $TEST_FILES_DIR/*.txt; 
    do
        for var3 in --number --number-nonblank --show-ends --show-tabs --squeeze-blank; 
        do
            var3="$var3"
            t3=$(echo $t2 VAR3 $filename | sed "s/VAR3/$var3/")
            infinite_loop_check
            RES="$(diff -s s21_cat_log.log cat_log.log)"
            if [ "$RES" == "Files s21_cat_log.log and cat_log.log are identical" ]; 
                     t3=$(basename "$filename")
            then
                ((SUCCESS++))
                echo -e "TEST $TEST_NUM: ./s21_cat $t3 $var1 $var2 $var3 ------>\tOK" >> $LOG_FILE
            else
                ((FAIL++))
                echo -e "TEST $TEST_NUM: ./s21_cat $t3 $var1 $var2 $var3 ---->\tFAIL" >> $LOG_FILE
                if [ $DONT_STOP_FLAG -eq 1 ]; 
                then 
                    exit_on_error
                fi
            fi
            ((TEST_NUM++))
        done
        if [ "$RES" != "Files s21_cat_log.log and cat_log.log are identical" ]; 
        then
            break
        fi
        if [ $LEAK_TEST_FLAG -eq 1 ]; 
            then 
                memory_leak_testing "$t3"
        fi
    done
}

# Первая партия тестов
function single_flag_testing() {
	for filename in $TEST_FILES_DIR/*.txt; 
	do
		for var3 in -b -e -n -s -t -v; 
# --number --number-nonblank --squeeze-blank --show-ends --show-tabs; 
		do
			var3="$var3"
			t3=$(echo $t2 VAR3 $filename | sed "s/VAR3/$var3/")
			infinite_loop_check
			RES="$(diff -s s21_cat_log.log cat_log.log)"
			if [ "$RES" == "Files s21_cat_log.log and cat_log.log are identical" ]; 
				 t3=$(basename "$filename")
			then
				((SUCCESS++))
				# echo -e "TEST $TEST_NUM: ./s21_cat $t3 ------>\tOK" >> $LOG_FILE
				echo -e "TEST $TEST_NUM: ./s21_cat $t3 $var1 $var2 $var3 ------>\tOK" >> $LOG_FILE
			else
				((FAIL++))
				# echo -e "TEST $TEST_NUM: ./s21_cat $t3 ------>\tFAIL" >> $LOG_FILE
				echo -e "TEST $TEST_NUM: ./s21_cat $t3 $var1 $var2 $var3 ---->\tFAIL" >> $LOG_FILE
				if [ $DONT_STOP_FLAG -eq 1 ]; 
				then 
						exit_on_error
				fi
			fi
			((TEST_NUM++))
		done
		if [ "$RES" != "Files s21_cat_log.log and cat_log.log are identical" ]; 
		then
			break
		fi
		if [ $LEAK_TEST_FLAG -eq 1 ]; 
        	then 
				memory_leak_testing "$t3"
		fi
	done
}



# main______________________________________________________________________________________________

echo -e "START SINGLE FLAG TESTING s21_cat------------------>\t"

show_loading_animation & animation_pid=$!

single_flag_testing "$PWD"
multi_flag_testing "$PWD"

kill $animation_pid
wait $animation_pid 2>/dev/null

# Удалить логи, если тесты успешны
if [ $FAIL -eq 0 ]; then 
	delete_logs
fi
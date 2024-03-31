#!/bin/bash

echo -e "START STYLE TESTING src/cat/files.c && files.h------>\t"

# добавить в тестирование автотест на утечки

# Функция для проверки стиля форматирования файла
check_style() {
    file=$1
    temp_file=$(mktemp)

    clang-format -style=Google "$file" > "$temp_file"

    diff_result=$(diff "$file" "$temp_file")

    if [[ ! -z "$diff_result" ]]; then
        echo -e "\033[032mФайл $file не соответствует стилю форматирования aka Google style\033[0m"
        
        read -p "Хотите привести файл в соответствие с стандартами стиля Google? (y/n): " answer
        
        if [[ "$answer" == "y" ]]; then
            clang-format -style=Google "$file" > "$file.tmp"
            mv "$file.tmp" "$file"
            echo -e "\033[032mФайл $file исправлен в соответствии со стилем форматирования Google\033[0m"
            
        elif [[ "$answer" == "n" ]]; then
            echo -e "\033[031mФайл $file оставлен в исходном состоянии. \nWARNING!!! ACHTUNG!!! ALARM!!! Если оставить файл в таком виде, хуевертер зафейлит проект!\033[0m"
            
            read -p "Ты точно не хочешь привести файл в соответствие с стандартами стиля Google и зафакапить проект? (y/n): " answer
            
            if [[ "$answer" == "y" ]]; then
                clang-format -style=Google "$file" > "$file.tmp"
                mv "$file.tmp" "$file"
                echo -e "\033[032mФайл $file исправлен в соответствии со стилем форматирования Google\033[0m"
            fi
            
            if [[ "$answer" == "n" ]]; then
                echo -e "\033[031mФайл $file оставлен в исходном состоянии. Вижу, ты не ищешь лёгких путей\033[0m"
                exit 1
            fi
            
        fi
    else
        echo -e "\033[032mФайл $file соответствует стилю форматирования aka Google style\033[0m"
    fi

    rm "$temp_file"
}

files=$(find ../ -maxdepth 1 -type f \( -name "*.c" -o -name "*.h" \))

for file in $files; do
    check_style "$file"
done

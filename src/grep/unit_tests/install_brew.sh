#!/bin/bash

if [[ "$(uname)" == "Darwin" ]]; then
    # Это Mac OS, проверяем наличие coreutils
    if ! brew list coreutils &>/dev/null; then
        # coreutils не установлен, загружаем Homebrew и устанавливаем coreutils
        echo "Загрузка Homebrew и установка coreutils для Mac OS..."
        /bin/bash -c "$(curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh)"
        brew install coreutils grep
    else
        # coreutils уже установлен
        echo "coreutils уже установлен с помощью Homebrew."
    fi
else
    # Для других операционных систем можно добавить соответствующие инструкции
    echo "Другая операционная система, не требуется установка Homebrew."
fi

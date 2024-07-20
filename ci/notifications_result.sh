BOT_TOKEN="$ENV_TOKEN"
CHAT_WITH_USER_ID="332612674"

COMMIT_USER=$(git log -1 --pretty=format:'%an') # Получаем имя пользователя, совершившего коммит

MESSAGE="Проект *$CI_PROJECT_NAME* успешно прошёл сборку и тесты ✅
Разработчик: *$COMMIT_USER*
Ветка: *$CI_COMMIT_BRANCH*
Комментарий: *$CI_COMMIT_MESSAGE*
Дата: *$CI_COMMIT_TIMESTAMP*
Раннер: *$CI_RUNNER_DESCRIPTION*
Для перехода к стадии деплоя требуется подтверждение! Перейдите по ссылке для подтверждения *$CI_PROJECT_URL/pipelines*"

curl -s -X POST https://api.telegram.org/bot${BOT_TOKEN}/sendMessage -d chat_id=${CHAT_WITH_USER_ID} -d text="${MESSAGE}" -d parse_mode="Markdown"

# переменные начинающиеся с "CI_" - это глобальные переменные gitlab-runner, список тут https://docs.gitlab.com/ee/ci/variables/predefined_variables.html 

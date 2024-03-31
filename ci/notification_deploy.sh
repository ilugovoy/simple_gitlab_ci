BOT_TOKEN="6725006255:AAEw9MquYSCXxkflncVrUBTA9DJBUY07pJU"
CHAT_WITH_USER_ID="332612674"

if [ "$CI_JOB_STATUS" == "success" ]; then
	MESSAGE="Деплой проекта *$CI_PROJECT_NAME* ✅
	*Артефакты сохранены на Прод-ВМ*
	Автор деплоя: *fungusgr*
	Ветка: *$CI_COMMIT_BRANCH*
	Дата: *$CI_COMMIT_TIMESTAMP*
	Раннер: *$CI_RUNNER_DESCRIPTION*"
else
	MESSAGE="Попытка деплоя проекта *$CI_PROJECT_NAME* неудачна 🚫"
fi


curl -s -X POST https://api.telegram.org/bot${BOT_TOKEN}/sendMessage -d chat_id=${CHAT_WITH_USER_ID} -d text="${MESSAGE}" -d parse_mode="Markdown"

# переменные начинающиеся с "CI_" - это глобальные переменные gitlab-runner, список тут https://docs.gitlab.com/ee/ci/variables/predefined_variables.html 

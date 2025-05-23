# File Manager

Консольный файловый менеджер с расширенными возможностями на базе ncurses

## Особенности

- 🗂️ Двухпанельный интерфейс с вкладками
- 📁 Основные файловые операции:
  - Создание файлов/директорий
  - Переименование/удаление
  - Копирование/перемещение
  - Изменение прав доступа
- 🔍 Поиск по имени файла
- 📊 Отображение метаданных:
  - Размер файла
  - Права доступа
  - Владелец/группа
  - Время изменения
- 🎨 Поддержка цветов и темизации
- 📋 Буфер обмена для операций
- 🔄 Автообновление содержимого
- ⌨️ Горячие клавиши для всех операций

## Установка и запуск

### Требования
- Linux-система
- Библиотека ncurses
- Компилятор C (gcc/clang)

```bash
# Установка зависимостей (Debian/Ubuntu)
sudo apt install libncurses5-dev gcc make git
```
# Сборка и запуск 
```
git clone https://github.com/ksenydmitri/fileManager.git
cd file_manager
make all
./build/bin/file_manager
make clean      # Очистка билдов
make debug      # Сборка с отладочной информацией
make install    # Установка в систему (требует прав)
```

##Использование
#Основные клавиши
#Комбинация	Действие
↑/↓	Навигация по файлам
←/→	Переключение вкладок
Enter	Открыть файл/директорию
F1	Создать файл
F2	Изменить владельца
F3	Удалить
F4  Просмотр метаданных файла
F5	Копировать
F6	Переместить
F7	Изменить права
F8  Переименовать
F9	Выход из приложения
S   Статистика о файловой системе
P   Поиск файла
CTRL + I или TAB Поиск аномалий(повторяющиеся файлы, давно неипользуемые файлы и поиск битых симлинков)

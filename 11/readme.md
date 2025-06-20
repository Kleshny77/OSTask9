# Операционные системы. Задание 11

**ФИО:** Самсонов Артём Арменович
**Группа:** БПИ238

## Задание

Разработать программу на языке C, определяющую глубину рекурсии для символических связей (symlinks) при попытке открытия файла. Дополнительно (опционально на +2 балла) реализовать аналогичную программу с использованием Bash скрипта.

Глубина рекурсии символических связей - это максимальное количество последовательных символических ссылок, которые ядро может разыменовать при доступе к файлу, прежде чем вернуть ошибку `ELOOP` (Too many levels of symbolic links).

## Решение (на 10 баллов)

Реализованы две версии программы для определения максимальной глубины рекурсии символических связей: одна на языке C и одна в виде Bash скрипта. Обе версии создают цепочку символических ссылок во временной директории до тех пор, пока попытка открыть (в случае C) или проследовать по (в случае Bash) следующей ссылке не завершится ошибкой `ELOOP`, что указывает на достижение лимита рекурсии. После определения глубины, все созданные временные файлы и директория автоматически удаляются.

### Версия на языке C (`symlink_recursion.c`)

Программа на C использует системные вызовы `symlink()` для создания символических связей и `open()` для попытки открытия файла, что и вызывает разыменование ссылок. Временная директория создается с помощью `mkdtemp()`.

### Версия на Bash (`find_symlink_recursion.sh`)

Bash скрипт также создает цепочку символических связей с помощью команды `ln -s`. Для определения глубины рекурсии используется команда `readlink -f`, которая пытается рекурсивно следовать по символическим ссылкам. Временная директория создается с помощью `mkdir` с уникальным именем.

## Запуск

Для запуска обеих версий программы:

1.  Откройте терминал и перейдите в директорию `11`:
    ```bash
    cd 11
    ```

2.  **Для версии на языке C (`symlink_recursion.c`):**
    *   Скомпилируйте программу:
        ```bash
        gcc symlink_recursion.c -o symlink_recursion
        ```
    *   Запустите скомпилированный файл:
        ```bash
        ./symlink_recursion
        ```

3.  **Для версии на Bash (`find_symlink_recursion.sh`):**
    *   Сделайте скрипт исполняемым:
        ```bash
        chmod +x find_symlink_recursion.sh
        ```
    *   Запустите скрипт:
        ```bash
        ./find_symlink_recursion.sh
        ```

Обе программы выведут на стандартный вывод найденную максимальную глубину рекурсии символических связей для операции открытия/следования по ссылке в системе.

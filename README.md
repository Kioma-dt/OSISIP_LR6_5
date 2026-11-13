# Лабораторная работа № 6: поиск по реестру Windows

Программа создаёт тестовые деревья в реестре, ищет подстроку в именах подразделов без учёта регистра и записывает результаты в `data/results.csv`.

Все команды ниже выполняются в **cmd.exe**, из папки проекта. Нужен MinGW-w64 с `g++` в `PATH`.

## Компиляция

```cmd
g++ -std=c++17 -O2 -Wall -Wextra -municode src/*.cpp -o build/reg.exe -ladvapi32
```

План содержит 18 деревьев с последовательными номерами:

- `Run_1`–`Run_3`: вероятность 50%, глубина поиска 2.
- `Run_4`–`Run_6`: вероятность 50%, глубина поиска 4.
- `Run_7`–`Run_9`: вероятность 50%, глубина поиска 6.
- `Run_10`–`Run_12`: вероятность 10%, глубина поиска 6.
- `Run_13`–`Run_15`: вероятность 50%, глубина поиска 6.
- `Run_16`–`Run_18`: вероятность 90%, глубина поиска 6.

Для серии с изменением вероятности глубина поиска зафиксирована равной 6. Таким образом, выполняются 18 запусков создания и затем 18 запусков поиска, по три повтора на каждый вариант.



## 1. Создание 18 деревьев


```cmd
for /L %r in (1,1,9) do @build\reg.exe --mode prepare --student_id 1 --run %r --target_pct 50 --quiet
for /L %r in (10,1,12) do @build\reg.exe --mode prepare --student_id 1 --run %r --target_pct 10 --quiet
for /L %r in (13,1,15) do @build\reg.exe --mode prepare --student_id 1 --run %r --target_pct 50 --quiet
for /L %r in (16,1,18) do @build\reg.exe --mode prepare --student_id 1 --run %r --target_pct 90 --quiet
```

Проверка 

``` cmd
reg query "HKCU\Software\OSiSP\Lab06\Student_1"
```

## 2. Эксперименты на созданных деревьях



### Изменение глубины обхода при вероятности 50%

```cmd
for /L %r in (1,1,3) do @build\reg.exe --mode search --student_id 1 --run %r --pattern target --depth 2 --target_pct 50 --quiet
for /L %r in (4,1,6) do @build\reg.exe --mode search --student_id 1 --run %r --pattern target --depth 4 --target_pct 50 --quiet
for /L %r in (7,1,9) do @build\reg.exe --mode search --student_id 1 --run %r --pattern target --depth 6 --target_pct 50 --quiet
```

### Изменение вероятности при глубине обхода 6

```cmd
for /L %r in (10,1,12) do @build\reg.exe --mode search --student_id 1 --run %r --pattern target --depth 6 --target_pct 10 --quiet
for /L %r in (13,1,15) do @build\reg.exe --mode search --student_id 1 --run %r --pattern target --depth 6 --target_pct 50 --quiet
for /L %r in (16,1,18) do @build\reg.exe --mode search --student_id 1 --run %r --pattern target --depth 6 --target_pct 90 --quiet
```


## 3. Очистка 18 созданных деревьев

Ввыполните:

```cmd
for /L %r in (1,1,18) do @reg delete "HKCU\Software\OSiSP\Lab06\Student_1\Run_%r" /f
```
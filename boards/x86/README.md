### Необходимые компиляторы:

### Этапы сборки проекта:
0. Получение исходников git модуля:  
```shell
git submodule update --init
```

1. Создание директории, в которой будет происходить сборка:  
```shell
mkdir x86-build
```

2. Добавление в переменную окружения `PATH` пути к кросс-компилятору:
```shell
export PATH=$PATH:"~/soft/i386-unknown-elf/bin/
```

3. Переход в сборочную директорию и создание в ней окружения сборки:  
```shell  
cd x86-build
clear
rm -rf ../x86-build/*
cmake .. -DBOARD_TYPE=x86 -DCMAKE_C_COMPILER=i386-unknown-elf-gcc -DUART_SIO=1
#or
rm -rf ../x86-build/*
cmake .. -DBOARD_TYPE=x86 -DCMAKE_C_COMPILER=i386-unknown-elf-gcc -DUART=0
```

4. Выполнение сборки  
```shell 
cmake --build . --clean-first -- -j8
```

5. Выполнение отладки  
```shell
cd x86-build
../boards/x86/run-debugger.sh
```

6. Запуск в qemu  
```shell 
../boards/x86/run-qemu.sh
```

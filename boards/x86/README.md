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
objcopy -I binary -O elf32-i386 ../boards/x86/microcode_update.bin microcode_update.out
rm -rf ../x86-build/*
cmake .. -DBOARD_TYPE=x86 -DCMAKE_C_COMPILER=i386-unknown-elf-gcc -DUART_SIO=1
#or
rm -rf ../build_x86/* ;
cmake .. -DBOARD_TYPE=x86 -DCMAKE_C_COMPILER=i386-unknown-elf-gcc -DMICROCODE_UPDATE=../../microcodes/baytrail_10.01.2018.mcb -DUART_TYPE=LOCAL -DUART_NUM=0;
```
`UART_TYPE` can be: `LOCAL`, `ITE8613`, `AST2500`  
`UART_NUM` can be form `0` to `3`  

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

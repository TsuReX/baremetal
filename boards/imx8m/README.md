### Необходимые компиляторы:
[GNU Toolchain for the A-profile  Architecture](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)  
- Для сборки `bare-metal` кода для процессоров с архитектурой `ARMv7A` необходимо компиялтор `AArch32` bare-metal target `arm-none-eabi`  
> Содержимое арххива распаковать в директорию `~/soft/armv7a-gcc`  
- Для сборки bare-metal кода для процессоров с архитектурой ARMv8A необходимо компиялтор `AArch64` ELF bare-metal target `aarch64-none-elf`  
> Содержимое арххива распаковать в директорию `~/soft/armv8a-gcc`  

### Этапы сборки проекта:
0. Получение исходников git модуля:  
```shell
git submodule update --init
```

1. Создание директории, в которой будет происходить сборка:  
```shell
mkdir build_board_type
```

2. Добавление в переменную окружения `PATH` пути к кросс-компилятору:
```shell
export PATH=$PATH:"~/soft/armv8a-gcc/bin"
```

3. Переход в сборочную директорию и создание в ней окружения сборки:  
```shell  
cd build
clear
rm -rf ../build/*
cmake .. -DBOARD_TYPE=imx8m
```

4. Выполнение сборки  
```shell 
cmake --build . --clean-first -- -j8
```

5. Выполнение отладки  
```shell
cd build_board_type
../boards/imx8m/run-debugger.sh
```

6. Запуск в qemu  
```shell 
../boards/imx8m/run-qemu.sh
```

### Необходимые компиляторы:

### Этапы сборки проекта:
0. Получение необходимых git-модулей и бинарных файлов:  
```shell
git submodule update --init
git clone https://github.com/intel/FSP.git
cp FSP/EagleStreamFspBinPkg/Fsp.fd
```

1. Создание директории, в которой будет происходить сборка:  
```shell
mkdir x86-build
```

2. Добавление в переменную окружения `PATH` пути к кросс-компилятору:
```shell
export PATH=$PATH:"/soft/bochs/bin/":"/soft/i386-unknown-elf/bin/"
```

3. Переход в сборочную директорию и создание в ней окружения сборки:  
```shell  
cd build_x86
rm -rf ../build_x86/*
cmake .. -DBOARD_TYPE=x86 -DCMAKE_C_COMPILER=i386-unknown-elf-gcc -DUART_TYPE=LOCAL -DUART_NUM=0
```
`UART_TYPE` can be: `LOCAL`, `ITE8613`, `AST2500`  
`UART_NUM` can be form `0` to `3`  

4. Выполнение сборки  
```shell 
cmake --build . --clean-first -- -j8
```
5. Подготовка образ
```shell
python ../boards/x86/SplitFspBin.py rebase -f ../fsp_eaglestream.fd -c s m t -b 0x00400000 0x0010000 0x00010000 -o .. -n fsp_eaglestream_rebased.fd

python ../boards/x86/SplitFspBin.py info -f ../fsp_eaglestream_rebased.fd

python ../boards/x86/SplitFspBin.py split -f ../fsp_eaglestream_rebased.fd -o .. -n fsp_eaglestream_rebased.fd

../boards/x86/setup-binary-table.sh --bios ./x86.bin --fspt-addr 0x00010000 --mcupd-addr 0x00020000

../boards/x86/create-bios-section.sh --bios ./x86.bin --fspt ../fsp_eaglestream_rebased_T.fd  --mcupd ../../microcodes/eaglestream_05.15.2023.mcb --image bios-section.bin
```
6. Запуск в qemu  
```shell 
../boards/x86/run-qemu.sh --bios ./bios-section.bin --fspt ../fsp_eaglestream_rebased_T.fd --fspt-addr 0x00010000 --mcupd ../../microcodes/eaglestream_05.09.2023.mcb --mcupd-addr 0x00020000
```

5. Выполнение отладки  
```shell
cd x86-build
../boards/x86/run-debugger.sh
```

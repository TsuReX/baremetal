Необходимые компиляторы:
GNU Arm Embedded Toolchain https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
Для сборки bare-metal кода для процессоров с архитектурой ARMvXM необходимо компиялтор gcc-arm-none-eabi
- Содержимое архива распаковать в директорию ~/soft/armvxm-gcc

GNU Toolchain for the A-profile Architecture https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
Для сборки bare-metal кода для процессоров с архитектурой ARMv7A необходимо компиялтор AArch32 bare-metal target (arm-none-eabi)
- Содержимое арххива распаковать в директорию ~/soft/armv7a-gcc
Для сборки bare-metal кода для процессоров с архитектурой ARMv8A необходимо компиялтор AArch64 ELF bare-metal target (aarch64-none-elf)
- Содержимое арххива распаковать в директорию ~/soft/armv8a-gcc

Система сборки:

Система сборки основана на cmake и состоит из CMakeLists.txt верхнего уровня, который расположен в корневой директории проекта и опциональных .cmake скриптов, которые располагаются в поддиректории boards/board_name и служат для упрощения процесса конфигурирования, они определяют дополнительные цели (flash - для выполнения прошивки), определяют переменные, которые влияют на сам процесс конфигуриования и сборки.

Этапы сборки проекта:
0. Получение исходников git модуля:
	git submodule update --init

1. Создание директории, в которой будет происходить сборка:
	mkdir build_board_type

2. Добавление в переменную окружения PATH пути к кросс-компилятору:
	для Cortex-M
	export PATH=$PATH:"~/soft/armvxm-gcc/bin"
	для Cortex-A ARMv7A
	export PATH=$PATH:"~/soft/armv7a-gcc/bin"
	для Cortex-A ARMv8A
	export PATH=$PATH:"~/soft/armv8a-gcc/bin"

3. Переход в сборочную директорию и создание в ней окружения сборки:
	cd build
	clear ;\
	rm -rf ../build/* ;\
	cmake .. -DBOARD_TYPE=nucleo_stm32f303k8t6
	
4. Выполнение сборки
	cmake --build . --clean-first -- -j8

5. Выполнение прошивки
	cmake --build .  -- -j8 --target flash

6. Выполнение отладки
	cd build_board_type
	openocd -f ../boards/stellaris_lm4f120h5qr/oocd_ti-icdi.cfg -c "init; reset halt;" > /dev/null &  arm-none-eabi-gdb ; echo shutdown | nc 127.0.0.1 4444
TODO: Автоматизировать отладку через использование цели сборки

7. Запуск в qemu
ARMv8:
qemu-system-aarch64 -machine type=virt,virtualization=on,secure=on -cpu cortex-a53 -smp cpus=2 -s -S -bios ./imx8m.bin

aarch64-none-elf-gdb -ex "target remote localhost:1234" -ex "layout asm" -ex "symbol-file imx8m.elf" -ex "focus cmd" -ex "info  thread"

* virtualization=on добавляет уровень EL2
* secure=on добавляет уровень EL3

Переменные, используемые на этапе (3) конфигурирования:

CMAKE_C_COMPILER_PREFIX - путь и префикс имени набора утилит сборки, например arm-none-eabi- или aarch64-none-elf-;

BOARD_TYPE - используемая платформа - имя директории в директории boards, возможные варианты типов платформ: 
china_stm32f407vet6
nucleo_stm32f303k8t6
stellaris_lm4f120h5qr
checkboard_lpc54618j512
psu_hs_stm32f030f4p6
psu_ls_stm32f030c6t6
cpe_stm32f030r8t6
devusb_stm32f103c8t6
hostusb_mdr32f9
hostusb_stm32f103c8t6
bluepill_stm32f103c8t6
baikalbmc_stm32f103ret6
baikalbmc_stm32l412kbu6
dbs
imx8m
pulsox_stm32l4r9aii6u

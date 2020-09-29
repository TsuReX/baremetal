Система сборки:

Система сборки основана на cmake и состоит из CMakeLists.txt верхнего уровня, который расположен в корневой директории проекта и опциональных .cmake скриптов, которые располагаются в поддиректории boards/board_name и служат для упрощения процесса конфигурирования, они определяют дополнительные цели (flash - для выполнения прошивки), определяют переменные, которые влияют на сам процесс конфигуриования и сборки.

Этапы сборки проекта:
0. Получение исходников git модуля:
	git submodule update --init

1. Создание директории, в которой будет происходить сборка:
	mkdir build_board_type

2. Добавление в переменную окружения PATH пути к кросс-компилятору:
	примерный вариант 1
	export PATH=$PATH:"/opt/gcc-arm-none-eabi/bin"
	примерный вариант 2
	export PATH=$PATH:"/home/yurchenko/soft/gcc-arm-none-eabi/bin"

3. Переход в сборочную директорию и создание в ней окружения сборки:
	cd build
	clear ;\
	rm -rf ../build/* ;\
	cmake .. -DCMAKE_C_COMPILER=arm-none-eabi-gcc -DBOARD_TYPE=nucleo_stm32f303k8t6
	
4. Выполнение сборки
	cmake --build . --clean-first -- -j8

5. Выполнение прошивки
	cmake --build .  -- -j8 --target flash

6. Выполнение отладки
	cd build_board_type
	openocd -f ../boards/stellaris_lm4f120h5qr/oocd_ti-icdi.cfg -c "init; reset halt;" > /dev/null &  arm-none-eabi-gdb ; echo shutdown | nc 127.0.0.1 4444
TODO: Автоматизировать отладку через использование цели сборки

Переменные, используемые на этапе (3) конфигурирования:

CMAKE_C_COMPILER - путь и имя компилятора или просто имя, если путь к нему прописан в PATH;

BOARD_TYPE - используемая платформа - имя директории в директории boards, возможные варианты типов платформ: 
china_stm32f103c8t6
china_stm32f407vet6
nucleo_stm32f303k8t6
stellaris_lm4f120h5qr
checkboard_lpc54618j512
depo_mdr32f9
psu_stm32f030r8t6
cpe_stm32f030r8t6

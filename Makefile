CPP_FLAGS = -m32 -mno-sse -mno-80387 -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-exceptions -fno-rtti -fno-leading-underscore

LD_FLAGS = -T linker.ld -melf_i386

all: kernel.bin

kernel.bin: build/loader.o build/kernel.o build/screen.o build/interrupts.o build/keyboard.o build/mouse.o build/memory.o build/scheduler.o
	ld $(LD_FLAGS) -o kernel.bin \
		build/loader.o build/kernel.o build/screen.o \
		build/interrupts.o build/keyboard.o build/mouse.o \
		build/memory.o build/scheduler.o

build/loader.o: src/bootloader/loader.s
	nasm -f elf32 src/bootloader/loader.s -o build/loader.o

build/kernel.o: src/kernel/kernel.cpp
	g++ $(CPP_FLAGS) -o build/kernel.o -c src/kernel/kernel.cpp

build/screen.o: src/kernel/screen.cpp
	g++ $(CPP_FLAGS) -o build/screen.o -c src/kernel/screen.cpp

build/interrupts.o: src/kernel/interrupts.cpp
	g++ $(CPP_FLAGS) -o build/interrupts.o -c src/kernel/interrupts.cpp

build/keyboard.o: src/kernel/keyboard.cpp
	g++ $(CPP_FLAGS) -o build/keyboard.o -c src/kernel/keyboard.cpp

build/mouse.o: src/kernel/mouse.cpp
	g++ $(CPP_FLAGS) -o build/mouse.o -c src/kernel/mouse.cpp

build/memory.o: src/memory/memory.cpp
	g++ $(CPP_FLAGS) -o build/memory.o -c src/memory/memory.cpp

build/scheduler.o: src/scheduler/scheduler.cpp
	g++ $(CPP_FLAGS) -o build/scheduler.o -c src/scheduler/scheduler.cpp

run: kernel.bin
	qemu-system-i386 -kernel kernel.bin

clean:
	rm -f build/*.o kernel.bin

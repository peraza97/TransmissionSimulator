DEVICE     = atmega1284
CLOCK      = 8000000
PROGRAMMER = atmelice_isp
PORT	   = usb
BAUD       = 19200
FILENAME   = main
COMPILE    = avr-gcc -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all: clean build upload

build:
	$(COMPILE) -o $(FILENAME).elf $(FILENAME).c ./../headers/io.c joystick.c adc.c
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex
	avr-size --format=avr --mcu=$(DEVICE) $(FILENAME).elf

upload:
	avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(FILENAME).hex:i 

clean:
	##rm main.o
	rm main.elf
	rm main.hex

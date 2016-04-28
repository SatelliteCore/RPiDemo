/* 
   Flash ACT LED on Raspberry Pi 3

   Converted to C from asm example on forum:

     https://www.raspberrypi.org/forums/viewtopic.php?f=72&t=139684

  How To Run:

  $ arm-none-eabi-gcc -march=armv7-a -mtune=cortex-a7 -nostartfiles rpi3blinkc.c -o rpi3blinkc.elf
  $ arm-none-eabi-objcopy rpi3blinkc.elf -O binary rpi3blinkc.img

  Then copy rpi3blinkc.img to your SD card boot partition and edit config.txt
  to have line:

    kernel=rpi3blinkc.img
*/

void blinker(void);

void _start(void) __attribute__((naked,section(".init")));
void _start(void)
{
  __asm("mov sp, #0x8000000");
  blinker();
}

// align 16?
unsigned int request[7] __attribute__((aligned(16))) = {
  0x1c, 0, 0x00040010, 4, 0, 0, 0
};

volatile unsigned int *pLed;
volatile unsigned int *pBal = (unsigned int *)0x3f00b880;
volatile unsigned int *pHtr = (unsigned int *)0x3f003004;

void ledInit(void)
{
  while (pBal[6] & 0x80000000) {}
  pBal[8] = (unsigned int)(request + 2);

  while (pBal[6] & 0x40000000) {}
  pLed = (unsigned int *)(request[5] & ~0xc0000000);
}

void ledOn(void)
{
  *pLed += 0x00010000;
}

void ledOff(void)
{
  *pLed += 0x00000001;
}

void delay(unsigned int ms)
{
  unsigned int t0 = *pHtr;
  while (*pHtr - t0 < ms*1000) {}
}

void blinker(void)
{
  ledInit();

  for (;;) {
    ledOn();
    delay(500);
    ledOff();
    delay(500);
  }
}

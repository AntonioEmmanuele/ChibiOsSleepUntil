/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio
    Modifications copyright (C) 2021 <Antonio Emmanuele> 
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#define SLEEP_DEBUG 1
static SerialConfig my_serial;
static BaseSequentialStream* bsp;
void Sleep_Until(systime_t*,systime_t);
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chRegSetThreadName("blinker");
  systime_t Time=chVTGetSystemTime();
  systime_t Previous=Time;

  while (true) {

    Time=chTimeAddX(Time,TIME_MS2I(200));
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    Sleep_Until(&Previous,Time);//Time will be the future instance


    Time=chTimeAddX(Time,TIME_MS2I(200));
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(200); //Simulate a delay
    Sleep_Until(&Previous,Time);//Time will be the future instance

  }
}
int main(void) {
  halInit();
  chSysInit();
  my_serial.speed=115200;
  my_serial.cr1=0;
  my_serial.cr2=USART_CR2_STOP1_BITS;
  my_serial.cr3=0;
  sdStart(&SD2, &my_serial);
  bsp=(BaseSequentialStream*)&SD2;

  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  while (true) {
    chThdSleepMilliseconds(500);
  }
}
void Sleep_Until(systime_t* Previous, systime_t  Future){
  uint8_t must_sleep;    //Variable that tells us
  systime_t now=chVTGetSystemTimeX();
  /* We are reasoning with the logic of clocks.
   * If Previous > now means that we are in a new counter cycle(The SysTickCounter has reached 0)
   * In this case (like in the other one) we must sleep if future is above now, future is above now.
   * However there is the possibility that execution time has exceeded so that future is in the previous
   * counter cycle and now in another clock cycle.
   * So we must delay if Previous > Future(we are in a new clock cycle, and if now < Future(in this new new clock cycle we
   * have not exceeded).
   * In the second case(the counter has not reached 0) we must sleep if Future is greater than now (this can not be true
   * if future is in another clock cycle) OR Previous is greater that future(now > Future but future is in a new clock cycle
   * so we haven't exceeded).
   *
   */
  chSysLock();
  if(now<*Previous)

    must_sleep=( Future < *Previous && now <Future );

  else  //Not a new clock cycle

    must_sleep=(Future > now || *Previous > Future);

#if SLEEP_DEBUG
  if(must_sleep==1)
    chprintf(bsp,"Task must sleep \r\n");
  else
    chprintf(bsp,"Task must continue \r\n");
#endif

  if(must_sleep)
    chThdSleepS(chTimeDiffX(now,Future));
  chSysUnlock();
  *Previous=Future;
}

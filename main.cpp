#include "ch.h"
#include "hal.h"

static adcsample_t adcBuffer[1] = {0};


static ADCConversionGroup convGroup =
{
    false,
    1,
    nullptr,
    nullptr,
    ADC_CFGR1_RES_12BIT,                  // CFGR1
    0, // CFGR2
    0, // TR1 through TR3
    0,
    0, 
    0, //awd2cr
    0, //awd2cr
    ADC_SMPR_SMP_160P5,      // SMPR
    ADC_CHSELR_CHSEL5
};

// blinky thread
static THD_WORKING_AREA(waThread1, 128);
static void Thread1(void* /*arg*/)
{
    while (true)
    {
        palTogglePad(GPIOA, 1);
        chThdSleepMilliseconds(3);
     }
}

// ADC thread
static THD_WORKING_AREA(waThread2, 128);
static void Thread2(void* /*arg*/)
{
    while (true)
    {
        palSetPad(GPIOA, 0);
        adcConvert(&ADCD1, &convGroup, adcBuffer, 1);
        palClearPad(GPIOA, 0);
    }
}

extern "C" void ctxSwitchHook() {
    palTogglePad(GPIOA, 4);
}

int main(void)
{
    halInit();
    chSysInit();

    // test pins
    palSetPadMode(GPIOA, 0, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 1, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);

    adcStart(&ADCD1, nullptr);

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
    chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO + 5, Thread2, NULL);

    while (true)
    {
        chThdSleepMilliseconds(100);
    }
}

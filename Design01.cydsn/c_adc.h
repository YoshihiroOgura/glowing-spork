#ifndef MRBC_UART_PSOC_H_
#define MRBC_UART_PSOC_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

#define adc_init(ah, NAME)                           \
  do {                                                \
    adc_init_m(ah,                                   \
                NAME ## _Start,                       \
                NAME ## _Stop,                        \
                NAME ## _GetResult);                 \
  } while( 0 )

typedef struct ADC_HANDLE {
  void (*Start)(void);
  void (*Stop)(void);
  int (*Get)(void);
} ADC_HANDLE;

void adc_init_m(ADC_HANDLE *ah,
                 void         *Start,
                 void         *Stop,
                 void         *Get)
{
    ah->Start         = Start;
    ah->Stop          = Stop;
    ah->Get          = Get;
}


void mrbc_init_class_adc(struct VM *vm);


#ifdef __cplusplus
}
#endif
#endif
#ifndef MRBC_DIGITAL_PSOC_H_
#define MRBC_DIGITAL_PSOC_H_

#include "value.h"
    
#ifdef __cplusplus
extern "C" {
#endif

#define digital_init(dh, NAME)                           \
    digital_init_m(dh,                                   \
                NAME ## _Write,                       \
                NAME ## _Read);                 

typedef struct DIGITAL_HANDLE {
  void (*Write)(uint8_t);
  int (*Read)(void);
} DIGITAL_HANDLE;

void digital_init_m(DIGITAL_HANDLE *dh,
                 void         *Write,
                 void         *Read);


void mrbc_init_class_digital(struct VM *vm);


#ifdef __cplusplus
}
#endif
#endif
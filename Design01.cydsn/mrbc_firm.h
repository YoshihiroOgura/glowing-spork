/*! @file
  @brief
  mruby/c firmware for mruby/c devkit 01

  <pre>
  Copyright (C) 2018 Kyushu Institute of Technology.
  Copyright (C) 2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/


#ifndef MRBC_FIRM_PSOC5LP_H_
#define MRBC_FIRM_PSOC5LP_H_

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define MRBC_MAX_BYTECODES 4	// multiple bytecodes maximum four programs
#define MRBC_SIZE_IREP_STRAGE (64*1024)	// size of flash rom.


extern const uint8_t CYCODE mruby_bytecode[];


void usbuart_clear(void);
int usbuart_can_read_line(void);
int usbuart_size(void);
char *usbuart_cstr(void);
void usbuart_put_string(const char *s);
int usbuart_read(void *buf, int nbyte);
void usbuart_write(const char *buf, int nbyte);
int usbuart_event(void);
void mode_monitor(void);


#ifdef __cplusplus
}
#endif
#endif

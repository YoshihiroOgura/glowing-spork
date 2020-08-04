#include <project.h>
#include "mrubyc.h"
#include <math.h>

#include "c_uart.h"
#include "c_i2c.h"
#include "c_adc.h"
#include "c_digital.h"
#include "c_math.h"
#include "c_standard_io.h"
#include <stdio.h>
#include "mrbc_firm.h"
#define MEMORY_SIZE (1025*30)
static uint8_t memory_pool[MEMORY_SIZE];
static volatile int sleep_int_counter_;
static int sleep_wakeup_timing_;


const uint8_t CYCODE mruby_bytecode[MRBC_SIZE_IREP_STRAGE] = {
  0xff,0xff, 0,0, 0,0, 0,0,
0x52,0x49,0x54,0x45,0x30,0x30,0x30,0x34,0xa8,0xd3,0x00,0x00,0x00,0x99,0x4d,0x41,
0x54,0x5a,0x30,0x30,0x30,0x30,0x49,0x52,0x45,0x50,0x00,0x00,0x00,0x7b,0x30,0x30,
0x30,0x30,0x00,0x00,0x00,0x73,0x00,0x01,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x11,
0x00,0x40,0x06,0x17,0x00,0x80,0x00,0x06,0x01,0x40,0x00,0x03,0x00,0x80,0x00,0xa0,
0x00,0x80,0x00,0x06,0x01,0x40,0x00,0x03,0x00,0x80,0x40,0xa0,0x00,0x80,0x00,0x06,
0x01,0x3f,0xff,0x83,0x00,0x80,0x00,0xa0,0x00,0x80,0x00,0x06,0x01,0x40,0x00,0x03,
0x00,0x80,0x40,0xa0,0x00,0x80,0x00,0x07,0x00,0xbf,0xf9,0x18,0x00,0x80,0x00,0x05,
0x00,0x00,0x00,0x4a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x0a,0x6c,0x65,
0x64,0x73,0x5f,0x77,0x72,0x69,0x74,0x65,0x00,0x00,0x05,0x73,0x6c,0x65,0x65,0x70,
0x00,0x45,0x4e,0x44,0x00,0x00,0x00,0x00,0x08,
};


CY_ISR(isr_Tick)
{
  mrbc_tick();
}

CY_ISR( isr_SleepTimer )
{
  SleepTimer_GetStatus();
  sleep_int_counter_++;
}

CY_ISR( isr_D1 )
{
  D1_ClearInterrupt();
}

static void p2_new(mrb_vm *vm, mrb_value v[], int argc)
{
    if(GET_INT_ARG(1) == 1){
        P2_DR = 0xffu;
    }else{
        P2_DR = 0x00u;
    }
}

//================================================================
 /*! P2 IO
*/
static void P_Write(mrb_vm *vm, mrb_value *v, int argc)
{
    uint8 mask = ((uint8)pow(2,(uint8)GET_INT_ARG(1)));
    uint8 staticBits = ((*(reg8 *) P2__DR) & (uint8)(~mask));
    P2_DR = staticBits | ((uint8)(GET_INT_ARG(2) << GET_INT_ARG(1))& mask);
}

static void P_byte_Write(mrb_vm *vm, mrb_value *v, int argc)
{
    uint8 staticBits = (P2_DR & (uint8)(~P2_MASK));
    P2_DR = staticBits | ((uint8)(GET_INT_ARG(1) << P2_SHIFT) & P2_MASK);
}

static void P_nibble_Write(mrb_vm *vm, mrb_value *v, int argc)
{
    uint8 bit = (GET_INT_ARG(2) << (4u*GET_INT_ARG(1)));
    uint8 mask = 15u << (4u*GET_INT_ARG(1));
    uint8 staticBits = (P2_DR & (uint8)(~mask));
    P2_DR = staticBits | ((uint8)(bit << P2_SHIFT) & mask);
}

static void P_read(mrb_vm *vm, mrb_value *v, int argc)
{
    uint8 mask = ((uint8)pow(2,(uint8)GET_INT_ARG(1)));
    SET_INT_RETURN((P2_PS & mask) >> GET_INT_ARG(1));
}

static void P_byte_read(mrb_vm *vm, mrb_value *v, int argc)
{
    SET_INT_RETURN(P2_PS);
}

//=========================================================================
/*! sleep Modes
*/
static void c_hibernate(mrb_vm *vm, mrb_value *v, int argc)
{
  I2C_1_Sleep();
  UART_1_Sleep();
  USBUART_Stop();
  isr_SleepTimer_Disable();
  CyPmSaveClocks();
  CyPmHibernate();
  CyPmRestoreClocks();
  UART_1_Wakeup();
  I2C_1_Wakeup();
  USBUART_Start(0, USBUART_3V_OPERATION);
  isr_SleepTimer_Enable();
}

static void c_sleep_until_nexttime(mrb_vm *vm, mrb_value v[], int argc)
{
  hal_flush(1);

  UART_1_Sleep();
  I2C_1_Sleep();
  USBUART_Stop();
  ADC_1_Sleep();
  CyPmSaveClocks();

  while( sleep_int_counter_ < sleep_wakeup_timing_ ) {
    CyPmSleep( PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW|PM_SLEEP_SRC_PICU );
    // CyPmHibernate();
  }
  sleep_int_counter_ = 0;

  CyPmRestoreClocks();
  USBUART_Start(0, USBUART_3V_OPERATION);
  UART_1_Wakeup();
  I2C_1_Wakeup();
  ADC_1_Wakeup();
}
static void c_sleep_set_interval(mrb_vm *vm, mrb_value v[], int argc)
{
  static const int MAX_INTERVAL_INDEX = 10;

  SleepTimer_Stop();
  sleep_int_counter_ = 0;

  int n = GET_INT_ARG(1)+9;

  if( n <= MAX_INTERVAL_INDEX ) {
    sleep_wakeup_timing_ = 1;
  } else {
    sleep_wakeup_timing_ = n - MAX_INTERVAL_INDEX + 1;
    n = MAX_INTERVAL_INDEX;
  }

  SleepTimer_SetInterval( n );
  SleepTimer_Start();
}

//================================================================
/*! HAL
*/
int hal_write(int fd, const void *buf, int nbytes)
{
  if(USBUART_bGetConfiguration()){
    usbuart_write( buf, nbytes );
  }
  return nbytes;
//#else
//  CONS_PutArray( buf, nbytes );
//  return nbytes;
//#endif
}

int hal_flush(int fd)
{
  return 0;
}

int _write(int fd, const void *buf, int nbytes)
{
  if(USBUART_bGetConfiguration()){
    usbuart_write( buf, nbytes );
  }
  return nbytes;
//#else
//  CONS_PutArray( buf, nbytes );
//  return nbytes;
//#endif
}

//================================================================
/*! main
*/

int check_timeout(void)
{
  for( int i = 0; i < 256; i++ ) {
    LED1_Write(((i>>4) | (i>>1)) & 0x01 );	// Blink LED
    usbuart_event();
    if( usbuart_can_read_line() ) {
      return 0;
    }
    CyDelay( 10 );
  }
  return 1;
}

static void clock_write(mrb_vm *vm, mrb_value *v, int argc)
{
    Clock_1_SetDividerValue(GET_INT_ARG(1));
}

void mrubyc(void)
{
  mrbc_init_class_uart(0);
  mrbc_init_class_i2c(0);
  mrbc_init_class_adc(0);
  mrbc_init_class_standard_io(0);
  mrbc_init_class_digital(0);
  mrbc_init_class_math(0);
  mrb_class *p2;
  p2 = mrbc_define_class(0, "P2",	mrbc_class_object);
  mrbc_define_method(0, p2, "new",   p2_new);
  mrbc_define_method(0, p2, "write", P_Write);
  mrbc_define_method(0, p2, "byte_write", P_byte_Write);
  mrbc_define_method(0, p2, "read", P_read);
  mrbc_define_method(0, p2, "byte_read", P_byte_read);
  mrbc_define_method(0, p2, "nibble_write", P_nibble_Write);
  mrbc_define_method(0, mrbc_class_object, "cw",   clock_write);
  mrbc_define_method(0, mrbc_class_object, "hibernate",   c_hibernate);
  mrbc_define_method(0, mrbc_class_object, "sleep_set_interval", c_sleep_set_interval);
  mrbc_define_method(0, mrbc_class_object, "sleep_until_nexttime", c_sleep_until_nexttime);

  uint16_t *tbl_bytecode_size = (uint16_t *)mruby_bytecode;
  const uint8_t *bytecode = mruby_bytecode + sizeof(uint16_t) * MRBC_MAX_BYTECODES;

  int i;
  for( i = 0 ; i < MRBC_MAX_BYTECODES ; i++ ) {
    if( tbl_bytecode_size[i] == 0 ) break;
    mrbc_create_task( bytecode, 0 );
    bytecode += tbl_bytecode_size[i];
  }

  mrbc_run();
}

int main()
{
  CyGlobalIntEnable; /* Enable global interrupts. */
  isr_Tick_StartEx(isr_Tick);
  USBUART_Start(0, USBUART_3V_OPERATION);
  Em_EEPROM_Start();
  UART_1_Start();
  isr_SleepTimer_StartEx( isr_SleepTimer );
  SleepTimer_Start();
  //CONS_Start();
  I2C_1_Start();
  usbuart_clear();
  mrbc_init(memory_pool, MEMORY_SIZE);

  //console_print("\r\n\x1b(B\x1b)B\x1b[2JStart Program.\n");

  if( check_timeout() == 0 ) {
    LED1_Write( 1 );
    mode_monitor();
  }
  LED1_Write(0);
  Em_EEPROM_Stop();
  mrubyc();
}


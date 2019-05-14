/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */
#include <project.h>
#include "mrubyc.h"

static void led_new(mrb_vm *vm, mrb_value v[], int argc)
{
    uint8 status = 0xf0;
    if(GET_INT_ARG(1) == 0){
        status = 0x00u;
    }
    LED1_DR = status;
}

static void sw(mrb_vm *vm, mrb_value *v, int argc)
{
  int sw1 = SW1_Read();
  SET_INT_RETURN(sw1);
}

static void led_write(mrb_vm *vm, mrb_value *v, int argc)
{
  uint8 led_mask = ((uint8)pow(2,(GET_INT_ARG(1)-1)) * 0x10u);
  uint8 staticBits = (LED1_DR & (uint8)(~led_mask));
  SET_INT_RETURN(LED1_DR);
  LED1_DR = staticBits | ((uint8)(GET_INT_ARG(2) << (GET_INT_ARG(1)+3u)) & led_mask);
}

static void decimal_led_write(mrb_vm *vm, mrb_value *v, int argc)
{
  uint8 led_mask = ((uint8)pow(2,0x10u));
  uint8 staticBits = (LED1_DR & (uint8)(~led_mask));
  LED1_DR = staticBits | ((uint8)(GET_INT_ARG(1) << 4u) & led_mask);
}

void mrbc_init_class_standard_io(struct VM *vm)
{
  mrb_class *led;
  led = mrbc_define_class(0, "LED",	mrbc_class_object);
  mrbc_define_method(0, led, "new",  led_new);
  mrbc_define_method(0, led, "write", led_write);
  mrbc_define_method(0, led, "nibble_write", decimal_led_write);
  mrbc_define_method(0, mrbc_class_object, "SW", sw);
}
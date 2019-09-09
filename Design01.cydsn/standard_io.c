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

static void sw(mrb_vm *vm, mrb_value *v, int argc)
{
  int sw1 = SW1_Read();
  SET_INT_RETURN(sw1);
}

//=========================================
/*オンボードLED書き込み
　　　　引数：0~15(4bit)*/
static void decimal_led_write(mrb_vm *vm, mrb_value *v, int argc)
{
  uint8 led_mask = ((uint8)pow(2,0x10u));
  uint8 staticBits = (LED1_DR & (uint8)(~led_mask));
  LED1_DR = staticBits | ((uint8)(GET_INT_ARG(1) << 4u) & led_mask);
}

//=========================================
/*オンボードスイッチ読み込み
　　　　戻り値：0,1*/
void mrbc_init_class_standard_io(struct VM *vm)
{
  mrbc_define_method(0, mrbc_class_object, "leds_write", decimal_led_write);
  mrbc_define_method(0, mrbc_class_object, "sw", sw);
}
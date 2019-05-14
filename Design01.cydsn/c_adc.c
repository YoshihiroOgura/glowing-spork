#include <project.h>
#include "vm_config.h"
#include "class.h"
#include "value.h"
#include "static.h"
#include "c_adc.h"

ADC_HANDLE ah[2];

int ADC_1_GetResult()
{
    ADC_1_StartConvert();
    while(ADC_1_IsEndConversion(ADC_1_RETURN_STATUS) == 0);
    int val = ADC_1_GetResult16();
    return val;
}
int ADC_2_GetResult()
{
    ADC_2_StartConvert();
    while(ADC_2_IsEndConversion(ADC_2_RETURN_STATUS) == 0);
    int val = ADC_2_GetResult16();
    return val;
}

static void adc_new(mrb_vm *vm, mrb_value v[], int argc)
{
    *v = mrbc_instance_new(vm, v->cls, sizeof(ADC_HANDLE *));
    *((ADC_HANDLE **)v->instance->data) = &ah[GET_INT_ARG(1)];
}

static void adc_start(mrb_vm *vm, mrb_value *v, int argc)
{
    ADC_HANDLE *handle = *(ADC_HANDLE **)v->instance->data;
    handle->Start();
}

static void adc_get(mrb_vm *vm, mrb_value *v, int argc)
{
    ADC_HANDLE *handle = *(ADC_HANDLE **)v->instance->data;
    SET_INT_RETURN(handle->Get());
}

static void adc_stop(mrb_vm *vm, mrb_value *v, int argc)
{
    ADC_HANDLE *handle = *(ADC_HANDLE **)v->instance->data;
    handle->Stop();
}

//================================================================
/*! initialize
*/
void mrbc_init_class_adc(struct VM *vm)
{
  // start physical device
  adc_init(&ah[0], ADC_1);
  adc_init(&ah[1], ADC_2);

  // define class and methods.
  mrb_class *adc;
  adc = mrbc_define_class(0, "ADC",	mrbc_class_object);
  mrbc_define_method(0, adc, "new",   adc_new);
  mrbc_define_method(0, adc, "start",   adc_start);
  mrbc_define_method(0, adc, "read", adc_get);
  mrbc_define_method(0, adc, "stop", adc_stop);
}

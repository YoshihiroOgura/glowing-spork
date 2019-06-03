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
//================================================================
 /*! Digital IO
*/
#include <project.h>
#include "vm_config.h"
#include "class.h"
#include "value.h"
#include "static.h"
#include "c_digital.h"

DIGITAL_HANDLE dh[2];

void digital_init_m(DIGITAL_HANDLE *dh,
                 void         *Write,
                 void         *Read)
{
    dh->Write         = Write;
    dh->Read          = Read;
}

static void digital_new(mrb_vm *vm, mrb_value v[], int argc)
{
    *v = mrbc_instance_new(vm, v->cls, sizeof(DIGITAL_HANDLE *));
    *((DIGITAL_HANDLE **)v->instance->data) = &dh[GET_INT_ARG(1)-1];
}

static void digital_write(mrb_vm *vm, mrb_value *v, int argc)
{
    DIGITAL_HANDLE *handle = *(DIGITAL_HANDLE **)v->instance->data;
    handle->Write(GET_INT_ARG(1));
}

static void digital_read(mrb_vm *vm, mrb_value *v, int argc)
{
    DIGITAL_HANDLE *handle = *(DIGITAL_HANDLE **)v->instance->data;
    SET_INT_RETURN(handle->Read());
}

void mrbc_init_class_digital(struct VM *vm)
{
  digital_init(&dh[0], D1);
  digital_init(&dh[1], D2);

  // define class and methods.
  mrb_class *digital;
  digital = mrbc_define_class(0, "Digital",	mrbc_class_object);
  mrbc_define_method(0, digital, "new",   digital_new);
  mrbc_define_method(0, digital, "write",   digital_write);
  mrbc_define_method(0, digital, "read", digital_read);
}
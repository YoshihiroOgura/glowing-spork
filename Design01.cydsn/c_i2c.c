/*! @file
  @brief
  I2C class for Cypress PSoC5LP

  <pre>
  Copyright (C) 2018 Kyushu Institute of Technology.
  Copyright (C) 2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  (Usage)
  * Place 'I2C Master' device by PSoC Creator.
  * Add file 'c_i2c.c' to PSoC Creator.
  * Add below to main.c.
      #include <c_i2c.h>
      mrbc_init_class_i2c(0);

  (Note)
  * Only I2C master mode and only 1 device.
    Preparing to handle multiple instances, but can't use it in this version.

  (on Ruby)
    i2c = I2C.new()

    device_register = 0xNN  # device specific register or nil
    i2c.write( ADRS, device_register, data1, ... )

    length = N              # read byte length
    res = i2c.read( ADRS, device_register, length )
    res[0].ord; res[1].ord; ...

    # convert byte array to uint16, int16 example.
    def to_uint16( b1, b2 )
      return (b1 << 8 | b2)
    end
    def to_int16( b1, b2 )
      return (b1 << 8 | b2) - ((b1 & 0x80) << 9)
    end

  </pre>
*/


#include "vm_config.h"
#include <stdint.h>
#include <project.h>	// auto generated by PSoC Creator.

#include "value.h"
#include "alloc.h"
#include "static.h"
#include "class.h"
#include "c_string.h"
#include "console.h"


// convert pseudo identifier to real identifier.
#define I2CNAME_MSTR_NO_ERROR		I2C_1_MSTR_NO_ERROR
#define I2CNAME_ACK_DATA		I2C_1_ACK_DATA
#define I2CNAME_NAK_DATA		I2C_1_NAK_DATA
#define I2CNAME_MasterClearStatus	I2C_1_MasterClearStatus
#define I2CNAME_MasterReadByte		I2C_1_MasterReadByte
#define I2CNAME_MasterSendRestart	I2C_1_MasterSendRestart
#define I2CNAME_MasterSendStart		I2C_1_MasterSendStart
#define I2CNAME_MasterSendStop		I2C_1_MasterSendStop
#define I2CNAME_MasterStatus		I2C_1_MasterStatus
#define I2CNAME_MasterWriteByte		I2C_1_MasterWriteByte
#define I2CNAME_Start			I2C_1_Start


//================================================================
/*! Define I2C attribute structure.
*/
struct I2C_attr
{
  uint8_t status;
  uint8_t flag_transaction;
};


//================================================================
/*! I2C constructor
*/
static void c_i2c_new(mrb_vm *vm, mrb_value v[], int argc)
{
  *v = mrbc_instance_new(vm, v->cls, sizeof(struct I2C_attr));
  struct I2C_attr *attr = (struct I2C_attr *)v->instance->data;
  attr->status = 0;
  attr->flag_transaction = 0;
}


//================================================================
/*! I2C get status
*/
static void c_i2c_status(mrb_vm *vm, mrb_value v[], int argc)
{
  struct I2C_attr *attr = (struct I2C_attr *)v->instance->data;
  int status = I2CNAME_MasterStatus();
  attr->status = status;
  SET_INT_RETURN( status );
}


//================================================================
/*! I2C clear status
*/
static void c_i2c_clear_status(mrb_vm *vm, mrb_value v[], int argc)
{
  struct I2C_attr *attr = (struct I2C_attr *)v->instance->data;

  I2CNAME_MasterClearStatus();
  attr->status = 0;
  attr->flag_transaction = 0;
}


//================================================================
/*! I2C read (convenience function)

  (mruby usage)
  s = i2c_read( i2c_adrs_7, device_register, read_bytes )
  s[n].ord  # bytes

  #i2c_adrs_7 = Fixnum
  #device_register = Fixnum or nil
  #read_byres = Fixnum

  (I2C Sequence, device_register specified.)
  S - ADRS W (A) - DR (A) - Sr - ADRS R (A) - data1 (A)... datan (N) - P

  (I2C Sequence, device_register not specified.)
  S - ADRS R (A) - data1 (A)... datan (N) - P

  (I2C Sequence, read_bytes not specified.)
  S - ADRS R (A)

    DR: device_register
    A : Ack
    N : Nack
*/
static void c_i2c_read(mrb_vm *vm, mrb_value v[], int argc)
{
  struct I2C_attr *attr = (struct I2C_attr *)v->instance->data;
  uint32_t status = 0;
  mrb_value ret = mrb_nil_value();
  uint8_t *buf = NULL;

  /*
    Get parameter
  */
  if( argc != 2 && argc != 3 ) goto ERROR_PARAM;
  if( v[1].tt != MRB_TT_FIXNUM ) goto ERROR_PARAM;
  int i2c_adrs_7 = GET_INT_ARG(1);

  int device_register;
  switch( v[2].tt ) {
  case MRB_TT_FIXNUM:	device_register = GET_INT_ARG(2);	break;
  case MRB_TT_NIL:	device_register = -1;			break;
  default:		goto ERROR_PARAM;
  }

  int read_bytes = -1;
  if( argc == 3 ) {
    if( v[3].tt != MRB_TT_FIXNUM ) goto ERROR_PARAM;
    read_bytes = GET_INT_ARG(3);
    buf = mrbc_alloc(vm, read_bytes + 1);
    if( !buf ) goto DONE;
  }


  /*
    Start I2C communication
  */
  // send START condition, slave address and R/W (1:read, 0:write)
  if( !attr->flag_transaction ) {
    I2CNAME_MasterClearStatus();
    status = I2CNAME_MasterSendStart( i2c_adrs_7, (device_register < 0));
  } else {
    status = I2CNAME_MasterSendRestart( i2c_adrs_7, (device_register < 0));
  }
  if( status != I2CNAME_MSTR_NO_ERROR ) goto ERROR;
  attr->flag_transaction = 1;

  // send DEVICE REGISTER if specified.
  if( device_register >= 0 ) {
    status = I2CNAME_MasterWriteByte( device_register );	// specify register
    if( status != I2CNAME_MSTR_NO_ERROR ) goto ERROR;

    status = I2CNAME_MasterSendRestart( i2c_adrs_7, 1 );	// 1:read
    if( status != I2CNAME_MSTR_NO_ERROR ) goto ERROR;
  }

  if( read_bytes < 0 ) goto DONE;

  // read data.
  uint8_t *p_buf = buf;
  for( int i = read_bytes - 1; i > 0; i-- ) {
    *p_buf++ = I2CNAME_MasterReadByte( I2CNAME_ACK_DATA );
  }
  *p_buf++ = I2CNAME_MasterReadByte( I2CNAME_NAK_DATA );
  *p_buf = 0;

  // send STOP condition.
  status = I2CNAME_MasterSendStop();
  if( status != I2CNAME_MSTR_NO_ERROR ) goto ERROR;

  ret = mrbc_string_new_alloc(vm, buf, read_bytes);
  attr->flag_transaction = 0;
  goto DONE;


 ERROR_PARAM:
  console_printf("i2c_read: parameter error.\n");
  goto DONE;

 ERROR:
  I2CNAME_MasterSendStop();
  attr->flag_transaction = 0;
  if( buf ) mrbc_raw_free(buf);
  goto DONE;


 DONE:
  attr->status = status;

  SET_RETURN( ret );
}


//================================================================
/*! I2C write (Convenience function)

  (mruby usage)
  i2c_write( i2c_adrs_7, device_register, write_data, ... )

  #i2c_adrs_7 = Fixnum
  #device_register = Fixnum or nil
  #write_data = String, or Fixnum...

  (I2C Sequence)
  S - ADRS W (A) - DR (A) - data1 (A)... - P

    DR: device_register, option.
    A : Ack
*/
static void c_i2c_write(mrb_vm *vm, mrb_value v[], int argc)
{
  struct I2C_attr *attr = (struct I2C_attr *)v->instance->data;
  uint32_t status = 0;

  /*
    Get parameter
  */
  if( argc < 2 ) goto ERROR_PARAM;
  if( v[1].tt != MRB_TT_FIXNUM ) goto ERROR_PARAM;
  int i2c_adrs_7 = GET_INT_ARG(1);

  int device_register;
  switch( v[2].tt ) {
  case MRB_TT_FIXNUM:	device_register = GET_INT_ARG(2);	break;
  case MRB_TT_NIL:	device_register = -1;			break;
  default:		goto ERROR_PARAM;
  }

  int write_bytes = -1;
  const char *buf = NULL;
  if( argc >= 3 ) {
    if( v[3].tt == MRB_TT_STRING ) {
      write_bytes = mrbc_string_size( &GET_ARG(3) );
      buf = mrbc_string_cstr( &GET_ARG(3) );
    } else {
      write_bytes = argc - 2;
    }
  }


  /*
    Start I2C communication
  */
  // send START condition, slave address and R/W (0:write)
  if( !attr->flag_transaction ) {
    I2CNAME_MasterClearStatus();
    status = I2CNAME_MasterSendStart( i2c_adrs_7, 0 );
  } else {
    status = I2CNAME_MasterSendRestart( i2c_adrs_7, 0 );
  }
  if( status != I2CNAME_MSTR_NO_ERROR ) goto ERROR;
  attr->flag_transaction = 1;

  // send DEVICE REGISTER if specified.
  if( device_register >= 0 ) {
    status = I2CNAME_MasterWriteByte( device_register );
    if( status != I2CNAME_MSTR_NO_ERROR ) goto ERROR;
  }

  if( write_bytes < 0 ) goto DONE;

  // write data.
  for( int i = 0; i < write_bytes; i++ ) {
    if( buf ) {
      status = I2CNAME_MasterWriteByte( *buf++ );
    } else {
      if( v[i+3].tt != MRB_TT_FIXNUM ) goto ERROR;
      status = I2CNAME_MasterWriteByte( v[i+3].i );
    }
    if( status != I2CNAME_MSTR_NO_ERROR ) goto ERROR;
  }

  // send STOP condition.
  status = I2CNAME_MasterSendStop();
  if( status != I2CNAME_MSTR_NO_ERROR ) goto ERROR;
  attr->flag_transaction = 0;

  goto DONE;


 ERROR_PARAM:
  console_printf("i2c.write: parameter error.\n");
  goto DONE;

 ERROR:
  I2CNAME_MasterSendStop();
  attr->flag_transaction = 0;
  goto DONE;

 DONE:
  attr->status = status;

  SET_INT_RETURN(status);
}



//================================================================
/*! initialize
*/
void mrbc_init_class_i2c(struct VM *vm)
{
  I2CNAME_Start();	// start physical device

  mrb_class *i2c;
  i2c = mrbc_define_class(vm, "I2C",	mrbc_class_object);

  mrbc_define_method(vm, i2c, "new",	c_i2c_new);
  mrbc_define_method(vm, i2c, "read",	c_i2c_read);
  mrbc_define_method(vm, i2c, "write",	c_i2c_write);
  mrbc_define_method(vm, i2c, "status",	c_i2c_status);
  mrbc_define_method(vm, i2c, "clear_status", c_i2c_clear_status);
}

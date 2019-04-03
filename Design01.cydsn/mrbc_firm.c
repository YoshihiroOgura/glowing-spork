/*! @file
  @brief
  mruby/c firmware for mruby/c devkit 01

  <pre>
  Copyright (C) 2018 Kyushu Institute of Technology.
  Copyright (C) 2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/


#include <project.h>
#include <stdlib.h>

#include "mrbc_firm.h"


#define MRUBYC_ROM_STRING       "mruby/c PSoC_5LP"
#define MRUBYC_VERSION_STRING   "v1.00 (2018/09/04)"

#define USBUART_BUFFER_SIZE 64	// max 64bytes. see USBFS manual.



//================================================================
/*! USBUART read buffer.
*/
static struct {
  uint8_t buf[USBUART_BUFFER_SIZE * 2 + 1];
  char flag_can_read_line;
  uint8_t *p_buf_w;
} usbuart_;



//================================================================
/*! USBUART buffer clear.
*/
void usbuart_clear(void)
{
  usbuart_.p_buf_w = usbuart_.buf;
  usbuart_.flag_can_read_line = 0;
}


//================================================================
/*! USBUART Can read line ?

  @retval	true if a line of data can be read.
*/
int usbuart_can_read_line(void)
{
  return usbuart_.flag_can_read_line;
}


//================================================================
/*! USBUART return the byte size of buffer data.

  @retval	size
*/
int usbuart_size(void)
{
  return usbuart_.p_buf_w - usbuart_.buf;
}


//================================================================
/*! USBUART return the pointer to read buffer.

  @retval	pointer to read buffer.
*/
char * usbuart_cstr(void)
{
  return (char*)usbuart_.buf;
}


//================================================================
/*! USBUART put string

  @param
*/
void usbuart_put_string( const char *s )
{
  //  To be sure that the previous data has finished sending.
  while( !USBUART_CDCIsReady() ) { /* busy loop. */ }

  USBUART_PutString(s);
}


//================================================================
/*! USBUART binary read.

  @param  buf	pointer to read buffer.
  @param  nbyte	maximum reading byte length.
  @retval	read length.
*/
int usbuart_read( void *buf, int nbyte )
{
  int len = nbyte;

  while( len > 0 ) {
    if( usbuart_size() == 0 ) {
      if( usbuart_event() < 0 ) {
	return 0;
      }
      continue;
    }

    int copy_size = usbuart_size() < len ? usbuart_size() : len;
    int remain = usbuart_size() - copy_size;

    memcpy( buf, usbuart_.buf, copy_size );
    if( remain > 0 ) {
      memmove( usbuart_.buf, usbuart_.buf + copy_size, remain );
      usbuart_.p_buf_w = usbuart_.buf + remain;
    } else {
      usbuart_clear();
    }

    buf += copy_size;
    len -= copy_size;
  }

  return nbyte;
}


//================================================================
/*! USBUART binary write.

  @param  buf	pointer to write data.
  @param  nbyte	write byte size.
*/
void usbuart_write( const char *buf, int nbyte )
{
  while( nbyte > 0 ) {
    //  To be sure that the previous data has finished sending.
    while( !USBUART_CDCIsReady() ) { /* busy loop. */ }

    int size = (nbyte < USBUART_BUFFER_SIZE) ? nbyte : USBUART_BUFFER_SIZE;
    USBUART_PutData( (uint8_t*)buf, size );
    buf += USBUART_BUFFER_SIZE;
    nbyte -= USBUART_BUFFER_SIZE;
  }

  if( nbyte == 0 ) {
    /* Send zero-length packet to PC. */
    while( !USBUART_CDCIsReady() ) { /* busy loop. */ }
    USBUART_PutData(NULL, 0);
  }
}


//================================================================
/*! USBUART event handler.

  @retval
*/
int usbuart_event(void)
{
  if( USBUART_IsConfigurationChanged() ) {
    /* Initialize IN endpoints when device is configured. */
    if( USBUART_GetConfiguration() ) {
      /* Enumeration is done, enable OUT endpoint to receive data from host. */
      USBUART_CDC_Init();
    }

    return 1;
  }

  if( !USBUART_GetConfiguration() ) {
    return -1;
  }

  if( USBUART_DataIsReady() ) {
    int count = USBUART_GetCount();
    int remain = sizeof(usbuart_.buf) - usbuart_size() - 1;

    if( count > remain ) count = remain;
    USBUART_GetData( usbuart_.p_buf_w, count );

    for( ; count != 0; count-- ) {
      if( *usbuart_.p_buf_w++ == '\n' ) {
	usbuart_.flag_can_read_line = 1;
      }
    }
    *usbuart_.p_buf_w = '\0';

    // TODO check buffer full
    return 2;
  }

  return 0;
}



//================================================================
/*! write bytecode

  @param  size	size of bytecode
*/
static int write_bytecode( uint16_t size )
{
  static const uint8_t *flash_addr = mruby_bytecode + sizeof(uint16_t) * MRBC_MAX_BYTECODES;
  static const uint8_t * flash_addr_end = mruby_bytecode + MRBC_SIZE_IREP_STRAGE;
  static int bytecode_index = 0;

  cystatus status;
  uint8_t buf[CY_FLASH_SIZEOF_ROW];
  uint16_t tbl_bytecode_size[MRBC_MAX_BYTECODES];

  if( bytecode_index >= MRBC_MAX_BYTECODES ) return 1;
  if( flash_addr + size > flash_addr_end ) return 2;
  if( bytecode_index == 0 ) {
    memset( tbl_bytecode_size, 0, sizeof(tbl_bytecode_size) );
  } else {
    memcpy( tbl_bytecode_size, mruby_bytecode, sizeof(tbl_bytecode_size) );
  }
  tbl_bytecode_size[bytecode_index] = size;

  status = Em_EEPROM_Write((const uint8_t *)tbl_bytecode_size,
			   mruby_bytecode, sizeof(tbl_bytecode_size));
  if (CYRET_SUCCESS != status) {
    //ogura
    //CONS_PutString("Em EEPROM size write error.\r\n");
    if(USBUART_bGetConfiguration()){usbuart_put_string("Em EEPROM size write error.\r\n");};
    while (1) { /* Will stay here */ }
  }

  while( size > 0 ) {
    int read_size = size < sizeof(buf) ? size : sizeof(buf);
    read_size = usbuart_read( buf, read_size );

    status = Em_EEPROM_Write(buf, flash_addr, read_size);

    if (CYRET_SUCCESS != status) {
      //ogura
      //CONS_PutString("Em EEPROM error3\r\n");
      if(USBUART_bGetConfiguration()){usbuart_put_string("Em EEPROM error3\r\n");};
      while (1) { /* Will stay here */ }
    }

    size -= read_size;
    flash_addr += read_size;
  }

  usbuart_clear();
  bytecode_index++;

  return 0;
}


//================================================================
/*! monitor mode

*/
void mode_monitor(void)
{
  static const char ws[] = " \t\r\n\f\v";
  static const char *help_msg =
    "Commands:\r\n"
    "  version\r\n"
    "  reset\r\n"
    "  execute\r\n"
    "  write [size]\r\n";

  while( 1 ) {
    usbuart_event();
    if( !usbuart_can_read_line() ) continue;

    // TODO check buffer full

    char *token = strtok( usbuart_cstr(), ws );
    if( token == NULL ) {
      usbuart_put_string( "+OK mruby/c\r\n" );
      goto DONE;
    }

    if( strcmp(token, "help") == 0 ) {
      usbuart_put_string( "+OK\r\n" );
      usbuart_put_string(help_msg);
      usbuart_put_string( "+DONE\r\n" );
      goto DONE;
    }

    if( strcmp(token, "version") == 0 ) {
      usbuart_put_string("+OK " MRUBYC_ROM_STRING " " MRUBYC_VERSION_STRING "\r\n");
      goto DONE;
    }

    if( strcmp(token, "reset") == 0 ) {
      usbuart_put_string("+OK\r\n");
      CyDelay(100);
      CySoftwareReset();
    }

    if( strcmp(token, "execute") == 0 ) {
      usbuart_put_string("+OK Execute mruby/c.\r\n");
      return;	// to execute mrubyc();
    }

    if( strcmp(token, "write") == 0 ) {
      token = strtok( NULL, ws );
      if( token != NULL ) {
	int size = atoi(token);
	if( size < 0 || size > MRBC_SIZE_IREP_STRAGE ) {
	  usbuart_put_string( "-ERR size overflow.\r\n" );
	  goto DONE;
	}

	usbuart_clear();
	usbuart_put_string( "+OK Write bytecode.\r\n" );
	switch( write_bytecode( size ) ) {
	case 0:
	  usbuart_put_string( "+DONE\r\n" );
	  goto DONE;
	case 1:
	  usbuart_put_string( "-ERR number of programs overflow.\r\n" );
	  goto DONE;
	case 2:
	  usbuart_put_string( "-ERR total bytecode size overflow.\r\n" );
	  goto DONE;
	}
      }

      usbuart_put_string( "-ERR\r\n" );
      goto DONE;
    }

    usbuart_put_string( "-ERR Illegal command.\r\n" );


  DONE:
    usbuart_clear();
  } // eternal loop.
}

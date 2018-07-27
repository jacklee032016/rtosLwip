/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 6658 $
 * $Date: 2009-10-30 14:49:58 +0800 (Fri, 30 Oct 2009) $
 *
 * Purpose : RTL8307H switch high-level API for RTL8367B
 * Feature : 
 *
 */
//frank stdlib (malloc) string (memcpy)
#include <stdlib.h>
#include <string.h>

#include <rtk_types.h>
#include <rtk_error.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_debug.h>

#include <rtl8307h_asicdrv_mem_ctrl.h>

#include <stdio.h>

uint32 rtl8307h_memCtrl_switch_reg_get(uint32 addr, uint32 *val)
{
    uint32 busy;
    uint32 cnt;
    
    reg_write(RTL8307H_UNIT, SWITCHACCESS_ADDRESS, addr); 
    
    reg_field_write(RTL8307H_UNIT, SWITCHACCESS_CONTROL, REG_ACCESS_TYPE, REG_READ);
    reg_field_write(RTL8307H_UNIT, SWITCHACCESS_CONTROL, REG_ACCESS_TRIGGRE, 1);

    cnt = 0;
    do {
        reg_field_read(RTL8307H_UNIT, SWITCHACCESS_CONTROL, REG_ACCESS_TRIGGRE, &busy);
    }while((busy == 1) && (cnt++ < 100));

    if (cnt >= 100)
        DEBUG_INFO(DBG_MEMCTRL, "rtl8307h_memCtrl_switch_reg_get() Time out!\n");
    
    reg_read(RTL8307H_UNIT, SWITCHACCESS_DATA, val);

    return SUCCESS;
}

uint32 rtl8307h_memCtrl_switch_reg_set(uint32 addr, uint32 val)
{
    uint32 busy;
    uint32 cnt;
 
    reg_write(RTL8307H_UNIT, SWITCHACCESS_ADDRESS, addr); 
    reg_write(RTL8307H_UNIT, SWITCHACCESS_DATA, val);

    reg_field_write(RTL8307H_UNIT, SWITCHACCESS_CONTROL, REG_ACCESS_TYPE, REG_WRITE);
    reg_field_write(RTL8307H_UNIT, SWITCHACCESS_CONTROL, REG_ACCESS_TRIGGRE, 1);

    cnt = 0;
    do {
        reg_field_read(RTL8307H_UNIT, SWITCHACCESS_CONTROL, REG_ACCESS_TRIGGRE, &busy);
    }while((busy == 1) && (cnt++ < 100));

    if (cnt >= 100)
        DEBUG_INFO(DBG_MEMCTRL, "rtl8307h_memCtrl_switch_reg_set() Time out!\n");
    
    return SUCCESS;
}

uint32 rtl8307h_memCtrl_sram_get(uint16 addr, uint32 *val_high, uint32 *val_low)
{
    uint32 busy;
    
    if ((addr & 0x7) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "rtl8307h_memCtrl_sram_get() addr not aligned\n");
        return RT_ERR_INPUT;    
    }
    
    reg_field_write(RTL8307H_UNIT, INTERNAL_MEMORY_CONTROL, MEMORY_ADDR, (addr >> 3));
    reg_field_write(RTL8307H_UNIT, INTERNAL_MEMORY_CONTROL, CMD_TYPE, 0);   
    reg_field_write(RTL8307H_UNIT, INTERNAL_MEMORY_CONTROL, CMD_TRIGGER, 1);

    do {
        reg_field_read(RTL8307H_UNIT, INTERNAL_MEMORY_CONTROL, CMD_TRIGGER, &busy);
    }while(busy);
    
    reg_read(RTL8307H_UNIT, INTERNAL_MEMORY_DATA0, val_low);
    reg_read(RTL8307H_UNIT, INTERNAL_MEMORY_DATA1, val_high);
    
    return SUCCESS;
}

uint32 rtl8307h_memCtrl_sram_set(uint16 addr, uint32 val_high, uint32 val_low)
{
    uint32 busy;

    if ((addr & 0x7) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "rtl8307h_memCtrl_sram_set() addr not aligned\n");
        return RT_ERR_INPUT;
    }

    reg_write(RTL8307H_UNIT, INTERNAL_MEMORY_DATA0, val_low);
    reg_write(RTL8307H_UNIT, INTERNAL_MEMORY_DATA1, val_high);
    
    reg_field_write(RTL8307H_UNIT, INTERNAL_MEMORY_CONTROL, MEMORY_ADDR, (addr >> 3));
    reg_field_write(RTL8307H_UNIT, INTERNAL_MEMORY_CONTROL, CMD_TYPE, 1);   
    reg_field_write(RTL8307H_UNIT, INTERNAL_MEMORY_CONTROL, CMD_TRIGGER, 1);

    do {
        reg_field_read(RTL8307H_UNIT, INTERNAL_MEMORY_CONTROL, CMD_TRIGGER, &busy);
    }while(busy); 
    
    return SUCCESS;
}

uint32 rtl8307h_memCtrl_sram_dump(uint32 start, uint32 end)
{
    uint32 val_high;
    uint32 val_low;
    uint32 i = 0;

    uint32 addr;

    if ((start & 0x7) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "WARNING: rtl8307h_memCtrl_sram_dump(): The start address is not aligned by 8 bytes\n");
        return RT_ERR_INPUT;
    }
    
    DEBUG_INFO(DBG_MEMCTRL, "Dump SRAM: 0x%08x \n", (start & 0xffff8)); /* aligned start by 16 bytes */
    for (addr = start & 0xffff8; addr < end; addr+=8, i+=8)
    {
        rtl8307h_memCtrl_sram_get(addr, &val_high, &val_low);
        DEBUG_INFO(DBG_MEMCTRL, "%02x ", (val_low & 0xff));
        DEBUG_INFO(DBG_MEMCTRL, "%02x ", ((val_low >> 8) & 0xff));
        DEBUG_INFO(DBG_MEMCTRL, "%02x ", ((val_low >> 16)& 0xff));
        DEBUG_INFO(DBG_MEMCTRL, "%02x ", ((val_low >> 24)& 0xff));

        DEBUG_INFO(DBG_MEMCTRL, "%02x ", (val_high & 0xff));
        DEBUG_INFO(DBG_MEMCTRL, "%02x ", ((val_high >> 8) & 0xff));
        DEBUG_INFO(DBG_MEMCTRL, "%02x ", ((val_high >> 16)& 0xff));
        DEBUG_INFO(DBG_MEMCTRL, "%02x ", ((val_high >> 24)& 0xff));
        
        DEBUG_INFO(DBG_MEMCTRL, " ");
        if ((i != 0) && (i % 16 == 8))
            DEBUG_INFO(DBG_MEMCTRL, "\n");
    }   
    DEBUG_INFO(DBG_MEMCTRL, "\n");

    return SUCCESS;
}

uint32 rtl8307h_memCtrl_sram_set2(uint16 addr, uint8 *buf, uint16 len)
{
    uint32 val_high;
    uint32 val_low;
    
    if ((addr & 0x7) != 0 || (len & 0x7) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "rtl8307h_memCtrl_sram_set2() addr or length not aligned\n");
        return RT_ERR_INPUT;
    }

    while (len > 0)
    {
        val_low = buf[0] | ((uint32)buf[1] << 8) \
                | ((uint32)buf[2] << 16) | ((uint32)buf[3] << 24);
        val_high = buf[4] | ((uint32)buf[5] << 8) \
                | ((uint32)buf[6] << 16) | ((uint32)buf[7] << 24);

        rtl8307h_memCtrl_sram_set(addr, val_high, val_low);

        addr+= 8;
        buf += 8;
        len -= 8;
    }
        
    return SUCCESS;
}

uint32 rtl8307h_memCtrl_sram_get2(uint16 addr, uint8 *buf, uint16 len)
{
    uint32 val_high;
    uint32 val_low;
    
    if ((addr & 0x7) != 0 || (len & 0x7) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "rtl8307h_memCtrl_sram_get2() addr or length not aligned\n");
        return RT_ERR_INPUT;
    }

    while (len > 0)
    {
        rtl8307h_memCtrl_sram_get(addr, &val_high, &val_low);

        buf[0] = val_low & 0xff;
        buf[1] = (val_low >> 8) & 0xff;
        buf[2] = (val_low >> 16)& 0xff;
        buf[3] = (val_low >> 24)& 0xff;

        buf[4] = val_high & 0xff;
        buf[5] = (val_high >> 8) & 0xff;
        buf[6] = (val_high >> 16)& 0xff;
        buf[7] = (val_high >> 24)& 0xff;      

        addr+= 8;
        buf += 8;
        len -= 8;
    }
        
    return SUCCESS;
}

uint32 rtl8307h_memCtrl_memcpy(uint16 dest, uint16 src, uint16 len)
{
    uint32 busy;

    if ((src & 0x7) != 0 || (dest & 0x7) != 0 || (len & 0x7) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "rtl8307h_memCtrl_memcpy() addr or length not aligned by 8\n");
        return RT_ERR_INPUT;
    } 

    reg_write(RTL8307H_UNIT, SOURCE_MEMORY_ADDRESS, (src >> 3));
    reg_write(RTL8307H_UNIT, DESTINATION_MEMORY_ADDRESS, (dest >> 3));
    reg_write(RTL8307H_UNIT, DATA_LENGTH_, (len >> 3));

    reg_write(RTL8307H_UNIT, DMA_CONTROL, 1);
    
    do {
        reg_read(RTL8307H_UNIT, DMA_CONTROL, &busy);
    }while(busy); 
    
    return SUCCESS;
}

uint32 rtl8307h_memCtrl_iorm_get(uint32 addr, uint8 *val)
{
    uint32 busy;
    uint32 reg_val;
        
    reg_write(RTL8307H_UNIT, IROM_ACCESS_ADDRESS, addr);
    reg_field_write(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TYPE, 0);   
    reg_field_write(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TRIGGER, 1);

    do {
        reg_field_read(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TRIGGER, &busy);
    }while(busy);
    
    reg_read(RTL8307H_UNIT, IROM_ACCESS_DATA, &reg_val);
    *val = reg_val;
    
    return SUCCESS;
}

uint32 rtl8307h_memCtrl_iorm_set(uint32 addr, uint8 val)
{
    uint32 busy;
        
    reg_write(RTL8307H_UNIT, IROM_ACCESS_ADDRESS, addr);
    reg_write(RTL8307H_UNIT, IROM_ACCESS_DATA, val);

    reg_field_write(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TYPE, 1);       
    reg_field_write(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TRIGGER, 1);

    do {
        reg_field_read(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TRIGGER, &busy);
    }while(busy);

    return SUCCESS;
}

void rtl8307h_memCtrl_iorm_set_4bytes(uint32 addr, uint32 val)
{
    uint32 busy;
        
    reg_write(RTL8307H_UNIT, IROM_ACCESS_ADDRESS, addr);
    reg_write(RTL8307H_UNIT, IROM_ACCESS_DATA, val);

    reg_field_write(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TYPE, 1);       
    reg_field_write(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TRIGGER, 1);

    do {
        reg_field_read(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_ACCESS_TRIGGER, &busy);
    }while(busy);

    return;
}


static void __spi_trigger(void)
{
    uint32 regval;

    /*trigger*/
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER,1);
  
    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, &regval);
    }while (regval);

    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_CONTROLLER_BUSY, &regval);
    }while (regval);       
}

/*read status world*/
uint8 __spi_flash_readSR(void)
{
    uint32 regval;
     
    /* Check Controller Ready */
    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_CONTROLLER_BUSY, &regval);
    }while (regval);  

    /* set cmd type*/
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 0x05); /* Read Status */
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 1); /* Read Status */
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, REG_READ); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 2); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
    
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 

    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, 1); 
    
    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, &regval);
    }while (regval);

    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_CONTROLLER_BUSY, &regval);
    }while (regval);   
    
    reg_read(RTL8307H_UNIT, SPI_SMODE_DATA0, &regval);
    return (regval & 0xff);
}

static void __spi_flash_waitRdy(void)
{
    uint8 regval;
    do {
        regval = __spi_flash_readSR();             
    } while (regval & SPI_FLASH_WIP);
    
}

void __spi_flash_waitWEL(void)
{
    uint32 regval;
    
    __spi_flash_waitRdy();
  
    /* Set WriteEnable */
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 0x06); /* Write Enable */
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 1); /* Read Status */
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, REG_WRITE); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
    
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 

    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, 1); 
        
    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, &regval);
    }while (regval);

    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_CONTROLLER_BUSY, &regval);
    }while (regval);   
  
    __spi_flash_waitRdy();

#ifdef	MUX_LAB
	while (!(__spi_flash_readSR() & SPI_FLASH_WEL) );
#else
    while (!__spi_flash_readSR() & SPI_FLASH_WEL);
#endif	
}

uint32 set_cmd(spi_cmd_t* param)
{
    uint32 reg_val;

    __spi_flash_waitRdy();
    
    reg_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, &reg_val);            
            
    reg_field_set(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, param->cmd_word, &reg_val); 
    reg_field_set(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, param->skip_addr, &reg_val); 
    reg_field_set(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, param->direction, &reg_val); 
    reg_field_set(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, param->byte_num, &reg_val); 
    reg_field_set(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, param->dummy_num, &reg_val); 
    
    reg_field_set(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, param->data_com_num, &reg_val); 
    reg_field_set(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, param->addr_com_num, &reg_val); 
    reg_field_set(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, 1, &reg_val); 

    reg_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, reg_val); 

    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, &reg_val);
    }while (reg_val);

    do {
        reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_CONTROLLER_BUSY, &reg_val);
    }while (reg_val);

    return SUCCESS;    
}

/*write to flash. inbuf[0]->highest byte */
uint8 spi_flash_write_single(uint32 start_adr, uint8 *buf, uint32 write_len)
{
    uint16 i;
    uint32 regval;
    
    regval = __spi_flash_readSR();   
    if ((regval & 0xBC) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "Flash is protected\n");   
        return -1;
    }
    
    for (i = 0; i < write_len; i++)
    {
        __spi_flash_waitWEL();

        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 2); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 


        reg_write(RTL8307H_UNIT, SPI_SMODE_ADDRESS, start_adr); 
        reg_write(RTL8307H_UNIT, SPI_SMODE_DATA0, *buf++);
           
        __spi_trigger();
        start_adr += 1;      
    }

    /* Step1: Check Write in Process bit */
    __spi_flash_waitRdy();

    return SUCCESS;
}

uint8 spi_flash_write_huge(uint32 start_adr, uint8 *buf, uint32 write_len)
{
    uint8* str;
    uint32 regval;
    uint16 left;
    uint16 i;
    
    regval = __spi_flash_readSR();   
    if ((regval & 0xBC) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "Flash is protected\n");   
        return -1;
    }

    if (start_adr % 16 != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "The Start address should aligned by 16\n");    
        return -1;
    }

    if (write_len > 100000)
    {
        DEBUG_INFO(DBG_MEMCTRL, "The Length is beyond 100000\n");    
        return -1;
    }    

    str = malloc(100000);
    if (str == NULL)
    {
        DEBUG_INFO(DBG_MEMCTRL, "Not enough memory\n");    
        return -1;
    }
    
    memcpy(str, buf, write_len);
    if (write_len % 16 != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "Warning: Program Addr: 0x%08x length: 0x%04x\n", start_adr, write_len);   
        DEBUG_INFO(DBG_MEMCTRL, "Append Payload with 0xff because the write_len is not multiple of 16\n");   
        left = 16 - (write_len % 16);

        while (left-- > 0)
            str[write_len++] = 0xff;    
    }
    
    for (i = 0; i < write_len; i+= 16)
    {
        __spi_flash_waitWEL();

        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 2); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 16); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 

        reg_write(RTL8307H_UNIT, SPI_SMODE_ADDRESS, start_adr); 

        regval = str[i] | ((uint32)str[i+1] << 8) | ((uint32)str[i+2] << 16) | ((uint32)str[i+3] << 24);
        reg_write(RTL8307H_UNIT, SPI_SMODE_DATA0, regval);

        regval = str[i+4] | ((uint32)str[i+5] << 8) | ((uint32)str[i+6] << 16) | ((uint32)str[i+7] << 24);
        reg_write(RTL8307H_UNIT, SPI_SMODE_DATA1, regval);

        regval = str[i+8] | ((uint32)str[i+9] << 8) | ((uint32)str[i+10] << 16) | ((uint32)str[i+11] << 24);
        reg_write(RTL8307H_UNIT, SPI_SMODE_DATA2, regval);

        regval = str[i+12] | ((uint32)str[i+13] << 8) | ((uint32)str[i+14] << 16) | ((uint32)str[i+15] << 24);
        reg_write(RTL8307H_UNIT, SPI_SMODE_DATA3, regval);
        
        __spi_trigger();
        start_adr += 16;    
    }

    /* Step1: Check Write in Process bit */
    __spi_flash_waitRdy();

    free(str);
    return SUCCESS;
}    

#if 0
uint8 spi_flash_write(uint32 start_adr, uint8 *buf, uint32 write_len)
{
    uint16 len = write_len;
    uint32 addr = start_adr;
    uint8 * data_buf = buf;
    uint16 left;
    uint32 regval;
    uint16 i;
       
    left = (4 - addr % 4) %4;

    for (i = 0; i < left; i++)
    {
        __spi_flash_waitWEL();

        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 2); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 


        reg_write(RTL8307H_UNIT, SPI_SMODE_ADDRESS, addr); 
        reg_write(RTL8307H_UNIT, SPI_SMODE_DATA0, *data_buf++);
           
        __spi_trigger();
        addr += 1;
        len -= 1;       
    }

    /* Step1: Check Write in Process bit */
    __spi_flash_waitRdy();


    /*
     * main program portion!
     */
    while (len >= 4)
    {   
        /* Set write enable */
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 0x06); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 1); 
        
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0);
        
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, 1); 
    
        do {
            reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_SINGLE_COMMAND_TRIGGER, &regval);
        }while (regval);

        do {
            reg_field_read(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_CONTROLLER_BUSY, &regval);
        }while (regval); 

        /* Page Program */ 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 2); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 4); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 

        reg_write(RTL8307H_UNIT, SPI_SMODE_ADDRESS, addr); 

        regval = *data_buf++;
        regval <<= 8;

        regval |= *data_buf++;
        regval <<= 8;        
                 
        regval |= *data_buf++;
        regval <<= 8; 

        regval |= *data_buf++;
        regval <<= 8; 

        reg_write(RTL8307H_UNIT, SPI_SMODE_DATA0, regval);

        __spi_trigger();

        /* Check Controller Ready in __spi_flash_readSR() can be ommitted 
            When performing optimization
          */     
        __spi_flash_waitRdy(); 

        addr += 4;                                       
        len -= 4;
    }

    /* Handle the left bytes which is not 4 bytes alligned */   
    while(len > 0)
    {
        __spi_flash_waitWEL();

        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 2); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 1); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 


        reg_write(RTL8307H_UNIT, SPI_SMODE_ADDRESS, addr); 
        reg_write(RTL8307H_UNIT, SPI_SMODE_DATA0, *data_buf++);
           
        __spi_trigger();       
        
        addr += 1;
        len -= 1;
    }
    __spi_flash_waitRdy();

    return SUCCESS;
}
#endif

/* erase flash at physical address*/
uint8 spi_flash_erase(uint32 erase_adr, uint8 flag)
{
    uint8 regval;

    regval = __spi_flash_readSR();   
    if ((regval & 0xBC) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "Flash is protected\n");   
        return -1;
    }
         
    __spi_flash_waitWEL();

    if (flag == 0)
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, SECTOR_ERASE);  /* sector erase*/
    else
        reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, BLOCK_ERASE);   /* block erase*/

    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 1); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 
    
    reg_write(RTL8307H_UNIT, SPI_SMODE_ADDRESS, erase_adr); 

    __spi_trigger();

    __spi_flash_waitRdy();
    
    return TRUE;
}

uint8 spi_chip_erase(void)
{   
    uint8 regval;

    regval = __spi_flash_readSR();   
    if ((regval & 0xBC) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "Flash is protected\n");   
        return -1;
    }   

    DEBUG_INFO(DBG_MEMCTRL, "Chip Erase... ...\n"); 
    
    __spi_flash_waitWEL();

    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, CHIP_ERASE);  /* sector erase*/
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 1); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 1); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 

    __spi_trigger();

    __spi_flash_waitRdy();
    
    return TRUE;
}

uint8 spi_flash_read_single(uint32 start_adr, uint8 * buf, uint32 read_len)
{  
    uint32 regval;

    __spi_flash_waitRdy();

    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_CMD, 0x0b);  
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_SKIP_ADDRESS, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_DIRECTION, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ACCESS_BYTE_NUMBER, 1); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DMY_CYCLE, 4); /* 8 cycle*/ 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_DATA_IO, 0); 
    reg_field_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, SPI_S_ADDRESS_IO, 0); 

    while (read_len > 0)
    {
        /* Step 1: Set Address */
        reg_write(RTL8307H_UNIT, SPI_SMODE_ADDRESS, start_adr);
        __spi_trigger();

        /* get data */
        reg_read(RTL8307H_UNIT, SPI_SMODE_DATA0, &regval);   
        *buf++ = regval & 0xff;

        start_adr++;
        read_len--;
    }

    return TRUE;
}

uint32 clear_spi_protection(void)
{
    uint8 regval;

    DEBUG_INFO(DBG_MEMCTRL, "Detecting protection....\n");  
    regval = __spi_flash_readSR();
    DEBUG_INFO(DBG_MEMCTRL, "initial status word: %d \t", regval);
    if ((regval & 0xBC) == 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "SPI hasn't been protected\n"); 
        return SUCCESS;
    }
      
    /* Clear protection & Set Write enable*/
    reg_write(RTL8307H_UNIT, SPI_SMODE_DATA0, 0x42); 
    reg_write(RTL8307H_UNIT, SPI_SMODE_CONTROL, 0x2c2001); /* Write Status */

    __spi_trigger();

    do {
        regval = __spi_flash_readSR();   
    }while(regval & 0x1);
    
    regval = __spi_flash_readSR();   
    DEBUG_INFO(DBG_MEMCTRL, "\nThe status word after trigger write status command: %d\n", regval);
    
    if ((regval & 0xBC) != 0)
    {
        DEBUG_INFO(DBG_MEMCTRL, "SPI Protection Cleaning Failed\n");   
        return -1;
    } 
  
    return SUCCESS;
}


uint32 spi_write_file(int8 * fileName)
{
    FILE *fp;
    uint8 buf[2048];
    uint32 len, addr;
    uint32 total_len;

    reg_field_write(RTL8307H_UNIT, CPU_8051_RESET, CPURST_HOLD, 1);
    DEBUG_INFO(DBG_MEMCTRL, "Stop 8051 \n");

    
    fp = fopen(fileName, "rb");
    if (fp == NULL)
    {
        DEBUG_INFO(DBG_MEMCTRL, "FILE %s open fail!\n", fileName);
        return -1;
    }

    //stop NIC
    reg_write(RTL8307H_UNIT, NIC_GLOBAL_CONTROL, 1);

    addr = 0;
    total_len = 0;
    while (1)
    {
        len = fread(buf, sizeof(char), 256, fp);
        if (len == 0)
            break;

        spi_flash_write_huge(addr, buf, len);
        
        total_len += len;
        addr += len;
        if (len < 256)
        {
            //last data
            break;
        }
    }
    
    DEBUG_INFO(DBG_MEMCTRL, "Image Length %d\n", total_len);
    return SUCCESS;
}

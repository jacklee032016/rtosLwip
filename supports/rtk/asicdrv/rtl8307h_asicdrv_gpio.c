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
 * $Revision: 8252 $
 * $Date: 2010-01-29 14:04:02 +0800 (Fri, 29 Jan 2010) $
 *
 * Purpose : RTL8307H switch high-level API for RTL8307H
 * Feature : 
 *
 */

#include <rtk_types.h>
#include <rtk_error.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>

#include <rtl8307h_asicdrv_gpio.h>

/* Function Name:
 *      rtl8307h_gpio_mode_set
 * Description:
 *      Set the GPIO mode. The mode can be either GPIO_INTR_INPUT, GPIO_INTR_OUTPUT,
 *      GPIO_DATA_INPUT or GPIO_DATA_OUTPUT.
 * Input:
 *      group                   - Group A, B
 *      io_port                 - (0, 1, 2, 3)
 *      mode                    - Either GPIO_INTR_INPUT, GPIO_INTR_OUTPUT, 
 *                                     GPIO_DATA_INPUT or GPIO_DATA_OUTPUT
 *      intr_type               - Valid only when mode is GPIO_INTR_INPUT.
 * Output:     
 *      None
 * Return: 
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
uint16 rtl8307h_gpio_mode_set(uint8 group, uint8 io_port, uint8 mode, Intr_mode_t intr_type)
{
    uint32 reg_val;
#ifdef	EXT_LAB
    uint8 bit_addr = 0;
#else
    uint8 bit_addr;
#endif
    uint8 intr_offset;

    if (group >= GPIO_GRP_C || io_port >= 4) 
        return RT_ERR_PORT_ID;

    if (mode != GPIO_INTR_INPUT && mode != GPIO_INTR_OUTPUT \
        && mode != GPIO_DATA_INPUT && mode != GPIO_DATA_OUTPUT)
        return RT_ERR_INPUT;

    /* Interrupt output applies only on gpio group B port 3 */
    if (mode == GPIO_INTR_OUTPUT)
    {
        if (group != GPIO_GRP_B || io_port != 3)
            return RT_ERR_INPUT;
    }

    /* configure gpio as interrrupt or data                 */         
    reg_read(RTL8307H_UNIT, GPIO_INT_STATUS_MASK_MODE,  &reg_val);

    if (group == GPIO_GRP_A)
    {
        bit_addr = 12 + io_port;
        intr_offset = 24;
    }
    else if (group == GPIO_GRP_B)
    {
        bit_addr = 16 + io_port;
        intr_offset = 26;
    }

    reg_val &= ~(0xfffUL); /* keep the interrupt bit */

    switch (mode)
    {
        case GPIO_INTR_INPUT:
            reg_val |= (1UL << bit_addr); 
            reg_val |= (1UL << (bit_addr - 12)); /* write 1 to clear the pending interrupt status bit */

            /* Setting Interrupt mode */
            reg_val &= ~(3UL << intr_offset);
            if (intr_type == INTR_LEVEL_LOW)
                reg_val |= (1UL << intr_offset);  
            else if (intr_type == INTR_LEVEL_HIGH) 
                reg_val |= (0UL << intr_offset);
            else if (intr_type == INTR_EDGE_LOW_TO_HIGH)
                reg_val |= (3UL << intr_offset);
            else
                reg_val |= (2UL << intr_offset);    
            break; 
        case GPIO_INTR_OUTPUT:            
        case GPIO_DATA_INPUT:
        case GPIO_DATA_OUTPUT:
            reg_val &= ~(1UL << bit_addr);
            break;
        default:
            break;
    }
       
    reg_write(RTL8307H_UNIT, GPIO_INT_STATUS_MASK_MODE, reg_val);

    /* configure the direction of gpio & resolve the multiplexing of gpios */     
    if (group == GPIO_GRP_A)
        bit_addr = 0 + io_port;
    else if (group == GPIO_GRP_B)
        bit_addr = 4 + io_port;
 
    reg_read(RTL8307H_UNIT, GPIO_FUNCSELECT_DIRECTR_DATA_REG,  &reg_val);

    /* resolve multiplexing */
    if (group == GPIO_GRP_B)
    {
        switch (mode)
        {
            case GPIO_INTR_OUTPUT:
                reg_val &= ~(1UL << (bit_addr+20));  
                break;
            case GPIO_INTR_INPUT:        
            case GPIO_DATA_INPUT:
            case GPIO_DATA_OUTPUT:
                reg_val |= (1UL << (bit_addr+20));  
                break;
            default:
                break;
        }
    }

    /* set direction      */
    switch (mode)
    {
        case GPIO_INTR_INPUT:
        case GPIO_DATA_INPUT:
            reg_val &= ~(1UL << bit_addr);
            break;
        case GPIO_INTR_OUTPUT:
        case GPIO_DATA_OUTPUT:
            reg_val |= (1UL << bit_addr);
            break;
        default:
            break;
    }
           
    reg_write(RTL8307H_UNIT, GPIO_FUNCSELECT_DIRECTR_DATA_REG, reg_val);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8307h_gpio_set
 * Description:
 *      Output specific level on the GPIO pin
 * Input:
 *      group                   - Group A, B
 *      io_port                 - (0, 1, 2, 3)
 *      value                   - (0:low, 1:high)
 * Output:     
 *      None
 * Return: 
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
uint16 rtl8307h_gpio_set(uint8 group, uint8 io_port, uint8 value)
{
    uint32 reg_val;
#ifdef	EXT_LAB
    uint8 bit_addr = 0;
#else
    uint8 bit_addr;
#endif

    if (group >= GPIO_GRP_C || io_port >= 4) 
        return RT_ERR_PORT_ID;
        
    if (group == GPIO_GRP_A)
        bit_addr = 12 + io_port;
    else if (group == GPIO_GRP_B)
        bit_addr = 16 + io_port;
        
    reg_read(RTL8307H_UNIT, GPIO_FUNCSELECT_DIRECTR_DATA_REG,  &reg_val);
    
    if (value == 0)
        reg_val &= ~(1UL << bit_addr);
    else
        reg_val |= (1UL << bit_addr);    

    reg_write(RTL8307H_UNIT, GPIO_FUNCSELECT_DIRECTR_DATA_REG, reg_val);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8307h_gpio_get
 * Description:
 *      Get the level on specified GPIO pin
 * Input:
 *      group                   - Group A, B
 *      io_port                 - (0, 1, 2, 3)
 * Output:     
 *      uint8                   - (0:low level, 1:high level)
 * Return: 
 *      None
 * Note:
 *      None
 */
uint8 rtl8307h_gpio_get(uint8 group, uint8 io_port)
{
    uint32 reg_val;
#ifdef	EXT_LAB
    uint8 bit_addr = 0;
#else
    uint8 bit_addr;
#endif

    if (group >= GPIO_GRP_C || io_port >= 4) 
        return RT_ERR_PORT_ID;
        
    if (group == GPIO_GRP_A)
        bit_addr = 12 + io_port;
    else if (group == GPIO_GRP_B)
        bit_addr = 16 + io_port;
        
    reg_read(RTL8307H_UNIT, GPIO_FUNCSELECT_DIRECTR_DATA_REG,  &reg_val);
    
    return ((reg_val >> bit_addr) & 0x1);
}

/* Function Name:
 *      rtl8307h_gpio_intr_clear
 * Description:
 *      Clear the pending interrupt on specific gpio
 * Input:
 *      group                   - Group A, B
 *      io_port                 - (0, 1, 2, 3)
 * Output:     
 *      None
 * Return: 
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
uint16 rtl8307h_gpio_intr_clear(uint8 group, uint8 io_port)
{
    uint32 reg_val;
#ifdef	EXT_LAB
    uint8 bit_addr = 0;
#else
    uint8 bit_addr;
#endif

    if (group >= GPIO_GRP_C || io_port >= 4) 
        return RT_ERR_PORT_ID;
        
    if (group == GPIO_GRP_A)
        bit_addr = 0 + io_port;
    else if (group == GPIO_GRP_B)
        bit_addr = 4 + io_port;
        
    reg_read(RTL8307H_UNIT, GPIO_INT_STATUS_MASK_MODE,  &reg_val);
    
    reg_val |= (1UL << bit_addr);    

    reg_write(RTL8307H_UNIT, GPIO_INT_STATUS_MASK_MODE, reg_val);

    return RT_ERR_OK;
}


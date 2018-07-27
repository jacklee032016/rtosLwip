#include <rtk_types.h>
#include <rtk_error.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>

#include <rtl8307h_debug.h>

#include <rtl8307h_asicdrv_lock.h>
/*
 *  This API is used before external CPU enters critical region which table access
 *  operation needs atomic operation 
 */
int32 rtl8307h_table_lock(void)
{  
#define TABLE_ACCESS_BUSY  1
#define TABLE_ACCESS_IDLE  0
    uint32 regval;

    DEBUG_INFO(DBG_LOCK, "Table Lock\n");

    /* Check whether external CPU has already get the resource, it indicates a software error
        due to external CPU doen't released the lock previously*/    
    reg_field_read(RTL8307H_UNIT, INDIRECT_OCCUPY_STATUS, EXTERNAL_CPU_OCCUPY, &regval);
    if (regval)
    {
        DEBUG_INFO(DBG_LOCK,"Lock Table Error: Relock by external CPU\n");
        return -1;            
    } 
    
    while (1)
    {
        reg_field_read(RTL8307H_UNIT, INDIRECT_OCCUPY_STATUS, INTERNAL_8051_OCCUPY, &regval);
        if (regval == TABLE_ACCESS_IDLE)
        {
            reg_field_write(RTL8307H_UNIT, INDIRECT_OCCUPY_STATUS, EXTERNAL_CPU_OCCUPY, 1);
    
            /* get the lock successfully */
            reg_field_read(RTL8307H_UNIT, INDIRECT_OCCUPY_STATUS, EXTERNAL_CPU_OCCUPY, &regval);
            if (regval == TABLE_ACCESS_BUSY)
                break;
            else
                DEBUG_INFO(DBG_LOCK, "the resource has been requested by internal CPU\n");
        }
    }

#ifdef	MUX_LAB
	return regval;
#endif	
}

/*
 *  This API is used after external leaves critical region 
 */
int32 rtl8307h_table_unlock(void)
{
    uint32 regval;

    DEBUG_INFO(DBG_LOCK, "Table UnLock\n");
    
    /* Judge whether internal cpu get the lock, 
      If so, the lock should acquired first by external CPU */

    reg_field_read(RTL8307H_UNIT, INDIRECT_OCCUPY_STATUS, INTERNAL_8051_OCCUPY, &regval);
    if (regval)
    {
        DEBUG_INFO(DBG_LOCK, "Unlock Table Error\n");
        return -1;
    }

    reg_write(RTL8307H_UNIT, INDIRECT_OCCUPY_STATUS, 0); 
#ifdef	MUX_LAB
	return regval;
#endif	
}

/*
 *  This API is used before external CPU enters critical region which when modify register
 *  bit fields operation needs atomic operation 
 */
void rtl8307h_reg_lock(void)
{   
#define REG_ACCESS_BUSY  1
#define REG_ACCESS_IDLE  0
    uint32 regval;    
    while (1)
    {
        reg_field_read(RTL8307H_UNIT, I2C_OCCUPY_STATUS, I2C_8051_OCCUPY, &regval);
        if (regval == REG_ACCESS_IDLE)
        {
            reg_field_write(RTL8307H_UNIT, I2C_OCCUPY_STATUS, I2C_CPU_OCCUPY, 1);
             /* get the lock successfully */
            reg_field_read(RTL8307H_UNIT, I2C_OCCUPY_STATUS, I2C_CPU_OCCUPY, &regval);
            if (regval == REG_ACCESS_BUSY)
                break;
            else
                DEBUG_INFO(DBG_REGVERIFY, "the resource has been requested by internal CPU\n");
            
        }
    }
}

/*
 *  This API is used after 8051 leaves critical region 
 */
void rtl8307h_reg_unlock(void)
{
    reg_write(RTL8307H_UNIT, I2C_OCCUPY_STATUS, 0); 
}

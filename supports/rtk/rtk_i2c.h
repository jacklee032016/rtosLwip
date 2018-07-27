
#ifndef __RTL8307H_I2C_H__
#define __RTL8307H_I2C_H__

#include <rtk_types.h>
#include <rtk_error.h>

/* Function Name:
 *     RTL8307H_I2C_init
 * Description:
 *     Initializtion. Normally, the GPIO of SDA and SCL in the master 
 *     should be configured as output
 * Input:
 *      None
 * Output:     
 *      None
 * Return: 
 *      None  
 * Note:
 *      None
 */
extern void RTL8307H_I2C_init(void);

/* Function Name:
 *      RTL8307H_I2C_READ
 * Description:
 *      Read the data with address switch_addr. 
 *      The MSB of *reg_val (i.e. (*reg_val >> 31) & 0x1) corresponds to bits 31:31
 *      in the register datasheet
 * Input:
 *      switch_addr               -  switch address
 * Output:     
 *      reg_val                   -  Register value
 * Return: 
 *      RT_ERR_OK  
 *      RT_ERR_NO_ACK
 * Note:
 *      None
 */
//frank extern int32 RTL8307H_I2C_READ(uint32 switch_addr, uint32 *reg_val);
int32 RTL8307H_I2C_READ(uint32 switch_addr, uint32 *reg_val);

/* Function Name:
 *      RTL8307H_I2C_WRITE
 * Description:
 *      Write the data at address switch_addr. 
 *      The MSB of reg_val (i.e. (*reg_val >> 31) & 0x1) corresponds to bits 31:31
 *      in the register datasheet
 * Input:
 *      switch_addr               -  switch address   
 *      reg_val                   -  Register value
 * Output:  
 *      None
 * Return: 
 *      RT_ERR_OK  
 *      RT_ERR_NO_ACK
 * Note:
 *      None
 */
//Frank extern int32 RTL8307H_I2C_WRITE(uint32 switch_addr, uint32 reg_val);
int32 RTL8307H_I2C_WRITE(uint32 switch_addr, uint32 reg_val);
#endif /* __RTL8307H_I2C_H__ */



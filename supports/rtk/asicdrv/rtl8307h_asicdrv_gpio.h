#ifndef _RTL8307H_ASICDRV_GPIO_H_
#define _RTL8307H_ASICDRV_GPIO_H_

#include <rtk_types.h>
#include <rtk_error.h>

typedef enum GPIO_Mode_e {
    GPIO_INTR_INPUT,
    GPIO_INTR_OUTPUT,
    GPIO_DATA_INPUT,
    GPIO_DATA_OUTPUT,
}GPIO_Mode_t;

typedef enum GPIO_Group_e {
    GPIO_GRP_A,
    GPIO_GRP_B,
    GPIO_GRP_C,
    GPIO_GRP_END,
}GPIO_Group_t;

typedef enum Intr_mode_e {
    INTR_LEVEL_LOW,
    INTR_LEVEL_HIGH,
    INTR_EDGE_LOW_TO_HIGH,
    INTR_EDGE_HIGH_TO_LOW,
}Intr_mode_t;


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
extern uint16 rtl8307h_gpio_mode_set(uint8 group, uint8 io_port, uint8 mode, Intr_mode_t intr_type);


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
extern uint16 rtl8307h_gpio_set(uint8 group, uint8 io_port, uint8 value);


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
extern uint8 rtl8307h_gpio_get(uint8 group, uint8 io_port);


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
extern uint16 rtl8307h_gpio_intr_clear(uint8 group, uint8 io_port);


#endif /*_RTL8307H_ASICDRV_GPIO_H_*/


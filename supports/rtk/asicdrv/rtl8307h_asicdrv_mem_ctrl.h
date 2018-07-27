#ifndef _RTL8307H_ASICDRV_MEM_CTRL_H_
#define _RTL8307H_ASICDRV_MEM_CTRL_H_

#include <rtk_types.h>
#include <rtk_error.h>

#define REG_READ      0
#define REG_WRITE     1
#define REG_TRIGGER 0x2

#define SPI_FLASH_WIP 0x1
#define SPI_FLASH_WEL 0x2

/* SPI Cmd Word */
#define WRITE_ENABLE    0x6
#define WRITE_DISABLE   0x4
#define READ_IDENTITY   0x9f
#define READ_STATUS     0x5
#define READ            0x3
#define FAST_READ       0xB
#define SECTOR_ERASE    0x20
#define BLOCK_ERASE     0xD8
#define CHIP_ERASE      0xC7 
#define PAGE_PROGRAM    0x02

typedef struct spi_cmd_s{
    uint8 cmd_word;
    uint8 skip_addr;
    uint8 direction;
    uint8 byte_num;
    uint8 dummy_num;
    uint8 data_com_num;
    uint8 addr_com_num; 
}spi_cmd_t;

extern uint32 rtl8307h_memCtrl_switch_reg_get(uint32 addr, uint32 *val);
extern uint32 rtl8307h_memCtrl_switch_reg_set(uint32 addr, uint32 val);

extern uint32 rtl8307h_memCtrl_sram_get(uint16 addr, uint32 *val_high, uint32 *val_low);
extern uint32 rtl8307h_memCtrl_sram_set(uint16 addr, uint32 val_high, uint32 val_low);
extern uint32 rtl8307h_memCtrl_sram_dump(uint32 start, uint32 end);
extern uint32 rtl8307h_memCtrl_sram_get2(uint16 addr, uint8 *buf, uint16 len);
extern uint32 rtl8307h_memCtrl_sram_set2(uint16 addr, uint8 *buf, uint16 len);

extern uint32 rtl8307h_memCtrl_memcpy(uint16 dest, uint16 src, uint16 len);

extern uint32 rtl8307h_memCtrl_iorm_get(uint32 addr, uint8 *val);
extern uint32 rtl8307h_memCtrl_iorm_set(uint32 addr, uint8 val);
extern void rtl8307h_memCtrl_iorm_set_4bytes(uint32 addr, uint32 val);

extern uint8 spi_flash_read_single(uint32 start_adr, uint8 * buf, uint32 read_len);
extern uint8 spi_flash_write_single(uint32 start_adr, uint8 *buf, uint32 write_len);

extern uint8 spi_flash_write_huge(uint32 start_adr, uint8 *buf, uint32 write_len);
extern uint8 spi_flash_read_huge(uint32 start_adr, uint8 * buf, uint32 read_len);

extern uint8 spi_flash_erase(uint32 erase_adr, uint8 flag);
extern uint8 spi_chip_erase(void);

extern uint8 __spi_flash_readSR(void);
extern void __spi_flash_waitWEL(void);
extern uint32 clear_spi_protection(void);
extern uint32 spi_write_file(int8 * fileName);
extern uint32 set_cmd(spi_cmd_t* param);

#endif /*_RTL8307H_ASICDRV_MEM_CTRL_H_*/



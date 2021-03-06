#include <rtk_types.h>
#include <rtk_error.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>

#include <rtl8307h_asicdrv.h>
#include <rtk_irom_init.h>

#define IROM_WR_CMD_MASK        0x1
#define IROM_WR_TRIGGER_MASK    0x2

const unsigned char RTL8307H_init_0_1_8_bin[] = {
0x02, 0xe1, 0x71, 0x02, 0xc3, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xea, 0xab, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x02, 0xaa, 0x71, 0x90, 0x7f, 0x4c, 0xe0, 0x75, 0x60, 0x00, 0xf5, 0x61, 0x24, 
0x03, 0xa3, 0xf0, 0xe5, 0x61, 0x64, 0x01, 0x45, 0x60, 0x70, 0x03, 0x02, 0x05, 0xc9, 0xe5, 0x61, 
0x64, 0x02, 0x45, 0x60, 0x70, 0x03, 0x02, 0x06, 0xc5, 0xe5, 0x61, 0x64, 0x03, 0x45, 0x60, 0x70, 
0x03, 0x02, 0x04, 0x2e, 0xe5, 0x61, 0x64, 0x04, 0x45, 0x60, 0x70, 0x03, 0x02, 0x0b, 0x2a, 0xe5, 
0x61, 0x64, 0x05, 0x45, 0x60, 0x70, 0x03, 0x02, 0x0b, 0x32, 0xe5, 0x61, 0x64, 0x06, 0x45, 0x60, 
0x70, 0x03, 0x02, 0x0b, 0x3a, 0xe5, 0x61, 0x64, 0x07, 0x45, 0x60, 0x70, 0x03, 0x12, 0x0b, 0x42, 
0x22, 0xe4, 0x90, 0x3c, 0x0e, 0xf0, 0xa3, 0xf0, 0x90, 0x3c, 0x29, 0xe0, 0xd3, 0x94, 0x01, 0x40, 
0x03, 0x02, 0x04, 0x2d, 0x7f, 0x00, 0x7e, 0x00, 0x7d, 0x01, 0x7c, 0xbb, 0x12, 0x0b, 0x09, 0x90, 
0x3c, 0x0a, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x0a, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 
0xa3, 0xe0, 0xff, 0x78, 0x03, 0x12, 0x0a, 0x71, 0xef, 0x54, 0x07, 0xff, 0xe4, 0xfe, 0xfd, 0xfc, 
0x90, 0x3c, 0x0a, 0x12, 0x0a, 0x97, 0xe4, 0x7f, 0x06, 0xfe, 0xfd, 0xfc, 0x90, 0x3c, 0x0a, 0xe0, 
0xf8, 0xa3, 0xe0, 0xf9, 0xa3, 0xe0, 0xfa, 0xa3, 0xe0, 0xfb, 0xc3, 0x12, 0x0a, 0x60, 0x70, 0x05, 
0x90, 0x3c, 0x0e, 0x04, 0xf0, 0x90, 0x3c, 0x28, 0xe0, 0x64, 0x07, 0x70, 0x72, 0xff, 0xfe, 0x7d, 
0x01, 0x7c, 0xbb, 0x12, 0x0b, 0x09, 0x90, 0x3c, 0x0a, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x0a, 0xe0, 
0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x78, 0x03, 0x12, 0x0a, 0x71, 0xef, 
0x54, 0x07, 0xff, 0xe4, 0xfe, 0xfd, 0xfc, 0x90, 0x3c, 0x0a, 0x12, 0x0a, 0x97, 0xe4, 0x7f, 0x06, 
0xfe, 0xfd, 0xfc, 0x90, 0x3c, 0x0a, 0xe0, 0xf8, 0xa3, 0xe0, 0xf9, 0xa3, 0xe0, 0xfa, 0xa3, 0xe0, 
0xfb, 0xc3, 0x12, 0x0a, 0x60, 0x70, 0x07, 0x90, 0x3c, 0x0e, 0x04, 0xf0, 0x80, 0x2e, 0xe4, 0x7f, 
0x07, 0xfe, 0xfd, 0xfc, 0x90, 0x3c, 0x0a, 0xe0, 0xf8, 0xa3, 0xe0, 0xf9, 0xa3, 0xe0, 0xfa, 0xa3, 
0xe0, 0xfb, 0xc3, 0x12, 0x0a, 0x60, 0x70, 0x14, 0x90, 0x3c, 0x0f, 0x04, 0xf0, 0x80, 0x0d, 0x90, 
0x3c, 0x28, 0xe0, 0xb4, 0x06, 0x06, 0x90, 0x3c, 0x0f, 0x74, 0x01, 0xf0, 0x90, 0x3c, 0x29, 0xe0, 
0xb4, 0x01, 0x4d, 0x7f, 0x00, 0x7e, 0x00, 0x7d, 0x06, 0x7c, 0x00, 0x12, 0x0b, 0x09, 0x90, 0x3c, 
0x0a, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x0a, 0xe0, 0xfc, 0xa3, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 
0xee, 0x44, 0x80, 0xfe, 0x74, 0xff, 0xfd, 0xec, 0x44, 0x07, 0xfc, 0x90, 0x3c, 0x0a, 0x12, 0x0a, 
0x97, 0x90, 0x3c, 0x0a, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x90, 
0x3c, 0x1e, 0x12, 0x0a, 0x97, 0x7f, 0x00, 0x7e, 0x00, 0x7d, 0x06, 0x7c, 0x00, 0x12, 0x0a, 0xd4, 
0xe4, 0x90, 0x3c, 0x10, 0xf0, 0x90, 0x3c, 0x10, 0xe0, 0xff, 0xc3, 0x94, 0x07, 0x50, 0x4a, 0x90, 
0x3c, 0x28, 0xe0, 0xb4, 0x05, 0x0d, 0xef, 0x64, 0x02, 0x60, 0x36, 0x90, 0x3c, 0x10, 0xe0, 0x64, 
0x03, 0x60, 0x2e, 0x90, 0x3c, 0x10, 0xe0, 0xff, 0x64, 0x04, 0x60, 0x25, 0xef, 0x64, 0x05, 0x60, 
0x20, 0xef, 0xb4, 0x06, 0x07, 0x90, 0x3c, 0x0e, 0xe0, 0xb4, 0x01, 0x15, 0x90, 0x3c, 0x10, 0xe0, 
0xff, 0x90, 0x3c, 0x14, 0xe4, 0xf0, 0xa3, 0x74, 0x05, 0xf0, 0x7b, 0x1d, 0x7d, 0x01, 0x12, 0x08, 
0x99, 0x90, 0x3c, 0x10, 0xe0, 0x04, 0xf0, 0x80, 0xac, 0x90, 0x3c, 0x1e, 0x12, 0x0a, 0xa3, 0x00, 
0x03, 0x03, 0xfd, 0x7f, 0x10, 0x7e, 0x00, 0x7d, 0x04, 0x7c, 0x00, 0x12, 0x0a, 0xd4, 0x90, 0x3c, 
0x14, 0xe4, 0xf0, 0xa3, 0x74, 0x46, 0xf0, 0x7b, 0x1e, 0x7d, 0x07, 0x7f, 0x04, 0x12, 0x08, 0x99, 
0x74, 0x77, 0x90, 0x3c, 0x14, 0xf0, 0xa3, 0xf0, 0x7b, 0x16, 0x7d, 0x07, 0x7f, 0x04, 0x12, 0x08, 
0x99, 0x90, 0x3c, 0x14, 0xe4, 0xf0, 0xa3, 0x74, 0x07, 0xf0, 0x7b, 0x1f, 0xe4, 0xfd, 0x7f, 0x04, 
0x12, 0x08, 0x99, 0x90, 0x3c, 0x14, 0xe4, 0xf0, 0xa3, 0x74, 0x20, 0xf0, 0x7b, 0x1e, 0x7d, 0x1f, 
0x7f, 0x04, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x14, 0x74, 0x11, 0xf0, 0xa3, 0xe4, 0xf0, 0x7b, 0x15, 
0x7d, 0x1f, 0x7f, 0x04, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x14, 0x74, 0xa0, 0xf0, 0xa3, 0x74, 0x3e, 
0xf0, 0x7b, 0x1b, 0x7d, 0x1f, 0x7f, 0x04, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x14, 0xe4, 0xf0, 0xa3, 
0x74, 0x78, 0xf0, 0x7b, 0x1e, 0x7d, 0x07, 0x7f, 0x04, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x14, 0xe4, 
0xf0, 0xa3, 0x74, 0x55, 0xf0, 0x7b, 0x17, 0x7d, 0x1f, 0x7f, 0x04, 0x12, 0x08, 0x99, 0x90, 0x3c, 
0x14, 0xe4, 0xf0, 0xa3, 0x74, 0xaa, 0xf0, 0x7b, 0x19, 0x7d, 0x1f, 0x7f, 0x04, 0x12, 0x08, 0x99, 
0xe4, 0x90, 0x3c, 0x14, 0xf0, 0xa3, 0xf0, 0x7b, 0x1f, 0xfd, 0x7f, 0x04, 0x12, 0x08, 0x99, 0x90, 
0x3c, 0x1e, 0x12, 0x0a, 0xa3, 0x00, 0x03, 0x03, 0xfc, 0x7f, 0x10, 0x7e, 0x00, 0x7d, 0x04, 0x7c, 
0x00, 0x12, 0x0a, 0xd4, 0xe4, 0x90, 0x3c, 0x10, 0xf0, 0x90, 0x3c, 0x10, 0xe0, 0xff, 0xc3, 0x94, 
0x07, 0x40, 0x03, 0x02, 0x03, 0xef, 0x90, 0x3c, 0x28, 0xe0, 0xb4, 0x05, 0x13, 0xef, 0x64, 0x02, 
0x70, 0x03, 0x02, 0x03, 0xe6, 0x90, 0x3c, 0x10, 0xe0, 0x64, 0x03, 0x70, 0x03, 0x02, 0x03, 0xe6, 
0x90, 0x3c, 0x10, 0xe0, 0xff, 0x64, 0x04, 0x70, 0x03, 0x02, 0x03, 0xe6, 0xef, 0x64, 0x05, 0x70, 
0x03, 0x02, 0x03, 0xe6, 0xef, 0xb4, 0x06, 0x0b, 0x90, 0x3c, 0x0e, 0xe0, 0x64, 0x01, 0x60, 0x03, 
0x02, 0x03, 0xe6, 0x90, 0x3c, 0x10, 0xe0, 0xff, 0x90, 0x3c, 0x14, 0x74, 0x12, 0xf0, 0xa3, 0x74, 
0x1f, 0xf0, 0x7b, 0x1d, 0x7d, 0x18, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x10, 0xe0, 0xff, 0x90, 0x3c, 
0x14, 0x74, 0x02, 0xf0, 0xa3, 0x74, 0x1f, 0xf0, 0x7b, 0x17, 0x7d, 0x19, 0x12, 0x08, 0x99, 0x90, 
0x3c, 0x10, 0xe0, 0xff, 0x90, 0x3c, 0x14, 0x74, 0x30, 0xf0, 0xa3, 0x74, 0x88, 0xf0, 0x7b, 0x1c, 
0x7d, 0x01, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x10, 0xe0, 0xff, 0x90, 0x3c, 0x14, 0x74, 0x40, 0xf0, 
0xa3, 0x74, 0xc2, 0xf0, 0x7b, 0x1c, 0xe4, 0xfd, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x10, 0xe0, 0xff, 
0x90, 0x3c, 0x14, 0x74, 0x5a, 0xf0, 0xa3, 0x74, 0x10, 0xf0, 0x7b, 0x11, 0x7d, 0x01, 0x12, 0x08, 
0x99, 0x90, 0x3c, 0x10, 0xe0, 0xff, 0x90, 0x3c, 0x14, 0xe4, 0xf0, 0xa3, 0x74, 0x25, 0xf0, 0x7b, 
0x1d, 0x7d, 0x01, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x10, 0xe0, 0x04, 0xf0, 0x02, 0x03, 0x19, 0x90, 
0x3c, 0x0f, 0xe0, 0x64, 0x01, 0x70, 0x36, 0x90, 0x3c, 0x14, 0xf0, 0xa3, 0x74, 0x05, 0xf0, 0x7b, 
0x1d, 0x7d, 0x01, 0x7f, 0x06, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x14, 0x74, 0x83, 0xf0, 0xa3, 0x74, 
0x88, 0xf0, 0x7b, 0x1c, 0x7d, 0x01, 0x7f, 0x06, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x14, 0x74, 0x88, 
0xf0, 0xa3, 0xe4, 0xf0, 0x7b, 0x13, 0x7d, 0x01, 0x7f, 0x06, 0x12, 0x08, 0x99, 0x22, 0xe4, 0x90, 
0x3c, 0x08, 0xf0, 0xa3, 0xf0, 0x90, 0x3c, 0x09, 0xe0, 0xff, 0xc3, 0x94, 0x07, 0x50, 0x21, 0xef, 
0x75, 0xf0, 0x04, 0xa4, 0xff, 0xef, 0x24, 0x20, 0xff, 0xe4, 0x35, 0xf0, 0xfe, 0xe4, 0x34, 0x04, 
0xfd, 0xe4, 0x34, 0xbb, 0xfc, 0x12, 0x0b, 0x09, 0x90, 0x3c, 0x09, 0xe0, 0x04, 0xf0, 0x80, 0xd5, 
0x90, 0x3c, 0x04, 0x12, 0x0a, 0xa3, 0x00, 0x00, 0x00, 0x00, 0xe4, 0x7f, 0xc0, 0xfe, 0xfd, 0xfc, 
0x90, 0x3c, 0x04, 0xe0, 0xf8, 0xa3, 0xe0, 0xf9, 0xa3, 0xe0, 0xfa, 0xa3, 0xe0, 0xfb, 0xc3, 0x12, 
0x0a, 0x60, 0x50, 0x21, 0x90, 0x3c, 0x04, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 
0xe0, 0x24, 0x01, 0xff, 0xe4, 0x3e, 0xfe, 0xe4, 0x3d, 0xfd, 0xe4, 0x3c, 0xfc, 0x90, 0x3c, 0x04, 
0x12, 0x0a, 0x97, 0x80, 0xc5, 0x90, 0x3c, 0x29, 0xe0, 0xd3, 0x94, 0x01, 0x40, 0x03, 0x02, 0x05, 
0xc8, 0x90, 0x3c, 0x28, 0xe0, 0xb4, 0x07, 0x3c, 0x7f, 0x00, 0x7e, 0x00, 0x7d, 0x01, 0x7c, 0xbb, 
0x12, 0x0b, 0x09, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x00, 0xe0, 0xfc, 0xa3, 0xe0, 
0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x78, 0x03, 0x12, 0x0a, 0x71, 0xef, 0x54, 0x07, 0xff, 
0xe4, 0xfe, 0xfd, 0xfc, 0x7b, 0x06, 0xfa, 0xf9, 0xf8, 0xc3, 0x12, 0x0a, 0x60, 0x70, 0x05, 0x90, 
0x3c, 0x08, 0x04, 0xf0, 0xe4, 0x90, 0x3c, 0x09, 0xf0, 0x90, 0x3c, 0x09, 0xe0, 0xff, 0xc3, 0x94, 
0x07, 0x40, 0x03, 0x02, 0x05, 0xc8, 0x90, 0x3c, 0x28, 0xe0, 0xb4, 0x05, 0x13, 0xef, 0x64, 0x02, 
0x70, 0x03, 0x02, 0x05, 0xbf, 0x90, 0x3c, 0x09, 0xe0, 0x64, 0x03, 0x70, 0x03, 0x02, 0x05, 0xbf, 
0x90, 0x3c, 0x09, 0xe0, 0xff, 0x64, 0x04, 0x70, 0x03, 0x02, 0x05, 0xbf, 0xef, 0x64, 0x05, 0x70, 
0x03, 0x02, 0x05, 0xbf, 0xef, 0xb4, 0x06, 0x08, 0x90, 0x3c, 0x08, 0xe0, 0x64, 0x01, 0x70, 0x7f, 
0x90, 0x3c, 0x09, 0xe0, 0x75, 0xf0, 0x04, 0xa4, 0xff, 0xef, 0x24, 0x20, 0xff, 0xe4, 0x35, 0xf0, 
0xfe, 0xe4, 0x34, 0x04, 0xfd, 0xe4, 0x34, 0xbb, 0xfc, 0x12, 0x0b, 0x09, 0x90, 0x3c, 0x00, 0x12, 
0x0a, 0x97, 0x90, 0x3c, 0x00, 0xa3, 0xa3, 0xa3, 0xe0, 0x54, 0x01, 0xff, 0xe4, 0xfe, 0xfc, 0xef, 
0x90, 0x3c, 0x09, 0x60, 0x21, 0xe0, 0xff, 0x90, 0x3c, 0x14, 0x74, 0x10, 0xf0, 0xa3, 0x74, 0x18, 
0xf0, 0x7b, 0x1d, 0xfd, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x09, 0xe0, 0xff, 0x90, 0x3c, 0x14, 0xe4, 
0xf0, 0xa3, 0x74, 0x18, 0x80, 0x21, 0xe0, 0xff, 0x90, 0x3c, 0x14, 0x74, 0x12, 0xf0, 0xa3, 0x74, 
0x1f, 0xf0, 0x7b, 0x1d, 0x7d, 0x18, 0x12, 0x08, 0x99, 0x90, 0x3c, 0x09, 0xe0, 0xff, 0x90, 0x3c, 
0x14, 0x74, 0x02, 0xf0, 0xa3, 0x74, 0x1f, 0xf0, 0x7b, 0x17, 0x7d, 0x19, 0x12, 0x08, 0x99, 0x90, 
0x3c, 0x09, 0xe0, 0x04, 0xf0, 0x02, 0x04, 0xf9, 0x22, 0x90, 0x7f, 0xc8, 0xe0, 0x44, 0x01, 0xf0, 
0x90, 0x3c, 0x1e, 0x12, 0x0a, 0xa3, 0x00, 0x00, 0x00, 0x01, 0x7f, 0x00, 0x7e, 0x00, 0x7d, 0x68, 
0x7c, 0x00, 0x12, 0x0a, 0xd4, 0x12, 0x07, 0xbb, 0x90, 0x3c, 0x29, 0xe0, 0xd3, 0x94, 0x01, 0x40, 
0x03, 0x02, 0x06, 0xc4, 0xe4, 0x90, 0x3c, 0x04, 0xf0, 0xe4, 0x90, 0x3c, 0x05, 0xf0, 0x90, 0x3c, 
0x04, 0xe0, 0xfe, 0xe4, 0x24, 0x08, 0xff, 0xe4, 0x3e, 0xfe, 0xe4, 0x34, 0xb5, 0xfd, 0xe4, 0x34, 
0xbb, 0xfc, 0xa8, 0x04, 0xa9, 0x05, 0xaa, 0x06, 0xab, 0x07, 0xa3, 0xe0, 0x75, 0xf0, 0x0c, 0xa4, 
0xff, 0xe4, 0xfc, 0xfd, 0xeb, 0x2f, 0xff, 0xea, 0x35, 0xf0, 0xfe, 0xed, 0x39, 0xfd, 0xec, 0x38, 
0xfc, 0xa3, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x06, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 
0xa3, 0xe0, 0xff, 0x12, 0x0b, 0x09, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x00, 0xe0, 
0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0x54, 0x7f, 0xff, 0xee, 0x54, 0xe0, 0xfe, 
0xec, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x00, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 
0xe0, 0xfe, 0xa3, 0xe0, 0x44, 0x80, 0xff, 0xec, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 
0x00, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x90, 0x3c, 0x1e, 0x12, 
0x0a, 0x97, 0x90, 0x3c, 0x06, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 
0x12, 0x0a, 0xd4, 0x90, 0x3c, 0x05, 0xe0, 0x04, 0xf0, 0xe0, 0xc3, 0x94, 0x04, 0x50, 0x03, 0x02, 
0x05, 0xfe, 0x90, 0x3c, 0x04, 0xe0, 0x04, 0xf0, 0xe0, 0xc3, 0x94, 0x08, 0x50, 0x03, 0x02, 0x05, 
0xf9, 0x12, 0x00, 0xa1, 0x22, 0x90, 0x3c, 0x29, 0xe0, 0xd3, 0x94, 0x01, 0x40, 0x03, 0x02, 0x07, 
0xba, 0x90, 0x7f, 0xc8, 0xe0, 0x44, 0x02, 0xf0, 0x7f, 0x10, 0x7e, 0x00, 0x7d, 0x51, 0x7c, 0xbb, 
0x12, 0x0b, 0x09, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x00, 0xe0, 0xfc, 0xa3, 0xe0, 
0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x78, 0x10, 0x12, 0x0a, 0x71, 0xef, 0x54, 0x7f, 0xff, 
0xe4, 0x90, 0x2e, 0xe0, 0xef, 0xf0, 0x7f, 0x10, 0x7e, 0x00, 0x7d, 0x29, 0x7c, 0xbb, 0x12, 0x0b, 
0x09, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x00, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 
0xe0, 0xfe, 0xa3, 0xe0, 0x54, 0x01, 0xff, 0xee, 0x54, 0xfe, 0xfe, 0xec, 0x90, 0x3c, 0x00, 0x12, 
0x0a, 0x97, 0x90, 0x3c, 0x00, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0x44, 
0x64, 0xff, 0xec, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x00, 0xe0, 0xfc, 0xa3, 0xe0, 
0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x90, 0x3c, 0x1e, 0x12, 0x0a, 0x97, 0x7f, 0x10, 0x7e, 
0x00, 0x7d, 0x29, 0x7c, 0xbb, 0x12, 0x0a, 0xd4, 0x7f, 0x0c, 0x7e, 0x00, 0x7d, 0x1b, 0x7c, 0xbb, 
0x12, 0x0b, 0x09, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x00, 0xa3, 0xe0, 0xfd, 0xa3, 
0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0xe4, 0xfc, 0x90, 0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x00, 
0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0xec, 0x44, 0x14, 0xfc, 0x90, 
0x3c, 0x00, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x1e, 0x12, 0x0a, 0xa3, 0x00, 0x00, 0x00, 0x14, 0x7f, 
0x0c, 0x7e, 0x00, 0x7d, 0x1b, 0x7c, 0xbb, 0x12, 0x0a, 0xd4, 0x22, 0x90, 0x3c, 0x1e, 0x12, 0x0a, 
0xa3, 0x00, 0x00, 0x00, 0x01, 0x7f, 0x34, 0x7e, 0x00, 0x7d, 0x06, 0x7c, 0xbb, 0x12, 0x0a, 0xd4, 
0x7f, 0x00, 0x7e, 0x00, 0x7d, 0x01, 0x7c, 0xbb, 0x12, 0x0b, 0x09, 0x78, 0x0b, 0x12, 0x0a, 0x71, 
0xef, 0x54, 0x07, 0xff, 0xe4, 0xfe, 0xfd, 0xfc, 0x90, 0x3c, 0x0a, 0x12, 0x0a, 0x97, 0xe4, 0x7f, 
0x06, 0xfe, 0xfd, 0xfc, 0x90, 0x3c, 0x0a, 0xe0, 0xf8, 0xa3, 0xe0, 0xf9, 0xa3, 0xe0, 0xfa, 0xa3, 
0xe0, 0xfb, 0xc3, 0x12, 0x0a, 0x60, 0x70, 0x08, 0x90, 0x3c, 0x28, 0x74, 0x06, 0xf0, 0x80, 0x5b, 
0xe4, 0x7f, 0x07, 0xfe, 0xfd, 0xfc, 0x90, 0x3c, 0x0a, 0xe0, 0xf8, 0xa3, 0xe0, 0xf9, 0xa3, 0xe0, 
0xfa, 0xa3, 0xe0, 0xfb, 0xc3, 0x12, 0x0a, 0x60, 0x70, 0x41, 0x7f, 0x2c, 0xfe, 0x7d, 0x06, 0x7c, 
0xbb, 0x12, 0x0b, 0x09, 0x78, 0x17, 0x12, 0x0a, 0x71, 0xef, 0x54, 0x01, 0xff, 0xe4, 0xfe, 0xfd, 
0xfc, 0x90, 0x3c, 0x0a, 0x12, 0x0a, 0x97, 0xe4, 0xff, 0xfe, 0xfd, 0xfc, 0x90, 0x3c, 0x0a, 0xe0, 
0xf8, 0xa3, 0xe0, 0xf9, 0xa3, 0xe0, 0xfa, 0xa3, 0xe0, 0xfb, 0xc3, 0x12, 0x0a, 0x60, 0x90, 0x3c, 
0x28, 0x70, 0x05, 0x74, 0x05, 0xf0, 0x80, 0x03, 0x74, 0x07, 0xf0, 0x7f, 0xfc, 0x7e, 0xff, 0x7d, 
0x06, 0x7c, 0x00, 0x12, 0x0b, 0x09, 0x90, 0x3c, 0x0a, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x0a, 0xe0, 
0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x78, 0x0c, 0x12, 0x0a, 0x71, 0xef, 
0x54, 0x0f, 0xff, 0x90, 0x3c, 0x29, 0xef, 0xf0, 0x22, 0x90, 0x3c, 0x11, 0xef, 0xf0, 0xa3, 0xed, 
0xf0, 0xa3, 0xeb, 0xf0, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0xe4, 0xfc, 0xfd, 0x78, 0x10, 0x12, 
0x0a, 0x84, 0xa8, 0x04, 0xa9, 0x05, 0xaa, 0x06, 0xab, 0x07, 0x90, 0x3c, 0x11, 0xe0, 0xc4, 0x33, 
0x54, 0xe0, 0xfe, 0xeb, 0xff, 0xea, 0x4e, 0xfe, 0xe9, 0xfd, 0xe8, 0xfc, 0xa8, 0x04, 0xa9, 0x05, 
0xaa, 0x06, 0xab, 0x07, 0x90, 0x3c, 0x13, 0xe0, 0xfe, 0xeb, 0xff, 0xea, 0x4e, 0xfe, 0xe9, 0xfd, 
0xe8, 0xfc, 0xa8, 0x04, 0xa9, 0x05, 0xaa, 0x06, 0xab, 0x07, 0x90, 0x3c, 0x12, 0xe0, 0x75, 0xf0, 
0x08, 0xa4, 0xff, 0xae, 0xf0, 0xee, 0x33, 0x95, 0xe0, 0xfd, 0xfc, 0xeb, 0x4f, 0xff, 0xea, 0x4e, 
0xfe, 0xe9, 0x4d, 0xfd, 0xe8, 0x4c, 0xfc, 0xef, 0x44, 0x01, 0xff, 0xec, 0x90, 0x3c, 0x16, 0x12, 
0x0a, 0x97, 0x90, 0x3c, 0x16, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 
0x90, 0x3c, 0x1e, 0x12, 0x0a, 0x97, 0x7f, 0x44, 0x7e, 0x00, 0x7d, 0x04, 0x7c, 0xbb, 0x12, 0x0a, 
0xd4, 0x7f, 0x44, 0x7e, 0x00, 0x7d, 0x04, 0x7c, 0xbb, 0x12, 0x0b, 0x09, 0x90, 0x3c, 0x16, 0x12, 
0x0a, 0x97, 0x90, 0x3c, 0x16, 0xa3, 0xa3, 0xa3, 0xe0, 0x54, 0x01, 0xff, 0xe4, 0xfe, 0xfd, 0xfc, 
0xfb, 0xfa, 0xf9, 0xf8, 0xc3, 0x12, 0x0a, 0x60, 0x70, 0xd7, 0x22, 0x90, 0x3c, 0x1e, 0xee, 0xf0, 
0xa3, 0xef, 0xf0, 0x90, 0x3c, 0x1e, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xf5, 0x82, 0x8c, 0x83, 0xa3, 
0xe0, 0xfe, 0x8d, 0x82, 0x8c, 0x83, 0xe0, 0xfd, 0xed, 0xff, 0xe4, 0xfc, 0xfd, 0xc0, 0x04, 0xc0, 
0x05, 0xc0, 0x06, 0xc0, 0x07, 0x90, 0x3c, 0x1e, 0xe0, 0xfe, 0xa3, 0xe0, 0xf5, 0x82, 0x8e, 0x83, 
0xa3, 0xa3, 0xe0, 0xff, 0xe4, 0xfe, 0x78, 0x10, 0x12, 0x0a, 0x84, 0xd0, 0x03, 0xd0, 0x02, 0xd0, 
0x01, 0xd0, 0x00, 0xef, 0x4b, 0xff, 0xee, 0x4a, 0xfe, 0xed, 0x49, 0xfd, 0xec, 0x48, 0xfc, 0xc0, 
0x04, 0xc0, 0x05, 0xc0, 0x06, 0xc0, 0x07, 0x90, 0x3c, 0x1e, 0xe0, 0xfe, 0xa3, 0xe0, 0xf5, 0x82, 
0x8e, 0x83, 0xa3, 0xa3, 0xa3, 0xe0, 0xff, 0xe4, 0xfc, 0xfd, 0xfe, 0x78, 0x18, 0x12, 0x0a, 0x84, 
0xd0, 0x03, 0xd0, 0x02, 0xd0, 0x01, 0xd0, 0x00, 0xef, 0x4b, 0xff, 0xee, 0x4a, 0xfe, 0xed, 0x49, 
0xfd, 0xec, 0x48, 0xfc, 0x22, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 
0x90, 0x3c, 0x24, 0x12, 0x0a, 0x97, 0x7f, 0x24, 0x7e, 0x7f, 0xab, 0x07, 0xaa, 0x06, 0x90, 0x3c, 
0x24, 0xa3, 0xa3, 0xa3, 0xe0, 0x8b, 0x82, 0x8a, 0x83, 0xf0, 0x90, 0x3c, 0x24, 0xe0, 0xfc, 0xa3, 
0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x78, 0x08, 0x12, 0x0a, 0x71, 0x8b, 0x82, 0x8a, 
0x83, 0xa3, 0xef, 0xf0, 0x90, 0x3c, 0x24, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 
0xe0, 0xff, 0x78, 0x10, 0x12, 0x0a, 0x71, 0x8b, 0x82, 0x8a, 0x83, 0xa3, 0xa3, 0xef, 0xf0, 0x90, 
0x3c, 0x24, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x78, 0x18, 0x12, 
0x0a, 0x71, 0xac, 0x02, 0xad, 0x03, 0x8d, 0x82, 0x8c, 0x83, 0xa3, 0xa3, 0xa3, 0xef, 0xf0, 0x22, 
0xeb, 0x9f, 0xf5, 0xf0, 0xea, 0x9e, 0x42, 0xf0, 0xe9, 0x9d, 0x42, 0xf0, 0xe8, 0x9c, 0x45, 0xf0, 
0x22, 0xe8, 0x60, 0x0f, 0xec, 0xc3, 0x13, 0xfc, 0xed, 0x13, 0xfd, 0xee, 0x13, 0xfe, 0xef, 0x13, 
0xff, 0xd8, 0xf1, 0x22, 0xe8, 0x60, 0x0f, 0xef, 0xc3, 0x33, 0xff, 0xee, 0x33, 0xfe, 0xed, 0x33, 
0xfd, 0xec, 0x33, 0xfc, 0xd8, 0xf1, 0x22, 0xec, 0xf0, 0xa3, 0xed, 0xf0, 0xa3, 0xee, 0xf0, 0xa3, 
0xef, 0xf0, 0x22, 0xa8, 0x82, 0x85, 0x83, 0xf0, 0xd0, 0x83, 0xd0, 0x82, 0x12, 0x0a, 0xba, 0x12, 
0x0a, 0xba, 0x12, 0x0a, 0xba, 0x12, 0x0a, 0xba, 0xe4, 0x73, 0xe4, 0x93, 0xa3, 0xc5, 0x83, 0xc5, 
0xf0, 0xc5, 0x83, 0xc8, 0xc5, 0x82, 0xc8, 0xf0, 0xa3, 0xc5, 0x83, 0xc5, 0xf0, 0xc5, 0x83, 0xc8, 
0xc5, 0x82, 0xc8, 0x22, 0x90, 0x3c, 0x1a, 0x12, 0x0a, 0x97, 0x90, 0x3c, 0x1a, 0x12, 0x09, 0xe5, 
0x90, 0x3c, 0x1e, 0xe0, 0xfc, 0xa3, 0xe0, 0xfd, 0xa3, 0xe0, 0xfe, 0xa3, 0xe0, 0xff, 0x90, 0x3c, 
0x24, 0x12, 0x0a, 0x97, 0x7f, 0x28, 0x7e, 0x7f, 0x12, 0x09, 0xfa, 0x90, 0x7f, 0x2c, 0x74, 0x03, 
0xf0, 0x90, 0x7f, 0x2c, 0xe0, 0x20, 0xe1, 0xf9, 0x22, 0x90, 0x3c, 0x1a, 0x12, 0x0a, 0x97, 0x90, 
0x3c, 0x1a, 0x12, 0x09, 0xe5, 0x90, 0x7f, 0x2c, 0x74, 0x02, 0xf0, 0x90, 0x7f, 0x2c, 0xe0, 0x20, 
0xe1, 0xf9, 0x7f, 0x28, 0x7e, 0x7f, 0x12, 0x09, 0x5b, 0x22, 0x90, 0x7f, 0xc8, 0xe0, 0x44, 0x08, 
0xf0, 0x22, 0x90, 0x7f, 0xc8, 0xe0, 0x44, 0x10, 0xf0, 0x22, 0x90, 0x7f, 0xc8, 0xe0, 0x44, 0x20, 
0xf0, 0x22, 0x90, 0x7f, 0xc8, 0xe0, 0x44, 0x40, 0xf0, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

/* Function Name:
 *      iorm_set
 * Description:
 *      Set IROM at address addr with content val
 * Input:
 *      addr              -  IROM address
 *      val               -  content 
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      
 */
static void iorm_set(uint16 addr, uint32 val)
{
    uint32 busy;
        
    reg_write(RTL8307H_UNIT, IROM_ACCESS_ADDRESS, addr);
    reg_write(RTL8307H_UNIT, IROM_ACCESS_DATA, val);

    /* trigger the write command */
    reg_write(RTL8307H_UNIT, IROM_ACCESS_CONTROL, IROM_WR_CMD_MASK | IROM_WR_TRIGGER_MASK);

    /* wait operation finished   */
    do {
        reg_read(RTL8307H_UNIT, IROM_ACCESS_CONTROL, &busy);
    }while(busy & IROM_WR_TRIGGER_MASK);
}

/* Function Name:
 *      update_IROM
 * Description:
 *      Burn file into IROM
 * Input:
 *      file_name         -  The name of the file to be updated into IROM
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      
 */
static void update_IROM(void)
{
    uint16 len;
    uint16 i;
    uint32 reg_val;

    len = sizeof(RTL8307H_init_0_1_8_bin);
    for (i = 0; i < len; i += 4)
    {
        reg_val = (uint32)RTL8307H_init_0_1_8_bin[i+3] << 24 | (uint32)RTL8307H_init_0_1_8_bin[i+2] << 16 \
                  | (uint32)RTL8307H_init_0_1_8_bin[i+1] << 8 | RTL8307H_init_0_1_8_bin[i];
        iorm_set(i, reg_val);
    }

}

/* Function Name:
 *      rtk_iROM_update
 * Description:
 *      Burn initialization code into RTL8307H and start internal CPU
 * Input:
 *      file_name         -  The name of the file to be burned
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      
 */
void rtk_iROM_update(void)
{
#ifndef EXT_LAB
	uint32 regval;
#endif

    /*   hold internal cpu          */
    reg_write(RTL8307H_UNIT, CPU_8051_RESET, 2);

    /*   burn image into irom       */
    update_IROM();

    /*   start internal cpu to run the code  */
    reg_write(RTL8307H_UNIT, CPU_8051_RESET, 0);

}

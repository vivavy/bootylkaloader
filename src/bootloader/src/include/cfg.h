#ifndef CFG_H
#define CFG_H
#include <stddef.h>
extern wchar_t *cfg_botylka_file;

void read_cfg(void);

void save_cfg_to_boot_info(void);
#endif
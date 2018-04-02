#include "sys/types.h"
#include "sys/kconfig.h"

#define C (char *)

extern struct driver rbbusdriver;
extern struct driver uartdriver;
extern struct driver uartdriver;
extern struct driver wddriver;

struct conf_ctlr conf_ctlr_init[] = {
   /* driver,		unit,	addr,		pri,	flags */
    { &rbbusdriver,	0,	C 0x00000000,	-1,	0x0 },
    { 0 }
};

struct conf_device conf_device_init[] = {
   /* driver,		ctlr driver,	unit,	ctlr,	drive,	flags,	pins */
    { &uartdriver,	&rbbusdriver,	0,	0,	-2,	0x0,	{0} },
    { &uartdriver,	&rbbusdriver,	1,	0,	-2,	0x0,	{0} },
    { &wddriver,	&rbbusdriver,	0,	0,	-2,	0x0,	{0} },
    { 0 }
};

struct conf_service conf_service_init[] = {
    { 0 }
};

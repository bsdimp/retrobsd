#include "sys/param.h"
#include "sys/conf.h"

dev_t	rootdev = makedev(0, 1);	/* wd0a */
dev_t	dumpdev = makedev(0, 2);	/* wd0b */
dev_t	swapdev = makedev(0, 2);	/* wd0b */

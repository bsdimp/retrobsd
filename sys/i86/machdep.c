/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/inode.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/fs.h>
#include <sys/map.h>
#include <sys/buf.h>
#include <sys/file.h>
#include <sys/clist.h>
#include <sys/callout.h>
#include <sys/reboot.h>
#include <sys/msgbuf.h>
#include <sys/namei.h>
#include <sys/mount.h>
#include <sys/systm.h>
#include <sys/kconfig.h>
#include <sys/tty.h>

int     hz = HZ;
int     usechz = (1000000L + HZ - 1) / HZ;
#ifdef TIMEZONE
struct  timezone tz = { TIMEZONE, DST };
#else
struct  timezone tz = { 8*60, 1 };
#endif
int     nproc = NPROC;

struct  namecache namecache [NNAMECACHE];
char    bufdata [NBUF * MAXBSIZE];
struct  inode inode [NINODE];
struct  callout callout [NCALL];
struct  mount mount [NMOUNT];
struct  buf buf [NBUF], bfreelist [BQUEUES];
struct  bufhd bufhash [BUFHSZ];
struct  cblock cfree [NCLIST];
struct  proc proc [NPROC];
struct  file file [NFILE];

/*
 * Remove the ifdef/endif to run the kernel in unsecure mode even when in
 * a multiuser state.  Normally 'init' raises the security level to 1
 * upon transitioning to multiuser.  Setting the securelevel to -1 prevents
 * the secure level from being raised by init.
 */
#ifdef  PERMANENTLY_INSECURE
int securelevel = -1;
#else
int securelevel = 0;
#endif

struct mapent   swapent[SMAPSIZ];
struct map  swapmap[1] = {
    { swapent,
      &swapent[SMAPSIZ],
      "swapmap" },
};

int waittime = -1;

static int
nodump(dev)
    dev_t dev;
{
    printf("\ndumping to dev %o off %D: not implemented\n", dumpdev, dumplo);
    return 0;
}

int (*dump)(dev_t) = nodump;

#ifdef CONFIG
/*
 * Build using old configuration utility (configsys).
 */
dev_t   rootdev = NODEV;
dev_t   swapdev = NODEV;
dev_t   dumpdev = NODEV;
#endif
dev_t   pipedev;
daddr_t dumplo = (daddr_t) 1024;

/*
 * Machine dependent startup code
 */
void
startup()
{
    extern void _etext();
    extern unsigned __data_start;

    /* XXX MORE */

    /* Get total RAM size. */
    physmem = 892 << 10;

    boothowto = 0;
}

static void cpuidentify()
{
}

/*
 * Configure all controllers and devices as specified
 * in the kernel configuration file.
 */
void kconfig()
{
    struct conf_ctlr *ctlr;
    struct conf_device *dev;

    cpuidentify();

    /* Probe and initialize controllers first. */
    for (ctlr = conf_ctlr_init; ctlr->ctlr_driver; ctlr++) {
        if ((*ctlr->ctlr_driver->d_init)(ctlr)) {
            ctlr->ctlr_alive = 1;
        }
    }

    /* Probe and initialize devices. */
    for (dev = conf_device_init; dev->dev_driver; dev++) {
        if (is_controller_alive(dev->dev_cdriver, dev->dev_ctlr)) {
            if ((*dev->dev_driver->d_init)(dev)) {
                dev->dev_alive = 1;
            }
        }
    }
}

/*
 * Sit and wait for something to happen...
 */
void
idle()
{
    int s;

    /* Indicate that no process is running. */
    noproc = 1;

    /* Set SPL low so we can be interrupted. */
    s = spl0();

    /* Wait for something to happen. */
/*    asm ("halt"); */

    /* Restore previous SPL. */
    splx(s);
}

void
boot(dev, howto)
    register dev_t dev;
    register int howto;
{
    if ((howto & RB_NOSYNC) == 0 && waittime < 0 && bfreelist[0].b_forw) {
        register struct fs *fp;
        register struct buf *bp;
        int iter, nbusy;

        /*
         * Force the root filesystem's superblock to be updated,
         * so the date will be as current as possible after
         * rebooting.
         */
        fp = getfs(rootdev);
        if (fp)
            fp->fs_fmod = 1;
        waittime = 0;
        printf("syncing disks... ");
        (void) splnet();
        sync();
        for (iter = 0; iter < 20; iter++) {
            nbusy = 0;
            for (bp = &buf[NBUF]; --bp >= buf; )
                if (bp->b_flags & B_BUSY)
                    nbusy++;
            if (nbusy == 0)
                break;
            printf("%d ", nbusy);
            udelay(40000L * iter);
        }
        printf("done\n");
    }
    (void) splhigh();
    if (! (howto & RB_HALT)) {
        if ((howto & RB_DUMP) && dumpdev != NODEV) {
            /*
             * Take a dump of memory by calling (*dump)(),
             * which must correspond to dumpdev.
             * It should dump from dumplo blocks to the end
             * of memory or to the end of the logical device.
             */
            (*dump)(dumpdev);
        }
        /* Restart from dev, howto */
    }
    printf("halted\n");

    if (howto & RB_BOOTLOADER) {
	    /* Reset */
    }

#ifdef HALTREBOOT
    printf("press any key to reboot...");
    cngetc();

	    /* Reset */
#endif

    for (;;) {
/*        asm ("halt"); */
    }
    /*NOTREACHED*/
}

void
udelay(usec)
    u_int usec;
{
	/* XXX what to do */
}

/*
 * Machine dependent constants for MIPS32.
 *
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
#ifndef ENDIAN

/*
 * Definitions for byte order,
 * according to byte significance from low address to high.
 */
#define LITTLE          1234            /* least-significant byte first (vax) */
#define BIG             4321            /* most-significant byte first */
#define PDP             3412            /* LSB first in word, MSW first in long (pdp) */
#define ENDIAN          LITTLE          /* byte order on pic32 */

/*
 * The time for a process to be blocked before being very swappable.
 * This is a number of seconds which the system takes as being a non-trivial
 * amount of real time.  You probably shouldn't change this;
 * it is used in subtle ways (fractions and multiples of it are, that is, like
 * half of a ``long time'', almost a long time, etc.)
 * It is related to human patience and other factors which don't really
 * change over time.
 */
#define MAXSLP          20

/*
 * Clock ticks per second. The HZ value must be an integer factor of 1000.
 */
#ifndef HZ
#define HZ              200
#endif

/*
 * System parameter formulae.
 */
#ifndef NBUF
#define NBUF            10                      /* number of i/o buffers */
#endif
#ifndef MAXUSERS
#define MAXUSERS        1                       /* number of user logins */
#endif
#ifndef NPROC
#define NPROC           25                      /* number of processes */
#endif
#ifndef NINODE
#define NINODE          24
#endif
#ifndef NFILE
#define NFILE           24
#endif
#define NNAMECACHE      (NINODE * 11/10)
#define NCALL           (16 + 2 * MAXUSERS)
#define NCLIST          32                      /* number or CBSIZE blocks */
#ifndef SMAPSIZ
#define SMAPSIZ         NPROC                   /* size of swap allocation map */
#endif

/*
 * Disk blocks.
 */
#define DEV_BSIZE       1024            /* the same as MAXBSIZE */
#define DEV_BSHIFT      10              /* log2(DEV_BSIZE) */
#define DEV_BMASK       (DEV_BSIZE-1)

/* Bytes to disk blocks */
#define btod(x)         (((x) + DEV_BSIZE-1) >> DEV_BSHIFT)

extern void _keram_start(), _keram_end();
#define KERAM_SIZE ((unsigned)((char*)&_keram_end-(char*)&_keram_start))
#define KERNEL_DATA_SIZE        (32*1024-KERAM_SIZE)

#define KERNEL_DATA_START       0
#define KERNEL_DATA_END         (KERNEL_DATA_START + KERNEL_DATA_SIZE)

#define USER_DATA_START         (0)
#define USER_DATA_END           (0 + 64*1024)

#define stacktop(siz)           (USER_DATA_END)
#define stackbas(siz)           (USER_DATA_END-(siz))

/*
 * User area: a user structure, followed by the kernel
 * stack.  The number for USIZE is determined empirically.
 */
#define USIZE           3072
#define SSIZE           2048            /* initial stack size (bytes) */

/*
 * Collect kernel statistics by default.
 */
#if !defined(UCB_METER) && !defined(NO_UCB_METER)
#define UCB_METER
#endif

#ifdef KERNEL
#include "machine/io.h"

/*
 * Macros to decode processor status word.
 */
#define USERMODE(ps)    (((ps) & ST_UM) != 0)
#define BASEPRI(ps)     (CA_RIPL(ps) == 0)

#define splbio()        mips_intr_disable()
#define spltty()        mips_intr_disable()
#define splclock()      mips_intr_disable()
#define splhigh()       mips_intr_disable()
#define splnet()        mips_intr_enable()
#define splsoftclock()  mips_intr_enable()
#define spl0()          mips_intr_enable()
#define splx(s)         mips_intr_restore(s)

#define noop()          asm("nop")

/*
 * Wait for something to happen.
 */
void idle();

/*
 * Microsecond delay routine.
 */
void udelay(/* unsigned usec */);

/*
 * Setup system timer for `hz' timer interrupts per second.
 */
void clkstart(/* void */);

#endif /* KERNEL */

#endif /* ENDIAN */

/* 
   TINY3D sample / (c) 2010 Hermes  <www.elotrolado.net>

*/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

#define _FPS

#include <io/pad.h>

#include <tiny3d.h>
#include <libfont.h>

// font 0: 224 chr from 32 to 255, 16 x 32 pix 2 bit depth
#include "font.h"

// font 1: 224 chr from 32 to 255, 16 x 32 pix 2 bit depth
#include "font_b.h"

// font 2: 255 chr from 0 to 254, 8 x 8 pix 1 bit depth
extern unsigned char msx[];

#include "console.h"

#include "ff.h"
#include "fflib.h"
#include "fm.h"
#include "util.h"

#define SDTEST
#ifdef SDTEST
#include "types.h"

typedef struct {
    int device;
    void *dirStruct;
} DIR_ITER;

#include "iosupport.h"
#include "storage.h"
#include <malloc.h>
#include <sys/file.h>
#include <lv2/mutex.h> 
#include <sys/errno.h>

#include <sys/file.h>
#include <ppu-lv2.h>
#include <sys/stat.h>
#include <lv2/sysfs.h>

#include <sysutil/disc.h>

#include <sysmodule/sysmodule.h>
//app
int app_init (int dt);      //1
int app_input (int dt);     //2
int app_update (int dt);    //3
int app_render (int dt);    //4
int app_cleanup (int dt);   //5
//
//pad buttons mask - for use in app
#define PAD_R1_MASK         0x0001
#define PAD_L1_MASK         0x0002
#define PAD_R2_MASK         0x0004
#define PAD_L2_MASK         0x0008
#define PAD_CR_MASK         0x0010
#define PAD_SQ_MASK         0x0020
#define PAD_CI_MASK         0x0040
#define PAD_TR_MASK         0x0080
#define PAD_SE_MASK         0x0100
#define PAD_ST_MASK         0x0200
#define PAD_R3_MASK         0x0400
#define PAD_L3_MASK         0x0800
#define PAD_UP_MASK         0x1000
#define PAD_DN_MASK         0x2000
#define PAD_LT_MASK         0x4000
#define PAD_RT_MASK         0x8000
//
struct fm_panel lp, rp;
//

int fddr[8] = {0};
char fdld[8][256];
char fn[256] = {0};
char wn[256] = {0};
char dn[256] = {0};
int ffd = -1;
static u64 ff_ps3id[8] = {
	0x010300000000000AULL, 0x010300000000000BULL, 0x010300000000000CULL, 0x010300000000000DULL,
	0x010300000000000EULL, 0x010300000000000FULL, 0x010300000000001FULL, 0x0103000000000020ULL 
	};
#if 0
static int dev_fd[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
#include "storage.h"
int sdopen (int fd)
{
	static device_info_t disc_info;
	disc_info.unknown03 = 0x12345678; // hack for Iris Manager Disc Less
	disc_info.sector_size = 0;
	//int ret = sys_storage_get_device_info(ff_ps3id[fd], &disc_info);
    int ret = sys_storage_open(ff_ps3id[fd], &dev_fd[fd]);
    if (0 == ret)
        sys_storage_close(dev_fd[fd]);
    return ret;
}
#else
//tools
FRESULT scan_files2 (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    FDIR dir;
    UINT i = 0;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) 
    {
        for (;;) 
        {
            FRESULT res1 = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res1 != FR_OK || fno.fname[0] == 0) 
                break;  /* Break on error or end of dir */
            i++;
            if (i > 7)
                break;
            if (fno.fattrib & AM_DIR) 
            {                    /* It is a directory */
                snprintf (fdld[i], 255, "/%s", fno.fname);
                //sprintf(&path[i], "/%s", fno.fname);
                //res = scan_files(path);                    /* Enter the directory */
                //if (res != FR_OK) break;
                //path[i] = 0;
            } 
            else 
            {                                       /* It is a file. */
                snprintf (fdld[i], 255, "%s", fno.fname);
                //printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }
    return res;
}

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    FDIR dir;
    static FILINFO fno;

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK)
    {
        //DPrintf("Listing path contents for '%s'\n", path);
        for (;;)
        {
            FRESULT res1 = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res1 != FR_OK || fno.fname[0] == 0) 
            {
                f_closedir(&dir);
                return res;  /* Break on error or end of dir */
            }
            if (fno.fattrib & AM_DIR) 
            {                    /* It is a directory */
                //DPrintf("/%s\n", fno.fname);
                snprintf (dn, 255, "%s%s", path, fno.fname);
                snprintf (wn, 255, "%s%s/fatfs.tst", path, fno.fname);
                //
                fm_panel_add (&lp, fno.fname, 1, 0);
            } 
            else 
            {                                       /* It is a file. */
                //DPrintf("%s\n", fno.fname);
                snprintf (fn, 255, "%s%s", path, fno.fname);
                fm_panel_add (&lp, fno.fname, 0, 0);
            }
        }
        f_closedir(&dir);
    }
    else
    {
        ;//DPrintf("!unable to open path '%s' result %d\n", path, res);
    }
    return res;
}
#if 0
fdisk -l /dev/sdb1
Disk /dev/sdb1: 223.6 GiB, 240055746560 bytes, 468858880 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0xf4f4f4f4

Device      Boot      Start        End    Sectors  Size Id Type
/dev/sdb1p1      4109694196 8219388391 4109694196  1.9T f4 SpeedStor
/dev/sdb1p2      4109694196 8219388391 4109694196  1.9T f4 SpeedStor
/dev/sdb1p3      4109694196 8219388391 4109694196  1.9T f4 SpeedStor
/dev/sdb1p4      4109694196 8219388391 4109694196  1.9T f4 SpeedStor
fsck.exfat /dev/sdb1
exfatfsck 1.2.5
Checking file system on /dev/sdb1.
File system version           1.0
Sector size                 512 bytes
Cluster size                128 KB
Volume size                 224 GB
Used space                   21 MB
Available space             224 GB
#endif
int fs_info(int i)
{
    char lbuf[10];
    FATFS fs;     /* Ponter to the filesystem object */
    FDIR fdir;
    snprintf(lbuf, 10, "%d:/", i);
    int ret = f_mount(&fs, lbuf, 0);                    /* Mount the default drive */
    if (ret != FR_OK)
        return ret;
    ret = f_opendir (&fdir, lbuf);
    if (ret == FR_OK)
    {
        FATFS *fsp = fdir.obj.fs;
        DPrintf("fs on '%s' drive: %d, type: %d\n", lbuf, i, fsp->fs_type);
        DPrintf("sector size: %d, cluster size [sectors]: %d, size of an FAT [sectors]: %d\n", fsp->ssize, fsp->csize, fsp->fsize);
        DPrintf("number of FAT entries (number of clusters + 2): %d, number of FATs (1 or 2): %d\n", fsp->n_fatent, fsp->n_fats);
        DPrintf("last cluster: %d, free clusters: %d\n", fsp->last_clst, fsp->free_clst);
        unsigned long capa = fsp->n_fatent / 1024;
        capa *= fsp->ssize;
        capa /= 1024;
        capa *= fsp->csize;
        capa /= 1024;
        DPrintf("capacity: %luGB\n", capa);
        f_closedir (&fdir);
    }
    //
    f_mount(NULL, lbuf, 0);                    /* UnMount the default drive */
    //
    return FR_OK;
}

int sdopen (int i)
{
    //FDIR fdir;
    char lbuf[10];
    FATFS fs;     /* Ponter to the filesystem object */
    snprintf (lbuf, 10, "%d:/", i);
    int ret = f_mount (&fs, lbuf, 0);                    /* Mount the default drive */
    if (ret != FR_OK)
        return ret;
    if (ffd == -1)
        ffd = i;
    lp.path = strdup (lbuf);
    //ret = scan_files(lbuf); //f_opendir (&fdir, lbuf);
    //
    f_mount(NULL, lbuf, 0);                    /* UnMount the default drive */

    if (ret == FR_OK)
    {
        struct fm_file *ptr = lp.entries;
        DPrintf("Listing path contents for '%s'\n", lp.path);
        while (ptr != NULL)
        {
            DPrintf("%s%c\n", ptr->name, ptr->dir?'/':' ');
            ptr = ptr->next;
        }
        fs_info (i);
    }
    //
    return ret;
}

int sdopen2 (int i)
{
    FDIR fdir;
    char lbuf[10];
    FATFS fs;     /* Ponter to the filesystem object */
    snprintf(lbuf, 10, "%d:/", i);
    int ret = f_mount(&fs, lbuf, 0);                    /* Mount the default drive */
    if (ret != FR_OK)
    {
        return ret;
    }
    ret = f_opendir (&fdir, lbuf);
    DPrintf("%d f_opendir %s drive %d ssize %d csize %d\n", ret, lbuf, i, fs.ssize, fs.csize);
    if (ret == FR_OK)
        f_closedir (&fdir);
    f_mount(0, lbuf, 0);                    /* Mount the default drive */
    //
    return ret;
}
#endif
//

int fatfs_init()
{
    fflib_init();
    //
    int k; for (k = 0; k < 8; k++)
    {
        fdld[k][0] = '\0';
        fddr[k] = fflib_attach (k, ff_ps3id[k], 1);
        DPrintf("open drive %d result %d for 0x%llx\n", k, fddr[k], (long long unsigned int)ff_ps3id[k]);
        if (fddr[k] == FR_OK)
            sdopen (k);
        else
            fflib_detach (k);
    }
    //
    return 0;
}
#endif

void LoadTexture()
{
    u32 * texture_mem = tiny3d_AllocTexture(64*1024*1024); // alloc 64MB of space for textures (this pointer can be global)    

    u32 * texture_pointer; // use to asign texture space without changes texture_mem

    if(!texture_mem) return; // fail!

    texture_pointer = texture_mem;

    ResetFont();
    texture_pointer = (u32 *) AddFontFromBitmapArray((u8 *) font  , (u8 *) texture_pointer, 32, 255, 16, 32, 2, BIT0_FIRST_PIXEL);
    texture_pointer = (u32 *) AddFontFromBitmapArray((u8 *) font_b, (u8 *) texture_pointer, 32, 255, 16, 32, 2, BIT0_FIRST_PIXEL);
    texture_pointer = (u32 *) AddFontFromBitmapArray((u8 *) msx   , (u8 *) texture_pointer,  0, 254,  8,  8, 1, BIT7_FIRST_PIXEL);

    // here you can add more textures using 'texture_pointer'. It is returned aligned to 16 bytes
}
//file write perf
int file_write_perf (int idx)
{
    time_t timer, timere;
    struct tm * timed;
    FATFS fs0;          /* Work area (filesystem object) for logical drives */
    FIL fdst;           /* File objects */
    #define MSZ (1024*1024)
    #define FWR (MSZ*1024)/* 1GB as slices of 1MB */
    #define BSZ (3*MSZ)
    BYTE *buffer = NULL;//[BSZ];   /* File copy buffer */
    FRESULT fr;         /* FatFs function common result code */
    UINT bw;            /* File read/write count */
    char fn[25];
    snprintf (fn, 25, "%d:/file_write.bin", idx);
    /* Register work area for each logical drive */
    f_mount (&fs0, fn, 0);
    //test writing 1G
    /* Create destination file on the drive 0 */
    fr = f_open (&fdst, fn, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr)
    {
        DPrintf ("!failed to create file '%s'\n", fn);
        return (int)fr;
    }
    /* Copy buffer to destination */
    buffer = malloc (BSZ);
    if (buffer == NULL)
    {
        DPrintf ("!failed to allocate buffer of %dbytes\n", BSZ);
        return FR_INT_ERR;
    }
    //
    DPrintf ("writing %dMB to file '%s'.. please wait\n", FWR/MSZ, fn);
    //
    time (&timer);
    timed = localtime (&timer);
    DPrintf ("writing %d blocks of %dMB: ", FWR/BSZ, BSZ/MSZ);
    time (&timer);
    int k; for (k = 0; k < FWR/BSZ; k++)
    {
        fr = f_write (&fdst, buffer, BSZ, &bw);            /* Write it to the destination file */
        if (fr || bw < BSZ)
            break; /* error or disk full */
        //usleep (100000);
        DPrintf ("..%d ", k + 1);
        if (app_input(0) & PAD_CI_MASK)
            break;
        DbgDraw ();
        tiny3d_Flip ();
    }
    time (&timere);
    /* Close open files */
    f_close (&fdst);
    DPrintf ("done.\n");
    int mbps = (k*BSZ)/(timere - timer);
    if (mbps > MSZ)
        DPrintf ("wrote %dMB to file '%s' in %dsec (%dMBps) bs %dbytes\n", (k*BSZ)/MSZ, fn, (timere - timer), mbps/MSZ, BSZ);
    else if (mbps > 1024)
        DPrintf ("wrote %dKB to file '%s' in %dsec (%dKBps) bs %dbytes\n", k*BSZ/1024, fn, (timere - timer), mbps/1024, BSZ);
    //
    FILINFO fno;
    if (f_stat (fn, &fno) == FR_OK)
        DPrintf ("FS: '%s' size: %luMB\n", fn, fno.fsize/MSZ);
    else
        DPrintf ("FS: '%s' size check failed!\n", fn);
    //test reading
    /* Open source file on the drive 1 */
    fr = f_open (&fdst, fn, FA_READ);
    if (fr == FR_OK)
    {
        DPrintf ("reading from file '%s'.. please wait\n", fn);
        //
        time (&timer);
        DPrintf ("reading blocks of %dMB: ", BSZ/MSZ);
        /* Copy source to destination */
        for (k = 0;;k++)
        {
            fr = f_read (&fdst, buffer, BSZ, &bw);  /* Read a chunk of source file */
            if (fr || bw == 0)
                break; /* error or eof */
            DPrintf ("..%d ", k + 1);
            if (app_input(0) & PAD_CI_MASK)
                break;
            DbgDraw ();
            tiny3d_Flip ();
        }
        time (&timere);
        /* Close open files */
        f_close (&fdst);
        DPrintf ("done.\n");
        mbps = (k*BSZ)/(timere - timer);
        if (mbps > MSZ)
            DPrintf ("read %dMB from file '%s' in %dsec (%dMBps) bs %dbytes\n", (k*BSZ)/MSZ, fn, (timere - timer), mbps/MSZ, BSZ);
        else if (mbps > 1024)
            DPrintf ("read %dKB to file '%s' in %dsec (%dKBps) bs %dbytes\n", k*BSZ/1024, fn, (timere - timer), mbps/1024, BSZ);
    }
    //
    free (buffer);
    /* Unregister work area prior to discard it */
    f_mount (NULL, fn, 0);
    return (int)fr;
}
//create file
//f_write > create_chain > find_bitmap
int file_read(char *fname);
int file_create (char *fname)
{
    FATFS fs;      /* Work area (filesystem object) for logical drives */
    FIL fdst;      /* File objects */
    BYTE buffer[] = "ThiS is A TeSt FiLe FoR wRiTiNg.\n\0";   /* File copy buffer */
    FRESULT fr;    /* FatFs function common result code */
    UINT bw;       /* File read/write count */

    /* Register work area for each logical drive */
    char dn[5];
    snprintf(dn, 4, "%.3s", fname);
    DPrintf("mounting drive '%s'\n", dn);
    DPrintf("writing to file '%s':\n", fname);
    f_mount(&fs, dn, 0);

    /* Create destination file on the drive 0 */
    fr = f_open(&fdst, fname, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr)
    {
        DPrintf("!failed creating the file '%s' result %d\n", fname, fr);
        /* Unregister work area prior to discard it */
        f_mount(0, dn, 0);
        return (int)fr;
    }
    /* Copy source to destination */
    fr = f_write(&fdst, buffer, sizeof(buffer), &bw);            /* Write it to the destination file */
    if (fr != FR_OK || bw != sizeof(buffer))
        DPrintf("!failed writing to file '%s' result %d wrote %d bytes vs %d\n", fname, fr, bw, sizeof(buffer));
    /* Close open files */
    f_close(&fdst);

    /* Unregister work area prior to discard it */
    f_mount(0, dn, 0);
    //read test
    file_read (fname);
    //
    return (int)fr;
}
//
int file_new(char *fname)
{
    if(!fname || !*fname)
        return 0;
    //debug console
    initConsole ();
    DbgHeader("File create test");
    DbgMess("Press o/circle to return");
    //
    file_create (fname);
    //
    int btn = 0;
    //
    while(1)
    {
        //2 input
        btn = app_input(0);
        if (btn & PAD_CI_MASK)
            break;
        //3
		//4
        DbgDraw();
        tiny3d_Flip();
    }
    return 0;
}

//dir contents
int dir_read (char *dname)
{
    FRESULT res;
    FDIR dir;
    static FILINFO fno;
    /* Register work area to the default drive */
    FATFS fs;   /* Work area (filesystem object) for logical drive */
    char drn[5];
    snprintf(drn, 4, "%.3s", dname);
    DPrintf("mounting drive '%s'\n", drn);
    DPrintf("listing directory '%s':\n", dname);
    f_mount(&fs, drn, 0);

    res = f_opendir(&dir, dname);                       /* Open the directory */
    if (res == FR_OK)
    {
        for (;;)
        {
            FRESULT res1 = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res1 != FR_OK || fno.fname[0] == 0) 
            {
                f_closedir(&dir);
                return res;  /* Break on error or end of dir */
            }
            if (fno.fattrib & AM_DIR) 
            {                    /* It is a directory */
                DPrintf("/%s\n", fno.fname);
                snprintf (dn, 255, "%s%s/", dname, fno.fname);
            } 
            else 
            {                                       /* It is a file. */
                DPrintf("%s\n", fno.fname);
                snprintf (fn, 255, "%s%s", dname, fno.fname);
            }
        }
        f_closedir(&dir);
    }
    else
    {
        DPrintf("!unable to open path '%s' result %d\n", dname, res);
    }
    //
    f_mount(0, drn, 0);
    //
    return res;
}

int dir_run(char *dname)
{
    if(!dname || !*dname)
        return 0;
    //debug console
    initConsole ();
    DbgHeader("DIR contents");
    DbgMess("Press o/circle to return");
    //
    dir_read (dname);
    //
    int btn = 0;
    //
    while(1)
    {
        //2 input
        btn = app_input(0);
        if (btn & PAD_CI_MASK)
            break;
        //3
		//4
        DbgDraw();
        tiny3d_Flip();
    }
    return 0;
}

//file contents
int file_read(char *fname)
{
    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */
    FATFS fs;   /* Work area (filesystem object) for logical drive */

    /* Register work area to the default drive */
    char dn[5];
    snprintf(dn, 4, "%.3s", fname);
    DPrintf("mounting drive '%s'\n", dn);
    DPrintf("reading file '%s':\n", fname);
    f_mount(&fs, dn, 0);

    /* Open a text file */
    fr = f_open(&fil, fname, FA_READ);
    if (fr)
    {
        DPrintf("!failed opening the file '%s' result %d\n", fname, fr);
        //
        f_mount(0, dn, 0);
        return (int)fr;
    }

    /* Read every line and display it */
    while (f_gets(line, sizeof (line), &fil)) 
    {
        DPrintf(line);
    }

    /* Close the file */
    f_close(&fil);
    //
    f_mount(0, dn, 0);
    //
    return 0;
}

int file_run(char *fname)
{
    if(!fname || !*fname)
        return 0;
    //debug console
    initConsole ();
    DbgHeader("File contents");
    DbgMess("Press o/circle to return");
    //
    file_read (fname);
    //
    int btn = 0;
    //
    while(1)
    {
        //2 input
        btn = app_input(0);
        if (btn & PAD_CI_MASK)
            break;
        //3
		//4
        DbgDraw();
        tiny3d_Flip();
    }
    return 0;
}
//app
struct fm_panel *app_active_panel()
{
    if (lp.active)
        return &lp;
    return &rp;
}
//restore app state after other module was executed
int _app_restore (char init)
{
    if (init)
        initConsole ();
    DbgHeader("FATFS EXFAT Example");
    DbgMess("Press o/circle to exit");
    DPrintf("\n");
    if (*fn)
        DPrintf("press triangle to list contents of file '%s'\n", fn);
    if (*wn)
        DPrintf("press cross to create a test file '%s'\n", wn);
    if (*dn)
        DPrintf("press rectangle to list contents of dir '%s'\n", dn);
    if (ffd != -1)
        DPrintf("press start for write/read test on drive '%d:/'\n", ffd);
    return 0;
}//1st

char sp[] = "fat1:/";
char sp2[] = "sys:/";
int app_init (int dt)
{
    // change to 2D context ( virtual size of the screen is 848.0 x 512.0)
    //for 8x8 font, we can split the screen into 106x64 chars - 2 panes w53chars
    //we leave 8 lines for progress/status/info below the panes
    fm_panel_init (&lp, 0, 0, 53*8, 56*8, 1);
    fm_panel_init (&rp, 53*8, 0, 53*8, 56*8, 0);
    //debug console
    initConsole ();
    //fatfs test
    fatfs_init ();
    //
    tiny3d_Init (1024*1024);
    //
	ioPadInit (7);
    //
	// Load texture
    LoadTexture ();
    //
    DbgHeader("FATFS EXFAT Example");
    DbgMess("Press x/cross to exit");
    //
    fm_panel_scan (&lp, NULL);
    fm_panel_scan (&rp, NULL);
    //
    return 1;
}
//2nd
#if 0
from
unsigned int BTN_LEFT : 1;      /*!< \brief left button */
unsigned int BTN_DOWN : 1;      /*!< \brief down button */
unsigned int BTN_RIGHT : 1;     /*!< \brief right button */
unsigned int BTN_UP : 1;        /*!< \brief up button */
unsigned int BTN_START : 1;     /*!< \brief start button */
unsigned int BTN_R3 : 1;        /*!< \brief R3 button */
unsigned int BTN_L3 : 1;        /*!< \brief L3 button */
unsigned int BTN_SELECT : 1;    /*!< \brief select button */
unsigned int BTN_SQUARE : 1;    /*!< \brief square button */
unsigned int BTN_CROSS : 1;     /*!< \brief cross button */
unsigned int BTN_CIRCLE : 1;    /*!< \brief circle button */
unsigned int BTN_TRIANGLE : 1;  /*!< \brief triangle button */
unsigned int BTN_R1 : 1;        /*!< \brief R1 button */
unsigned int BTN_L1 : 1;        /*!< \brief L1 button */
unsigned int BTN_R2 : 1;        /*!< \brief R2 button */
unsigned int BTN_L2 : 1;        /*!< \brief L2 button */ 
to 
#define PAD_R1_MASK         0x0001
#define PAD_L1_MASK         0x0002
#define PAD_R2_MASK         0x0004
#define PAD_L2_MASK         0x0008
#define PAD_CR_MASK         0x0010
#define PAD_SQ_MASK         0x0020
#define PAD_CI_MASK         0x0040
#define PAD_TR_MASK         0x0080
#define PAD_SE_MASK         0x0100
#define PAD_ST_MASK         0x0200
#define PAD_R3_MASK         0x0400
#define PAD_L3_MASK         0x0800
#define PAD_UP_MASK         0x1000
#define PAD_DN_MASK         0x2000
#define PAD_LT_MASK         0x4000
#define PAD_RT_MASK         0x8000
#endif
int app_input(int dat)
{
	padInfo padinfo;
	padData paddata;
    int i;
    int ret = 0;
    // Check the pads.
    ioPadGetInfo(&padinfo);

    for(i = 0; i < MAX_PADS; i++)
    {
        if(padinfo.status[i])
        {
            ioPadGetData(i, &paddata);
            //map buttons
            if(paddata.BTN_UP)
                ret |= PAD_UP_MASK;
            if(paddata.BTN_DOWN)
                ret |= PAD_DN_MASK;
            if(paddata.BTN_LEFT)
                ret |= PAD_LT_MASK;
            if(paddata.BTN_RIGHT)
                ret |= PAD_RT_MASK;
            if(paddata.BTN_CROSS)
                ret |= PAD_CR_MASK;
            if(paddata.BTN_CIRCLE)
                ret |= PAD_CI_MASK;
            if(paddata.BTN_TRIANGLE)
                ret |= PAD_TR_MASK;
            if(paddata.BTN_SQUARE)
                ret |= PAD_SQ_MASK;
            if(paddata.BTN_START)
                ret |= PAD_ST_MASK;
            if(paddata.BTN_SELECT)
                ret |= PAD_SE_MASK;
            if(paddata.BTN_R1)
                ret |= PAD_R1_MASK;
            if(paddata.BTN_L1)
                ret |= PAD_L1_MASK;
            if(paddata.BTN_R2)
                ret |= PAD_R2_MASK;
            if(paddata.BTN_L2)
                ret |= PAD_L2_MASK;
        }
    }
    return ret;
}
//3rd
int app_update(int dat)
{
    //2 input
    int btn = app_input (0);
    //quit?
    if (btn & PAD_CI_MASK)
    {
        if (fm_panel_exit (app_active_panel ()))
            return -1;
    }
    //activate left panel
    else if(btn & PAD_L1_MASK)
    {
        lp.active = 1;
        rp.active = 0;
    }
    //activate right panel
    else if(btn & PAD_R1_MASK)
    {
        lp.active = 0;
        rp.active = 1;
    }
    //scroll panel up
    else if(btn & PAD_UP_MASK)
    {
        fm_panel_scroll (app_active_panel (), FALSE);
    }
    //scroll panel dn
    else if(btn & PAD_DN_MASK)
    {
        fm_panel_scroll (app_active_panel (), TRUE);
    }
    //enter dir
    else if(btn & PAD_RT_MASK)
    {
        fm_panel_enter (app_active_panel ());
    }
    //exit dir
    else if(btn & PAD_LT_MASK)
    {
        fm_panel_exit (app_active_panel ());
    }
    //file create
    else if(btn & PAD_CR_MASK)
    {
    }
    //file contents
    else if(btn & PAD_TR_MASK)
    {
    }
    //dir listing
    else if(btn & PAD_SQ_MASK)
    {
    }
    //file write
    else if(btn & PAD_ST_MASK)
    {
    }
    //
    return 0;
}
//4th
int app_render(int dat)
{
    /* DRAWING STARTS HERE */
    tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);
        
    // Enable alpha Test
    tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);

   // Enable alpha blending.
            tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
                TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,
                TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
    //
    //DbgDraw ();
    // change to 2D context ( virtual size of the screen is 848.0 x 512.0)
    fm_panel_draw (&lp);
    fm_panel_draw (&rp);
    //
#ifdef _FPS
    char sfps[8];
    snprintf (sfps, 7, "%dfps", fps_update ());
    SetFontColor (0xff0000ff, 0x00000000);
    SetFontAutoCenter (0);
    DrawString (800, 0, sfps);
#endif
    //
    tiny3d_Flip ();

    return 1;
}
//5th
int app_cleanup(int dat)
{
    ioPadEnd();

    return 1;
}

//
s32 main(s32 argc, const char* argv[])
{
    //1 init
	app_init (0);
    _app_restore (0);
    app_input (0);
	// Ok, everything is setup. Now for the main loop.
	while(1) 
    {
        //3
        if (app_update (0) == -1)
            break;
		//4
        app_render (0);
	}
    //5
    app_cleanup (0);
    //
	return 0;
}


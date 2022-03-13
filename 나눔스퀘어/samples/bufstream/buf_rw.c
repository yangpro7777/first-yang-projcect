/* ----------------------------------------------------------------------------
 * File: buf_rw.c
 *
 * Desc: Read/Write bufstream implementation
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__APPLE__) && !defined(__linux__)
    #include <io.h>
#else
    #include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "buf_rw.h"
#include "auxinfo.h"


#if !defined(__APPLE__) && !defined(__linux__)
    #define rw_file_close _close
    #define rw_file_read _read 
    #define rw_file_seek _lseeki64
    #define rw_file_write _write
#else
    #ifdef __linux__
        #define rw_file_seek lseek64
    #else
        #define rw_file_seek lseek
    #endif

    #define rw_file_read  read 
    #define rw_file_close close
    #define rw_file_write write
#endif


static int32_t origin_values[3] = 
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

//implementation structure
struct impl_stream
{
    int32_t       io;
    uint8_t      *pBfr;
    uint32_t      idx;
    uint32_t      bfr_size;
    uint32_t      bfr_count;
    uint32_t      chunk_size;
    uint64_t      bytecount;
    uint64_t      file_size;
    uint8_t       mode;
    BS_fnMemAlloc memAlloc;
    BS_fnMemFree  memFree;
};


static void *internal_memAlloc(unsigned int size) 
{ 
    return malloc((size_t)size); 
} 
 
 
static void internal_memFree(void *pMemBlk) 
{ 
    free(pMemBlk); 
} 


static uint8_t *fw_request(bufstream_tt *pBS, uint32_t numbytes)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;

    if (numbytes > pImp->bfr_size)
    {
        return NULL;
    }

    if (pImp->idx + numbytes <= pImp->bfr_size)
    {
        return pImp->pBfr + pImp->idx;
    }

    if (pImp->idx)
    {
        uint32_t n;

        n = rw_file_write(pImp->io, pImp->pBfr, pImp->idx);
        if (n != pImp->idx)
        {
            return NULL;
        }
    }

    pImp->idx = 0;

    return pImp->pBfr;
}


static uint8_t *fr_request(bufstream_tt *pBS, uint32_t numbytes)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;

    if (pImp->idx + numbytes <= pImp->bfr_count)
    {
        return pImp->pBfr + pImp->idx;
    }

    if (pImp->idx == pImp->bfr_count)
    {
        pImp->idx = 0;
        pImp->bfr_count = 0;
    }

    if (pImp->idx + numbytes > pImp->bfr_size)
    {
        memcpy(pImp->pBfr, pImp->pBfr + pImp->idx, pImp->bfr_count - pImp->idx);
        pImp->bfr_count -= pImp->idx;
        pImp->idx = 0;
    }

    pImp->bfr_count += rw_file_read(pImp->io, pImp->pBfr + pImp->bfr_count, pImp->bfr_size - pImp->bfr_count);

    if (pImp->idx + numbytes <= pImp->bfr_count)
    {
        return pImp->pBfr + pImp->idx;
    }

    return NULL;
}


static uint32_t fw_copybytes(bufstream_tt *pBS, uint8_t *pPtr, uint32_t numbytes)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;
    uint32_t n;

    if (pImp->idx)
    {
        n = rw_file_write(pImp->io, pImp->pBfr, pImp->idx);
        if (n != pImp->idx)
        {
            pImp->idx = 0;
            return 0;
        }
        pImp->idx = 0;
    }

    n = rw_file_write(pImp->io, pPtr, numbytes);
    pImp->bytecount += n;
    if (pImp->bytecount > pImp->file_size)
    {
        pImp->file_size = pImp->bytecount;
    }

    if (n != numbytes)
    {
        return 0;
    }

    return numbytes;
}


static uint32_t fr_copybytes(bufstream_tt *pBS, uint8_t *pPtr, uint32_t numbytes)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;

    if (pImp->bfr_count != pImp->idx)
    {
        if (pImp->idx + numbytes <= pImp->bfr_count)
        {
            memcpy(pPtr, pImp->pBfr + pImp->idx, numbytes);
            pImp->idx += numbytes;
            pImp->bytecount +=numbytes;
            return numbytes;
        }
        else
        {
            int32_t c, i;
            memcpy(pPtr, pImp->pBfr + pImp->idx, pImp->bfr_count - pImp->idx);
            c = numbytes - (pImp->bfr_count - pImp->idx);
            pImp->bytecount += pImp->bfr_count - pImp->idx;
            pImp->idx = 0;
            pImp->bfr_count = 0;

            i = rw_file_read(pImp->io, pPtr + numbytes - c, c);
            pImp->bytecount += i;
            c -= i;
            return numbytes - c;
        }
    }
    else
    {
        int32_t n;

        pImp->idx = 0;
        pImp->bfr_count = 0;
        n = rw_file_read(pImp->io, pPtr, numbytes);
        pImp->bytecount += n;

        return n;
    }
}

static uint32_t bs_usable_bytes(bufstream_tt *pBS)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;

    return pImp->bfr_size - pImp->idx;
}


static uint32_t bs_confirm(bufstream_tt *pBS, uint32_t numbytes)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;

    pImp->idx       += numbytes;
    pImp->bytecount += numbytes;
    if (pImp->bytecount > pImp->file_size)
    {
        pImp->file_size = pImp->bytecount;
    }

    return numbytes;
}


static uint32_t bs_chunksize(bufstream_tt *pBS)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;

    return pImp->chunk_size;
}


static void bs_done(bufstream_tt *pBS, int32_t abort)
{
    struct impl_stream* pImp = pBS->Buf_IO_struct;

    if (pImp->io != -1)
    {
        if (!abort && (pImp->mode == BS_FILE_WRITE_MODE))
        {
            if (rw_file_write(pImp->io, pImp->pBfr, pImp->idx)){};
        }
        rw_file_close(pImp->io);
        pImp->io = -1;
    }
}


static void bs_free(bufstream_tt *pBS)
{
    struct impl_stream* pImp = pBS->Buf_IO_struct;
    BS_fnMemFree memFree = internal_memFree;

    if (pImp)
    {
        memFree = pImp->memFree;
        pBS->done(pBS, 0);
        memFree(pImp->pBfr);
        memFree(pImp);
    }
    memFree(pBS);
}


static int32_t bs_seek(bufstream_tt *pBS, int32_t origin, int64_t pos)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;
    int32_t n;

    if ((pImp->mode == BS_FILE_WRITE_MODE) && (pImp->idx > 0))
    {
        n = rw_file_write(pImp->io, pImp->pBfr, pImp->idx);
        if (n != (int32_t)pImp->idx)
        {
            return BS_ERROR;
        }
    }

    if (rw_file_seek(pImp->io, pos, origin_values[origin]) == -1)
    {
        return BS_ERROR;
    }

    pImp->idx = 0;
    pImp->bytecount = pos;
        
    return BS_OK;
}


static int32_t bs_switch_mode(bufstream_tt *pBS, uint8_t mode)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;

    if (pImp->mode != mode)
    {
        if (bs_seek(pBS, BS_SEEK_START, pImp->bytecount) == BS_ERROR)
        {
            return BS_ERROR;
        }

        if (mode == BS_FILE_WRITE_MODE)
        {
            pImp->mode     = BS_FILE_WRITE_MODE;
            pBS->request   = fw_request;
            pBS->copybytes = fw_copybytes;
        }
        else
        {
            pImp->mode     = BS_FILE_READ_MODE;
            pBS->request   = fr_request;
            pBS->copybytes = fr_copybytes;
        }
    }

    return BS_OK;
}


static int32_t bs_seek_info(bufstream_tt *pBS, struct buf_seek_info *pSeekInfo)
{
    struct impl_stream *pImp = pBS->Buf_IO_struct;
    uint64_t bytecount = pImp->bytecount;
    int32_t n;

    if ((pImp->mode == BS_FILE_WRITE_MODE) && (pImp->idx > 0))
    {
        n = rw_file_write(pImp->io, pImp->pBfr, pImp->idx);
        if (n != (int32_t)pImp->idx)
        {
            return BS_ERROR;
        }
        bytecount = pImp->bytecount + pImp->idx;
        pImp->idx = 0;
    }

    if (bs_seek(pBS, BS_SEEK_START, pSeekInfo->seek_pos) == BS_ERROR)
    {
        return BS_ERROR;
    }

    n = rw_file_write(pImp->io, pSeekInfo->bfr, pSeekInfo->bfr_size);
    if (n != (int32_t)pSeekInfo->bfr_size)
    {
        // see if we can seek back to the original position
        bs_seek(pBS, BS_SEEK_START, bytecount);
        return BS_ERROR;
    }

    return bs_seek(pBS, BS_SEEK_START, bytecount);
}


static uint32_t bs_auxinfo(bufstream_tt *pBS, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
    struct impl_stream* pImp = pBS->Buf_IO_struct;
    int32_t retVal = BS_OK;

    switch(info_ID)
    {
    case BYTECOUNT_INFO:
        if (info_ptr && (info_size == sizeof(uint64_t)))
        {
            uint64_t *pPtr = (uint64_t*)info_ptr;
            *pPtr = pImp->bytecount;
        }
        break;

    case FILESIZE_INFO:
        if (info_ptr && (info_size == sizeof(uint64_t)))
        {
            uint64_t *pPtr = (uint64_t*)info_ptr;
            *pPtr = pImp->file_size;
        }
        break;

    case FILESEEK_INFO:
        if (info_ptr && (info_size == sizeof(int64_t)))
        {
            int64_t *pPtr = (int64_t*)info_ptr;
            retVal = bs_seek(pBS, offs, *pPtr);
        }
        break;

    case FILEWRITE_MODE:
        retVal = bs_switch_mode(pBS, BS_FILE_WRITE_MODE);
        break;

    case FILEREAD_MODE:
        retVal = bs_switch_mode(pBS, BS_FILE_READ_MODE);
        break;

    case BUF_SEEK_INFO:
        if (info_size == sizeof(struct buf_seek_info))
        {
            struct buf_seek_info *pSeekInfo = (struct buf_seek_info*)info_ptr;
            retVal = bs_seek_info(pBS, pSeekInfo);
        }
        break;
    }
  
    return retVal;
}


#ifdef _BS_UNICODE
bufstream_tt *open_file_buf_rw(const wchar_t* pFilename,
                               uint32_t bufsize,
                               uint8_t initialMode,
                               BS_fnGetRC fnGetRC)
#else
bufstream_tt *open_file_buf_rw(const char *pFilename,
                               uint32_t bufsize,
                               uint8_t initialMode,
                               BS_fnGetRC fnGetRC)
#endif
{
    bufstream_tt *pBS = NULL;
    struct impl_stream *pImp = NULL;
    BS_fnMemAlloc memAlloc = NULL;
    BS_fnMemFree  memFree = NULL;
    int32_t fde;

#if !defined(__APPLE__) && !defined(__linux__) && !defined(__QNX__)
    struct _stati64 stat_data;
#else
    #if defined(__QNX__)
        struct stat64 stat_data;
    #else
        struct stat stat_data;
    #endif
#endif

#if !defined(__APPLE__) && !defined(__linux__) && !defined(__QNX__)
    #ifdef _BS_UNICODE
        fde = _wstati64(pFilename, &stat_data);
    #else
        fde = _stati64(pFilename, &stat_data);
    #endif
#else
    #if defined(__QNX__)
        fde = stat64(pFilename, &stat_data);
    #else
        fde = stat(pFilename, &stat_data);
    #endif
#endif

    if (fnGetRC)
    {
        memAlloc = (BS_fnMemAlloc)fnGetRC("malloc");
        memFree = (BS_fnMemFree)fnGetRC("free");
    }

    if (!memAlloc)
    {
        memAlloc = internal_memAlloc;
    }
    if (!memFree)
    {
        memFree = internal_memFree;
    }

    pBS = (bufstream_tt*)memAlloc(sizeof(bufstream_tt));
    if (pBS == NULL)
    {
        return NULL;
    }
    memset(pBS, 0, sizeof(bufstream_tt));

    pBS->Buf_IO_struct = (struct impl_stream*)memAlloc(sizeof(struct impl_stream));
    if (pBS->Buf_IO_struct == NULL)
    {
        memFree(pBS);
        return NULL;
    }

    pImp = pBS->Buf_IO_struct;
    memset(pImp, 0, sizeof(struct impl_stream));

    pImp->memAlloc = memAlloc;
    pImp->memFree = memFree;

    if (initialMode == BS_FILE_WRITE_MODE)
    {
        if (fde < 0)
        {
            // the file does not exist, create it
#if !defined(__APPLE__) && !defined(__linux__)
    #ifdef _BS_UNICODE
            pImp->io = _wopen(pFilename, _O_RDWR | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE);
    #else
            pImp->io = _open(pFilename, _O_RDWR | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE);
    #endif
#else
            pImp->io = open(pFilename, O_RDWR | O_CREAT, 0777 );
#endif
        }
        else
        {
            // the file exists, truncate it
#if !defined(__APPLE__) && !defined(__linux__)
    #ifdef _BS_UNICODE
            pImp->io = _wopen(pFilename, _O_RDWR | _O_TRUNC | _O_BINARY);
    #else
            pImp->io = _open(pFilename, _O_RDWR | _O_TRUNC | _O_BINARY);
    #endif
#else
            pImp->io = open(pFilename, O_RDWR | O_TRUNC, 0777 );
#endif
        }
    }
    else
    {
        if (fde < 0)
        {
            // the file must exist!
            memFree(pBS->Buf_IO_struct);
            memFree(pBS);
            return NULL;
        }

        pImp->file_size = stat_data.st_size;

#if !defined(__APPLE__) && !defined(__linux__)
    #ifdef _BS_UNICODE
        pImp->io = _wopen(pFilename, _O_RDWR | _O_BINARY);
    #else
        pImp->io = _open(pFilename, _O_RDWR | _O_BINARY);
    #endif
#else
        pImp->io = open(pFilename, O_RDWR, 0777 );
#endif
    }

    if (pImp->io == -1)
    {
        memFree(pBS->Buf_IO_struct);
        memFree(pBS);
        return NULL;
    }

    pImp->pBfr = (uint8_t*)malloc(bufsize);
    if (pImp->pBfr == NULL)
    {
        rw_file_close(pImp->io);
        memFree(pBS->Buf_IO_struct);
        memFree(pBS);
        return NULL;
    }

    pImp->bfr_size    = bufsize;
    pImp->chunk_size  = bufsize;
    pImp->mode        = initialMode;

    pBS->usable_bytes = bs_usable_bytes;

    if (initialMode == BS_FILE_WRITE_MODE)
    {
        pBS->request      = fw_request;
        pBS->copybytes    = fw_copybytes;
    }
    else
    {
        pBS->request      = fr_request;
        pBS->copybytes    = fr_copybytes;
    }
    pBS->confirm      = bs_confirm;
    pBS->chunksize    = bs_chunksize;
    pBS->free         = bs_free;
    pBS->auxinfo      = bs_auxinfo;
    pBS->done         = bs_done;
    pBS->flags        = BS_FLAGS_DST;

    return pBS;
}

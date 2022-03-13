#include "buf_tmp_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxinfo.h"

bufstream_tt *open_file_buf_tmp(const char *bs_filename, uint32_t bufsize, void (*DisplayError)(char *txt));
int32_t init_file_buf_tmp(bufstream_tt *bs, const char *bs_filename, uint32_t bufsize, void (*DisplayError)(char *txt));

//implementation structure
struct impl_stream
{
    FILE *io;

    uint8_t *bfr;
    uint32_t idx;       // read-write index
    uint32_t bfr_size;  // allocated size
    uint32_t bfr_count; // filled size (for read)
    uint32_t chunk_size; 
    uint64_t bytecount;

    char filename[_BS_MAX_PATH];
    char temp_filename[_BS_MAX_PATH];
};


bufstream_tt * open_buf_tmp_file(const char * filename, uint32_t chunksize, void (*DisplayError)(char *txt))
{
    char temp_filename[_BS_MAX_PATH];
    bufstream_tt* bs = NULL;
    struct impl_stream* p = NULL;
    strcpy(temp_filename, filename);
    strcat(temp_filename, ".temp");

    bs=(bufstream_tt*)malloc(sizeof(bufstream_tt));
    if(bs)
    {
        if(BS_OK != init_file_buf_tmp(bs, temp_filename, chunksize, DisplayError))
        {
            free(bs);
            bs = NULL;
            return bs;
        }
    }

    p = bs->Buf_IO_struct;

    strcpy(p->temp_filename, temp_filename);
    strcpy(p->filename, filename);
    return bs;
};

void close_tmp_buf(bufstream_tt* bs, int32_t Abort)
{
    bs->done(bs, Abort);
    bs->free(bs);
}

static uint32_t usable_bytes_tmp(bufstream_tt *bs)
{
    struct impl_stream* p = bs->Buf_IO_struct;
    return p->bfr_size-p->idx;
}


//request the buffer with at least numbytes-bytes
static uint8_t *request_tmp(bufstream_tt *bs, uint32_t numbytes)
{
    struct impl_stream* p = bs->Buf_IO_struct;

    if (numbytes > p->bfr_size)
        return NULL;

    if(p->idx+numbytes <= p->bfr_size)
        return p->bfr+p->idx;

    if(p->idx)
    {

        uint32_t n;
        n = (uint32_t) fwrite(p->bfr, sizeof(uint8_t), p->idx, p->io);
        if(n != p->idx)
        {
            return NULL;
        }
    }

    p->idx=0;
    return p->bfr;
}

static uint32_t confirm_tmp(bufstream_tt *bs, uint32_t numbytes)
{
    bs->Buf_IO_struct->idx       += numbytes;
    bs->Buf_IO_struct->bytecount += numbytes;
    return numbytes;
}

static uint32_t copybytes_tmp(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
    struct impl_stream* p = bs->Buf_IO_struct;
    uint32_t n;
    if(p->idx)
    {
        n = (uint32_t) fwrite(p->bfr, sizeof(uint8_t), p->idx, p->io);
        if(n != p->idx)
        {
            p->idx=0;
            return 0;
        }
        p->idx=0;
    }

    n = (uint32_t) fwrite(ptr, sizeof(uint8_t), numbytes, p->io);
    bs->Buf_IO_struct->bytecount += n;

    if(n != numbytes)
    {
        return 0;
    }

    return numbytes;
}

static uint32_t chunksize_tmp(bufstream_tt *bs)
{
    return bs->Buf_IO_struct->chunk_size;
}

static void free_tmp(bufstream_tt *bs)
{
    if(bs->Buf_IO_struct)
        bs->done(bs,0);

    free(bs);
}

//inform bufstream about some additional info
static uint32_t auxinfo_tmp(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
    uint64_t *ptr;
    switch (info_ID)
    {

    case BYTECOUNT_INFO:
        ptr = (uint64_t*)info_ptr;
        if (ptr && (info_size == sizeof(uint64_t)))
            *ptr = bs->Buf_IO_struct->bytecount;
        break;

    case FLUSH_BUFFER:
        if (fflush(bs->Buf_IO_struct->io))
            ; // Do nothing	
        break;

    case SPLIT_OUTPUT:
        bs->split(bs);
        break;

    case FILENAME_INFO:
    {
#ifdef _BS_UNICODE
        wchar_t *ptr2 = (wchar_t*)info_ptr;
        if (ptr2 && (info_size > wcslen(bs->Buf_IO_struct->filename) * 2))
            wcscpy(ptr2, bs->Buf_IO_struct->filename);
#else
        char *ptr2 = (char*)info_ptr;
        if (ptr2 && (info_size > strlen(bs->Buf_IO_struct->filename)))
            strcpy(ptr2, bs->Buf_IO_struct->filename);
#endif
        break;
    }
    }
    return BS_OK;
}


static uint32_t MC_EXPORT_API split_tmp(bufstream_tt *bs)
{
    struct impl_stream* p = bs->Buf_IO_struct;
    fwrite(p->bfr, sizeof(uint8_t), p->idx, p->io);
    fclose(p->io);
    p->io = NULL;

//TODO: need to check p->temp_filename existence ?
    remove(p->filename);
    rename (p->temp_filename, p->filename);

    p->bfr_count  = 0;
    p->idx        = 0;

    p->io = fopen(p->temp_filename, "wb");
    if(!p->io)
    {
        return BS_ERROR;
    }

    p->bytecount = 0;
    return 0;
}


static void MC_EXPORT_API done_tmp(bufstream_tt *bs, int32_t abort)
{
    struct impl_stream* p = bs->Buf_IO_struct;
    if (abort)  // remove compile warning
        ; // Do nothing

    if (p->idx != 0)
        fwrite(p->bfr, sizeof(uint8_t), p->idx, p->io);
    fclose(p->io);

    if (p->idx != 0)
    {
        remove (p->filename);
        rename (p->temp_filename, p->filename);
    }
    else
        remove (p->temp_filename);

    free(p->bfr);
    free(p);
    bs->Buf_IO_struct = NULL;
}


static int32_t init_bs_tmp(bufstream_tt *bs, uint32_t bufsize)
{
    bs->Buf_IO_struct->bfr = (uint8_t*)malloc(bufsize);
    if(!bs->Buf_IO_struct->bfr)
    {

        fclose(bs->Buf_IO_struct->io);
        free(bs->Buf_IO_struct);
        return BS_ERROR;
    }

    bs->Buf_IO_struct->bfr_size           = bufsize;
    bs->Buf_IO_struct->bfr_count          = 0;
    bs->Buf_IO_struct->chunk_size         = bufsize;
    bs->Buf_IO_struct->idx                = 0;
    bs->Buf_IO_struct->bytecount          = 0;

    bs->usable_bytes = usable_bytes_tmp;
    bs->request      = request_tmp;
    bs->confirm      = confirm_tmp;
    bs->copybytes    = copybytes_tmp;
    bs->split        = split_tmp;
    bs->chunksize    = chunksize_tmp;
    bs->free         = free_tmp;
    bs->auxinfo      = auxinfo_tmp;
    bs->done         = done_tmp;
    bs->drive_ptr    = NULL;
    bs->drive        = NULL;

    bs->state        = NULL;
    bs->flags        = 0;
    return BS_OK;
}

int32_t init_file_buf_tmp(bufstream_tt *bs,
    const char *bs_filename,
    uint32_t bufsize, 
    void (*DisplayError)(char *txt))
{
    if (DisplayError)  // remove compile warning
        ; // Do nothing

    bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
    if(!bs->Buf_IO_struct)
    {
        return BS_ERROR;
    }

    bs->Buf_IO_struct->io = fopen(bs_filename, "wb");

    if(!bs->Buf_IO_struct->io)

    {
        free(bs->Buf_IO_struct);
        return BS_ERROR;
    }

    strcpy(bs->Buf_IO_struct->filename, bs_filename);

    return init_bs_tmp(bs, bufsize);
}


/*!
 *
 * Copyright (c) 2018 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.  Unauthorized
 * reproduction or distribution of any portion is prohibited by law.
 **/

#pragma once
#include "mcdefs.h"

#include <cstdlib>
#include <stddef.h>
#include <cstddef>
#include <cstring>
#include <new>


namespace common {

    callbacks_t default_callbacks();
    callbacks_t callbacks(const callbacks_t* callbacks);

    void* default_cbs_malloc(context_t, size_t size);
    void default_cbs_free(context_t, void* ptr);
    void* default_cbs_realloc(context_t, void* ptr, size_t size);

    struct MemFuncs{
        context_t context;
        void* (MC_EXPORT_API* lalloc)(context_t context, size_t size);
        void (MC_EXPORT_API* lfree)(context_t context, void* ptr);
    };

    MemFuncs memfuncs(const callbacks_t* cbs);

    class BaseAllocator {
    public:
        explicit BaseAllocator(const callbacks_t * cbs) noexcept;

        explicit BaseAllocator(callbacks_t && cbs) noexcept;
        explicit BaseAllocator(const callbacks_t & cbs) noexcept;

        ~BaseAllocator() = default;

        void* operator new(size_t block_size, const MemFuncs &allocContext);

        void operator delete(void *p, const MemFuncs &allocContext);

        void operator delete(void *p);

    protected:
        MemFuncs context = {};
    };
}

#ifndef SAFE_COPY_ALLOC_STRING
#define SAFE_COPY_ALLOC_STRING(_to, _from, _fromlen)\
SAFE_DELETE_ARRAY_L(_to);\
_to = (char *)context.lalloc(context.context, ((_fromlen) + 1));\
memset( _to, 0, (_fromlen) + 1 );\
memcpy( _to, _from, _fromlen );
#endif

#ifndef SAFE_COPY_ALLOC_WIDE_STRING
#define SAFE_COPY_ALLOC_WIDE_STRING(_to, _from, _fromlen)\
SAFE_DELETE_ARRAY_L(_to);\
_to = (wchar_t *)context.lalloc(context.context, ((_fromlen) + 1)*sizeof(wchar_t) );\
memset( _to, 0, ((_fromlen) + 1)*sizeof(wchar_t) );\
memcpy( _to, _from, (_fromlen)*sizeof(wchar_t) );
#endif

#ifndef SAFE_DELETE_ARRAY_L
#define SAFE_DELETE_ARRAY_L(_p) {if((_p) != NULL) { context.lfree(context.context, _p); (_p) = NULL; }}
#endif

template<typename T>
void SAFE_DELETE(T *& p) {
    delete p;
    p = nullptr;
}

template<typename T>
void SAFE_FREE(T *& p) {
    free(p);
    p = nullptr;
}

template<typename T>
void SAFE_DELETE_ARRAY(T *& p) {
    delete [] p;
    p = nullptr;
}

template<typename T>
void SAFE_RELEASE(T *& p) {
    if (p)
        p->Release();
    p = nullptr;
}

void SAFE_COPY_STRING(char *& dst, const char * const src, size_t len);

void SAFE_COPY_STRING_SHORT(char *& dst, const char * const src);

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************/


/*------------------------------------------------------------------------------
 *
 *   UCommonData   An abstract interface for dealing with ICU Common Data Files.
 *                 ICU Common Data Files are a grouping of a number of individual
 *                 data items (resources, converters, tables, anything) into a
 *                 single file or dll.  The combined format includes a table of
 *                 contents for locating the individual items by name.
 *
 *                 Two formats for the table of contents are supported, which is
 *                 why there is an abstract interface involved.
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucmndata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udatamem.h"

#ਜੇ defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#ਜੇ_ਅੰਤ

U_CFUNC uint16_t
udata_get_ਸਿਰਲੇਖ_ਮਾਪ(ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *udh) {
    ਜੇ(udh==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ ਜੇ(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ਵਾਪਸ udh->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ;
    } ਨਹੀਂਤਾਂ {
        /* opposite endianness */
        uint16_t x=udh->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ;
        ਵਾਪਸ (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_ਮਾਪ(ਅਚਲ UDataInfo *info) {
    ਜੇ(info==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ ਜੇ(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ਵਾਪਸ info->size;
    } ਨਹੀਂਤਾਂ {
        /* opposite endianness */
        uint16_t x=info->size;
        ਵਾਪਸ (uint16_t)((x<<8)|(x>>8));
    }
}

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *  Pointer TOCs.   TODO: This form of table-of-contents should be removed     *
 *                  because DLLs must be relocated on loading to correct the   *
 *                  pointer values and this operation makes shared memory      *
 *                  mapping of the data much less likely to work.              *
 *                                                                             *
 *-----------------------------------------------------------------------------*/
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅਚਲ ਅੱਖਰ       *ਇੰਦਰਾਜ_Name;
    ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *p_ਸਿਰਲੇਖ;
} ਸੂਚਕ_TOCEntry;


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    ਸੂਚਕ_TOCEntry   ਇੰਦਰਾਜ[1];
}  ਸੂਚਕ_TOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MIN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN(a,b) (((a)<(b)) ? (a) : (b))
#ਜੇ_ਅੰਤ

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
ਸਥਿਰ int32_t
strcmp_After_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *s1, ਅਚਲ ਅੱਖਰ *s2, int32_t *p_ਅਗੇਤਰ_ਲੰਮਾਈ) {
    int32_t pl=*p_ਅਗੇਤਰ_ਲੰਮਾਈ;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    ਲਈ(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        ਜੇ(cmp!=0 || c1==0) {  /* different or done */
            ਰੋਕੋ;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *p_ਅਗੇਤਰ_ਲੰਮਾਈ=pl;
    ਵਾਪਸ cmp;
}

ਸਥਿਰ int32_t
ਵਿੱਥ_TOCPrefix_ਦੁਆਧਾਰੀ_ਖੋਜ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *ਨਾਂ,
                            ਅਚਲ UData_ਵਿੱਥ_TOCEntry *toc, int32_t count) {
    int32_t ਸ਼ੁਰੂ=0;
    int32_t ਹੱਦਬੰਦੀ=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t ਸ਼ੁਰੂ_ਅਗੇਤਰ_ਲੰਮਾਈ=0;
    int32_t ਹੱਦਬੰਦੀ_ਅਗੇਤਰ_ਲੰਮਾਈ=0;
    ਜੇ(count==0) {
        ਵਾਪਸ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ਜੇ(0==strcmp_After_ਅਗੇਤਰ(s, ਨਾਂ+toc[0].name_ਵਿੱਥ, &ਸ਼ੁਰੂ_ਅਗੇਤਰ_ਲੰਮਾਈ)) {
        ਵਾਪਸ 0;
    }
    ++ਸ਼ੁਰੂ;
    --ਹੱਦਬੰਦੀ;
    ਜੇ(0==strcmp_After_ਅਗੇਤਰ(s, ਨਾਂ+toc[ਹੱਦਬੰਦੀ].name_ਵਿੱਥ, &ਹੱਦਬੰਦੀ_ਅਗੇਤਰ_ਲੰਮਾਈ)) {
        ਵਾਪਸ ਹੱਦਬੰਦੀ;
    }
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        int32_t i=(ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ)/2;
        int32_t ਅਗੇਤਰ_ਲੰਮਾਈ=MIN(ਸ਼ੁਰੂ_ਅਗੇਤਰ_ਲੰਮਾਈ, ਹੱਦਬੰਦੀ_ਅਗੇਤਰ_ਲੰਮਾਈ);
        int32_t cmp=strcmp_After_ਅਗੇਤਰ(s, ਨਾਂ+toc[i].name_ਵਿੱਥ, &ਅਗੇਤਰ_ਲੰਮਾਈ);
        ਜੇ(cmp<0) {
            ਹੱਦਬੰਦੀ=i;
            ਹੱਦਬੰਦੀ_ਅਗੇਤਰ_ਲੰਮਾਈ=ਅਗੇਤਰ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ(cmp==0) {
            ਵਾਪਸ i;
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ=i+1;
            ਸ਼ੁਰੂ_ਅਗੇਤਰ_ਲੰਮਾਈ=ਅਗੇਤਰ_ਲੰਮਾਈ;
        }
    }
    ਵਾਪਸ -1;
}

ਸਥਿਰ int32_t
ਸੂਚਕ_TOCPrefix_ਦੁਆਧਾਰੀ_ਖੋਜ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਸੂਚਕ_TOCEntry *toc, int32_t count) {
    int32_t ਸ਼ੁਰੂ=0;
    int32_t ਹੱਦਬੰਦੀ=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t ਸ਼ੁਰੂ_ਅਗੇਤਰ_ਲੰਮਾਈ=0;
    int32_t ਹੱਦਬੰਦੀ_ਅਗੇਤਰ_ਲੰਮਾਈ=0;
    ਜੇ(count==0) {
        ਵਾਪਸ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ਜੇ(0==strcmp_After_ਅਗੇਤਰ(s, toc[0].ਇੰਦਰਾਜ_Name, &ਸ਼ੁਰੂ_ਅਗੇਤਰ_ਲੰਮਾਈ)) {
        ਵਾਪਸ 0;
    }
    ++ਸ਼ੁਰੂ;
    --ਹੱਦਬੰਦੀ;
    ਜੇ(0==strcmp_After_ਅਗੇਤਰ(s, toc[ਹੱਦਬੰਦੀ].ਇੰਦਰਾਜ_Name, &ਹੱਦਬੰਦੀ_ਅਗੇਤਰ_ਲੰਮਾਈ)) {
        ਵਾਪਸ ਹੱਦਬੰਦੀ;
    }
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        int32_t i=(ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ)/2;
        int32_t ਅਗੇਤਰ_ਲੰਮਾਈ=MIN(ਸ਼ੁਰੂ_ਅਗੇਤਰ_ਲੰਮਾਈ, ਹੱਦਬੰਦੀ_ਅਗੇਤਰ_ਲੰਮਾਈ);
        int32_t cmp=strcmp_After_ਅਗੇਤਰ(s, toc[i].ਇੰਦਰਾਜ_Name, &ਅਗੇਤਰ_ਲੰਮਾਈ);
        ਜੇ(cmp<0) {
            ਹੱਦਬੰਦੀ=i;
            ਹੱਦਬੰਦੀ_ਅਗੇਤਰ_ਲੰਮਾਈ=ਅਗੇਤਰ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ(cmp==0) {
            ਵਾਪਸ i;
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ=i+1;
            ਸ਼ੁਰੂ_ਅਗੇਤਰ_ਲੰਮਾਈ=ਅਗੇਤਰ_ਲੰਮਾਈ;
        }
    }
    ਵਾਪਸ -1;
}

U_CDECL_BEGIN
ਸਥਿਰ uint32_t U_CALLCONV
ਵਿੱਥ_TOCEntry_ਗਿਣਤੀ(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ) {
    int32_t          retVal=0;
    ਅਚਲ UData_ਵਿੱਥ_TOC *toc = (UData_ਵਿੱਥ_TOC *)p_ਅੰਕੜੇ->toc;
    ਜੇ (toc != ਖਾਲੀ_ਸੂਚਕ) {
        retVal = toc->count;
    }
    ਵਾਪਸ retVal;
}

ਸਥਿਰ ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ * U_CALLCONV
ਵਿੱਥ_TOCLookup_Fn(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ,
                  ਅਚਲ ਅੱਖਰ *toc_ਇੰਦਰਾਜ_Name,
                  int32_t *p_ਲੰਮਾਈ,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    ਅਚਲ UData_ਵਿੱਥ_TOC  *toc = (UData_ਵਿੱਥ_TOC *)p_ਅੰਕੜੇ->toc;
    ਜੇ(toc!=ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ ਅੱਖਰ *ਅਧਾਰ=(ਅਚਲ ਅੱਖਰ *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#ਜੇ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        ਲਈ(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &ਅਧਾਰ[toc->ਇੰਦਰਾਜ[number].name_ਵਿੱਥ]);
        }
#ਜੇ_ਅੰਤ
        number=ਵਿੱਥ_TOCPrefix_ਦੁਆਧਾਰੀ_ਖੋਜ(toc_ਇੰਦਰਾਜ_Name, ਅਧਾਰ, toc->ਇੰਦਰਾਜ, count);
        ਜੇ(number>=0) {
            /* found it */
            ਅਚਲ UData_ਵਿੱਥ_TOCEntry *ਇੰਦਰਾਜ=toc->ਇੰਦਰਾਜ+number;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_ਇੰਦਰਾਜ_Name);
#ਜੇ_ਅੰਤ
            ਜੇ((number+1) < count) {
                *p_ਲੰਮਾਈ = (int32_t)(ਇੰਦਰਾਜ[1].ਅੰਕੜੇ_ਵਿੱਥ - ਇੰਦਰਾਜ->ਅੰਕੜੇ_ਵਿੱਥ);
            } ਨਹੀਂਤਾਂ {
                *p_ਲੰਮਾਈ = -1;
            }
            ਵਾਪਸ (ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)(ਅਧਾਰ+ਇੰਦਰਾਜ->ਅੰਕੜੇ_ਵਿੱਥ);
        } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_ਇੰਦਰਾਜ_Name);
#ਜੇ_ਅੰਤ
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#ਜੇ_ਅੰਤ

        ਵਾਪਸ p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ;
    }
}


ਸਥਿਰ uint32_t U_CALLCONV ਸੂਚਕ_TOCEntry_ਗਿਣਤੀ(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ) {
    ਅਚਲ ਸੂਚਕ_TOC *toc = (ਸੂਚਕ_TOC *)p_ਅੰਕੜੇ->toc;
    ਵਾਪਸ toc != ਖਾਲੀ_ਸੂਚਕ ? toc->count : 0;
}

ਸਥਿਰ ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ * U_CALLCONV ਸੂਚਕ_TOCLookup_Fn(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ,
                   ਅਚਲ ਅੱਖਰ *name,
                   int32_t *p_ਲੰਮਾਈ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    ਜੇ(p_ਅੰਕੜੇ->toc!=ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ ਸੂਚਕ_TOC *toc = (ਸੂਚਕ_TOC *)p_ਅੰਕੜੇ->toc;
        int32_t number, count=(int32_t)toc->count;

#ਜੇ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        ਲਈ(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->ਇੰਦਰਾਜ[number].ਇੰਦਰਾਜ_Name);
        }
#ਜੇ_ਅੰਤ
        number=ਸੂਚਕ_TOCPrefix_ਦੁਆਧਾਰੀ_ਖੋਜ(name, toc->ਇੰਦਰਾਜ, count);
        ਜੇ(number>=0) {
            /* found it */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->ਇੰਦਰਾਜ[number].ਇੰਦਰਾਜ_Name);
#ਜੇ_ਅੰਤ
            *p_ਲੰਮਾਈ=-1;
            ਵਾਪਸ UData_ਸਮ੍ਰਿਤੀ_normalize_ਅੰਕੜੇ_ਸੂਚਕ(toc->ਇੰਦਰਾਜ[number].p_ਸਿਰਲੇਖ);
        } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#ਜੇ_ਅੰਤ
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ;
    }
}
U_CDECL_ਸਿਰਾ


ਸਥਿਰ ਅਚਲ common_ਅੰਕੜੇ_Funcs CmnDFuncs = {ਵਿੱਥ_TOCLookup_Fn,  ਵਿੱਥ_TOCEntry_ਗਿਣਤੀ};
ਸਥਿਰ ਅਚਲ common_ਅੰਕੜੇ_Funcs ਮੰਜ਼ਿਲ_CPFuncs = {ਸੂਚਕ_TOCLookup_Fn, ਸੂਚਕ_TOCEntry_ਗਿਣਤੀ};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC ਖਾਲੀ udata_check_Common_ਅੰਕੜੇ(UData_ਸਮ੍ਰਿਤੀ *udm, UError_ਕੋਡ *err) {
    ਜੇ (U_FAILURE(*err)) {
        ਵਾਪਸ;
    }

    ਜੇ(udm==ਖਾਲੀ_ਸੂਚਕ || udm->p_ਸਿਰਲੇਖ==ਖਾਲੀ_ਸੂਚਕ) {
      *err=U_INVALID_FORMAT_ERROR;
    } ਨਹੀਂਤਾਂ ਜੇ(!(udm->p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic1==0xda &&
        udm->p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic2==0x27 &&
        udm->p_ਸਿਰਲੇਖ->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_ਸਿਰਲੇਖ->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    ਨਹੀਂਤਾਂ ਜੇ (udm->p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[0]==0x43 &&
        udm->p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[1]==0x6d &&
        udm->p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[2]==0x6e &&
        udm->p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[3]==0x44 &&
        udm->p_ਸਿਰਲੇਖ->info.format_ਸੰਸਕਰਣ[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ਅਚਲ ਅੱਖਰ *)udm->p_ਸਿਰਲੇਖ+udata_get_ਸਿਰਲੇਖ_ਮਾਪ(udm->p_ਸਿਰਲੇਖ);
    }
    ਨਹੀਂਤਾਂ ਜੇ(udm->p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[0]==0x54 &&
        udm->p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[1]==0x6f &&
        udm->p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[2]==0x43 &&
        udm->p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[3]==0x50 &&
        udm->p_ਸਿਰਲੇਖ->info.format_ਸੰਸਕਰਣ[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ਮੰਜ਼ਿਲ_CPFuncs;
        udm->toc=(ਅਚਲ ਅੱਖਰ *)udm->p_ਸਿਰਲੇਖ+udata_get_ਸਿਰਲੇਖ_ਮਾਪ(udm->p_ਸਿਰਲੇਖ);
    }
    ਨਹੀਂਤਾਂ {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    ਜੇ (U_FAILURE(*err)) {
        /* If the data is no good and we memory-mapped it ourselves,
         *  close the memory mapping so it doesn't leak.  Note that this has
         *  no effect on non-memory mapped data, other than clearing fields in udm.
         */
        udata_close(udm);
    }
}

/*
 * TODO: Add a udata_swapPackageHeader() function that swaps an ICU .dat package
 * header but not its sub-items.
 * This function will be needed for automatic runtime swapping.
 * Sub-items should not be swapped to limit the swapping to the parts of the
 * package that are actually used.
 *
 * Since lengths of items are implicit in the order and offsets of their
 * ToC entries, and since offsets are relative to the start of the ToC,
 * a swapped version may need to generate a different data structure
 * with pointers to the original data items and with their lengths
 * (-1 for the last one if it is not known), and maybe even pointers to the
 * swapped versions of the items.
 * These pointers to swapped versions would establish a cache;
 * instead, each open data item could simply own the storage for its swapped
 * data. This fits better with the current design.
 *
 * markus 2003sep18 Jitterbug 2235
 */

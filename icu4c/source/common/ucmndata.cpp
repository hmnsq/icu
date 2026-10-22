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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ucmndata.h"
#ಅನ್ತರ್ಭಾವ "udatamem.h"

#ಆದರೆ defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

U_CFUNC uint16_t
udata_get_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *udh) {
    ಆದರೆ(udh==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ಹಿಂತಿರುಗಿ udh->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* opposite endianness */
        uint16_t x=udh->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
        ಹಿಂತಿರುಗಿ (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ UDataInfo *info) {
    ಆದರೆ(info==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ಹಿಂತಿರುಗಿ info->size;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* opposite endianness */
        uint16_t x=info->size;
        ಹಿಂತಿರುಗಿ (uint16_t)((x<<8)|(x>>8));
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ       *ನಮೂದು_Name;
    ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *p_ಶೀರ್ಷಿಕೆ;
} ಸೂಚಕ_TOCEntry;


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    ಸೂಚಕ_TOCEntry   ನಮೂದು[1];
}  ಸೂಚಕ_TOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MIN
#ವ್ಯಾಖ್ಯಾನಿಸು MIN(a,b) (((a)<(b)) ? (a) : (b))
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
ಸ್ಥಿರ int32_t
strcmp_After_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s2, int32_t *p_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ) {
    int32_t pl=*p_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    ಗಾಗಿ(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        ಆದರೆ(cmp!=0 || c1==0) {  /* different or done */
            ನಿಲ್ಲಿಸು;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *p_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=pl;
    ಹಿಂತಿರುಗಿ cmp;
}

ಸ್ಥಿರ int32_t
ಸ್ಥಾನಾಂತರ_TOCPrefix_ದ್ವಿಮಾನ_ಹುಡುಕು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಹೆಸರುಗಳು,
                            ಸ್ಥಿರಾಂಕ UData_ಸ್ಥಾನಾಂತರ_TOCEntry *toc, int32_t count) {
    int32_t ಆರಂಭ=0;
    int32_t ಎಲ್ಲೆ=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t ಆರಂಭ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=0;
    int32_t ಎಲ್ಲೆ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=0;
    ಆದರೆ(count==0) {
        ಹಿಂತಿರುಗಿ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ಆದರೆ(0==strcmp_After_ಪೂರ್ವಪ್ರತ್ಯಯ(s, ಹೆಸರುಗಳು+toc[0].name_ಸ್ಥಾನಾಂತರ, &ಆರಂಭ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ++ಆರಂಭ;
    --ಎಲ್ಲೆ;
    ಆದರೆ(0==strcmp_After_ಪೂರ್ವಪ್ರತ್ಯಯ(s, ಹೆಸರುಗಳು+toc[ಎಲ್ಲೆ].name_ಸ್ಥಾನಾಂತರ, &ಎಲ್ಲೆ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ)) {
        ಹಿಂತಿರುಗಿ ಎಲ್ಲೆ;
    }
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        int32_t i=(ಆರಂಭ+ಎಲ್ಲೆ)/2;
        int32_t ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=MIN(ಆರಂಭ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ, ಎಲ್ಲೆ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ);
        int32_t cmp=strcmp_After_ಪೂರ್ವಪ್ರತ್ಯಯ(s, ಹೆಸರುಗಳು+toc[i].name_ಸ್ಥಾನಾಂತರ, &ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ);
        ಆದರೆ(cmp<0) {
            ಎಲ್ಲೆ=i;
            ಎಲ್ಲೆ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cmp==0) {
            ಹಿಂತಿರುಗಿ i;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ=i+1;
            ಆರಂಭ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ int32_t
ಸೂಚಕ_TOCPrefix_ದ್ವಿಮಾನ_ಹುಡುಕು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಸೂಚಕ_TOCEntry *toc, int32_t count) {
    int32_t ಆರಂಭ=0;
    int32_t ಎಲ್ಲೆ=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t ಆರಂಭ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=0;
    int32_t ಎಲ್ಲೆ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=0;
    ಆದರೆ(count==0) {
        ಹಿಂತಿರುಗಿ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ಆದರೆ(0==strcmp_After_ಪೂರ್ವಪ್ರತ್ಯಯ(s, toc[0].ನಮೂದು_Name, &ಆರಂಭ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ++ಆರಂಭ;
    --ಎಲ್ಲೆ;
    ಆದರೆ(0==strcmp_After_ಪೂರ್ವಪ್ರತ್ಯಯ(s, toc[ಎಲ್ಲೆ].ನಮೂದು_Name, &ಎಲ್ಲೆ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ)) {
        ಹಿಂತಿರುಗಿ ಎಲ್ಲೆ;
    }
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        int32_t i=(ಆರಂಭ+ಎಲ್ಲೆ)/2;
        int32_t ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=MIN(ಆರಂಭ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ, ಎಲ್ಲೆ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ);
        int32_t cmp=strcmp_After_ಪೂರ್ವಪ್ರತ್ಯಯ(s, toc[i].ನಮೂದು_Name, &ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ);
        ಆದರೆ(cmp<0) {
            ಎಲ್ಲೆ=i;
            ಎಲ್ಲೆ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cmp==0) {
            ಹಿಂತಿರುಗಿ i;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ=i+1;
            ಆರಂಭ_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

U_CDECL_BEGIN
ಸ್ಥಿರ uint32_t U_CALLCONV
ಸ್ಥಾನಾಂತರ_TOCEntry_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ) {
    int32_t          retVal=0;
    ಸ್ಥಿರಾಂಕ UData_ಸ್ಥಾನಾಂತರ_TOC *toc = (UData_ಸ್ಥಾನಾಂತರ_TOC *)p_ದತ್ತಾಂಶ->toc;
    ಆದರೆ (toc != ಶೂನ್ಯಸೂಚಕ) {
        retVal = toc->count;
    }
    ಹಿಂತಿರುಗಿ retVal;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ * U_CALLCONV
ಸ್ಥಾನಾಂತರ_TOCLookup_Fn(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *toc_ನಮೂದು_Name,
                  int32_t *p_ಉದ್ದಳತೆ,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    ಸ್ಥಿರಾಂಕ UData_ಸ್ಥಾನಾಂತರ_TOC  *toc = (UData_ಸ್ಥಾನಾಂತರ_TOC *)p_ದತ್ತಾಂಶ->toc;
    ಆದರೆ(toc!=ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತಳ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#ಆದರೆ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        ಗಾಗಿ(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &ತಳ[toc->ನಮೂದು[number].name_ಸ್ಥಾನಾಂತರ]);
        }
#ಆದರೆ_ಅಂತ್ಯ
        number=ಸ್ಥಾನಾಂತರ_TOCPrefix_ದ್ವಿಮಾನ_ಹುಡುಕು(toc_ನಮೂದು_Name, ತಳ, toc->ನಮೂದು, count);
        ಆದರೆ(number>=0) {
            /* found it */
            ಸ್ಥಿರಾಂಕ UData_ಸ್ಥಾನಾಂತರ_TOCEntry *ನಮೂದು=toc->ನಮೂದು+number;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_ನಮೂದು_Name);
#ಆದರೆ_ಅಂತ್ಯ
            ಆದರೆ((number+1) < count) {
                *p_ಉದ್ದಳತೆ = (int32_t)(ನಮೂದು[1].ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ - ನಮೂದು->ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                *p_ಉದ್ದಳತೆ = -1;
            }
            ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)(ತಳ+ನಮೂದು->ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
        } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_ನಮೂದು_Name);
#ಆದರೆ_ಅಂತ್ಯ
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#ಆದರೆ_ಅಂತ್ಯ

        ಹಿಂತಿರುಗಿ p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ;
    }
}


ಸ್ಥಿರ uint32_t U_CALLCONV ಸೂಚಕ_TOCEntry_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ) {
    ಸ್ಥಿರಾಂಕ ಸೂಚಕ_TOC *toc = (ಸೂಚಕ_TOC *)p_ದತ್ತಾಂಶ->toc;
    ಹಿಂತಿರುಗಿ toc != ಶೂನ್ಯಸೂಚಕ ? toc->count : 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ * U_CALLCONV ಸೂಚಕ_TOCLookup_Fn(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                   int32_t *p_ಉದ್ದಳತೆ,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    ಆದರೆ(p_ದತ್ತಾಂಶ->toc!=ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ ಸೂಚಕ_TOC *toc = (ಸೂಚಕ_TOC *)p_ದತ್ತಾಂಶ->toc;
        int32_t number, count=(int32_t)toc->count;

#ಆದರೆ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        ಗಾಗಿ(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->ನಮೂದು[number].ನಮೂದು_Name);
        }
#ಆದರೆ_ಅಂತ್ಯ
        number=ಸೂಚಕ_TOCPrefix_ದ್ವಿಮಾನ_ಹುಡುಕು(name, toc->ನಮೂದು, count);
        ಆದರೆ(number>=0) {
            /* found it */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->ನಮೂದು[number].ನಮೂದು_Name);
#ಆದರೆ_ಅಂತ್ಯ
            *p_ಉದ್ದಳತೆ=-1;
            ಹಿಂತಿರುಗಿ UData_ಸ್ಮರಣೆ_normalize_ದತ್ತಾಂಶ_ಸೂಚಕ(toc->ನಮೂದು[number].p_ಶೀರ್ಷಿಕೆ);
        } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#ಆದರೆ_ಅಂತ್ಯ
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ;
    }
}
U_CDECL_ಕೊನೆ


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ common_ದತ್ತಾಂಶ_Funcs CmnDFuncs = {ಸ್ಥಾನಾಂತರ_TOCLookup_Fn,  ಸ್ಥಾನಾಂತರ_TOCEntry_ಎಣಿಕೆ};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ common_ದತ್ತಾಂಶ_Funcs ಗಮ್ಯ_CPFuncs = {ಸೂಚಕ_TOCLookup_Fn, ಸೂಚಕ_TOCEntry_ಎಣಿಕೆ};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC ಶೂನ್ಯ udata_check_Common_ದತ್ತಾಂಶ(UData_ಸ್ಮರಣೆ *udm, UError_ಸಂಕೇತ *err) {
    ಆದರೆ (U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(udm==ಶೂನ್ಯಸೂಚಕ || udm->p_ಶೀರ್ಷಿಕೆ==ಶೂನ್ಯಸೂಚಕ) {
      *err=U_INVALID_FORMAT_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!(udm->p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic1==0xda &&
        udm->p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic2==0x27 &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (udm->p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[0]==0x43 &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[1]==0x6d &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[2]==0x6e &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[3]==0x44 &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.format_ಆವೃತ್ತಿ[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)udm->p_ಶೀರ್ಷಿಕೆ+udata_get_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ(udm->p_ಶೀರ್ಷಿಕೆ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(udm->p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[0]==0x54 &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[1]==0x6f &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[2]==0x43 &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[3]==0x50 &&
        udm->p_ಶೀರ್ಷಿಕೆ->info.format_ಆವೃತ್ತಿ[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ಗಮ್ಯ_CPFuncs;
        udm->toc=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)udm->p_ಶೀರ್ಷಿಕೆ+udata_get_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ(udm->p_ಶೀರ್ಷಿಕೆ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    ಆದರೆ (U_FAILURE(*err)) {
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

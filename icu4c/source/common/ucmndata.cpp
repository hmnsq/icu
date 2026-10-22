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

#ಯದಿ defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#endif

U_CFUNC uint16_t
udata_getHeaderSize(ಧ್ರುವ DataHeader *udh) {
    ಯದಿ(udh==nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    } ಅನ್ಯಥಾ ಯದಿ(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ಪ್ರತ್ಯಾಗಮ udh->dataHeader.headerSize;
    } ಅನ್ಯಥಾ {
        /* opposite endianness */
        uint16_t x=udh->dataHeader.headerSize;
        ಪ್ರತ್ಯಾಗಮ (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_getInfoSize(ಧ್ರುವ UDataInfo *info) {
    ಯದಿ(info==nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    } ಅನ್ಯಥಾ ಯದಿ(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ಪ್ರತ್ಯಾಗಮ info->size;
    } ಅನ್ಯಥಾ {
        /* opposite endianness */
        uint16_t x=info->size;
        ಪ್ರತ್ಯಾಗಮ (uint16_t)((x<<8)|(x>>8));
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಧ್ರುವ ವರ್ಣ       *entryName;
    ಧ್ರುವ DataHeader *pHeader;
} PointerTOCEntry;


ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    PointerTOCEntry   entry[1];
}  PointerTOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
ಸ್ಥಿರ int32_t
strcmpAfterPrefix(ಧ್ರುವ ವರ್ಣ *s1, ಧ್ರುವ ವರ್ಣ *s2, int32_t *pPrefixLength) {
    int32_t pl=*pPrefixLength;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    ಪ್ರತಿ(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        ಯದಿ(cmp!=0 || c1==0) {  /* different or done */
            ಭಙ್ಗ;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *pPrefixLength=pl;
    ಪ್ರತ್ಯಾಗಮ cmp;
}

ಸ್ಥಿರ int32_t
offset_TOCPrefix_ದ್ವಿಮಾನ_ಹುಡುಕು(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *names,
                            ಧ್ರುವ UDataOffsetTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    ಯದಿ(count==0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ಯದಿ(0==strcmpAfterPrefix(s, names+toc[0].nameOffset, &startPrefixLength)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ++start;
    --limit;
    ಯದಿ(0==strcmpAfterPrefix(s, names+toc[limit].nameOffset, &limitPrefixLength)) {
        ಪ್ರತ್ಯಾಗಮ limit;
    }
    ಯಾವತ್(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, names+toc[i].nameOffset, &prefixLength);
        ಯದಿ(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } ಅನ್ಯಥಾ ಯದಿ(cmp==0) {
            ಪ್ರತ್ಯಾಗಮ i;
        } ಅನ್ಯಥಾ {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ int32_t
pointer_TOCPrefix_ದ್ವಿಮಾನ_ಹುಡುಕು(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    ಯದಿ(count==0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ಯದಿ(0==strcmpAfterPrefix(s, toc[0].entryName, &startPrefixLength)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ++start;
    --limit;
    ಯದಿ(0==strcmpAfterPrefix(s, toc[limit].entryName, &limitPrefixLength)) {
        ಪ್ರತ್ಯಾಗಮ limit;
    }
    ಯಾವತ್(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, toc[i].entryName, &prefixLength);
        ಯದಿ(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } ಅನ್ಯಥಾ ಯದಿ(cmp==0) {
            ಪ್ರತ್ಯಾಗಮ i;
        } ಅನ್ಯಥಾ {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

U_CDECL_BEGIN
ಸ್ಥಿರ uint32_t U_CALLCONV
offset_TOCEntry_ಎಣಿಕೆ(ಧ್ರುವ UDataMemory *pData) {
    int32_t          retVal=0;
    ಧ್ರುವ UDataOffsetTOC *toc = (UDataOffsetTOC *)pData->toc;
    ಯದಿ (toc != nullptr) {
        retVal = toc->count;
    }
    ಪ್ರತ್ಯಾಗಮ retVal;
}

ಸ್ಥಿರ ಧ್ರುವ DataHeader * U_CALLCONV
offsetTOCLookupFn(ಧ್ರುವ UDataMemory *pData,
                  ಧ್ರುವ ವರ್ಣ *tocEntryName,
                  int32_t *pLength,
                  UErrorCode *pErrorCode) {
    (ಶೂನ್ಯ)pErrorCode;
    ಧ್ರುವ UDataOffsetTOC  *toc = (UDataOffsetTOC *)pData->toc;
    ಯದಿ(toc!=nullptr) {
        ಧ್ರುವ ವರ್ಣ *base=(ಧ್ರುವ ವರ್ಣ *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#ಯದಿ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        ಪ್ರತಿ(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &base[toc->entry[number].nameOffset]);
        }
#endif
        number=offset_TOCPrefix_ದ್ವಿಮಾನ_ಹುಡುಕು(tocEntryName, base, toc->entry, count);
        ಯದಿ(number>=0) {
            /* found it */
            ಧ್ರುವ UDataOffsetTOCEntry *entry=toc->entry+number;
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", tocEntryName);
#endif
            ಯದಿ((number+1) < count) {
                *pLength = (int32_t)(entry[1].dataOffset - entry->dataOffset);
            } ಅನ್ಯಥಾ {
                *pLength = -1;
            }
            ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ DataHeader *)(base+entry->dataOffset);
        } ಅನ್ಯಥಾ {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", tocEntryName);
#endif
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    } ಅನ್ಯಥಾ {
#ifdef UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#endif

        ಪ್ರತ್ಯಾಗಮ pData->pHeader;
    }
}


ಸ್ಥಿರ uint32_t U_CALLCONV pointer_TOCEntry_ಎಣಿಕೆ(ಧ್ರುವ UDataMemory *pData) {
    ಧ್ರುವ PointerTOC *toc = (PointerTOC *)pData->toc;
    ಪ್ರತ್ಯಾಗಮ toc != nullptr ? toc->count : 0;
}

ಸ್ಥಿರ ಧ್ರುವ DataHeader * U_CALLCONV pointerTOCLookupFn(ಧ್ರುವ UDataMemory *pData,
                   ಧ್ರುವ ವರ್ಣ *name,
                   int32_t *pLength,
                   UErrorCode *pErrorCode) {
    (ಶೂನ್ಯ)pErrorCode;
    ಯದಿ(pData->toc!=nullptr) {
        ಧ್ರುವ PointerTOC *toc = (PointerTOC *)pData->toc;
        int32_t number, count=(int32_t)toc->count;

#ಯದಿ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        ಪ್ರತಿ(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->entry[number].entryName);
        }
#endif
        number=pointer_TOCPrefix_ದ್ವಿಮಾನ_ಹುಡುಕು(name, toc->entry, count);
        ಯದಿ(number>=0) {
            /* found it */
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->entry[number].entryName);
#endif
            *pLength=-1;
            ಪ್ರತ್ಯಾಗಮ UDataMemory_normalizeDataPointer(toc->entry[number].pHeader);
        } ಅನ್ಯಥಾ {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#endif
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ pData->pHeader;
    }
}
U_CDECL_END


ಸ್ಥಿರ ಧ್ರುವ commonDataFuncs CmnDFuncs = {offsetTOCLookupFn,  offset_TOCEntry_ಎಣಿಕೆ};
ಸ್ಥಿರ ಧ್ರುವ commonDataFuncs ToCPFuncs = {pointerTOCLookupFn, pointer_TOCEntry_ಎಣಿಕೆ};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC ಶೂನ್ಯ udata_checkCommonData(UDataMemory *udm, UErrorCode *err) {
    ಯದಿ (U_FAILURE(*err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ(udm==nullptr || udm->pHeader==nullptr) {
      *err=U_INVALID_FORMAT_ERROR;
    } ಅನ್ಯಥಾ ಯದಿ(!(udm->pHeader->dataHeader.magic1==0xda &&
        udm->pHeader->dataHeader.magic2==0x27 &&
        udm->pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    ಅನ್ಯಥಾ ಯದಿ (udm->pHeader->info.dataFormat[0]==0x43 &&
        udm->pHeader->info.dataFormat[1]==0x6d &&
        udm->pHeader->info.dataFormat[2]==0x6e &&
        udm->pHeader->info.dataFormat[3]==0x44 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ಧ್ರುವ ವರ್ಣ *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    ಅನ್ಯಥಾ ಯದಿ(udm->pHeader->info.dataFormat[0]==0x54 &&
        udm->pHeader->info.dataFormat[1]==0x6f &&
        udm->pHeader->info.dataFormat[2]==0x43 &&
        udm->pHeader->info.dataFormat[3]==0x50 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ToCPFuncs;
        udm->toc=(ಧ್ರುವ ವರ್ಣ *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    ಅನ್ಯಥಾ {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    ಯದಿ (U_FAILURE(*err)) {
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

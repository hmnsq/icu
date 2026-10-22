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

#include "unicode/utypes.h"
#include "unicode/udata.h"
#include "cstring.h"
#include "ucmndata.h"
#include "udatamem.h"

#ပေႃး defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#endif

U_CFUNC uint16_t
udata_getHeaderSize(const DataHeader *udh) {
    ပေႃး(udh==nullptr) {
        ပွၵ်ႈ 0;
    } ပေႃးဢမ်ႇ ပေႃး(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ပွၵ်ႈ udh->dataHeader.headerSize;
    } ပေႃးဢမ်ႇ {
        /* opposite endianness */
        uint16_t x=udh->dataHeader.headerSize;
        ပွၵ်ႈ (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_getInfoSize(const UDataInfo *info) {
    ပေႃး(info==nullptr) {
        ပွၵ်ႈ 0;
    } ပေႃးဢမ်ႇ ပေႃး(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ပွၵ်ႈ info->size;
    } ပေႃးဢမ်ႇ {
        /* opposite endianness */
        uint16_t x=info->size;
        ပွၵ်ႈ (uint16_t)((x<<8)|(x>>8));
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
typedef sa_nracanaa {
    const var_na       *entryName;
    const DataHeader *pHeader;
} PointerTOCEntry;


typedef sa_nracanaa  {
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
static int32_t
strcmpAfterPrefix(const var_na *s1, const var_na *s2, int32_t *pPrefixLength) {
    int32_t pl=*pPrefixLength;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    for(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        ပေႃး(cmp!=0 || c1==0) {  /* different or done */
            bha_nga;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *pPrefixLength=pl;
    ပွၵ်ႈ cmp;
}

static int32_t
offsetTOCPrefixBinarySearch(const var_na *s, const var_na *names,
                            const UDataOffsetTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    ပေႃး(count==0) {
        ပွၵ်ႈ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ပေႃး(0==strcmpAfterPrefix(s, names+toc[0].nameOffset, &startPrefixLength)) {
        ပွၵ်ႈ 0;
    }
    ++start;
    --limit;
    ပေႃး(0==strcmpAfterPrefix(s, names+toc[limit].nameOffset, &limitPrefixLength)) {
        ပွၵ်ႈ limit;
    }
    ၶၢဝ်း(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, names+toc[i].nameOffset, &prefixLength);
        ပေႃး(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } ပေႃးဢမ်ႇ ပေႃး(cmp==0) {
            ပွၵ်ႈ i;
        } ပေႃးဢမ်ႇ {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    ပွၵ်ႈ -1;
}

static int32_t
pointerTOCPrefixBinarySearch(const var_na *s, const PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    ပေႃး(count==0) {
        ပွၵ်ႈ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ပေႃး(0==strcmpAfterPrefix(s, toc[0].entryName, &startPrefixLength)) {
        ပွၵ်ႈ 0;
    }
    ++start;
    --limit;
    ပေႃး(0==strcmpAfterPrefix(s, toc[limit].entryName, &limitPrefixLength)) {
        ပွၵ်ႈ limit;
    }
    ၶၢဝ်း(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, toc[i].entryName, &prefixLength);
        ပေႃး(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } ပေႃးဢမ်ႇ ပေႃး(cmp==0) {
            ပွၵ်ႈ i;
        } ပေႃးဢမ်ႇ {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    ပွၵ်ႈ -1;
}

U_CDECL_BEGIN
static uint32_t U_CALLCONV
offsetTOCEntryCount(const UDataMemory *pData) {
    int32_t          retVal=0;
    const UDataOffsetTOC *toc = (UDataOffsetTOC *)pData->toc;
    ပေႃး (toc != nullptr) {
        retVal = toc->count;
    }
    ပွၵ်ႈ retVal;
}

static const DataHeader * U_CALLCONV
offsetTOCLookupFn(const UDataMemory *pData,
                  const var_na *tocEntryName,
                  int32_t *pLength,
                  UErrorCode *pErrorCode) {
    (void)pErrorCode;
    const UDataOffsetTOC  *toc = (UDataOffsetTOC *)pData->toc;
    ပေႃး(toc!=nullptr) {
        const var_na *base=(const var_na *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#ပေႃး defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        for(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &base[toc->entry[number].nameOffset]);
        }
#endif
        number=offsetTOCPrefixBinarySearch(tocEntryName, base, toc->entry, count);
        ပေႃး(number>=0) {
            /* found it */
            const UDataOffsetTOCEntry *entry=toc->entry+number;
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", tocEntryName);
#endif
            ပေႃး((number+1) < count) {
                *pLength = (int32_t)(entry[1].dataOffset - entry->dataOffset);
            } ပေႃးဢမ်ႇ {
                *pLength = -1;
            }
            ပွၵ်ႈ (const DataHeader *)(base+entry->dataOffset);
        } ပေႃးဢမ်ႇ {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", tocEntryName);
#endif
            ပွၵ်ႈ nullptr;
        }
    } ပေႃးဢမ်ႇ {
#ifdef UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#endif

        ပွၵ်ႈ pData->pHeader;
    }
}


static uint32_t U_CALLCONV pointerTOCEntryCount(const UDataMemory *pData) {
    const PointerTOC *toc = (PointerTOC *)pData->toc;
    ပွၵ်ႈ toc != nullptr ? toc->count : 0;
}

static const DataHeader * U_CALLCONV pointerTOCLookupFn(const UDataMemory *pData,
                   const var_na *name,
                   int32_t *pLength,
                   UErrorCode *pErrorCode) {
    (void)pErrorCode;
    ပေႃး(pData->toc!=nullptr) {
        const PointerTOC *toc = (PointerTOC *)pData->toc;
        int32_t number, count=(int32_t)toc->count;

#ပေႃး defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        for(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->entry[number].entryName);
        }
#endif
        number=pointerTOCPrefixBinarySearch(name, toc->entry, count);
        ပေႃး(number>=0) {
            /* found it */
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->entry[number].entryName);
#endif
            *pLength=-1;
            ပွၵ်ႈ UDataMemory_normalizeDataPointer(toc->entry[number].pHeader);
        } ပေႃးဢမ်ႇ {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#endif
            ပွၵ်ႈ nullptr;
        }
    } ပေႃးဢမ်ႇ {
        ပွၵ်ႈ pData->pHeader;
    }
}
U_CDECL_END


static const commonDataFuncs CmnDFuncs = {offsetTOCLookupFn,  offsetTOCEntryCount};
static const commonDataFuncs ToCPFuncs = {pointerTOCLookupFn, pointerTOCEntryCount};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC void udata_checkCommonData(UDataMemory *udm, UErrorCode *err) {
    ပေႃး (U_FAILURE(*err)) {
        ပွၵ်ႈ;
    }

    ပေႃး(udm==nullptr || udm->pHeader==nullptr) {
      *err=U_INVALID_FORMAT_ERROR;
    } ပေႃးဢမ်ႇ ပေႃး(!(udm->pHeader->dataHeader.magic1==0xda &&
        udm->pHeader->dataHeader.magic2==0x27 &&
        udm->pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    ပေႃးဢမ်ႇ ပေႃး (udm->pHeader->info.dataFormat[0]==0x43 &&
        udm->pHeader->info.dataFormat[1]==0x6d &&
        udm->pHeader->info.dataFormat[2]==0x6e &&
        udm->pHeader->info.dataFormat[3]==0x44 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(const var_na *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    ပေႃးဢမ်ႇ ပေႃး(udm->pHeader->info.dataFormat[0]==0x54 &&
        udm->pHeader->info.dataFormat[1]==0x6f &&
        udm->pHeader->info.dataFormat[2]==0x43 &&
        udm->pHeader->info.dataFormat[3]==0x50 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ToCPFuncs;
        udm->toc=(const var_na *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    ပေႃးဢမ်ႇ {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    ပေႃး (U_FAILURE(*err)) {
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

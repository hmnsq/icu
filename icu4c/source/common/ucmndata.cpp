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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "ucmndata.h"
#ଅନ୍ତର୍ଭାଵ "udatamem.h"

#ଯଦି defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#endif

U_CFUNC uint16_t
udata_getHeaderSize(ଅଚଳ DataHeader *udh) {
    ଯଦି(udh==nullptr) {
        ଫେରନ୍ତୁ 0;
    } ଅନ୍ୟଥା ଯଦି(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ଫେରନ୍ତୁ udh->dataHeader.headerSize;
    } ଅନ୍ୟଥା {
        /* opposite endianness */
        uint16_t x=udh->dataHeader.headerSize;
        ଫେରନ୍ତୁ (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_getInfoSize(ଅଚଳ UDataInfo *info) {
    ଯଦି(info==nullptr) {
        ଫେରନ୍ତୁ 0;
    } ଅନ୍ୟଥା ଯଦି(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ଫେରନ୍ତୁ info->size;
    } ଅନ୍ୟଥା {
        /* opposite endianness */
        uint16_t x=info->size;
        ଫେରନ୍ତୁ (uint16_t)((x<<8)|(x>>8));
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅଚଳ ଅକ୍ଷର       *entryName;
    ଅଚଳ DataHeader *pHeader;
} PointerTOCEntry;


ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା  {
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
ସ୍ଥିର int32_t
strcmpAfterPrefix(ଅଚଳ ଅକ୍ଷର *s1, ଅଚଳ ଅକ୍ଷର *s2, int32_t *pPrefixLength) {
    int32_t pl=*pPrefixLength;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    ପାଇଁ(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        ଯଦି(cmp!=0 || c1==0) {  /* different or done */
            ଭାଙ୍ଗ;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *pPrefixLength=pl;
    ଫେରନ୍ତୁ cmp;
}

ସ୍ଥିର int32_t
offset_TOCPrefix_ଦ୍ୱିମିକ_ସନ୍ଧାନ(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *names,
                            ଅଚଳ UDataOffsetTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    ଯଦି(count==0) {
        ଫେରନ୍ତୁ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ଯଦି(0==strcmpAfterPrefix(s, names+toc[0].nameOffset, &startPrefixLength)) {
        ଫେରନ୍ତୁ 0;
    }
    ++start;
    --limit;
    ଯଦି(0==strcmpAfterPrefix(s, names+toc[limit].nameOffset, &limitPrefixLength)) {
        ଫେରନ୍ତୁ limit;
    }
    ଯେତେବେଳେ(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, names+toc[i].nameOffset, &prefixLength);
        ଯଦି(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } ଅନ୍ୟଥା ଯଦି(cmp==0) {
            ଫେରନ୍ତୁ i;
        } ଅନ୍ୟଥା {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର int32_t
pointer_TOCPrefix_ଦ୍ୱିମିକ_ସନ୍ଧାନ(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    ଯଦି(count==0) {
        ଫେରନ୍ତୁ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ଯଦି(0==strcmpAfterPrefix(s, toc[0].entryName, &startPrefixLength)) {
        ଫେରନ୍ତୁ 0;
    }
    ++start;
    --limit;
    ଯଦି(0==strcmpAfterPrefix(s, toc[limit].entryName, &limitPrefixLength)) {
        ଫେରନ୍ତୁ limit;
    }
    ଯେତେବେଳେ(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, toc[i].entryName, &prefixLength);
        ଯଦି(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } ଅନ୍ୟଥା ଯଦି(cmp==0) {
            ଫେରନ୍ତୁ i;
        } ଅନ୍ୟଥା {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    ଫେରନ୍ତୁ -1;
}

U_CDECL_BEGIN
ସ୍ଥିର uint32_t U_CALLCONV
offset_TOCEntry_ଗଣନା(ଅଚଳ UDataMemory *pData) {
    int32_t          retVal=0;
    ଅଚଳ UDataOffsetTOC *toc = (UDataOffsetTOC *)pData->toc;
    ଯଦି (toc != nullptr) {
        retVal = toc->count;
    }
    ଫେରନ୍ତୁ retVal;
}

ସ୍ଥିର ଅଚଳ DataHeader * U_CALLCONV
offsetTOCLookupFn(ଅଚଳ UDataMemory *pData,
                  ଅଚଳ ଅକ୍ଷର *tocEntryName,
                  int32_t *pLength,
                  UErrorCode *pErrorCode) {
    (ଶୂନ୍ୟ)pErrorCode;
    ଅଚଳ UDataOffsetTOC  *toc = (UDataOffsetTOC *)pData->toc;
    ଯଦି(toc!=nullptr) {
        ଅଚଳ ଅକ୍ଷର *base=(ଅଚଳ ଅକ୍ଷର *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#ଯଦି defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        ପାଇଁ(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &base[toc->entry[number].nameOffset]);
        }
#endif
        number=offset_TOCPrefix_ଦ୍ୱିମିକ_ସନ୍ଧାନ(tocEntryName, base, toc->entry, count);
        ଯଦି(number>=0) {
            /* found it */
            ଅଚଳ UDataOffsetTOCEntry *entry=toc->entry+number;
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", tocEntryName);
#endif
            ଯଦି((number+1) < count) {
                *pLength = (int32_t)(entry[1].dataOffset - entry->dataOffset);
            } ଅନ୍ୟଥା {
                *pLength = -1;
            }
            ଫେରନ୍ତୁ (ଅଚଳ DataHeader *)(base+entry->dataOffset);
        } ଅନ୍ୟଥା {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", tocEntryName);
#endif
            ଫେରନ୍ତୁ nullptr;
        }
    } ଅନ୍ୟଥା {
#ifdef UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#endif

        ଫେରନ୍ତୁ pData->pHeader;
    }
}


ସ୍ଥିର uint32_t U_CALLCONV pointer_TOCEntry_ଗଣନା(ଅଚଳ UDataMemory *pData) {
    ଅଚଳ PointerTOC *toc = (PointerTOC *)pData->toc;
    ଫେରନ୍ତୁ toc != nullptr ? toc->count : 0;
}

ସ୍ଥିର ଅଚଳ DataHeader * U_CALLCONV pointerTOCLookupFn(ଅଚଳ UDataMemory *pData,
                   ଅଚଳ ଅକ୍ଷର *name,
                   int32_t *pLength,
                   UErrorCode *pErrorCode) {
    (ଶୂନ୍ୟ)pErrorCode;
    ଯଦି(pData->toc!=nullptr) {
        ଅଚଳ PointerTOC *toc = (PointerTOC *)pData->toc;
        int32_t number, count=(int32_t)toc->count;

#ଯଦି defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        ପାଇଁ(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->entry[number].entryName);
        }
#endif
        number=pointer_TOCPrefix_ଦ୍ୱିମିକ_ସନ୍ଧାନ(name, toc->entry, count);
        ଯଦି(number>=0) {
            /* found it */
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->entry[number].entryName);
#endif
            *pLength=-1;
            ଫେରନ୍ତୁ UDataMemory_normalizeDataPointer(toc->entry[number].pHeader);
        } ଅନ୍ୟଥା {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#endif
            ଫେରନ୍ତୁ nullptr;
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ pData->pHeader;
    }
}
U_CDECL_END


ସ୍ଥିର ଅଚଳ commonDataFuncs CmnDFuncs = {offsetTOCLookupFn,  offset_TOCEntry_ଗଣନା};
ସ୍ଥିର ଅଚଳ commonDataFuncs ToCPFuncs = {pointerTOCLookupFn, pointer_TOCEntry_ଗଣନା};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC ଶୂନ୍ୟ udata_checkCommonData(UDataMemory *udm, UErrorCode *err) {
    ଯଦି (U_FAILURE(*err)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି(udm==nullptr || udm->pHeader==nullptr) {
      *err=U_INVALID_FORMAT_ERROR;
    } ଅନ୍ୟଥା ଯଦି(!(udm->pHeader->dataHeader.magic1==0xda &&
        udm->pHeader->dataHeader.magic2==0x27 &&
        udm->pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    ଅନ୍ୟଥା ଯଦି (udm->pHeader->info.dataFormat[0]==0x43 &&
        udm->pHeader->info.dataFormat[1]==0x6d &&
        udm->pHeader->info.dataFormat[2]==0x6e &&
        udm->pHeader->info.dataFormat[3]==0x44 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ଅଚଳ ଅକ୍ଷର *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    ଅନ୍ୟଥା ଯଦି(udm->pHeader->info.dataFormat[0]==0x54 &&
        udm->pHeader->info.dataFormat[1]==0x6f &&
        udm->pHeader->info.dataFormat[2]==0x43 &&
        udm->pHeader->info.dataFormat[3]==0x50 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ToCPFuncs;
        udm->toc=(ଅଚଳ ଅକ୍ଷର *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    ଅନ୍ୟଥା {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    ଯଦି (U_FAILURE(*err)) {
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

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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "ucmndata.h"
#அந்தர்பாவ "udatamem.h"

#எனில் defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#endif

U_CFUNC uint16_t
udata_getHeaderSize(மாறிலி DataHeader *udh) {
    எனில்(udh==nullptr) {
        திரும்பு 0;
    } இல்லையெனில் எனில்(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        திரும்பு udh->dataHeader.headerSize;
    } இல்லையெனில் {
        /* opposite endianness */
        uint16_t x=udh->dataHeader.headerSize;
        திரும்பு (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_getInfoSize(மாறிலி UDataInfo *info) {
    எனில்(info==nullptr) {
        திரும்பு 0;
    } இல்லையெனில் எனில்(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        திரும்பு info->size;
    } இல்லையெனில் {
        /* opposite endianness */
        uint16_t x=info->size;
        திரும்பு (uint16_t)((x<<8)|(x>>8));
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
வகைவரை கட்டமைப்பு {
    மாறிலி எழுத்து       *entryName;
    மாறிலி DataHeader *pHeader;
} PointerTOCEntry;


வகைவரை கட்டமைப்பு  {
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
நிலையான int32_t
strcmpAfterPrefix(மாறிலி எழுத்து *s1, மாறிலி எழுத்து *s2, int32_t *pPrefixLength) {
    int32_t pl=*pPrefixLength;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    க்கு(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        எனில்(cmp!=0 || c1==0) {  /* different or done */
            நிறுத்து;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *pPrefixLength=pl;
    திரும்பு cmp;
}

நிலையான int32_t
offset_TOCPrefix_இருமம்_தேடல்(மாறிலி எழுத்து *s, மாறிலி எழுத்து *names,
                            மாறிலி UDataOffsetTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    எனில்(count==0) {
        திரும்பு -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    எனில்(0==strcmpAfterPrefix(s, names+toc[0].nameOffset, &startPrefixLength)) {
        திரும்பு 0;
    }
    ++start;
    --limit;
    எனில்(0==strcmpAfterPrefix(s, names+toc[limit].nameOffset, &limitPrefixLength)) {
        திரும்பு limit;
    }
    போது(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, names+toc[i].nameOffset, &prefixLength);
        எனில்(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } இல்லையெனில் எனில்(cmp==0) {
            திரும்பு i;
        } இல்லையெனில் {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    திரும்பு -1;
}

நிலையான int32_t
pointer_TOCPrefix_இருமம்_தேடல்(மாறிலி எழுத்து *s, மாறிலி PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    எனில்(count==0) {
        திரும்பு -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    எனில்(0==strcmpAfterPrefix(s, toc[0].entryName, &startPrefixLength)) {
        திரும்பு 0;
    }
    ++start;
    --limit;
    எனில்(0==strcmpAfterPrefix(s, toc[limit].entryName, &limitPrefixLength)) {
        திரும்பு limit;
    }
    போது(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, toc[i].entryName, &prefixLength);
        எனில்(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } இல்லையெனில் எனில்(cmp==0) {
            திரும்பு i;
        } இல்லையெனில் {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    திரும்பு -1;
}

U_CDECL_BEGIN
நிலையான uint32_t U_CALLCONV
offset_TOCEntry_மொத்தம்(மாறிலி UDataMemory *pData) {
    int32_t          retVal=0;
    மாறிலி UDataOffsetTOC *toc = (UDataOffsetTOC *)pData->toc;
    எனில் (toc != nullptr) {
        retVal = toc->count;
    }
    திரும்பு retVal;
}

நிலையான மாறிலி DataHeader * U_CALLCONV
offsetTOCLookupFn(மாறிலி UDataMemory *pData,
                  மாறிலி எழுத்து *tocEntryName,
                  int32_t *pLength,
                  UErrorCode *pErrorCode) {
    (வெறுமை)pErrorCode;
    மாறிலி UDataOffsetTOC  *toc = (UDataOffsetTOC *)pData->toc;
    எனில்(toc!=nullptr) {
        மாறிலி எழுத்து *base=(மாறிலி எழுத்து *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#எனில் defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        க்கு(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &base[toc->entry[number].nameOffset]);
        }
#endif
        number=offset_TOCPrefix_இருமம்_தேடல்(tocEntryName, base, toc->entry, count);
        எனில்(number>=0) {
            /* found it */
            மாறிலி UDataOffsetTOCEntry *entry=toc->entry+number;
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", tocEntryName);
#endif
            எனில்((number+1) < count) {
                *pLength = (int32_t)(entry[1].dataOffset - entry->dataOffset);
            } இல்லையெனில் {
                *pLength = -1;
            }
            திரும்பு (மாறிலி DataHeader *)(base+entry->dataOffset);
        } இல்லையெனில் {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", tocEntryName);
#endif
            திரும்பு nullptr;
        }
    } இல்லையெனில் {
#ifdef UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#endif

        திரும்பு pData->pHeader;
    }
}


நிலையான uint32_t U_CALLCONV pointer_TOCEntry_மொத்தம்(மாறிலி UDataMemory *pData) {
    மாறிலி PointerTOC *toc = (PointerTOC *)pData->toc;
    திரும்பு toc != nullptr ? toc->count : 0;
}

நிலையான மாறிலி DataHeader * U_CALLCONV pointerTOCLookupFn(மாறிலி UDataMemory *pData,
                   மாறிலி எழுத்து *name,
                   int32_t *pLength,
                   UErrorCode *pErrorCode) {
    (வெறுமை)pErrorCode;
    எனில்(pData->toc!=nullptr) {
        மாறிலி PointerTOC *toc = (PointerTOC *)pData->toc;
        int32_t number, count=(int32_t)toc->count;

#எனில் defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        க்கு(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->entry[number].entryName);
        }
#endif
        number=pointer_TOCPrefix_இருமம்_தேடல்(name, toc->entry, count);
        எனில்(number>=0) {
            /* found it */
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->entry[number].entryName);
#endif
            *pLength=-1;
            திரும்பு UDataMemory_normalizeDataPointer(toc->entry[number].pHeader);
        } இல்லையெனில் {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#endif
            திரும்பு nullptr;
        }
    } இல்லையெனில் {
        திரும்பு pData->pHeader;
    }
}
U_CDECL_END


நிலையான மாறிலி commonDataFuncs CmnDFuncs = {offsetTOCLookupFn,  offset_TOCEntry_மொத்தம்};
நிலையான மாறிலி commonDataFuncs ToCPFuncs = {pointerTOCLookupFn, pointer_TOCEntry_மொத்தம்};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC வெறுமை udata_checkCommonData(UDataMemory *udm, UErrorCode *err) {
    எனில் (U_FAILURE(*err)) {
        திரும்பு;
    }

    எனில்(udm==nullptr || udm->pHeader==nullptr) {
      *err=U_INVALID_FORMAT_ERROR;
    } இல்லையெனில் எனில்(!(udm->pHeader->dataHeader.magic1==0xda &&
        udm->pHeader->dataHeader.magic2==0x27 &&
        udm->pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    இல்லையெனில் எனில் (udm->pHeader->info.dataFormat[0]==0x43 &&
        udm->pHeader->info.dataFormat[1]==0x6d &&
        udm->pHeader->info.dataFormat[2]==0x6e &&
        udm->pHeader->info.dataFormat[3]==0x44 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(மாறிலி எழுத்து *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    இல்லையெனில் எனில்(udm->pHeader->info.dataFormat[0]==0x54 &&
        udm->pHeader->info.dataFormat[1]==0x6f &&
        udm->pHeader->info.dataFormat[2]==0x43 &&
        udm->pHeader->info.dataFormat[3]==0x50 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ToCPFuncs;
        udm->toc=(மாறிலி எழுத்து *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    இல்லையெனில் {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    எனில் (U_FAILURE(*err)) {
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

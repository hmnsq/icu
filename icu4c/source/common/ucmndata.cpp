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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "ucmndata.h"
#അന്തര്ഭാവ "udatamem.h"

#എങ്കിൽ defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#endif

U_CFUNC uint16_t
udata_getHeaderSize(സ്ഥിരം DataHeader *udh) {
    എങ്കിൽ(udh==nullptr) {
        തിരികെ 0;
    } അല്ലെങ്കിൽ എങ്കിൽ(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        തിരികെ udh->dataHeader.headerSize;
    } അല്ലെങ്കിൽ {
        /* opposite endianness */
        uint16_t x=udh->dataHeader.headerSize;
        തിരികെ (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_getInfoSize(സ്ഥിരം UDataInfo *info) {
    എങ്കിൽ(info==nullptr) {
        തിരികെ 0;
    } അല്ലെങ്കിൽ എങ്കിൽ(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        തിരികെ info->size;
    } അല്ലെങ്കിൽ {
        /* opposite endianness */
        uint16_t x=info->size;
        തിരികെ (uint16_t)((x<<8)|(x>>8));
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
തരനിർവചനം ഘടന {
    സ്ഥിരം അക്ഷരം       *entryName;
    സ്ഥിരം DataHeader *pHeader;
} PointerTOCEntry;


തരനിർവചനം ഘടന  {
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
സ്ഥിതം int32_t
strcmpAfterPrefix(സ്ഥിരം അക്ഷരം *s1, സ്ഥിരം അക്ഷരം *s2, int32_t *pPrefixLength) {
    int32_t pl=*pPrefixLength;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    വേണ്ടി(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        എങ്കിൽ(cmp!=0 || c1==0) {  /* different or done */
            നിർത്തുക;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *pPrefixLength=pl;
    തിരികെ cmp;
}

സ്ഥിതം int32_t
offset_TOCPrefix_ദ്വിമാന_തിരയൽ(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *names,
                            സ്ഥിരം UDataOffsetTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    എങ്കിൽ(count==0) {
        തിരികെ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    എങ്കിൽ(0==strcmpAfterPrefix(s, names+toc[0].nameOffset, &startPrefixLength)) {
        തിരികെ 0;
    }
    ++start;
    --limit;
    എങ്കിൽ(0==strcmpAfterPrefix(s, names+toc[limit].nameOffset, &limitPrefixLength)) {
        തിരികെ limit;
    }
    ആകുമ്പോൾ(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, names+toc[i].nameOffset, &prefixLength);
        എങ്കിൽ(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } അല്ലെങ്കിൽ എങ്കിൽ(cmp==0) {
            തിരികെ i;
        } അല്ലെങ്കിൽ {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    തിരികെ -1;
}

സ്ഥിതം int32_t
pointer_TOCPrefix_ദ്വിമാന_തിരയൽ(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    എങ്കിൽ(count==0) {
        തിരികെ -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    എങ്കിൽ(0==strcmpAfterPrefix(s, toc[0].entryName, &startPrefixLength)) {
        തിരികെ 0;
    }
    ++start;
    --limit;
    എങ്കിൽ(0==strcmpAfterPrefix(s, toc[limit].entryName, &limitPrefixLength)) {
        തിരികെ limit;
    }
    ആകുമ്പോൾ(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, toc[i].entryName, &prefixLength);
        എങ്കിൽ(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } അല്ലെങ്കിൽ എങ്കിൽ(cmp==0) {
            തിരികെ i;
        } അല്ലെങ്കിൽ {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    തിരികെ -1;
}

U_CDECL_BEGIN
സ്ഥിതം uint32_t U_CALLCONV
offset_TOCEntry_എണ്ണം(സ്ഥിരം UDataMemory *pData) {
    int32_t          retVal=0;
    സ്ഥിരം UDataOffsetTOC *toc = (UDataOffsetTOC *)pData->toc;
    എങ്കിൽ (toc != nullptr) {
        retVal = toc->count;
    }
    തിരികെ retVal;
}

സ്ഥിതം സ്ഥിരം DataHeader * U_CALLCONV
offsetTOCLookupFn(സ്ഥിരം UDataMemory *pData,
                  സ്ഥിരം അക്ഷരം *tocEntryName,
                  int32_t *pLength,
                  UErrorCode *pErrorCode) {
    (ശൂന്യം)pErrorCode;
    സ്ഥിരം UDataOffsetTOC  *toc = (UDataOffsetTOC *)pData->toc;
    എങ്കിൽ(toc!=nullptr) {
        സ്ഥിരം അക്ഷരം *base=(സ്ഥിരം അക്ഷരം *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#എങ്കിൽ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        വേണ്ടി(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &base[toc->entry[number].nameOffset]);
        }
#endif
        number=offset_TOCPrefix_ദ്വിമാന_തിരയൽ(tocEntryName, base, toc->entry, count);
        എങ്കിൽ(number>=0) {
            /* found it */
            സ്ഥിരം UDataOffsetTOCEntry *entry=toc->entry+number;
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", tocEntryName);
#endif
            എങ്കിൽ((number+1) < count) {
                *pLength = (int32_t)(entry[1].dataOffset - entry->dataOffset);
            } അല്ലെങ്കിൽ {
                *pLength = -1;
            }
            തിരികെ (സ്ഥിരം DataHeader *)(base+entry->dataOffset);
        } അല്ലെങ്കിൽ {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", tocEntryName);
#endif
            തിരികെ nullptr;
        }
    } അല്ലെങ്കിൽ {
#ifdef UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#endif

        തിരികെ pData->pHeader;
    }
}


സ്ഥിതം uint32_t U_CALLCONV pointer_TOCEntry_എണ്ണം(സ്ഥിരം UDataMemory *pData) {
    സ്ഥിരം PointerTOC *toc = (PointerTOC *)pData->toc;
    തിരികെ toc != nullptr ? toc->count : 0;
}

സ്ഥിതം സ്ഥിരം DataHeader * U_CALLCONV pointerTOCLookupFn(സ്ഥിരം UDataMemory *pData,
                   സ്ഥിരം അക്ഷരം *name,
                   int32_t *pLength,
                   UErrorCode *pErrorCode) {
    (ശൂന്യം)pErrorCode;
    എങ്കിൽ(pData->toc!=nullptr) {
        സ്ഥിരം PointerTOC *toc = (PointerTOC *)pData->toc;
        int32_t number, count=(int32_t)toc->count;

#എങ്കിൽ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        വേണ്ടി(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->entry[number].entryName);
        }
#endif
        number=pointer_TOCPrefix_ദ്വിമാന_തിരയൽ(name, toc->entry, count);
        എങ്കിൽ(number>=0) {
            /* found it */
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->entry[number].entryName);
#endif
            *pLength=-1;
            തിരികെ UDataMemory_normalizeDataPointer(toc->entry[number].pHeader);
        } അല്ലെങ്കിൽ {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#endif
            തിരികെ nullptr;
        }
    } അല്ലെങ്കിൽ {
        തിരികെ pData->pHeader;
    }
}
U_CDECL_END


സ്ഥിതം സ്ഥിരം commonDataFuncs CmnDFuncs = {offsetTOCLookupFn,  offset_TOCEntry_എണ്ണം};
സ്ഥിതം സ്ഥിരം commonDataFuncs ToCPFuncs = {pointerTOCLookupFn, pointer_TOCEntry_എണ്ണം};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC ശൂന്യം udata_checkCommonData(UDataMemory *udm, UErrorCode *err) {
    എങ്കിൽ (U_FAILURE(*err)) {
        തിരികെ;
    }

    എങ്കിൽ(udm==nullptr || udm->pHeader==nullptr) {
      *err=U_INVALID_FORMAT_ERROR;
    } അല്ലെങ്കിൽ എങ്കിൽ(!(udm->pHeader->dataHeader.magic1==0xda &&
        udm->pHeader->dataHeader.magic2==0x27 &&
        udm->pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (udm->pHeader->info.dataFormat[0]==0x43 &&
        udm->pHeader->info.dataFormat[1]==0x6d &&
        udm->pHeader->info.dataFormat[2]==0x6e &&
        udm->pHeader->info.dataFormat[3]==0x44 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(സ്ഥിരം അക്ഷരം *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(udm->pHeader->info.dataFormat[0]==0x54 &&
        udm->pHeader->info.dataFormat[1]==0x6f &&
        udm->pHeader->info.dataFormat[2]==0x43 &&
        udm->pHeader->info.dataFormat[3]==0x50 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ToCPFuncs;
        udm->toc=(സ്ഥിരം അക്ഷരം *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    അല്ലെങ്കിൽ {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    എങ്കിൽ (U_FAILURE(*err)) {
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

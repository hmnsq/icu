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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ucmndata.h"
#अन्तर्भाव "udatamem.h"

#यदि defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#endif

U_CFUNC uint16_t
udata_getHeaderSize(ध्रुव DataHeader *udh) {
    यदि(udh==nullptr) {
        प्रत्यागम 0;
    } अन्यथा यदि(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        प्रत्यागम udh->dataHeader.headerSize;
    } अन्यथा {
        /* opposite endianness */
        uint16_t x=udh->dataHeader.headerSize;
        प्रत्यागम (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_getInfoSize(ध्रुव UDataInfo *info) {
    यदि(info==nullptr) {
        प्रत्यागम 0;
    } अन्यथा यदि(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        प्रत्यागम info->size;
    } अन्यथा {
        /* opposite endianness */
        uint16_t x=info->size;
        प्रत्यागम (uint16_t)((x<<8)|(x>>8));
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
प्रकारनिर्देश संरचना {
    ध्रुव वर्ण       *entryName;
    ध्रुव DataHeader *pHeader;
} PointerTOCEntry;


प्रकारनिर्देश संरचना  {
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
स्थिर int32_t
strcmpAfterPrefix(ध्रुव वर्ण *s1, ध्रुव वर्ण *s2, int32_t *pPrefixLength) {
    int32_t pl=*pPrefixLength;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    प्रति(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        यदि(cmp!=0 || c1==0) {  /* different or done */
            भङ्ग;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *pPrefixLength=pl;
    प्रत्यागम cmp;
}

स्थिर int32_t
offset_TOCPrefix_द्विआधारी_खोज(ध्रुव वर्ण *s, ध्रुव वर्ण *names,
                            ध्रुव UDataOffsetTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    यदि(count==0) {
        प्रत्यागम -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    यदि(0==strcmpAfterPrefix(s, names+toc[0].nameOffset, &startPrefixLength)) {
        प्रत्यागम 0;
    }
    ++start;
    --limit;
    यदि(0==strcmpAfterPrefix(s, names+toc[limit].nameOffset, &limitPrefixLength)) {
        प्रत्यागम limit;
    }
    यावत्(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, names+toc[i].nameOffset, &prefixLength);
        यदि(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } अन्यथा यदि(cmp==0) {
            प्रत्यागम i;
        } अन्यथा {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    प्रत्यागम -1;
}

स्थिर int32_t
pointer_TOCPrefix_द्विआधारी_खोज(ध्रुव वर्ण *s, ध्रुव PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t startPrefixLength=0;
    int32_t limitPrefixLength=0;
    यदि(count==0) {
        प्रत्यागम -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    यदि(0==strcmpAfterPrefix(s, toc[0].entryName, &startPrefixLength)) {
        प्रत्यागम 0;
    }
    ++start;
    --limit;
    यदि(0==strcmpAfterPrefix(s, toc[limit].entryName, &limitPrefixLength)) {
        प्रत्यागम limit;
    }
    यावत्(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefixLength=MIN(startPrefixLength, limitPrefixLength);
        int32_t cmp=strcmpAfterPrefix(s, toc[i].entryName, &prefixLength);
        यदि(cmp<0) {
            limit=i;
            limitPrefixLength=prefixLength;
        } अन्यथा यदि(cmp==0) {
            प्रत्यागम i;
        } अन्यथा {
            start=i+1;
            startPrefixLength=prefixLength;
        }
    }
    प्रत्यागम -1;
}

U_CDECL_BEGIN
स्थिर uint32_t U_CALLCONV
offset_TOCEntry_गिनती(ध्रुव UDataMemory *pData) {
    int32_t          retVal=0;
    ध्रुव UDataOffsetTOC *toc = (UDataOffsetTOC *)pData->toc;
    यदि (toc != nullptr) {
        retVal = toc->count;
    }
    प्रत्यागम retVal;
}

स्थिर ध्रुव DataHeader * U_CALLCONV
offsetTOCLookupFn(ध्रुव UDataMemory *pData,
                  ध्रुव वर्ण *tocEntryName,
                  int32_t *pLength,
                  UErrorCode *pErrorCode) {
    (शून्य)pErrorCode;
    ध्रुव UDataOffsetTOC  *toc = (UDataOffsetTOC *)pData->toc;
    यदि(toc!=nullptr) {
        ध्रुव वर्ण *base=(ध्रुव वर्ण *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#यदि defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        प्रति(number=0; number<count; ++number) {
            ख_लिखो(मानक_त्रुटि, "\tx%d: %s\n", number, &base[toc->entry[number].nameOffset]);
        }
#endif
        number=offset_TOCPrefix_द्विआधारी_खोज(tocEntryName, base, toc->entry, count);
        यदि(number>=0) {
            /* found it */
            ध्रुव UDataOffsetTOCEntry *entry=toc->entry+number;
#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: Found.\n", tocEntryName);
#endif
            यदि((number+1) < count) {
                *pLength = (int32_t)(entry[1].dataOffset - entry->dataOffset);
            } अन्यथा {
                *pLength = -1;
            }
            प्रत्यागम (ध्रुव DataHeader *)(base+entry->dataOffset);
        } अन्यथा {
#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: Not found.\n", tocEntryName);
#endif
            प्रत्यागम nullptr;
        }
    } अन्यथा {
#ifdef UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "returning header\n");
#endif

        प्रत्यागम pData->pHeader;
    }
}


स्थिर uint32_t U_CALLCONV pointer_TOCEntry_गिनती(ध्रुव UDataMemory *pData) {
    ध्रुव PointerTOC *toc = (PointerTOC *)pData->toc;
    प्रत्यागम toc != nullptr ? toc->count : 0;
}

स्थिर ध्रुव DataHeader * U_CALLCONV pointerTOCLookupFn(ध्रुव UDataMemory *pData,
                   ध्रुव वर्ण *name,
                   int32_t *pLength,
                   UErrorCode *pErrorCode) {
    (शून्य)pErrorCode;
    यदि(pData->toc!=nullptr) {
        ध्रुव PointerTOC *toc = (PointerTOC *)pData->toc;
        int32_t number, count=(int32_t)toc->count;

#यदि defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        प्रति(number=0; number<count; ++number) {
            ख_लिखो(मानक_त्रुटि, "\tx%d: %s\n", number, toc->entry[number].entryName);
        }
#endif
        number=pointer_TOCPrefix_द्विआधारी_खोज(name, toc->entry, count);
        यदि(number>=0) {
            /* found it */
#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: Found.\n", toc->entry[number].entryName);
#endif
            *pLength=-1;
            प्रत्यागम UDataMemory_normalizeDataPointer(toc->entry[number].pHeader);
        } अन्यथा {
#ifdef UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: Not found.\n", name);
#endif
            प्रत्यागम nullptr;
        }
    } अन्यथा {
        प्रत्यागम pData->pHeader;
    }
}
U_CDECL_END


स्थिर ध्रुव commonDataFuncs CmnDFuncs = {offsetTOCLookupFn,  offset_TOCEntry_गिनती};
स्थिर ध्रुव commonDataFuncs ToCPFuncs = {pointerTOCLookupFn, pointer_TOCEntry_गिनती};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC शून्य udata_checkCommonData(UDataMemory *udm, UErrorCode *err) {
    यदि (U_FAILURE(*err)) {
        प्रत्यागम;
    }

    यदि(udm==nullptr || udm->pHeader==nullptr) {
      *err=U_INVALID_FORMAT_ERROR;
    } अन्यथा यदि(!(udm->pHeader->dataHeader.magic1==0xda &&
        udm->pHeader->dataHeader.magic2==0x27 &&
        udm->pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    अन्यथा यदि (udm->pHeader->info.dataFormat[0]==0x43 &&
        udm->pHeader->info.dataFormat[1]==0x6d &&
        udm->pHeader->info.dataFormat[2]==0x6e &&
        udm->pHeader->info.dataFormat[3]==0x44 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ध्रुव वर्ण *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    अन्यथा यदि(udm->pHeader->info.dataFormat[0]==0x54 &&
        udm->pHeader->info.dataFormat[1]==0x6f &&
        udm->pHeader->info.dataFormat[2]==0x43 &&
        udm->pHeader->info.dataFormat[3]==0x50 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ToCPFuncs;
        udm->toc=(ध्रुव वर्ण *)udm->pHeader+udata_getHeaderSize(udm->pHeader);
    }
    अन्यथा {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    यदि (U_FAILURE(*err)) {
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

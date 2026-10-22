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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "ucmndata.h"
#অন্তর্ভাব "udatamem.h"

#যদি defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#endif

U_CFUNC uint16_t
udata_get_Header_আকার(ধ্রুবক তথ্য_Header *udh) {
    যদি(udh==nullptr) {
        ফেরত 0;
    } অন্যথা যদি(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ফেরত udh->তথ্য_Header_2.header_আকার;
    } অন্যথা {
        /* opposite endianness */
        uint16_t x=udh->তথ্য_Header_2.header_আকার;
        ফেরত (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_আকার(ধ্রুবক UDataInfo *info) {
    যদি(info==nullptr) {
        ফেরত 0;
    } অন্যথা যদি(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        ফেরত info->size;
    } অন্যথা {
        /* opposite endianness */
        uint16_t x=info->size;
        ফেরত (uint16_t)((x<<8)|(x>>8));
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
প্রকারলেখ কাঠামো {
    ধ্রুবক অক্ষর       *entryName;
    ধ্রুবক তথ্য_Header *pHeader;
} PointerTOCEntry;


প্রকারলেখ কাঠামো  {
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
স্থির int32_t
strcmpAfterPrefix(ধ্রুবক অক্ষর *s1, ধ্রুবক অক্ষর *s2, int32_t *p_Prefix_দৈর্ঘ্য) {
    int32_t pl=*p_Prefix_দৈর্ঘ্য;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    জন্য(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        যদি(cmp!=0 || c1==0) {  /* different or done */
            বিরতি;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *p_Prefix_দৈর্ঘ্য=pl;
    ফেরত cmp;
}

স্থির int32_t
offset_TOCPrefix_দ্বিমিক_অনুসন্ধান(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *names,
                            ধ্রুবক UDataOffsetTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t start_Prefix_দৈর্ঘ্য=0;
    int32_t limit_Prefix_দৈর্ঘ্য=0;
    যদি(count==0) {
        ফেরত -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    যদি(0==strcmpAfterPrefix(s, names+toc[0].nameOffset, &start_Prefix_দৈর্ঘ্য)) {
        ফেরত 0;
    }
    ++start;
    --limit;
    যদি(0==strcmpAfterPrefix(s, names+toc[limit].nameOffset, &limit_Prefix_দৈর্ঘ্য)) {
        ফেরত limit;
    }
    যখন(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefix_দৈর্ঘ্য=MIN(start_Prefix_দৈর্ঘ্য, limit_Prefix_দৈর্ঘ্য);
        int32_t cmp=strcmpAfterPrefix(s, names+toc[i].nameOffset, &prefix_দৈর্ঘ্য);
        যদি(cmp<0) {
            limit=i;
            limit_Prefix_দৈর্ঘ্য=prefix_দৈর্ঘ্য;
        } অন্যথা যদি(cmp==0) {
            ফেরত i;
        } অন্যথা {
            start=i+1;
            start_Prefix_দৈর্ঘ্য=prefix_দৈর্ঘ্য;
        }
    }
    ফেরত -1;
}

স্থির int32_t
pointer_TOCPrefix_দ্বিমিক_অনুসন্ধান(ধ্রুবক অক্ষর *s, ধ্রুবক PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t start_Prefix_দৈর্ঘ্য=0;
    int32_t limit_Prefix_দৈর্ঘ্য=0;
    যদি(count==0) {
        ফেরত -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    যদি(0==strcmpAfterPrefix(s, toc[0].entryName, &start_Prefix_দৈর্ঘ্য)) {
        ফেরত 0;
    }
    ++start;
    --limit;
    যদি(0==strcmpAfterPrefix(s, toc[limit].entryName, &limit_Prefix_দৈর্ঘ্য)) {
        ফেরত limit;
    }
    যখন(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefix_দৈর্ঘ্য=MIN(start_Prefix_দৈর্ঘ্য, limit_Prefix_দৈর্ঘ্য);
        int32_t cmp=strcmpAfterPrefix(s, toc[i].entryName, &prefix_দৈর্ঘ্য);
        যদি(cmp<0) {
            limit=i;
            limit_Prefix_দৈর্ঘ্য=prefix_দৈর্ঘ্য;
        } অন্যথা যদি(cmp==0) {
            ফেরত i;
        } অন্যথা {
            start=i+1;
            start_Prefix_দৈর্ঘ্য=prefix_দৈর্ঘ্য;
        }
    }
    ফেরত -1;
}

U_CDECL_BEGIN
স্থির uint32_t U_CALLCONV
offset_TOCEntry_গুনতি(ধ্রুবক UDataMemory *p_তথ্য) {
    int32_t          retVal=0;
    ধ্রুবক UDataOffsetTOC *toc = (UDataOffsetTOC *)p_তথ্য->toc;
    যদি (toc != nullptr) {
        retVal = toc->count;
    }
    ফেরত retVal;
}

স্থির ধ্রুবক তথ্য_Header * U_CALLCONV
offsetTOCLookupFn(ধ্রুবক UDataMemory *p_তথ্য,
                  ধ্রুবক অক্ষর *tocEntryName,
                  int32_t *p_দৈর্ঘ্য,
                  UErrorCode *pErrorCode) {
    (শূন্য)pErrorCode;
    ধ্রুবক UDataOffsetTOC  *toc = (UDataOffsetTOC *)p_তথ্য->toc;
    যদি(toc!=nullptr) {
        ধ্রুবক অক্ষর *base=(ধ্রুবক অক্ষর *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#যদি defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        জন্য(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &base[toc->entry[number].nameOffset]);
        }
#endif
        number=offset_TOCPrefix_দ্বিমিক_অনুসন্ধান(tocEntryName, base, toc->entry, count);
        যদি(number>=0) {
            /* found it */
            ধ্রুবক UDataOffsetTOCEntry *entry=toc->entry+number;
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", tocEntryName);
#endif
            যদি((number+1) < count) {
                *p_দৈর্ঘ্য = (int32_t)(entry[1].তথ্য_Offset - entry->তথ্য_Offset);
            } অন্যথা {
                *p_দৈর্ঘ্য = -1;
            }
            ফেরত (ধ্রুবক তথ্য_Header *)(base+entry->তথ্য_Offset);
        } অন্যথা {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", tocEntryName);
#endif
            ফেরত nullptr;
        }
    } অন্যথা {
#ifdef UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#endif

        ফেরত p_তথ্য->pHeader;
    }
}


স্থির uint32_t U_CALLCONV pointer_TOCEntry_গুনতি(ধ্রুবক UDataMemory *p_তথ্য) {
    ধ্রুবক PointerTOC *toc = (PointerTOC *)p_তথ্য->toc;
    ফেরত toc != nullptr ? toc->count : 0;
}

স্থির ধ্রুবক তথ্য_Header * U_CALLCONV pointerTOCLookupFn(ধ্রুবক UDataMemory *p_তথ্য,
                   ধ্রুবক অক্ষর *name,
                   int32_t *p_দৈর্ঘ্য,
                   UErrorCode *pErrorCode) {
    (শূন্য)pErrorCode;
    যদি(p_তথ্য->toc!=nullptr) {
        ধ্রুবক PointerTOC *toc = (PointerTOC *)p_তথ্য->toc;
        int32_t number, count=(int32_t)toc->count;

#যদি defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        জন্য(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->entry[number].entryName);
        }
#endif
        number=pointer_TOCPrefix_দ্বিমিক_অনুসন্ধান(name, toc->entry, count);
        যদি(number>=0) {
            /* found it */
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->entry[number].entryName);
#endif
            *p_দৈর্ঘ্য=-1;
            ফেরত UData_Memory_normalize_তথ্য_Pointer(toc->entry[number].pHeader);
        } অন্যথা {
#ifdef UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#endif
            ফেরত nullptr;
        }
    } অন্যথা {
        ফেরত p_তথ্য->pHeader;
    }
}
U_CDECL_END


স্থির ধ্রুবক common_তথ্য_Funcs CmnDFuncs = {offsetTOCLookupFn,  offset_TOCEntry_গুনতি};
স্থির ধ্রুবক common_তথ্য_Funcs ToCPFuncs = {pointerTOCLookupFn, pointer_TOCEntry_গুনতি};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC শূন্য udata_check_Common_তথ্য(UDataMemory *udm, UErrorCode *err) {
    যদি (U_FAILURE(*err)) {
        ফেরত;
    }

    যদি(udm==nullptr || udm->pHeader==nullptr) {
      *err=U_INVALID_FORMAT_ERROR;
    } অন্যথা যদি(!(udm->pHeader->তথ্য_Header_2.magic1==0xda &&
        udm->pHeader->তথ্য_Header_2.magic2==0x27 &&
        udm->pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    অন্যথা যদি (udm->pHeader->info.তথ্য_Format[0]==0x43 &&
        udm->pHeader->info.তথ্য_Format[1]==0x6d &&
        udm->pHeader->info.তথ্য_Format[2]==0x6e &&
        udm->pHeader->info.তথ্য_Format[3]==0x44 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ধ্রুবক অক্ষর *)udm->pHeader+udata_get_Header_আকার(udm->pHeader);
    }
    অন্যথা যদি(udm->pHeader->info.তথ্য_Format[0]==0x54 &&
        udm->pHeader->info.তথ্য_Format[1]==0x6f &&
        udm->pHeader->info.তথ্য_Format[2]==0x43 &&
        udm->pHeader->info.তথ্য_Format[3]==0x50 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ToCPFuncs;
        udm->toc=(ধ্রুবক অক্ষর *)udm->pHeader+udata_get_Header_আকার(udm->pHeader);
    }
    অন্যথা {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    যদি (U_FAILURE(*err)) {
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

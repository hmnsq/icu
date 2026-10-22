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

#समावेश "unicode/utypes.h"
#समावेश "unicode/udata.h"
#समावेश "cstring.h"
#समावेश "ucmndata.h"
#समावेश "udatamem.h"

#अगर defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#पूर्ण_अगर

U_CFUNC uint16_t
udata_get_Header_आकार(स्थिर तथ्य_Header *udh) {
    अगर(udh==nullptr) {
        वापस 0;
    } अन्यथा अगर(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        वापस udh->तथ्य_Header_2.header_आकार;
    } अन्यथा {
        /* opposite endianness */
        uint16_t x=udh->तथ्य_Header_2.header_आकार;
        वापस (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_आकार(स्थिर UDataInfo *info) {
    अगर(info==nullptr) {
        वापस 0;
    } अन्यथा अगर(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        वापस info->size;
    } अन्यथा {
        /* opposite endianness */
        uint16_t x=info->size;
        वापस (uint16_t)((x<<8)|(x>>8));
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
प्रकार काष्ठा {
    स्थिर अक्षर       *entryName;
    स्थिर तथ्य_Header *pHeader;
} PointerTOCEntry;


प्रकार काष्ठा  {
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

#अगर_अघोषित MIN
#घोषणा MIN(a,b) (((a)<(b)) ? (a) : (b))
#पूर्ण_अगर

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
अटल int32_t
strcmpAfterPrefix(स्थिर अक्षर *s1, स्थिर अक्षर *s2, int32_t *p_Prefix_लंबाई) {
    int32_t pl=*p_Prefix_लंबाई;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    क्रम(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        अगर(cmp!=0 || c1==0) {  /* different or done */
            अवरोध;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *p_Prefix_लंबाई=pl;
    वापस cmp;
}

अटल int32_t
offset_TOCPrefix_द्विआधारी_खोज(स्थिर अक्षर *s, स्थिर अक्षर *names,
                            स्थिर UDataOffsetTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t start_Prefix_लंबाई=0;
    int32_t limit_Prefix_लंबाई=0;
    अगर(count==0) {
        वापस -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    अगर(0==strcmpAfterPrefix(s, names+toc[0].nameOffset, &start_Prefix_लंबाई)) {
        वापस 0;
    }
    ++start;
    --limit;
    अगर(0==strcmpAfterPrefix(s, names+toc[limit].nameOffset, &limit_Prefix_लंबाई)) {
        वापस limit;
    }
    जबतक(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefix_लंबाई=MIN(start_Prefix_लंबाई, limit_Prefix_लंबाई);
        int32_t cmp=strcmpAfterPrefix(s, names+toc[i].nameOffset, &prefix_लंबाई);
        अगर(cmp<0) {
            limit=i;
            limit_Prefix_लंबाई=prefix_लंबाई;
        } अन्यथा अगर(cmp==0) {
            वापस i;
        } अन्यथा {
            start=i+1;
            start_Prefix_लंबाई=prefix_लंबाई;
        }
    }
    वापस -1;
}

अटल int32_t
pointer_TOCPrefix_द्विआधारी_खोज(स्थिर अक्षर *s, स्थिर PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t limit=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t start_Prefix_लंबाई=0;
    int32_t limit_Prefix_लंबाई=0;
    अगर(count==0) {
        वापस -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    अगर(0==strcmpAfterPrefix(s, toc[0].entryName, &start_Prefix_लंबाई)) {
        वापस 0;
    }
    ++start;
    --limit;
    अगर(0==strcmpAfterPrefix(s, toc[limit].entryName, &limit_Prefix_लंबाई)) {
        वापस limit;
    }
    जबतक(start<limit) {
        int32_t i=(start+limit)/2;
        int32_t prefix_लंबाई=MIN(start_Prefix_लंबाई, limit_Prefix_लंबाई);
        int32_t cmp=strcmpAfterPrefix(s, toc[i].entryName, &prefix_लंबाई);
        अगर(cmp<0) {
            limit=i;
            limit_Prefix_लंबाई=prefix_लंबाई;
        } अन्यथा अगर(cmp==0) {
            वापस i;
        } अन्यथा {
            start=i+1;
            start_Prefix_लंबाई=prefix_लंबाई;
        }
    }
    वापस -1;
}

U_CDECL_BEGIN
अटल uint32_t U_CALLCONV
offset_TOCEntry_गिनती(स्थिर UDataMemory *p_तथ्य) {
    int32_t          retVal=0;
    स्थिर UDataOffsetTOC *toc = (UDataOffsetTOC *)p_तथ्य->toc;
    अगर (toc != nullptr) {
        retVal = toc->count;
    }
    वापस retVal;
}

अटल स्थिर तथ्य_Header * U_CALLCONV
offsetTOCLookupFn(स्थिर UDataMemory *p_तथ्य,
                  स्थिर अक्षर *tocEntryName,
                  int32_t *p_लंबाई,
                  UErrorCode *pErrorCode) {
    (व्योम)pErrorCode;
    स्थिर UDataOffsetTOC  *toc = (UDataOffsetTOC *)p_तथ्य->toc;
    अगर(toc!=nullptr) {
        स्थिर अक्षर *base=(स्थिर अक्षर *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#अगर defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        क्रम(number=0; number<count; ++number) {
            ख_लिखो(मानक_त्रुटि, "\tx%d: %s\n", number, &base[toc->entry[number].nameOffset]);
        }
#पूर्ण_अगर
        number=offset_TOCPrefix_द्विआधारी_खोज(tocEntryName, base, toc->entry, count);
        अगर(number>=0) {
            /* found it */
            स्थिर UDataOffsetTOCEntry *entry=toc->entry+number;
#अगर_घोषित UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: Found.\n", tocEntryName);
#पूर्ण_अगर
            अगर((number+1) < count) {
                *p_लंबाई = (int32_t)(entry[1].तथ्य_Offset - entry->तथ्य_Offset);
            } अन्यथा {
                *p_लंबाई = -1;
            }
            वापस (स्थिर तथ्य_Header *)(base+entry->तथ्य_Offset);
        } अन्यथा {
#अगर_घोषित UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: Not found.\n", tocEntryName);
#पूर्ण_अगर
            वापस nullptr;
        }
    } अन्यथा {
#अगर_घोषित UDATA_DEBUG
        ख_लिखो(मानक_त्रुटि, "returning header\n");
#पूर्ण_अगर

        वापस p_तथ्य->pHeader;
    }
}


अटल uint32_t U_CALLCONV pointer_TOCEntry_गिनती(स्थिर UDataMemory *p_तथ्य) {
    स्थिर PointerTOC *toc = (PointerTOC *)p_तथ्य->toc;
    वापस toc != nullptr ? toc->count : 0;
}

अटल स्थिर तथ्य_Header * U_CALLCONV pointerTOCLookupFn(स्थिर UDataMemory *p_तथ्य,
                   स्थिर अक्षर *name,
                   int32_t *p_लंबाई,
                   UErrorCode *pErrorCode) {
    (व्योम)pErrorCode;
    अगर(p_तथ्य->toc!=nullptr) {
        स्थिर PointerTOC *toc = (PointerTOC *)p_तथ्य->toc;
        int32_t number, count=(int32_t)toc->count;

#अगर defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        क्रम(number=0; number<count; ++number) {
            ख_लिखो(मानक_त्रुटि, "\tx%d: %s\n", number, toc->entry[number].entryName);
        }
#पूर्ण_अगर
        number=pointer_TOCPrefix_द्विआधारी_खोज(name, toc->entry, count);
        अगर(number>=0) {
            /* found it */
#अगर_घोषित UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: Found.\n", toc->entry[number].entryName);
#पूर्ण_अगर
            *p_लंबाई=-1;
            वापस UData_Memory_normalize_तथ्य_Pointer(toc->entry[number].pHeader);
        } अन्यथा {
#अगर_घोषित UDATA_DEBUG
            ख_लिखो(मानक_त्रुटि, "%s: Not found.\n", name);
#पूर्ण_अगर
            वापस nullptr;
        }
    } अन्यथा {
        वापस p_तथ्य->pHeader;
    }
}
U_CDECL_END


अटल स्थिर common_तथ्य_Funcs CmnDFuncs = {offsetTOCLookupFn,  offset_TOCEntry_गिनती};
अटल स्थिर common_तथ्य_Funcs ToCPFuncs = {pointerTOCLookupFn, pointer_TOCEntry_गिनती};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC व्योम udata_check_Common_तथ्य(UDataMemory *udm, UErrorCode *err) {
    अगर (U_FAILURE(*err)) {
        वापस;
    }

    अगर(udm==nullptr || udm->pHeader==nullptr) {
      *err=U_INVALID_FORMAT_ERROR;
    } अन्यथा अगर(!(udm->pHeader->तथ्य_Header_2.magic1==0xda &&
        udm->pHeader->तथ्य_Header_2.magic2==0x27 &&
        udm->pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    अन्यथा अगर (udm->pHeader->info.तथ्य_Format[0]==0x43 &&
        udm->pHeader->info.तथ्य_Format[1]==0x6d &&
        udm->pHeader->info.तथ्य_Format[2]==0x6e &&
        udm->pHeader->info.तथ्य_Format[3]==0x44 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(स्थिर अक्षर *)udm->pHeader+udata_get_Header_आकार(udm->pHeader);
    }
    अन्यथा अगर(udm->pHeader->info.तथ्य_Format[0]==0x54 &&
        udm->pHeader->info.तथ्य_Format[1]==0x6f &&
        udm->pHeader->info.तथ्य_Format[2]==0x43 &&
        udm->pHeader->info.तथ्य_Format[3]==0x50 &&
        udm->pHeader->info.formatVersion[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &ToCPFuncs;
        udm->toc=(स्थिर अक्षर *)udm->pHeader+udata_get_Header_आकार(udm->pHeader);
    }
    अन्यथा {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    अगर (U_FAILURE(*err)) {
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

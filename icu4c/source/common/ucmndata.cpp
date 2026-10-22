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
#यदि_अन्तः

U_CFUNC uint16_t
udata_get_शीर्षकम्_आकार(ध्रुव दत्त_शीर्षकम् *udh) {
    यदि(udh==शून्यसूचकः) {
        प्रत्यागम 0;
    } अन्यथा यदि(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        प्रत्यागम udh->दत्त_शीर्षकम्_2.शीर्षकम्_आकार;
    } अन्यथा {
        /* opposite endianness */
        uint16_t x=udh->दत्त_शीर्षकम्_2.शीर्षकम्_आकार;
        प्रत्यागम (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_आकार(ध्रुव UDataInfo *info) {
    यदि(info==शून्यसूचकः) {
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
    ध्रुव वर्ण       *प्रविष्टिः_Name;
    ध्रुव दत्त_शीर्षकम् *p_शीर्षकम्;
} PointerTOCEntry;


प्रकारनिर्देश संरचना  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    PointerTOCEntry   प्रविष्टिः[1];
}  PointerTOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#यदि_अपरिभाषितम् MIN
#परिभाषय MIN(a,b) (((a)<(b)) ? (a) : (b))
#यदि_अन्तः

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
स्थिर int32_t
strcmp_After_उपसर्गः(ध्रुव वर्ण *s1, ध्रुव वर्ण *s2, int32_t *p_उपसर्गः_दीर्घता) {
    int32_t pl=*p_उपसर्गः_दीर्घता;
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
    *p_उपसर्गः_दीर्घता=pl;
    प्रत्यागम cmp;
}

स्थिर int32_t
विस्थापनम्_TOCPrefix_द्विमान_अन्वेषणम्(ध्रुव वर्ण *s, ध्रुव वर्ण *नामानि,
                            ध्रुव UData_विस्थापनम्_TOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t सीमा=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t start_उपसर्गः_दीर्घता=0;
    int32_t सीमा_उपसर्गः_दीर्घता=0;
    यदि(count==0) {
        प्रत्यागम -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    यदि(0==strcmp_After_उपसर्गः(s, नामानि+toc[0].name_विस्थापनम्, &start_उपसर्गः_दीर्घता)) {
        प्रत्यागम 0;
    }
    ++start;
    --सीमा;
    यदि(0==strcmp_After_उपसर्गः(s, नामानि+toc[सीमा].name_विस्थापनम्, &सीमा_उपसर्गः_दीर्घता)) {
        प्रत्यागम सीमा;
    }
    यावत्(start<सीमा) {
        int32_t i=(start+सीमा)/2;
        int32_t उपसर्गः_दीर्घता=MIN(start_उपसर्गः_दीर्घता, सीमा_उपसर्गः_दीर्घता);
        int32_t cmp=strcmp_After_उपसर्गः(s, नामानि+toc[i].name_विस्थापनम्, &उपसर्गः_दीर्घता);
        यदि(cmp<0) {
            सीमा=i;
            सीमा_उपसर्गः_दीर्घता=उपसर्गः_दीर्घता;
        } अन्यथा यदि(cmp==0) {
            प्रत्यागम i;
        } अन्यथा {
            start=i+1;
            start_उपसर्गः_दीर्घता=उपसर्गः_दीर्घता;
        }
    }
    प्रत्यागम -1;
}

स्थिर int32_t
pointer_TOCPrefix_द्विमान_अन्वेषणम्(ध्रुव वर्ण *s, ध्रुव PointerTOCEntry *toc, int32_t count) {
    int32_t start=0;
    int32_t सीमा=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t start_उपसर्गः_दीर्घता=0;
    int32_t सीमा_उपसर्गः_दीर्घता=0;
    यदि(count==0) {
        प्रत्यागम -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    यदि(0==strcmp_After_उपसर्गः(s, toc[0].प्रविष्टिः_Name, &start_उपसर्गः_दीर्घता)) {
        प्रत्यागम 0;
    }
    ++start;
    --सीमा;
    यदि(0==strcmp_After_उपसर्गः(s, toc[सीमा].प्रविष्टिः_Name, &सीमा_उपसर्गः_दीर्घता)) {
        प्रत्यागम सीमा;
    }
    यावत्(start<सीमा) {
        int32_t i=(start+सीमा)/2;
        int32_t उपसर्गः_दीर्घता=MIN(start_उपसर्गः_दीर्घता, सीमा_उपसर्गः_दीर्घता);
        int32_t cmp=strcmp_After_उपसर्गः(s, toc[i].प्रविष्टिः_Name, &उपसर्गः_दीर्घता);
        यदि(cmp<0) {
            सीमा=i;
            सीमा_उपसर्गः_दीर्घता=उपसर्गः_दीर्घता;
        } अन्यथा यदि(cmp==0) {
            प्रत्यागम i;
        } अन्यथा {
            start=i+1;
            start_उपसर्गः_दीर्घता=उपसर्गः_दीर्घता;
        }
    }
    प्रत्यागम -1;
}

U_CDECL_BEGIN
स्थिर uint32_t U_CALLCONV
विस्थापनम्_TOCEntry_गणनम्(ध्रुव UData_स्मृतिः *p_दत्त) {
    int32_t          retVal=0;
    ध्रुव UData_विस्थापनम्_TOC *toc = (UData_विस्थापनम्_TOC *)p_दत्त->toc;
    यदि (toc != शून्यसूचकः) {
        retVal = toc->count;
    }
    प्रत्यागम retVal;
}

स्थिर ध्रुव दत्त_शीर्षकम् * U_CALLCONV
विस्थापनम्_TOCLookup_Fn(ध्रुव UData_स्मृतिः *p_दत्त,
                  ध्रुव वर्ण *toc_प्रविष्टिः_Name,
                  int32_t *p_दीर्घता,
                  UError_संहिता *p_Error_संहिता) {
    (शून्य)p_Error_संहिता;
    ध्रुव UData_विस्थापनम्_TOC  *toc = (UData_विस्थापनम्_TOC *)p_दत्त->toc;
    यदि(toc!=शून्यसूचकः) {
        ध्रुव वर्ण *आधारः=(ध्रुव वर्ण *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#यदि defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        प्रति(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &आधारः[toc->प्रविष्टिः[number].name_विस्थापनम्]);
        }
#यदि_अन्तः
        number=विस्थापनम्_TOCPrefix_द्विमान_अन्वेषणम्(toc_प्रविष्टिः_Name, आधारः, toc->प्रविष्टिः, count);
        यदि(number>=0) {
            /* found it */
            ध्रुव UData_विस्थापनम्_TOCEntry *प्रविष्टिः=toc->प्रविष्टिः+number;
#यदि_परिभाषितम् UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_प्रविष्टिः_Name);
#यदि_अन्तः
            यदि((number+1) < count) {
                *p_दीर्घता = (int32_t)(प्रविष्टिः[1].दत्त_विस्थापनम् - प्रविष्टिः->दत्त_विस्थापनम्);
            } अन्यथा {
                *p_दीर्घता = -1;
            }
            प्रत्यागम (ध्रुव दत्त_शीर्षकम् *)(आधारः+प्रविष्टिः->दत्त_विस्थापनम्);
        } अन्यथा {
#यदि_परिभाषितम् UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_प्रविष्टिः_Name);
#यदि_अन्तः
            प्रत्यागम शून्यसूचकः;
        }
    } अन्यथा {
#यदि_परिभाषितम् UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#यदि_अन्तः

        प्रत्यागम p_दत्त->p_शीर्षकम्;
    }
}


स्थिर uint32_t U_CALLCONV pointer_TOCEntry_गणनम्(ध्रुव UData_स्मृतिः *p_दत्त) {
    ध्रुव PointerTOC *toc = (PointerTOC *)p_दत्त->toc;
    प्रत्यागम toc != शून्यसूचकः ? toc->count : 0;
}

स्थिर ध्रुव दत्त_शीर्षकम् * U_CALLCONV pointerTOCLookupFn(ध्रुव UData_स्मृतिः *p_दत्त,
                   ध्रुव वर्ण *name,
                   int32_t *p_दीर्घता,
                   UError_संहिता *p_Error_संहिता) {
    (शून्य)p_Error_संहिता;
    यदि(p_दत्त->toc!=शून्यसूचकः) {
        ध्रुव PointerTOC *toc = (PointerTOC *)p_दत्त->toc;
        int32_t number, count=(int32_t)toc->count;

#यदि defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        प्रति(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->प्रविष्टिः[number].प्रविष्टिः_Name);
        }
#यदि_अन्तः
        number=pointer_TOCPrefix_द्विमान_अन्वेषणम्(name, toc->प्रविष्टिः, count);
        यदि(number>=0) {
            /* found it */
#यदि_परिभाषितम् UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->प्रविष्टिः[number].प्रविष्टिः_Name);
#यदि_अन्तः
            *p_दीर्घता=-1;
            प्रत्यागम UData_स्मृतिः_normalize_दत्त_Pointer(toc->प्रविष्टिः[number].p_शीर्षकम्);
        } अन्यथा {
#यदि_परिभाषितम् UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#यदि_अन्तः
            प्रत्यागम शून्यसूचकः;
        }
    } अन्यथा {
        प्रत्यागम p_दत्त->p_शीर्षकम्;
    }
}
U_CDECL_अन्तः


स्थिर ध्रुव common_दत्त_Funcs CmnDFuncs = {विस्थापनम्_TOCLookup_Fn,  विस्थापनम्_TOCEntry_गणनम्};
स्थिर ध्रुव common_दत्त_Funcs गन्तव्यम्_CPFuncs = {pointerTOCLookupFn, pointer_TOCEntry_गणनम्};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC शून्य udata_check_Common_दत्त(UData_स्मृतिः *udm, UError_संहिता *err) {
    यदि (U_FAILURE(*err)) {
        प्रत्यागम;
    }

    यदि(udm==शून्यसूचकः || udm->p_शीर्षकम्==शून्यसूचकः) {
      *err=U_INVALID_FORMAT_ERROR;
    } अन्यथा यदि(!(udm->p_शीर्षकम्->दत्त_शीर्षकम्_2.magic1==0xda &&
        udm->p_शीर्षकम्->दत्त_शीर्षकम्_2.magic2==0x27 &&
        udm->p_शीर्षकम्->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_शीर्षकम्->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    अन्यथा यदि (udm->p_शीर्षकम्->info.दत्त_Format[0]==0x43 &&
        udm->p_शीर्षकम्->info.दत्त_Format[1]==0x6d &&
        udm->p_शीर्षकम्->info.दत्त_Format[2]==0x6e &&
        udm->p_शीर्षकम्->info.दत्त_Format[3]==0x44 &&
        udm->p_शीर्षकम्->info.format_संस्करणम्[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ध्रुव वर्ण *)udm->p_शीर्षकम्+udata_get_शीर्षकम्_आकार(udm->p_शीर्षकम्);
    }
    अन्यथा यदि(udm->p_शीर्षकम्->info.दत्त_Format[0]==0x54 &&
        udm->p_शीर्षकम्->info.दत्त_Format[1]==0x6f &&
        udm->p_शीर्षकम्->info.दत्त_Format[2]==0x43 &&
        udm->p_शीर्षकम्->info.दत्त_Format[3]==0x50 &&
        udm->p_शीर्षकम्->info.format_संस्करणम्[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &गन्तव्यम्_CPFuncs;
        udm->toc=(ध्रुव वर्ण *)udm->p_शीर्षकम्+udata_get_शीर्षकम्_आकार(udm->p_शीर्षकम्);
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

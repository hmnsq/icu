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

#ضمن "unicode/utypes.h"
#ضمن "unicode/udata.h"
#ضمن "cstring.h"
#ضمن "ucmndata.h"
#ضمن "udatamem.h"

#إذا defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#جيڪڏهن_پورو

U_CFUNC uint16_t
udata_get_سرو_سائيز(ثابت معلومات_سرو *udh) {
    إذا(udh==خالي_اشارو) {
        أرجع 0;
    } وإلا إذا(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        أرجع udh->معلومات_سرو_2.سرو_سائيز;
    } وإلا {
        /* opposite endianness */
        uint16_t x=udh->معلومات_سرو_2.سرو_سائيز;
        أرجع (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_سائيز(ثابت UDataInfo *info) {
    إذا(info==خالي_اشارو) {
        أرجع 0;
    } وإلا إذا(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        أرجع info->size;
    } وإلا {
        /* opposite endianness */
        uint16_t x=info->size;
        أرجع (uint16_t)((x<<8)|(x>>8));
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
تعريفنوع بنية {
    ثابت حرف       *داخلا_Name;
    ثابت معلومات_سرو *p_سرو;
} اشارو_TOCEntry;


تعريفنوع بنية  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    اشارو_TOCEntry   داخلا_2[1];
}  اشارو_TOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#جيڪڏهن_وصف_نه_ٿيل MIN
#وصف_ڪريو MIN(a,b) (((a)<(b)) ? (a) : (b))
#جيڪڏهن_پورو

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
ساكن int32_t
strcmp_After_اڳياڙي(ثابت حرف *s1, ثابت حرف *s2, int32_t *p_اڳياڙي_ڊيگهائي) {
    int32_t pl=*p_اڳياڙي_ڊيگهائي;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    لكل(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        إذا(cmp!=0 || c1==0) {  /* different or done */
            اكسر;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *p_اڳياڙي_ڊيگهائي=pl;
    أرجع cmp;
}

ساكن int32_t
وٿي_TOCPrefix_ٻٽو_ڳولا(ثابت حرف *s, ثابت حرف *نالا,
                            ثابت UData_وٿي_TOCEntry *toc, int32_t count) {
    int32_t شروعات=0;
    int32_t حد_بندي=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t شروعات_اڳياڙي_ڊيگهائي=0;
    int32_t حد_بندي_اڳياڙي_ڊيگهائي=0;
    إذا(count==0) {
        أرجع -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    إذا(0==strcmp_After_اڳياڙي(s, نالا+toc[0].name_وٿي, &شروعات_اڳياڙي_ڊيگهائي)) {
        أرجع 0;
    }
    ++شروعات;
    --حد_بندي;
    إذا(0==strcmp_After_اڳياڙي(s, نالا+toc[حد_بندي].name_وٿي, &حد_بندي_اڳياڙي_ڊيگهائي)) {
        أرجع حد_بندي;
    }
    طالما(شروعات<حد_بندي) {
        int32_t i=(شروعات+حد_بندي)/2;
        int32_t اڳياڙي_ڊيگهائي=MIN(شروعات_اڳياڙي_ڊيگهائي, حد_بندي_اڳياڙي_ڊيگهائي);
        int32_t cmp=strcmp_After_اڳياڙي(s, نالا+toc[i].name_وٿي, &اڳياڙي_ڊيگهائي);
        إذا(cmp<0) {
            حد_بندي=i;
            حد_بندي_اڳياڙي_ڊيگهائي=اڳياڙي_ڊيگهائي;
        } وإلا إذا(cmp==0) {
            أرجع i;
        } وإلا {
            شروعات=i+1;
            شروعات_اڳياڙي_ڊيگهائي=اڳياڙي_ڊيگهائي;
        }
    }
    أرجع -1;
}

ساكن int32_t
اشارو_TOCPrefix_ٻٽو_ڳولا(ثابت حرف *s, ثابت اشارو_TOCEntry *toc, int32_t count) {
    int32_t شروعات=0;
    int32_t حد_بندي=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t شروعات_اڳياڙي_ڊيگهائي=0;
    int32_t حد_بندي_اڳياڙي_ڊيگهائي=0;
    إذا(count==0) {
        أرجع -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    إذا(0==strcmp_After_اڳياڙي(s, toc[0].داخلا_Name, &شروعات_اڳياڙي_ڊيگهائي)) {
        أرجع 0;
    }
    ++شروعات;
    --حد_بندي;
    إذا(0==strcmp_After_اڳياڙي(s, toc[حد_بندي].داخلا_Name, &حد_بندي_اڳياڙي_ڊيگهائي)) {
        أرجع حد_بندي;
    }
    طالما(شروعات<حد_بندي) {
        int32_t i=(شروعات+حد_بندي)/2;
        int32_t اڳياڙي_ڊيگهائي=MIN(شروعات_اڳياڙي_ڊيگهائي, حد_بندي_اڳياڙي_ڊيگهائي);
        int32_t cmp=strcmp_After_اڳياڙي(s, toc[i].داخلا_Name, &اڳياڙي_ڊيگهائي);
        إذا(cmp<0) {
            حد_بندي=i;
            حد_بندي_اڳياڙي_ڊيگهائي=اڳياڙي_ڊيگهائي;
        } وإلا إذا(cmp==0) {
            أرجع i;
        } وإلا {
            شروعات=i+1;
            شروعات_اڳياڙي_ڊيگهائي=اڳياڙي_ڊيگهائي;
        }
    }
    أرجع -1;
}

U_CDECL_BEGIN
ساكن uint32_t U_CALLCONV
وٿي_TOCEntry_ڳڻپ(ثابت UData_يادگيري *p_معلومات) {
    int32_t          retVal=0;
    ثابت UData_وٿي_TOC *toc = (UData_وٿي_TOC *)p_معلومات->toc;
    إذا (toc != خالي_اشارو) {
        retVal = toc->count;
    }
    أرجع retVal;
}

ساكن ثابت معلومات_سرو * U_CALLCONV
وٿي_TOCLookup_Fn(ثابت UData_يادگيري *p_معلومات,
                  ثابت حرف *toc_داخلا_Name,
                  int32_t *p_ڊيگهائي,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    (فراغ)p_Error_ڪوڊ;
    ثابت UData_وٿي_TOC  *toc = (UData_وٿي_TOC *)p_معلومات->toc;
    إذا(toc!=خالي_اشارو) {
        ثابت حرف *بنياد=(ثابت حرف *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#إذا defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        لكل(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &بنياد[toc->داخلا_2[number].name_وٿي]);
        }
#جيڪڏهن_پورو
        number=وٿي_TOCPrefix_ٻٽو_ڳولا(toc_داخلا_Name, بنياد, toc->داخلا_2, count);
        إذا(number>=0) {
            /* found it */
            ثابت UData_وٿي_TOCEntry *داخلا_2=toc->داخلا_2+number;
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_داخلا_Name);
#جيڪڏهن_پورو
            إذا((number+1) < count) {
                *p_ڊيگهائي = (int32_t)(داخلا_2[1].معلومات_وٿي - داخلا_2->معلومات_وٿي);
            } وإلا {
                *p_ڊيگهائي = -1;
            }
            أرجع (ثابت معلومات_سرو *)(بنياد+داخلا_2->معلومات_وٿي);
        } وإلا {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_داخلا_Name);
#جيڪڏهن_پورو
            أرجع خالي_اشارو;
        }
    } وإلا {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#جيڪڏهن_پورو

        أرجع p_معلومات->p_سرو;
    }
}


ساكن uint32_t U_CALLCONV اشارو_TOCEntry_ڳڻپ(ثابت UData_يادگيري *p_معلومات) {
    ثابت اشارو_TOC *toc = (اشارو_TOC *)p_معلومات->toc;
    أرجع toc != خالي_اشارو ? toc->count : 0;
}

ساكن ثابت معلومات_سرو * U_CALLCONV اشارو_TOCLookup_Fn(ثابت UData_يادگيري *p_معلومات,
                   ثابت حرف *name,
                   int32_t *p_ڊيگهائي,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    (فراغ)p_Error_ڪوڊ;
    إذا(p_معلومات->toc!=خالي_اشارو) {
        ثابت اشارو_TOC *toc = (اشارو_TOC *)p_معلومات->toc;
        int32_t number, count=(int32_t)toc->count;

#إذا defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        لكل(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->داخلا_2[number].داخلا_Name);
        }
#جيڪڏهن_پورو
        number=اشارو_TOCPrefix_ٻٽو_ڳولا(name, toc->داخلا_2, count);
        إذا(number>=0) {
            /* found it */
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->داخلا_2[number].داخلا_Name);
#جيڪڏهن_پورو
            *p_ڊيگهائي=-1;
            أرجع UData_يادگيري_normalize_معلومات_اشارو(toc->داخلا_2[number].p_سرو);
        } وإلا {
#جيڪڏهن_وصف_ٿيل UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#جيڪڏهن_پورو
            أرجع خالي_اشارو;
        }
    } وإلا {
        أرجع p_معلومات->p_سرو;
    }
}
U_CDECL_پڇاڙي


ساكن ثابت common_معلومات_Funcs CmnDFuncs = {وٿي_TOCLookup_Fn,  وٿي_TOCEntry_ڳڻپ};
ساكن ثابت common_معلومات_Funcs منزل_CPFuncs = {اشارو_TOCLookup_Fn, اشارو_TOCEntry_ڳڻپ};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC فراغ udata_check_Common_معلومات(UData_يادگيري *udm, UError_ڪوڊ *err) {
    إذا (U_FAILURE(*err)) {
        أرجع;
    }

    إذا(udm==خالي_اشارو || udm->p_سرو==خالي_اشارو) {
      *err=U_INVALID_FORMAT_ERROR;
    } وإلا إذا(!(udm->p_سرو->معلومات_سرو_2.magic1==0xda &&
        udm->p_سرو->معلومات_سرو_2.magic2==0x27 &&
        udm->p_سرو->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_سرو->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    وإلا إذا (udm->p_سرو->info.معلومات_Format[0]==0x43 &&
        udm->p_سرو->info.معلومات_Format[1]==0x6d &&
        udm->p_سرو->info.معلومات_Format[2]==0x6e &&
        udm->p_سرو->info.معلومات_Format[3]==0x44 &&
        udm->p_سرو->info.format_نسخو[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرو+udata_get_سرو_سائيز(udm->p_سرو);
    }
    وإلا إذا(udm->p_سرو->info.معلومات_Format[0]==0x54 &&
        udm->p_سرو->info.معلومات_Format[1]==0x6f &&
        udm->p_سرو->info.معلومات_Format[2]==0x43 &&
        udm->p_سرو->info.معلومات_Format[3]==0x50 &&
        udm->p_سرو->info.format_نسخو[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &منزل_CPFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرو+udata_get_سرو_سائيز(udm->p_سرو);
    }
    وإلا {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    إذا (U_FAILURE(*err)) {
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

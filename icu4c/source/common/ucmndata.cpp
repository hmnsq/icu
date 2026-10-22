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
#جے_انت

U_CFUNC uint16_t
udata_get_سرخی_ناپ(ثابت انکڑے_سرخی *udh) {
    إذا(udh==خالی_سوچک) {
        أرجع 0;
    } وإلا إذا(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        أرجع udh->انکڑے_سرخی_2.سرخی_ناپ;
    } وإلا {
        /* opposite endianness */
        uint16_t x=udh->انکڑے_سرخی_2.سرخی_ناپ;
        أرجع (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_ناپ(ثابت UDataInfo *info) {
    إذا(info==خالی_سوچک) {
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
    ثابت حرف       *اندراج_Name;
    ثابت انکڑے_سرخی *p_سرخی;
} اشارہ_گر_TOCEntry;


تعريفنوع بنية  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    اشارہ_گر_TOCEntry   اندراج[1];
}  اشارہ_گر_TOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#جے_بنا_تعریف MIN
#تعریف_کرو MIN(a,b) (((a)<(b)) ? (a) : (b))
#جے_انت

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
ساكن int32_t
strcmp_After_اگیتر(ثابت حرف *s1, ثابت حرف *s2, int32_t *p_اگیتر_لمائی) {
    int32_t pl=*p_اگیتر_لمائی;
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
    *p_اگیتر_لمائی=pl;
    أرجع cmp;
}

ساكن int32_t
وتھ_TOCPrefix_ثنائی_تلاش(ثابت حرف *s, ثابت حرف *ناں,
                            ثابت UData_وتھ_TOCEntry *toc, int32_t count) {
    int32_t شروع=0;
    int32_t حد_بندی=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t شروع_اگیتر_لمائی=0;
    int32_t حد_بندی_اگیتر_لمائی=0;
    إذا(count==0) {
        أرجع -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    إذا(0==strcmp_After_اگیتر(s, ناں+toc[0].name_وتھ, &شروع_اگیتر_لمائی)) {
        أرجع 0;
    }
    ++شروع;
    --حد_بندی;
    إذا(0==strcmp_After_اگیتر(s, ناں+toc[حد_بندی].name_وتھ, &حد_بندی_اگیتر_لمائی)) {
        أرجع حد_بندی;
    }
    طالما(شروع<حد_بندی) {
        int32_t i=(شروع+حد_بندی)/2;
        int32_t اگیتر_لمائی=MIN(شروع_اگیتر_لمائی, حد_بندی_اگیتر_لمائی);
        int32_t cmp=strcmp_After_اگیتر(s, ناں+toc[i].name_وتھ, &اگیتر_لمائی);
        إذا(cmp<0) {
            حد_بندی=i;
            حد_بندی_اگیتر_لمائی=اگیتر_لمائی;
        } وإلا إذا(cmp==0) {
            أرجع i;
        } وإلا {
            شروع=i+1;
            شروع_اگیتر_لمائی=اگیتر_لمائی;
        }
    }
    أرجع -1;
}

ساكن int32_t
اشارہ_گر_TOCPrefix_ثنائی_تلاش(ثابت حرف *s, ثابت اشارہ_گر_TOCEntry *toc, int32_t count) {
    int32_t شروع=0;
    int32_t حد_بندی=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t شروع_اگیتر_لمائی=0;
    int32_t حد_بندی_اگیتر_لمائی=0;
    إذا(count==0) {
        أرجع -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    إذا(0==strcmp_After_اگیتر(s, toc[0].اندراج_Name, &شروع_اگیتر_لمائی)) {
        أرجع 0;
    }
    ++شروع;
    --حد_بندی;
    إذا(0==strcmp_After_اگیتر(s, toc[حد_بندی].اندراج_Name, &حد_بندی_اگیتر_لمائی)) {
        أرجع حد_بندی;
    }
    طالما(شروع<حد_بندی) {
        int32_t i=(شروع+حد_بندی)/2;
        int32_t اگیتر_لمائی=MIN(شروع_اگیتر_لمائی, حد_بندی_اگیتر_لمائی);
        int32_t cmp=strcmp_After_اگیتر(s, toc[i].اندراج_Name, &اگیتر_لمائی);
        إذا(cmp<0) {
            حد_بندی=i;
            حد_بندی_اگیتر_لمائی=اگیتر_لمائی;
        } وإلا إذا(cmp==0) {
            أرجع i;
        } وإلا {
            شروع=i+1;
            شروع_اگیتر_لمائی=اگیتر_لمائی;
        }
    }
    أرجع -1;
}

U_CDECL_BEGIN
ساكن uint32_t U_CALLCONV
وتھ_TOCEntry_گنتی(ثابت UData_حافظہ *p_انکڑے) {
    int32_t          retVal=0;
    ثابت UData_وتھ_TOC *toc = (UData_وتھ_TOC *)p_انکڑے->toc;
    إذا (toc != خالی_سوچک) {
        retVal = toc->count;
    }
    أرجع retVal;
}

ساكن ثابت انکڑے_سرخی * U_CALLCONV
وتھ_TOCLookup_Fn(ثابت UData_حافظہ *p_انکڑے,
                  ثابت حرف *toc_اندراج_Name,
                  int32_t *p_لمائی,
                  UError_کوڈ *p_Error_کوڈ) {
    (فراغ)p_Error_کوڈ;
    ثابت UData_وتھ_TOC  *toc = (UData_وتھ_TOC *)p_انکڑے->toc;
    إذا(toc!=خالی_سوچک) {
        ثابت حرف *ادھار=(ثابت حرف *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#إذا defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        لكل(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &ادھار[toc->اندراج[number].name_وتھ]);
        }
#جے_انت
        number=وتھ_TOCPrefix_ثنائی_تلاش(toc_اندراج_Name, ادھار, toc->اندراج, count);
        إذا(number>=0) {
            /* found it */
            ثابت UData_وتھ_TOCEntry *اندراج=toc->اندراج+number;
#جے_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_اندراج_Name);
#جے_انت
            إذا((number+1) < count) {
                *p_لمائی = (int32_t)(اندراج[1].انکڑے_وتھ - اندراج->انکڑے_وتھ);
            } وإلا {
                *p_لمائی = -1;
            }
            أرجع (ثابت انکڑے_سرخی *)(ادھار+اندراج->انکڑے_وتھ);
        } وإلا {
#جے_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_اندراج_Name);
#جے_انت
            أرجع خالی_سوچک;
        }
    } وإلا {
#جے_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#جے_انت

        أرجع p_انکڑے->p_سرخی;
    }
}


ساكن uint32_t U_CALLCONV اشارہ_گر_TOCEntry_گنتی(ثابت UData_حافظہ *p_انکڑے) {
    ثابت اشارہ_گر_TOC *toc = (اشارہ_گر_TOC *)p_انکڑے->toc;
    أرجع toc != خالی_سوچک ? toc->count : 0;
}

ساكن ثابت انکڑے_سرخی * U_CALLCONV اشارہ_گر_TOCLookup_Fn(ثابت UData_حافظہ *p_انکڑے,
                   ثابت حرف *name,
                   int32_t *p_لمائی,
                   UError_کوڈ *p_Error_کوڈ) {
    (فراغ)p_Error_کوڈ;
    إذا(p_انکڑے->toc!=خالی_سوچک) {
        ثابت اشارہ_گر_TOC *toc = (اشارہ_گر_TOC *)p_انکڑے->toc;
        int32_t number, count=(int32_t)toc->count;

#إذا defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        لكل(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->اندراج[number].اندراج_Name);
        }
#جے_انت
        number=اشارہ_گر_TOCPrefix_ثنائی_تلاش(name, toc->اندراج, count);
        إذا(number>=0) {
            /* found it */
#جے_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->اندراج[number].اندراج_Name);
#جے_انت
            *p_لمائی=-1;
            أرجع UData_حافظہ_normalize_انکڑے_اشارہ_گر(toc->اندراج[number].p_سرخی);
        } وإلا {
#جے_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#جے_انت
            أرجع خالی_سوچک;
        }
    } وإلا {
        أرجع p_انکڑے->p_سرخی;
    }
}
U_CDECL_سرا


ساكن ثابت common_انکڑے_Funcs CmnDFuncs = {وتھ_TOCLookup_Fn,  وتھ_TOCEntry_گنتی};
ساكن ثابت common_انکڑے_Funcs منزل_CPFuncs = {اشارہ_گر_TOCLookup_Fn, اشارہ_گر_TOCEntry_گنتی};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC فراغ udata_check_Common_انکڑے(UData_حافظہ *udm, UError_کوڈ *err) {
    إذا (U_FAILURE(*err)) {
        أرجع;
    }

    إذا(udm==خالی_سوچک || udm->p_سرخی==خالی_سوچک) {
      *err=U_INVALID_FORMAT_ERROR;
    } وإلا إذا(!(udm->p_سرخی->انکڑے_سرخی_2.magic1==0xda &&
        udm->p_سرخی->انکڑے_سرخی_2.magic2==0x27 &&
        udm->p_سرخی->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_سرخی->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    وإلا إذا (udm->p_سرخی->info.انکڑے_Format[0]==0x43 &&
        udm->p_سرخی->info.انکڑے_Format[1]==0x6d &&
        udm->p_سرخی->info.انکڑے_Format[2]==0x6e &&
        udm->p_سرخی->info.انکڑے_Format[3]==0x44 &&
        udm->p_سرخی->info.format_نسخہ[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرخی+udata_get_سرخی_ناپ(udm->p_سرخی);
    }
    وإلا إذا(udm->p_سرخی->info.انکڑے_Format[0]==0x54 &&
        udm->p_سرخی->info.انکڑے_Format[1]==0x6f &&
        udm->p_سرخی->info.انکڑے_Format[2]==0x43 &&
        udm->p_سرخی->info.انکڑے_Format[3]==0x50 &&
        udm->p_سرخی->info.format_نسخہ[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &منزل_CPFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرخی+udata_get_سرخی_ناپ(udm->p_سرخی);
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

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
#پایان_اگر

U_CFUNC uint16_t
udata_get_سرآیند_اندازه(ثابت داده_سرآیند *udh) {
    إذا(udh==اشاره_تهی) {
        أرجع 0;
    } وإلا إذا(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        أرجع udh->داده_سرآیند_2.سرآیند_اندازه;
    } وإلا {
        /* opposite endianness */
        uint16_t x=udh->داده_سرآیند_2.سرآیند_اندازه;
        أرجع (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_اندازه(ثابت UDataInfo *info) {
    إذا(info==اشاره_تهی) {
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
    ثابت حرف       *مدخل_Name;
    ثابت داده_سرآیند *p_سرآیند;
} اشاره_گر_TOCEntry;


تعريفنوع بنية  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    اشاره_گر_TOCEntry   مدخل[1];
}  اشاره_گر_TOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#اگر_تعریف_نشده MIN
#تعریف_کن MIN(a,b) (((a)<(b)) ? (a) : (b))
#پایان_اگر

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
ساكن int32_t
strcmp_After_پیشوند(ثابت حرف *s1, ثابت حرف *s2, int32_t *p_پیشوند_درازی) {
    int32_t pl=*p_پیشوند_درازی;
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
    *p_پیشوند_درازی=pl;
    أرجع cmp;
}

ساكن int32_t
جابه_جایی_TOCPrefix_دودویی_جستجو(ثابت حرف *s, ثابت حرف *نامها,
                            ثابت UData_جابه_جایی_TOCEntry *toc, int32_t count) {
    int32_t شروع=0;
    int32_t حد=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t شروع_پیشوند_درازی=0;
    int32_t حد_پیشوند_درازی=0;
    إذا(count==0) {
        أرجع -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    إذا(0==strcmp_After_پیشوند(s, نامها+toc[0].name_جابه_جایی, &شروع_پیشوند_درازی)) {
        أرجع 0;
    }
    ++شروع;
    --حد;
    إذا(0==strcmp_After_پیشوند(s, نامها+toc[حد].name_جابه_جایی, &حد_پیشوند_درازی)) {
        أرجع حد;
    }
    طالما(شروع<حد) {
        int32_t i=(شروع+حد)/2;
        int32_t پیشوند_درازی=MIN(شروع_پیشوند_درازی, حد_پیشوند_درازی);
        int32_t cmp=strcmp_After_پیشوند(s, نامها+toc[i].name_جابه_جایی, &پیشوند_درازی);
        إذا(cmp<0) {
            حد=i;
            حد_پیشوند_درازی=پیشوند_درازی;
        } وإلا إذا(cmp==0) {
            أرجع i;
        } وإلا {
            شروع=i+1;
            شروع_پیشوند_درازی=پیشوند_درازی;
        }
    }
    أرجع -1;
}

ساكن int32_t
اشاره_گر_TOCPrefix_دودویی_جستجو(ثابت حرف *s, ثابت اشاره_گر_TOCEntry *toc, int32_t count) {
    int32_t شروع=0;
    int32_t حد=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t شروع_پیشوند_درازی=0;
    int32_t حد_پیشوند_درازی=0;
    إذا(count==0) {
        أرجع -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    إذا(0==strcmp_After_پیشوند(s, toc[0].مدخل_Name, &شروع_پیشوند_درازی)) {
        أرجع 0;
    }
    ++شروع;
    --حد;
    إذا(0==strcmp_After_پیشوند(s, toc[حد].مدخل_Name, &حد_پیشوند_درازی)) {
        أرجع حد;
    }
    طالما(شروع<حد) {
        int32_t i=(شروع+حد)/2;
        int32_t پیشوند_درازی=MIN(شروع_پیشوند_درازی, حد_پیشوند_درازی);
        int32_t cmp=strcmp_After_پیشوند(s, toc[i].مدخل_Name, &پیشوند_درازی);
        إذا(cmp<0) {
            حد=i;
            حد_پیشوند_درازی=پیشوند_درازی;
        } وإلا إذا(cmp==0) {
            أرجع i;
        } وإلا {
            شروع=i+1;
            شروع_پیشوند_درازی=پیشوند_درازی;
        }
    }
    أرجع -1;
}

U_CDECL_BEGIN
ساكن uint32_t U_CALLCONV
جابه_جایی_TOCEntry_شمار(ثابت UData_حافظه *p_داده) {
    int32_t          retVal=0;
    ثابت UData_جابه_جایی_TOC *toc = (UData_جابه_جایی_TOC *)p_داده->toc;
    إذا (toc != اشاره_تهی) {
        retVal = toc->count;
    }
    أرجع retVal;
}

ساكن ثابت داده_سرآیند * U_CALLCONV
جابه_جایی_TOCLookup_Fn(ثابت UData_حافظه *p_داده,
                  ثابت حرف *toc_مدخل_Name,
                  int32_t *p_درازی,
                  UError_کد *p_Error_کد) {
    (فراغ)p_Error_کد;
    ثابت UData_جابه_جایی_TOC  *toc = (UData_جابه_جایی_TOC *)p_داده->toc;
    إذا(toc!=اشاره_تهی) {
        ثابت حرف *پایه=(ثابت حرف *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#إذا defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        لكل(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &پایه[toc->مدخل[number].name_جابه_جایی]);
        }
#پایان_اگر
        number=جابه_جایی_TOCPrefix_دودویی_جستجو(toc_مدخل_Name, پایه, toc->مدخل, count);
        إذا(number>=0) {
            /* found it */
            ثابت UData_جابه_جایی_TOCEntry *مدخل=toc->مدخل+number;
#اگر_تعریف_شده UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_مدخل_Name);
#پایان_اگر
            إذا((number+1) < count) {
                *p_درازی = (int32_t)(مدخل[1].داده_جابه_جایی - مدخل->داده_جابه_جایی);
            } وإلا {
                *p_درازی = -1;
            }
            أرجع (ثابت داده_سرآیند *)(پایه+مدخل->داده_جابه_جایی);
        } وإلا {
#اگر_تعریف_شده UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_مدخل_Name);
#پایان_اگر
            أرجع اشاره_تهی;
        }
    } وإلا {
#اگر_تعریف_شده UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#پایان_اگر

        أرجع p_داده->p_سرآیند;
    }
}


ساكن uint32_t U_CALLCONV اشاره_گر_TOCEntry_شمار(ثابت UData_حافظه *p_داده) {
    ثابت اشاره_گر_TOC *toc = (اشاره_گر_TOC *)p_داده->toc;
    أرجع toc != اشاره_تهی ? toc->count : 0;
}

ساكن ثابت داده_سرآیند * U_CALLCONV اشاره_گر_TOCLookup_Fn(ثابت UData_حافظه *p_داده,
                   ثابت حرف *name,
                   int32_t *p_درازی,
                   UError_کد *p_Error_کد) {
    (فراغ)p_Error_کد;
    إذا(p_داده->toc!=اشاره_تهی) {
        ثابت اشاره_گر_TOC *toc = (اشاره_گر_TOC *)p_داده->toc;
        int32_t number, count=(int32_t)toc->count;

#إذا defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        لكل(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->مدخل[number].مدخل_Name);
        }
#پایان_اگر
        number=اشاره_گر_TOCPrefix_دودویی_جستجو(name, toc->مدخل, count);
        إذا(number>=0) {
            /* found it */
#اگر_تعریف_شده UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->مدخل[number].مدخل_Name);
#پایان_اگر
            *p_درازی=-1;
            أرجع UData_حافظه_normalize_داده_اشاره_گر(toc->مدخل[number].p_سرآیند);
        } وإلا {
#اگر_تعریف_شده UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#پایان_اگر
            أرجع اشاره_تهی;
        }
    } وإلا {
        أرجع p_داده->p_سرآیند;
    }
}
U_CDECL_انتها


ساكن ثابت common_داده_Funcs CmnDFuncs = {جابه_جایی_TOCLookup_Fn,  جابه_جایی_TOCEntry_شمار};
ساكن ثابت common_داده_Funcs مقصد_CPFuncs = {اشاره_گر_TOCLookup_Fn, اشاره_گر_TOCEntry_شمار};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC فراغ udata_check_Common_داده(UData_حافظه *udm, UError_کد *err) {
    إذا (U_FAILURE(*err)) {
        أرجع;
    }

    إذا(udm==اشاره_تهی || udm->p_سرآیند==اشاره_تهی) {
      *err=U_INVALID_FORMAT_ERROR;
    } وإلا إذا(!(udm->p_سرآیند->داده_سرآیند_2.magic1==0xda &&
        udm->p_سرآیند->داده_سرآیند_2.magic2==0x27 &&
        udm->p_سرآیند->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_سرآیند->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    وإلا إذا (udm->p_سرآیند->info.داده_Format[0]==0x43 &&
        udm->p_سرآیند->info.داده_Format[1]==0x6d &&
        udm->p_سرآیند->info.داده_Format[2]==0x6e &&
        udm->p_سرآیند->info.داده_Format[3]==0x44 &&
        udm->p_سرآیند->info.format_نسخه[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرآیند+udata_get_سرآیند_اندازه(udm->p_سرآیند);
    }
    وإلا إذا(udm->p_سرآیند->info.داده_Format[0]==0x54 &&
        udm->p_سرآیند->info.داده_Format[1]==0x6f &&
        udm->p_سرآیند->info.داده_Format[2]==0x43 &&
        udm->p_سرآیند->info.داده_Format[3]==0x50 &&
        udm->p_سرآیند->info.format_نسخه[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &مقصد_CPFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرآیند+udata_get_سرآیند_اندازه(udm->p_سرآیند);
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

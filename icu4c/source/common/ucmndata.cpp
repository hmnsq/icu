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

#شامل "unicode/utypes.h"
#شامل "unicode/udata.h"
#شامل "cstring.h"
#شامل "ucmndata.h"
#شامل "udatamem.h"

#اگر defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#اگر_ختم

U_CFUNC uint16_t
udata_get_سرخی_سائز(ثابت ڈیٹا_سرخی *udh) {
    اگر(udh==خالی_اشاریہ) {
        واپس 0;
    } ورنہ اگر(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        واپس udh->ڈیٹا_سرخی_2.سرخی_سائز;
    } ورنہ {
        /* opposite endianness */
        uint16_t x=udh->ڈیٹا_سرخی_2.سرخی_سائز;
        واپس (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_سائز(ثابت UDataInfo *info) {
    اگر(info==خالی_اشاریہ) {
        واپس 0;
    } ورنہ اگر(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        واپس info->size;
    } ورنہ {
        /* opposite endianness */
        uint16_t x=info->size;
        واپس (uint16_t)((x<<8)|(x>>8));
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
قسمتعریف ساخت {
    ثابت حرف       *اندراج_Name;
    ثابت ڈیٹا_سرخی *p_سرخی;
} اشارہ_گر_TOCEntry;


قسمتعریف ساخت  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    اشارہ_گر_TOCEntry   اندراج_2[1];
}  اشارہ_گر_TOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#اگر_غیر_تعریف_شدہ MIN
#تعریف_کرو MIN(a,b) (((a)<(b)) ? (a) : (b))
#اگر_ختم

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
ساکن int32_t
strcmp_After_سابقہ(ثابت حرف *s1, ثابت حرف *s2, int32_t *p_سابقہ_لمبائی) {
    int32_t pl=*p_سابقہ_لمبائی;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    برائے(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        اگر(cmp!=0 || c1==0) {  /* different or done */
            توڑو;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *p_سابقہ_لمبائی=pl;
    واپس cmp;
}

ساکن int32_t
انحراف_TOCPrefix_ثنائی_تلاش(ثابت حرف *s, ثابت حرف *نام,
                            ثابت UData_انحراف_TOCEntry *toc, int32_t count) {
    int32_t آغاز=0;
    int32_t حد_بندی=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t آغاز_سابقہ_لمبائی=0;
    int32_t حد_بندی_سابقہ_لمبائی=0;
    اگر(count==0) {
        واپس -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    اگر(0==strcmp_After_سابقہ(s, نام+toc[0].name_انحراف, &آغاز_سابقہ_لمبائی)) {
        واپس 0;
    }
    ++آغاز;
    --حد_بندی;
    اگر(0==strcmp_After_سابقہ(s, نام+toc[حد_بندی].name_انحراف, &حد_بندی_سابقہ_لمبائی)) {
        واپس حد_بندی;
    }
    جب(آغاز<حد_بندی) {
        int32_t i=(آغاز+حد_بندی)/2;
        int32_t سابقہ_لمبائی=MIN(آغاز_سابقہ_لمبائی, حد_بندی_سابقہ_لمبائی);
        int32_t cmp=strcmp_After_سابقہ(s, نام+toc[i].name_انحراف, &سابقہ_لمبائی);
        اگر(cmp<0) {
            حد_بندی=i;
            حد_بندی_سابقہ_لمبائی=سابقہ_لمبائی;
        } ورنہ اگر(cmp==0) {
            واپس i;
        } ورنہ {
            آغاز=i+1;
            آغاز_سابقہ_لمبائی=سابقہ_لمبائی;
        }
    }
    واپس -1;
}

ساکن int32_t
اشارہ_گر_TOCPrefix_ثنائی_تلاش(ثابت حرف *s, ثابت اشارہ_گر_TOCEntry *toc, int32_t count) {
    int32_t آغاز=0;
    int32_t حد_بندی=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t آغاز_سابقہ_لمبائی=0;
    int32_t حد_بندی_سابقہ_لمبائی=0;
    اگر(count==0) {
        واپس -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    اگر(0==strcmp_After_سابقہ(s, toc[0].اندراج_Name, &آغاز_سابقہ_لمبائی)) {
        واپس 0;
    }
    ++آغاز;
    --حد_بندی;
    اگر(0==strcmp_After_سابقہ(s, toc[حد_بندی].اندراج_Name, &حد_بندی_سابقہ_لمبائی)) {
        واپس حد_بندی;
    }
    جب(آغاز<حد_بندی) {
        int32_t i=(آغاز+حد_بندی)/2;
        int32_t سابقہ_لمبائی=MIN(آغاز_سابقہ_لمبائی, حد_بندی_سابقہ_لمبائی);
        int32_t cmp=strcmp_After_سابقہ(s, toc[i].اندراج_Name, &سابقہ_لمبائی);
        اگر(cmp<0) {
            حد_بندی=i;
            حد_بندی_سابقہ_لمبائی=سابقہ_لمبائی;
        } ورنہ اگر(cmp==0) {
            واپس i;
        } ورنہ {
            آغاز=i+1;
            آغاز_سابقہ_لمبائی=سابقہ_لمبائی;
        }
    }
    واپس -1;
}

U_CDECL_BEGIN
ساکن uint32_t U_CALLCONV
انحراف_TOCEntry_گنتی(ثابت UData_حافظہ *p_ڈیٹا) {
    int32_t          retVal=0;
    ثابت UData_انحراف_TOC *toc = (UData_انحراف_TOC *)p_ڈیٹا->toc;
    اگر (toc != خالی_اشاریہ) {
        retVal = toc->count;
    }
    واپس retVal;
}

ساکن ثابت ڈیٹا_سرخی * U_CALLCONV
انحراف_TOCLookup_Fn(ثابت UData_حافظہ *p_ڈیٹا,
                  ثابت حرف *toc_اندراج_Name,
                  int32_t *p_لمبائی,
                  UError_رمز *p_Error_رمز) {
    (خالی)p_Error_رمز;
    ثابت UData_انحراف_TOC  *toc = (UData_انحراف_TOC *)p_ڈیٹا->toc;
    اگر(toc!=خالی_اشاریہ) {
        ثابت حرف *بنیاد=(ثابت حرف *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#اگر defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        برائے(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &بنیاد[toc->اندراج_2[number].name_انحراف]);
        }
#اگر_ختم
        number=انحراف_TOCPrefix_ثنائی_تلاش(toc_اندراج_Name, بنیاد, toc->اندراج_2, count);
        اگر(number>=0) {
            /* found it */
            ثابت UData_انحراف_TOCEntry *اندراج_2=toc->اندراج_2+number;
#اگر_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_اندراج_Name);
#اگر_ختم
            اگر((number+1) < count) {
                *p_لمبائی = (int32_t)(اندراج_2[1].ڈیٹا_انحراف - اندراج_2->ڈیٹا_انحراف);
            } ورنہ {
                *p_لمبائی = -1;
            }
            واپس (ثابت ڈیٹا_سرخی *)(بنیاد+اندراج_2->ڈیٹا_انحراف);
        } ورنہ {
#اگر_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_اندراج_Name);
#اگر_ختم
            واپس خالی_اشاریہ;
        }
    } ورنہ {
#اگر_تعریف_شدہ UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#اگر_ختم

        واپس p_ڈیٹا->p_سرخی;
    }
}


ساکن uint32_t U_CALLCONV اشارہ_گر_TOCEntry_گنتی(ثابت UData_حافظہ *p_ڈیٹا) {
    ثابت اشارہ_گر_TOC *toc = (اشارہ_گر_TOC *)p_ڈیٹا->toc;
    واپس toc != خالی_اشاریہ ? toc->count : 0;
}

ساکن ثابت ڈیٹا_سرخی * U_CALLCONV اشارہ_گر_TOCLookup_Fn(ثابت UData_حافظہ *p_ڈیٹا,
                   ثابت حرف *name,
                   int32_t *p_لمبائی,
                   UError_رمز *p_Error_رمز) {
    (خالی)p_Error_رمز;
    اگر(p_ڈیٹا->toc!=خالی_اشاریہ) {
        ثابت اشارہ_گر_TOC *toc = (اشارہ_گر_TOC *)p_ڈیٹا->toc;
        int32_t number, count=(int32_t)toc->count;

#اگر defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        برائے(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->اندراج_2[number].اندراج_Name);
        }
#اگر_ختم
        number=اشارہ_گر_TOCPrefix_ثنائی_تلاش(name, toc->اندراج_2, count);
        اگر(number>=0) {
            /* found it */
#اگر_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->اندراج_2[number].اندراج_Name);
#اگر_ختم
            *p_لمبائی=-1;
            واپس UData_حافظہ_normalize_ڈیٹا_اشارہ_گر(toc->اندراج_2[number].p_سرخی);
        } ورنہ {
#اگر_تعریف_شدہ UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#اگر_ختم
            واپس خالی_اشاریہ;
        }
    } ورنہ {
        واپس p_ڈیٹا->p_سرخی;
    }
}
U_CDECL_انتہا


ساکن ثابت common_ڈیٹا_Funcs CmnDFuncs = {انحراف_TOCLookup_Fn,  انحراف_TOCEntry_گنتی};
ساکن ثابت common_ڈیٹا_Funcs مقصد_CPFuncs = {اشارہ_گر_TOCLookup_Fn, اشارہ_گر_TOCEntry_گنتی};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC خالی udata_check_Common_ڈیٹا(UData_حافظہ *udm, UError_رمز *err) {
    اگر (U_FAILURE(*err)) {
        واپس;
    }

    اگر(udm==خالی_اشاریہ || udm->p_سرخی==خالی_اشاریہ) {
      *err=U_INVALID_FORMAT_ERROR;
    } ورنہ اگر(!(udm->p_سرخی->ڈیٹا_سرخی_2.magic1==0xda &&
        udm->p_سرخی->ڈیٹا_سرخی_2.magic2==0x27 &&
        udm->p_سرخی->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_سرخی->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    ورنہ اگر (udm->p_سرخی->info.ڈیٹا_Format[0]==0x43 &&
        udm->p_سرخی->info.ڈیٹا_Format[1]==0x6d &&
        udm->p_سرخی->info.ڈیٹا_Format[2]==0x6e &&
        udm->p_سرخی->info.ڈیٹا_Format[3]==0x44 &&
        udm->p_سرخی->info.format_نسخہ[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرخی+udata_get_سرخی_سائز(udm->p_سرخی);
    }
    ورنہ اگر(udm->p_سرخی->info.ڈیٹا_Format[0]==0x54 &&
        udm->p_سرخی->info.ڈیٹا_Format[1]==0x6f &&
        udm->p_سرخی->info.ڈیٹا_Format[2]==0x43 &&
        udm->p_سرخی->info.ڈیٹا_Format[3]==0x50 &&
        udm->p_سرخی->info.format_نسخہ[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &مقصد_CPFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرخی+udata_get_سرخی_سائز(udm->p_سرخی);
    }
    ورنہ {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    اگر (U_FAILURE(*err)) {
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

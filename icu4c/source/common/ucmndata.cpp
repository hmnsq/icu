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
#د_که_پای

U_CFUNC uint16_t
udata_get_سرلیک_اندازه(ثابت معلومات_سرلیک *udh) {
    إذا(udh==تش_اشاره) {
        أرجع 0;
    } وإلا إذا(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        أرجع udh->معلومات_سرلیک_2.سرلیک_اندازه;
    } وإلا {
        /* opposite endianness */
        uint16_t x=udh->معلومات_سرلیک_2.سرلیک_اندازه;
        أرجع (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_اندازه(ثابت UDataInfo *info) {
    إذا(info==تش_اشاره) {
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
    ثابت حرف       *ثبت_Name;
    ثابت معلومات_سرلیک *p_سرلیک;
} اشاره_ګر_TOCEntry;


تعريفنوع بنية  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    اشاره_ګر_TOCEntry   ثبت[1];
}  اشاره_ګر_TOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#که_نه_تعریف_شوی MIN
#تعریف_کړه MIN(a,b) (((a)<(b)) ? (a) : (b))
#د_که_پای

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
ساكن int32_t
strcmp_After_مختاړی(ثابت حرف *s1, ثابت حرف *s2, int32_t *p_مختاړی_اوږدتیا) {
    int32_t pl=*p_مختاړی_اوږدتیا;
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
    *p_مختاړی_اوږدتیا=pl;
    أرجع cmp;
}

ساكن int32_t
دننه_واټن_TOCPrefix_دوګونی_پلټنه(ثابت حرف *s, ثابت حرف *نومونه,
                            ثابت UData_دننه_واټن_TOCEntry *toc, int32_t count) {
    int32_t پیل=0;
    int32_t حد=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t پیل_مختاړی_اوږدتیا=0;
    int32_t حد_مختاړی_اوږدتیا=0;
    إذا(count==0) {
        أرجع -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    إذا(0==strcmp_After_مختاړی(s, نومونه+toc[0].name_دننه_واټن, &پیل_مختاړی_اوږدتیا)) {
        أرجع 0;
    }
    ++پیل;
    --حد;
    إذا(0==strcmp_After_مختاړی(s, نومونه+toc[حد].name_دننه_واټن, &حد_مختاړی_اوږدتیا)) {
        أرجع حد;
    }
    طالما(پیل<حد) {
        int32_t i=(پیل+حد)/2;
        int32_t مختاړی_اوږدتیا=MIN(پیل_مختاړی_اوږدتیا, حد_مختاړی_اوږدتیا);
        int32_t cmp=strcmp_After_مختاړی(s, نومونه+toc[i].name_دننه_واټن, &مختاړی_اوږدتیا);
        إذا(cmp<0) {
            حد=i;
            حد_مختاړی_اوږدتیا=مختاړی_اوږدتیا;
        } وإلا إذا(cmp==0) {
            أرجع i;
        } وإلا {
            پیل=i+1;
            پیل_مختاړی_اوږدتیا=مختاړی_اوږدتیا;
        }
    }
    أرجع -1;
}

ساكن int32_t
اشاره_ګر_TOCPrefix_دوګونی_پلټنه(ثابت حرف *s, ثابت اشاره_ګر_TOCEntry *toc, int32_t count) {
    int32_t پیل=0;
    int32_t حد=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t پیل_مختاړی_اوږدتیا=0;
    int32_t حد_مختاړی_اوږدتیا=0;
    إذا(count==0) {
        أرجع -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    إذا(0==strcmp_After_مختاړی(s, toc[0].ثبت_Name, &پیل_مختاړی_اوږدتیا)) {
        أرجع 0;
    }
    ++پیل;
    --حد;
    إذا(0==strcmp_After_مختاړی(s, toc[حد].ثبت_Name, &حد_مختاړی_اوږدتیا)) {
        أرجع حد;
    }
    طالما(پیل<حد) {
        int32_t i=(پیل+حد)/2;
        int32_t مختاړی_اوږدتیا=MIN(پیل_مختاړی_اوږدتیا, حد_مختاړی_اوږدتیا);
        int32_t cmp=strcmp_After_مختاړی(s, toc[i].ثبت_Name, &مختاړی_اوږدتیا);
        إذا(cmp<0) {
            حد=i;
            حد_مختاړی_اوږدتیا=مختاړی_اوږدتیا;
        } وإلا إذا(cmp==0) {
            أرجع i;
        } وإلا {
            پیل=i+1;
            پیل_مختاړی_اوږدتیا=مختاړی_اوږدتیا;
        }
    }
    أرجع -1;
}

U_CDECL_BEGIN
ساكن uint32_t U_CALLCONV
دننه_واټن_TOCEntry_شمېر(ثابت UData_حافظه *p_معلومات) {
    int32_t          retVal=0;
    ثابت UData_دننه_واټن_TOC *toc = (UData_دننه_واټن_TOC *)p_معلومات->toc;
    إذا (toc != تش_اشاره) {
        retVal = toc->count;
    }
    أرجع retVal;
}

ساكن ثابت معلومات_سرلیک * U_CALLCONV
دننه_واټن_TOCLookup_Fn(ثابت UData_حافظه *p_معلومات,
                  ثابت حرف *toc_ثبت_Name,
                  int32_t *p_اوږدتیا,
                  UError_کوډ *p_Error_کوډ) {
    (فراغ)p_Error_کوډ;
    ثابت UData_دننه_واټن_TOC  *toc = (UData_دننه_واټن_TOC *)p_معلومات->toc;
    إذا(toc!=تش_اشاره) {
        ثابت حرف *بنسټ=(ثابت حرف *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#إذا defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        لكل(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &بنسټ[toc->ثبت[number].name_دننه_واټن]);
        }
#د_که_پای
        number=دننه_واټن_TOCPrefix_دوګونی_پلټنه(toc_ثبت_Name, بنسټ, toc->ثبت, count);
        إذا(number>=0) {
            /* found it */
            ثابت UData_دننه_واټن_TOCEntry *ثبت=toc->ثبت+number;
#که_تعریف_شوی UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_ثبت_Name);
#د_که_پای
            إذا((number+1) < count) {
                *p_اوږدتیا = (int32_t)(ثبت[1].معلومات_دننه_واټن - ثبت->معلومات_دننه_واټن);
            } وإلا {
                *p_اوږدتیا = -1;
            }
            أرجع (ثابت معلومات_سرلیک *)(بنسټ+ثبت->معلومات_دننه_واټن);
        } وإلا {
#که_تعریف_شوی UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_ثبت_Name);
#د_که_پای
            أرجع تش_اشاره;
        }
    } وإلا {
#که_تعریف_شوی UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#د_که_پای

        أرجع p_معلومات->p_سرلیک;
    }
}


ساكن uint32_t U_CALLCONV اشاره_ګر_TOCEntry_شمېر(ثابت UData_حافظه *p_معلومات) {
    ثابت اشاره_ګر_TOC *toc = (اشاره_ګر_TOC *)p_معلومات->toc;
    أرجع toc != تش_اشاره ? toc->count : 0;
}

ساكن ثابت معلومات_سرلیک * U_CALLCONV اشاره_ګر_TOCLookup_Fn(ثابت UData_حافظه *p_معلومات,
                   ثابت حرف *name,
                   int32_t *p_اوږدتیا,
                   UError_کوډ *p_Error_کوډ) {
    (فراغ)p_Error_کوډ;
    إذا(p_معلومات->toc!=تش_اشاره) {
        ثابت اشاره_ګر_TOC *toc = (اشاره_ګر_TOC *)p_معلومات->toc;
        int32_t number, count=(int32_t)toc->count;

#إذا defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        لكل(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->ثبت[number].ثبت_Name);
        }
#د_که_پای
        number=اشاره_ګر_TOCPrefix_دوګونی_پلټنه(name, toc->ثبت, count);
        إذا(number>=0) {
            /* found it */
#که_تعریف_شوی UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->ثبت[number].ثبت_Name);
#د_که_پای
            *p_اوږدتیا=-1;
            أرجع UData_حافظه_normalize_معلومات_اشاره_ګر(toc->ثبت[number].p_سرلیک);
        } وإلا {
#که_تعریف_شوی UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#د_که_پای
            أرجع تش_اشاره;
        }
    } وإلا {
        أرجع p_معلومات->p_سرلیک;
    }
}
U_CDECL_پای_ټکی


ساكن ثابت common_معلومات_Funcs CmnDFuncs = {دننه_واټن_TOCLookup_Fn,  دننه_واټن_TOCEntry_شمېر};
ساكن ثابت common_معلومات_Funcs منزل_CPFuncs = {اشاره_ګر_TOCLookup_Fn, اشاره_ګر_TOCEntry_شمېر};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC فراغ udata_check_Common_معلومات(UData_حافظه *udm, UError_کوډ *err) {
    إذا (U_FAILURE(*err)) {
        أرجع;
    }

    إذا(udm==تش_اشاره || udm->p_سرلیک==تش_اشاره) {
      *err=U_INVALID_FORMAT_ERROR;
    } وإلا إذا(!(udm->p_سرلیک->معلومات_سرلیک_2.magic1==0xda &&
        udm->p_سرلیک->معلومات_سرلیک_2.magic2==0x27 &&
        udm->p_سرلیک->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_سرلیک->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    وإلا إذا (udm->p_سرلیک->info.معلومات_Format[0]==0x43 &&
        udm->p_سرلیک->info.معلومات_Format[1]==0x6d &&
        udm->p_سرلیک->info.معلومات_Format[2]==0x6e &&
        udm->p_سرلیک->info.معلومات_Format[3]==0x44 &&
        udm->p_سرلیک->info.format_نسخه[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرلیک+udata_get_سرلیک_اندازه(udm->p_سرلیک);
    }
    وإلا إذا(udm->p_سرلیک->info.معلومات_Format[0]==0x54 &&
        udm->p_سرلیک->info.معلومات_Format[1]==0x6f &&
        udm->p_سرلیک->info.معلومات_Format[2]==0x43 &&
        udm->p_سرلیک->info.معلومات_Format[3]==0x50 &&
        udm->p_سرلیک->info.format_نسخه[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &منزل_CPFuncs;
        udm->toc=(ثابت حرف *)udm->p_سرلیک+udata_get_سرلیک_اندازه(udm->p_سرلیک);
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

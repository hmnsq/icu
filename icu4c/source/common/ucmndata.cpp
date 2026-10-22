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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "ucmndata.h"
#అన్తర్భావ "udatamem.h"

#ఒకవేళ defined(UDATA_DEBUG) || defined(UDATA_DEBUG_DUMP)
#   include <stdio.h>
#అయితే_ముగింపు

U_CFUNC uint16_t
udata_get_శీర్షిక_కొలత(స్థిరరాశి దత్తాంశం_శీర్షిక *udh) {
    ఒకవేళ(udh==శూన్యసూచిక) {
        తిరిగి 0;
    } లేకపోతే ఒకవేళ(udh->info.isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        తిరిగి udh->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత;
    } లేకపోతే {
        /* opposite endianness */
        uint16_t x=udh->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత;
        తిరిగి (uint16_t)((x<<8)|(x>>8));
    }
}

U_CFUNC uint16_t
udata_get_Info_కొలత(స్థిరరాశి UDataInfo *info) {
    ఒకవేళ(info==శూన్యసూచిక) {
        తిరిగి 0;
    } లేకపోతే ఒకవేళ(info->isBigEndian==U_IS_BIG_ENDIAN) {
        /* same endianness */
        తిరిగి info->size;
    } లేకపోతే {
        /* opposite endianness */
        uint16_t x=info->size;
        తిరిగి (uint16_t)((x<<8)|(x>>8));
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
రకనిర్వచనం నిర్మాణం {
    స్థిరరాశి అక్షరం       *నమోదు_Name;
    స్థిరరాశి దత్తాంశం_శీర్షిక *p_శీర్షిక;
} సూచిక_TOCEntry;


రకనిర్వచనం నిర్మాణం  {
    uint32_t          count;
    uint32_t          reserved;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    సూచిక_TOCEntry   నమోదు_2[1];
}  సూచిక_TOC;


/* definition of OffsetTOC struct types moved to ucmndata.h */

/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    entry point lookup implementations                                       *
 *                                                                             *
 *-----------------------------------------------------------------------------*/

#నిర్వచితం_కాకపోతే MIN
#నిర్వచించు MIN(a,b) (((a)<(b)) ? (a) : (b))
#అయితే_ముగింపు

/**
 * Compare strings where we know the shared prefix length,
 * and advance the prefix length as we find that the strings share even more characters.
 */
స్థిర int32_t
strcmp_After_ఉపసర్గ(స్థిరరాశి అక్షరం *s1, స్థిరరాశి అక్షరం *s2, int32_t *p_ఉపసర్గ_నిడివి) {
    int32_t pl=*p_ఉపసర్గ_నిడివి;
    int32_t cmp=0;
    s1+=pl;
    s2+=pl;
    కొరకు(;;) {
        int32_t c1 = static_cast<uint8_t>(*s1++);
        int32_t c2 = static_cast<uint8_t>(*s2++);
        cmp=c1-c2;
        ఒకవేళ(cmp!=0 || c1==0) {  /* different or done */
            ఆపు;
        }
        ++pl;  /* increment shared same-prefix length */
    }
    *p_ఉపసర్గ_నిడివి=pl;
    తిరిగి cmp;
}

స్థిర int32_t
స్థానభ్రంశం_TOCPrefix_ద్విమాన_శోధన(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *పేర్లు,
                            స్థిరరాశి UData_స్థానభ్రంశం_TOCEntry *toc, int32_t count) {
    int32_t ప్రారంభం=0;
    int32_t హద్దు=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t ప్రారంభం_ఉపసర్గ_నిడివి=0;
    int32_t హద్దు_ఉపసర్గ_నిడివి=0;
    ఒకవేళ(count==0) {
        తిరిగి -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ఒకవేళ(0==strcmp_After_ఉపసర్గ(s, పేర్లు+toc[0].name_స్థానభ్రంశం, &ప్రారంభం_ఉపసర్గ_నిడివి)) {
        తిరిగి 0;
    }
    ++ప్రారంభం;
    --హద్దు;
    ఒకవేళ(0==strcmp_After_ఉపసర్గ(s, పేర్లు+toc[హద్దు].name_స్థానభ్రంశం, &హద్దు_ఉపసర్గ_నిడివి)) {
        తిరిగి హద్దు;
    }
    అయితే(ప్రారంభం<హద్దు) {
        int32_t i=(ప్రారంభం+హద్దు)/2;
        int32_t ఉపసర్గ_నిడివి=MIN(ప్రారంభం_ఉపసర్గ_నిడివి, హద్దు_ఉపసర్గ_నిడివి);
        int32_t cmp=strcmp_After_ఉపసర్గ(s, పేర్లు+toc[i].name_స్థానభ్రంశం, &ఉపసర్గ_నిడివి);
        ఒకవేళ(cmp<0) {
            హద్దు=i;
            హద్దు_ఉపసర్గ_నిడివి=ఉపసర్గ_నిడివి;
        } లేకపోతే ఒకవేళ(cmp==0) {
            తిరిగి i;
        } లేకపోతే {
            ప్రారంభం=i+1;
            ప్రారంభం_ఉపసర్గ_నిడివి=ఉపసర్గ_నిడివి;
        }
    }
    తిరిగి -1;
}

స్థిర int32_t
సూచిక_TOCPrefix_ద్విమాన_శోధన(స్థిరరాశి అక్షరం *s, స్థిరరాశి సూచిక_TOCEntry *toc, int32_t count) {
    int32_t ప్రారంభం=0;
    int32_t హద్దు=count;
    /*
     * Remember the shared prefix between s, start and limit,
     * and don't compare that shared prefix again.
     * The shared prefix should get longer as we narrow the [start, limit[ range.
     */
    int32_t ప్రారంభం_ఉపసర్గ_నిడివి=0;
    int32_t హద్దు_ఉపసర్గ_నిడివి=0;
    ఒకవేళ(count==0) {
        తిరిగి -1;
    }
    /*
     * Prime the prefix lengths so that we don't keep prefixLength at 0 until
     * both the start and limit indexes have moved.
     * At the same time, we find if s is one of the start and (limit-1) names,
     * and if not, exclude them from the actual binary search.
     */
    ఒకవేళ(0==strcmp_After_ఉపసర్గ(s, toc[0].నమోదు_Name, &ప్రారంభం_ఉపసర్గ_నిడివి)) {
        తిరిగి 0;
    }
    ++ప్రారంభం;
    --హద్దు;
    ఒకవేళ(0==strcmp_After_ఉపసర్గ(s, toc[హద్దు].నమోదు_Name, &హద్దు_ఉపసర్గ_నిడివి)) {
        తిరిగి హద్దు;
    }
    అయితే(ప్రారంభం<హద్దు) {
        int32_t i=(ప్రారంభం+హద్దు)/2;
        int32_t ఉపసర్గ_నిడివి=MIN(ప్రారంభం_ఉపసర్గ_నిడివి, హద్దు_ఉపసర్గ_నిడివి);
        int32_t cmp=strcmp_After_ఉపసర్గ(s, toc[i].నమోదు_Name, &ఉపసర్గ_నిడివి);
        ఒకవేళ(cmp<0) {
            హద్దు=i;
            హద్దు_ఉపసర్గ_నిడివి=ఉపసర్గ_నిడివి;
        } లేకపోతే ఒకవేళ(cmp==0) {
            తిరిగి i;
        } లేకపోతే {
            ప్రారంభం=i+1;
            ప్రారంభం_ఉపసర్గ_నిడివి=ఉపసర్గ_నిడివి;
        }
    }
    తిరిగి -1;
}

U_CDECL_BEGIN
స్థిర uint32_t U_CALLCONV
స్థానభ్రంశం_TOCEntry_లెక్క(స్థిరరాశి UData_జ్ఞాపకం *p_దత్తాంశం) {
    int32_t          retVal=0;
    స్థిరరాశి UData_స్థానభ్రంశం_TOC *toc = (UData_స్థానభ్రంశం_TOC *)p_దత్తాంశం->toc;
    ఒకవేళ (toc != శూన్యసూచిక) {
        retVal = toc->count;
    }
    తిరిగి retVal;
}

స్థిర స్థిరరాశి దత్తాంశం_శీర్షిక * U_CALLCONV
స్థానభ్రంశం_TOCLookup_Fn(స్థిరరాశి UData_జ్ఞాపకం *p_దత్తాంశం,
                  స్థిరరాశి అక్షరం *toc_నమోదు_Name,
                  int32_t *p_నిడివి,
                  UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)p_Error_సంకేతం;
    స్థిరరాశి UData_స్థానభ్రంశం_TOC  *toc = (UData_స్థానభ్రంశం_TOC *)p_దత్తాంశం->toc;
    ఒకవేళ(toc!=శూన్యసూచిక) {
        స్థిరరాశి అక్షరం *పునాది=(స్థిరరాశి అక్షరం *)toc;
        int32_t number, count=(int32_t)toc->count;

        /* perform a binary search for the data in the common data's table of contents */
#ఒకవేళ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        కొరకు(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, &పునాది[toc->నమోదు_2[number].name_స్థానభ్రంశం]);
        }
#అయితే_ముగింపు
        number=స్థానభ్రంశం_TOCPrefix_ద్విమాన_శోధన(toc_నమోదు_Name, పునాది, toc->నమోదు_2, count);
        ఒకవేళ(number>=0) {
            /* found it */
            స్థిరరాశి UData_స్థానభ్రంశం_TOCEntry *నమోదు_2=toc->నమోదు_2+number;
#నిర్వచితమైతే UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc_నమోదు_Name);
#అయితే_ముగింపు
            ఒకవేళ((number+1) < count) {
                *p_నిడివి = (int32_t)(నమోదు_2[1].దత్తాంశం_స్థానభ్రంశం - నమోదు_2->దత్తాంశం_స్థానభ్రంశం);
            } లేకపోతే {
                *p_నిడివి = -1;
            }
            తిరిగి (స్థిరరాశి దత్తాంశం_శీర్షిక *)(పునాది+నమోదు_2->దత్తాంశం_స్థానభ్రంశం);
        } లేకపోతే {
#నిర్వచితమైతే UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", toc_నమోదు_Name);
#అయితే_ముగింపు
            తిరిగి శూన్యసూచిక;
        }
    } లేకపోతే {
#నిర్వచితమైతే UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#అయితే_ముగింపు

        తిరిగి p_దత్తాంశం->p_శీర్షిక;
    }
}


స్థిర uint32_t U_CALLCONV సూచిక_TOCEntry_లెక్క(స్థిరరాశి UData_జ్ఞాపకం *p_దత్తాంశం) {
    స్థిరరాశి సూచిక_TOC *toc = (సూచిక_TOC *)p_దత్తాంశం->toc;
    తిరిగి toc != శూన్యసూచిక ? toc->count : 0;
}

స్థిర స్థిరరాశి దత్తాంశం_శీర్షిక * U_CALLCONV సూచిక_TOCLookup_Fn(స్థిరరాశి UData_జ్ఞాపకం *p_దత్తాంశం,
                   స్థిరరాశి అక్షరం *name,
                   int32_t *p_నిడివి,
                   UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)p_Error_సంకేతం;
    ఒకవేళ(p_దత్తాంశం->toc!=శూన్యసూచిక) {
        స్థిరరాశి సూచిక_TOC *toc = (సూచిక_TOC *)p_దత్తాంశం->toc;
        int32_t number, count=(int32_t)toc->count;

#ఒకవేళ defined (UDATA_DEBUG_DUMP)
        /* list the contents of the TOC each time .. not recommended */
        కొరకు(number=0; number<count; ++number) {
            fprintf(stderr, "\tx%d: %s\n", number, toc->నమోదు_2[number].నమోదు_Name);
        }
#అయితే_ముగింపు
        number=సూచిక_TOCPrefix_ద్విమాన_శోధన(name, toc->నమోదు_2, count);
        ఒకవేళ(number>=0) {
            /* found it */
#నిర్వచితమైతే UDATA_DEBUG
            fprintf(stderr, "%s: Found.\n", toc->నమోదు_2[number].నమోదు_Name);
#అయితే_ముగింపు
            *p_నిడివి=-1;
            తిరిగి UData_జ్ఞాపకం_normalize_దత్తాంశం_సూచిక(toc->నమోదు_2[number].p_శీర్షిక);
        } లేకపోతే {
#నిర్వచితమైతే UDATA_DEBUG
            fprintf(stderr, "%s: Not found.\n", name);
#అయితే_ముగింపు
            తిరిగి శూన్యసూచిక;
        }
    } లేకపోతే {
        తిరిగి p_దత్తాంశం->p_శీర్షిక;
    }
}
U_CDECL_చివర


స్థిర స్థిరరాశి common_దత్తాంశం_Funcs CmnDFuncs = {స్థానభ్రంశం_TOCLookup_Fn,  స్థానభ్రంశం_TOCEntry_లెక్క};
స్థిర స్థిరరాశి common_దత్తాంశం_Funcs గమ్యం_CPFuncs = {సూచిక_TOCLookup_Fn, సూచిక_TOCEntry_లెక్క};



/*----------------------------------------------------------------------*
 *                                                                      *
 *  checkCommonData   Validate the format of a common data file.        *
 *                    Fill in the virtual function ptr based on TOC type *
 *                    If the data is invalid, close the UDataMemory     *
 *                    and set the appropriate error code.               *
 *                                                                      *
 *----------------------------------------------------------------------*/
U_CFUNC శూన్య udata_check_Common_దత్తాంశం(UData_జ్ఞాపకం *udm, UError_సంకేతం *err) {
    ఒకవేళ (U_FAILURE(*err)) {
        తిరిగి;
    }

    ఒకవేళ(udm==శూన్యసూచిక || udm->p_శీర్షిక==శూన్యసూచిక) {
      *err=U_INVALID_FORMAT_ERROR;
    } లేకపోతే ఒకవేళ(!(udm->p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic1==0xda &&
        udm->p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic2==0x27 &&
        udm->p_శీర్షిక->info.isBigEndian==U_IS_BIG_ENDIAN &&
        udm->p_శీర్షిక->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
        /* header not valid */
        *err=U_INVALID_FORMAT_ERROR;
    }
    లేకపోతే ఒకవేళ (udm->p_శీర్షిక->info.దత్తాంశం_Format[0]==0x43 &&
        udm->p_శీర్షిక->info.దత్తాంశం_Format[1]==0x6d &&
        udm->p_శీర్షిక->info.దత్తాంశం_Format[2]==0x6e &&
        udm->p_శీర్షిక->info.దత్తాంశం_Format[3]==0x44 &&
        udm->p_శీర్షిక->info.format_సంస్కరణ[0]==1
        ) {
        /* dataFormat="CmnD" */
        udm->vFuncs = &CmnDFuncs;
        udm->toc=(స్థిరరాశి అక్షరం *)udm->p_శీర్షిక+udata_get_శీర్షిక_కొలత(udm->p_శీర్షిక);
    }
    లేకపోతే ఒకవేళ(udm->p_శీర్షిక->info.దత్తాంశం_Format[0]==0x54 &&
        udm->p_శీర్షిక->info.దత్తాంశం_Format[1]==0x6f &&
        udm->p_శీర్షిక->info.దత్తాంశం_Format[2]==0x43 &&
        udm->p_శీర్షిక->info.దత్తాంశం_Format[3]==0x50 &&
        udm->p_శీర్షిక->info.format_సంస్కరణ[0]==1
        ) {
        /* dataFormat="ToCP" */
        udm->vFuncs = &గమ్యం_CPFuncs;
        udm->toc=(స్థిరరాశి అక్షరం *)udm->p_శీర్షిక+udata_get_శీర్షిక_కొలత(udm->p_శీర్షిక);
    }
    లేకపోతే {
        /* dataFormat not recognized */
        *err=U_INVALID_FORMAT_ERROR;
    }

    ఒకవేళ (U_FAILURE(*err)) {
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

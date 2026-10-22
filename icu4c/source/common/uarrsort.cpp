// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uarrsort.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003aug04
*   created by: Markus W. Scherer
*
*   Internal function for sorting arrays.
*/

#అన్తర్భావ <cstddef>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uarrsort.h"

గణన {
    /**
     * "from Knuth"
     *
     * A binary search over 8 items performs 4 comparisons:
     * log2(8)=3 to subdivide, +1 to check for equality.
     * A linear search over 8 items on average also performs 4 comparisons.
     */
    MIN_QSORT=9,
    దొంతర_అంశం_కొలత=200
};

స్థిర constexpr int32_t కొలత_లోపల_Max_Align_Ts(int32_t కొలత_లోపల_బైట్లు) {
    తిరిగి (కొలత_లోపల_బైట్లు + పరిమాణం(std::max_align_t) - 1) / పరిమాణం(std::max_align_t);
}

/* UComparator convenience implementations ---------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    (శూన్య)context;
    తిరిగి (int32_t)*(స్థిరరాశి uint16_t *)ఎడమ - (int32_t)*(స్థిరరాశి uint16_t *)కుడి;
}

U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    (శూన్య)context;
    తిరిగి *(స్థిరరాశి int32_t *)ఎడమ - *(స్థిరరాశి int32_t *)కుడి;
}

U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    (శూన్య)context;
    uint32_t l=*(స్థిరరాశి uint32_t *)ఎడమ, r=*(స్థిరరాశి uint32_t *)కుడి;

    /* compare directly because (l-r) would overflow the int32_t result */
    ఒకవేళ(l<r) {
        తిరిగి -1;
    } లేకపోతే ఒకవేళ(l==r) {
        తిరిగి 0;
    } లేకపోతే /* l>r */ {
        తిరిగి 1;
    }
}

/* Insertion sort using binary search --------------------------------------- */

U_CAPI int32_t U_EXPORT2
uprv_stable_ద్విమాన_శోధన(అక్షరం *array, int32_t హద్దు, శూన్య *అంశం, int32_t అంశం_కొలత,
                        UComparator *cmp, స్థిరరాశి శూన్య *context) {
    int32_t ప్రారంభం=0;
    UBool దొరికింది=అబద్ధం;

    /* Binary search until we get down to a tiny sub-array. */
    అయితే((హద్దు-ప్రారంభం)>=MIN_QSORT) {
        int32_t i=(ప్రారంభం+హద్దు)/2;
        int32_t diff=cmp(context, అంశం, array+i*అంశం_కొలత);
        ఒకవేళ(diff==0) {
            /*
             * Found the item. We look for the *last* occurrence of such
             * an item, for stable sorting.
             * If we knew that there will be only few equal items,
             * we could break now and enter the linear search.
             * However, if there are many equal items, then it should be
             * faster to continue with the binary search.
             * It seems likely that we either have all unique items
             * (where found will never become true in the insertion sort)
             * or potentially many duplicates.
             */
            దొరికింది=నిజం;
            ప్రారంభం=i+1;
        } లేకపోతే ఒకవేళ(diff<0) {
            హద్దు=i;
        } లేకపోతే {
            ప్రారంభం=i;
        }
    }

    /* Linear search over the remaining tiny sub-array. */
    అయితే(ప్రారంభం<హద్దు) {
        int32_t diff=cmp(context, అంశం, array+ప్రారంభం*అంశం_కొలత);
        ఒకవేళ(diff==0) {
            దొరికింది=నిజం;
        } లేకపోతే ఒకవేళ(diff<0) {
            ఆపు;
        }
        ++ప్రారంభం;
    }
    తిరిగి దొరికింది ? (ప్రారంభం-1) : ~ప్రారంభం;
}

స్థిర శూన్య
do_Insertion_క్రమీకరించు(అక్షరం *array, int32_t length, int32_t అంశం_కొలత,
                UComparator *cmp, స్థిరరాశి శూన్య *context, శూన్య *pv) {
    int32_t j;

    కొరకు(j=1; j<length; ++j) {
        అక్షరం *అంశం=array+j*అంశం_కొలత;
        int32_t insertion_బిందువు=uprv_stable_ద్విమాన_శోధన(array, j, అంశం, అంశం_కొలత, cmp, context);
        ఒకవేళ(insertion_బిందువు<0) {
            insertion_బిందువు=~insertion_బిందువు;
        } లేకపోతే {
            ++insertion_బిందువు;  /* one past the last equal item */
        }
        ఒకవేళ(insertion_బిందువు<j) {
            అక్షరం *dest=array+insertion_బిందువు*అంశం_కొలత;
            uprv_memcpy(pv, అంశం, అంశం_కొలత);  /* v=array[j] */
            uprv_memmove(dest+అంశం_కొలత, dest, (j-insertion_బిందువు)*(size_t)అంశం_కొలత);
            uprv_memcpy(dest, pv, అంశం_కొలత);  /* array[insertionPoint]=v */
        }
    }
}

స్థిర శూన్య
insertion_క్రమీకరించు(అక్షరం *array, int32_t length, int32_t అంశం_కొలత,
              UComparator *cmp, స్థిరరాశి శూన్య *context, UError_సంకేతం *p_Error_సంకేతం) {

    icu::Maybe_దొంతర_Array<std::max_align_t, కొలత_లోపల_Max_Align_Ts(దొంతర_అంశం_కొలత)> v;
    ఒకవేళ (కొలత_లోపల_Max_Align_Ts(అంశం_కొలత) > v.get_సామర్థ్యం() &&
            v.resize(కొలత_లోపల_Max_Align_Ts(అంశం_కొలత)) == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    do_Insertion_క్రమీకరించు(array, length, అంశం_కొలత, cmp, context, v.getAlias());
}

/* QuickSort ---------------------------------------------------------------- */

/*
 * This implementation is semi-recursive:
 * It recurses for the smaller sub-array to shorten the recursion depth,
 * and loops for the larger sub-array.
 *
 * Loosely after QuickSort algorithms in
 * Niklaus Wirth
 * Algorithmen und Datenstrukturen mit Modula-2
 * B.G. Teubner Stuttgart
 * 4. Auflage 1986
 * ISBN 3-519-02260-5
 */
స్థిర శూన్య
sub_శీఘ్ర_క్రమీకరించు(అక్షరం *array, int32_t ప్రారంభం, int32_t హద్దు, int32_t అంశం_కొలత,
             UComparator *cmp, స్థిరరాశి శూన్య *context,
             శూన్య *px, శూన్య *pw) {
    int32_t ఎడమ, కుడి;

    /* start and left are inclusive, limit and right are exclusive */
    చేయి {
        ఒకవేళ((ప్రారంభం+MIN_QSORT)>=హద్దు) {
            do_Insertion_క్రమీకరించు(array+ప్రారంభం*అంశం_కొలత, హద్దు-ప్రారంభం, అంశం_కొలత, cmp, context, px);
            ఆపు;
        }

        ఎడమ=ప్రారంభం;
        కుడి=హద్దు;

        /* x=array[middle] */
        uprv_memcpy(px, array+(size_t)((ప్రారంభం+హద్దు)/2)*అంశం_కొలత, అంశం_కొలత);

        చేయి {
            అయితే(/* array[left]<x */
                  cmp(context, array+ఎడమ*అంశం_కొలత, px)<0
            ) {
                ++ఎడమ;
            }
            అయితే(/* x<array[right-1] */
                  cmp(context, px, array+(కుడి-1)*అంశం_కొలత)<0
            ) {
                --కుడి;
            }

            /* swap array[left] and array[right-1] via w; ++left; --right */
            ఒకవేళ(ఎడమ<కుడి) {
                --కుడి;

                ఒకవేళ(ఎడమ<కుడి) {
                    uprv_memcpy(pw, array+(size_t)ఎడమ*అంశం_కొలత, అంశం_కొలత);
                    uprv_memcpy(array+(size_t)ఎడమ*అంశం_కొలత, array+(size_t)కుడి*అంశం_కొలత, అంశం_కొలత);
                    uprv_memcpy(array+(size_t)కుడి*అంశం_కొలత, pw, అంశం_కొలత);
                }

                ++ఎడమ;
            }
        } అయితే(ఎడమ<కుడి);

        /* sort sub-arrays */
        ఒకవేళ((కుడి-ప్రారంభం)<(హద్దు-ఎడమ)) {
            /* sort [start..right[ */
            ఒకవేళ(ప్రారంభం<(కుడి-1)) {
                sub_శీఘ్ర_క్రమీకరించు(array, ప్రారంభం, కుడి, అంశం_కొలత, cmp, context, px, pw);
            }

            /* sort [left..limit[ */
            ప్రారంభం=ఎడమ;
        } లేకపోతే {
            /* sort [left..limit[ */
            ఒకవేళ(ఎడమ<(హద్దు-1)) {
                sub_శీఘ్ర_క్రమీకరించు(array, ఎడమ, హద్దు, అంశం_కొలత, cmp, context, px, pw);
            }

            /* sort [start..right[ */
            హద్దు=కుడి;
        }
    } అయితే(ప్రారంభం<(హద్దు-1));
}

స్థిర శూన్య
శీఘ్ర_క్రమీకరించు(అక్షరం *array, int32_t length, int32_t అంశం_కొలత,
            UComparator *cmp, స్థిరరాశి శూన్య *context, UError_సంకేతం *p_Error_సంకేతం) {
    /* allocate two intermediate item variables (x and w) */
    icu::Maybe_దొంతర_Array<std::max_align_t, కొలత_లోపల_Max_Align_Ts(దొంతర_అంశం_కొలత) * 2> xw;
    ఒకవేళ(కొలత_లోపల_Max_Align_Ts(అంశం_కొలత)*2 > xw.get_సామర్థ్యం() &&
            xw.resize(కొలత_లోపల_Max_Align_Ts(అంశం_కొలత) * 2) == శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    sub_శీఘ్ర_క్రమీకరించు(array, 0, length, అంశం_కొలత, cmp, context,
                 xw.getAlias(), xw.getAlias() + కొలత_లోపల_Max_Align_Ts(అంశం_కొలత));
}

/* uprv_sortArray() API ----------------------------------------------------- */

/*
 * Check arguments, select an appropriate implementation,
 * cast the array to char * so that array+i*itemSize works.
 */
U_CAPI శూన్య U_EXPORT2
uprv_క్రమీకరించు_Array(శూన్య *array, int32_t length, int32_t అంశం_కొలత,
               UComparator *cmp, స్థిరరాశి శూన్య *context,
               UBool క్రమీకరించు_Stable, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ((length>0 && array==శూన్యసూచిక) || length<0 || అంశం_కొలత<=0 || cmp==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ఒకవేళ(length<=1) {
        తిరిగి;
    } లేకపోతే ఒకవేళ(length<MIN_QSORT || క్రమీకరించు_Stable) {
        insertion_క్రమీకరించు((అక్షరం *)array, length, అంశం_కొలత, cmp, context, p_Error_సంకేతం);
    } లేకపోతే {
        శీఘ్ర_క్రమీకరించు((అక్షరం *)array, length, అంశం_కొలత, cmp, context, p_Error_సంకేతం);
    }
}

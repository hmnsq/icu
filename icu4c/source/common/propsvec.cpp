// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  propsvec.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002feb22
*   created by: Markus W. Scherer
*
*   Store bits (Unicode character properties) in bit set vectors.
*/

#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "utrie.h"
#অন্তর্ভাব "utrie2.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "propsvec.h"
#অন্তর্ভাব "uassert.h"

কাঠামো UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t maxRows;
    int32_t rows;
    int32_t পূর্ব_Row;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#define UPVEC_INITIAL_ROWS (1<<12)
#define UPVEC_MEDIUM_ROWS ((int32_t)1<<16)
#define UPVEC_MAX_ROWS (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UErrorCode *pErrorCode) {
    UPropsVectors *pv;
    uint32_t *v, *row;
    uint32_t cp;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(আকার(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    যদি(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memset(pv, 0, আকার(UPropsVectors));
    pv->v=v;
    pv->columns=columns;
    pv->maxRows=UPVEC_INITIAL_ROWS;
    pv->rows=2+(UPVEC_MAX_CP-UPVEC_FIRST_SPECIAL_CP);

    /* set the all-Unicode row and the special-value rows */
    row=pv->v;
    uprv_memset(row, 0, pv->rows*columns*4);
    row[0]=0;
    row[1]=0x110000;
    row+=columns;
    জন্য(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    ফেরত pv;
}

U_CAPI শূন্য U_EXPORT2
upvec_close(UPropsVectors *pv) {
    যদি(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

স্থির uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, পূর্ব_Row;

    columns=pv->columns;
    limit=pv->rows;
    পূর্ব_Row=pv->পূর্ব_Row;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+পূর্ব_Row*columns;
    যদি (rangeStart >= static_cast<UChar32>(row[0])) {
        যদি (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            ফেরত row;
        } অন্যথা যদি (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->পূর্ব_Row=পূর্ব_Row+1;
            ফেরত row;
        } অন্যথা যদি (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->পূর্ব_Row=পূর্ব_Row+2;
            ফেরত row;
        } অন্যথা যদি ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            পূর্ব_Row+=2;
            করো {
                ++পূর্ব_Row;
                row+=columns;
            } যখন (rangeStart >= static_cast<UChar32>(row[1]));
            pv->পূর্ব_Row=পূর্ব_Row;
            ফেরত row;
        }
    } অন্যথা যদি (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->পূর্ব_Row=0;
        ফেরত pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    যখন(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        যদি (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } অন্যথা যদি (rangeStart < static_cast<UChar32>(row[1])) {
            pv->পূর্ব_Row=i;
            ফেরত row;
        } অন্যথা {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->পূর্ব_Row=start;
    ফেরত pv->v+start*columns;
}

U_CAPI শূন্য U_EXPORT2
upvec_set_মান(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    যদি( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    যদি(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        ফেরত;
    }
    limit=end+1;

    /* initialize */
    columns=pv->columns;
    column+=2; /* skip range start and limit columns */
    value&=mask;

    /* find the rows whose ranges overlap with the input range */

    /* find the first and last rows, always successful */
    firstRow=_findRow(pv, start);
    lastRow=_findRow(pv, end);

    /*
     * Rows need to be split if they partially overlap with the
     * input range (only possible for the first and last rows)
     * and if their value differs from the input value.
     */
    splitFirstRow = start != static_cast<UChar32>(firstRow[0]) && value != (firstRow[column] & mask);
    splitLastRow = limit != static_cast<UChar32>(lastRow[1]) && value != (lastRow[column] & mask);

    /* split first/last rows if necessary */
    যদি(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        যদি((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            যদি(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } অন্যথা যদি(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } অন্যথা {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                ফেরত;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            যদি(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            uprv_memcpy(newVectors, pv->v, (size_t)rows*columns*4);
            firstRow=newVectors+(firstRow-pv->v);
            lastRow=newVectors+(lastRow-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->maxRows=newMaxRows;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+rows*columns)-(lastRow+columns));
        যদি(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        যদি(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        যদি(splitLastRow) {
            /* copy the last row data */
            uprv_memcpy(lastRow+columns, lastRow, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            lastRow[1]=lastRow[columns]=(uint32_t)limit;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->পূর্ব_Row=(int32_t)((lastRow-(pv->v))/columns);

    /* set the input value in all remaining rows */
    firstRow+=column;
    lastRow+=column;
    mask=~mask;
    জন্য(;;) {
        *firstRow=(*firstRow&mask)|value;
        যদি(firstRow==lastRow) {
            বিরতি;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_মান(ধ্রুবক UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    যদি(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        ফেরত 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    ফেরত row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(ধ্রুবক UPropsVectors *pv, int32_t row_সূচক,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    যদি(pv->isCompacted || row_সূচক<0 || row_সূচক>=pv->rows) {
        ফেরত nullptr;
    }

    columns=pv->columns;
    row=pv->v+row_সূচক*columns;
    যদি(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    যদি(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    ফেরত row+2;
}

স্থির int32_t U_CALLCONV
upvec_compareRows(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *l, ধ্রুবক শূন্য *r) {
    ধ্রুবক uint32_t* left = static_cast<ধ্রুবক uint32_t*>(l), *ডান = static_cast<ধ্রুবক uint32_t*>(r);
    ধ্রুবক UPropsVectors* pv = static_cast<ধ্রুবক UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    করো {
        যদি(left[i]!=ডান[i]) {
            ফেরত left[i]<ডান[i] ? -1 : 1;
        }
        যদি(++i==columns) {
            i=0;
        }
    } যখন(--count>0);

    ফেরত 0;
}

U_CAPI শূন্য U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, শূন্য *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, মান_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    যদি(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    যদি(pv->isCompacted) {
        ফেরত;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=সত্য;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    মান_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, মিথ্যা, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-মান_Columns;
    জন্য(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        যদি(count<0 || 0!=uprv_memcmp(row+2, row-মান_Columns, মান_Columns*4)) {
            count+=মান_Columns;
        }

        যদি(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, মান_Columns, pErrorCode);
            যদি(U_FAILURE(*pErrorCode)) {
                ফেরত;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=মান_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_মানসমূহ_CP, UPVEC_START_REAL_মানসমূহ_CP,
            count, row-মান_Columns, মান_Columns, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-মান_Columns;
    জন্য(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        যদি(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, মান_Columns*4)) {
            count+=মান_Columns;
            uprv_memmove(pv->v+count, row+2, (size_t)মান_Columns*4);
        }

        যদি(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, মান_Columns, pErrorCode);
            যদি(U_FAILURE(*pErrorCode)) {
                ফেরত;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/মান_Columns+1;
}

U_CAPI ধ্রুবক uint32_t * U_EXPORT2
upvec_getArray(ধ্রুবক UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    যদি(!pv->isCompacted) {
        ফেরত nullptr;
    }
    যদি(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    যদি(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    ফেরত pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ধ্রুবক UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byte_দৈর্ঘ্য;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    byte_দৈর্ঘ্য=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_দৈর্ঘ্য);
    যদি(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byte_দৈর্ঘ্য);
    যদি(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    যদি(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    ফেরত clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_মান_BITS, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    ফেরত toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI শূন্য U_CALLCONV
upvec_compactToUTrie2Handler(শূন্য *context,
                             UChar32 start, UChar32 end,
                             int32_t row_সূচক, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (শূন্য)row;
    (শূন্য)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    যদি(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)row_সূচক, সত্য, pErrorCode);
    } অন্যথা {
        নির্বাচন(start) {
        ক্ষেত্র UPVEC_INITIAL_মান_CP:
            toUTrie2->initial_মান=row_সূচক;
            বিরতি;
        ক্ষেত্র UPVEC_ERROR_মান_CP:
            toUTrie2->error_মান=row_সূচক;
            বিরতি;
        ক্ষেত্র UPVEC_START_REAL_মানসমূহ_CP:
            toUTrie2->max_মান=row_সূচক;
            যদি(row_সূচক>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            } অন্যথা {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_মান,
                                           toUTrie2->error_মান, pErrorCode);
            }
            বিরতি;
        পূর্বনির্ধারিত:
            বিরতি;
        }
    }
}

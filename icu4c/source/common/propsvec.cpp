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

#ضمن <stdlib.h>
#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "utrie.h"
#ضمن "utrie2.h"
#ضمن "uarrsort.h"
#ضمن "propsvec.h"
#ضمن "uassert.h"

بنية UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t maxRows;
    int32_t rows;
    int32_t prevRow;  /* search optimization: remember last row seen */
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

    𐡄𐡍(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    𐡄𐡍(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(حجم(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    𐡄𐡍(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memset(pv, 0, حجم(UPropsVectors));
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
    لكل(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    أرجع pv;
}

U_CAPI فراغ U_EXPORT2
upvec_close(UPropsVectors *pv) {
    𐡄𐡍(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

ساكن uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, prevRow;

    columns=pv->columns;
    limit=pv->rows;
    prevRow=pv->prevRow;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+prevRow*columns;
    𐡄𐡍 (rangeStart >= static_cast<UChar32>(row[0])) {
        𐡄𐡍 (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            أرجع row;
        } وإلا 𐡄𐡍 (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->prevRow=prevRow+1;
            أرجع row;
        } وإلا 𐡄𐡍 (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->prevRow=prevRow+2;
            أرجع row;
        } وإلا 𐡄𐡍 ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            prevRow+=2;
            𐡏𐡁𐡃 {
                ++prevRow;
                row+=columns;
            } طالما (rangeStart >= static_cast<UChar32>(row[1]));
            pv->prevRow=prevRow;
            أرجع row;
        }
    } وإلا 𐡄𐡍 (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prevRow=0;
        أرجع pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    طالما(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        𐡄𐡍 (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } وإلا 𐡄𐡍 (rangeStart < static_cast<UChar32>(row[1])) {
            pv->prevRow=i;
            أرجع row;
        } وإلا {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prevRow=start;
    أرجع pv->v+start*columns;
}

U_CAPI فراغ U_EXPORT2
upvec_setValue(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    𐡄𐡍(U_FAILURE(*pErrorCode)) {
        أرجع;
    }
    𐡄𐡍( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    𐡄𐡍(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        أرجع;
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
    𐡄𐡍(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        𐡄𐡍((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            𐡄𐡍(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } وإلا 𐡄𐡍(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } وإلا {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                أرجع;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            𐡄𐡍(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                أرجع;
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
        𐡄𐡍(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        𐡄𐡍(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (size_t)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        𐡄𐡍(splitLastRow) {
            /* copy the last row data */
            uprv_memcpy(lastRow+columns, lastRow, (size_t)columns*4);

            /* split the range and move the firstRow pointer */
            lastRow[1]=lastRow[columns]=(uint32_t)limit;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->prevRow=(int32_t)((lastRow-(pv->v))/columns);

    /* set the input value in all remaining rows */
    firstRow+=column;
    lastRow+=column;
    mask=~mask;
    لكل(;;) {
        *firstRow=(*firstRow&mask)|value;
        𐡄𐡍(firstRow==lastRow) {
            اكسر;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_getValue(ثابت UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    𐡄𐡍(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        أرجع 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    أرجع row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(ثابت UPropsVectors *pv, int32_t rowIndex,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    𐡄𐡍(pv->isCompacted || rowIndex<0 || rowIndex>=pv->rows) {
        أرجع nullptr;
    }

    columns=pv->columns;
    row=pv->v+rowIndex*columns;
    𐡄𐡍(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    𐡄𐡍(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    أرجع row+2;
}

ساكن int32_t U_CALLCONV
upvec_compareRows(ثابت فراغ *context, ثابت فراغ *l, ثابت فراغ *r) {
    ثابت uint32_t* left = static_cast<ثابت uint32_t*>(l), *right = static_cast<ثابت uint32_t*>(r);
    ثابت UPropsVectors* pv = static_cast<ثابت UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    𐡏𐡁𐡃 {
        𐡄𐡍(left[i]!=right[i]) {
            أرجع left[i]<right[i] ? -1 : 1;
        }
        𐡄𐡍(++i==columns) {
            i=0;
        }
    } طالما(--count>0);

    أرجع 0;
}

U_CAPI فراغ U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, فراغ *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, valueColumns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    𐡄𐡍(U_FAILURE(*pErrorCode)) {
        أرجع;
    }
    𐡄𐡍(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    𐡄𐡍(pv->isCompacted) {
        أرجع;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=𐡒𐡔𐡈;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    valueColumns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, خطأ, pErrorCode);
    𐡄𐡍(U_FAILURE(*pErrorCode)) {
        أرجع;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-valueColumns;
    لكل(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        𐡄𐡍(count<0 || 0!=uprv_memcmp(row+2, row-valueColumns, valueColumns*4)) {
            count+=valueColumns;
        }

        𐡄𐡍(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, valueColumns, pErrorCode);
            𐡄𐡍(U_FAILURE(*pErrorCode)) {
                أرجع;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=valueColumns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_VALUES_CP, UPVEC_START_REAL_VALUES_CP,
            count, row-valueColumns, valueColumns, pErrorCode);
    𐡄𐡍(U_FAILURE(*pErrorCode)) {
        أرجع;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-valueColumns;
    لكل(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        𐡄𐡍(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, valueColumns*4)) {
            count+=valueColumns;
            uprv_memmove(pv->v+count, row+2, (size_t)valueColumns*4);
        }

        𐡄𐡍(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, valueColumns, pErrorCode);
            𐡄𐡍(U_FAILURE(*pErrorCode)) {
                أرجع;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/valueColumns+1;
}

U_CAPI ثابت uint32_t * U_EXPORT2
upvec_getArray(ثابت UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    𐡄𐡍(!pv->isCompacted) {
        أرجع nullptr;
    }
    𐡄𐡍(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    𐡄𐡍(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    أرجع pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(ثابت UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byteLength;

    𐡄𐡍(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    𐡄𐡍(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    byteLength=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byteLength);
    𐡄𐡍(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byteLength);
    𐡄𐡍(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    𐡄𐡍(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    أرجع clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_VALUE_BITS, pErrorCode);
    𐡄𐡍(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    أرجع toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI فراغ U_CALLCONV
upvec_compactToUTrie2Handler(فراغ *context,
                             UChar32 start, UChar32 end,
                             int32_t rowIndex, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (فراغ)row;
    (فراغ)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    𐡄𐡍(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)rowIndex, 𐡒𐡔𐡈, pErrorCode);
    } وإلا {
        بدل(start) {
        حالة UPVEC_INITIAL_VALUE_CP:
            toUTrie2->initialValue=rowIndex;
            اكسر;
        حالة UPVEC_ERROR_VALUE_CP:
            toUTrie2->errorValue=rowIndex;
            اكسر;
        حالة UPVEC_START_REAL_VALUES_CP:
            toUTrie2->maxValue=rowIndex;
            𐡄𐡍(rowIndex>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            } وإلا {
                toUTrie2->trie=utrie2_open(toUTrie2->initialValue,
                                           toUTrie2->errorValue, pErrorCode);
            }
            اكسر;
        افتراضي:
            اكسر;
        }
    }
}

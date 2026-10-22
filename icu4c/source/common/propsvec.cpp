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

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "utrie.h"
#अन्तर्भाव "utrie2.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "propsvec.h"
#अन्तर्भाव "uassert.h"

संरचना UPropsVectors {
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

    जर(U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }
    जर(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(आकार(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    जर(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    uprv_memset(pv, 0, आकार(UPropsVectors));
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
    साठी(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    परत pv;
}

U_CAPI शून्य U_EXPORT2
upvec_close(UPropsVectors *pv) {
    जर(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

स्थिर uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, prevRow;

    columns=pv->columns;
    limit=pv->rows;
    prevRow=pv->prevRow;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+prevRow*columns;
    जर (rangeStart >= static_cast<UChar32>(row[0])) {
        जर (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            परत row;
        } अन्यथा जर (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->prevRow=prevRow+1;
            परत row;
        } अन्यथा जर (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->prevRow=prevRow+2;
            परत row;
        } अन्यथा जर ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            prevRow+=2;
            करा {
                ++prevRow;
                row+=columns;
            } पर्यंत (rangeStart >= static_cast<UChar32>(row[1]));
            pv->prevRow=prevRow;
            परत row;
        }
    } अन्यथा जर (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->prevRow=0;
        परत pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    पर्यंत(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        जर (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } अन्यथा जर (rangeStart < static_cast<UChar32>(row[1])) {
            pv->prevRow=i;
            परत row;
        } अन्यथा {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->prevRow=start;
    परत pv->v+start*columns;
}

U_CAPI शून्य U_EXPORT2
upvec_set_मूल्य(UPropsVectors *pv,
               UChar32 start, UChar32 end,
               int32_t column,
               uint32_t value, uint32_t mask,
               UErrorCode *pErrorCode) {
    uint32_t *firstRow, *lastRow;
    int32_t columns;
    UChar32 limit;
    UBool splitFirstRow, splitLastRow;

    /* argument checking */
    जर(U_FAILURE(*pErrorCode)) {
        परत;
    }
    जर( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    जर(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        परत;
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
    जर(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        जर((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            जर(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } अन्यथा जर(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } अन्यथा {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                परत;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            जर(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                परत;
            }
            uprv_memcpy(newVectors, pv->v, (माप_प्रकार)rows*columns*4);
            firstRow=newVectors+(firstRow-pv->v);
            lastRow=newVectors+(lastRow-pv->v);
            uprv_free(pv->v);
            pv->v=newVectors;
            pv->maxRows=newMaxRows;
        }

        /* count the number of row cells to move after the last row, and move them */
        count = (int32_t)((pv->v+rows*columns)-(lastRow+columns));
        जर(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        जर(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (माप_प्रकार)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        जर(splitLastRow) {
            /* copy the last row data */
            uprv_memcpy(lastRow+columns, lastRow, (माप_प्रकार)columns*4);

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
    साठी(;;) {
        *firstRow=(*firstRow&mask)|value;
        जर(firstRow==lastRow) {
            थांबा;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_मूल्य(स्थिरांक UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    जर(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        परत 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    परत row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(स्थिरांक UPropsVectors *pv, int32_t rowIndex,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    जर(pv->isCompacted || rowIndex<0 || rowIndex>=pv->rows) {
        परत nullptr;
    }

    columns=pv->columns;
    row=pv->v+rowIndex*columns;
    जर(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    जर(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    परत row+2;
}

स्थिर int32_t U_CALLCONV
upvec_compareRows(स्थिरांक शून्य *context, स्थिरांक शून्य *l, स्थिरांक शून्य *r) {
    स्थिरांक uint32_t* left = static_cast<स्थिरांक uint32_t*>(l), *उजवा = static_cast<स्थिरांक uint32_t*>(r);
    स्थिरांक UPropsVectors* pv = static_cast<स्थिरांक UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    करा {
        जर(left[i]!=उजवा[i]) {
            परत left[i]<उजवा[i] ? -1 : 1;
        }
        जर(++i==columns) {
            i=0;
        }
    } पर्यंत(--count>0);

    परत 0;
}

U_CAPI शून्य U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, शून्य *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, मूल्य_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    जर(U_FAILURE(*pErrorCode)) {
        परत;
    }
    जर(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    जर(pv->isCompacted) {
        परत;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=सत्य;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    मूल्य_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, असत्य, pErrorCode);
    जर(U_FAILURE(*pErrorCode)) {
        परत;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-मूल्य_Columns;
    साठी(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        जर(count<0 || 0!=uprv_memcmp(row+2, row-मूल्य_Columns, मूल्य_Columns*4)) {
            count+=मूल्य_Columns;
        }

        जर(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, मूल्य_Columns, pErrorCode);
            जर(U_FAILURE(*pErrorCode)) {
                परत;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=मूल्य_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_मूल्ये_CP, UPVEC_START_REAL_मूल्ये_CP,
            count, row-मूल्य_Columns, मूल्य_Columns, pErrorCode);
    जर(U_FAILURE(*pErrorCode)) {
        परत;
    }

    /*
     * Move vector contents up to a contiguous array with only unique
     * vector values, and call the handler function for each vector.
     *
     * This destroys the Properties Vector structure and replaces it
     * with an array of just vector values.
     */
    row=pv->v;
    count=-मूल्य_Columns;
    साठी(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        जर(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, मूल्य_Columns*4)) {
            count+=मूल्य_Columns;
            uprv_memmove(pv->v+count, row+2, (माप_प्रकार)मूल्य_Columns*4);
        }

        जर(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, मूल्य_Columns, pErrorCode);
            जर(U_FAILURE(*pErrorCode)) {
                परत;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/मूल्य_Columns+1;
}

U_CAPI स्थिरांक uint32_t * U_EXPORT2
upvec_getArray(स्थिरांक UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    जर(!pv->isCompacted) {
        परत nullptr;
    }
    जर(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    जर(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    परत pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(स्थिरांक UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byteLength;

    जर(U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }
    जर(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    byteLength=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byteLength);
    जर(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byteLength);
    जर(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    जर(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    परत clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_मूल्य_BITS, pErrorCode);
    जर(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    परत toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI शून्य U_CALLCONV
upvec_compactToUTrie2Handler(शून्य *context,
                             UChar32 start, UChar32 end,
                             int32_t rowIndex, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (शून्य)row;
    (शून्य)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    जर(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)rowIndex, सत्य, pErrorCode);
    } अन्यथा {
        निवड(start) {
        स्थिती UPVEC_INITIAL_मूल्य_CP:
            toUTrie2->initial_मूल्य=rowIndex;
            थांबा;
        स्थिती UPVEC_ERROR_मूल्य_CP:
            toUTrie2->error_मूल्य=rowIndex;
            थांबा;
        स्थिती UPVEC_START_REAL_मूल्ये_CP:
            toUTrie2->max_मूल्य=rowIndex;
            जर(rowIndex>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            } अन्यथा {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_मूल्य,
                                           toUTrie2->error_मूल्य, pErrorCode);
            }
            थांबा;
        पूर्वनिर्धारित:
            थांबा;
        }
    }
}

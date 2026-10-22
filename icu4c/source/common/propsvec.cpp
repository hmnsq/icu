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

#समावेश <stdlib.h>
#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"
#समावेश "utrie.h"
#समावेश "utrie2.h"
#समावेश "uarrsort.h"
#समावेश "propsvec.h"
#समावेश "uassert.h"

काष्ठा UPropsVectors {
    uint32_t *v;
    int32_t columns;  /* number of columns, plus two for start & limit values */
    int32_t maxRows;
    int32_t rows;
    int32_t पूर्व_Row;  /* search optimization: remember last row seen */
    UBool isCompacted;
};

#घोषणा UPVEC_INITIAL_ROWS (1<<12)
#घोषणा UPVEC_MEDIUM_ROWS ((int32_t)1<<16)
#घोषणा UPVEC_MAX_ROWS (UPVEC_MAX_CP+1)

U_CAPI UPropsVectors * U_EXPORT2
upvec_open(int32_t columns, UErrorCode *pErrorCode) {
    UPropsVectors *pv;
    uint32_t *v, *row;
    uint32_t cp;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर(columns<1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    columns+=2; /* count range start and limit columns */

    pv=(UPropsVectors *)uprv_malloc(माप(UPropsVectors));
    v=(uint32_t *)uprv_malloc(UPVEC_INITIAL_ROWS*columns*4);
    अगर(pv==nullptr || v==nullptr) {
        uprv_free(pv);
        uprv_free(v);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memset(pv, 0, माप(UPropsVectors));
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
    क्रम(cp=UPVEC_FIRST_SPECIAL_CP; cp<=UPVEC_MAX_CP; ++cp) {
        row[0]=cp;
        row[1]=cp+1;
        row+=columns;
    }
    वापस pv;
}

U_CAPI व्योम U_EXPORT2
upvec_close(UPropsVectors *pv) {
    अगर(pv!=nullptr) {
        uprv_free(pv->v);
        uprv_free(pv);
    }
}

अटल uint32_t *
_findRow(UPropsVectors *pv, UChar32 rangeStart) {
    uint32_t *row;
    int32_t columns, i, start, limit, पूर्व_Row;

    columns=pv->columns;
    limit=pv->rows;
    पूर्व_Row=pv->पूर्व_Row;

    /* check the vicinity of the last-seen row (start searching with an unrolled loop) */
    row=pv->v+पूर्व_Row*columns;
    अगर (rangeStart >= static_cast<UChar32>(row[0])) {
        अगर (rangeStart < static_cast<UChar32>(row[1])) {
            /* same row as last seen */
            वापस row;
        } अन्यथा अगर (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* next row after the last one */
            pv->पूर्व_Row=पूर्व_Row+1;
            वापस row;
        } अन्यथा अगर (rangeStart < static_cast<UChar32>((row += columns)[1])) {
            /* second row after the last one */
            pv->पूर्व_Row=पूर्व_Row+2;
            वापस row;
        } अन्यथा अगर ((rangeStart - static_cast<UChar32>(row[1])) < 10) {
            /* we are close, continue looping */
            पूर्व_Row+=2;
            करो {
                ++पूर्व_Row;
                row+=columns;
            } जबतक (rangeStart >= static_cast<UChar32>(row[1]));
            pv->पूर्व_Row=पूर्व_Row;
            वापस row;
        }
    } अन्यथा अगर (rangeStart < static_cast<UChar32>(pv->v[1])) {
        /* the very first row */
        pv->पूर्व_Row=0;
        वापस pv->v;
    }

    /* do a binary search for the start of the range */
    start=0;
    जबतक(start<limit-1) {
        i=(start+limit)/2;
        row=pv->v+i*columns;
        अगर (rangeStart < static_cast<UChar32>(row[0])) {
            limit=i;
        } अन्यथा अगर (rangeStart < static_cast<UChar32>(row[1])) {
            pv->पूर्व_Row=i;
            वापस row;
        } अन्यथा {
            start=i;
        }
    }

    /* must be found because all ranges together always cover all of Unicode */
    pv->पूर्व_Row=start;
    वापस pv->v+start*columns;
}

U_CAPI व्योम U_EXPORT2
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
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    अगर( pv==nullptr ||
        start<0 || start>end || end>UPVEC_MAX_CP ||
        column<0 || column>=(pv->columns-2)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर(pv->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        वापस;
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
    अगर(splitFirstRow || splitLastRow) {
        int32_t count, rows;

        rows=pv->rows;
        अगर((rows+splitFirstRow+splitLastRow)>pv->maxRows) {
            uint32_t *newVectors;
            int32_t newMaxRows;

            अगर(pv->maxRows<UPVEC_MEDIUM_ROWS) {
                newMaxRows=UPVEC_MEDIUM_ROWS;
            } अन्यथा अगर(pv->maxRows<UPVEC_MAX_ROWS) {
                newMaxRows=UPVEC_MAX_ROWS;
            } अन्यथा {
                /* Implementation bug, or UPVEC_MAX_ROWS too low. */
                *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                वापस;
            }
            newVectors=(uint32_t *)uprv_malloc(newMaxRows*columns*4);
            अगर(newVectors==nullptr) {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                वापस;
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
        अगर(count>0) {
            uprv_memmove(
                lastRow+(1+splitFirstRow+splitLastRow)*columns,
                lastRow+columns,
                count*4);
        }
        pv->rows=rows+splitFirstRow+splitLastRow;

        /* split the first row, and move the firstRow pointer to the second part */
        अगर(splitFirstRow) {
            /* copy all affected rows up one and move the lastRow pointer */
            count = (int32_t)((lastRow-firstRow)+columns);
            uprv_memmove(firstRow+columns, firstRow, (माप_प्रकार)count*4);
            lastRow+=columns;

            /* split the range and move the firstRow pointer */
            firstRow[1]=firstRow[columns]=(uint32_t)start;
            firstRow+=columns;
        }

        /* split the last row */
        अगर(splitLastRow) {
            /* copy the last row data */
            uprv_memcpy(lastRow+columns, lastRow, (माप_प्रकार)columns*4);

            /* split the range and move the firstRow pointer */
            lastRow[1]=lastRow[columns]=(uint32_t)limit;
        }
    }

    /* set the "row last seen" to the last row for the range */
    pv->पूर्व_Row=(int32_t)((lastRow-(pv->v))/columns);

    /* set the input value in all remaining rows */
    firstRow+=column;
    lastRow+=column;
    mask=~mask;
    क्रम(;;) {
        *firstRow=(*firstRow&mask)|value;
        अगर(firstRow==lastRow) {
            अवरोध;
        }
        firstRow+=columns;
    }
}

U_CAPI uint32_t U_EXPORT2
upvec_get_मूल्य(स्थिर UPropsVectors *pv, UChar32 c, int32_t column) {
    uint32_t *row;
    UPropsVectors *ncpv;

    अगर(pv->isCompacted || c<0 || c>UPVEC_MAX_CP || column<0 || column>=(pv->columns-2)) {
        वापस 0;
    }
    ncpv=(UPropsVectors *)pv;
    row=_findRow(ncpv, c);
    वापस row[2+column];
}

U_CAPI uint32_t * U_EXPORT2
upvec_getRow(स्थिर UPropsVectors *pv, int32_t row_सूचकांक,
             UChar32 *pRangeStart, UChar32 *pRangeEnd) {
    uint32_t *row;
    int32_t columns;

    अगर(pv->isCompacted || row_सूचकांक<0 || row_सूचकांक>=pv->rows) {
        वापस nullptr;
    }

    columns=pv->columns;
    row=pv->v+row_सूचकांक*columns;
    अगर(pRangeStart!=nullptr) {
        *pRangeStart=(UChar32)row[0];
    }
    अगर(pRangeEnd!=nullptr) {
        *pRangeEnd=(UChar32)row[1]-1;
    }
    वापस row+2;
}

अटल int32_t U_CALLCONV
upvec_compareRows(स्थिर व्योम *context, स्थिर व्योम *l, स्थिर व्योम *r) {
    स्थिर uint32_t* बाँए = static_cast<स्थिर uint32_t*>(l), *दाँए = static_cast<स्थिर uint32_t*>(r);
    स्थिर UPropsVectors* pv = static_cast<स्थिर UPropsVectors*>(context);
    int32_t i, count, columns;

    count=columns=pv->columns; /* includes start/limit columns */

    /* start comparing after start/limit but wrap around to them */
    i=2;
    करो {
        अगर(बाँए[i]!=दाँए[i]) {
            वापस बाँए[i]<दाँए[i] ? -1 : 1;
        }
        अगर(++i==columns) {
            i=0;
        }
    } जबतक(--count>0);

    वापस 0;
}

U_CAPI व्योम U_EXPORT2
upvec_compact(UPropsVectors *pv, UPVecCompactHandler *handler, व्योम *context, UErrorCode *pErrorCode) {
    uint32_t *row;
    int32_t i, columns, मूल्य_Columns, rows, count;
    UChar32 start, limit;

    /* argument checking */
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    अगर(handler==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर(pv->isCompacted) {
        वापस;
    }

    /* Set the flag now: Sorting and compacting destroys the builder data structure. */
    pv->isCompacted=true;

    rows=pv->rows;
    columns=pv->columns;
    U_ASSERT(columns>=3); /* upvec_open asserts this */
    मूल्य_Columns=columns-2; /* not counting start & limit */

    /* sort the properties vectors to find unique vector values */
    uprv_sortArray(pv->v, rows, columns*4,
                   upvec_compareRows, pv, false, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }

    /*
     * Find and set the special values.
     * This has to do almost the same work as the compaction below,
     * to find the indexes where the special-value rows will move.
     */
    row=pv->v;
    count=-मूल्य_Columns;
    क्रम(i=0; i<rows; ++i) {
        start=(UChar32)row[0];

        /* count a new values vector if it is different from the current one */
        अगर(count<0 || 0!=uprv_memcmp(row+2, row-मूल्य_Columns, मूल्य_Columns*4)) {
            count+=मूल्य_Columns;
        }

        अगर(start>=UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, start, count, row+2, मूल्य_Columns, pErrorCode);
            अगर(U_FAILURE(*pErrorCode)) {
                वापस;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add valueColumns to include that last vector */
    count+=मूल्य_Columns;

    /* Call the handler once more to signal the start of delivering real values. */
    handler(context, UPVEC_START_REAL_मूल्यावली_CP, UPVEC_START_REAL_मूल्यावली_CP,
            count, row-मूल्य_Columns, मूल्य_Columns, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
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
    क्रम(i=0; i<rows; ++i) {
        /* fetch these first before memmove() may overwrite them */
        start=(UChar32)row[0];
        limit=(UChar32)row[1];

        /* add a new values vector if it is different from the current one */
        अगर(count<0 || 0!=uprv_memcmp(row+2, pv->v+count, मूल्य_Columns*4)) {
            count+=मूल्य_Columns;
            uprv_memmove(pv->v+count, row+2, (माप_प्रकार)मूल्य_Columns*4);
        }

        अगर(start<UPVEC_FIRST_SPECIAL_CP) {
            handler(context, start, limit-1, count, pv->v+count, मूल्य_Columns, pErrorCode);
            अगर(U_FAILURE(*pErrorCode)) {
                वापस;
            }
        }

        row+=columns;
    }

    /* count is at the beginning of the last vector, add one to include that last vector */
    pv->rows=count/मूल्य_Columns+1;
}

U_CAPI स्थिर uint32_t * U_EXPORT2
upvec_getArray(स्थिर UPropsVectors *pv, int32_t *pRows, int32_t *pColumns) {
    अगर(!pv->isCompacted) {
        वापस nullptr;
    }
    अगर(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    अगर(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    वापस pv->v;
}

U_CAPI uint32_t * U_EXPORT2
upvec_cloneArray(स्थिर UPropsVectors *pv,
                 int32_t *pRows, int32_t *pColumns, UErrorCode *pErrorCode) {
    uint32_t *clonedArray;
    int32_t byte_लंबाई;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर(!pv->isCompacted) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    byte_लंबाई=pv->rows*(pv->columns-2)*4;
    clonedArray=(uint32_t *)uprv_malloc(byte_लंबाई);
    अगर(clonedArray==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memcpy(clonedArray, pv->v, byte_लंबाई);
    अगर(pRows!=nullptr) {
        *pRows=pv->rows;
    }
    अगर(pColumns!=nullptr) {
        *pColumns=pv->columns-2;
    }
    वापस clonedArray;
}

U_CAPI UTrie2 * U_EXPORT2
upvec_compactToUTrie2WithRowIndexes(UPropsVectors *pv, UErrorCode *pErrorCode) {
    UPVecToUTrie2Context toUTrie2={ nullptr, 0, 0, 0 };
    upvec_compact(pv, upvec_compactToUTrie2Handler, &toUTrie2, pErrorCode);
    utrie2_freeze(toUTrie2.trie, UTRIE2_16_मूल्य_BITS, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        utrie2_close(toUTrie2.trie);
        toUTrie2.trie=nullptr;
    }
    वापस toUTrie2.trie;
}

/*
 * TODO(markus): Add upvec_16BitsToUTrie2() function that enumerates all rows, extracts
 * some 16-bit field and builds and returns a UTrie2.
 */

U_CAPI व्योम U_CALLCONV
upvec_compactToUTrie2Handler(व्योम *context,
                             UChar32 start, UChar32 end,
                             int32_t row_सूचकांक, uint32_t *row, int32_t columns,
                             UErrorCode *pErrorCode) {
    (व्योम)row;
    (व्योम)columns;
    UPVecToUTrie2Context *toUTrie2=(UPVecToUTrie2Context *)context;
    अगर(start<UPVEC_FIRST_SPECIAL_CP) {
        utrie2_setRange32(toUTrie2->trie, start, end, (uint32_t)row_सूचकांक, true, pErrorCode);
    } अन्यथा {
        चयन(start) {
        हाल UPVEC_INITIAL_मूल्य_CP:
            toUTrie2->initial_मूल्य=row_सूचकांक;
            अवरोध;
        हाल UPVEC_ERROR_मूल्य_CP:
            toUTrie2->error_मूल्य=row_सूचकांक;
            अवरोध;
        हाल UPVEC_START_REAL_मूल्यावली_CP:
            toUTrie2->max_मूल्य=row_सूचकांक;
            अगर(row_सूचकांक>0xffff) {
                /* too many rows for a 16-bit trie */
                *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            } अन्यथा {
                toUTrie2->trie=utrie2_open(toUTrie2->initial_मूल्य,
                                           toUTrie2->error_मूल्य, pErrorCode);
            }
            अवरोध;
        शेष:
            अवरोध;
        }
    }
}

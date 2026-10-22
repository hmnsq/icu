// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
 *  ucnv_cb.c:
 *  External APIs for the ICU's codeset conversion library
 *  Helena Shih
 *
 * Modification History:
 *
 *   Date        Name        Description
 *   7/28/2000   srl         Implementation
 */

/**
 * @name Character Conversion C API
 *
 */

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ucnv_cb.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI व्योम  U_EXPORT2
ucnv_cbFromUWriteBytes (UConverterFromUnicodeArgs *args,
                       स्थिर अक्षर* source,
                       int32_t length,
                       int32_t offset_सूचकांक,
                       UErrorCode * err)
{
    अगर(U_FAILURE(*err)) {
        वापस;
    }

    ucnv_fromUWriteBytes(
        args->converter,
        source, length,
        &args->target, args->targetLimit,
        &args->offsets, offset_सूचकांक,
        err);
}

U_CAPI व्योम  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverterFromUnicodeArgs *args,
                             स्थिर char16_t** source,
                             स्थिर char16_t*  sourceLimit,
                             int32_t offset_सूचकांक,
                             UErrorCode * err)
{
    /*
    This is a fun one.  Recursion can occur - we're basically going to
    just retry shoving data through the same converter. Note, if you got
    here through some kind of invalid sequence, you maybe should emit a
    reset sequence of some kind and/or call ucnv_reset().  Since this
    IS an actual conversion, take care that you've changed the callback
    or the data, or you'll get an infinite loop.

    Please set the err value to something reasonable before calling
    into this.
    */

    अक्षर *oldTarget;

    अगर(U_FAILURE(*err))
    {
        वापस;
    }

    oldTarget = args->target;

    ucnv_fromUnicode(args->converter,
        &args->target,
        args->targetLimit,
        source,
        sourceLimit,
        nullptr, /* no offsets */
        false, /* no flush */
        err);

    अगर(args->offsets)
    {
        जबतक (args->target != oldTarget)  /* if it moved at all.. */
        {
            *(args->offsets)++ = offset_सूचकांक;
            oldTarget++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    अगर(*err == U_बफर_OVERFLOW_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        अक्षर *newTarget;
        स्थिर अक्षर *newTargetLimit;
        UErrorCode err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = args->converter->char_Error_बफर_लंबाई;

        /* start the new target at the first free slot in the errbuff.. */
        newTarget = (अक्षर *)(args->converter->char_Error_बफर + errBuffLen);

        newTargetLimit = (अक्षर *)(args->converter->char_Error_बफर +
            माप(args->converter->char_Error_बफर));

        अगर(newTarget >= newTargetLimit)
        {
            *err = U_INTERNAL_PROGRAM_ERROR;
            वापस;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        args->converter->char_Error_बफर_लंबाई = 0;

        ucnv_fromUnicode(args->converter,
                         &newTarget,
                         newTargetLimit,
                         source,
                         sourceLimit,
                         nullptr,
                         false,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        args->converter->char_Error_बफर_लंबाई = (int8_t)(
            newTarget - (अक्षर*)args->converter->char_Error_बफर);

        अगर((newTarget >= newTargetLimit) || (err2 == U_बफर_OVERFLOW_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_INTERNAL_PROGRAM_ERROR;
            वापस;
        }
        /*else {*/
            /* sub errs could be invalid/truncated/illegal chars or w/e.
            These might want to be passed on up.. But the problem is, we already
            need to pass U_BUFFER_OVERFLOW_ERROR. That has to override these
            other errs.. */

            /*
            if(U_FAILURE(err2))
            ??
            */
        /*}*/
    }
}

U_CAPI व्योम  U_EXPORT2
ucnv_cbFromUWriteSub (UConverterFromUnicodeArgs *args,
                           int32_t offset_सूचकांक,
                           UErrorCode * err)
{
    UConverter *converter;
    int32_t length;

    अगर(U_FAILURE(*err)) {
        वापस;
    }
    converter = args->converter;
    length = converter->subCharLen;

    अगर(length == 0) {
        वापस;
    }

    अगर(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        स्थिर char16_t *source = (स्थिर char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(args, &source, source - length, offset_सूचकांक, err);
        वापस;
    }

    अगर(converter->shared_तथ्य->impl->writeSub!=nullptr) {
        converter->shared_तथ्य->impl->writeSub(args, offset_सूचकांक, err);
    }
    अन्यथा अगर(converter->subChar1!=0 && (uint16_t)converter->invalid_UChar_बफर[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cbFromUWriteBytes(args,
                               (स्थिर अक्षर *)&converter->subChar1, 1,
                               offset_सूचकांक, err);
    }
    अन्यथा {
        ucnv_cbFromUWriteBytes(args,
                               (स्थिर अक्षर *)converter->subChars, length,
                               offset_सूचकांक, err);
    }
}

U_CAPI व्योम  U_EXPORT2
ucnv_cbToUWriteUChars (UConverterToUnicodeArgs *args,
                            स्थिर char16_t* source,
                            int32_t length,
                            int32_t offset_सूचकांक,
                            UErrorCode * err)
{
    अगर(U_FAILURE(*err)) {
        वापस;
    }

    ucnv_toUWriteUChars(
        args->converter,
        source, length,
        &args->target, args->targetLimit,
        &args->offsets, offset_सूचकांक,
        err);
}

U_CAPI व्योम  U_EXPORT2
ucnv_cbToUWriteSub (UConverterToUnicodeArgs *args,
                         int32_t offset_सूचकांक,
                       UErrorCode * err)
{
    अटल स्थिर char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    अगर(args->converter->invalid_Char_लंबाई == 1 && args->converter->subChar1 != 0) {
        ucnv_cbToUWriteUChars(args, &kSubstituteChar1, 1, offset_सूचकांक, err);
    } अन्यथा {
        ucnv_cbToUWriteUChars(args, &kSubstituteChar, 1, offset_सूचकांक, err);
    }
}

#पूर्ण_अगर

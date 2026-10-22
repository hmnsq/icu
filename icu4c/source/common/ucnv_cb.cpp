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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI शून्य  U_EXPORT2
ucnv_cb_From_UWrite_अष्टकानि (UConverter_From_Unicode_तर्काः *तर्काः_2,
                       ध्रुव वर्ण* source,
                       int32_t length,
                       int32_t विस्थापनम्_सूचकांक,
                       UError_संहिता * err)
{
    यदि(U_FAILURE(*err)) {
        प्रत्यागम;
    }

    ucnv_from_UWrite_अष्टकानि(
        तर्काः_2->converter,
        source, length,
        &तर्काः_2->target, तर्काः_2->target_सीमा,
        &तर्काः_2->offsets, विस्थापनम्_सूचकांक,
        err);
}

U_CAPI शून्य  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverter_From_Unicode_तर्काः *तर्काः_2,
                             ध्रुव char16_t** source,
                             ध्रुव char16_t*  source_सीमा,
                             int32_t विस्थापनम्_सूचकांक,
                             UError_संहिता * err)
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

    वर्ण *oldTarget;

    यदि(U_FAILURE(*err))
    {
        प्रत्यागम;
    }

    oldTarget = तर्काः_2->target;

    ucnv_fromUnicode(तर्काः_2->converter,
        &तर्काः_2->target,
        तर्काः_2->target_सीमा,
        source,
        source_सीमा,
        शून्यसूचकः, /* no offsets */
        असत्य, /* no flush */
        err);

    यदि(तर्काः_2->offsets)
    {
        यावत् (तर्काः_2->target != oldTarget)  /* if it moved at all.. */
        {
            *(तर्काः_2->offsets)++ = विस्थापनम्_सूचकांक;
            oldTarget++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    यदि(*err == U_बफर_अतिप्रवाहः_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        वर्ण *newTarget;
        ध्रुव वर्ण *new_Target_सीमा;
        UError_संहिता err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = तर्काः_2->converter->char_Error_बफर_दीर्घता;

        /* start the new target at the first free slot in the errbuff.. */
        newTarget = (वर्ण *)(तर्काः_2->converter->char_Error_बफर + errBuffLen);

        new_Target_सीमा = (वर्ण *)(तर्काः_2->converter->char_Error_बफर +
            परिमाण(तर्काः_2->converter->char_Error_बफर));

        यदि(newTarget >= new_Target_सीमा)
        {
            *err = U_आन्तरिकम्_क्रमादेशः_ERROR;
            प्रत्यागम;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        तर्काः_2->converter->char_Error_बफर_दीर्घता = 0;

        ucnv_fromUnicode(तर्काः_2->converter,
                         &newTarget,
                         new_Target_सीमा,
                         source,
                         source_सीमा,
                         शून्यसूचकः,
                         असत्य,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        तर्काः_2->converter->char_Error_बफर_दीर्घता = (int8_t)(
            newTarget - (वर्ण*)तर्काः_2->converter->char_Error_बफर);

        यदि((newTarget >= new_Target_सीमा) || (err2 == U_बफर_अतिप्रवाहः_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_आन्तरिकम्_क्रमादेशः_ERROR;
            प्रत्यागम;
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

U_CAPI शून्य  U_EXPORT2
ucnv_cbFromUWriteSub (UConverter_From_Unicode_तर्काः *तर्काः_2,
                           int32_t विस्थापनम्_सूचकांक,
                           UError_संहिता * err)
{
    UConverter *converter;
    int32_t length;

    यदि(U_FAILURE(*err)) {
        प्रत्यागम;
    }
    converter = तर्काः_2->converter;
    length = converter->subCharLen;

    यदि(length == 0) {
        प्रत्यागम;
    }

    यदि(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        ध्रुव char16_t *source = (ध्रुव char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(तर्काः_2, &source, source - length, विस्थापनम्_सूचकांक, err);
        प्रत्यागम;
    }

    यदि(converter->shared_दत्त->impl->writeSub!=शून्यसूचकः) {
        converter->shared_दत्त->impl->writeSub(तर्काः_2, विस्थापनम्_सूचकांक, err);
    }
    अन्यथा यदि(converter->subChar1!=0 && (uint16_t)converter->invalid_UChar_बफर[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cb_From_UWrite_अष्टकानि(तर्काः_2,
                               (ध्रुव वर्ण *)&converter->subChar1, 1,
                               विस्थापनम्_सूचकांक, err);
    }
    अन्यथा {
        ucnv_cb_From_UWrite_अष्टकानि(तर्काः_2,
                               (ध्रुव वर्ण *)converter->subChars, length,
                               विस्थापनम्_सूचकांक, err);
    }
}

U_CAPI शून्य  U_EXPORT2
ucnv_cb_गन्तव्यम्_UWrite_UChars (UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                            ध्रुव char16_t* source,
                            int32_t length,
                            int32_t विस्थापनम्_सूचकांक,
                            UError_संहिता * err)
{
    यदि(U_FAILURE(*err)) {
        प्रत्यागम;
    }

    ucnv_गन्तव्यम्_UWrite_UChars(
        तर्काः_2->converter,
        source, length,
        &तर्काः_2->target, तर्काः_2->target_सीमा,
        &तर्काः_2->offsets, विस्थापनम्_सूचकांक,
        err);
}

U_CAPI शून्य  U_EXPORT2
ucnv_cb_गन्तव्यम्_UWrite_Sub (UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                         int32_t विस्थापनम्_सूचकांक,
                       UError_संहिता * err)
{
    स्थिर ध्रुव char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    यदि(तर्काः_2->converter->invalid_Char_दीर्घता == 1 && तर्काः_2->converter->subChar1 != 0) {
        ucnv_cb_गन्तव्यम्_UWrite_UChars(तर्काः_2, &kSubstituteChar1, 1, विस्थापनम्_सूचकांक, err);
    } अन्यथा {
        ucnv_cb_गन्तव्यम्_UWrite_UChars(तर्काः_2, &kSubstituteChar, 1, विस्थापनम्_सूचकांक, err);
    }
}

#यदि_अन्तः

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2005-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  utext.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2005apr12
*   created by: Markus W. Scherer
*/

#అన్తర్భావ <cstddef>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/chariter.h"
#అన్తర్భావ "unicode/utext.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "putilimp.h"

U_NAMESPACE_USE

#నిర్వచించు I32_FLAG(బిట్టు_సూచీ) ((int32_t)1<<(బిట్టు_సూచీ))


స్థిర UBool
utext_access(UText *ut, int64_t సూచీ, UBool forward) {
    తిరిగి ut->pFuncs->access(ut, సూచీ, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    ఒకవేళ (delta > 0) {
        చేయి {
            ఒకవేళ(ut->chunk_స్థానభ్రంశం>=ut->chunk_నిడివి && !utext_access(ut, ut->chunk_Native_హద్దు, నిజం)) {
                తిరిగి అబద్ధం;
            }
            c = ut->chunkContents[ut->chunk_స్థానభ్రంశం];
            ఒకవేళ (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                ఒకవేళ (c == U_SENTINEL) {
                    తిరిగి అబద్ధం;
                }
            } లేకపోతే {
                ut->chunk_స్థానభ్రంశం++;
            }
        } అయితే(--delta>0);

    } లేకపోతే ఒకవేళ (delta<0) {
        చేయి {
            ఒకవేళ(ut->chunk_స్థానభ్రంశం<=0 && !utext_access(ut, ut->chunk_Native_ప్రారంభం, అబద్ధం)) {
                తిరిగి అబద్ధం;
            }
            c = ut->chunkContents[ut->chunk_స్థానభ్రంశం-1];
            ఒకవేళ (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                ఒకవేళ (c == U_SENTINEL) {
                    తిరిగి అబద్ధం;
                }
            } లేకపోతే {
                ut->chunk_స్థానభ్రంశం--;
            }
        } అయితే(++delta<0);
    }

    తిరిగి నిజం;
}


U_CAPI int64_t U_EXPORT2
utext_native_నిడివి(UText *ut) {
    తిరిగి ut->pFuncs->native_నిడివి(ut);
}


U_CAPI UBool U_EXPORT2
utext_is_నిడివి_Expensive(స్థిరరాశి UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE)) != 0;
    తిరిగి r;
}


U_CAPI int64_t U_EXPORT2
utext_get_Native_సూచీ(స్థిరరాశి UText *ut) {
    ఒకవేళ(ut->chunk_స్థానభ్రంశం <= ut->native_Indexing_హద్దు) {
        తిరిగి ut->chunk_Native_ప్రారంభం+ut->chunk_స్థానభ్రంశం;
    } లేకపోతే {
        తిరిగి ut->pFuncs->map_స్థానభ్రంశం_గమ్యం_Native(ut);
    }
}


U_CAPI శూన్య U_EXPORT2
utext_సమితి_Native_సూచీ(UText *ut, int64_t సూచీ) {
    ఒకవేళ(సూచీ<ut->chunk_Native_ప్రారంభం || సూచీ>=ut->chunk_Native_హద్దు) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, సూచీ, నిజం);
    } లేకపోతే ఒకవేళ((int32_t)(సూచీ - ut->chunk_Native_ప్రారంభం) <= ut->native_Indexing_హద్దు) {
        // utf-16 indexing.
        ut->chunk_స్థానభ్రంశం=(int32_t)(సూచీ-ut->chunk_Native_ప్రారంభం);
    } లేకపోతే {
         ut->chunk_స్థానభ్రంశం=ut->pFuncs->map_Native_సూచీ_గమ్యం_UTF16(ut, సూచీ);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    ఒకవేళ (ut->chunk_స్థానభ్రంశం<ut->chunk_నిడివి) {
        char16_t c= ut->chunkContents[ut->chunk_స్థానభ్రంశం];
        ఒకవేళ (U16_IS_TRAIL(c)) {
            ఒకవేళ (ut->chunk_స్థానభ్రంశం==0) {
                ut->pFuncs->access(ut, ut->chunk_Native_ప్రారంభం, అబద్ధం);
            }
            ఒకవేళ (ut->chunk_స్థానభ్రంశం>0) {
                char16_t lead = ut->chunkContents[ut->chunk_స్థానభ్రంశం-1];
                ఒకవేళ (U16_IS_LEAD(lead)) {
                    ut->chunk_స్థానభ్రంశం--;
                }
            }
        }
    }
}



U_CAPI int64_t U_EXPORT2
utext_get_మునుపటి_Native_సూచీ(UText *ut) {
    //
    //  Fast-path the common case.
    //     Common means current position is not at the beginning of a chunk
    //     and the preceding character is not supplementary.
    //
    int32_t i = ut->chunk_స్థానభ్రంశం - 1;
    int64_t ఫలితం;
    ఒకవేళ (i >= 0) {
        char16_t c = ut->chunkContents[i];
        ఒకవేళ (U16_IS_TRAIL(c) == అబద్ధం) {
            ఒకవేళ (i <= ut->native_Indexing_హద్దు) {
                ఫలితం = ut->chunk_Native_ప్రారంభం + i;
            } లేకపోతే {
                ut->chunk_స్థానభ్రంశం = i;
                ఫలితం = ut->pFuncs->map_స్థానభ్రంశం_గమ్యం_Native(ut);
                ut->chunk_స్థానభ్రంశం++;
            }
            తిరిగి ఫలితం;
        }
    }

    // If at the start of text, simply return 0.
    ఒకవేళ (ut->chunk_స్థానభ్రంశం==0 && ut->chunk_Native_ప్రారంభం==0) {
        తిరిగి 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    ఫలితం = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    తిరిగి ఫలితం;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    ఒకవేళ (ut->chunk_స్థానభ్రంశం==ut->chunk_నిడివి) {
        // Current position is just off the end of the chunk.
        ఒకవేళ (ut->pFuncs->access(ut, ut->chunk_Native_హద్దు, నిజం) == అబద్ధం) {
            // Off the end of the text.
            తిరిగి U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_స్థానభ్రంశం];
    ఒకవేళ (U16_IS_LEAD(c) == అబద్ధం) {
        // Normal, non-supplementary case.
        తిరిగి c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    ఒకవేళ ((ut->chunk_స్థానభ్రంశం+1) < ut->chunk_నిడివి) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunk_స్థానభ్రంశం+1];
    } లేకపోతే {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunk_Native_హద్దు;
        ఒకవేళ (ut->pFuncs->access(ut, nativePosition, నిజం)) {
            trail = ut->chunkContents[ut->chunk_స్థానభ్రంశం];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, అబద్ధం);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunk_స్థానభ్రంశం = ut->chunk_నిడివి - 1;
        ఒకవేళ(!r) {
            తిరిగి U_SENTINEL;
        }
    }

    ఒకవేళ (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    తిరిగి supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t native_సూచీ) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    ఒకవేళ (native_సూచీ>=ut->chunk_Native_ప్రారంభం && native_సూచీ < ut->chunk_Native_ప్రారంభం + ut->native_Indexing_హద్దు) {
        ut->chunk_స్థానభ్రంశం = (int32_t)(native_సూచీ - ut->chunk_Native_ప్రారంభం);
        c = ut->chunkContents[ut->chunk_స్థానభ్రంశం];
        ఒకవేళ (U16_IS_SURROGATE(c) == అబద్ధం) {
            తిరిగి c;
        }
    }


    utext_సమితి_Native_సూచీ(ut, native_సూచీ);
    ఒకవేళ (native_సూచీ>=ut->chunk_Native_ప్రారంభం && ut->chunk_స్థానభ్రంశం<ut->chunk_నిడివి) {
        c = ut->chunkContents[ut->chunk_స్థానభ్రంశం];
        ఒకవేళ (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    తిరిగి c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    ఒకవేళ (ut->chunk_స్థానభ్రంశం >= ut->chunk_నిడివి) {
        ఒకవేళ (ut->pFuncs->access(ut, ut->chunk_Native_హద్దు, నిజం) == అబద్ధం) {
            తిరిగి U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_స్థానభ్రంశం++];
    ఒకవేళ (U16_IS_LEAD(c) == అబద్ధం) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        తిరిగి c;
    }

    ఒకవేళ (ut->chunk_స్థానభ్రంశం >= ut->chunk_నిడివి) {
        ఒకవేళ (ut->pFuncs->access(ut, ut->chunk_Native_హద్దు, నిజం) == అబద్ధం) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            తిరిగి c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunk_స్థానభ్రంశం];
    ఒకవేళ (U16_IS_TRAIL(trail) == అబద్ధం) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        తిరిగి c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunk_స్థానభ్రంశం++;   // move iteration position over the trail surrogate.
    తిరిగి supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    ఒకవేళ (ut->chunk_స్థానభ్రంశం <= 0) {
        ఒకవేళ (ut->pFuncs->access(ut, ut->chunk_Native_ప్రారంభం, అబద్ధం) == అబద్ధం) {
            తిరిగి U_SENTINEL;
        }
    }
    ut->chunk_స్థానభ్రంశం--;
    c = ut->chunkContents[ut->chunk_స్థానభ్రంశం];
    ఒకవేళ (U16_IS_TRAIL(c) == అబద్ధం) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        తిరిగి c;
    }

    ఒకవేళ (ut->chunk_స్థానభ్రంశం <= 0) {
        ఒకవేళ (ut->pFuncs->access(ut, ut->chunk_Native_ప్రారంభం, అబద్ధం) == అబద్ధం) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            తిరిగి c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunk_స్థానభ్రంశం-1];
    ఒకవేళ (U16_IS_LEAD(lead) == అబద్ధం) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        తిరిగి c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunk_స్థానభ్రంశం--;   // move iteration position over the lead surrogate.
    తిరిగి supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t సూచీ) {
    UChar32       c      = U_SENTINEL;

    ఒకవేళ(సూచీ<ut->chunk_Native_ప్రారంభం || సూచీ>=ut->chunk_Native_హద్దు) {
        // Desired position is outside of the current chunk.
        ఒకవేళ(!ut->pFuncs->access(ut, సూచీ, నిజం)) {
            // no chunk available here
            తిరిగి U_SENTINEL;
        }
    } లేకపోతే ఒకవేళ (సూచీ - ut->chunk_Native_ప్రారంభం  <= (int64_t)ut->native_Indexing_హద్దు) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunk_స్థానభ్రంశం = (int32_t)(సూచీ - ut->chunk_Native_ప్రారంభం);
    } లేకపోతే {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunk_స్థానభ్రంశం = ut->pFuncs->map_Native_సూచీ_గమ్యం_UTF16(ut, సూచీ);
    }

    c = ut->chunkContents[ut->chunk_స్థానభ్రంశం++];
    ఒకవేళ (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_సమితి_Native_సూచీ(ut, సూచీ);
        c = utext_next32(ut);
    }
    తిరిగి c;
}


U_CAPI UChar32 U_EXPORT2
utext_previous32From(UText *ut, int64_t సూచీ) {
    //
    //  Return the character preceding the specified index.
    //  Leave the iteration position at the start of the character that was returned.
    //
    UChar32     cPrev;    // The character preceding cCurr, which is what we will return.

    // Address the chunk containing the position preceding the incoming index
    // A tricky edge case:
    //   We try to test the requested native index against the chunkNativeStart to determine
    //    whether the character preceding the one at the index is in the current chunk.
    //    BUT, this test can fail with UTF-8 (or any other multibyte encoding), when the
    //    requested index is on something other than the first position of the first char.
    //
    ఒకవేళ(సూచీ<=ut->chunk_Native_ప్రారంభం || సూచీ>ut->chunk_Native_హద్దు) {
        // Requested native index is outside of the current chunk.
        ఒకవేళ(!ut->pFuncs->access(ut, సూచీ, అబద్ధం)) {
            // no chunk available here
            తిరిగి U_SENTINEL;
        }
    } లేకపోతే ఒకవేళ(సూచీ - ut->chunk_Native_ప్రారంభం <= (int64_t)ut->native_Indexing_హద్దు) {
        // Direct UTF-16 indexing.
        ut->chunk_స్థానభ్రంశం = (int32_t)(సూచీ - ut->chunk_Native_ప్రారంభం);
    } లేకపోతే {
        ut->chunk_స్థానభ్రంశం=ut->pFuncs->map_Native_సూచీ_గమ్యం_UTF16(ut, సూచీ);
        ఒకవేళ (ut->chunk_స్థానభ్రంశం==0 && !ut->pFuncs->access(ut, సూచీ, అబద్ధం)) {
            // no chunk available here
            తిరిగి U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunk_స్థానభ్రంశం--;
    cPrev = ut->chunkContents[ut->chunk_స్థానభ్రంశం];

    ఒకవేళ (U16_IS_SURROGATE(cPrev)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_సమితి_Native_సూచీ(ut, సూచీ);
        cPrev = utext_previous32(ut);
    }
    తిరిగి cPrev;
}


U_CAPI int32_t U_EXPORT2
utext_వెలికితీయి(UText *ut,
             int64_t ప్రారంభం, int64_t హద్దు,
             char16_t *dest, int32_t dest_సామర్థ్యం,
             UError_సంకేతం *స్థితి_2) {
                 తిరిగి ut->pFuncs->extract(ut, ప్రారంభం, హద్దు, dest, dest_సామర్థ్యం, స్థితి_2);
             }



U_CAPI UBool U_EXPORT2
utext_equals(స్థిరరాశి UText *a, స్థిరరాశి UText *b) {
    ఒకవేళ (a==శూన్యసూచిక || b==శూన్యసూచిక ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            తిరిగి అబద్ధం;
    }

    ఒకవేళ (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        తిరిగి అబద్ధం;
    }

    ఒకవేళ (a->context != b->context) {
        // Different sources (different strings)
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (utext_get_Native_సూచీ(a) != utext_get_Native_సూచీ(b)) {
        // Different current position in the string.
        తిరిగి అబద్ధం;
    }

    తిరిగి నిజం;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(స్థిరరాశి UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    తిరిగి b;
}


U_CAPI శూన్య U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_has_Meta_దత్తాంశం(స్థిరరాశి UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_దత్తాంశం)) != 0;
    తిరిగి b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t native_ప్రారంభం, int64_t native_హద్దు,
             స్థిరరాశి char16_t *replacement_పాఠ్యం, int32_t replacement_నిడివి,
             UError_సంకేతం *స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *స్థితి_2 = U_NO_WRITE_PERMISSION;
        తిరిగి 0;
    }
    int32_t i = ut->pFuncs->replace(ut, native_ప్రారంభం, native_హద్దు, replacement_పాఠ్యం, replacement_నిడివి, స్థితి_2);
    తిరిగి i;
}

U_CAPI శూన్య U_EXPORT2
utext_copy(UText *ut,
          int64_t native_ప్రారంభం, int64_t native_హద్దు,
          int64_t dest_సూచీ,
          UBool move,
          UError_సంకేతం *స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *స్థితి_2 = U_NO_WRITE_PERMISSION;
        తిరిగి;
    }
    ut->pFuncs->copy(ut, native_ప్రారంభం, native_హద్దు, dest_సూచీ, move, స్థితి_2);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            స్థిరరాశి UText* src,
            UBool deep,
            UBool readOnly,
            UError_సంకేతం* స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి dest;
    }
    UText *ఫలితం = src->pFuncs->clone(dest, src, deep, స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి ఫలితం;
    }
    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి ఫలితం;
    }
    ఒకవేళ (readOnly) {
        utext_freeze(ఫలితం);
    }
    తిరిగి ఫలితం;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
గణన {
    UTEXT_కుప్ప_ALLOCATED  = 1,      //  1 if ICU has allocated this UText struct on the heap.
                                    //  0 if caller provided storage for the UText.

    UTEXT_EXTRA_కుప్ప_ALLOCATED = 2, //  1 if ICU has allocated extra storage as a separate
                                    //     heap block.
                                    //  0 if there is no separate allocation.  Either no extra
                                    //     storage was requested, or it is appended to the end
                                    //     of the main UText storage.

    UTEXT_OPEN = 4                  //  1 if this UText is currently open
                                    //  0 if this UText is not open.
};


//
//  Extended form of a UText.  The purpose is to aid in computing the total size required
//    when a provider asks for a UText to be allocated with extra storage.

నిర్మాణం ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

స్థిర స్థిరరాశి UText ఖాళీలు_పాఠ్యం = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_అమరిక(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UError_సంకేతం* స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి ut;
    }

    ఒకవేళ (ut == శూన్యసూచిక) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = పరిమాణం(UText);
        ఒకవేళ (extraSpace > 0) {
            spaceRequired = పరిమాణం(ExtendedUText) + extraSpace - పరిమాణం(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        ఒకవేళ (ut == శూన్యసూచిక) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        } లేకపోతే {
            *ut = ఖాళీలు_పాఠ్యం;
            ut->flags |= UTEXT_కుప్ప_ALLOCATED;
            ఒకవేళ (spaceRequired>0) {
                ut->extra_కొలత = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } లేకపోతే {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        ఒకవేళ (ut->magic != UTEXT_MAGIC) {
            *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        ఒకవేళ ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != శూన్యసూచిక)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        ఒకవేళ (extraSpace > ut->extra_కొలత) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            ఒకవేళ (ut->flags & UTEXT_EXTRA_కుప్ప_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extra_కొలత = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            ఒకవేళ (ut->pExtra == శూన్యసూచిక) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            } లేకపోతే {
                ut->extra_కొలత = extraSpace;
                ut->flags |= UTEXT_EXTRA_కుప్ప_ALLOCATED;
            }
        }
    }
    ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
        ut->flags |= UTEXT_OPEN;

        // Initialize all remaining fields of the UText.
        //
        ut->context             = శూన్యసూచిక;
        ut->chunkContents       = శూన్యసూచిక;
        ut->p                   = శూన్యసూచిక;
        ut->q                   = శూన్యసూచిక;
        ut->r                   = శూన్యసూచిక;
        ut->a                   = 0;
        ut->b                   = 0;
        ut->c                   = 0;
        ut->chunk_స్థానభ్రంశం         = 0;
        ut->chunk_నిడివి         = 0;
        ut->chunk_Native_ప్రారంభం    = 0;
        ut->chunk_Native_హద్దు    = 0;
        ut->native_Indexing_హద్దు = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = శూన్యసూచిక;
        ఒకవేళ (ut->pExtra!=శూన్యసూచిక && ut->extra_కొలత>0)
            uprv_memset(ut->pExtra, 0, ut->extra_కొలత);

    }
    తిరిగి ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    ఒకవేళ (ut==శూన్యసూచిక ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        తిరిగి ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    ఒకవేళ (ut->pFuncs->close != శూన్యసూచిక) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    ఒకవేళ (ut->flags & UTEXT_EXTRA_కుప్ప_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = శూన్యసూచిక;
        ut->flags &= ~UTEXT_EXTRA_కుప్ప_ALLOCATED;
        ut->extra_కొలత = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = శూన్యసూచిక;

    ఒకవేళ (ut->flags & UTEXT_కుప్ప_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = శూన్యసూచిక;
    }
    తిరిగి ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
స్థిర శూన్య
invalidateChunk(UText *ut) {
    ut->chunk_నిడివి = 0;
    ut->chunk_Native_హద్దు = 0;
    ut->chunk_Native_ప్రారంభం = 0;
    ut->chunk_స్థానభ్రంశం = 0;
    ut->native_Indexing_హద్దు = 0;
}

//
// pinIndex        Do range pinning on a native index parameter.
//                 64 bit pinning is done in place.
//                 32 bit truncated result is returned as a convenience for
//                        use in providers that don't need 64 bits.
స్థిర int32_t
pin_సూచీ(int64_t &సూచీ, int64_t హద్దు) {
    ఒకవేళ (సూచీ<0) {
        సూచీ = 0;
    } లేకపోతే ఒకవేళ (సూచీ > హద్దు) {
        సూచీ = హద్దు;
    }
    తిరిగి static_cast<int32_t>(సూచీ);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
స్థిర శూన్య adjust_సూచిక(UText *dest, స్థిరరాశి శూన్య **destPtr, స్థిరరాశి UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    అక్షరం  *dptr = (అక్షరం *)*destPtr;
    అక్షరం  *dUText = (అక్షరం *)dest;
    అక్షరం  *sUText = (అక్షరం *)src;

    ఒకవేళ (dptr >= (అక్షరం *)src->pExtra && dptr < ((అక్షరం*)src->pExtra)+src->extra_కొలత) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((అక్షరం *)dest->pExtra) + (dptr - (అక్షరం *)src->pExtra);
    } లేకపోతే ఒకవేళ (dptr>=sUText && dptr < sUText+src->కొలత_యొక్క_Struct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
స్థిర UText * U_CALLCONV
shallow_పాఠ్యం_Clone(UText * dest, స్థిరరాశి UText * src, UError_సంకేతం * స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    int32_t  src_Extra_కొలత = src->extra_కొలత;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_అమరిక(dest, src_Extra_కొలత, స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    శూన్య *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    పూర్ణాంకం కొలత_గమ్యం_Copy = src->కొలత_యొక్క_Struct;
    ఒకవేళ (కొలత_గమ్యం_Copy > dest->కొలత_యొక్క_Struct) {
        కొలత_గమ్యం_Copy = dest->కొలత_యొక్క_Struct;
    }
    uprv_memcpy(dest, src, కొలత_గమ్యం_Copy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    ఒకవేళ (src_Extra_కొలత > 0) {
        uprv_memcpy(dest->pExtra, src->pExtra, src_Extra_కొలత);
    }

    //
    // Relocate any pointers in the target that refer to the UText itself
    //   to point to the cloned copy rather than the original source.
    //
    adjust_సూచిక(dest, &dest->context, src);
    adjust_సూచిక(dest, &dest->p, src);
    adjust_సూచిక(dest, &dest->q, src);
    adjust_సూచిక(dest, &dest->r, src);
    adjust_సూచిక(dest, (స్థిరరాశి శూన్య **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం);

    తిరిగి dest;
}


U_CDECL_చివర



//------------------------------------------------------------------------------
//
//     UText implementation for UTF-8 char * strings (read-only)
//     Limitation:  string length must be <= 0x7fffffff in length.
//                  (length must for in an int32_t variable)
//
//         Use of UText data members:
//              context    pointer to UTF-8 string
//              utext.b    is the input string length (bytes).
//              utext.c    Length scanned so far in string
//                           (for optimizing finding length of zero terminated strings.)
//              utext.p    pointer to the current buffer
//              utext.q    pointer to the other buffer.
//
//------------------------------------------------------------------------------

// Chunk size.
//     Must be less than 85 (256/3), because of byte mapping from char16_t indexes to native indexes.
//     Worst case is three native bytes to one char16_t.  (Supplemenaries are 4 native bytes
//     to two UChars.)
//     The longest illegal byte sequence treated as a single error (and converted to U+FFFD)
//     is a three-byte sequence (truncated four-byte sequence).
//
గణన { UTF8_పాఠ్యం_CHUNK_కొలత=32 };

//
// UTF8Buf  Two of these structs will be set up in the UText's extra allocated space.
//          Each contains the char16_t chunk buffer, the to and from native maps, and
//          header info.
//
//     because backwards iteration fills the buffers starting at the end and
//     working towards the front, the filled part of the buffers may not begin
//     at the start of the available storage for the buffers.
//
//     Buffer size is one bigger than the specified UTF8_TEXT_CHUNK_SIZE to allow for
//     the last character added being a supplementary, and thus requiring a surrogate
//     pair.  Doing this is simpler than checking for the edge case.
//

నిర్మాణం UTF8Buf {
    int32_t   buf_Native_ప్రారంభం;                        // Native index of first char in char16_t buf
    int32_t   buf_Native_హద్దు;                        // Native index following last char in buf.
    int32_t   buf_ప్రారంభం_Idx;                           // First filled position in buf.
    int32_t   buf_హద్దు_Idx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   గమ్యం_UChars_Map_ప్రారంభం;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_పాఠ్యం_CHUNK_కొలత+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   map_గమ్యం_Native[UTF8_పాఠ్యం_CHUNK_కొలత+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   map_గమ్యం_UChars[UTF8_పాఠ్యం_CHUNK_కొలత*3+6]; // Map native offset from bufNativeStart to
                                                     //   corresponding offset in filled part of buf.
    int32_t   align;
};

U_CDECL_BEGIN

//
//   utf8TextLength
//
//        Get the length of the string.  If we don't already know it,
//              we'll need to scan for the trailing  nul.
//
స్థిర int64_t U_CALLCONV
utf8_పాఠ్యం_నిడివి(UText *ut) {
    ఒకవేళ (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        స్థిరరాశి అక్షరం *r = (స్థిరరాశి అక్షరం *)ut->context + ut->c;
        అయితే (*r != 0) {
            r++;
        }
        ఒకవేళ ((r - (స్థిరరాశి అక్షరం *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (స్థిరరాశి అక్షరం *)ut->context);
        } లేకపోతే {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE);
    }
    తిరిగి ut->b;
}






స్థిర UBool U_CALLCONV
utf8_పాఠ్యం_Access(UText *ut, int64_t సూచీ, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    స్థిరరాశి uint8_t *s8=(స్థిరరాశి uint8_t *)ut->context;
    UTF8Buf *u8b = శూన్యసూచిక;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)సూచీ;     // Requested index, trimmed to 32 bits.
    int32_t  map_సూచీ = 0;
    ఒకవేళ (సూచీ<0) {
        ix=0;
    } లేకపోతే ఒకవేళ (సూచీ > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    ఒకవేళ (ix>length) {
        ఒకవేళ (length>=0) {
            ix=length;
        } లేకపోతే ఒకవేళ (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            అయితే (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            ఒకవేళ (s8[ut->c] == 0) {
                // We just found the actual length of the string.
                //  Trim the requested index back to that.
                ix     = ut->c;
                ut->b  = ut->c;
                length = ut->c;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    ఒకవేళ (forward) {
        ఒకవేళ (ix==ut->chunk_Native_హద్దు) {
            // Check for normal sequential iteration cases first.
            ఒకవేళ (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunk_స్థానభ్రంశం = ut->chunk_నిడివి;
                తిరిగి అబద్ధం;
            } లేకపోతే {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                ఒకవేళ (ix>=altB->buf_Native_ప్రారంభం && ix<altB->buf_Native_హద్దు) {
                    వెళ్ళు మార్పిడి_Buffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            ఒకవేళ (ix>=u8b->buf_Native_ప్రారంభం && ix<u8b->buf_Native_హద్దు) {
                // Requested index is in the other buffer.
                వెళ్ళు మార్పిడి_Buffers;
            }
            ఒకవేళ (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                ఒకవేళ (ix == ut->chunk_Native_హద్దు) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunk_స్థానభ్రంశం = ut->chunk_నిడివి;
                    తిరిగి అబద్ధం;
                }
                ఒకవేళ (ix == u8b->buf_Native_హద్దు) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    వెళ్ళు మార్పిడి_Buffers_And_Fail;
                }

                // Neither existing buffer extends to the end of the string.
                వెళ్ళు make_Stub_బఫరు;
            }

            ఒకవేళ (ix<ut->chunk_Native_ప్రారంభం || ix>=ut->chunk_Native_హద్దు) {
                // Requested index is in neither buffer.
                వెళ్ళు fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            map_సూచీ = ix - u8b->గమ్యం_UChars_Map_ప్రారంభం;
            U_ASSERT(map_సూచీ < (int32_t)పరిమాణం(UTF8Buf::map_గమ్యం_UChars));
            ut->chunk_స్థానభ్రంశం = u8b->map_గమ్యం_UChars[map_సూచీ] - u8b->buf_ప్రారంభం_Idx;
            తిరిగి నిజం;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    ఒకవేళ (ix==ut->chunk_Native_ప్రారంభం) {
        // Check for normal sequential iteration cases first.
        ఒకవేళ (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunk_స్థానభ్రంశం = 0;
            తిరిగి అబద్ధం;
        } లేకపోతే {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            ఒకవేళ (ix>altB->buf_Native_ప్రారంభం && ix<=altB->buf_Native_హద్దు) {
                వెళ్ళు మార్పిడి_Buffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    ఒకవేళ (ix>u8b->buf_Native_ప్రారంభం && ix<=u8b->buf_Native_హద్దు) {
        // Requested index is in the other buffer.
        వెళ్ళు మార్పిడి_Buffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    ఒకవేళ (ix==0) {
        ఒకవేళ (u8b->buf_Native_ప్రారంభం==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            వెళ్ళు మార్పిడి_Buffers_And_Fail;
        } లేకపోతే {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            వెళ్ళు make_Stub_బఫరు;
        }
    }

    ఒకవేళ (ix<=ut->chunk_Native_ప్రారంభం || ix>ut->chunk_Native_హద్దు) {
        // Requested index is in neither buffer.
        వెళ్ళు fill_తిరగేయి;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    map_సూచీ = ix - u8b->గమ్యం_UChars_Map_ప్రారంభం;
    ut->chunk_స్థానభ్రంశం = u8b->map_గమ్యం_UChars[map_సూచీ] - u8b->buf_ప్రారంభం_Idx;
    ఒకవేళ (ut->chunk_స్థానభ్రంశం==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        తిరిగి అబద్ధం;
    } లేకపోతే {
        తిరిగి నిజం;
    }



మార్పిడి_Buffers:
    //  The alternate buffer (ut->q) has the string data that was requested.
    //  Swap the primary and alternate buffers, and set the
    //   chunk index into the new primary buffer.
    {
        u8b   = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b;
        ut->chunkContents       = &u8b->buf[u8b->buf_ప్రారంభం_Idx];
        ut->chunk_నిడివి         = u8b->buf_హద్దు_Idx - u8b->buf_ప్రారంభం_Idx;
        ut->chunk_Native_ప్రారంభం    = u8b->buf_Native_ప్రారంభం;
        ut->chunk_Native_హద్దు    = u8b->buf_Native_హద్దు;
        ut->native_Indexing_హద్దు = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->buf_Native_ప్రారంభం);
        U_ASSERT(ix<=u8b->buf_Native_హద్దు);
        map_సూచీ = ix - u8b->గమ్యం_UChars_Map_ప్రారంభం;
        U_ASSERT(map_సూచీ>=0);
        U_ASSERT(map_సూచీ<(int32_t)పరిమాణం(u8b->map_గమ్యం_UChars));
        ut->chunk_స్థానభ్రంశం = u8b->map_గమ్యం_UChars[map_సూచీ] - u8b->buf_ప్రారంభం_Idx;

        తిరిగి నిజం;
    }


 మార్పిడి_Buffers_And_Fail:
    // We got a request for either the start or end of the string,
    //  with iteration continuing in the out-of-bounds direction.
    // The alternate buffer already contains the data up to the
    //  start/end.
    // Swap the buffers, then return failure, indicating that we couldn't
    //  make things correct for continuing the iteration in the requested
    //  direction.  The position & buffer are correct should the
    //  user decide to iterate in the opposite direction.
    u8b   = (UTF8Buf *)ut->q;
    ut->q = ut->p;
    ut->p = u8b;
    ut->chunkContents       = &u8b->buf[u8b->buf_ప్రారంభం_Idx];
    ut->chunk_నిడివి         = u8b->buf_హద్దు_Idx - u8b->buf_ప్రారంభం_Idx;
    ut->chunk_Native_ప్రారంభం    = u8b->buf_Native_ప్రారంభం;
    ut->chunk_Native_హద్దు    = u8b->buf_Native_హద్దు;
    ut->native_Indexing_హద్దు = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    ఒకవేళ (ix==u8b->buf_Native_హద్దు) {
        ut->chunk_స్థానభ్రంశం = ut->chunk_నిడివి;
    } లేకపోతే  {
        ut->chunk_స్థానభ్రంశం = 0;
        U_ASSERT(ix == u8b->buf_Native_ప్రారంభం);
    }
    తిరిగి అబద్ధం;

make_Stub_బఫరు:
    //   The user has done a seek/access past the start or end
    //   of the string.  Rather than loading data that is likely
    //   to never be used, just set up a zero-length buffer at
    //   the position.
    u8b = (UTF8Buf *)ut->q;
    u8b->buf_Native_ప్రారంభం   = ix;
    u8b->buf_Native_హద్దు   = ix;
    u8b->buf_ప్రారంభం_Idx      = 0;
    u8b->buf_హద్దు_Idx      = 0;
    u8b->bufNILimit       = 0;
    u8b->గమ్యం_UChars_Map_ప్రారంభం = ix;
    u8b->map_గమ్యం_Native[0]   = 0;
    u8b->map_గమ్యం_UChars[0]   = 0;
    వెళ్ళు మార్పిడి_Buffers_And_Fail;



fillForward:
    {
        // Move the incoming index to a code point boundary.
        U8_సమితి_CP_ప్రారంభం(s8, 0, ix);

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_మార్పిడి = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_మార్పిడి;

        int32_t strLen = ut->b;
        UBool   nulTerminated = అబద్ధం;
        ఒకవేళ (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = నిజం;
        }

        char16_t   *buf = u8b_మార్పిడి->buf;
        uint8_t *map_గమ్యం_Native  = u8b_మార్పిడి->map_గమ్యం_Native;
        uint8_t *map_గమ్యం_UChars  = u8b_మార్పిడి->map_గమ్యం_UChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    చూసిన_Non_Ascii = అబద్ధం;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        అయితే (destIx<UTF8_పాఠ్యం_CHUNK_కొలత) {
            c = s8[srcIx];
            ఒకవేళ (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                map_గమ్యం_Native[destIx]    = (uint8_t)(srcIx - ix);
                map_గమ్యం_UChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } లేకపోతే {
                // General case, handle everything.
                ఒకవేళ (చూసిన_Non_Ascii == అబద్ధం) {
                    చూసిన_Non_Ascii = నిజం;
                    u8b_మార్పిడి->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_తదుపరి_OR_FFFD(s8, srcIx, strLen, c);
                ఒకవేళ (c==0 && nulTerminated) {
                    srcIx--;
                    ఆపు;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                చేయి {
                    map_గమ్యం_Native[dIx++] = (uint8_t)(cIx - ix);
                } అయితే (dIx < destIx);

                చేయి {
                    map_గమ్యం_UChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } అయితే (cIx < srcIx);
            }
            ఒకవేళ (srcIx>=strLen) {
                ఆపు;
            }

        }

        //  store Native <--> Chunk Map entries for the end of the buffer.
        //    There is no actual character here, but the index position is valid.
        map_గమ్యం_Native[destIx]     = (uint8_t)(srcIx - ix);
        map_గమ్యం_UChars[srcIx - ix] = (uint8_t)destIx;

        //  fill in Buffer descriptor
        u8b_మార్పిడి->buf_Native_ప్రారంభం     = ix;
        u8b_మార్పిడి->buf_Native_హద్దు     = srcIx;
        u8b_మార్పిడి->buf_ప్రారంభం_Idx        = 0;
        u8b_మార్పిడి->buf_హద్దు_Idx        = destIx;
        ఒకవేళ (చూసిన_Non_Ascii == అబద్ధం) {
            u8b_మార్పిడి->bufNILimit     = destIx;
        }
        u8b_మార్పిడి->గమ్యం_UChars_Map_ప్రారంభం   = u8b_మార్పిడి->buf_Native_ప్రారంభం;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunk_స్థానభ్రంశం         = 0;
        ut->chunk_నిడివి         = u8b_మార్పిడి->buf_హద్దు_Idx;
        ut->chunk_Native_ప్రారంభం    = u8b_మార్పిడి->buf_Native_ప్రారంభం;
        ut->chunk_Native_హద్దు    = u8b_మార్పిడి->buf_Native_హద్దు;
        ut->native_Indexing_హద్దు = u8b_మార్పిడి->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        ఒకవేళ (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            ఒకవేళ (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE);
            }
        }
        తిరిగి నిజం;
    }


fill_తిరగేయి:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        ఒకవేళ (ix != ut->b) {
            // Note: this function will only move the index back if it is on a trail byte
            //       and there is a preceding lead byte and the sequence from the lead 
            //       through this trail could be part of a valid UTF-8 sequence
            //       Otherwise the index remains unchanged.
            U8_సమితి_CP_ప్రారంభం(s8, 0, ix);
        }

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_మార్పిడి = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_మార్పిడి;

        char16_t   *buf = u8b_మార్పిడి->buf;
        uint8_t *map_గమ్యం_Native = u8b_మార్పిడి->map_గమ్యం_Native;
        uint8_t *map_గమ్యం_UChars = u8b_మార్పిడి->map_గమ్యం_UChars;
        int32_t  గమ్యం_UChars_Map_ప్రారంభం = ix - పరిమాణం(UTF8Buf::map_గమ్యం_UChars) + 1;
        // Note that toUCharsMapStart can be negative. Happens when the remaining
        // text from current position to the beginning is less than the buffer size.
        // + 1 because mapToUChars must have a slot at the end for the bufNativeLimit entry.
        int32_t  destIx = UTF8_పాఠ్యం_CHUNK_కొలత+2;   // Start in the overflow region
                                                    //   at end of buffer to leave room
                                                    //   for a surrogate pair at the
                                                    //   buffer start.
        int32_t  srcIx  = ix;
        int32_t  bufNILimit = destIx;
        UChar32   c;

        // Map to/from Native Indexes, fill in for the position at the end of
        //   the buffer.
        //
        map_గమ్యం_Native[destIx] = (uint8_t)(srcIx - గమ్యం_UChars_Map_ప్రారంభం);
        map_గమ్యం_UChars[srcIx - గమ్యం_UChars_Map_ప్రారంభం] = (uint8_t)destIx;

        // Fill the chunk buffer
        // Work backwards, filling from the end of the buffer towards the front.
        //
        అయితే (destIx>2 && (srcIx - గమ్యం_UChars_Map_ప్రారంభం > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            ఒకవేళ (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(గమ్యం_UChars_Map_ప్రారంభం <= srcIx);
                map_గమ్యం_UChars[srcIx - గమ్యం_UChars_Map_ప్రారంభం] = (uint8_t)destIx;
                map_గమ్యం_Native[destIx] = (uint8_t)(srcIx - గమ్యం_UChars_Map_ప్రారంభం);
            } లేకపోతే {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_prev_Char_సురక్షిత_Body(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                ఒకవేళ (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    map_గమ్యం_Native[destIx] = (uint8_t)(srcIx - గమ్యం_UChars_Map_ప్రారంభం);
                } లేకపోతే {
                    buf[destIx]         = U16_TRAIL(c);
                    map_గమ్యం_Native[destIx] = (uint8_t)(srcIx - గమ్యం_UChars_Map_ప్రారంభం);
                    buf[--destIx]       = U16_LEAD(c);
                    map_గమ్యం_Native[destIx] = (uint8_t)(srcIx - గమ్యం_UChars_Map_ప్రారంభం);
                }

                // Fill in the map from native indexes to UChars buf index.
                చేయి {
                    map_గమ్యం_UChars[sIx-- - గమ్యం_UChars_Map_ప్రారంభం] = (uint8_t)destIx;
                } అయితే (sIx >= srcIx);
                U_ASSERT(గమ్యం_UChars_Map_ప్రారంభం <= (srcIx+1));

                // Set native indexing limit to be the current position.
                //   We are processing a non-ascii, non-native-indexing char now;
                //     the limit will be here if the rest of the chars to be
                //     added to this buffer are ascii.
                bufNILimit = destIx;
            }
        }
        u8b_మార్పిడి->buf_Native_ప్రారంభం     = srcIx;
        u8b_మార్పిడి->buf_Native_హద్దు     = ix;
        u8b_మార్పిడి->buf_ప్రారంభం_Idx        = destIx;
        u8b_మార్పిడి->buf_హద్దు_Idx        = UTF8_పాఠ్యం_CHUNK_కొలత+2;
        u8b_మార్పిడి->bufNILimit         = bufNILimit - u8b_మార్పిడి->buf_ప్రారంభం_Idx;
        u8b_మార్పిడి->గమ్యం_UChars_Map_ప్రారంభం   = గమ్యం_UChars_Map_ప్రారంభం;

        ut->chunkContents       = &buf[u8b_మార్పిడి->buf_ప్రారంభం_Idx];
        ut->chunk_నిడివి         = u8b_మార్పిడి->buf_హద్దు_Idx - u8b_మార్పిడి->buf_ప్రారంభం_Idx;
        ut->chunk_స్థానభ్రంశం         = ut->chunk_నిడివి;
        ut->chunk_Native_ప్రారంభం    = u8b_మార్పిడి->buf_Native_ప్రారంభం;
        ut->chunk_Native_హద్దు    = u8b_మార్పిడి->buf_Native_హద్దు;
        ut->native_Indexing_హద్దు = u8b_మార్పిడి->bufNILimit;
        తిరిగి నిజం;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
స్థిర char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t dest_సామర్థ్యం,
              int32_t *p_Dest_నిడివి,
              స్థిరరాశి అక్షరం* src,
              int32_t src_నిడివి,        // required.  NUL terminated not supported.
              UError_సంకేతం *p_Error_సంకేతం
              )
{

    char16_t *pDest = dest;
    char16_t *p_Dest_హద్దు = (dest!=శూన్యసూచిక)?(dest+dest_సామర్థ్యం):శూన్యసూచిక;
    UChar32 ch=0;
    int32_t సూచీ = 0;
    int32_t req_నిడివి = 0;
    uint8_t* pSrc = (uint8_t*) src;


    అయితే((సూచీ < src_నిడివి)&&(pDest<p_Dest_హద్దు)){
        ch = pSrc[సూచీ++];
        ఒకవేళ(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }లేకపోతే{
            ch=utf8_తదుపరి_Char_సురక్షిత_Body(pSrc, &సూచీ, src_నిడివి, ch, -3);
            ఒకవేళ(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }లేకపోతే{
                *(pDest++)=U16_LEAD(ch);
                ఒకవేళ(pDest<p_Dest_హద్దు){
                    *(pDest++)=U16_TRAIL(ch);
                }లేకపోతే{
                    req_నిడివి++;
                    ఆపు;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    అయితే(సూచీ < src_నిడివి){
        ch = pSrc[సూచీ++];
        ఒకవేళ(ch <= 0x7f){
            req_నిడివి++;
        }లేకపోతే{
            ch=utf8_తదుపరి_Char_సురక్షిత_Body(pSrc, &సూచీ, src_నిడివి, ch, -3);
            req_నిడివి+=U16_నిడివి(ch);
        }
    }

    req_నిడివి+=(int32_t)(pDest - dest);

    ఒకవేళ(p_Dest_నిడివి){
        *p_Dest_నిడివి = req_నిడివి;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_సామర్థ్యం,req_నిడివి,p_Error_సంకేతం);

    తిరిగి dest;
}



స్థిర int32_t U_CALLCONV
utf8_పాఠ్యం_వెలికితీయి(UText *ut,
                int64_t ప్రారంభం, int64_t హద్దు,
                char16_t *dest, int32_t dest_సామర్థ్యం,
                UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం>0)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pin_సూచీ(ప్రారంభం, length);
    int32_t  limit32 = pin_సూచీ(హద్దు, length);

    ఒకవేళ(start32>limit32) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    స్థిరరాశి uint8_t *buf = (స్థిరరాశి uint8_t *)ut->context;
    పూర్ణాంకం i;
    ఒకవేళ (start32 < ut->chunk_Native_హద్దు) {
        కొరకు (i=0; i<3; i++) {
            ఒకవేళ (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                ఆపు;
            }
            start32--;
        }
    }

    ఒకవేళ (limit32 < ut->chunk_Native_హద్దు) {
        కొరకు (i=0; i<3; i++) {
            ఒకవేళ (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                ఆపు;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t dest_నిడివి=0;
    utext_strFromUTF8(dest, dest_సామర్థ్యం, &dest_నిడివి,
                    (స్థిరరాశి అక్షరం *)ut->context+start32, limit32-start32,
                    p_Error_సంకేతం);
    utf8_పాఠ్యం_Access(ut, limit32, నిజం);
    తిరిగి dest_నిడివి;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
స్థిర int64_t U_CALLCONV
utf8_పాఠ్యం_Map_స్థానభ్రంశం_గమ్యం_Native(స్థిరరాశి UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunk_స్థానభ్రంశం>ut->native_Indexing_హద్దు && ut->chunk_స్థానభ్రంశం<=ut->chunk_నిడివి);
    int32_t native_స్థానభ్రంశం = u8b->map_గమ్యం_Native[ut->chunk_స్థానభ్రంశం + u8b->buf_ప్రారంభం_Idx] + u8b->గమ్యం_UChars_Map_ప్రారంభం;
    U_ASSERT(native_స్థానభ్రంశం >= ut->chunk_Native_ప్రారంభం && native_స్థానభ్రంశం <= ut->chunk_Native_హద్దు);
    తిరిగి native_స్థానభ్రంశం;
}

//
// Map a native index to the corresponding chunk offset
//
స్థిర int32_t U_CALLCONV
utf8_పాఠ్యం_Map_సూచీ_గమ్యం_UTF16(స్థిరరాశి UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t సూచీ = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(సూచీ>=ut->chunk_Native_ప్రారంభం+ut->native_Indexing_హద్దు);
    U_ASSERT(సూచీ<=ut->chunk_Native_హద్దు);
    int32_t map_సూచీ = సూచీ - u8b->గమ్యం_UChars_Map_ప్రారంభం;
    U_ASSERT(map_సూచీ < (int32_t)పరిమాణం(UTF8Buf::map_గమ్యం_UChars));
    int32_t స్థానభ్రంశం = u8b->map_గమ్యం_UChars[map_సూచీ] - u8b->buf_ప్రారంభం_Idx;
    U_ASSERT(స్థానభ్రంశం>=0 && స్థానభ్రంశం<=ut->chunk_నిడివి);
    తిరిగి స్థానభ్రంశం;
}

స్థిర UText * U_CALLCONV
utf8_పాఠ్యం_Clone(UText *dest, స్థిరరాశి UText *src, UBool deep, UError_సంకేతం *స్థితి_2)
{
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_పాఠ్యం_Clone(dest, src, స్థితి_2);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //
    // TODO:  There is an issue with using utext_nativeLength().
    //        That function is non-const in cases where the input was NUL terminated
    //          and the length has not yet been determined.
    //        This function (clone()) is const.
    //        There potentially a thread safety issue lurking here.
    //
    ఒకవేళ (deep && U_SUCCESS(*స్థితి_2)) {
        int32_t  len = (int32_t)utext_native_నిడివి((UText *)src);
        అక్షరం *copyStr = (అక్షరం *)uprv_malloc(len+1);
        ఒకవేళ (copyStr == శూన్యసూచిక) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం);
        }
    }
    తిరిగి dest;
}


స్థిర శూన్య U_CALLCONV
utf8_పాఠ్యం_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ఒకవేళ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం)) {
        అక్షరం *s = (అక్షరం *)ut->context;
        uprv_free(s);
        ut->context = శూన్యసూచిక;
    }
}

U_CDECL_చివర


స్థిర స్థిరరాశి నిర్మాణం UTextFuncs utf8Funcs =
{
    పరిమాణం(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8_పాఠ్యం_Clone,
    utf8_పాఠ్యం_నిడివి,
    utf8_పాఠ్యం_Access,
    utf8_పాఠ్యం_వెలికితీయి,
    శూన్యసూచిక,                /* replace*/
    శూన్యసూచిక,                /* copy   */
    utf8_పాఠ్యం_Map_స్థానభ్రంశం_గమ్యం_Native,
    utf8_పాఠ్యం_Map_సూచీ_గమ్యం_UTF16,
    utf8_పాఠ్యం_Close,
    శూన్యసూచిక,                // spare 1
    శూన్యసూచిక,                // spare 2
    శూన్యసూచిక                 // spare 3
};


స్థిర స్థిరరాశి అక్షరం g_ఖాళీలు_String[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               స్థిరరాశి అక్షరం* s U_LIFETIME_BOUND,
               int64_t length,
               UError_సంకేతం* స్థితి_2) {
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(s==శూన్యసూచిక && length==0) {
        s = g_ఖాళీలు_String;
    }

    ఒకవేళ(s==శూన్యసూచిక || length<-1 || length>INT32_MAX) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ut = utext_అమరిక(ut, పరిమాణం(UTF8Buf) * 2, స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    ఒకవేళ (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (అక్షరం *)ut->pExtra + పరిమాణం(UTF8Buf);
    తిరిగి ut;

}








//------------------------------------------------------------------------------
//
//     UText implementation wrapper for Replaceable (read/write)
//
//         Use of UText data members:
//            context    pointer to Replaceable.
//            p          pointer to Replaceable if it is owned by the UText.
//
//------------------------------------------------------------------------------



// minimum chunk size for this implementation: 3
// to allow for possible trimming for code point boundaries
గణన { REP_పాఠ్యం_CHUNK_కొలత=10 };

నిర్మాణం ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_పాఠ్యం_CHUNK_కొలత+1];
};


U_CDECL_BEGIN

స్థిర UText * U_CALLCONV
rep_పాఠ్యం_Clone(UText *dest, స్థిరరాశి UText *src, UBool deep, UError_సంకేతం *స్థితి_2) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_పాఠ్యం_Clone(dest, src, స్థితి_2);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    ఒకవేళ (deep && U_SUCCESS(*స్థితి_2)) {
        స్థిరరాశి Replaceable *replSrc = (స్థిరరాశి Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    తిరిగి dest;
}


స్థిర శూన్య U_CALLCONV
rep_పాఠ్యం_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ఒకవేళ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం)) {
        Replaceable *rep = (Replaceable *)ut->context;
        తొలగించు rep;
        ut->context = శూన్యసూచిక;
    }
}


స్థిర int64_t U_CALLCONV
rep_పాఠ్యం_నిడివి(UText *ut) {
    స్థిరరాశి Replaceable *replSrc = (స్థిరరాశి Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    తిరిగి len;
}


స్థిర UBool U_CALLCONV
rep_పాఠ్యం_Access(UText *ut, int64_t సూచీ, UBool forward) {
    స్థిరరాశి Replaceable *rep=(స్థిరరాశి Replaceable *)ut->context;
    int32_t length=rep->length();   // Full length of the input text (bigger than a chunk)

    // clip the requested index to the limits of the text.
    int32_t index32 = pin_సూచీ(సూచీ, length);
    U_ASSERT(సూచీ<=INT32_MAX);


    /*
     * Compute start/limit boundaries around index, for a segment of text
     * to be extracted.
     * To allow for the possibility that our user gave an index to the trailing
     * half of a surrogate pair, we must request one extra preceding char16_t when
     * going in the forward direction.  This will ensure that the buffer has the
     * entire code point at the specified index.
     */
    ఒకవేళ(forward) {

        ఒకవేళ (index32>=ut->chunk_Native_ప్రారంభం && index32<ut->chunk_Native_హద్దు) {
            // Buffer already contains the requested position.
            ut->chunk_స్థానభ్రంశం = (int32_t)(సూచీ - ut->chunk_Native_ప్రారంభం);
            తిరిగి నిజం;
        }
        ఒకవేళ (index32>=length && ut->chunk_Native_హద్దు==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunk_స్థానభ్రంశం = length - (int32_t)ut->chunk_Native_ప్రారంభం;
            తిరిగి అబద్ధం;
        }

        ut->chunk_Native_హద్దు = సూచీ + REP_పాఠ్యం_CHUNK_కొలత - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        ఒకవేళ(ut->chunk_Native_హద్దు > length) {
            ut->chunk_Native_హద్దు = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunk_Native_ప్రారంభం = ut->chunk_Native_హద్దు - REP_పాఠ్యం_CHUNK_కొలత;
        ఒకవేళ(ut->chunk_Native_ప్రారంభం < 0) {
            ut->chunk_Native_ప్రారంభం = 0;
        }
    } లేకపోతే {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        ఒకవేళ (index32>ut->chunk_Native_ప్రారంభం && index32<=ut->chunk_Native_హద్దు) {
            // Requested position already in buffer.
            ut->chunk_స్థానభ్రంశం = index32 - (int32_t)ut->chunk_Native_ప్రారంభం;
            తిరిగి నిజం;
        }
        ఒకవేళ (index32==0 && ut->chunk_Native_ప్రారంభం==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunk_స్థానభ్రంశం = 0;
            తిరిగి అబద్ధం;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunk_Native_ప్రారంభం = index32 + 1 - REP_పాఠ్యం_CHUNK_కొలత;
        ఒకవేళ (ut->chunk_Native_ప్రారంభం < 0) {
            ut->chunk_Native_ప్రారంభం = 0;
        }

        ut->chunk_Native_హద్దు = index32 + 1;
        ఒకవేళ (ut->chunk_Native_హద్దు > length) {
            ut->chunk_Native_హద్దు = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString బఫరు(ex->s, 0 /*buffer length*/, REP_పాఠ్యం_CHUNK_కొలత /*buffer capacity*/);
    rep->వెలికితీయి_Between((int32_t)ut->chunk_Native_ప్రారంభం, (int32_t)ut->chunk_Native_హద్దు, బఫరు);

    ut->chunkContents  = ex->s;
    ut->chunk_నిడివి    = (int32_t)(ut->chunk_Native_హద్దు - ut->chunk_Native_ప్రారంభం);
    ut->chunk_స్థానభ్రంశం    = (int32_t)(index32 - ut->chunk_Native_ప్రారంభం);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    ఒకవేళ (ut->chunk_Native_హద్దు < length &&
        U16_IS_LEAD(ex->s[ut->chunk_నిడివి-1])) {
            ut->chunk_నిడివి--;
            ut->chunk_Native_హద్దు--;
            ఒకవేళ (ut->chunk_స్థానభ్రంశం > ut->chunk_నిడివి) {
                ut->chunk_స్థానభ్రంశం = ut->chunk_నిడివి;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    ఒకవేళ(ut->chunk_Native_ప్రారంభం>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunk_Native_ప్రారంభం);
        --(ut->chunk_నిడివి);
        --(ut->chunk_స్థానభ్రంశం);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_సమితి_CP_ప్రారంభం(ut->chunkContents, 0, ut->chunk_స్థానభ్రంశం);

    // Use fast indexing for get/setNativeIndex()
    ut->native_Indexing_హద్దు = ut->chunk_నిడివి;

    తిరిగి నిజం;
}



స్థిర int32_t U_CALLCONV
rep_పాఠ్యం_వెలికితీయి(UText *ut,
               int64_t ప్రారంభం, int64_t హద్దు,
               char16_t *dest, int32_t dest_సామర్థ్యం,
               UError_సంకేతం *స్థితి_2) {
    స్థిరరాశి Replaceable *rep=(స్థిరరాశి Replaceable *)ut->context;
    int32_t  length=rep->length();

    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం>0)) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
    }
    ఒకవేళ(ప్రారంభం>హద్దు) {
        *స్థితి_2=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    int32_t  start32 = pin_సూచీ(ప్రారంభం, length);
    int32_t  limit32 = pin_సూచీ(హద్దు, length);

    // adjust start, limit if they point to trail half of surrogates
    ఒకవేళ (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    ఒకవేళ (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    ఒకవేళ(length>dest_సామర్థ్యం) {
        limit32 = start32 + dest_సామర్థ్యం;
    }
    UnicodeString బఫరు(dest, 0, dest_సామర్థ్యం); // writable alias
    rep->వెలికితీయి_Between(start32, limit32, బఫరు);
    rep_పాఠ్యం_Access(ut, limit32, నిజం);

    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, length, స్థితి_2);
}

స్థిర int32_t U_CALLCONV
rep_పాఠ్యం_Replace(UText *ut,
               int64_t ప్రారంభం, int64_t హద్దు,
               స్థిరరాశి char16_t *src, int32_t length,
               UError_సంకేతం *స్థితి_2) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t old_నిడివి;

    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ(src==శూన్యసూచిక && length!=0) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    old_నిడివి=rep->length(); // will subtract from new length
    ఒకవేళ(ప్రారంభం>హద్దు ) {
        *స్థితి_2=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    int32_t start32 = pin_సూచీ(ప్రారంభం, old_నిడివి);
    int32_t limit32 = pin_సూచీ(హద్దు, old_నిడివి);

    // Snap start & limit to code point boundaries.
    ఒకవేళ (start32<old_నిడివి && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    ఒకవేళ (limit32<old_నిడివి && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t new_నిడివి = rep->length();
    int32_t నిడివి_Delta = new_నిడివి - old_నిడివి;

    // Is the UText chunk buffer OK?
    ఒకవేళ (ut->chunk_Native_హద్దు > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t new_సూచీ_Pos = limit32 + నిడివి_Delta;
    rep_పాఠ్యం_Access(ut, new_సూచీ_Pos, నిజం);

    తిరిగి నిడివి_Delta;
}


స్థిర శూన్య U_CALLCONV
rep_పాఠ్యం_Copy(UText *ut,
                int64_t ప్రారంభం, int64_t హద్దు,
                int64_t dest_సూచీ,
                UBool move,
                UError_సంకేతం *స్థితి_2)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (ప్రారంభం>హద్దు || (ప్రారంభం<dest_సూచీ && dest_సూచీ<హద్దు))
    {
        *స్థితి_2=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }

    int32_t start32     = pin_సూచీ(ప్రారంభం, length);
    int32_t limit32     = pin_సూచీ(హద్దు, length);
    int32_t destIndex32 = pin_సూచీ(dest_సూచీ, length);

    // TODO:  snap input parameters to code point boundaries.

    ఒకవేళ(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t seg_నిడివి=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        ఒకవేళ(destIndex32<start32) {
            start32+=seg_నిడివి;
            limit32+=seg_నిడివి;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } లేకపోతే {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t మొదటి_Affected_సూచీ = destIndex32;
    ఒకవేళ (move && start32<మొదటి_Affected_సూచీ) {
        మొదటి_Affected_సూచీ = start32;
    }
    ఒకవేళ (మొదటి_Affected_సూచీ < ut->chunk_Native_హద్దు) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  native_Iter_సూచీ = destIndex32 + limit32 - start32;
    ఒకవేళ (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        native_Iter_సూచీ = destIndex32;
    }

    // Set position, reload chunk if needed.
    rep_పాఠ్యం_Access(ut, native_Iter_సూచీ, నిజం);
}

స్థిర స్థిరరాశి నిర్మాణం UTextFuncs repFuncs =
{
    పరిమాణం(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    rep_పాఠ్యం_Clone,
    rep_పాఠ్యం_నిడివి,
    rep_పాఠ్యం_Access,
    rep_పాఠ్యం_వెలికితీయి,
    rep_పాఠ్యం_Replace,
    rep_పాఠ్యం_Copy,
    శూన్యసూచిక,              // MapOffsetToNative,
    శూన్యసూచిక,              // MapIndexToUTF16,
    rep_పాఠ్యం_Close,
    శూన్యసూచిక,              // spare 1
    శూన్యసూచిక,              // spare 2
    శూన్యసూచిక               // spare 3
};


U_CAPI UText* U_EXPORT2
utext_openReplaceable(UText* ut U_LIFETIME_BOUND,
                      Replaceable* rep U_LIFETIME_BOUND,
                      UError_సంకేతం* స్థితి_2) {
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(rep==శూన్యసూచిక) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ut = utext_అమరిక(ut, పరిమాణం(ReplExtra), స్థితి_2);
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    ఒకవేళ(rep->has_Meta_దత్తాంశం()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_దత్తాంశం);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    తిరిగి ut;
}

U_CDECL_చివర








//------------------------------------------------------------------------------
//
//     UText implementation for UnicodeString (read/write)  and
//                    for const UnicodeString (read only)
//             (same implementation, only the flags are different)
//
//         Use of UText data members:
//            context    pointer to UnicodeString
//            p          pointer to UnicodeString IF this UText owns the string
//                       and it must be deleted on close().  nullptr otherwise.
//
//------------------------------------------------------------------------------

U_CDECL_BEGIN


స్థిర UText * U_CALLCONV
unistr_పాఠ్యం_Clone(UText *dest, స్థిరరాశి UText *src, UBool deep, UError_సంకేతం *స్థితి_2) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_పాఠ్యం_Clone(dest, src, స్థితి_2);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    ఒకవేళ (deep && U_SUCCESS(*స్థితి_2)) {
        స్థిరరాశి UnicodeString *srcString = (స్థిరరాశి UnicodeString *)src->context;
        dest->context = కొత్త UnicodeString(*srcString);
        dest->chunkContents = ((స్థిరరాశి UnicodeString *)dest->context)->get_బఫరు();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    తిరిగి dest;
}

స్థిర శూన్య U_CALLCONV
unistr_పాఠ్యం_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ఒకవేళ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        తొలగించు str;
        ut->context = శూన్యసూచిక;
    }
}


స్థిర int64_t U_CALLCONV
unistr_పాఠ్యం_నిడివి(UText *t) {
    తిరిగి ((స్థిరరాశి UnicodeString *)t->context)->length();
}


స్థిర UBool U_CALLCONV
unistr_పాఠ్యం_Access(UText *ut, int64_t సూచీ, UBool  forward) {
    int32_t length  = ut->chunk_నిడివి;
    ut->chunk_స్థానభ్రంశం = pin_సూచీ(సూచీ, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && సూచీ<length) || (!forward && సూచీ>0);
    తిరిగి retVal;
}



స్థిర int32_t U_CALLCONV
unistr_పాఠ్యం_వెలికితీయి(UText *t,
                  int64_t ప్రారంభం, int64_t హద్దు,
                  char16_t *dest, int32_t dest_సామర్థ్యం,
                  UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UnicodeString *us=(స్థిరరాశి UnicodeString *)t->context;
    int32_t length=us->length();

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం>0)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    }
    ఒకవేళ(ప్రారంభం<0 || ప్రారంభం>హద్దు) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    int32_t start32 = ప్రారంభం<length ? us->get_Char32_ప్రారంభం((int32_t)ప్రారంభం) : length;
    int32_t limit32 = హద్దు<length ? us->get_Char32_ప్రారంభం((int32_t)హద్దు) : length;

    length=limit32-start32;
    ఒకవేళ (dest_సామర్థ్యం>0 && dest!=శూన్యసూచిక) {
        int32_t trimmed_నిడివి = length;
        ఒకవేళ(trimmed_నిడివి>dest_సామర్థ్యం) {
            trimmed_నిడివి=dest_సామర్థ్యం;
        }
        us->extract(start32, trimmed_నిడివి, dest);
        t->chunk_స్థానభ్రంశం = start32+trimmed_నిడివి;
    } లేకపోతే {
        t->chunk_స్థానభ్రంశం = start32;
    }
    u_terminateUChars(dest, dest_సామర్థ్యం, length, p_Error_సంకేతం);
    తిరిగి length;
}

స్థిర int32_t U_CALLCONV
unistr_పాఠ్యం_Replace(UText *ut,
                  int64_t ప్రారంభం, int64_t హద్దు,
                  స్థిరరాశి char16_t *src, int32_t length,
                  UError_సంకేతం *p_Error_సంకేతం) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t old_నిడివి;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(src==శూన్యసూచిక && length!=0) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    }
    ఒకవేళ(ప్రారంభం>హద్దు) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }
    old_నిడివి=us->length();
    int32_t start32 = pin_సూచీ(ప్రారంభం, old_నిడివి);
    int32_t limit32 = pin_సూచీ(హద్దు, old_నిడివి);
    ఒకవేళ (start32 < old_నిడివి) {
        start32 = us->get_Char32_ప్రారంభం(start32);
    }
    ఒకవేళ (limit32 < old_నిడివి) {
        limit32 = us->get_Char32_ప్రారంభం(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t new_నిడివి = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->get_బఫరు();
    ut->chunk_నిడివి      = new_నిడివి;
    ut->chunk_Native_హద్దు = new_నిడివి;
    ut->native_Indexing_హద్దు = new_నిడివి;

    // Set iteration position to the point just following the newly inserted text.
    int32_t నిడివి_Delta = new_నిడివి - old_నిడివి;
    ut->chunk_స్థానభ్రంశం = limit32 + నిడివి_Delta;

    తిరిగి నిడివి_Delta;
}

స్థిర శూన్య U_CALLCONV
unistr_పాఠ్యం_Copy(UText *ut,
               int64_t ప్రారంభం, int64_t హద్దు,
               int64_t dest_సూచీ,
               UBool move,
               UError_సంకేతం *p_Error_సంకేతం) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    int32_t start32 = pin_సూచీ(ప్రారంభం, length);
    int32_t limit32 = pin_సూచీ(హద్దు, length);
    int32_t destIndex32 = pin_సూచీ(dest_సూచీ, length);

    ఒకవేళ( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }

    ఒకవేళ(move) {
        // move: copy to destIndex, then remove original
        int32_t seg_నిడివి=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        ఒకవేళ(destIndex32<start32) {
            start32+=seg_నిడివి;
        }
        us->remove(start32, seg_నిడివి);
    } లేకపోతే {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->get_బఫరు();
    ఒకవేళ (move==అబద్ధం) {
        // copy operation, string length grows
        ut->chunk_నిడివి += limit32-start32;
        ut->chunk_Native_హద్దు = ut->chunk_నిడివి;
        ut->native_Indexing_హద్దు = ut->chunk_నిడివి;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunk_స్థానభ్రంశం = destIndex32+limit32-start32;
    ఒకవేళ (move && destIndex32>start32) {
        ut->chunk_స్థానభ్రంశం = destIndex32;
    }

}

స్థిర స్థిరరాశి నిర్మాణం UTextFuncs unistrFuncs =
{
    పరిమాణం(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistr_పాఠ్యం_Clone,
    unistr_పాఠ్యం_నిడివి,
    unistr_పాఠ్యం_Access,
    unistr_పాఠ్యం_వెలికితీయి,
    unistr_పాఠ్యం_Replace,
    unistr_పాఠ్యం_Copy,
    శూన్యసూచిక,                // MapOffsetToNative,
    శూన్యసూచిక,                // MapIndexToUTF16,
    unistr_పాఠ్యం_Close,
    శూన్యసూచిక,                // spare 1
    శూన్యసూచిక,                // spare 2
    శూన్యసూచిక                 // spare 3
};



U_CDECL_చివర


U_CAPI UText* U_EXPORT2
utext_openUnicodeString(UText* ut U_LIFETIME_BOUND,
                        UnicodeString* s U_LIFETIME_BOUND,
                        UError_సంకేతం* స్థితి_2) {
    ut = utext_openConstUnicodeString(ut, s, స్థితి_2);
    ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    తిరిగి ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             స్థిరరాశి UnicodeString* s U_LIFETIME_BOUND,
                             UError_సంకేతం* స్థితి_2) {
    ఒకవేళ (U_SUCCESS(*స్థితి_2) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, శూన్యసూచిక, 0, స్థితి_2);
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి ut;
    }
    ut = utext_అమరిక(ut, 0, స్థితి_2);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->get_బఫరు();
        ut->chunk_నిడివి         = s->length();
        ut->chunk_Native_ప్రారంభం    = 0;
        ut->chunk_Native_హద్దు    = ut->chunk_నిడివి;
        ut->native_Indexing_హద్దు = ut->chunk_నిడివి;
    }
    తిరిగి ut;
}

//------------------------------------------------------------------------------
//
//     UText implementation for const char16_t * strings
//
//         Use of UText data members:
//            context    pointer to UnicodeString
//            a          length.  -1 if not yet known.
//
//         TODO:  support 64 bit lengths.
//
//------------------------------------------------------------------------------

U_CDECL_BEGIN


స్థిర UText * U_CALLCONV
ucstr_పాఠ్యం_Clone(UText *dest, స్థిరరాశి UText * src, UBool deep, UError_సంకేతం * స్థితి_2) {
    // First do a generic shallow clone.
    dest = shallow_పాఠ్యం_Clone(dest, src, స్థితి_2);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    ఒకవేళ (deep && U_SUCCESS(*స్థితి_2)) {
        U_ASSERT(utext_native_నిడివి(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_native_నిడివి(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        స్థిరరాశి char16_t *srcStr = (స్థిరరాశి char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * పరిమాణం(char16_t));
        ఒకవేళ (copyStr == శూన్యసూచిక) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            int64_t i;
            కొరకు (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం);
        }
    }
    తిరిగి dest;
}


స్థిర శూన్య U_CALLCONV
ucstr_పాఠ్యం_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ఒకవేళ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_పాఠ్యం)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = శూన్యసూచిక;
    }
}



స్థిర int64_t U_CALLCONV
ucstr_పాఠ్యం_నిడివి(UText *ut) {
    ఒకవేళ (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        స్థిరరాశి char16_t  *str = (స్థిరరాశి char16_t *)ut->context;
        కొరకు (;;) {
            ఒకవేళ (str[ut->chunk_Native_హద్దు] == 0) {
                ఆపు;
            }
            ut->chunk_Native_హద్దు++;
        }
        ut->a = ut->chunk_Native_హద్దు;
        ut->chunk_నిడివి = (int32_t)ut->chunk_Native_హద్దు;
        ut->native_Indexing_హద్దు = ut->chunk_నిడివి;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE);
    }
    తిరిగి ut->a;
}


స్థిర UBool U_CALLCONV
ucstr_పాఠ్యం_Access(UText *ut, int64_t సూచీ, UBool  forward) {
    స్థిరరాశి char16_t *str   = (స్థిరరాశి char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    ఒకవేళ (సూచీ<0) {
        సూచీ = 0;
    } లేకపోతే ఒకవేళ (సూచీ < ut->chunk_Native_హద్దు) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_సమితి_CP_ప్రారంభం(str, 0, సూచీ);
    } లేకపోతే ఒకవేళ (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        సూచీ = ut->a;
    } లేకపోతే {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t పరిశీలన_హద్దు = (int32_t)సూచీ + 32;
        ఒకవేళ ((సూచీ + 32)>INT32_MAX || (సూచీ + 32)<0 ) {   // note: int64 expression
            పరిశీలన_హద్దు = INT32_MAX;
        }

        int32_t chunk_హద్దు = (int32_t)ut->chunk_Native_హద్దు;
        కొరకు (; chunk_హద్దు<పరిశీలన_హద్దు; chunk_హద్దు++) {
            ఒకవేళ (str[chunk_హద్దు] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunk_హద్దు;
                ut->chunk_నిడివి = chunk_హద్దు;
                ut->native_Indexing_హద్దు = chunk_హద్దు;
                ఒకవేళ (సూచీ >= chunk_హద్దు) {
                    సూచీ = chunk_హద్దు;
                } లేకపోతే {
                    U16_సమితి_CP_ప్రారంభం(str, 0, సూచీ);
                }

                ut->chunk_Native_హద్దు = chunk_హద్దు;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE);
                వెళ్ళు breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_సమితి_CP_ప్రారంభం(str, 0, సూచీ);
        ఒకవేళ (chunk_హద్దు == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunk_హద్దు;
            ut->chunk_నిడివి = chunk_హద్దు;
            ut->native_Indexing_హద్దు = chunk_హద్దు;
            ఒకవేళ (సూచీ > chunk_హద్దు) {
                సూచీ = chunk_హద్దు;
            }
            ut->chunk_Native_హద్దు = chunk_హద్దు;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE);
        } లేకపోతే {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            ఒకవేళ (U16_IS_LEAD(str[chunk_హద్దు-1])) {
                --chunk_హద్దు;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunk_Native_హద్దు = chunk_హద్దు;
            ut->native_Indexing_హద్దు = chunk_హద్దు;
            ut->chunk_నిడివి = chunk_హద్దు;
        }

    }
breakout:
    U_ASSERT(సూచీ<=INT32_MAX);
    ut->chunk_స్థానభ్రంశం = (int32_t)సూచీ;

    // Check whether request is at the start or end
    UBool retVal = (forward && సూచీ<ut->chunk_Native_హద్దు) || (!forward && సూచీ>0);
    తిరిగి retVal;
}



స్థిర int32_t U_CALLCONV
ucstr_పాఠ్యం_వెలికితీయి(UText *ut,
                  int64_t ప్రారంభం, int64_t హద్దు,
                  char16_t *dest, int32_t dest_సామర్థ్యం,
                  UError_సంకేతం *p_Error_సంకేతం)
{
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం>0) || ప్రారంభం>హద్దు) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstr_పాఠ్యం_Access(ut, ప్రారంభం, నిజం);
    స్థిరరాశి char16_t *s=ut->chunkContents;
    start32 = ut->chunk_స్థానభ్రంశం;

    int32_t str_నిడివి=(int32_t)ut->a;
    ఒకవేళ (str_నిడివి >= 0) {
        limit32 = pin_సూచీ(హద్దు, str_నిడివి);
    } లేకపోతే {
        limit32 = pin_సూచీ(హద్దు, INT32_MAX);
    }
    di = 0;
    కొరకు (si=start32; si<limit32; si++) {
        ఒకవేళ (str_నిడివి<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunk_Native_హద్దు    = si;
            ut->chunk_నిడివి         = si;
            ut->native_Indexing_హద్దు = si;
            str_నిడివి               = si;
            limit32                 = si;
            ఆపు;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        ఒకవేళ (di<dest_సామర్థ్యం) {
            // only store if there is space.
            dest[di] = s[si];
        } లేకపోతే {
            ఒకవేళ (str_నిడివి>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                ఆపు;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    ఒకవేళ (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<str_నిడివి || str_నిడివి<0)  && U16_IS_TRAIL(s[si])))
    {
        ఒకవేళ (di<dest_సామర్థ్యం) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    ఒకవేళ (si <= ut->chunk_Native_హద్దు) {
        ut->chunk_స్థానభ్రంశం = si;
    } లేకపోతే {
        ucstr_పాఠ్యం_Access(ut, si, నిజం);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, dest_సామర్థ్యం, di, p_Error_సంకేతం);
    తిరిగి di;
}

స్థిర స్థిరరాశి నిర్మాణం UTextFuncs ucstrFuncs =
{
    పరిమాణం(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstr_పాఠ్యం_Clone,
    ucstr_పాఠ్యం_నిడివి,
    ucstr_పాఠ్యం_Access,
    ucstr_పాఠ్యం_వెలికితీయి,
    శూన్యసూచిక,              // Replace
    శూన్యసూచిక,              // Copy
    శూన్యసూచిక,              // MapOffsetToNative,
    శూన్యసూచిక,              // MapIndexToUTF16,
    ucstr_పాఠ్యం_Close,
    శూన్యసూచిక,              // spare 1
    శూన్యసూచిక,              // spare 2
    శూన్యసూచిక,              // spare 3
};

U_CDECL_చివర

స్థిర స్థిరరాశి char16_t g_ఖాళీలు_UString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 స్థిరరాశి char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UError_సంకేతం* స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(s==శూన్యసూచిక && length==0) {
        s = g_ఖాళీలు_UString;
    }
    ఒకవేళ (s==శూన్యసూచిక || length < -1 || length>INT32_MAX) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ut = utext_అమరిక(ut, 0, స్థితి_2);
    ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ఒకవేళ (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_నిడివి_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunk_Native_ప్రారంభం     = 0;
        ut->chunk_Native_హద్దు     = length>=0? length : 0;
        ut->chunk_నిడివి          = (int32_t)ut->chunk_Native_హద్దు;
        ut->chunk_స్థానభ్రంశం          = 0;
        ut->native_Indexing_హద్దు  = ut->chunk_నిడివి;
    }
    తిరిగి ut;
}


//------------------------------------------------------------------------------
//
//     UText implementation for text from ICU CharacterIterators
//
//         Use of UText data members:
//            context    pointer to the CharacterIterator
//            a          length of the full text.
//            p          pointer to  buffer 1
//            b          start index of local buffer 1 contents
//            q          pointer to buffer 2
//            c          start index of local buffer 2 contents
//            r          pointer to the character iterator if the UText owns it.
//                       Null otherwise.
//
//------------------------------------------------------------------------------
#నిర్వచించు CIBuf_కొలత 16

U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
char_Iter_పాఠ్యం_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    తొలగించు ci;
    ut->r = శూన్యసూచిక;
}

స్థిర int64_t U_CALLCONV
char_Iter_పాఠ్యం_నిడివి(UText *ut) {
    తిరిగి (int32_t)ut->a;
}

స్థిర UBool U_CALLCONV
char_Iter_పాఠ్యం_Access(UText *ut, int64_t సూచీ, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clipped_సూచీ = (int32_t)సూచీ;
    ఒకవేళ (clipped_సూచీ<0) {
        clipped_సూచీ=0;
    } లేకపోతే ఒకవేళ (clipped_సూచీ>=ut->a) {
        clipped_సూచీ=(int32_t)ut->a;
    }
    int32_t needed_సూచీ = clipped_సూచీ;
    ఒకవేళ (!forward && needed_సూచీ>0) {
        // reverse iteration, want the position just before what was asked for.
        needed_సూచీ--;
    } లేకపోతే ఒకవేళ (forward && needed_సూచీ==ut->a && needed_సూచీ>0) {
        // Forward iteration, don't ask for something past the end of the text.
        needed_సూచీ--;
    }

    // Find the native index of the start of the buffer containing what we want.
    needed_సూచీ -= needed_సూచీ % CIBuf_కొలత;

    char16_t *buf = శూన్యసూచిక;
    UBool  need_Chunk_అమరిక = నిజం;
    పూర్ణాంకం    i;
    ఒకవేళ (ut->chunk_Native_ప్రారంభం == needed_సూచీ) {
        // The buffer we want is already the current chunk.
        need_Chunk_అమరిక = అబద్ధం;
    } లేకపోతే ఒకవేళ (ut->b == needed_సూచీ) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } లేకపోతే ఒకవేళ (ut->c == needed_సూచీ) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } లేకపోతే {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        ఒకవేళ (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->సమితి_సూచీ(needed_సూచీ);
        కొరకు (i=0; i<CIBuf_కొలత; i++) {
            buf[i] = ci->తదుపరి_Post_Inc();
            ఒకవేళ (i+needed_సూచీ > ut->a) {
                ఆపు;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    ఒకవేళ (need_Chunk_అమరిక) {
        ut->chunkContents = buf;
        ut->chunk_నిడివి   = CIBuf_కొలత;
        ut->chunk_Native_ప్రారంభం = needed_సూచీ;
        ut->chunk_Native_హద్దు = needed_సూచీ + CIBuf_కొలత;
        ఒకవేళ (ut->chunk_Native_హద్దు > ut->a) {
            ut->chunk_Native_హద్దు = ut->a;
            ut->chunk_నిడివి  = (int32_t)(ut->chunk_Native_హద్దు)-(int32_t)(ut->chunk_Native_ప్రారంభం);
        }
        ut->native_Indexing_హద్దు = ut->chunk_నిడివి;
        U_ASSERT(ut->chunk_స్థానభ్రంశం>=0 && ut->chunk_స్థానభ్రంశం<=CIBuf_కొలత);
    }
    ut->chunk_స్థానభ్రంశం = clipped_సూచీ - (int32_t)ut->chunk_Native_ప్రారంభం;
    UBool success = (forward? ut->chunk_స్థానభ్రంశం<ut->chunk_నిడివి : ut->chunk_స్థానభ్రంశం>0);
    తిరిగి success;
}

స్థిర UText * U_CALLCONV
char_Iter_పాఠ్యం_Clone(UText *dest, స్థిరరాశి UText *src, UBool deep, UError_సంకేతం * స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, స్థితి_2);
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_get_Native_సూచీ((UText *)src);
        utext_సమితి_Native_సూచీ(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    తిరిగి dest;
}

స్థిర int32_t U_CALLCONV
char_Iter_పాఠ్యం_వెలికితీయి(UText *ut,
                  int64_t ప్రారంభం, int64_t హద్దు,
                  char16_t *dest, int32_t dest_సామర్థ్యం,
                  UError_సంకేతం *స్థితి_2)
{
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం>0) || ప్రారంభం>హద్దు) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pin_సూచీ(ప్రారంభం, length);
    int32_t  limit32 = pin_సూచీ(హద్దు, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copy_హద్దు;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->సమితి_Index32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->get_సూచీ();
    copy_హద్దు = srci;
    అయితే (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_నిడివి(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        ఒకవేళ (desti+len <= dest_సామర్థ్యం) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copy_హద్దు = srci+len;
        } లేకపోతే {
            desti += len;
            *స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
        }
        srci += len;
    }

    char_Iter_పాఠ్యం_Access(ut, copy_హద్దు, నిజం);

    u_terminateUChars(dest, dest_సామర్థ్యం, desti, స్థితి_2);
    తిరిగి desti;
}

స్థిర స్థిరరాశి నిర్మాణం UTextFuncs charIterFuncs =
{
    పరిమాణం(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    char_Iter_పాఠ్యం_Clone,
    char_Iter_పాఠ్యం_నిడివి,
    char_Iter_పాఠ్యం_Access,
    char_Iter_పాఠ్యం_వెలికితీయి,
    శూన్యసూచిక,                // Replace
    శూన్యసూచిక,                // Copy
    శూన్యసూచిక,                // MapOffsetToNative,
    శూన్యసూచిక,                // MapIndexToUTF16,
    char_Iter_పాఠ్యం_Close,
    శూన్యసూచిక,                // spare 1
    శూన్యసూచిక,                // spare 2
    శూన్యసూచిక                 // spare 3
};
U_CDECL_చివర

U_CAPI UText* U_EXPORT2
utext_openCharacterIterator(UText* ut U_LIFETIME_BOUND,
                            CharacterIterator* ci U_LIFETIME_BOUND,
                            UError_సంకేతం* స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (ci->ప్రారంభం_సూచీ() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBuf_కొలత * పరిమాణం(char16_t);
    ut = utext_అమరిక(ut, extraSpace, స్థితి_2);
    ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->చివర_సూచీ();        // Length of text
        ut->p                    = ut->pExtra;            // First buffer
        ut->b                    = -1;                    // Native index of first buffer contents
        ut->q                    = (char16_t*)ut->pExtra+CIBuf_కొలత;  // Second buffer
        ut->c                    = -1;                    // Native index of second buffer contents

        // Initialize current chunk contents to be empty.
        //   First access will fault something in.
        //   Note:  The initial nativeStart and chunkOffset must sum to zero
        //          so that getNativeIndex() will correctly compute to zero
        //          if no call to Access() has ever been made.  They can't be both
        //          zero without Access() thinking that the chunk is valid.
        ut->chunkContents        = (char16_t *)ut->p;
        ut->chunk_Native_ప్రారంభం     = -1;
        ut->chunk_స్థానభ్రంశం          = 1;
        ut->chunk_Native_హద్దు     = 0;
        ut->chunk_నిడివి          = 0;
        ut->native_Indexing_హద్దు  = ut->chunk_స్థానభ్రంశం;  // enables native indexing
    }
    తిరిగి ut;
}

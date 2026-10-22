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

#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/chariter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"

U_NAMESPACE_USE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ I32_FLAG(ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ) ((int32_t)1<<(ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ))


ਸਥਿਰ UBool
utext_access(UText *ut, int64_t ਸੂਚਕਾਂਕ, UBool forward) {
    ਵਾਪਸ ut->pFuncs->access(ut, ਸੂਚਕਾਂਕ, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    ਜੇ (delta > 0) {
        ਕਰੋ {
            ਜੇ(ut->chunk_ਵਿੱਥ>=ut->chunk_ਲੰਮਾਈ && !utext_access(ut, ut->chunk_Native_ਹੱਦਬੰਦੀ, ਸੱਚ)) {
                ਵਾਪਸ ਝੂਠ;
            }
            c = ut->chunkContents[ut->chunk_ਵਿੱਥ];
            ਜੇ (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                ਜੇ (c == U_SENTINEL) {
                    ਵਾਪਸ ਝੂਠ;
                }
            } ਨਹੀਂਤਾਂ {
                ut->chunk_ਵਿੱਥ++;
            }
        } ਜਦੋਂ(--delta>0);

    } ਨਹੀਂਤਾਂ ਜੇ (delta<0) {
        ਕਰੋ {
            ਜੇ(ut->chunk_ਵਿੱਥ<=0 && !utext_access(ut, ut->chunk_Native_ਸ਼ੁਰੂ, ਝੂਠ)) {
                ਵਾਪਸ ਝੂਠ;
            }
            c = ut->chunkContents[ut->chunk_ਵਿੱਥ-1];
            ਜੇ (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                ਜੇ (c == U_SENTINEL) {
                    ਵਾਪਸ ਝੂਠ;
                }
            } ਨਹੀਂਤਾਂ {
                ut->chunk_ਵਿੱਥ--;
            }
        } ਜਦੋਂ(++delta<0);
    }

    ਵਾਪਸ ਸੱਚ;
}


U_CAPI int64_t U_EXPORT2
utext_native_ਲੰਮਾਈ(UText *ut) {
    ਵਾਪਸ ut->pFuncs->native_ਲੰਮਾਈ(ut);
}


U_CAPI UBool U_EXPORT2
utext_is_ਲੰਮਾਈ_Expensive(ਅਚਲ UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE)) != 0;
    ਵਾਪਸ r;
}


U_CAPI int64_t U_EXPORT2
utext_get_Native_ਸੂਚਕਾਂਕ(ਅਚਲ UText *ut) {
    ਜੇ(ut->chunk_ਵਿੱਥ <= ut->native_Indexing_ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ ut->chunk_Native_ਸ਼ੁਰੂ+ut->chunk_ਵਿੱਥ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ut->pFuncs->map_ਵਿੱਥ_ਮੰਜ਼ਿਲ_Native(ut);
    }
}


U_CAPI ਖਾਲੀ U_EXPORT2
utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(UText *ut, int64_t ਸੂਚਕਾਂਕ) {
    ਜੇ(ਸੂਚਕਾਂਕ<ut->chunk_Native_ਸ਼ੁਰੂ || ਸੂਚਕਾਂਕ>=ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, ਸੂਚਕਾਂਕ, ਸੱਚ);
    } ਨਹੀਂਤਾਂ ਜੇ((int32_t)(ਸੂਚਕਾਂਕ - ut->chunk_Native_ਸ਼ੁਰੂ) <= ut->native_Indexing_ਹੱਦਬੰਦੀ) {
        // utf-16 indexing.
        ut->chunk_ਵਿੱਥ=(int32_t)(ਸੂਚਕਾਂਕ-ut->chunk_Native_ਸ਼ੁਰੂ);
    } ਨਹੀਂਤਾਂ {
         ut->chunk_ਵਿੱਥ=ut->pFuncs->map_Native_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_UTF16(ut, ਸੂਚਕਾਂਕ);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    ਜੇ (ut->chunk_ਵਿੱਥ<ut->chunk_ਲੰਮਾਈ) {
        char16_t c= ut->chunkContents[ut->chunk_ਵਿੱਥ];
        ਜੇ (U16_IS_TRAIL(c)) {
            ਜੇ (ut->chunk_ਵਿੱਥ==0) {
                ut->pFuncs->access(ut, ut->chunk_Native_ਸ਼ੁਰੂ, ਝੂਠ);
            }
            ਜੇ (ut->chunk_ਵਿੱਥ>0) {
                char16_t lead = ut->chunkContents[ut->chunk_ਵਿੱਥ-1];
                ਜੇ (U16_IS_LEAD(lead)) {
                    ut->chunk_ਵਿੱਥ--;
                }
            }
        }
    }
}



U_CAPI int64_t U_EXPORT2
utext_get_ਪਿਛਲਾ_Native_ਸੂਚਕਾਂਕ(UText *ut) {
    //
    //  Fast-path the common case.
    //     Common means current position is not at the beginning of a chunk
    //     and the preceding character is not supplementary.
    //
    int32_t i = ut->chunk_ਵਿੱਥ - 1;
    int64_t ਨਤੀਜਾ;
    ਜੇ (i >= 0) {
        char16_t c = ut->chunkContents[i];
        ਜੇ (U16_IS_TRAIL(c) == ਝੂਠ) {
            ਜੇ (i <= ut->native_Indexing_ਹੱਦਬੰਦੀ) {
                ਨਤੀਜਾ = ut->chunk_Native_ਸ਼ੁਰੂ + i;
            } ਨਹੀਂਤਾਂ {
                ut->chunk_ਵਿੱਥ = i;
                ਨਤੀਜਾ = ut->pFuncs->map_ਵਿੱਥ_ਮੰਜ਼ਿਲ_Native(ut);
                ut->chunk_ਵਿੱਥ++;
            }
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    }

    // If at the start of text, simply return 0.
    ਜੇ (ut->chunk_ਵਿੱਥ==0 && ut->chunk_Native_ਸ਼ੁਰੂ==0) {
        ਵਾਪਸ 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    ਨਤੀਜਾ = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    ਵਾਪਸ ਨਤੀਜਾ;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    ਜੇ (ut->chunk_ਵਿੱਥ==ut->chunk_ਲੰਮਾਈ) {
        // Current position is just off the end of the chunk.
        ਜੇ (ut->pFuncs->access(ut, ut->chunk_Native_ਹੱਦਬੰਦੀ, ਸੱਚ) == ਝੂਠ) {
            // Off the end of the text.
            ਵਾਪਸ U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_ਵਿੱਥ];
    ਜੇ (U16_IS_LEAD(c) == ਝੂਠ) {
        // Normal, non-supplementary case.
        ਵਾਪਸ c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    ਜੇ ((ut->chunk_ਵਿੱਥ+1) < ut->chunk_ਲੰਮਾਈ) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunk_ਵਿੱਥ+1];
    } ਨਹੀਂਤਾਂ {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunk_Native_ਹੱਦਬੰਦੀ;
        ਜੇ (ut->pFuncs->access(ut, nativePosition, ਸੱਚ)) {
            trail = ut->chunkContents[ut->chunk_ਵਿੱਥ];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, ਝੂਠ);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunk_ਵਿੱਥ = ut->chunk_ਲੰਮਾਈ - 1;
        ਜੇ(!r) {
            ਵਾਪਸ U_SENTINEL;
        }
    }

    ਜੇ (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    ਵਾਪਸ supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t native_ਸੂਚਕਾਂਕ) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    ਜੇ (native_ਸੂਚਕਾਂਕ>=ut->chunk_Native_ਸ਼ੁਰੂ && native_ਸੂਚਕਾਂਕ < ut->chunk_Native_ਸ਼ੁਰੂ + ut->native_Indexing_ਹੱਦਬੰਦੀ) {
        ut->chunk_ਵਿੱਥ = (int32_t)(native_ਸੂਚਕਾਂਕ - ut->chunk_Native_ਸ਼ੁਰੂ);
        c = ut->chunkContents[ut->chunk_ਵਿੱਥ];
        ਜੇ (U16_IS_SURROGATE(c) == ਝੂਠ) {
            ਵਾਪਸ c;
        }
    }


    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ut, native_ਸੂਚਕਾਂਕ);
    ਜੇ (native_ਸੂਚਕਾਂਕ>=ut->chunk_Native_ਸ਼ੁਰੂ && ut->chunk_ਵਿੱਥ<ut->chunk_ਲੰਮਾਈ) {
        c = ut->chunkContents[ut->chunk_ਵਿੱਥ];
        ਜੇ (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    ਵਾਪਸ c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    ਜੇ (ut->chunk_ਵਿੱਥ >= ut->chunk_ਲੰਮਾਈ) {
        ਜੇ (ut->pFuncs->access(ut, ut->chunk_Native_ਹੱਦਬੰਦੀ, ਸੱਚ) == ਝੂਠ) {
            ਵਾਪਸ U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_ਵਿੱਥ++];
    ਜੇ (U16_IS_LEAD(c) == ਝੂਠ) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        ਵਾਪਸ c;
    }

    ਜੇ (ut->chunk_ਵਿੱਥ >= ut->chunk_ਲੰਮਾਈ) {
        ਜੇ (ut->pFuncs->access(ut, ut->chunk_Native_ਹੱਦਬੰਦੀ, ਸੱਚ) == ਝੂਠ) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            ਵਾਪਸ c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunk_ਵਿੱਥ];
    ਜੇ (U16_IS_TRAIL(trail) == ਝੂਠ) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        ਵਾਪਸ c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunk_ਵਿੱਥ++;   // move iteration position over the trail surrogate.
    ਵਾਪਸ supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    ਜੇ (ut->chunk_ਵਿੱਥ <= 0) {
        ਜੇ (ut->pFuncs->access(ut, ut->chunk_Native_ਸ਼ੁਰੂ, ਝੂਠ) == ਝੂਠ) {
            ਵਾਪਸ U_SENTINEL;
        }
    }
    ut->chunk_ਵਿੱਥ--;
    c = ut->chunkContents[ut->chunk_ਵਿੱਥ];
    ਜੇ (U16_IS_TRAIL(c) == ਝੂਠ) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        ਵਾਪਸ c;
    }

    ਜੇ (ut->chunk_ਵਿੱਥ <= 0) {
        ਜੇ (ut->pFuncs->access(ut, ut->chunk_Native_ਸ਼ੁਰੂ, ਝੂਠ) == ਝੂਠ) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            ਵਾਪਸ c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunk_ਵਿੱਥ-1];
    ਜੇ (U16_IS_LEAD(lead) == ਝੂਠ) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        ਵਾਪਸ c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunk_ਵਿੱਥ--;   // move iteration position over the lead surrogate.
    ਵਾਪਸ supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t ਸੂਚਕਾਂਕ) {
    UChar32       c      = U_SENTINEL;

    ਜੇ(ਸੂਚਕਾਂਕ<ut->chunk_Native_ਸ਼ੁਰੂ || ਸੂਚਕਾਂਕ>=ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        // Desired position is outside of the current chunk.
        ਜੇ(!ut->pFuncs->access(ut, ਸੂਚਕਾਂਕ, ਸੱਚ)) {
            // no chunk available here
            ਵਾਪਸ U_SENTINEL;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (ਸੂਚਕਾਂਕ - ut->chunk_Native_ਸ਼ੁਰੂ  <= (int64_t)ut->native_Indexing_ਹੱਦਬੰਦੀ) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunk_ਵਿੱਥ = (int32_t)(ਸੂਚਕਾਂਕ - ut->chunk_Native_ਸ਼ੁਰੂ);
    } ਨਹੀਂਤਾਂ {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunk_ਵਿੱਥ = ut->pFuncs->map_Native_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_UTF16(ut, ਸੂਚਕਾਂਕ);
    }

    c = ut->chunkContents[ut->chunk_ਵਿੱਥ++];
    ਜੇ (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ut, ਸੂਚਕਾਂਕ);
        c = utext_next32(ut);
    }
    ਵਾਪਸ c;
}


U_CAPI UChar32 U_EXPORT2
utext_previous32From(UText *ut, int64_t ਸੂਚਕਾਂਕ) {
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
    ਜੇ(ਸੂਚਕਾਂਕ<=ut->chunk_Native_ਸ਼ੁਰੂ || ਸੂਚਕਾਂਕ>ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        // Requested native index is outside of the current chunk.
        ਜੇ(!ut->pFuncs->access(ut, ਸੂਚਕਾਂਕ, ਝੂਠ)) {
            // no chunk available here
            ਵਾਪਸ U_SENTINEL;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ - ut->chunk_Native_ਸ਼ੁਰੂ <= (int64_t)ut->native_Indexing_ਹੱਦਬੰਦੀ) {
        // Direct UTF-16 indexing.
        ut->chunk_ਵਿੱਥ = (int32_t)(ਸੂਚਕਾਂਕ - ut->chunk_Native_ਸ਼ੁਰੂ);
    } ਨਹੀਂਤਾਂ {
        ut->chunk_ਵਿੱਥ=ut->pFuncs->map_Native_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_UTF16(ut, ਸੂਚਕਾਂਕ);
        ਜੇ (ut->chunk_ਵਿੱਥ==0 && !ut->pFuncs->access(ut, ਸੂਚਕਾਂਕ, ਝੂਠ)) {
            // no chunk available here
            ਵਾਪਸ U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunk_ਵਿੱਥ--;
    cPrev = ut->chunkContents[ut->chunk_ਵਿੱਥ];

    ਜੇ (U16_IS_SURROGATE(cPrev)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ut, ਸੂਚਕਾਂਕ);
        cPrev = utext_previous32(ut);
    }
    ਵਾਪਸ cPrev;
}


U_CAPI int32_t U_EXPORT2
utext_ਕੱਢੋ(UText *ut,
             int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
             char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
             UError_ਕੋਡ *ਸਥਿਤੀ) {
                 ਵਾਪਸ ut->pFuncs->extract(ut, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, dest, dest_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
             }



U_CAPI UBool U_EXPORT2
utext_equals(ਅਚਲ UText *a, ਅਚਲ UText *b) {
    ਜੇ (a==ਖਾਲੀ_ਸੂਚਕ || b==ਖਾਲੀ_ਸੂਚਕ ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ (a->context != b->context) {
        // Different sources (different strings)
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (utext_get_Native_ਸੂਚਕਾਂਕ(a) != utext_get_Native_ਸੂਚਕਾਂਕ(b)) {
        // Different current position in the string.
        ਵਾਪਸ ਝੂਠ;
    }

    ਵਾਪਸ ਸੱਚ;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(ਅਚਲ UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    ਵਾਪਸ b;
}


U_CAPI ਖਾਲੀ U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_has_Meta_ਅੰਕੜੇ(ਅਚਲ UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_ਅੰਕੜੇ)) != 0;
    ਵਾਪਸ b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t native_ਸ਼ੁਰੂ, int64_t native_ਹੱਦਬੰਦੀ,
             ਅਚਲ char16_t *replacement_ਪਾਠ, int32_t replacement_ਲੰਮਾਈ,
             UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *ਸਥਿਤੀ = U_NO_WRITE_PERMISSION;
        ਵਾਪਸ 0;
    }
    int32_t i = ut->pFuncs->replace(ut, native_ਸ਼ੁਰੂ, native_ਹੱਦਬੰਦੀ, replacement_ਪਾਠ, replacement_ਲੰਮਾਈ, ਸਥਿਤੀ);
    ਵਾਪਸ i;
}

U_CAPI ਖਾਲੀ U_EXPORT2
utext_copy(UText *ut,
          int64_t native_ਸ਼ੁਰੂ, int64_t native_ਹੱਦਬੰਦੀ,
          int64_t dest_ਸੂਚਕਾਂਕ,
          UBool move,
          UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *ਸਥਿਤੀ = U_NO_WRITE_PERMISSION;
        ਵਾਪਸ;
    }
    ut->pFuncs->copy(ut, native_ਸ਼ੁਰੂ, native_ਹੱਦਬੰਦੀ, dest_ਸੂਚਕਾਂਕ, move, ਸਥਿਤੀ);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            ਅਚਲ UText* src,
            UBool deep,
            UBool readOnly,
            UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ dest;
    }
    UText *ਨਤੀਜਾ = src->pFuncs->clone(dest, src, deep, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜੇ (readOnly) {
        utext_freeze(ਨਤੀਜਾ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
ਗਿਣਤੀ {
    UTEXT_ਢੇਰ_ALLOCATED  = 1,      //  1 if ICU has allocated this UText struct on the heap.
                                    //  0 if caller provided storage for the UText.

    UTEXT_EXTRA_ਢੇਰ_ALLOCATED = 2, //  1 if ICU has allocated extra storage as a separate
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

ਬਣਤਰ ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

ਸਥਿਰ ਅਚਲ UText ਖਾਲੀ_ਥਾਵਾਂ_ਪਾਠ = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_ਤਿਆਰੀ(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ut;
    }

    ਜੇ (ut == ਖਾਲੀ_ਸੂਚਕ) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = ਆਕਾਰ(UText);
        ਜੇ (extraSpace > 0) {
            spaceRequired = ਆਕਾਰ(ExtendedUText) + extraSpace - ਆਕਾਰ(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        ਜੇ (ut == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        } ਨਹੀਂਤਾਂ {
            *ut = ਖਾਲੀ_ਥਾਵਾਂ_ਪਾਠ;
            ut->flags |= UTEXT_ਢੇਰ_ALLOCATED;
            ਜੇ (spaceRequired>0) {
                ut->extra_ਮਾਪ = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } ਨਹੀਂਤਾਂ {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        ਜੇ (ut->magic != UTEXT_MAGIC) {
            *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        ਜੇ ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != ਖਾਲੀ_ਸੂਚਕ)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        ਜੇ (extraSpace > ut->extra_ਮਾਪ) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            ਜੇ (ut->flags & UTEXT_EXTRA_ਢੇਰ_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extra_ਮਾਪ = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            ਜੇ (ut->pExtra == ਖਾਲੀ_ਸੂਚਕ) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            } ਨਹੀਂਤਾਂ {
                ut->extra_ਮਾਪ = extraSpace;
                ut->flags |= UTEXT_EXTRA_ਢੇਰ_ALLOCATED;
            }
        }
    }
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
        ut->flags |= UTEXT_OPEN;

        // Initialize all remaining fields of the UText.
        //
        ut->context             = ਖਾਲੀ_ਸੂਚਕ;
        ut->chunkContents       = ਖਾਲੀ_ਸੂਚਕ;
        ut->p                   = ਖਾਲੀ_ਸੂਚਕ;
        ut->q                   = ਖਾਲੀ_ਸੂਚਕ;
        ut->r                   = ਖਾਲੀ_ਸੂਚਕ;
        ut->a                   = 0;
        ut->b                   = 0;
        ut->c                   = 0;
        ut->chunk_ਵਿੱਥ         = 0;
        ut->chunk_ਲੰਮਾਈ         = 0;
        ut->chunk_Native_ਸ਼ੁਰੂ    = 0;
        ut->chunk_Native_ਹੱਦਬੰਦੀ    = 0;
        ut->native_Indexing_ਹੱਦਬੰਦੀ = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = ਖਾਲੀ_ਸੂਚਕ;
        ਜੇ (ut->pExtra!=ਖਾਲੀ_ਸੂਚਕ && ut->extra_ਮਾਪ>0)
            uprv_memset(ut->pExtra, 0, ut->extra_ਮਾਪ);

    }
    ਵਾਪਸ ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    ਜੇ (ut==ਖਾਲੀ_ਸੂਚਕ ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        ਵਾਪਸ ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    ਜੇ (ut->pFuncs->close != ਖਾਲੀ_ਸੂਚਕ) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    ਜੇ (ut->flags & UTEXT_EXTRA_ਢੇਰ_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = ਖਾਲੀ_ਸੂਚਕ;
        ut->flags &= ~UTEXT_EXTRA_ਢੇਰ_ALLOCATED;
        ut->extra_ਮਾਪ = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (ut->flags & UTEXT_ਢੇਰ_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
ਸਥਿਰ ਖਾਲੀ
invalidateChunk(UText *ut) {
    ut->chunk_ਲੰਮਾਈ = 0;
    ut->chunk_Native_ਹੱਦਬੰਦੀ = 0;
    ut->chunk_Native_ਸ਼ੁਰੂ = 0;
    ut->chunk_ਵਿੱਥ = 0;
    ut->native_Indexing_ਹੱਦਬੰਦੀ = 0;
}

//
// pinIndex        Do range pinning on a native index parameter.
//                 64 bit pinning is done in place.
//                 32 bit truncated result is returned as a convenience for
//                        use in providers that don't need 64 bits.
ਸਥਿਰ int32_t
pin_ਸੂਚਕਾਂਕ(int64_t &ਸੂਚਕਾਂਕ, int64_t ਹੱਦਬੰਦੀ) {
    ਜੇ (ਸੂਚਕਾਂਕ<0) {
        ਸੂਚਕਾਂਕ = 0;
    } ਨਹੀਂਤਾਂ ਜੇ (ਸੂਚਕਾਂਕ > ਹੱਦਬੰਦੀ) {
        ਸੂਚਕਾਂਕ = ਹੱਦਬੰਦੀ;
    }
    ਵਾਪਸ static_cast<int32_t>(ਸੂਚਕਾਂਕ);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
ਸਥਿਰ ਖਾਲੀ adjust_ਸੂਚਕ(UText *dest, ਅਚਲ ਖਾਲੀ **destPtr, ਅਚਲ UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    ਅੱਖਰ  *dptr = (ਅੱਖਰ *)*destPtr;
    ਅੱਖਰ  *dUText = (ਅੱਖਰ *)dest;
    ਅੱਖਰ  *sUText = (ਅੱਖਰ *)src;

    ਜੇ (dptr >= (ਅੱਖਰ *)src->pExtra && dptr < ((ਅੱਖਰ*)src->pExtra)+src->extra_ਮਾਪ) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((ਅੱਖਰ *)dest->pExtra) + (dptr - (ਅੱਖਰ *)src->pExtra);
    } ਨਹੀਂਤਾਂ ਜੇ (dptr>=sUText && dptr < sUText+src->ਮਾਪ_ਦਾ_Struct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
ਸਥਿਰ UText * U_CALLCONV
shallow_ਪਾਠ_Clone(UText * dest, ਅਚਲ UText * src, UError_ਕੋਡ * ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    int32_t  src_Extra_ਮਾਪ = src->extra_ਮਾਪ;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_ਤਿਆਰੀ(dest, src_Extra_ਮਾਪ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    ਖਾਲੀ *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    ਪੂਰਨ ਮਾਪ_ਮੰਜ਼ਿਲ_Copy = src->ਮਾਪ_ਦਾ_Struct;
    ਜੇ (ਮਾਪ_ਮੰਜ਼ਿਲ_Copy > dest->ਮਾਪ_ਦਾ_Struct) {
        ਮਾਪ_ਮੰਜ਼ਿਲ_Copy = dest->ਮਾਪ_ਦਾ_Struct;
    }
    uprv_memcpy(dest, src, ਮਾਪ_ਮੰਜ਼ਿਲ_Copy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    ਜੇ (src_Extra_ਮਾਪ > 0) {
        uprv_memcpy(dest->pExtra, src->pExtra, src_Extra_ਮਾਪ);
    }

    //
    // Relocate any pointers in the target that refer to the UText itself
    //   to point to the cloned copy rather than the original source.
    //
    adjust_ਸੂਚਕ(dest, &dest->context, src);
    adjust_ਸੂਚਕ(dest, &dest->p, src);
    adjust_ਸੂਚਕ(dest, &dest->q, src);
    adjust_ਸੂਚਕ(dest, &dest->r, src);
    adjust_ਸੂਚਕ(dest, (ਅਚਲ ਖਾਲੀ **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ);

    ਵਾਪਸ dest;
}


U_CDECL_ਸਿਰਾ



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
ਗਿਣਤੀ { UTF8_ਪਾਠ_CHUNK_ਮਾਪ=32 };

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

ਬਣਤਰ UTF8Buf {
    int32_t   buf_Native_ਸ਼ੁਰੂ;                        // Native index of first char in char16_t buf
    int32_t   buf_Native_ਹੱਦਬੰਦੀ;                        // Native index following last char in buf.
    int32_t   buf_ਸ਼ੁਰੂ_Idx;                           // First filled position in buf.
    int32_t   buf_ਹੱਦਬੰਦੀ_Idx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_ਪਾਠ_CHUNK_ਮਾਪ+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   map_ਮੰਜ਼ਿਲ_Native[UTF8_ਪਾਠ_CHUNK_ਮਾਪ+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   map_ਮੰਜ਼ਿਲ_UChars[UTF8_ਪਾਠ_CHUNK_ਮਾਪ*3+6]; // Map native offset from bufNativeStart to
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
ਸਥਿਰ int64_t U_CALLCONV
utf8_ਪਾਠ_ਲੰਮਾਈ(UText *ut) {
    ਜੇ (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        ਅਚਲ ਅੱਖਰ *r = (ਅਚਲ ਅੱਖਰ *)ut->context + ut->c;
        ਜਦੋਂ (*r != 0) {
            r++;
        }
        ਜੇ ((r - (ਅਚਲ ਅੱਖਰ *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (ਅਚਲ ਅੱਖਰ *)ut->context);
        } ਨਹੀਂਤਾਂ {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE);
    }
    ਵਾਪਸ ut->b;
}






ਸਥਿਰ UBool U_CALLCONV
utf8_ਪਾਠ_Access(UText *ut, int64_t ਸੂਚਕਾਂਕ, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    ਅਚਲ uint8_t *s8=(ਅਚਲ uint8_t *)ut->context;
    UTF8Buf *u8b = ਖਾਲੀ_ਸੂਚਕ;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)ਸੂਚਕਾਂਕ;     // Requested index, trimmed to 32 bits.
    int32_t  map_ਸੂਚਕਾਂਕ = 0;
    ਜੇ (ਸੂਚਕਾਂਕ<0) {
        ix=0;
    } ਨਹੀਂਤਾਂ ਜੇ (ਸੂਚਕਾਂਕ > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    ਜੇ (ix>length) {
        ਜੇ (length>=0) {
            ix=length;
        } ਨਹੀਂਤਾਂ ਜੇ (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            ਜਦੋਂ (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            ਜੇ (s8[ut->c] == 0) {
                // We just found the actual length of the string.
                //  Trim the requested index back to that.
                ix     = ut->c;
                ut->b  = ut->c;
                length = ut->c;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    ਜੇ (forward) {
        ਜੇ (ix==ut->chunk_Native_ਹੱਦਬੰਦੀ) {
            // Check for normal sequential iteration cases first.
            ਜੇ (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunk_ਵਿੱਥ = ut->chunk_ਲੰਮਾਈ;
                ਵਾਪਸ ਝੂਠ;
            } ਨਹੀਂਤਾਂ {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                ਜੇ (ix>=altB->buf_Native_ਸ਼ੁਰੂ && ix<altB->buf_Native_ਹੱਦਬੰਦੀ) {
                    ਜਾਓ ਅਦਲਾ_ਬਦਲੀ_Buffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            ਜੇ (ix>=u8b->buf_Native_ਸ਼ੁਰੂ && ix<u8b->buf_Native_ਹੱਦਬੰਦੀ) {
                // Requested index is in the other buffer.
                ਜਾਓ ਅਦਲਾ_ਬਦਲੀ_Buffers;
            }
            ਜੇ (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                ਜੇ (ix == ut->chunk_Native_ਹੱਦਬੰਦੀ) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunk_ਵਿੱਥ = ut->chunk_ਲੰਮਾਈ;
                    ਵਾਪਸ ਝੂਠ;
                }
                ਜੇ (ix == u8b->buf_Native_ਹੱਦਬੰਦੀ) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    ਜਾਓ ਅਦਲਾ_ਬਦਲੀ_Buffers_And_Fail;
                }

                // Neither existing buffer extends to the end of the string.
                ਜਾਓ make_Stub_ਬਫਰ;
            }

            ਜੇ (ix<ut->chunk_Native_ਸ਼ੁਰੂ || ix>=ut->chunk_Native_ਹੱਦਬੰਦੀ) {
                // Requested index is in neither buffer.
                ਜਾਓ fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            map_ਸੂਚਕਾਂਕ = ix - u8b->ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ;
            U_ASSERT(map_ਸੂਚਕਾਂਕ < (int32_t)ਆਕਾਰ(UTF8Buf::map_ਮੰਜ਼ਿਲ_UChars));
            ut->chunk_ਵਿੱਥ = u8b->map_ਮੰਜ਼ਿਲ_UChars[map_ਸੂਚਕਾਂਕ] - u8b->buf_ਸ਼ੁਰੂ_Idx;
            ਵਾਪਸ ਸੱਚ;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    ਜੇ (ix==ut->chunk_Native_ਸ਼ੁਰੂ) {
        // Check for normal sequential iteration cases first.
        ਜੇ (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunk_ਵਿੱਥ = 0;
            ਵਾਪਸ ਝੂਠ;
        } ਨਹੀਂਤਾਂ {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            ਜੇ (ix>altB->buf_Native_ਸ਼ੁਰੂ && ix<=altB->buf_Native_ਹੱਦਬੰਦੀ) {
                ਜਾਓ ਅਦਲਾ_ਬਦਲੀ_Buffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    ਜੇ (ix>u8b->buf_Native_ਸ਼ੁਰੂ && ix<=u8b->buf_Native_ਹੱਦਬੰਦੀ) {
        // Requested index is in the other buffer.
        ਜਾਓ ਅਦਲਾ_ਬਦਲੀ_Buffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    ਜੇ (ix==0) {
        ਜੇ (u8b->buf_Native_ਸ਼ੁਰੂ==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            ਜਾਓ ਅਦਲਾ_ਬਦਲੀ_Buffers_And_Fail;
        } ਨਹੀਂਤਾਂ {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            ਜਾਓ make_Stub_ਬਫਰ;
        }
    }

    ਜੇ (ix<=ut->chunk_Native_ਸ਼ੁਰੂ || ix>ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        // Requested index is in neither buffer.
        ਜਾਓ fill_ਉਲਟ;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    map_ਸੂਚਕਾਂਕ = ix - u8b->ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ;
    ut->chunk_ਵਿੱਥ = u8b->map_ਮੰਜ਼ਿਲ_UChars[map_ਸੂਚਕਾਂਕ] - u8b->buf_ਸ਼ੁਰੂ_Idx;
    ਜੇ (ut->chunk_ਵਿੱਥ==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਸੱਚ;
    }



ਅਦਲਾ_ਬਦਲੀ_Buffers:
    //  The alternate buffer (ut->q) has the string data that was requested.
    //  Swap the primary and alternate buffers, and set the
    //   chunk index into the new primary buffer.
    {
        u8b   = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b;
        ut->chunkContents       = &u8b->buf[u8b->buf_ਸ਼ੁਰੂ_Idx];
        ut->chunk_ਲੰਮਾਈ         = u8b->buf_ਹੱਦਬੰਦੀ_Idx - u8b->buf_ਸ਼ੁਰੂ_Idx;
        ut->chunk_Native_ਸ਼ੁਰੂ    = u8b->buf_Native_ਸ਼ੁਰੂ;
        ut->chunk_Native_ਹੱਦਬੰਦੀ    = u8b->buf_Native_ਹੱਦਬੰਦੀ;
        ut->native_Indexing_ਹੱਦਬੰਦੀ = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->buf_Native_ਸ਼ੁਰੂ);
        U_ASSERT(ix<=u8b->buf_Native_ਹੱਦਬੰਦੀ);
        map_ਸੂਚਕਾਂਕ = ix - u8b->ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ;
        U_ASSERT(map_ਸੂਚਕਾਂਕ>=0);
        U_ASSERT(map_ਸੂਚਕਾਂਕ<(int32_t)ਆਕਾਰ(u8b->map_ਮੰਜ਼ਿਲ_UChars));
        ut->chunk_ਵਿੱਥ = u8b->map_ਮੰਜ਼ਿਲ_UChars[map_ਸੂਚਕਾਂਕ] - u8b->buf_ਸ਼ੁਰੂ_Idx;

        ਵਾਪਸ ਸੱਚ;
    }


 ਅਦਲਾ_ਬਦਲੀ_Buffers_And_Fail:
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
    ut->chunkContents       = &u8b->buf[u8b->buf_ਸ਼ੁਰੂ_Idx];
    ut->chunk_ਲੰਮਾਈ         = u8b->buf_ਹੱਦਬੰਦੀ_Idx - u8b->buf_ਸ਼ੁਰੂ_Idx;
    ut->chunk_Native_ਸ਼ੁਰੂ    = u8b->buf_Native_ਸ਼ੁਰੂ;
    ut->chunk_Native_ਹੱਦਬੰਦੀ    = u8b->buf_Native_ਹੱਦਬੰਦੀ;
    ut->native_Indexing_ਹੱਦਬੰਦੀ = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    ਜੇ (ix==u8b->buf_Native_ਹੱਦਬੰਦੀ) {
        ut->chunk_ਵਿੱਥ = ut->chunk_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ  {
        ut->chunk_ਵਿੱਥ = 0;
        U_ASSERT(ix == u8b->buf_Native_ਸ਼ੁਰੂ);
    }
    ਵਾਪਸ ਝੂਠ;

make_Stub_ਬਫਰ:
    //   The user has done a seek/access past the start or end
    //   of the string.  Rather than loading data that is likely
    //   to never be used, just set up a zero-length buffer at
    //   the position.
    u8b = (UTF8Buf *)ut->q;
    u8b->buf_Native_ਸ਼ੁਰੂ   = ix;
    u8b->buf_Native_ਹੱਦਬੰਦੀ   = ix;
    u8b->buf_ਸ਼ੁਰੂ_Idx      = 0;
    u8b->buf_ਹੱਦਬੰਦੀ_Idx      = 0;
    u8b->bufNILimit       = 0;
    u8b->ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ = ix;
    u8b->map_ਮੰਜ਼ਿਲ_Native[0]   = 0;
    u8b->map_ਮੰਜ਼ਿਲ_UChars[0]   = 0;
    ਜਾਓ ਅਦਲਾ_ਬਦਲੀ_Buffers_And_Fail;



fillForward:
    {
        // Move the incoming index to a code point boundary.
        U8_ਸੈੱਟ_CP_ਸ਼ੁਰੂ(s8, 0, ix);

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_ਅਦਲਾ_ਬਦਲੀ = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_ਅਦਲਾ_ਬਦਲੀ;

        int32_t strLen = ut->b;
        UBool   nulTerminated = ਝੂਠ;
        ਜੇ (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = ਸੱਚ;
        }

        char16_t   *buf = u8b_ਅਦਲਾ_ਬਦਲੀ->buf;
        uint8_t *map_ਮੰਜ਼ਿਲ_Native  = u8b_ਅਦਲਾ_ਬਦਲੀ->map_ਮੰਜ਼ਿਲ_Native;
        uint8_t *map_ਮੰਜ਼ਿਲ_UChars  = u8b_ਅਦਲਾ_ਬਦਲੀ->map_ਮੰਜ਼ਿਲ_UChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    ਵੇਖਿਆ_Non_Ascii = ਝੂਠ;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        ਜਦੋਂ (destIx<UTF8_ਪਾਠ_CHUNK_ਮਾਪ) {
            c = s8[srcIx];
            ਜੇ (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                map_ਮੰਜ਼ਿਲ_Native[destIx]    = (uint8_t)(srcIx - ix);
                map_ਮੰਜ਼ਿਲ_UChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } ਨਹੀਂਤਾਂ {
                // General case, handle everything.
                ਜੇ (ਵੇਖਿਆ_Non_Ascii == ਝੂਠ) {
                    ਵੇਖਿਆ_Non_Ascii = ਸੱਚ;
                    u8b_ਅਦਲਾ_ਬਦਲੀ->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_ਅਗਲਾ_OR_FFFD(s8, srcIx, strLen, c);
                ਜੇ (c==0 && nulTerminated) {
                    srcIx--;
                    ਰੋਕੋ;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                ਕਰੋ {
                    map_ਮੰਜ਼ਿਲ_Native[dIx++] = (uint8_t)(cIx - ix);
                } ਜਦੋਂ (dIx < destIx);

                ਕਰੋ {
                    map_ਮੰਜ਼ਿਲ_UChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } ਜਦੋਂ (cIx < srcIx);
            }
            ਜੇ (srcIx>=strLen) {
                ਰੋਕੋ;
            }

        }

        //  store Native <--> Chunk Map entries for the end of the buffer.
        //    There is no actual character here, but the index position is valid.
        map_ਮੰਜ਼ਿਲ_Native[destIx]     = (uint8_t)(srcIx - ix);
        map_ਮੰਜ਼ਿਲ_UChars[srcIx - ix] = (uint8_t)destIx;

        //  fill in Buffer descriptor
        u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਸ਼ੁਰੂ     = ix;
        u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਹੱਦਬੰਦੀ     = srcIx;
        u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਸ਼ੁਰੂ_Idx        = 0;
        u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਹੱਦਬੰਦੀ_Idx        = destIx;
        ਜੇ (ਵੇਖਿਆ_Non_Ascii == ਝੂਠ) {
            u8b_ਅਦਲਾ_ਬਦਲੀ->bufNILimit     = destIx;
        }
        u8b_ਅਦਲਾ_ਬਦਲੀ->ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ   = u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਸ਼ੁਰੂ;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunk_ਵਿੱਥ         = 0;
        ut->chunk_ਲੰਮਾਈ         = u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਹੱਦਬੰਦੀ_Idx;
        ut->chunk_Native_ਸ਼ੁਰੂ    = u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਸ਼ੁਰੂ;
        ut->chunk_Native_ਹੱਦਬੰਦੀ    = u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਹੱਦਬੰਦੀ;
        ut->native_Indexing_ਹੱਦਬੰਦੀ = u8b_ਅਦਲਾ_ਬਦਲੀ->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        ਜੇ (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            ਜੇ (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE);
            }
        }
        ਵਾਪਸ ਸੱਚ;
    }


fill_ਉਲਟ:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        ਜੇ (ix != ut->b) {
            // Note: this function will only move the index back if it is on a trail byte
            //       and there is a preceding lead byte and the sequence from the lead 
            //       through this trail could be part of a valid UTF-8 sequence
            //       Otherwise the index remains unchanged.
            U8_ਸੈੱਟ_CP_ਸ਼ੁਰੂ(s8, 0, ix);
        }

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_ਅਦਲਾ_ਬਦਲੀ = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_ਅਦਲਾ_ਬਦਲੀ;

        char16_t   *buf = u8b_ਅਦਲਾ_ਬਦਲੀ->buf;
        uint8_t *map_ਮੰਜ਼ਿਲ_Native = u8b_ਅਦਲਾ_ਬਦਲੀ->map_ਮੰਜ਼ਿਲ_Native;
        uint8_t *map_ਮੰਜ਼ਿਲ_UChars = u8b_ਅਦਲਾ_ਬਦਲੀ->map_ਮੰਜ਼ਿਲ_UChars;
        int32_t  ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ = ix - ਆਕਾਰ(UTF8Buf::map_ਮੰਜ਼ਿਲ_UChars) + 1;
        // Note that toUCharsMapStart can be negative. Happens when the remaining
        // text from current position to the beginning is less than the buffer size.
        // + 1 because mapToUChars must have a slot at the end for the bufNativeLimit entry.
        int32_t  destIx = UTF8_ਪਾਠ_CHUNK_ਮਾਪ+2;   // Start in the overflow region
                                                    //   at end of buffer to leave room
                                                    //   for a surrogate pair at the
                                                    //   buffer start.
        int32_t  srcIx  = ix;
        int32_t  bufNILimit = destIx;
        UChar32   c;

        // Map to/from Native Indexes, fill in for the position at the end of
        //   the buffer.
        //
        map_ਮੰਜ਼ਿਲ_Native[destIx] = (uint8_t)(srcIx - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ);
        map_ਮੰਜ਼ਿਲ_UChars[srcIx - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ] = (uint8_t)destIx;

        // Fill the chunk buffer
        // Work backwards, filling from the end of the buffer towards the front.
        //
        ਜਦੋਂ (destIx>2 && (srcIx - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            ਜੇ (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ <= srcIx);
                map_ਮੰਜ਼ਿਲ_UChars[srcIx - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ] = (uint8_t)destIx;
                map_ਮੰਜ਼ਿਲ_Native[destIx] = (uint8_t)(srcIx - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ);
            } ਨਹੀਂਤਾਂ {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_prev_Char_ਸੁਰੱਖਿਅਤ_Body(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                ਜੇ (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    map_ਮੰਜ਼ਿਲ_Native[destIx] = (uint8_t)(srcIx - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ);
                } ਨਹੀਂਤਾਂ {
                    buf[destIx]         = U16_TRAIL(c);
                    map_ਮੰਜ਼ਿਲ_Native[destIx] = (uint8_t)(srcIx - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ);
                    buf[--destIx]       = U16_LEAD(c);
                    map_ਮੰਜ਼ਿਲ_Native[destIx] = (uint8_t)(srcIx - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ);
                }

                // Fill in the map from native indexes to UChars buf index.
                ਕਰੋ {
                    map_ਮੰਜ਼ਿਲ_UChars[sIx-- - ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ] = (uint8_t)destIx;
                } ਜਦੋਂ (sIx >= srcIx);
                U_ASSERT(ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ <= (srcIx+1));

                // Set native indexing limit to be the current position.
                //   We are processing a non-ascii, non-native-indexing char now;
                //     the limit will be here if the rest of the chars to be
                //     added to this buffer are ascii.
                bufNILimit = destIx;
            }
        }
        u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਸ਼ੁਰੂ     = srcIx;
        u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਹੱਦਬੰਦੀ     = ix;
        u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਸ਼ੁਰੂ_Idx        = destIx;
        u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਹੱਦਬੰਦੀ_Idx        = UTF8_ਪਾਠ_CHUNK_ਮਾਪ+2;
        u8b_ਅਦਲਾ_ਬਦਲੀ->bufNILimit         = bufNILimit - u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਸ਼ੁਰੂ_Idx;
        u8b_ਅਦਲਾ_ਬਦਲੀ->ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ   = ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ;

        ut->chunkContents       = &buf[u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਸ਼ੁਰੂ_Idx];
        ut->chunk_ਲੰਮਾਈ         = u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਹੱਦਬੰਦੀ_Idx - u8b_ਅਦਲਾ_ਬਦਲੀ->buf_ਸ਼ੁਰੂ_Idx;
        ut->chunk_ਵਿੱਥ         = ut->chunk_ਲੰਮਾਈ;
        ut->chunk_Native_ਸ਼ੁਰੂ    = u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਸ਼ੁਰੂ;
        ut->chunk_Native_ਹੱਦਬੰਦੀ    = u8b_ਅਦਲਾ_ਬਦਲੀ->buf_Native_ਹੱਦਬੰਦੀ;
        ut->native_Indexing_ਹੱਦਬੰਦੀ = u8b_ਅਦਲਾ_ਬਦਲੀ->bufNILimit;
        ਵਾਪਸ ਸੱਚ;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
ਸਥਿਰ char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t dest_ਸਮਰੱਥਾ,
              int32_t *p_Dest_ਲੰਮਾਈ,
              ਅਚਲ ਅੱਖਰ* src,
              int32_t src_ਲੰਮਾਈ,        // required.  NUL terminated not supported.
              UError_ਕੋਡ *p_Error_ਕੋਡ
              )
{

    char16_t *pDest = dest;
    char16_t *p_Dest_ਹੱਦਬੰਦੀ = (dest!=ਖਾਲੀ_ਸੂਚਕ)?(dest+dest_ਸਮਰੱਥਾ):ਖਾਲੀ_ਸੂਚਕ;
    UChar32 ch=0;
    int32_t ਸੂਚਕਾਂਕ = 0;
    int32_t req_ਲੰਮਾਈ = 0;
    uint8_t* pSrc = (uint8_t*) src;


    ਜਦੋਂ((ਸੂਚਕਾਂਕ < src_ਲੰਮਾਈ)&&(pDest<p_Dest_ਹੱਦਬੰਦੀ)){
        ch = pSrc[ਸੂਚਕਾਂਕ++];
        ਜੇ(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }ਨਹੀਂਤਾਂ{
            ch=utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body(pSrc, &ਸੂਚਕਾਂਕ, src_ਲੰਮਾਈ, ch, -3);
            ਜੇ(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }ਨਹੀਂਤਾਂ{
                *(pDest++)=U16_LEAD(ch);
                ਜੇ(pDest<p_Dest_ਹੱਦਬੰਦੀ){
                    *(pDest++)=U16_TRAIL(ch);
                }ਨਹੀਂਤਾਂ{
                    req_ਲੰਮਾਈ++;
                    ਰੋਕੋ;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    ਜਦੋਂ(ਸੂਚਕਾਂਕ < src_ਲੰਮਾਈ){
        ch = pSrc[ਸੂਚਕਾਂਕ++];
        ਜੇ(ch <= 0x7f){
            req_ਲੰਮਾਈ++;
        }ਨਹੀਂਤਾਂ{
            ch=utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body(pSrc, &ਸੂਚਕਾਂਕ, src_ਲੰਮਾਈ, ch, -3);
            req_ਲੰਮਾਈ+=U16_ਲੰਮਾਈ(ch);
        }
    }

    req_ਲੰਮਾਈ+=(int32_t)(pDest - dest);

    ਜੇ(p_Dest_ਲੰਮਾਈ){
        *p_Dest_ਲੰਮਾਈ = req_ਲੰਮਾਈ;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ਸਮਰੱਥਾ,req_ਲੰਮਾਈ,p_Error_ਕੋਡ);

    ਵਾਪਸ dest;
}



ਸਥਿਰ int32_t U_CALLCONV
utf8_ਪਾਠ_ਕੱਢੋ(UText *ut,
                int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
                char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ, length);
    int32_t  limit32 = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, length);

    ਜੇ(start32>limit32) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    ਅਚਲ uint8_t *buf = (ਅਚਲ uint8_t *)ut->context;
    ਪੂਰਨ i;
    ਜੇ (start32 < ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        ਲਈ (i=0; i<3; i++) {
            ਜੇ (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                ਰੋਕੋ;
            }
            start32--;
        }
    }

    ਜੇ (limit32 < ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        ਲਈ (i=0; i<3; i++) {
            ਜੇ (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                ਰੋਕੋ;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t dest_ਲੰਮਾਈ=0;
    utext_strFromUTF8(dest, dest_ਸਮਰੱਥਾ, &dest_ਲੰਮਾਈ,
                    (ਅਚਲ ਅੱਖਰ *)ut->context+start32, limit32-start32,
                    p_Error_ਕੋਡ);
    utf8_ਪਾਠ_Access(ut, limit32, ਸੱਚ);
    ਵਾਪਸ dest_ਲੰਮਾਈ;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
ਸਥਿਰ int64_t U_CALLCONV
utf8_ਪਾਠ_Map_ਵਿੱਥ_ਮੰਜ਼ਿਲ_Native(ਅਚਲ UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunk_ਵਿੱਥ>ut->native_Indexing_ਹੱਦਬੰਦੀ && ut->chunk_ਵਿੱਥ<=ut->chunk_ਲੰਮਾਈ);
    int32_t native_ਵਿੱਥ = u8b->map_ਮੰਜ਼ਿਲ_Native[ut->chunk_ਵਿੱਥ + u8b->buf_ਸ਼ੁਰੂ_Idx] + u8b->ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ;
    U_ASSERT(native_ਵਿੱਥ >= ut->chunk_Native_ਸ਼ੁਰੂ && native_ਵਿੱਥ <= ut->chunk_Native_ਹੱਦਬੰਦੀ);
    ਵਾਪਸ native_ਵਿੱਥ;
}

//
// Map a native index to the corresponding chunk offset
//
ਸਥਿਰ int32_t U_CALLCONV
utf8_ਪਾਠ_Map_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_UTF16(ਅਚਲ UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t ਸੂਚਕਾਂਕ = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ਸੂਚਕਾਂਕ>=ut->chunk_Native_ਸ਼ੁਰੂ+ut->native_Indexing_ਹੱਦਬੰਦੀ);
    U_ASSERT(ਸੂਚਕਾਂਕ<=ut->chunk_Native_ਹੱਦਬੰਦੀ);
    int32_t map_ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ - u8b->ਮੰਜ਼ਿਲ_UChars_Map_ਸ਼ੁਰੂ;
    U_ASSERT(map_ਸੂਚਕਾਂਕ < (int32_t)ਆਕਾਰ(UTF8Buf::map_ਮੰਜ਼ਿਲ_UChars));
    int32_t ਵਿੱਥ = u8b->map_ਮੰਜ਼ਿਲ_UChars[map_ਸੂਚਕਾਂਕ] - u8b->buf_ਸ਼ੁਰੂ_Idx;
    U_ASSERT(ਵਿੱਥ>=0 && ਵਿੱਥ<=ut->chunk_ਲੰਮਾਈ);
    ਵਾਪਸ ਵਿੱਥ;
}

ਸਥਿਰ UText * U_CALLCONV
utf8_ਪਾਠ_Clone(UText *dest, ਅਚਲ UText *src, UBool deep, UError_ਕੋਡ *ਸਥਿਤੀ)
{
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_ਪਾਠ_Clone(dest, src, ਸਥਿਤੀ);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //
    // TODO:  There is an issue with using utext_nativeLength().
    //        That function is non-const in cases where the input was NUL terminated
    //          and the length has not yet been determined.
    //        This function (clone()) is const.
    //        There potentially a thread safety issue lurking here.
    //
    ਜੇ (deep && U_SUCCESS(*ਸਥਿਤੀ)) {
        int32_t  len = (int32_t)utext_native_ਲੰਮਾਈ((UText *)src);
        ਅੱਖਰ *copyStr = (ਅੱਖਰ *)uprv_malloc(len+1);
        ਜੇ (copyStr == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ);
        }
    }
    ਵਾਪਸ dest;
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
utf8_ਪਾਠ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ਜੇ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ)) {
        ਅੱਖਰ *s = (ਅੱਖਰ *)ut->context;
        uprv_free(s);
        ut->context = ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CDECL_ਸਿਰਾ


ਸਥਿਰ ਅਚਲ ਬਣਤਰ UTextFuncs utf8Funcs =
{
    ਆਕਾਰ(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8_ਪਾਠ_Clone,
    utf8_ਪਾਠ_ਲੰਮਾਈ,
    utf8_ਪਾਠ_Access,
    utf8_ਪਾਠ_ਕੱਢੋ,
    ਖਾਲੀ_ਸੂਚਕ,                /* replace*/
    ਖਾਲੀ_ਸੂਚਕ,                /* copy   */
    utf8_ਪਾਠ_Map_ਵਿੱਥ_ਮੰਜ਼ਿਲ_Native,
    utf8_ਪਾਠ_Map_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_UTF16,
    utf8_ਪਾਠ_Close,
    ਖਾਲੀ_ਸੂਚਕ,                // spare 1
    ਖਾਲੀ_ਸੂਚਕ,                // spare 2
    ਖਾਲੀ_ਸੂਚਕ                 // spare 3
};


ਸਥਿਰ ਅਚਲ ਅੱਖਰ g_ਖਾਲੀ_ਥਾਵਾਂ_String[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               ਅਚਲ ਅੱਖਰ* s U_LIFETIME_BOUND,
               int64_t length,
               UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ && length==0) {
        s = g_ਖਾਲੀ_ਥਾਵਾਂ_String;
    }

    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ || length<-1 || length>INT32_MAX) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ut = utext_ਤਿਆਰੀ(ut, ਆਕਾਰ(UTF8Buf) * 2, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    ਜੇ (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (ਅੱਖਰ *)ut->pExtra + ਆਕਾਰ(UTF8Buf);
    ਵਾਪਸ ut;

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
ਗਿਣਤੀ { REP_ਪਾਠ_CHUNK_ਮਾਪ=10 };

ਬਣਤਰ ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_ਪਾਠ_CHUNK_ਮਾਪ+1];
};


U_CDECL_BEGIN

ਸਥਿਰ UText * U_CALLCONV
rep_ਪਾਠ_Clone(UText *dest, ਅਚਲ UText *src, UBool deep, UError_ਕੋਡ *ਸਥਿਤੀ) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_ਪਾਠ_Clone(dest, src, ਸਥਿਤੀ);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    ਜੇ (deep && U_SUCCESS(*ਸਥਿਤੀ)) {
        ਅਚਲ Replaceable *replSrc = (ਅਚਲ Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ਵਾਪਸ dest;
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
rep_ਪਾਠ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ਜੇ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ)) {
        Replaceable *rep = (Replaceable *)ut->context;
        ਮਿਟਾਓ rep;
        ut->context = ਖਾਲੀ_ਸੂਚਕ;
    }
}


ਸਥਿਰ int64_t U_CALLCONV
rep_ਪਾਠ_ਲੰਮਾਈ(UText *ut) {
    ਅਚਲ Replaceable *replSrc = (ਅਚਲ Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    ਵਾਪਸ len;
}


ਸਥਿਰ UBool U_CALLCONV
rep_ਪਾਠ_Access(UText *ut, int64_t ਸੂਚਕਾਂਕ, UBool forward) {
    ਅਚਲ Replaceable *rep=(ਅਚਲ Replaceable *)ut->context;
    int32_t length=rep->length();   // Full length of the input text (bigger than a chunk)

    // clip the requested index to the limits of the text.
    int32_t index32 = pin_ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ, length);
    U_ASSERT(ਸੂਚਕਾਂਕ<=INT32_MAX);


    /*
     * Compute start/limit boundaries around index, for a segment of text
     * to be extracted.
     * To allow for the possibility that our user gave an index to the trailing
     * half of a surrogate pair, we must request one extra preceding char16_t when
     * going in the forward direction.  This will ensure that the buffer has the
     * entire code point at the specified index.
     */
    ਜੇ(forward) {

        ਜੇ (index32>=ut->chunk_Native_ਸ਼ੁਰੂ && index32<ut->chunk_Native_ਹੱਦਬੰਦੀ) {
            // Buffer already contains the requested position.
            ut->chunk_ਵਿੱਥ = (int32_t)(ਸੂਚਕਾਂਕ - ut->chunk_Native_ਸ਼ੁਰੂ);
            ਵਾਪਸ ਸੱਚ;
        }
        ਜੇ (index32>=length && ut->chunk_Native_ਹੱਦਬੰਦੀ==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunk_ਵਿੱਥ = length - (int32_t)ut->chunk_Native_ਸ਼ੁਰੂ;
            ਵਾਪਸ ਝੂਠ;
        }

        ut->chunk_Native_ਹੱਦਬੰਦੀ = ਸੂਚਕਾਂਕ + REP_ਪਾਠ_CHUNK_ਮਾਪ - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        ਜੇ(ut->chunk_Native_ਹੱਦਬੰਦੀ > length) {
            ut->chunk_Native_ਹੱਦਬੰਦੀ = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunk_Native_ਸ਼ੁਰੂ = ut->chunk_Native_ਹੱਦਬੰਦੀ - REP_ਪਾਠ_CHUNK_ਮਾਪ;
        ਜੇ(ut->chunk_Native_ਸ਼ੁਰੂ < 0) {
            ut->chunk_Native_ਸ਼ੁਰੂ = 0;
        }
    } ਨਹੀਂਤਾਂ {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        ਜੇ (index32>ut->chunk_Native_ਸ਼ੁਰੂ && index32<=ut->chunk_Native_ਹੱਦਬੰਦੀ) {
            // Requested position already in buffer.
            ut->chunk_ਵਿੱਥ = index32 - (int32_t)ut->chunk_Native_ਸ਼ੁਰੂ;
            ਵਾਪਸ ਸੱਚ;
        }
        ਜੇ (index32==0 && ut->chunk_Native_ਸ਼ੁਰੂ==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunk_ਵਿੱਥ = 0;
            ਵਾਪਸ ਝੂਠ;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunk_Native_ਸ਼ੁਰੂ = index32 + 1 - REP_ਪਾਠ_CHUNK_ਮਾਪ;
        ਜੇ (ut->chunk_Native_ਸ਼ੁਰੂ < 0) {
            ut->chunk_Native_ਸ਼ੁਰੂ = 0;
        }

        ut->chunk_Native_ਹੱਦਬੰਦੀ = index32 + 1;
        ਜੇ (ut->chunk_Native_ਹੱਦਬੰਦੀ > length) {
            ut->chunk_Native_ਹੱਦਬੰਦੀ = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString ਬਫਰ(ex->s, 0 /*buffer length*/, REP_ਪਾਠ_CHUNK_ਮਾਪ /*buffer capacity*/);
    rep->ਕੱਢੋ_Between((int32_t)ut->chunk_Native_ਸ਼ੁਰੂ, (int32_t)ut->chunk_Native_ਹੱਦਬੰਦੀ, ਬਫਰ);

    ut->chunkContents  = ex->s;
    ut->chunk_ਲੰਮਾਈ    = (int32_t)(ut->chunk_Native_ਹੱਦਬੰਦੀ - ut->chunk_Native_ਸ਼ੁਰੂ);
    ut->chunk_ਵਿੱਥ    = (int32_t)(index32 - ut->chunk_Native_ਸ਼ੁਰੂ);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    ਜੇ (ut->chunk_Native_ਹੱਦਬੰਦੀ < length &&
        U16_IS_LEAD(ex->s[ut->chunk_ਲੰਮਾਈ-1])) {
            ut->chunk_ਲੰਮਾਈ--;
            ut->chunk_Native_ਹੱਦਬੰਦੀ--;
            ਜੇ (ut->chunk_ਵਿੱਥ > ut->chunk_ਲੰਮਾਈ) {
                ut->chunk_ਵਿੱਥ = ut->chunk_ਲੰਮਾਈ;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    ਜੇ(ut->chunk_Native_ਸ਼ੁਰੂ>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunk_Native_ਸ਼ੁਰੂ);
        --(ut->chunk_ਲੰਮਾਈ);
        --(ut->chunk_ਵਿੱਥ);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_ਸੈੱਟ_CP_ਸ਼ੁਰੂ(ut->chunkContents, 0, ut->chunk_ਵਿੱਥ);

    // Use fast indexing for get/setNativeIndex()
    ut->native_Indexing_ਹੱਦਬੰਦੀ = ut->chunk_ਲੰਮਾਈ;

    ਵਾਪਸ ਸੱਚ;
}



ਸਥਿਰ int32_t U_CALLCONV
rep_ਪਾਠ_ਕੱਢੋ(UText *ut,
               int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
               char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
               UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਅਚਲ Replaceable *rep=(ਅਚਲ Replaceable *)ut->context;
    int32_t  length=rep->length();

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0)) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਜੇ(ਸ਼ੁਰੂ>ਹੱਦਬੰਦੀ) {
        *ਸਥਿਤੀ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    int32_t  start32 = pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ, length);
    int32_t  limit32 = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, length);

    // adjust start, limit if they point to trail half of surrogates
    ਜੇ (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    ਜੇ (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    ਜੇ(length>dest_ਸਮਰੱਥਾ) {
        limit32 = start32 + dest_ਸਮਰੱਥਾ;
    }
    UnicodeString ਬਫਰ(dest, 0, dest_ਸਮਰੱਥਾ); // writable alias
    rep->ਕੱਢੋ_Between(start32, limit32, ਬਫਰ);
    rep_ਪਾਠ_Access(ut, limit32, ਸੱਚ);

    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, length, ਸਥਿਤੀ);
}

ਸਥਿਰ int32_t U_CALLCONV
rep_ਪਾਠ_Replace(UText *ut,
               int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
               ਅਚਲ char16_t *src, int32_t length,
               UError_ਕੋਡ *ਸਥਿਤੀ) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t old_ਲੰਮਾਈ;

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(src==ਖਾਲੀ_ਸੂਚਕ && length!=0) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    old_ਲੰਮਾਈ=rep->length(); // will subtract from new length
    ਜੇ(ਸ਼ੁਰੂ>ਹੱਦਬੰਦੀ ) {
        *ਸਥਿਤੀ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    int32_t start32 = pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ, old_ਲੰਮਾਈ);
    int32_t limit32 = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, old_ਲੰਮਾਈ);

    // Snap start & limit to code point boundaries.
    ਜੇ (start32<old_ਲੰਮਾਈ && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    ਜੇ (limit32<old_ਲੰਮਾਈ && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t new_ਲੰਮਾਈ = rep->length();
    int32_t ਲੰਮਾਈ_Delta = new_ਲੰਮਾਈ - old_ਲੰਮਾਈ;

    // Is the UText chunk buffer OK?
    ਜੇ (ut->chunk_Native_ਹੱਦਬੰਦੀ > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t new_ਸੂਚਕਾਂਕ_Pos = limit32 + ਲੰਮਾਈ_Delta;
    rep_ਪਾਠ_Access(ut, new_ਸੂਚਕਾਂਕ_Pos, ਸੱਚ);

    ਵਾਪਸ ਲੰਮਾਈ_Delta;
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
rep_ਪਾਠ_Copy(UText *ut,
                int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
                int64_t dest_ਸੂਚਕਾਂਕ,
                UBool move,
                UError_ਕੋਡ *ਸਥਿਤੀ)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (ਸ਼ੁਰੂ>ਹੱਦਬੰਦੀ || (ਸ਼ੁਰੂ<dest_ਸੂਚਕਾਂਕ && dest_ਸੂਚਕਾਂਕ<ਹੱਦਬੰਦੀ))
    {
        *ਸਥਿਤੀ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }

    int32_t start32     = pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ, length);
    int32_t limit32     = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, length);
    int32_t destIndex32 = pin_ਸੂਚਕਾਂਕ(dest_ਸੂਚਕਾਂਕ, length);

    // TODO:  snap input parameters to code point boundaries.

    ਜੇ(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t seg_ਲੰਮਾਈ=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        ਜੇ(destIndex32<start32) {
            start32+=seg_ਲੰਮਾਈ;
            limit32+=seg_ਲੰਮਾਈ;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } ਨਹੀਂਤਾਂ {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t ਪਹਿਲਾ_Affected_ਸੂਚਕਾਂਕ = destIndex32;
    ਜੇ (move && start32<ਪਹਿਲਾ_Affected_ਸੂਚਕਾਂਕ) {
        ਪਹਿਲਾ_Affected_ਸੂਚਕਾਂਕ = start32;
    }
    ਜੇ (ਪਹਿਲਾ_Affected_ਸੂਚਕਾਂਕ < ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  native_Iter_ਸੂਚਕਾਂਕ = destIndex32 + limit32 - start32;
    ਜੇ (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        native_Iter_ਸੂਚਕਾਂਕ = destIndex32;
    }

    // Set position, reload chunk if needed.
    rep_ਪਾਠ_Access(ut, native_Iter_ਸੂਚਕਾਂਕ, ਸੱਚ);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ UTextFuncs repFuncs =
{
    ਆਕਾਰ(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    rep_ਪਾਠ_Clone,
    rep_ਪਾਠ_ਲੰਮਾਈ,
    rep_ਪਾਠ_Access,
    rep_ਪਾਠ_ਕੱਢੋ,
    rep_ਪਾਠ_Replace,
    rep_ਪਾਠ_Copy,
    ਖਾਲੀ_ਸੂਚਕ,              // MapOffsetToNative,
    ਖਾਲੀ_ਸੂਚਕ,              // MapIndexToUTF16,
    rep_ਪਾਠ_Close,
    ਖਾਲੀ_ਸੂਚਕ,              // spare 1
    ਖਾਲੀ_ਸੂਚਕ,              // spare 2
    ਖਾਲੀ_ਸੂਚਕ               // spare 3
};


U_CAPI UText* U_EXPORT2
utext_openReplaceable(UText* ut U_LIFETIME_BOUND,
                      Replaceable* rep U_LIFETIME_BOUND,
                      UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(rep==ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ut = utext_ਤਿਆਰੀ(ut, ਆਕਾਰ(ReplExtra), ਸਥਿਤੀ);
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    ਜੇ(rep->has_Meta_ਅੰਕੜੇ()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_ਅੰਕੜੇ);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    ਵਾਪਸ ut;
}

U_CDECL_ਸਿਰਾ








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


ਸਥਿਰ UText * U_CALLCONV
unistr_ਪਾਠ_Clone(UText *dest, ਅਚਲ UText *src, UBool deep, UError_ਕੋਡ *ਸਥਿਤੀ) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_ਪਾਠ_Clone(dest, src, ਸਥਿਤੀ);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    ਜੇ (deep && U_SUCCESS(*ਸਥਿਤੀ)) {
        ਅਚਲ UnicodeString *srcString = (ਅਚਲ UnicodeString *)src->context;
        dest->context = ਨਵਾਂ UnicodeString(*srcString);
        dest->chunkContents = ((ਅਚਲ UnicodeString *)dest->context)->get_ਬਫਰ();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ਵਾਪਸ dest;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
unistr_ਪਾਠ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ਜੇ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        ਮਿਟਾਓ str;
        ut->context = ਖਾਲੀ_ਸੂਚਕ;
    }
}


ਸਥਿਰ int64_t U_CALLCONV
unistr_ਪਾਠ_ਲੰਮਾਈ(UText *t) {
    ਵਾਪਸ ((ਅਚਲ UnicodeString *)t->context)->length();
}


ਸਥਿਰ UBool U_CALLCONV
unistr_ਪਾਠ_Access(UText *ut, int64_t ਸੂਚਕਾਂਕ, UBool  forward) {
    int32_t length  = ut->chunk_ਲੰਮਾਈ;
    ut->chunk_ਵਿੱਥ = pin_ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && ਸੂਚਕਾਂਕ<length) || (!forward && ਸੂਚਕਾਂਕ>0);
    ਵਾਪਸ retVal;
}



ਸਥਿਰ int32_t U_CALLCONV
unistr_ਪਾਠ_ਕੱਢੋ(UText *t,
                  int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
                  char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UnicodeString *us=(ਅਚਲ UnicodeString *)t->context;
    int32_t length=us->length();

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਜੇ(ਸ਼ੁਰੂ<0 || ਸ਼ੁਰੂ>ਹੱਦਬੰਦੀ) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    int32_t start32 = ਸ਼ੁਰੂ<length ? us->get_Char32_ਸ਼ੁਰੂ((int32_t)ਸ਼ੁਰੂ) : length;
    int32_t limit32 = ਹੱਦਬੰਦੀ<length ? us->get_Char32_ਸ਼ੁਰੂ((int32_t)ਹੱਦਬੰਦੀ) : length;

    length=limit32-start32;
    ਜੇ (dest_ਸਮਰੱਥਾ>0 && dest!=ਖਾਲੀ_ਸੂਚਕ) {
        int32_t trimmed_ਲੰਮਾਈ = length;
        ਜੇ(trimmed_ਲੰਮਾਈ>dest_ਸਮਰੱਥਾ) {
            trimmed_ਲੰਮਾਈ=dest_ਸਮਰੱਥਾ;
        }
        us->extract(start32, trimmed_ਲੰਮਾਈ, dest);
        t->chunk_ਵਿੱਥ = start32+trimmed_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        t->chunk_ਵਿੱਥ = start32;
    }
    u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, length, p_Error_ਕੋਡ);
    ਵਾਪਸ length;
}

ਸਥਿਰ int32_t U_CALLCONV
unistr_ਪਾਠ_Replace(UText *ut,
                  int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
                  ਅਚਲ char16_t *src, int32_t length,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t old_ਲੰਮਾਈ;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(src==ਖਾਲੀ_ਸੂਚਕ && length!=0) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਜੇ(ਸ਼ੁਰੂ>ਹੱਦਬੰਦੀ) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }
    old_ਲੰਮਾਈ=us->length();
    int32_t start32 = pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ, old_ਲੰਮਾਈ);
    int32_t limit32 = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, old_ਲੰਮਾਈ);
    ਜੇ (start32 < old_ਲੰਮਾਈ) {
        start32 = us->get_Char32_ਸ਼ੁਰੂ(start32);
    }
    ਜੇ (limit32 < old_ਲੰਮਾਈ) {
        limit32 = us->get_Char32_ਸ਼ੁਰੂ(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t new_ਲੰਮਾਈ = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->get_ਬਫਰ();
    ut->chunk_ਲੰਮਾਈ      = new_ਲੰਮਾਈ;
    ut->chunk_Native_ਹੱਦਬੰਦੀ = new_ਲੰਮਾਈ;
    ut->native_Indexing_ਹੱਦਬੰਦੀ = new_ਲੰਮਾਈ;

    // Set iteration position to the point just following the newly inserted text.
    int32_t ਲੰਮਾਈ_Delta = new_ਲੰਮਾਈ - old_ਲੰਮਾਈ;
    ut->chunk_ਵਿੱਥ = limit32 + ਲੰਮਾਈ_Delta;

    ਵਾਪਸ ਲੰਮਾਈ_Delta;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
unistr_ਪਾਠ_Copy(UText *ut,
               int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
               int64_t dest_ਸੂਚਕਾਂਕ,
               UBool move,
               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    int32_t start32 = pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ, length);
    int32_t limit32 = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, length);
    int32_t destIndex32 = pin_ਸੂਚਕਾਂਕ(dest_ਸੂਚਕਾਂਕ, length);

    ਜੇ( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ(move) {
        // move: copy to destIndex, then remove original
        int32_t seg_ਲੰਮਾਈ=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        ਜੇ(destIndex32<start32) {
            start32+=seg_ਲੰਮਾਈ;
        }
        us->remove(start32, seg_ਲੰਮਾਈ);
    } ਨਹੀਂਤਾਂ {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->get_ਬਫਰ();
    ਜੇ (move==ਝੂਠ) {
        // copy operation, string length grows
        ut->chunk_ਲੰਮਾਈ += limit32-start32;
        ut->chunk_Native_ਹੱਦਬੰਦੀ = ut->chunk_ਲੰਮਾਈ;
        ut->native_Indexing_ਹੱਦਬੰਦੀ = ut->chunk_ਲੰਮਾਈ;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunk_ਵਿੱਥ = destIndex32+limit32-start32;
    ਜੇ (move && destIndex32>start32) {
        ut->chunk_ਵਿੱਥ = destIndex32;
    }

}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ UTextFuncs unistrFuncs =
{
    ਆਕਾਰ(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistr_ਪਾਠ_Clone,
    unistr_ਪਾਠ_ਲੰਮਾਈ,
    unistr_ਪਾਠ_Access,
    unistr_ਪਾਠ_ਕੱਢੋ,
    unistr_ਪਾਠ_Replace,
    unistr_ਪਾਠ_Copy,
    ਖਾਲੀ_ਸੂਚਕ,                // MapOffsetToNative,
    ਖਾਲੀ_ਸੂਚਕ,                // MapIndexToUTF16,
    unistr_ਪਾਠ_Close,
    ਖਾਲੀ_ਸੂਚਕ,                // spare 1
    ਖਾਲੀ_ਸੂਚਕ,                // spare 2
    ਖਾਲੀ_ਸੂਚਕ                 // spare 3
};



U_CDECL_ਸਿਰਾ


U_CAPI UText* U_EXPORT2
utext_openUnicodeString(UText* ut U_LIFETIME_BOUND,
                        UnicodeString* s U_LIFETIME_BOUND,
                        UError_ਕੋਡ* ਸਥਿਤੀ) {
    ut = utext_openConstUnicodeString(ut, s, ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ਵਾਪਸ ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             ਅਚਲ UnicodeString* s U_LIFETIME_BOUND,
                             UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, ਖਾਲੀ_ਸੂਚਕ, 0, ਸਥਿਤੀ);
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ut;
    }
    ut = utext_ਤਿਆਰੀ(ut, 0, ਸਥਿਤੀ);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->get_ਬਫਰ();
        ut->chunk_ਲੰਮਾਈ         = s->length();
        ut->chunk_Native_ਸ਼ੁਰੂ    = 0;
        ut->chunk_Native_ਹੱਦਬੰਦੀ    = ut->chunk_ਲੰਮਾਈ;
        ut->native_Indexing_ਹੱਦਬੰਦੀ = ut->chunk_ਲੰਮਾਈ;
    }
    ਵਾਪਸ ut;
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


ਸਥਿਰ UText * U_CALLCONV
ucstr_ਪਾਠ_Clone(UText *dest, ਅਚਲ UText * src, UBool deep, UError_ਕੋਡ * ਸਥਿਤੀ) {
    // First do a generic shallow clone.
    dest = shallow_ਪਾਠ_Clone(dest, src, ਸਥਿਤੀ);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    ਜੇ (deep && U_SUCCESS(*ਸਥਿਤੀ)) {
        U_ASSERT(utext_native_ਲੰਮਾਈ(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_native_ਲੰਮਾਈ(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        ਅਚਲ char16_t *srcStr = (ਅਚਲ char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * ਆਕਾਰ(char16_t));
        ਜੇ (copyStr == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            int64_t i;
            ਲਈ (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ);
        }
    }
    ਵਾਪਸ dest;
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucstr_ਪਾਠ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ਜੇ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_ਪਾਠ)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = ਖਾਲੀ_ਸੂਚਕ;
    }
}



ਸਥਿਰ int64_t U_CALLCONV
ucstr_ਪਾਠ_ਲੰਮਾਈ(UText *ut) {
    ਜੇ (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        ਅਚਲ char16_t  *str = (ਅਚਲ char16_t *)ut->context;
        ਲਈ (;;) {
            ਜੇ (str[ut->chunk_Native_ਹੱਦਬੰਦੀ] == 0) {
                ਰੋਕੋ;
            }
            ut->chunk_Native_ਹੱਦਬੰਦੀ++;
        }
        ut->a = ut->chunk_Native_ਹੱਦਬੰਦੀ;
        ut->chunk_ਲੰਮਾਈ = (int32_t)ut->chunk_Native_ਹੱਦਬੰਦੀ;
        ut->native_Indexing_ਹੱਦਬੰਦੀ = ut->chunk_ਲੰਮਾਈ;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE);
    }
    ਵਾਪਸ ut->a;
}


ਸਥਿਰ UBool U_CALLCONV
ucstr_ਪਾਠ_Access(UText *ut, int64_t ਸੂਚਕਾਂਕ, UBool  forward) {
    ਅਚਲ char16_t *str   = (ਅਚਲ char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    ਜੇ (ਸੂਚਕਾਂਕ<0) {
        ਸੂਚਕਾਂਕ = 0;
    } ਨਹੀਂਤਾਂ ਜੇ (ਸੂਚਕਾਂਕ < ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_ਸੈੱਟ_CP_ਸ਼ੁਰੂ(str, 0, ਸੂਚਕਾਂਕ);
    } ਨਹੀਂਤਾਂ ਜੇ (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        ਸੂਚਕਾਂਕ = ut->a;
    } ਨਹੀਂਤਾਂ {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t ਜਾਂਚ_ਹੱਦਬੰਦੀ = (int32_t)ਸੂਚਕਾਂਕ + 32;
        ਜੇ ((ਸੂਚਕਾਂਕ + 32)>INT32_MAX || (ਸੂਚਕਾਂਕ + 32)<0 ) {   // note: int64 expression
            ਜਾਂਚ_ਹੱਦਬੰਦੀ = INT32_MAX;
        }

        int32_t chunk_ਹੱਦਬੰਦੀ = (int32_t)ut->chunk_Native_ਹੱਦਬੰਦੀ;
        ਲਈ (; chunk_ਹੱਦਬੰਦੀ<ਜਾਂਚ_ਹੱਦਬੰਦੀ; chunk_ਹੱਦਬੰਦੀ++) {
            ਜੇ (str[chunk_ਹੱਦਬੰਦੀ] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunk_ਹੱਦਬੰਦੀ;
                ut->chunk_ਲੰਮਾਈ = chunk_ਹੱਦਬੰਦੀ;
                ut->native_Indexing_ਹੱਦਬੰਦੀ = chunk_ਹੱਦਬੰਦੀ;
                ਜੇ (ਸੂਚਕਾਂਕ >= chunk_ਹੱਦਬੰਦੀ) {
                    ਸੂਚਕਾਂਕ = chunk_ਹੱਦਬੰਦੀ;
                } ਨਹੀਂਤਾਂ {
                    U16_ਸੈੱਟ_CP_ਸ਼ੁਰੂ(str, 0, ਸੂਚਕਾਂਕ);
                }

                ut->chunk_Native_ਹੱਦਬੰਦੀ = chunk_ਹੱਦਬੰਦੀ;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE);
                ਜਾਓ breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_ਸੈੱਟ_CP_ਸ਼ੁਰੂ(str, 0, ਸੂਚਕਾਂਕ);
        ਜੇ (chunk_ਹੱਦਬੰਦੀ == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunk_ਹੱਦਬੰਦੀ;
            ut->chunk_ਲੰਮਾਈ = chunk_ਹੱਦਬੰਦੀ;
            ut->native_Indexing_ਹੱਦਬੰਦੀ = chunk_ਹੱਦਬੰਦੀ;
            ਜੇ (ਸੂਚਕਾਂਕ > chunk_ਹੱਦਬੰਦੀ) {
                ਸੂਚਕਾਂਕ = chunk_ਹੱਦਬੰਦੀ;
            }
            ut->chunk_Native_ਹੱਦਬੰਦੀ = chunk_ਹੱਦਬੰਦੀ;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE);
        } ਨਹੀਂਤਾਂ {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            ਜੇ (U16_IS_LEAD(str[chunk_ਹੱਦਬੰਦੀ-1])) {
                --chunk_ਹੱਦਬੰਦੀ;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunk_Native_ਹੱਦਬੰਦੀ = chunk_ਹੱਦਬੰਦੀ;
            ut->native_Indexing_ਹੱਦਬੰਦੀ = chunk_ਹੱਦਬੰਦੀ;
            ut->chunk_ਲੰਮਾਈ = chunk_ਹੱਦਬੰਦੀ;
        }

    }
breakout:
    U_ASSERT(ਸੂਚਕਾਂਕ<=INT32_MAX);
    ut->chunk_ਵਿੱਥ = (int32_t)ਸੂਚਕਾਂਕ;

    // Check whether request is at the start or end
    UBool retVal = (forward && ਸੂਚਕਾਂਕ<ut->chunk_Native_ਹੱਦਬੰਦੀ) || (!forward && ਸੂਚਕਾਂਕ>0);
    ਵਾਪਸ retVal;
}



ਸਥਿਰ int32_t U_CALLCONV
ucstr_ਪਾਠ_ਕੱਢੋ(UText *ut,
                  int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
                  char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                  UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0) || ਸ਼ੁਰੂ>ਹੱਦਬੰਦੀ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstr_ਪਾਠ_Access(ut, ਸ਼ੁਰੂ, ਸੱਚ);
    ਅਚਲ char16_t *s=ut->chunkContents;
    start32 = ut->chunk_ਵਿੱਥ;

    int32_t str_ਲੰਮਾਈ=(int32_t)ut->a;
    ਜੇ (str_ਲੰਮਾਈ >= 0) {
        limit32 = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, str_ਲੰਮਾਈ);
    } ਨਹੀਂਤਾਂ {
        limit32 = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, INT32_MAX);
    }
    di = 0;
    ਲਈ (si=start32; si<limit32; si++) {
        ਜੇ (str_ਲੰਮਾਈ<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunk_Native_ਹੱਦਬੰਦੀ    = si;
            ut->chunk_ਲੰਮਾਈ         = si;
            ut->native_Indexing_ਹੱਦਬੰਦੀ = si;
            str_ਲੰਮਾਈ               = si;
            limit32                 = si;
            ਰੋਕੋ;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        ਜੇ (di<dest_ਸਮਰੱਥਾ) {
            // only store if there is space.
            dest[di] = s[si];
        } ਨਹੀਂਤਾਂ {
            ਜੇ (str_ਲੰਮਾਈ>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                ਰੋਕੋ;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    ਜੇ (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<str_ਲੰਮਾਈ || str_ਲੰਮਾਈ<0)  && U16_IS_TRAIL(s[si])))
    {
        ਜੇ (di<dest_ਸਮਰੱਥਾ) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    ਜੇ (si <= ut->chunk_Native_ਹੱਦਬੰਦੀ) {
        ut->chunk_ਵਿੱਥ = si;
    } ਨਹੀਂਤਾਂ {
        ucstr_ਪਾਠ_Access(ut, si, ਸੱਚ);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, di, p_Error_ਕੋਡ);
    ਵਾਪਸ di;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ UTextFuncs ucstrFuncs =
{
    ਆਕਾਰ(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstr_ਪਾਠ_Clone,
    ucstr_ਪਾਠ_ਲੰਮਾਈ,
    ucstr_ਪਾਠ_Access,
    ucstr_ਪਾਠ_ਕੱਢੋ,
    ਖਾਲੀ_ਸੂਚਕ,              // Replace
    ਖਾਲੀ_ਸੂਚਕ,              // Copy
    ਖਾਲੀ_ਸੂਚਕ,              // MapOffsetToNative,
    ਖਾਲੀ_ਸੂਚਕ,              // MapIndexToUTF16,
    ucstr_ਪਾਠ_Close,
    ਖਾਲੀ_ਸੂਚਕ,              // spare 1
    ਖਾਲੀ_ਸੂਚਕ,              // spare 2
    ਖਾਲੀ_ਸੂਚਕ,              // spare 3
};

U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ char16_t g_ਖਾਲੀ_ਥਾਵਾਂ_UString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 ਅਚਲ char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ && length==0) {
        s = g_ਖਾਲੀ_ਥਾਵਾਂ_UString;
    }
    ਜੇ (s==ਖਾਲੀ_ਸੂਚਕ || length < -1 || length>INT32_MAX) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ut = utext_ਤਿਆਰੀ(ut, 0, ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ਜੇ (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_ਲੰਮਾਈ_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunk_Native_ਸ਼ੁਰੂ     = 0;
        ut->chunk_Native_ਹੱਦਬੰਦੀ     = length>=0? length : 0;
        ut->chunk_ਲੰਮਾਈ          = (int32_t)ut->chunk_Native_ਹੱਦਬੰਦੀ;
        ut->chunk_ਵਿੱਥ          = 0;
        ut->native_Indexing_ਹੱਦਬੰਦੀ  = ut->chunk_ਲੰਮਾਈ;
    }
    ਵਾਪਸ ut;
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CIBuf_ਮਾਪ 16

U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
char_Iter_ਪਾਠ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    ਮਿਟਾਓ ci;
    ut->r = ਖਾਲੀ_ਸੂਚਕ;
}

ਸਥਿਰ int64_t U_CALLCONV
char_Iter_ਪਾਠ_ਲੰਮਾਈ(UText *ut) {
    ਵਾਪਸ (int32_t)ut->a;
}

ਸਥਿਰ UBool U_CALLCONV
char_Iter_ਪਾਠ_Access(UText *ut, int64_t ਸੂਚਕਾਂਕ, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clipped_ਸੂਚਕਾਂਕ = (int32_t)ਸੂਚਕਾਂਕ;
    ਜੇ (clipped_ਸੂਚਕਾਂਕ<0) {
        clipped_ਸੂਚਕਾਂਕ=0;
    } ਨਹੀਂਤਾਂ ਜੇ (clipped_ਸੂਚਕਾਂਕ>=ut->a) {
        clipped_ਸੂਚਕਾਂਕ=(int32_t)ut->a;
    }
    int32_t needed_ਸੂਚਕਾਂਕ = clipped_ਸੂਚਕਾਂਕ;
    ਜੇ (!forward && needed_ਸੂਚਕਾਂਕ>0) {
        // reverse iteration, want the position just before what was asked for.
        needed_ਸੂਚਕਾਂਕ--;
    } ਨਹੀਂਤਾਂ ਜੇ (forward && needed_ਸੂਚਕਾਂਕ==ut->a && needed_ਸੂਚਕਾਂਕ>0) {
        // Forward iteration, don't ask for something past the end of the text.
        needed_ਸੂਚਕਾਂਕ--;
    }

    // Find the native index of the start of the buffer containing what we want.
    needed_ਸੂਚਕਾਂਕ -= needed_ਸੂਚਕਾਂਕ % CIBuf_ਮਾਪ;

    char16_t *buf = ਖਾਲੀ_ਸੂਚਕ;
    UBool  need_Chunk_ਤਿਆਰੀ = ਸੱਚ;
    ਪੂਰਨ    i;
    ਜੇ (ut->chunk_Native_ਸ਼ੁਰੂ == needed_ਸੂਚਕਾਂਕ) {
        // The buffer we want is already the current chunk.
        need_Chunk_ਤਿਆਰੀ = ਝੂਠ;
    } ਨਹੀਂਤਾਂ ਜੇ (ut->b == needed_ਸੂਚਕਾਂਕ) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } ਨਹੀਂਤਾਂ ਜੇ (ut->c == needed_ਸੂਚਕਾਂਕ) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } ਨਹੀਂਤਾਂ {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        ਜੇ (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->ਸੈੱਟ_ਸੂਚਕਾਂਕ(needed_ਸੂਚਕਾਂਕ);
        ਲਈ (i=0; i<CIBuf_ਮਾਪ; i++) {
            buf[i] = ci->ਅਗਲਾ_Post_Inc();
            ਜੇ (i+needed_ਸੂਚਕਾਂਕ > ut->a) {
                ਰੋਕੋ;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    ਜੇ (need_Chunk_ਤਿਆਰੀ) {
        ut->chunkContents = buf;
        ut->chunk_ਲੰਮਾਈ   = CIBuf_ਮਾਪ;
        ut->chunk_Native_ਸ਼ੁਰੂ = needed_ਸੂਚਕਾਂਕ;
        ut->chunk_Native_ਹੱਦਬੰਦੀ = needed_ਸੂਚਕਾਂਕ + CIBuf_ਮਾਪ;
        ਜੇ (ut->chunk_Native_ਹੱਦਬੰਦੀ > ut->a) {
            ut->chunk_Native_ਹੱਦਬੰਦੀ = ut->a;
            ut->chunk_ਲੰਮਾਈ  = (int32_t)(ut->chunk_Native_ਹੱਦਬੰਦੀ)-(int32_t)(ut->chunk_Native_ਸ਼ੁਰੂ);
        }
        ut->native_Indexing_ਹੱਦਬੰਦੀ = ut->chunk_ਲੰਮਾਈ;
        U_ASSERT(ut->chunk_ਵਿੱਥ>=0 && ut->chunk_ਵਿੱਥ<=CIBuf_ਮਾਪ);
    }
    ut->chunk_ਵਿੱਥ = clipped_ਸੂਚਕਾਂਕ - (int32_t)ut->chunk_Native_ਸ਼ੁਰੂ;
    UBool success = (forward? ut->chunk_ਵਿੱਥ<ut->chunk_ਲੰਮਾਈ : ut->chunk_ਵਿੱਥ>0);
    ਵਾਪਸ success;
}

ਸਥਿਰ UText * U_CALLCONV
char_Iter_ਪਾਠ_Clone(UText *dest, ਅਚਲ UText *src, UBool deep, UError_ਕੋਡ * ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_get_Native_ਸੂਚਕਾਂਕ((UText *)src);
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    ਵਾਪਸ dest;
}

ਸਥਿਰ int32_t U_CALLCONV
char_Iter_ਪਾਠ_ਕੱਢੋ(UText *ut,
                  int64_t ਸ਼ੁਰੂ, int64_t ਹੱਦਬੰਦੀ,
                  char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                  UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0) || ਸ਼ੁਰੂ>ਹੱਦਬੰਦੀ) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ, length);
    int32_t  limit32 = pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copy_ਹੱਦਬੰਦੀ;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->ਸੈੱਟ_Index32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->get_ਸੂਚਕਾਂਕ();
    copy_ਹੱਦਬੰਦੀ = srci;
    ਜਦੋਂ (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_ਲੰਮਾਈ(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        ਜੇ (desti+len <= dest_ਸਮਰੱਥਾ) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copy_ਹੱਦਬੰਦੀ = srci+len;
        } ਨਹੀਂਤਾਂ {
            desti += len;
            *ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
        srci += len;
    }

    char_Iter_ਪਾਠ_Access(ut, copy_ਹੱਦਬੰਦੀ, ਸੱਚ);

    u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, desti, ਸਥਿਤੀ);
    ਵਾਪਸ desti;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ UTextFuncs charIterFuncs =
{
    ਆਕਾਰ(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    char_Iter_ਪਾਠ_Clone,
    char_Iter_ਪਾਠ_ਲੰਮਾਈ,
    char_Iter_ਪਾਠ_Access,
    char_Iter_ਪਾਠ_ਕੱਢੋ,
    ਖਾਲੀ_ਸੂਚਕ,                // Replace
    ਖਾਲੀ_ਸੂਚਕ,                // Copy
    ਖਾਲੀ_ਸੂਚਕ,                // MapOffsetToNative,
    ਖਾਲੀ_ਸੂਚਕ,                // MapIndexToUTF16,
    char_Iter_ਪਾਠ_Close,
    ਖਾਲੀ_ਸੂਚਕ,                // spare 1
    ਖਾਲੀ_ਸੂਚਕ,                // spare 2
    ਖਾਲੀ_ਸੂਚਕ                 // spare 3
};
U_CDECL_ਸਿਰਾ

U_CAPI UText* U_EXPORT2
utext_openCharacterIterator(UText* ut U_LIFETIME_BOUND,
                            CharacterIterator* ci U_LIFETIME_BOUND,
                            UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (ci->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBuf_ਮਾਪ * ਆਕਾਰ(char16_t);
    ut = utext_ਤਿਆਰੀ(ut, extraSpace, ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->ਸਿਰਾ_ਸੂਚਕਾਂਕ();        // Length of text
        ut->p                    = ut->pExtra;            // First buffer
        ut->b                    = -1;                    // Native index of first buffer contents
        ut->q                    = (char16_t*)ut->pExtra+CIBuf_ਮਾਪ;  // Second buffer
        ut->c                    = -1;                    // Native index of second buffer contents

        // Initialize current chunk contents to be empty.
        //   First access will fault something in.
        //   Note:  The initial nativeStart and chunkOffset must sum to zero
        //          so that getNativeIndex() will correctly compute to zero
        //          if no call to Access() has ever been made.  They can't be both
        //          zero without Access() thinking that the chunk is valid.
        ut->chunkContents        = (char16_t *)ut->p;
        ut->chunk_Native_ਸ਼ੁਰੂ     = -1;
        ut->chunk_ਵਿੱਥ          = 1;
        ut->chunk_Native_ਹੱਦਬੰਦੀ     = 0;
        ut->chunk_ਲੰਮਾਈ          = 0;
        ut->native_Indexing_ਹੱਦਬੰਦੀ  = ut->chunk_ਵਿੱਥ;  // enables native indexing
    }
    ਵਾਪਸ ut;
}

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

#ಅನ್ತರ್ಭಾವ <cstddef>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/utext.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"

U_NAMESPACE_USE

#ವ್ಯಾಖ್ಯಾನಿಸು I32_FLAG(ಬಿಟ್_ಸೂಚ್ಯಂಕ) ((int32_t)1<<(ಬಿಟ್_ಸೂಚ್ಯಂಕ))


ಸ್ಥಿರ UBool
utext_access(UText *ut, int64_t ಸೂಚ್ಯಂಕ, UBool forward) {
    ಹಿಂತಿರುಗಿ ut->pFuncs->access(ut, ಸೂಚ್ಯಂಕ, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    ಆದರೆ (delta > 0) {
        ಮಾಡು {
            ಆದರೆ(ut->chunk_ಸ್ಥಾನಾಂತರ>=ut->chunk_ಉದ್ದಳತೆ && !utext_access(ut, ut->chunk_Native_ಎಲ್ಲೆ, ನಿಜ)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            c = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];
            ಆದರೆ (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                ಆದರೆ (c == U_SENTINEL) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ut->chunk_ಸ್ಥಾನಾಂತರ++;
            }
        } ಆಗ(--delta>0);

    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (delta<0) {
        ಮಾಡು {
            ಆದರೆ(ut->chunk_ಸ್ಥಾನಾಂತರ<=0 && !utext_access(ut, ut->chunk_Native_ಆರಂಭ, ಸುಳ್ಳು)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            c = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ-1];
            ಆದರೆ (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                ಆದರೆ (c == U_SENTINEL) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ut->chunk_ಸ್ಥಾನಾಂತರ--;
            }
        } ಆಗ(++delta<0);
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}


U_CAPI int64_t U_EXPORT2
utext_native_ಉದ್ದಳತೆ(UText *ut) {
    ಹಿಂತಿರುಗಿ ut->pFuncs->native_ಉದ್ದಳತೆ(ut);
}


U_CAPI UBool U_EXPORT2
utext_is_ಉದ್ದಳತೆ_Expensive(ಸ್ಥಿರಾಂಕ UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE)) != 0;
    ಹಿಂತಿರುಗಿ r;
}


U_CAPI int64_t U_EXPORT2
utext_get_Native_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UText *ut) {
    ಆದರೆ(ut->chunk_ಸ್ಥಾನಾಂತರ <= ut->native_Indexing_ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ ut->chunk_Native_ಆರಂಭ+ut->chunk_ಸ್ಥಾನಾಂತರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ut->pFuncs->map_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_Native(ut);
    }
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(UText *ut, int64_t ಸೂಚ್ಯಂಕ) {
    ಆದರೆ(ಸೂಚ್ಯಂಕ<ut->chunk_Native_ಆರಂಭ || ಸೂಚ್ಯಂಕ>=ut->chunk_Native_ಎಲ್ಲೆ) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, ಸೂಚ್ಯಂಕ, ನಿಜ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((int32_t)(ಸೂಚ್ಯಂಕ - ut->chunk_Native_ಆರಂಭ) <= ut->native_Indexing_ಎಲ್ಲೆ) {
        // utf-16 indexing.
        ut->chunk_ಸ್ಥಾನಾಂತರ=(int32_t)(ಸೂಚ್ಯಂಕ-ut->chunk_Native_ಆರಂಭ);
    } ಇಲ್ಲದಿದ್ದರೆ {
         ut->chunk_ಸ್ಥಾನಾಂತರ=ut->pFuncs->map_Native_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_UTF16(ut, ಸೂಚ್ಯಂಕ);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ<ut->chunk_ಉದ್ದಳತೆ) {
        char16_t c= ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];
        ಆದರೆ (U16_IS_TRAIL(c)) {
            ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ==0) {
                ut->pFuncs->access(ut, ut->chunk_Native_ಆರಂಭ, ಸುಳ್ಳು);
            }
            ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ>0) {
                char16_t lead = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ-1];
                ಆದರೆ (U16_IS_LEAD(lead)) {
                    ut->chunk_ಸ್ಥಾನಾಂತರ--;
                }
            }
        }
    }
}



U_CAPI int64_t U_EXPORT2
utext_get_ಹಿಂದಿನ_Native_ಸೂಚ್ಯಂಕ(UText *ut) {
    //
    //  Fast-path the common case.
    //     Common means current position is not at the beginning of a chunk
    //     and the preceding character is not supplementary.
    //
    int32_t i = ut->chunk_ಸ್ಥಾನಾಂತರ - 1;
    int64_t ಫಲಿತಾಂಶ;
    ಆದರೆ (i >= 0) {
        char16_t c = ut->chunkContents[i];
        ಆದರೆ (U16_IS_TRAIL(c) == ಸುಳ್ಳು) {
            ಆದರೆ (i <= ut->native_Indexing_ಎಲ್ಲೆ) {
                ಫಲಿತಾಂಶ = ut->chunk_Native_ಆರಂಭ + i;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ut->chunk_ಸ್ಥಾನಾಂತರ = i;
                ಫಲಿತಾಂಶ = ut->pFuncs->map_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_Native(ut);
                ut->chunk_ಸ್ಥಾನಾಂತರ++;
            }
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    }

    // If at the start of text, simply return 0.
    ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ==0 && ut->chunk_Native_ಆರಂಭ==0) {
        ಹಿಂತಿರುಗಿ 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    ಫಲಿತಾಂಶ = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ==ut->chunk_ಉದ್ದಳತೆ) {
        // Current position is just off the end of the chunk.
        ಆದರೆ (ut->pFuncs->access(ut, ut->chunk_Native_ಎಲ್ಲೆ, ನಿಜ) == ಸುಳ್ಳು) {
            // Off the end of the text.
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];
    ಆದರೆ (U16_IS_LEAD(c) == ಸುಳ್ಳು) {
        // Normal, non-supplementary case.
        ಹಿಂತಿರುಗಿ c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    ಆದರೆ ((ut->chunk_ಸ್ಥಾನಾಂತರ+1) < ut->chunk_ಉದ್ದಳತೆ) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ+1];
    } ಇಲ್ಲದಿದ್ದರೆ {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunk_Native_ಎಲ್ಲೆ;
        ಆದರೆ (ut->pFuncs->access(ut, nativePosition, ನಿಜ)) {
            trail = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, ಸುಳ್ಳು);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunk_ಸ್ಥಾನಾಂತರ = ut->chunk_ಉದ್ದಳತೆ - 1;
        ಆದರೆ(!r) {
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        }
    }

    ಆದರೆ (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    ಹಿಂತಿರುಗಿ supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t native_ಸೂಚ್ಯಂಕ) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    ಆದರೆ (native_ಸೂಚ್ಯಂಕ>=ut->chunk_Native_ಆರಂಭ && native_ಸೂಚ್ಯಂಕ < ut->chunk_Native_ಆರಂಭ + ut->native_Indexing_ಎಲ್ಲೆ) {
        ut->chunk_ಸ್ಥಾನಾಂತರ = (int32_t)(native_ಸೂಚ್ಯಂಕ - ut->chunk_Native_ಆರಂಭ);
        c = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];
        ಆದರೆ (U16_IS_SURROGATE(c) == ಸುಳ್ಳು) {
            ಹಿಂತಿರುಗಿ c;
        }
    }


    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ut, native_ಸೂಚ್ಯಂಕ);
    ಆದರೆ (native_ಸೂಚ್ಯಂಕ>=ut->chunk_Native_ಆರಂಭ && ut->chunk_ಸ್ಥಾನಾಂತರ<ut->chunk_ಉದ್ದಳತೆ) {
        c = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];
        ಆದರೆ (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    ಹಿಂತಿರುಗಿ c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ >= ut->chunk_ಉದ್ದಳತೆ) {
        ಆದರೆ (ut->pFuncs->access(ut, ut->chunk_Native_ಎಲ್ಲೆ, ನಿಜ) == ಸುಳ್ಳು) {
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ++];
    ಆದರೆ (U16_IS_LEAD(c) == ಸುಳ್ಳು) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        ಹಿಂತಿರುಗಿ c;
    }

    ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ >= ut->chunk_ಉದ್ದಳತೆ) {
        ಆದರೆ (ut->pFuncs->access(ut, ut->chunk_Native_ಎಲ್ಲೆ, ನಿಜ) == ಸುಳ್ಳು) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            ಹಿಂತಿರುಗಿ c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];
    ಆದರೆ (U16_IS_TRAIL(trail) == ಸುಳ್ಳು) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        ಹಿಂತಿರುಗಿ c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunk_ಸ್ಥಾನಾಂತರ++;   // move iteration position over the trail surrogate.
    ಹಿಂತಿರುಗಿ supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ <= 0) {
        ಆದರೆ (ut->pFuncs->access(ut, ut->chunk_Native_ಆರಂಭ, ಸುಳ್ಳು) == ಸುಳ್ಳು) {
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        }
    }
    ut->chunk_ಸ್ಥಾನಾಂತರ--;
    c = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];
    ಆದರೆ (U16_IS_TRAIL(c) == ಸುಳ್ಳು) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        ಹಿಂತಿರುಗಿ c;
    }

    ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ <= 0) {
        ಆದರೆ (ut->pFuncs->access(ut, ut->chunk_Native_ಆರಂಭ, ಸುಳ್ಳು) == ಸುಳ್ಳು) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            ಹಿಂತಿರುಗಿ c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ-1];
    ಆದರೆ (U16_IS_LEAD(lead) == ಸುಳ್ಳು) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        ಹಿಂತಿರುಗಿ c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunk_ಸ್ಥಾನಾಂತರ--;   // move iteration position over the lead surrogate.
    ಹಿಂತಿರುಗಿ supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t ಸೂಚ್ಯಂಕ) {
    UChar32       c      = U_SENTINEL;

    ಆದರೆ(ಸೂಚ್ಯಂಕ<ut->chunk_Native_ಆರಂಭ || ಸೂಚ್ಯಂಕ>=ut->chunk_Native_ಎಲ್ಲೆ) {
        // Desired position is outside of the current chunk.
        ಆದರೆ(!ut->pFuncs->access(ut, ಸೂಚ್ಯಂಕ, ನಿಜ)) {
            // no chunk available here
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೂಚ್ಯಂಕ - ut->chunk_Native_ಆರಂಭ  <= (int64_t)ut->native_Indexing_ಎಲ್ಲೆ) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunk_ಸ್ಥಾನಾಂತರ = (int32_t)(ಸೂಚ್ಯಂಕ - ut->chunk_Native_ಆರಂಭ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunk_ಸ್ಥಾನಾಂತರ = ut->pFuncs->map_Native_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_UTF16(ut, ಸೂಚ್ಯಂಕ);
    }

    c = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ++];
    ಆದರೆ (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ut, ಸೂಚ್ಯಂಕ);
        c = utext_next32(ut);
    }
    ಹಿಂತಿರುಗಿ c;
}


U_CAPI UChar32 U_EXPORT2
utext_previous32From(UText *ut, int64_t ಸೂಚ್ಯಂಕ) {
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
    ಆದರೆ(ಸೂಚ್ಯಂಕ<=ut->chunk_Native_ಆರಂಭ || ಸೂಚ್ಯಂಕ>ut->chunk_Native_ಎಲ್ಲೆ) {
        // Requested native index is outside of the current chunk.
        ಆದರೆ(!ut->pFuncs->access(ut, ಸೂಚ್ಯಂಕ, ಸುಳ್ಳು)) {
            // no chunk available here
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ - ut->chunk_Native_ಆರಂಭ <= (int64_t)ut->native_Indexing_ಎಲ್ಲೆ) {
        // Direct UTF-16 indexing.
        ut->chunk_ಸ್ಥಾನಾಂತರ = (int32_t)(ಸೂಚ್ಯಂಕ - ut->chunk_Native_ಆರಂಭ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ut->chunk_ಸ್ಥಾನಾಂತರ=ut->pFuncs->map_Native_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_UTF16(ut, ಸೂಚ್ಯಂಕ);
        ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ==0 && !ut->pFuncs->access(ut, ಸೂಚ್ಯಂಕ, ಸುಳ್ಳು)) {
            // no chunk available here
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunk_ಸ್ಥಾನಾಂತರ--;
    cPrev = ut->chunkContents[ut->chunk_ಸ್ಥಾನಾಂತರ];

    ಆದರೆ (U16_IS_SURROGATE(cPrev)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ut, ಸೂಚ್ಯಂಕ);
        cPrev = utext_previous32(ut);
    }
    ಹಿಂತಿರುಗಿ cPrev;
}


U_CAPI int32_t U_EXPORT2
utext_ಹೊರತೆಗೆ(UText *ut,
             int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
             char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
             UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
                 ಹಿಂತಿರುಗಿ ut->pFuncs->extract(ut, ಆರಂಭ, ಎಲ್ಲೆ, dest, dest_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2);
             }



U_CAPI UBool U_EXPORT2
utext_equals(ಸ್ಥಿರಾಂಕ UText *a, ಸ್ಥಿರಾಂಕ UText *b) {
    ಆದರೆ (a==ಶೂನ್ಯಸೂಚಕ || b==ಶೂನ್ಯಸೂಚಕ ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ (a->context != b->context) {
        // Different sources (different strings)
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (utext_get_Native_ಸೂಚ್ಯಂಕ(a) != utext_get_Native_ಸೂಚ್ಯಂಕ(b)) {
        // Different current position in the string.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(ಸ್ಥಿರಾಂಕ UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    ಹಿಂತಿರುಗಿ b;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_has_Meta_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_ದತ್ತಾಂಶ)) != 0;
    ಹಿಂತಿರುಗಿ b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t native_ಆರಂಭ, int64_t native_ಎಲ್ಲೆ,
             ಸ್ಥಿರಾಂಕ char16_t *replacement_ಪಠ್ಯ, int32_t replacement_ಉದ್ದಳತೆ,
             UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *ಸ್ಥಿತಿ_2 = U_NO_WRITE_PERMISSION;
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t i = ut->pFuncs->replace(ut, native_ಆರಂಭ, native_ಎಲ್ಲೆ, replacement_ಪಠ್ಯ, replacement_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ i;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utext_copy(UText *ut,
          int64_t native_ಆರಂಭ, int64_t native_ಎಲ್ಲೆ,
          int64_t dest_ಸೂಚ್ಯಂಕ,
          UBool move,
          UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *ಸ್ಥಿತಿ_2 = U_NO_WRITE_PERMISSION;
        ಹಿಂತಿರುಗಿ;
    }
    ut->pFuncs->copy(ut, native_ಆರಂಭ, native_ಎಲ್ಲೆ, dest_ಸೂಚ್ಯಂಕ, move, ಸ್ಥಿತಿ_2);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            ಸ್ಥಿರಾಂಕ UText* src,
            UBool deep,
            UBool readOnly,
            UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ dest;
    }
    UText *ಫಲಿತಾಂಶ = src->pFuncs->clone(dest, src, deep, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆದರೆ (readOnly) {
        utext_freeze(ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
ಎಣಿಕೆ {
    UTEXT_ರಾಶಿ_ALLOCATED  = 1,      //  1 if ICU has allocated this UText struct on the heap.
                                    //  0 if caller provided storage for the UText.

    UTEXT_EXTRA_ರಾಶಿ_ALLOCATED = 2, //  1 if ICU has allocated extra storage as a separate
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

ರಚನೆ ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UText ಖಾಲಿಗಳು_ಪಠ್ಯ = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_ಸಿದ್ಧತೆ(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ut;
    }

    ಆದರೆ (ut == ಶೂನ್ಯಸೂಚಕ) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = ಗಾತ್ರ(UText);
        ಆದರೆ (extraSpace > 0) {
            spaceRequired = ಗಾತ್ರ(ExtendedUText) + extraSpace - ಗಾತ್ರ(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        ಆದರೆ (ut == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *ut = ಖಾಲಿಗಳು_ಪಠ್ಯ;
            ut->flags |= UTEXT_ರಾಶಿ_ALLOCATED;
            ಆದರೆ (spaceRequired>0) {
                ut->extra_ಗಾತ್ರ = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        ಆದರೆ (ut->magic != UTEXT_MAGIC) {
            *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        ಆದರೆ ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != ಶೂನ್ಯಸೂಚಕ)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        ಆದರೆ (extraSpace > ut->extra_ಗಾತ್ರ) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            ಆದರೆ (ut->flags & UTEXT_EXTRA_ರಾಶಿ_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extra_ಗಾತ್ರ = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            ಆದರೆ (ut->pExtra == ಶೂನ್ಯಸೂಚಕ) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ut->extra_ಗಾತ್ರ = extraSpace;
                ut->flags |= UTEXT_EXTRA_ರಾಶಿ_ALLOCATED;
            }
        }
    }
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ut->flags |= UTEXT_OPEN;

        // Initialize all remaining fields of the UText.
        //
        ut->context             = ಶೂನ್ಯಸೂಚಕ;
        ut->chunkContents       = ಶೂನ್ಯಸೂಚಕ;
        ut->p                   = ಶೂನ್ಯಸೂಚಕ;
        ut->q                   = ಶೂನ್ಯಸೂಚಕ;
        ut->r                   = ಶೂನ್ಯಸೂಚಕ;
        ut->a                   = 0;
        ut->b                   = 0;
        ut->c                   = 0;
        ut->chunk_ಸ್ಥಾನಾಂತರ         = 0;
        ut->chunk_ಉದ್ದಳತೆ         = 0;
        ut->chunk_Native_ಆರಂಭ    = 0;
        ut->chunk_Native_ಎಲ್ಲೆ    = 0;
        ut->native_Indexing_ಎಲ್ಲೆ = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = ಶೂನ್ಯಸೂಚಕ;
        ಆದರೆ (ut->pExtra!=ಶೂನ್ಯಸೂಚಕ && ut->extra_ಗಾತ್ರ>0)
            uprv_memset(ut->pExtra, 0, ut->extra_ಗಾತ್ರ);

    }
    ಹಿಂತಿರುಗಿ ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    ಆದರೆ (ut==ಶೂನ್ಯಸೂಚಕ ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        ಹಿಂತಿರುಗಿ ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    ಆದರೆ (ut->pFuncs->close != ಶೂನ್ಯಸೂಚಕ) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    ಆದರೆ (ut->flags & UTEXT_EXTRA_ರಾಶಿ_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = ಶೂನ್ಯಸೂಚಕ;
        ut->flags &= ~UTEXT_EXTRA_ರಾಶಿ_ALLOCATED;
        ut->extra_ಗಾತ್ರ = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (ut->flags & UTEXT_ರಾಶಿ_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
ಸ್ಥಿರ ಶೂನ್ಯ
invalidateChunk(UText *ut) {
    ut->chunk_ಉದ್ದಳತೆ = 0;
    ut->chunk_Native_ಎಲ್ಲೆ = 0;
    ut->chunk_Native_ಆರಂಭ = 0;
    ut->chunk_ಸ್ಥಾನಾಂತರ = 0;
    ut->native_Indexing_ಎಲ್ಲೆ = 0;
}

//
// pinIndex        Do range pinning on a native index parameter.
//                 64 bit pinning is done in place.
//                 32 bit truncated result is returned as a convenience for
//                        use in providers that don't need 64 bits.
ಸ್ಥಿರ int32_t
pin_ಸೂಚ್ಯಂಕ(int64_t &ಸೂಚ್ಯಂಕ, int64_t ಎಲ್ಲೆ) {
    ಆದರೆ (ಸೂಚ್ಯಂಕ<0) {
        ಸೂಚ್ಯಂಕ = 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೂಚ್ಯಂಕ > ಎಲ್ಲೆ) {
        ಸೂಚ್ಯಂಕ = ಎಲ್ಲೆ;
    }
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಸೂಚ್ಯಂಕ);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
ಸ್ಥಿರ ಶೂನ್ಯ adjust_ಸೂಚಕ(UText *dest, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **destPtr, ಸ್ಥಿರಾಂಕ UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    ಅಕ್ಷರ  *dptr = (ಅಕ್ಷರ *)*destPtr;
    ಅಕ್ಷರ  *dUText = (ಅಕ್ಷರ *)dest;
    ಅಕ್ಷರ  *sUText = (ಅಕ್ಷರ *)src;

    ಆದರೆ (dptr >= (ಅಕ್ಷರ *)src->pExtra && dptr < ((ಅಕ್ಷರ*)src->pExtra)+src->extra_ಗಾತ್ರ) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((ಅಕ್ಷರ *)dest->pExtra) + (dptr - (ಅಕ್ಷರ *)src->pExtra);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dptr>=sUText && dptr < sUText+src->ಗಾತ್ರ_ದ_Struct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
ಸ್ಥಿರ UText * U_CALLCONV
shallow_ಪಠ್ಯ_Clone(UText * dest, ಸ್ಥಿರಾಂಕ UText * src, UError_ಸಂಕೇತ * ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    int32_t  src_Extra_ಗಾತ್ರ = src->extra_ಗಾತ್ರ;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_ಸಿದ್ಧತೆ(dest, src_Extra_ಗಾತ್ರ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    ಶೂನ್ಯ *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_ಗಮ್ಯ_Copy = src->ಗಾತ್ರ_ದ_Struct;
    ಆದರೆ (ಗಾತ್ರ_ಗಮ್ಯ_Copy > dest->ಗಾತ್ರ_ದ_Struct) {
        ಗಾತ್ರ_ಗಮ್ಯ_Copy = dest->ಗಾತ್ರ_ದ_Struct;
    }
    uprv_memcpy(dest, src, ಗಾತ್ರ_ಗಮ್ಯ_Copy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    ಆದರೆ (src_Extra_ಗಾತ್ರ > 0) {
        uprv_memcpy(dest->pExtra, src->pExtra, src_Extra_ಗಾತ್ರ);
    }

    //
    // Relocate any pointers in the target that refer to the UText itself
    //   to point to the cloned copy rather than the original source.
    //
    adjust_ಸೂಚಕ(dest, &dest->context, src);
    adjust_ಸೂಚಕ(dest, &dest->p, src);
    adjust_ಸೂಚಕ(dest, &dest->q, src);
    adjust_ಸೂಚಕ(dest, &dest->r, src);
    adjust_ಸೂಚಕ(dest, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ);

    ಹಿಂತಿರುಗಿ dest;
}


U_CDECL_ಕೊನೆ



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
ಎಣಿಕೆ { UTF8_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ=32 };

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

ರಚನೆ UTF8Buf {
    int32_t   buf_Native_ಆರಂಭ;                        // Native index of first char in char16_t buf
    int32_t   buf_Native_ಎಲ್ಲೆ;                        // Native index following last char in buf.
    int32_t   buf_ಆರಂಭ_Idx;                           // First filled position in buf.
    int32_t   buf_ಎಲ್ಲೆ_Idx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   ಗಮ್ಯ_UChars_Map_ಆರಂಭ;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   map_ಗಮ್ಯ_Native[UTF8_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   map_ಗಮ್ಯ_UChars[UTF8_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ*3+6]; // Map native offset from bufNativeStart to
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
ಸ್ಥಿರ int64_t U_CALLCONV
utf8_ಪಠ್ಯ_ಉದ್ದಳತೆ(UText *ut) {
    ಆದರೆ (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *r = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ut->context + ut->c;
        ಆಗ (*r != 0) {
            r++;
        }
        ಆದರೆ ((r - (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ut->context);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE);
    }
    ಹಿಂತಿರುಗಿ ut->b;
}






ಸ್ಥಿರ UBool U_CALLCONV
utf8_ಪಠ್ಯ_Access(UText *ut, int64_t ಸೂಚ್ಯಂಕ, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    ಸ್ಥಿರಾಂಕ uint8_t *s8=(ಸ್ಥಿರಾಂಕ uint8_t *)ut->context;
    UTF8Buf *u8b = ಶೂನ್ಯಸೂಚಕ;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)ಸೂಚ್ಯಂಕ;     // Requested index, trimmed to 32 bits.
    int32_t  map_ಸೂಚ್ಯಂಕ = 0;
    ಆದರೆ (ಸೂಚ್ಯಂಕ<0) {
        ix=0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೂಚ್ಯಂಕ > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    ಆದರೆ (ix>length) {
        ಆದರೆ (length>=0) {
            ix=length;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            ಆಗ (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            ಆದರೆ (s8[ut->c] == 0) {
                // We just found the actual length of the string.
                //  Trim the requested index back to that.
                ix     = ut->c;
                ut->b  = ut->c;
                length = ut->c;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    ಆದರೆ (forward) {
        ಆದರೆ (ix==ut->chunk_Native_ಎಲ್ಲೆ) {
            // Check for normal sequential iteration cases first.
            ಆದರೆ (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunk_ಸ್ಥಾನಾಂತರ = ut->chunk_ಉದ್ದಳತೆ;
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                ಆದರೆ (ix>=altB->buf_Native_ಆರಂಭ && ix<altB->buf_Native_ಎಲ್ಲೆ) {
                    ಹೋಗು ಅದಲುಬದಲು_Buffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            ಆದರೆ (ix>=u8b->buf_Native_ಆರಂಭ && ix<u8b->buf_Native_ಎಲ್ಲೆ) {
                // Requested index is in the other buffer.
                ಹೋಗು ಅದಲುಬದಲು_Buffers;
            }
            ಆದರೆ (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                ಆದರೆ (ix == ut->chunk_Native_ಎಲ್ಲೆ) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunk_ಸ್ಥಾನಾಂತರ = ut->chunk_ಉದ್ದಳತೆ;
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                ಆದರೆ (ix == u8b->buf_Native_ಎಲ್ಲೆ) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    ಹೋಗು ಅದಲುಬದಲು_Buffers_And_Fail;
                }

                // Neither existing buffer extends to the end of the string.
                ಹೋಗು make_Stub_ಬಫರ್;
            }

            ಆದರೆ (ix<ut->chunk_Native_ಆರಂಭ || ix>=ut->chunk_Native_ಎಲ್ಲೆ) {
                // Requested index is in neither buffer.
                ಹೋಗು fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            map_ಸೂಚ್ಯಂಕ = ix - u8b->ಗಮ್ಯ_UChars_Map_ಆರಂಭ;
            U_ASSERT(map_ಸೂಚ್ಯಂಕ < (int32_t)ಗಾತ್ರ(UTF8Buf::map_ಗಮ್ಯ_UChars));
            ut->chunk_ಸ್ಥಾನಾಂತರ = u8b->map_ಗಮ್ಯ_UChars[map_ಸೂಚ್ಯಂಕ] - u8b->buf_ಆರಂಭ_Idx;
            ಹಿಂತಿರುಗಿ ನಿಜ;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    ಆದರೆ (ix==ut->chunk_Native_ಆರಂಭ) {
        // Check for normal sequential iteration cases first.
        ಆದರೆ (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunk_ಸ್ಥಾನಾಂತರ = 0;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            ಆದರೆ (ix>altB->buf_Native_ಆರಂಭ && ix<=altB->buf_Native_ಎಲ್ಲೆ) {
                ಹೋಗು ಅದಲುಬದಲು_Buffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    ಆದರೆ (ix>u8b->buf_Native_ಆರಂಭ && ix<=u8b->buf_Native_ಎಲ್ಲೆ) {
        // Requested index is in the other buffer.
        ಹೋಗು ಅದಲುಬದಲು_Buffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    ಆದರೆ (ix==0) {
        ಆದರೆ (u8b->buf_Native_ಆರಂಭ==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            ಹೋಗು ಅದಲುಬದಲು_Buffers_And_Fail;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            ಹೋಗು make_Stub_ಬಫರ್;
        }
    }

    ಆದರೆ (ix<=ut->chunk_Native_ಆರಂಭ || ix>ut->chunk_Native_ಎಲ್ಲೆ) {
        // Requested index is in neither buffer.
        ಹೋಗು fill_ಹಿಮ್ಮುಖ;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    map_ಸೂಚ್ಯಂಕ = ix - u8b->ಗಮ್ಯ_UChars_Map_ಆರಂಭ;
    ut->chunk_ಸ್ಥಾನಾಂತರ = u8b->map_ಗಮ್ಯ_UChars[map_ಸೂಚ್ಯಂಕ] - u8b->buf_ಆರಂಭ_Idx;
    ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }



ಅದಲುಬದಲು_Buffers:
    //  The alternate buffer (ut->q) has the string data that was requested.
    //  Swap the primary and alternate buffers, and set the
    //   chunk index into the new primary buffer.
    {
        u8b   = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b;
        ut->chunkContents       = &u8b->buf[u8b->buf_ಆರಂಭ_Idx];
        ut->chunk_ಉದ್ದಳತೆ         = u8b->buf_ಎಲ್ಲೆ_Idx - u8b->buf_ಆರಂಭ_Idx;
        ut->chunk_Native_ಆರಂಭ    = u8b->buf_Native_ಆರಂಭ;
        ut->chunk_Native_ಎಲ್ಲೆ    = u8b->buf_Native_ಎಲ್ಲೆ;
        ut->native_Indexing_ಎಲ್ಲೆ = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->buf_Native_ಆರಂಭ);
        U_ASSERT(ix<=u8b->buf_Native_ಎಲ್ಲೆ);
        map_ಸೂಚ್ಯಂಕ = ix - u8b->ಗಮ್ಯ_UChars_Map_ಆರಂಭ;
        U_ASSERT(map_ಸೂಚ್ಯಂಕ>=0);
        U_ASSERT(map_ಸೂಚ್ಯಂಕ<(int32_t)ಗಾತ್ರ(u8b->map_ಗಮ್ಯ_UChars));
        ut->chunk_ಸ್ಥಾನಾಂತರ = u8b->map_ಗಮ್ಯ_UChars[map_ಸೂಚ್ಯಂಕ] - u8b->buf_ಆರಂಭ_Idx;

        ಹಿಂತಿರುಗಿ ನಿಜ;
    }


 ಅದಲುಬದಲು_Buffers_And_Fail:
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
    ut->chunkContents       = &u8b->buf[u8b->buf_ಆರಂಭ_Idx];
    ut->chunk_ಉದ್ದಳತೆ         = u8b->buf_ಎಲ್ಲೆ_Idx - u8b->buf_ಆರಂಭ_Idx;
    ut->chunk_Native_ಆರಂಭ    = u8b->buf_Native_ಆರಂಭ;
    ut->chunk_Native_ಎಲ್ಲೆ    = u8b->buf_Native_ಎಲ್ಲೆ;
    ut->native_Indexing_ಎಲ್ಲೆ = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    ಆದರೆ (ix==u8b->buf_Native_ಎಲ್ಲೆ) {
        ut->chunk_ಸ್ಥಾನಾಂತರ = ut->chunk_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ  {
        ut->chunk_ಸ್ಥಾನಾಂತರ = 0;
        U_ASSERT(ix == u8b->buf_Native_ಆರಂಭ);
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

make_Stub_ಬಫರ್:
    //   The user has done a seek/access past the start or end
    //   of the string.  Rather than loading data that is likely
    //   to never be used, just set up a zero-length buffer at
    //   the position.
    u8b = (UTF8Buf *)ut->q;
    u8b->buf_Native_ಆರಂಭ   = ix;
    u8b->buf_Native_ಎಲ್ಲೆ   = ix;
    u8b->buf_ಆರಂಭ_Idx      = 0;
    u8b->buf_ಎಲ್ಲೆ_Idx      = 0;
    u8b->bufNILimit       = 0;
    u8b->ಗಮ್ಯ_UChars_Map_ಆರಂಭ = ix;
    u8b->map_ಗಮ್ಯ_Native[0]   = 0;
    u8b->map_ಗಮ್ಯ_UChars[0]   = 0;
    ಹೋಗು ಅದಲುಬದಲು_Buffers_And_Fail;



fillForward:
    {
        // Move the incoming index to a code point boundary.
        U8_ಗಣ_CP_ಆರಂಭ(s8, 0, ix);

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_ಅದಲುಬದಲು = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_ಅದಲುಬದಲು;

        int32_t strLen = ut->b;
        UBool   nulTerminated = ಸುಳ್ಳು;
        ಆದರೆ (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = ನಿಜ;
        }

        char16_t   *buf = u8b_ಅದಲುಬದಲು->buf;
        uint8_t *map_ಗಮ್ಯ_Native  = u8b_ಅದಲುಬದಲು->map_ಗಮ್ಯ_Native;
        uint8_t *map_ಗಮ್ಯ_UChars  = u8b_ಅದಲುಬದಲು->map_ಗಮ್ಯ_UChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    ನೋಡಿದ_Non_Ascii = ಸುಳ್ಳು;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        ಆಗ (destIx<UTF8_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ) {
            c = s8[srcIx];
            ಆದರೆ (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                map_ಗಮ್ಯ_Native[destIx]    = (uint8_t)(srcIx - ix);
                map_ಗಮ್ಯ_UChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // General case, handle everything.
                ಆದರೆ (ನೋಡಿದ_Non_Ascii == ಸುಳ್ಳು) {
                    ನೋಡಿದ_Non_Ascii = ನಿಜ;
                    u8b_ಅದಲುಬದಲು->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_ಮುಂದಿನ_OR_FFFD(s8, srcIx, strLen, c);
                ಆದರೆ (c==0 && nulTerminated) {
                    srcIx--;
                    ನಿಲ್ಲಿಸು;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                ಮಾಡು {
                    map_ಗಮ್ಯ_Native[dIx++] = (uint8_t)(cIx - ix);
                } ಆಗ (dIx < destIx);

                ಮಾಡು {
                    map_ಗಮ್ಯ_UChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } ಆಗ (cIx < srcIx);
            }
            ಆದರೆ (srcIx>=strLen) {
                ನಿಲ್ಲಿಸು;
            }

        }

        //  store Native <--> Chunk Map entries for the end of the buffer.
        //    There is no actual character here, but the index position is valid.
        map_ಗಮ್ಯ_Native[destIx]     = (uint8_t)(srcIx - ix);
        map_ಗಮ್ಯ_UChars[srcIx - ix] = (uint8_t)destIx;

        //  fill in Buffer descriptor
        u8b_ಅದಲುಬದಲು->buf_Native_ಆರಂಭ     = ix;
        u8b_ಅದಲುಬದಲು->buf_Native_ಎಲ್ಲೆ     = srcIx;
        u8b_ಅದಲುಬದಲು->buf_ಆರಂಭ_Idx        = 0;
        u8b_ಅದಲುಬದಲು->buf_ಎಲ್ಲೆ_Idx        = destIx;
        ಆದರೆ (ನೋಡಿದ_Non_Ascii == ಸುಳ್ಳು) {
            u8b_ಅದಲುಬದಲು->bufNILimit     = destIx;
        }
        u8b_ಅದಲುಬದಲು->ಗಮ್ಯ_UChars_Map_ಆರಂಭ   = u8b_ಅದಲುಬದಲು->buf_Native_ಆರಂಭ;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunk_ಸ್ಥಾನಾಂತರ         = 0;
        ut->chunk_ಉದ್ದಳತೆ         = u8b_ಅದಲುಬದಲು->buf_ಎಲ್ಲೆ_Idx;
        ut->chunk_Native_ಆರಂಭ    = u8b_ಅದಲುಬದಲು->buf_Native_ಆರಂಭ;
        ut->chunk_Native_ಎಲ್ಲೆ    = u8b_ಅದಲುಬದಲು->buf_Native_ಎಲ್ಲೆ;
        ut->native_Indexing_ಎಲ್ಲೆ = u8b_ಅದಲುಬದಲು->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        ಆದರೆ (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            ಆದರೆ (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE);
            }
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }


fill_ಹಿಮ್ಮುಖ:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        ಆದರೆ (ix != ut->b) {
            // Note: this function will only move the index back if it is on a trail byte
            //       and there is a preceding lead byte and the sequence from the lead 
            //       through this trail could be part of a valid UTF-8 sequence
            //       Otherwise the index remains unchanged.
            U8_ಗಣ_CP_ಆರಂಭ(s8, 0, ix);
        }

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_ಅದಲುಬದಲು = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_ಅದಲುಬದಲು;

        char16_t   *buf = u8b_ಅದಲುಬದಲು->buf;
        uint8_t *map_ಗಮ್ಯ_Native = u8b_ಅದಲುಬದಲು->map_ಗಮ್ಯ_Native;
        uint8_t *map_ಗಮ್ಯ_UChars = u8b_ಅದಲುಬದಲು->map_ಗಮ್ಯ_UChars;
        int32_t  ಗಮ್ಯ_UChars_Map_ಆರಂಭ = ix - ಗಾತ್ರ(UTF8Buf::map_ಗಮ್ಯ_UChars) + 1;
        // Note that toUCharsMapStart can be negative. Happens when the remaining
        // text from current position to the beginning is less than the buffer size.
        // + 1 because mapToUChars must have a slot at the end for the bufNativeLimit entry.
        int32_t  destIx = UTF8_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ+2;   // Start in the overflow region
                                                    //   at end of buffer to leave room
                                                    //   for a surrogate pair at the
                                                    //   buffer start.
        int32_t  srcIx  = ix;
        int32_t  bufNILimit = destIx;
        UChar32   c;

        // Map to/from Native Indexes, fill in for the position at the end of
        //   the buffer.
        //
        map_ಗಮ್ಯ_Native[destIx] = (uint8_t)(srcIx - ಗಮ್ಯ_UChars_Map_ಆರಂಭ);
        map_ಗಮ್ಯ_UChars[srcIx - ಗಮ್ಯ_UChars_Map_ಆರಂಭ] = (uint8_t)destIx;

        // Fill the chunk buffer
        // Work backwards, filling from the end of the buffer towards the front.
        //
        ಆಗ (destIx>2 && (srcIx - ಗಮ್ಯ_UChars_Map_ಆರಂಭ > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            ಆದರೆ (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(ಗಮ್ಯ_UChars_Map_ಆರಂಭ <= srcIx);
                map_ಗಮ್ಯ_UChars[srcIx - ಗಮ್ಯ_UChars_Map_ಆರಂಭ] = (uint8_t)destIx;
                map_ಗಮ್ಯ_Native[destIx] = (uint8_t)(srcIx - ಗಮ್ಯ_UChars_Map_ಆರಂಭ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_prev_Char_ಸುರಕ್ಷಿತ_Body(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                ಆದರೆ (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    map_ಗಮ್ಯ_Native[destIx] = (uint8_t)(srcIx - ಗಮ್ಯ_UChars_Map_ಆರಂಭ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    buf[destIx]         = U16_TRAIL(c);
                    map_ಗಮ್ಯ_Native[destIx] = (uint8_t)(srcIx - ಗಮ್ಯ_UChars_Map_ಆರಂಭ);
                    buf[--destIx]       = U16_LEAD(c);
                    map_ಗಮ್ಯ_Native[destIx] = (uint8_t)(srcIx - ಗಮ್ಯ_UChars_Map_ಆರಂಭ);
                }

                // Fill in the map from native indexes to UChars buf index.
                ಮಾಡು {
                    map_ಗಮ್ಯ_UChars[sIx-- - ಗಮ್ಯ_UChars_Map_ಆರಂಭ] = (uint8_t)destIx;
                } ಆಗ (sIx >= srcIx);
                U_ASSERT(ಗಮ್ಯ_UChars_Map_ಆರಂಭ <= (srcIx+1));

                // Set native indexing limit to be the current position.
                //   We are processing a non-ascii, non-native-indexing char now;
                //     the limit will be here if the rest of the chars to be
                //     added to this buffer are ascii.
                bufNILimit = destIx;
            }
        }
        u8b_ಅದಲುಬದಲು->buf_Native_ಆರಂಭ     = srcIx;
        u8b_ಅದಲುಬದಲು->buf_Native_ಎಲ್ಲೆ     = ix;
        u8b_ಅದಲುಬದಲು->buf_ಆರಂಭ_Idx        = destIx;
        u8b_ಅದಲುಬದಲು->buf_ಎಲ್ಲೆ_Idx        = UTF8_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ+2;
        u8b_ಅದಲುಬದಲು->bufNILimit         = bufNILimit - u8b_ಅದಲುಬದಲು->buf_ಆರಂಭ_Idx;
        u8b_ಅದಲುಬದಲು->ಗಮ್ಯ_UChars_Map_ಆರಂಭ   = ಗಮ್ಯ_UChars_Map_ಆರಂಭ;

        ut->chunkContents       = &buf[u8b_ಅದಲುಬದಲು->buf_ಆರಂಭ_Idx];
        ut->chunk_ಉದ್ದಳತೆ         = u8b_ಅದಲುಬದಲು->buf_ಎಲ್ಲೆ_Idx - u8b_ಅದಲುಬದಲು->buf_ಆರಂಭ_Idx;
        ut->chunk_ಸ್ಥಾನಾಂತರ         = ut->chunk_ಉದ್ದಳತೆ;
        ut->chunk_Native_ಆರಂಭ    = u8b_ಅದಲುಬದಲು->buf_Native_ಆರಂಭ;
        ut->chunk_Native_ಎಲ್ಲೆ    = u8b_ಅದಲುಬದಲು->buf_Native_ಎಲ್ಲೆ;
        ut->native_Indexing_ಎಲ್ಲೆ = u8b_ಅದಲುಬದಲು->bufNILimit;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
ಸ್ಥಿರ char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t dest_ಸಾಮರ್ಥ್ಯ,
              int32_t *p_Dest_ಉದ್ದಳತೆ,
              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* src,
              int32_t src_ಉದ್ದಳತೆ,        // required.  NUL terminated not supported.
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ
              )
{

    char16_t *pDest = dest;
    char16_t *p_Dest_ಎಲ್ಲೆ = (dest!=ಶೂನ್ಯಸೂಚಕ)?(dest+dest_ಸಾಮರ್ಥ್ಯ):ಶೂನ್ಯಸೂಚಕ;
    UChar32 ch=0;
    int32_t ಸೂಚ್ಯಂಕ = 0;
    int32_t req_ಉದ್ದಳತೆ = 0;
    uint8_t* pSrc = (uint8_t*) src;


    ಆಗ((ಸೂಚ್ಯಂಕ < src_ಉದ್ದಳತೆ)&&(pDest<p_Dest_ಎಲ್ಲೆ)){
        ch = pSrc[ಸೂಚ್ಯಂಕ++];
        ಆದರೆ(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }ಇಲ್ಲದಿದ್ದರೆ{
            ch=utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body(pSrc, &ಸೂಚ್ಯಂಕ, src_ಉದ್ದಳತೆ, ch, -3);
            ಆದರೆ(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }ಇಲ್ಲದಿದ್ದರೆ{
                *(pDest++)=U16_LEAD(ch);
                ಆದರೆ(pDest<p_Dest_ಎಲ್ಲೆ){
                    *(pDest++)=U16_TRAIL(ch);
                }ಇಲ್ಲದಿದ್ದರೆ{
                    req_ಉದ್ದಳತೆ++;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    ಆಗ(ಸೂಚ್ಯಂಕ < src_ಉದ್ದಳತೆ){
        ch = pSrc[ಸೂಚ್ಯಂಕ++];
        ಆದರೆ(ch <= 0x7f){
            req_ಉದ್ದಳತೆ++;
        }ಇಲ್ಲದಿದ್ದರೆ{
            ch=utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body(pSrc, &ಸೂಚ್ಯಂಕ, src_ಉದ್ದಳತೆ, ch, -3);
            req_ಉದ್ದಳತೆ+=U16_ಉದ್ದಳತೆ(ch);
        }
    }

    req_ಉದ್ದಳತೆ+=(int32_t)(pDest - dest);

    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
        *p_Dest_ಉದ್ದಳತೆ = req_ಉದ್ದಳತೆ;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ಸಾಮರ್ಥ್ಯ,req_ಉದ್ದಳತೆ,p_Error_ಸಂಕೇತ);

    ಹಿಂತಿರುಗಿ dest;
}



ಸ್ಥಿರ int32_t U_CALLCONV
utf8_ಪಠ್ಯ_ಹೊರತೆಗೆ(UText *ut,
                int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
                char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ, length);
    int32_t  limit32 = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, length);

    ಆದರೆ(start32>limit32) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    ಸ್ಥಿರಾಂಕ uint8_t *buf = (ಸ್ಥಿರಾಂಕ uint8_t *)ut->context;
    ಪೂರ್ಣಾಂಕ i;
    ಆದರೆ (start32 < ut->chunk_Native_ಎಲ್ಲೆ) {
        ಗಾಗಿ (i=0; i<3; i++) {
            ಆದರೆ (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                ನಿಲ್ಲಿಸು;
            }
            start32--;
        }
    }

    ಆದರೆ (limit32 < ut->chunk_Native_ಎಲ್ಲೆ) {
        ಗಾಗಿ (i=0; i<3; i++) {
            ಆದರೆ (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                ನಿಲ್ಲಿಸು;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t dest_ಉದ್ದಳತೆ=0;
    utext_strFromUTF8(dest, dest_ಸಾಮರ್ಥ್ಯ, &dest_ಉದ್ದಳತೆ,
                    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ut->context+start32, limit32-start32,
                    p_Error_ಸಂಕೇತ);
    utf8_ಪಠ್ಯ_Access(ut, limit32, ನಿಜ);
    ಹಿಂತಿರುಗಿ dest_ಉದ್ದಳತೆ;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
ಸ್ಥಿರ int64_t U_CALLCONV
utf8_ಪಠ್ಯ_Map_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_Native(ಸ್ಥಿರಾಂಕ UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunk_ಸ್ಥಾನಾಂತರ>ut->native_Indexing_ಎಲ್ಲೆ && ut->chunk_ಸ್ಥಾನಾಂತರ<=ut->chunk_ಉದ್ದಳತೆ);
    int32_t native_ಸ್ಥಾನಾಂತರ = u8b->map_ಗಮ್ಯ_Native[ut->chunk_ಸ್ಥಾನಾಂತರ + u8b->buf_ಆರಂಭ_Idx] + u8b->ಗಮ್ಯ_UChars_Map_ಆರಂಭ;
    U_ASSERT(native_ಸ್ಥಾನಾಂತರ >= ut->chunk_Native_ಆರಂಭ && native_ಸ್ಥಾನಾಂತರ <= ut->chunk_Native_ಎಲ್ಲೆ);
    ಹಿಂತಿರುಗಿ native_ಸ್ಥಾನಾಂತರ;
}

//
// Map a native index to the corresponding chunk offset
//
ಸ್ಥಿರ int32_t U_CALLCONV
utf8_ಪಠ್ಯ_Map_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_UTF16(ಸ್ಥಿರಾಂಕ UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t ಸೂಚ್ಯಂಕ = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ಸೂಚ್ಯಂಕ>=ut->chunk_Native_ಆರಂಭ+ut->native_Indexing_ಎಲ್ಲೆ);
    U_ASSERT(ಸೂಚ್ಯಂಕ<=ut->chunk_Native_ಎಲ್ಲೆ);
    int32_t map_ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ - u8b->ಗಮ್ಯ_UChars_Map_ಆರಂಭ;
    U_ASSERT(map_ಸೂಚ್ಯಂಕ < (int32_t)ಗಾತ್ರ(UTF8Buf::map_ಗಮ್ಯ_UChars));
    int32_t ಸ್ಥಾನಾಂತರ = u8b->map_ಗಮ್ಯ_UChars[map_ಸೂಚ್ಯಂಕ] - u8b->buf_ಆರಂಭ_Idx;
    U_ASSERT(ಸ್ಥಾನಾಂತರ>=0 && ಸ್ಥಾನಾಂತರ<=ut->chunk_ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಸ್ಥಿರ UText * U_CALLCONV
utf8_ಪಠ್ಯ_Clone(UText *dest, ಸ್ಥಿರಾಂಕ UText *src, UBool deep, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_ಪಠ್ಯ_Clone(dest, src, ಸ್ಥಿತಿ_2);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //
    // TODO:  There is an issue with using utext_nativeLength().
    //        That function is non-const in cases where the input was NUL terminated
    //          and the length has not yet been determined.
    //        This function (clone()) is const.
    //        There potentially a thread safety issue lurking here.
    //
    ಆದರೆ (deep && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        int32_t  len = (int32_t)utext_native_ಉದ್ದಳತೆ((UText *)src);
        ಅಕ್ಷರ *copyStr = (ಅಕ್ಷರ *)uprv_malloc(len+1);
        ಆದರೆ (copyStr == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ);
        }
    }
    ಹಿಂತಿರುಗಿ dest;
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
utf8_ಪಠ್ಯ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ಆದರೆ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ)) {
        ಅಕ್ಷರ *s = (ಅಕ್ಷರ *)ut->context;
        uprv_free(s);
        ut->context = ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CDECL_ಕೊನೆ


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ UTextFuncs utf8Funcs =
{
    ಗಾತ್ರ(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8_ಪಠ್ಯ_Clone,
    utf8_ಪಠ್ಯ_ಉದ್ದಳತೆ,
    utf8_ಪಠ್ಯ_Access,
    utf8_ಪಠ್ಯ_ಹೊರತೆಗೆ,
    ಶೂನ್ಯಸೂಚಕ,                /* replace*/
    ಶೂನ್ಯಸೂಚಕ,                /* copy   */
    utf8_ಪಠ್ಯ_Map_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_Native,
    utf8_ಪಠ್ಯ_Map_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_UTF16,
    utf8_ಪಠ್ಯ_Close,
    ಶೂನ್ಯಸೂಚಕ,                // spare 1
    ಶೂನ್ಯಸೂಚಕ,                // spare 2
    ಶೂನ್ಯಸೂಚಕ                 // spare 3
};


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ g_ಖಾಲಿಗಳು_String[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s U_LIFETIME_BOUND,
               int64_t length,
               UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ && length==0) {
        s = g_ಖಾಲಿಗಳು_String;
    }

    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ || length<-1 || length>INT32_MAX) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ut = utext_ಸಿದ್ಧತೆ(ut, ಗಾತ್ರ(UTF8Buf) * 2, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    ಆದರೆ (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (ಅಕ್ಷರ *)ut->pExtra + ಗಾತ್ರ(UTF8Buf);
    ಹಿಂತಿರುಗಿ ut;

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
ಎಣಿಕೆ { REP_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ=10 };

ರಚನೆ ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ+1];
};


U_CDECL_BEGIN

ಸ್ಥಿರ UText * U_CALLCONV
rep_ಪಠ್ಯ_Clone(UText *dest, ಸ್ಥಿರಾಂಕ UText *src, UBool deep, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_ಪಠ್ಯ_Clone(dest, src, ಸ್ಥಿತಿ_2);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    ಆದರೆ (deep && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ಸ್ಥಿರಾಂಕ Replaceable *replSrc = (ಸ್ಥಿರಾಂಕ Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ಹಿಂತಿರುಗಿ dest;
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
rep_ಪಠ್ಯ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ಆದರೆ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ)) {
        Replaceable *rep = (Replaceable *)ut->context;
        ಅಳಿಸು rep;
        ut->context = ಶೂನ್ಯಸೂಚಕ;
    }
}


ಸ್ಥಿರ int64_t U_CALLCONV
rep_ಪಠ್ಯ_ಉದ್ದಳತೆ(UText *ut) {
    ಸ್ಥಿರಾಂಕ Replaceable *replSrc = (ಸ್ಥಿರಾಂಕ Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    ಹಿಂತಿರುಗಿ len;
}


ಸ್ಥಿರ UBool U_CALLCONV
rep_ಪಠ್ಯ_Access(UText *ut, int64_t ಸೂಚ್ಯಂಕ, UBool forward) {
    ಸ್ಥಿರಾಂಕ Replaceable *rep=(ಸ್ಥಿರಾಂಕ Replaceable *)ut->context;
    int32_t length=rep->length();   // Full length of the input text (bigger than a chunk)

    // clip the requested index to the limits of the text.
    int32_t index32 = pin_ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ, length);
    U_ASSERT(ಸೂಚ್ಯಂಕ<=INT32_MAX);


    /*
     * Compute start/limit boundaries around index, for a segment of text
     * to be extracted.
     * To allow for the possibility that our user gave an index to the trailing
     * half of a surrogate pair, we must request one extra preceding char16_t when
     * going in the forward direction.  This will ensure that the buffer has the
     * entire code point at the specified index.
     */
    ಆದರೆ(forward) {

        ಆದರೆ (index32>=ut->chunk_Native_ಆರಂಭ && index32<ut->chunk_Native_ಎಲ್ಲೆ) {
            // Buffer already contains the requested position.
            ut->chunk_ಸ್ಥಾನಾಂತರ = (int32_t)(ಸೂಚ್ಯಂಕ - ut->chunk_Native_ಆರಂಭ);
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಆದರೆ (index32>=length && ut->chunk_Native_ಎಲ್ಲೆ==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunk_ಸ್ಥಾನಾಂತರ = length - (int32_t)ut->chunk_Native_ಆರಂಭ;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        ut->chunk_Native_ಎಲ್ಲೆ = ಸೂಚ್ಯಂಕ + REP_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        ಆದರೆ(ut->chunk_Native_ಎಲ್ಲೆ > length) {
            ut->chunk_Native_ಎಲ್ಲೆ = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunk_Native_ಆರಂಭ = ut->chunk_Native_ಎಲ್ಲೆ - REP_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ;
        ಆದರೆ(ut->chunk_Native_ಆರಂಭ < 0) {
            ut->chunk_Native_ಆರಂಭ = 0;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        ಆದರೆ (index32>ut->chunk_Native_ಆರಂಭ && index32<=ut->chunk_Native_ಎಲ್ಲೆ) {
            // Requested position already in buffer.
            ut->chunk_ಸ್ಥಾನಾಂತರ = index32 - (int32_t)ut->chunk_Native_ಆರಂಭ;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಆದರೆ (index32==0 && ut->chunk_Native_ಆರಂಭ==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunk_ಸ್ಥಾನಾಂತರ = 0;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunk_Native_ಆರಂಭ = index32 + 1 - REP_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ;
        ಆದರೆ (ut->chunk_Native_ಆರಂಭ < 0) {
            ut->chunk_Native_ಆರಂಭ = 0;
        }

        ut->chunk_Native_ಎಲ್ಲೆ = index32 + 1;
        ಆದರೆ (ut->chunk_Native_ಎಲ್ಲೆ > length) {
            ut->chunk_Native_ಎಲ್ಲೆ = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString ಬಫರ್(ex->s, 0 /*buffer length*/, REP_ಪಠ್ಯ_CHUNK_ಗಾತ್ರ /*buffer capacity*/);
    rep->ಹೊರತೆಗೆ_Between((int32_t)ut->chunk_Native_ಆರಂಭ, (int32_t)ut->chunk_Native_ಎಲ್ಲೆ, ಬಫರ್);

    ut->chunkContents  = ex->s;
    ut->chunk_ಉದ್ದಳತೆ    = (int32_t)(ut->chunk_Native_ಎಲ್ಲೆ - ut->chunk_Native_ಆರಂಭ);
    ut->chunk_ಸ್ಥಾನಾಂತರ    = (int32_t)(index32 - ut->chunk_Native_ಆರಂಭ);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    ಆದರೆ (ut->chunk_Native_ಎಲ್ಲೆ < length &&
        U16_IS_LEAD(ex->s[ut->chunk_ಉದ್ದಳತೆ-1])) {
            ut->chunk_ಉದ್ದಳತೆ--;
            ut->chunk_Native_ಎಲ್ಲೆ--;
            ಆದರೆ (ut->chunk_ಸ್ಥಾನಾಂತರ > ut->chunk_ಉದ್ದಳತೆ) {
                ut->chunk_ಸ್ಥಾನಾಂತರ = ut->chunk_ಉದ್ದಳತೆ;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    ಆದರೆ(ut->chunk_Native_ಆರಂಭ>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunk_Native_ಆರಂಭ);
        --(ut->chunk_ಉದ್ದಳತೆ);
        --(ut->chunk_ಸ್ಥಾನಾಂತರ);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_ಗಣ_CP_ಆರಂಭ(ut->chunkContents, 0, ut->chunk_ಸ್ಥಾನಾಂತರ);

    // Use fast indexing for get/setNativeIndex()
    ut->native_Indexing_ಎಲ್ಲೆ = ut->chunk_ಉದ್ದಳತೆ;

    ಹಿಂತಿರುಗಿ ನಿಜ;
}



ಸ್ಥಿರ int32_t U_CALLCONV
rep_ಪಠ್ಯ_ಹೊರತೆಗೆ(UText *ut,
               int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
               char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
               UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ Replaceable *rep=(ಸ್ಥಿರಾಂಕ Replaceable *)ut->context;
    int32_t  length=rep->length();

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0)) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
    }
    ಆದರೆ(ಆರಂಭ>ಎಲ್ಲೆ) {
        *ಸ್ಥಿತಿ_2=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t  start32 = pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ, length);
    int32_t  limit32 = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, length);

    // adjust start, limit if they point to trail half of surrogates
    ಆದರೆ (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    ಆದರೆ (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    ಆದರೆ(length>dest_ಸಾಮರ್ಥ್ಯ) {
        limit32 = start32 + dest_ಸಾಮರ್ಥ್ಯ;
    }
    UnicodeString ಬಫರ್(dest, 0, dest_ಸಾಮರ್ಥ್ಯ); // writable alias
    rep->ಹೊರತೆಗೆ_Between(start32, limit32, ಬಫರ್);
    rep_ಪಠ್ಯ_Access(ut, limit32, ನಿಜ);

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, length, ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರ int32_t U_CALLCONV
rep_ಪಠ್ಯ_Replace(UText *ut,
               int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
               ಸ್ಥಿರಾಂಕ char16_t *src, int32_t length,
               UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t old_ಉದ್ದಳತೆ;

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(src==ಶೂನ್ಯಸೂಚಕ && length!=0) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    old_ಉದ್ದಳತೆ=rep->length(); // will subtract from new length
    ಆದರೆ(ಆರಂಭ>ಎಲ್ಲೆ ) {
        *ಸ್ಥಿತಿ_2=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t start32 = pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ, old_ಉದ್ದಳತೆ);
    int32_t limit32 = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, old_ಉದ್ದಳತೆ);

    // Snap start & limit to code point boundaries.
    ಆದರೆ (start32<old_ಉದ್ದಳತೆ && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    ಆದರೆ (limit32<old_ಉದ್ದಳತೆ && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t new_ಉದ್ದಳತೆ = rep->length();
    int32_t ಉದ್ದಳತೆ_Delta = new_ಉದ್ದಳತೆ - old_ಉದ್ದಳತೆ;

    // Is the UText chunk buffer OK?
    ಆದರೆ (ut->chunk_Native_ಎಲ್ಲೆ > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t new_ಸೂಚ್ಯಂಕ_Pos = limit32 + ಉದ್ದಳತೆ_Delta;
    rep_ಪಠ್ಯ_Access(ut, new_ಸೂಚ್ಯಂಕ_Pos, ನಿಜ);

    ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_Delta;
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
rep_ಪಠ್ಯ_Copy(UText *ut,
                int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
                int64_t dest_ಸೂಚ್ಯಂಕ,
                UBool move,
                UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಆರಂಭ>ಎಲ್ಲೆ || (ಆರಂಭ<dest_ಸೂಚ್ಯಂಕ && dest_ಸೂಚ್ಯಂಕ<ಎಲ್ಲೆ))
    {
        *ಸ್ಥಿತಿ_2=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    int32_t start32     = pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ, length);
    int32_t limit32     = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, length);
    int32_t destIndex32 = pin_ಸೂಚ್ಯಂಕ(dest_ಸೂಚ್ಯಂಕ, length);

    // TODO:  snap input parameters to code point boundaries.

    ಆದರೆ(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t seg_ಉದ್ದಳತೆ=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        ಆದರೆ(destIndex32<start32) {
            start32+=seg_ಉದ್ದಳತೆ;
            limit32+=seg_ಉದ್ದಳತೆ;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } ಇಲ್ಲದಿದ್ದರೆ {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t ಮೊದಲ_Affected_ಸೂಚ್ಯಂಕ = destIndex32;
    ಆದರೆ (move && start32<ಮೊದಲ_Affected_ಸೂಚ್ಯಂಕ) {
        ಮೊದಲ_Affected_ಸೂಚ್ಯಂಕ = start32;
    }
    ಆದರೆ (ಮೊದಲ_Affected_ಸೂಚ್ಯಂಕ < ut->chunk_Native_ಎಲ್ಲೆ) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  native_Iter_ಸೂಚ್ಯಂಕ = destIndex32 + limit32 - start32;
    ಆದರೆ (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        native_Iter_ಸೂಚ್ಯಂಕ = destIndex32;
    }

    // Set position, reload chunk if needed.
    rep_ಪಠ್ಯ_Access(ut, native_Iter_ಸೂಚ್ಯಂಕ, ನಿಜ);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ UTextFuncs repFuncs =
{
    ಗಾತ್ರ(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    rep_ಪಠ್ಯ_Clone,
    rep_ಪಠ್ಯ_ಉದ್ದಳತೆ,
    rep_ಪಠ್ಯ_Access,
    rep_ಪಠ್ಯ_ಹೊರತೆಗೆ,
    rep_ಪಠ್ಯ_Replace,
    rep_ಪಠ್ಯ_Copy,
    ಶೂನ್ಯಸೂಚಕ,              // MapOffsetToNative,
    ಶೂನ್ಯಸೂಚಕ,              // MapIndexToUTF16,
    rep_ಪಠ್ಯ_Close,
    ಶೂನ್ಯಸೂಚಕ,              // spare 1
    ಶೂನ್ಯಸೂಚಕ,              // spare 2
    ಶೂನ್ಯಸೂಚಕ               // spare 3
};


U_CAPI UText* U_EXPORT2
utext_openReplaceable(UText* ut U_LIFETIME_BOUND,
                      Replaceable* rep U_LIFETIME_BOUND,
                      UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(rep==ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ut = utext_ಸಿದ್ಧತೆ(ut, ಗಾತ್ರ(ReplExtra), ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    ಆದರೆ(rep->has_Meta_ದತ್ತಾಂಶ()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_ದತ್ತಾಂಶ);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    ಹಿಂತಿರುಗಿ ut;
}

U_CDECL_ಕೊನೆ








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


ಸ್ಥಿರ UText * U_CALLCONV
unistr_ಪಠ್ಯ_Clone(UText *dest, ಸ್ಥಿರಾಂಕ UText *src, UBool deep, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_ಪಠ್ಯ_Clone(dest, src, ಸ್ಥಿತಿ_2);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    ಆದರೆ (deep && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ಸ್ಥಿರಾಂಕ UnicodeString *srcString = (ಸ್ಥಿರಾಂಕ UnicodeString *)src->context;
        dest->context = ಹೊಸ UnicodeString(*srcString);
        dest->chunkContents = ((ಸ್ಥಿರಾಂಕ UnicodeString *)dest->context)->get_ಬಫರ್();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ಹಿಂತಿರುಗಿ dest;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
unistr_ಪಠ್ಯ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ಆದರೆ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        ಅಳಿಸು str;
        ut->context = ಶೂನ್ಯಸೂಚಕ;
    }
}


ಸ್ಥಿರ int64_t U_CALLCONV
unistr_ಪಠ್ಯ_ಉದ್ದಳತೆ(UText *t) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ UnicodeString *)t->context)->length();
}


ಸ್ಥಿರ UBool U_CALLCONV
unistr_ಪಠ್ಯ_Access(UText *ut, int64_t ಸೂಚ್ಯಂಕ, UBool  forward) {
    int32_t length  = ut->chunk_ಉದ್ದಳತೆ;
    ut->chunk_ಸ್ಥಾನಾಂತರ = pin_ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && ಸೂಚ್ಯಂಕ<length) || (!forward && ಸೂಚ್ಯಂಕ>0);
    ಹಿಂತಿರುಗಿ retVal;
}



ಸ್ಥಿರ int32_t U_CALLCONV
unistr_ಪಠ್ಯ_ಹೊರತೆಗೆ(UText *t,
                  int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
                  char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UnicodeString *us=(ಸ್ಥಿರಾಂಕ UnicodeString *)t->context;
    int32_t length=us->length();

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    }
    ಆದರೆ(ಆರಂಭ<0 || ಆರಂಭ>ಎಲ್ಲೆ) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t start32 = ಆರಂಭ<length ? us->get_Char32_ಆರಂಭ((int32_t)ಆರಂಭ) : length;
    int32_t limit32 = ಎಲ್ಲೆ<length ? us->get_Char32_ಆರಂಭ((int32_t)ಎಲ್ಲೆ) : length;

    length=limit32-start32;
    ಆದರೆ (dest_ಸಾಮರ್ಥ್ಯ>0 && dest!=ಶೂನ್ಯಸೂಚಕ) {
        int32_t trimmed_ಉದ್ದಳತೆ = length;
        ಆದರೆ(trimmed_ಉದ್ದಳತೆ>dest_ಸಾಮರ್ಥ್ಯ) {
            trimmed_ಉದ್ದಳತೆ=dest_ಸಾಮರ್ಥ್ಯ;
        }
        us->extract(start32, trimmed_ಉದ್ದಳತೆ, dest);
        t->chunk_ಸ್ಥಾನಾಂತರ = start32+trimmed_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        t->chunk_ಸ್ಥಾನಾಂತರ = start32;
    }
    u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, length, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
unistr_ಪಠ್ಯ_Replace(UText *ut,
                  int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
                  ಸ್ಥಿರಾಂಕ char16_t *src, int32_t length,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t old_ಉದ್ದಳತೆ;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(src==ಶೂನ್ಯಸೂಚಕ && length!=0) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    }
    ಆದರೆ(ಆರಂಭ>ಎಲ್ಲೆ) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    old_ಉದ್ದಳತೆ=us->length();
    int32_t start32 = pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ, old_ಉದ್ದಳತೆ);
    int32_t limit32 = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, old_ಉದ್ದಳತೆ);
    ಆದರೆ (start32 < old_ಉದ್ದಳತೆ) {
        start32 = us->get_Char32_ಆರಂಭ(start32);
    }
    ಆದರೆ (limit32 < old_ಉದ್ದಳತೆ) {
        limit32 = us->get_Char32_ಆರಂಭ(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t new_ಉದ್ದಳತೆ = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->get_ಬಫರ್();
    ut->chunk_ಉದ್ದಳತೆ      = new_ಉದ್ದಳತೆ;
    ut->chunk_Native_ಎಲ್ಲೆ = new_ಉದ್ದಳತೆ;
    ut->native_Indexing_ಎಲ್ಲೆ = new_ಉದ್ದಳತೆ;

    // Set iteration position to the point just following the newly inserted text.
    int32_t ಉದ್ದಳತೆ_Delta = new_ಉದ್ದಳತೆ - old_ಉದ್ದಳತೆ;
    ut->chunk_ಸ್ಥಾನಾಂತರ = limit32 + ಉದ್ದಳತೆ_Delta;

    ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_Delta;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
unistr_ಪಠ್ಯ_Copy(UText *ut,
               int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
               int64_t dest_ಸೂಚ್ಯಂಕ,
               UBool move,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    int32_t start32 = pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ, length);
    int32_t limit32 = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, length);
    int32_t destIndex32 = pin_ಸೂಚ್ಯಂಕ(dest_ಸೂಚ್ಯಂಕ, length);

    ಆದರೆ( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(move) {
        // move: copy to destIndex, then remove original
        int32_t seg_ಉದ್ದಳತೆ=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        ಆದರೆ(destIndex32<start32) {
            start32+=seg_ಉದ್ದಳತೆ;
        }
        us->remove(start32, seg_ಉದ್ದಳತೆ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->get_ಬಫರ್();
    ಆದರೆ (move==ಸುಳ್ಳು) {
        // copy operation, string length grows
        ut->chunk_ಉದ್ದಳತೆ += limit32-start32;
        ut->chunk_Native_ಎಲ್ಲೆ = ut->chunk_ಉದ್ದಳತೆ;
        ut->native_Indexing_ಎಲ್ಲೆ = ut->chunk_ಉದ್ದಳತೆ;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunk_ಸ್ಥಾನಾಂತರ = destIndex32+limit32-start32;
    ಆದರೆ (move && destIndex32>start32) {
        ut->chunk_ಸ್ಥಾನಾಂತರ = destIndex32;
    }

}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ UTextFuncs unistrFuncs =
{
    ಗಾತ್ರ(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistr_ಪಠ್ಯ_Clone,
    unistr_ಪಠ್ಯ_ಉದ್ದಳತೆ,
    unistr_ಪಠ್ಯ_Access,
    unistr_ಪಠ್ಯ_ಹೊರತೆಗೆ,
    unistr_ಪಠ್ಯ_Replace,
    unistr_ಪಠ್ಯ_Copy,
    ಶೂನ್ಯಸೂಚಕ,                // MapOffsetToNative,
    ಶೂನ್ಯಸೂಚಕ,                // MapIndexToUTF16,
    unistr_ಪಠ್ಯ_Close,
    ಶೂನ್ಯಸೂಚಕ,                // spare 1
    ಶೂನ್ಯಸೂಚಕ,                // spare 2
    ಶೂನ್ಯಸೂಚಕ                 // spare 3
};



U_CDECL_ಕೊನೆ


U_CAPI UText* U_EXPORT2
utext_openUnicodeString(UText* ut U_LIFETIME_BOUND,
                        UnicodeString* s U_LIFETIME_BOUND,
                        UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ut = utext_openConstUnicodeString(ut, s, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ಹಿಂತಿರುಗಿ ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             ಸ್ಥಿರಾಂಕ UnicodeString* s U_LIFETIME_BOUND,
                             UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, ಶೂನ್ಯಸೂಚಕ, 0, ಸ್ಥಿತಿ_2);
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ut;
    }
    ut = utext_ಸಿದ್ಧತೆ(ut, 0, ಸ್ಥಿತಿ_2);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->get_ಬಫರ್();
        ut->chunk_ಉದ್ದಳತೆ         = s->length();
        ut->chunk_Native_ಆರಂಭ    = 0;
        ut->chunk_Native_ಎಲ್ಲೆ    = ut->chunk_ಉದ್ದಳತೆ;
        ut->native_Indexing_ಎಲ್ಲೆ = ut->chunk_ಉದ್ದಳತೆ;
    }
    ಹಿಂತಿರುಗಿ ut;
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


ಸ್ಥಿರ UText * U_CALLCONV
ucstr_ಪಠ್ಯ_Clone(UText *dest, ಸ್ಥಿರಾಂಕ UText * src, UBool deep, UError_ಸಂಕೇತ * ಸ್ಥಿತಿ_2) {
    // First do a generic shallow clone.
    dest = shallow_ಪಠ್ಯ_Clone(dest, src, ಸ್ಥಿತಿ_2);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    ಆದರೆ (deep && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        U_ASSERT(utext_native_ಉದ್ದಳತೆ(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_native_ಉದ್ದಳತೆ(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        ಸ್ಥಿರಾಂಕ char16_t *srcStr = (ಸ್ಥಿರಾಂಕ char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * ಗಾತ್ರ(char16_t));
        ಆದರೆ (copyStr == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            int64_t i;
            ಗಾಗಿ (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ);
        }
    }
    ಹಿಂತಿರುಗಿ dest;
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucstr_ಪಠ್ಯ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    ಆದರೆ (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_ಪಠ್ಯ)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = ಶೂನ್ಯಸೂಚಕ;
    }
}



ಸ್ಥಿರ int64_t U_CALLCONV
ucstr_ಪಠ್ಯ_ಉದ್ದಳತೆ(UText *ut) {
    ಆದರೆ (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        ಸ್ಥಿರಾಂಕ char16_t  *str = (ಸ್ಥಿರಾಂಕ char16_t *)ut->context;
        ಗಾಗಿ (;;) {
            ಆದರೆ (str[ut->chunk_Native_ಎಲ್ಲೆ] == 0) {
                ನಿಲ್ಲಿಸು;
            }
            ut->chunk_Native_ಎಲ್ಲೆ++;
        }
        ut->a = ut->chunk_Native_ಎಲ್ಲೆ;
        ut->chunk_ಉದ್ದಳತೆ = (int32_t)ut->chunk_Native_ಎಲ್ಲೆ;
        ut->native_Indexing_ಎಲ್ಲೆ = ut->chunk_ಉದ್ದಳತೆ;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE);
    }
    ಹಿಂತಿರುಗಿ ut->a;
}


ಸ್ಥಿರ UBool U_CALLCONV
ucstr_ಪಠ್ಯ_Access(UText *ut, int64_t ಸೂಚ್ಯಂಕ, UBool  forward) {
    ಸ್ಥಿರಾಂಕ char16_t *str   = (ಸ್ಥಿರಾಂಕ char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    ಆದರೆ (ಸೂಚ್ಯಂಕ<0) {
        ಸೂಚ್ಯಂಕ = 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೂಚ್ಯಂಕ < ut->chunk_Native_ಎಲ್ಲೆ) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_ಗಣ_CP_ಆರಂಭ(str, 0, ಸೂಚ್ಯಂಕ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        ಸೂಚ್ಯಂಕ = ut->a;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t ಪರಿಶೀಲನೆ_ಎಲ್ಲೆ = (int32_t)ಸೂಚ್ಯಂಕ + 32;
        ಆದರೆ ((ಸೂಚ್ಯಂಕ + 32)>INT32_MAX || (ಸೂಚ್ಯಂಕ + 32)<0 ) {   // note: int64 expression
            ಪರಿಶೀಲನೆ_ಎಲ್ಲೆ = INT32_MAX;
        }

        int32_t chunk_ಎಲ್ಲೆ = (int32_t)ut->chunk_Native_ಎಲ್ಲೆ;
        ಗಾಗಿ (; chunk_ಎಲ್ಲೆ<ಪರಿಶೀಲನೆ_ಎಲ್ಲೆ; chunk_ಎಲ್ಲೆ++) {
            ಆದರೆ (str[chunk_ಎಲ್ಲೆ] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunk_ಎಲ್ಲೆ;
                ut->chunk_ಉದ್ದಳತೆ = chunk_ಎಲ್ಲೆ;
                ut->native_Indexing_ಎಲ್ಲೆ = chunk_ಎಲ್ಲೆ;
                ಆದರೆ (ಸೂಚ್ಯಂಕ >= chunk_ಎಲ್ಲೆ) {
                    ಸೂಚ್ಯಂಕ = chunk_ಎಲ್ಲೆ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    U16_ಗಣ_CP_ಆರಂಭ(str, 0, ಸೂಚ್ಯಂಕ);
                }

                ut->chunk_Native_ಎಲ್ಲೆ = chunk_ಎಲ್ಲೆ;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE);
                ಹೋಗು breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_ಗಣ_CP_ಆರಂಭ(str, 0, ಸೂಚ್ಯಂಕ);
        ಆದರೆ (chunk_ಎಲ್ಲೆ == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunk_ಎಲ್ಲೆ;
            ut->chunk_ಉದ್ದಳತೆ = chunk_ಎಲ್ಲೆ;
            ut->native_Indexing_ಎಲ್ಲೆ = chunk_ಎಲ್ಲೆ;
            ಆದರೆ (ಸೂಚ್ಯಂಕ > chunk_ಎಲ್ಲೆ) {
                ಸೂಚ್ಯಂಕ = chunk_ಎಲ್ಲೆ;
            }
            ut->chunk_Native_ಎಲ್ಲೆ = chunk_ಎಲ್ಲೆ;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            ಆದರೆ (U16_IS_LEAD(str[chunk_ಎಲ್ಲೆ-1])) {
                --chunk_ಎಲ್ಲೆ;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunk_Native_ಎಲ್ಲೆ = chunk_ಎಲ್ಲೆ;
            ut->native_Indexing_ಎಲ್ಲೆ = chunk_ಎಲ್ಲೆ;
            ut->chunk_ಉದ್ದಳತೆ = chunk_ಎಲ್ಲೆ;
        }

    }
breakout:
    U_ASSERT(ಸೂಚ್ಯಂಕ<=INT32_MAX);
    ut->chunk_ಸ್ಥಾನಾಂತರ = (int32_t)ಸೂಚ್ಯಂಕ;

    // Check whether request is at the start or end
    UBool retVal = (forward && ಸೂಚ್ಯಂಕ<ut->chunk_Native_ಎಲ್ಲೆ) || (!forward && ಸೂಚ್ಯಂಕ>0);
    ಹಿಂತಿರುಗಿ retVal;
}



ಸ್ಥಿರ int32_t U_CALLCONV
ucstr_ಪಠ್ಯ_ಹೊರತೆಗೆ(UText *ut,
                  int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
                  char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0) || ಆರಂಭ>ಎಲ್ಲೆ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstr_ಪಠ್ಯ_Access(ut, ಆರಂಭ, ನಿಜ);
    ಸ್ಥಿರಾಂಕ char16_t *s=ut->chunkContents;
    start32 = ut->chunk_ಸ್ಥಾನಾಂತರ;

    int32_t str_ಉದ್ದಳತೆ=(int32_t)ut->a;
    ಆದರೆ (str_ಉದ್ದಳತೆ >= 0) {
        limit32 = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, str_ಉದ್ದಳತೆ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        limit32 = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, INT32_MAX);
    }
    di = 0;
    ಗಾಗಿ (si=start32; si<limit32; si++) {
        ಆದರೆ (str_ಉದ್ದಳತೆ<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunk_Native_ಎಲ್ಲೆ    = si;
            ut->chunk_ಉದ್ದಳತೆ         = si;
            ut->native_Indexing_ಎಲ್ಲೆ = si;
            str_ಉದ್ದಳತೆ               = si;
            limit32                 = si;
            ನಿಲ್ಲಿಸು;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        ಆದರೆ (di<dest_ಸಾಮರ್ಥ್ಯ) {
            // only store if there is space.
            dest[di] = s[si];
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (str_ಉದ್ದಳತೆ>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                ನಿಲ್ಲಿಸು;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    ಆದರೆ (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<str_ಉದ್ದಳತೆ || str_ಉದ್ದಳತೆ<0)  && U16_IS_TRAIL(s[si])))
    {
        ಆದರೆ (di<dest_ಸಾಮರ್ಥ್ಯ) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    ಆದರೆ (si <= ut->chunk_Native_ಎಲ್ಲೆ) {
        ut->chunk_ಸ್ಥಾನಾಂತರ = si;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ucstr_ಪಠ್ಯ_Access(ut, si, ನಿಜ);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, di, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ di;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ UTextFuncs ucstrFuncs =
{
    ಗಾತ್ರ(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstr_ಪಠ್ಯ_Clone,
    ucstr_ಪಠ್ಯ_ಉದ್ದಳತೆ,
    ucstr_ಪಠ್ಯ_Access,
    ucstr_ಪಠ್ಯ_ಹೊರತೆಗೆ,
    ಶೂನ್ಯಸೂಚಕ,              // Replace
    ಶೂನ್ಯಸೂಚಕ,              // Copy
    ಶೂನ್ಯಸೂಚಕ,              // MapOffsetToNative,
    ಶೂನ್ಯಸೂಚಕ,              // MapIndexToUTF16,
    ucstr_ಪಠ್ಯ_Close,
    ಶೂನ್ಯಸೂಚಕ,              // spare 1
    ಶೂನ್ಯಸೂಚಕ,              // spare 2
    ಶೂನ್ಯಸೂಚಕ,              // spare 3
};

U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t g_ಖಾಲಿಗಳು_UString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ && length==0) {
        s = g_ಖಾಲಿಗಳು_UString;
    }
    ಆದರೆ (s==ಶೂನ್ಯಸೂಚಕ || length < -1 || length>INT32_MAX) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ut = utext_ಸಿದ್ಧತೆ(ut, 0, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ಆದರೆ (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_ಉದ್ದಳತೆ_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunk_Native_ಆರಂಭ     = 0;
        ut->chunk_Native_ಎಲ್ಲೆ     = length>=0? length : 0;
        ut->chunk_ಉದ್ದಳತೆ          = (int32_t)ut->chunk_Native_ಎಲ್ಲೆ;
        ut->chunk_ಸ್ಥಾನಾಂತರ          = 0;
        ut->native_Indexing_ಎಲ್ಲೆ  = ut->chunk_ಉದ್ದಳತೆ;
    }
    ಹಿಂತಿರುಗಿ ut;
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
#ವ್ಯಾಖ್ಯಾನಿಸು CIBuf_ಗಾತ್ರ 16

U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
char_Iter_ಪಠ್ಯ_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    ಅಳಿಸು ci;
    ut->r = ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರ int64_t U_CALLCONV
char_Iter_ಪಠ್ಯ_ಉದ್ದಳತೆ(UText *ut) {
    ಹಿಂತಿರುಗಿ (int32_t)ut->a;
}

ಸ್ಥಿರ UBool U_CALLCONV
char_Iter_ಪಠ್ಯ_Access(UText *ut, int64_t ಸೂಚ್ಯಂಕ, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clipped_ಸೂಚ್ಯಂಕ = (int32_t)ಸೂಚ್ಯಂಕ;
    ಆದರೆ (clipped_ಸೂಚ್ಯಂಕ<0) {
        clipped_ಸೂಚ್ಯಂಕ=0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipped_ಸೂಚ್ಯಂಕ>=ut->a) {
        clipped_ಸೂಚ್ಯಂಕ=(int32_t)ut->a;
    }
    int32_t needed_ಸೂಚ್ಯಂಕ = clipped_ಸೂಚ್ಯಂಕ;
    ಆದರೆ (!forward && needed_ಸೂಚ್ಯಂಕ>0) {
        // reverse iteration, want the position just before what was asked for.
        needed_ಸೂಚ್ಯಂಕ--;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (forward && needed_ಸೂಚ್ಯಂಕ==ut->a && needed_ಸೂಚ್ಯಂಕ>0) {
        // Forward iteration, don't ask for something past the end of the text.
        needed_ಸೂಚ್ಯಂಕ--;
    }

    // Find the native index of the start of the buffer containing what we want.
    needed_ಸೂಚ್ಯಂಕ -= needed_ಸೂಚ್ಯಂಕ % CIBuf_ಗಾತ್ರ;

    char16_t *buf = ಶೂನ್ಯಸೂಚಕ;
    UBool  need_Chunk_ಸಿದ್ಧತೆ = ನಿಜ;
    ಪೂರ್ಣಾಂಕ    i;
    ಆದರೆ (ut->chunk_Native_ಆರಂಭ == needed_ಸೂಚ್ಯಂಕ) {
        // The buffer we want is already the current chunk.
        need_Chunk_ಸಿದ್ಧತೆ = ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ut->b == needed_ಸೂಚ್ಯಂಕ) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ut->c == needed_ಸೂಚ್ಯಂಕ) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        ಆದರೆ (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->ಗಣ_ಸೂಚ್ಯಂಕ(needed_ಸೂಚ್ಯಂಕ);
        ಗಾಗಿ (i=0; i<CIBuf_ಗಾತ್ರ; i++) {
            buf[i] = ci->ಮುಂದಿನ_Post_Inc();
            ಆದರೆ (i+needed_ಸೂಚ್ಯಂಕ > ut->a) {
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    ಆದರೆ (need_Chunk_ಸಿದ್ಧತೆ) {
        ut->chunkContents = buf;
        ut->chunk_ಉದ್ದಳತೆ   = CIBuf_ಗಾತ್ರ;
        ut->chunk_Native_ಆರಂಭ = needed_ಸೂಚ್ಯಂಕ;
        ut->chunk_Native_ಎಲ್ಲೆ = needed_ಸೂಚ್ಯಂಕ + CIBuf_ಗಾತ್ರ;
        ಆದರೆ (ut->chunk_Native_ಎಲ್ಲೆ > ut->a) {
            ut->chunk_Native_ಎಲ್ಲೆ = ut->a;
            ut->chunk_ಉದ್ದಳತೆ  = (int32_t)(ut->chunk_Native_ಎಲ್ಲೆ)-(int32_t)(ut->chunk_Native_ಆರಂಭ);
        }
        ut->native_Indexing_ಎಲ್ಲೆ = ut->chunk_ಉದ್ದಳತೆ;
        U_ASSERT(ut->chunk_ಸ್ಥಾನಾಂತರ>=0 && ut->chunk_ಸ್ಥಾನಾಂತರ<=CIBuf_ಗಾತ್ರ);
    }
    ut->chunk_ಸ್ಥಾನಾಂತರ = clipped_ಸೂಚ್ಯಂಕ - (int32_t)ut->chunk_Native_ಆರಂಭ;
    UBool success = (forward? ut->chunk_ಸ್ಥಾನಾಂತರ<ut->chunk_ಉದ್ದಳತೆ : ut->chunk_ಸ್ಥಾನಾಂತರ>0);
    ಹಿಂತಿರುಗಿ success;
}

ಸ್ಥಿರ UText * U_CALLCONV
char_Iter_ಪಠ್ಯ_Clone(UText *dest, ಸ್ಥಿರಾಂಕ UText *src, UBool deep, UError_ಸಂಕೇತ * ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_get_Native_ಸೂಚ್ಯಂಕ((UText *)src);
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    ಹಿಂತಿರುಗಿ dest;
}

ಸ್ಥಿರ int32_t U_CALLCONV
char_Iter_ಪಠ್ಯ_ಹೊರತೆಗೆ(UText *ut,
                  int64_t ಆರಂಭ, int64_t ಎಲ್ಲೆ,
                  char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                  UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0) || ಆರಂಭ>ಎಲ್ಲೆ) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ, length);
    int32_t  limit32 = pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copy_ಎಲ್ಲೆ;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->ಗಣ_Index32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->get_ಸೂಚ್ಯಂಕ();
    copy_ಎಲ್ಲೆ = srci;
    ಆಗ (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_ಉದ್ದಳತೆ(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        ಆದರೆ (desti+len <= dest_ಸಾಮರ್ಥ್ಯ) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copy_ಎಲ್ಲೆ = srci+len;
        } ಇಲ್ಲದಿದ್ದರೆ {
            desti += len;
            *ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
        srci += len;
    }

    char_Iter_ಪಠ್ಯ_Access(ut, copy_ಎಲ್ಲೆ, ನಿಜ);

    u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, desti, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ desti;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ UTextFuncs charIterFuncs =
{
    ಗಾತ್ರ(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    char_Iter_ಪಠ್ಯ_Clone,
    char_Iter_ಪಠ್ಯ_ಉದ್ದಳತೆ,
    char_Iter_ಪಠ್ಯ_Access,
    char_Iter_ಪಠ್ಯ_ಹೊರತೆಗೆ,
    ಶೂನ್ಯಸೂಚಕ,                // Replace
    ಶೂನ್ಯಸೂಚಕ,                // Copy
    ಶೂನ್ಯಸೂಚಕ,                // MapOffsetToNative,
    ಶೂನ್ಯಸೂಚಕ,                // MapIndexToUTF16,
    char_Iter_ಪಠ್ಯ_Close,
    ಶೂನ್ಯಸೂಚಕ,                // spare 1
    ಶೂನ್ಯಸೂಚಕ,                // spare 2
    ಶೂನ್ಯಸೂಚಕ                 // spare 3
};
U_CDECL_ಕೊನೆ

U_CAPI UText* U_EXPORT2
utext_openCharacterIterator(UText* ut U_LIFETIME_BOUND,
                            CharacterIterator* ci U_LIFETIME_BOUND,
                            UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (ci->ಆರಂಭ_ಸೂಚ್ಯಂಕ() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBuf_ಗಾತ್ರ * ಗಾತ್ರ(char16_t);
    ut = utext_ಸಿದ್ಧತೆ(ut, extraSpace, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->ಕೊನೆ_ಸೂಚ್ಯಂಕ();        // Length of text
        ut->p                    = ut->pExtra;            // First buffer
        ut->b                    = -1;                    // Native index of first buffer contents
        ut->q                    = (char16_t*)ut->pExtra+CIBuf_ಗಾತ್ರ;  // Second buffer
        ut->c                    = -1;                    // Native index of second buffer contents

        // Initialize current chunk contents to be empty.
        //   First access will fault something in.
        //   Note:  The initial nativeStart and chunkOffset must sum to zero
        //          so that getNativeIndex() will correctly compute to zero
        //          if no call to Access() has ever been made.  They can't be both
        //          zero without Access() thinking that the chunk is valid.
        ut->chunkContents        = (char16_t *)ut->p;
        ut->chunk_Native_ಆರಂಭ     = -1;
        ut->chunk_ಸ್ಥಾನಾಂತರ          = 1;
        ut->chunk_Native_ಎಲ್ಲೆ     = 0;
        ut->chunk_ಉದ್ದಳತೆ          = 0;
        ut->native_Indexing_ಎಲ್ಲೆ  = ut->chunk_ಸ್ಥಾನಾಂತರ;  // enables native indexing
    }
    ಹಿಂತಿರುಗಿ ut;
}

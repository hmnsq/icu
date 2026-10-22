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

#ضمن <cstddef>

#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/chariter.h"
#ضمن "unicode/utext.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "ustr_imp.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uassert.h"
#ضمن "putilimp.h"

U_NAMESPACE_USE

#تعریف_کرو I32_FLAG(ثنائی_ہندسہ_اشاریہ) ((int32_t)1<<(ثنائی_ہندسہ_اشاریہ))


ساكن UBool
utext_access(UText *ut, int64_t اشاریہ, UBool forward) {
    أرجع ut->pFuncs->access(ut, اشاریہ, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    إذا (delta > 0) {
        افعل {
            إذا(ut->chunk_وتھ>=ut->chunk_لمائی && !utext_access(ut, ut->chunk_Native_حد_بندی, سچ)) {
                أرجع خطأ;
            }
            c = ut->chunkContents[ut->chunk_وتھ];
            إذا (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                إذا (c == U_SENTINEL) {
                    أرجع خطأ;
                }
            } وإلا {
                ut->chunk_وتھ++;
            }
        } طالما(--delta>0);

    } وإلا إذا (delta<0) {
        افعل {
            إذا(ut->chunk_وتھ<=0 && !utext_access(ut, ut->chunk_Native_شروع, خطأ)) {
                أرجع خطأ;
            }
            c = ut->chunkContents[ut->chunk_وتھ-1];
            إذا (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                إذا (c == U_SENTINEL) {
                    أرجع خطأ;
                }
            } وإلا {
                ut->chunk_وتھ--;
            }
        } طالما(++delta<0);
    }

    أرجع سچ;
}


U_CAPI int64_t U_EXPORT2
utext_native_لمائی(UText *ut) {
    أرجع ut->pFuncs->native_لمائی(ut);
}


U_CAPI UBool U_EXPORT2
utext_is_لمائی_Expensive(ثابت UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE)) != 0;
    أرجع r;
}


U_CAPI int64_t U_EXPORT2
utext_get_Native_اشاریہ(ثابت UText *ut) {
    إذا(ut->chunk_وتھ <= ut->native_Indexing_حد_بندی) {
        أرجع ut->chunk_Native_شروع+ut->chunk_وتھ;
    } وإلا {
        أرجع ut->pFuncs->map_وتھ_منزل_Native(ut);
    }
}


U_CAPI فراغ U_EXPORT2
utext_سیٹ_Native_اشاریہ(UText *ut, int64_t اشاریہ) {
    إذا(اشاریہ<ut->chunk_Native_شروع || اشاریہ>=ut->chunk_Native_حد_بندی) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, اشاریہ, سچ);
    } وإلا إذا((int32_t)(اشاریہ - ut->chunk_Native_شروع) <= ut->native_Indexing_حد_بندی) {
        // utf-16 indexing.
        ut->chunk_وتھ=(int32_t)(اشاریہ-ut->chunk_Native_شروع);
    } وإلا {
         ut->chunk_وتھ=ut->pFuncs->map_Native_اشاریہ_منزل_UTF16(ut, اشاریہ);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    إذا (ut->chunk_وتھ<ut->chunk_لمائی) {
        char16_t c= ut->chunkContents[ut->chunk_وتھ];
        إذا (U16_IS_TRAIL(c)) {
            إذا (ut->chunk_وتھ==0) {
                ut->pFuncs->access(ut, ut->chunk_Native_شروع, خطأ);
            }
            إذا (ut->chunk_وتھ>0) {
                char16_t lead = ut->chunkContents[ut->chunk_وتھ-1];
                إذا (U16_IS_LEAD(lead)) {
                    ut->chunk_وتھ--;
                }
            }
        }
    }
}



U_CAPI int64_t U_EXPORT2
utext_get_پچھلا_Native_اشاریہ(UText *ut) {
    //
    //  Fast-path the common case.
    //     Common means current position is not at the beginning of a chunk
    //     and the preceding character is not supplementary.
    //
    int32_t i = ut->chunk_وتھ - 1;
    int64_t نتیجہ;
    إذا (i >= 0) {
        char16_t c = ut->chunkContents[i];
        إذا (U16_IS_TRAIL(c) == خطأ) {
            إذا (i <= ut->native_Indexing_حد_بندی) {
                نتیجہ = ut->chunk_Native_شروع + i;
            } وإلا {
                ut->chunk_وتھ = i;
                نتیجہ = ut->pFuncs->map_وتھ_منزل_Native(ut);
                ut->chunk_وتھ++;
            }
            أرجع نتیجہ;
        }
    }

    // If at the start of text, simply return 0.
    إذا (ut->chunk_وتھ==0 && ut->chunk_Native_شروع==0) {
        أرجع 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    نتیجہ = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    أرجع نتیجہ;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    إذا (ut->chunk_وتھ==ut->chunk_لمائی) {
        // Current position is just off the end of the chunk.
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_حد_بندی, سچ) == خطأ) {
            // Off the end of the text.
            أرجع U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_وتھ];
    إذا (U16_IS_LEAD(c) == خطأ) {
        // Normal, non-supplementary case.
        أرجع c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    إذا ((ut->chunk_وتھ+1) < ut->chunk_لمائی) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunk_وتھ+1];
    } وإلا {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunk_Native_حد_بندی;
        إذا (ut->pFuncs->access(ut, nativePosition, سچ)) {
            trail = ut->chunkContents[ut->chunk_وتھ];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, خطأ);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunk_وتھ = ut->chunk_لمائی - 1;
        إذا(!r) {
            أرجع U_SENTINEL;
        }
    }

    إذا (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    أرجع supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t native_اشاریہ) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    إذا (native_اشاریہ>=ut->chunk_Native_شروع && native_اشاریہ < ut->chunk_Native_شروع + ut->native_Indexing_حد_بندی) {
        ut->chunk_وتھ = (int32_t)(native_اشاریہ - ut->chunk_Native_شروع);
        c = ut->chunkContents[ut->chunk_وتھ];
        إذا (U16_IS_SURROGATE(c) == خطأ) {
            أرجع c;
        }
    }


    utext_سیٹ_Native_اشاریہ(ut, native_اشاریہ);
    إذا (native_اشاریہ>=ut->chunk_Native_شروع && ut->chunk_وتھ<ut->chunk_لمائی) {
        c = ut->chunkContents[ut->chunk_وتھ];
        إذا (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    أرجع c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    إذا (ut->chunk_وتھ >= ut->chunk_لمائی) {
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_حد_بندی, سچ) == خطأ) {
            أرجع U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_وتھ++];
    إذا (U16_IS_LEAD(c) == خطأ) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        أرجع c;
    }

    إذا (ut->chunk_وتھ >= ut->chunk_لمائی) {
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_حد_بندی, سچ) == خطأ) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            أرجع c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunk_وتھ];
    إذا (U16_IS_TRAIL(trail) == خطأ) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        أرجع c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunk_وتھ++;   // move iteration position over the trail surrogate.
    أرجع supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    إذا (ut->chunk_وتھ <= 0) {
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_شروع, خطأ) == خطأ) {
            أرجع U_SENTINEL;
        }
    }
    ut->chunk_وتھ--;
    c = ut->chunkContents[ut->chunk_وتھ];
    إذا (U16_IS_TRAIL(c) == خطأ) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        أرجع c;
    }

    إذا (ut->chunk_وتھ <= 0) {
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_شروع, خطأ) == خطأ) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            أرجع c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunk_وتھ-1];
    إذا (U16_IS_LEAD(lead) == خطأ) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        أرجع c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunk_وتھ--;   // move iteration position over the lead surrogate.
    أرجع supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t اشاریہ) {
    UChar32       c      = U_SENTINEL;

    إذا(اشاریہ<ut->chunk_Native_شروع || اشاریہ>=ut->chunk_Native_حد_بندی) {
        // Desired position is outside of the current chunk.
        إذا(!ut->pFuncs->access(ut, اشاریہ, سچ)) {
            // no chunk available here
            أرجع U_SENTINEL;
        }
    } وإلا إذا (اشاریہ - ut->chunk_Native_شروع  <= (int64_t)ut->native_Indexing_حد_بندی) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunk_وتھ = (int32_t)(اشاریہ - ut->chunk_Native_شروع);
    } وإلا {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunk_وتھ = ut->pFuncs->map_Native_اشاریہ_منزل_UTF16(ut, اشاریہ);
    }

    c = ut->chunkContents[ut->chunk_وتھ++];
    إذا (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_سیٹ_Native_اشاریہ(ut, اشاریہ);
        c = utext_next32(ut);
    }
    أرجع c;
}


U_CAPI UChar32 U_EXPORT2
utext_previous32From(UText *ut, int64_t اشاریہ) {
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
    إذا(اشاریہ<=ut->chunk_Native_شروع || اشاریہ>ut->chunk_Native_حد_بندی) {
        // Requested native index is outside of the current chunk.
        إذا(!ut->pFuncs->access(ut, اشاریہ, خطأ)) {
            // no chunk available here
            أرجع U_SENTINEL;
        }
    } وإلا إذا(اشاریہ - ut->chunk_Native_شروع <= (int64_t)ut->native_Indexing_حد_بندی) {
        // Direct UTF-16 indexing.
        ut->chunk_وتھ = (int32_t)(اشاریہ - ut->chunk_Native_شروع);
    } وإلا {
        ut->chunk_وتھ=ut->pFuncs->map_Native_اشاریہ_منزل_UTF16(ut, اشاریہ);
        إذا (ut->chunk_وتھ==0 && !ut->pFuncs->access(ut, اشاریہ, خطأ)) {
            // no chunk available here
            أرجع U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunk_وتھ--;
    cPrev = ut->chunkContents[ut->chunk_وتھ];

    إذا (U16_IS_SURROGATE(cPrev)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_سیٹ_Native_اشاریہ(ut, اشاریہ);
        cPrev = utext_previous32(ut);
    }
    أرجع cPrev;
}


U_CAPI int32_t U_EXPORT2
utext_کڈھو(UText *ut,
             int64_t شروع, int64_t حد_بندی,
             char16_t *dest, int32_t dest_سمرتھا,
             UError_کوڈ *حالت) {
                 أرجع ut->pFuncs->extract(ut, شروع, حد_بندی, dest, dest_سمرتھا, حالت);
             }



U_CAPI UBool U_EXPORT2
utext_equals(ثابت UText *a, ثابت UText *b) {
    إذا (a==خالی_سوچک || b==خالی_سوچک ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            أرجع خطأ;
    }

    إذا (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        أرجع خطأ;
    }

    إذا (a->context != b->context) {
        // Different sources (different strings)
        أرجع خطأ;
    }
    إذا (utext_get_Native_اشاریہ(a) != utext_get_Native_اشاریہ(b)) {
        // Different current position in the string.
        أرجع خطأ;
    }

    أرجع سچ;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(ثابت UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    أرجع b;
}


U_CAPI فراغ U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_has_Meta_انکڑے(ثابت UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_انکڑے)) != 0;
    أرجع b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t native_شروع, int64_t native_حد_بندی,
             ثابت char16_t *replacement_متن, int32_t replacement_لمائی,
             UError_کوڈ *حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *حالت = U_NO_WRITE_PERMISSION;
        أرجع 0;
    }
    int32_t i = ut->pFuncs->replace(ut, native_شروع, native_حد_بندی, replacement_متن, replacement_لمائی, حالت);
    أرجع i;
}

U_CAPI فراغ U_EXPORT2
utext_copy(UText *ut,
          int64_t native_شروع, int64_t native_حد_بندی,
          int64_t dest_اشاریہ,
          UBool move,
          UError_کوڈ *حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع;
    }
    إذا ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *حالت = U_NO_WRITE_PERMISSION;
        أرجع;
    }
    ut->pFuncs->copy(ut, native_شروع, native_حد_بندی, dest_اشاریہ, move, حالت);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            ثابت UText* src,
            UBool deep,
            UBool readOnly,
            UError_کوڈ* حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع dest;
    }
    UText *نتیجہ = src->pFuncs->clone(dest, src, deep, حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع نتیجہ;
    }
    إذا (نتیجہ == خالی_سوچک) {
        *حالت = U_حافظہ_ونڈ_ERROR;
        أرجع نتیجہ;
    }
    إذا (readOnly) {
        utext_freeze(نتیجہ);
    }
    أرجع نتیجہ;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
تعداد {
    UTEXT_ڈھیر_ALLOCATED  = 1,      //  1 if ICU has allocated this UText struct on the heap.
                                    //  0 if caller provided storage for the UText.

    UTEXT_EXTRA_ڈھیر_ALLOCATED = 2, //  1 if ICU has allocated extra storage as a separate
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

بنية ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

ساكن ثابت UText خالی_تھاواں_متن = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_تیاری(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UError_کوڈ* حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع ut;
    }

    إذا (ut == خالی_سوچک) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = حجم(UText);
        إذا (extraSpace > 0) {
            spaceRequired = حجم(ExtendedUText) + extraSpace - حجم(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        إذا (ut == خالی_سوچک) {
            *حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        } وإلا {
            *ut = خالی_تھاواں_متن;
            ut->flags |= UTEXT_ڈھیر_ALLOCATED;
            إذا (spaceRequired>0) {
                ut->extra_ناپ = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } وإلا {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        إذا (ut->magic != UTEXT_MAGIC) {
            *حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        إذا ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != خالی_سوچک)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        إذا (extraSpace > ut->extra_ناپ) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            إذا (ut->flags & UTEXT_EXTRA_ڈھیر_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extra_ناپ = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            إذا (ut->pExtra == خالی_سوچک) {
                *حالت = U_حافظہ_ونڈ_ERROR;
            } وإلا {
                ut->extra_ناپ = extraSpace;
                ut->flags |= UTEXT_EXTRA_ڈھیر_ALLOCATED;
            }
        }
    }
    إذا (U_SUCCESS(*حالت)) {
        ut->flags |= UTEXT_OPEN;

        // Initialize all remaining fields of the UText.
        //
        ut->context             = خالی_سوچک;
        ut->chunkContents       = خالی_سوچک;
        ut->p                   = خالی_سوچک;
        ut->q                   = خالی_سوچک;
        ut->r                   = خالی_سوچک;
        ut->a                   = 0;
        ut->b                   = 0;
        ut->c                   = 0;
        ut->chunk_وتھ         = 0;
        ut->chunk_لمائی         = 0;
        ut->chunk_Native_شروع    = 0;
        ut->chunk_Native_حد_بندی    = 0;
        ut->native_Indexing_حد_بندی = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = خالی_سوچک;
        إذا (ut->pExtra!=خالی_سوچک && ut->extra_ناپ>0)
            uprv_memset(ut->pExtra, 0, ut->extra_ناپ);

    }
    أرجع ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    إذا (ut==خالی_سوچک ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        أرجع ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    إذا (ut->pFuncs->close != خالی_سوچک) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    إذا (ut->flags & UTEXT_EXTRA_ڈھیر_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = خالی_سوچک;
        ut->flags &= ~UTEXT_EXTRA_ڈھیر_ALLOCATED;
        ut->extra_ناپ = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = خالی_سوچک;

    إذا (ut->flags & UTEXT_ڈھیر_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = خالی_سوچک;
    }
    أرجع ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
ساكن فراغ
invalidateChunk(UText *ut) {
    ut->chunk_لمائی = 0;
    ut->chunk_Native_حد_بندی = 0;
    ut->chunk_Native_شروع = 0;
    ut->chunk_وتھ = 0;
    ut->native_Indexing_حد_بندی = 0;
}

//
// pinIndex        Do range pinning on a native index parameter.
//                 64 bit pinning is done in place.
//                 32 bit truncated result is returned as a convenience for
//                        use in providers that don't need 64 bits.
ساكن int32_t
pin_اشاریہ(int64_t &اشاریہ, int64_t حد_بندی) {
    إذا (اشاریہ<0) {
        اشاریہ = 0;
    } وإلا إذا (اشاریہ > حد_بندی) {
        اشاریہ = حد_بندی;
    }
    أرجع static_cast<int32_t>(اشاریہ);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
ساكن فراغ adjust_اشارہ_گر(UText *dest, ثابت فراغ **destPtr, ثابت UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    حرف  *dptr = (حرف *)*destPtr;
    حرف  *dUText = (حرف *)dest;
    حرف  *sUText = (حرف *)src;

    إذا (dptr >= (حرف *)src->pExtra && dptr < ((حرف*)src->pExtra)+src->extra_ناپ) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((حرف *)dest->pExtra) + (dptr - (حرف *)src->pExtra);
    } وإلا إذا (dptr>=sUText && dptr < sUText+src->ناپ_دا_Struct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
ساكن UText * U_CALLCONV
shallow_متن_Clone(UText * dest, ثابت UText * src, UError_کوڈ * حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    int32_t  src_Extra_ناپ = src->extra_ناپ;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_تیاری(dest, src_Extra_ناپ, حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    فراغ *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    صحيح ناپ_منزل_Copy = src->ناپ_دا_Struct;
    إذا (ناپ_منزل_Copy > dest->ناپ_دا_Struct) {
        ناپ_منزل_Copy = dest->ناپ_دا_Struct;
    }
    uprv_memcpy(dest, src, ناپ_منزل_Copy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    إذا (src_Extra_ناپ > 0) {
        uprv_memcpy(dest->pExtra, src->pExtra, src_Extra_ناپ);
    }

    //
    // Relocate any pointers in the target that refer to the UText itself
    //   to point to the cloned copy rather than the original source.
    //
    adjust_اشارہ_گر(dest, &dest->context, src);
    adjust_اشارہ_گر(dest, &dest->p, src);
    adjust_اشارہ_گر(dest, &dest->q, src);
    adjust_اشارہ_گر(dest, &dest->r, src);
    adjust_اشارہ_گر(dest, (ثابت فراغ **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_متن);

    أرجع dest;
}


U_CDECL_سرا



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
تعداد { UTF8_متن_CHUNK_ناپ=32 };

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

بنية UTF8Buf {
    int32_t   buf_Native_شروع;                        // Native index of first char in char16_t buf
    int32_t   buf_Native_حد_بندی;                        // Native index following last char in buf.
    int32_t   buf_شروع_Idx;                           // First filled position in buf.
    int32_t   buf_حد_بندی_Idx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   منزل_UChars_Map_شروع;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_متن_CHUNK_ناپ+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   map_منزل_Native[UTF8_متن_CHUNK_ناپ+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   map_منزل_UChars[UTF8_متن_CHUNK_ناپ*3+6]; // Map native offset from bufNativeStart to
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
ساكن int64_t U_CALLCONV
utf8_متن_لمائی(UText *ut) {
    إذا (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        ثابت حرف *r = (ثابت حرف *)ut->context + ut->c;
        طالما (*r != 0) {
            r++;
        }
        إذا ((r - (ثابت حرف *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (ثابت حرف *)ut->context);
        } وإلا {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE);
    }
    أرجع ut->b;
}






ساكن UBool U_CALLCONV
utf8_متن_Access(UText *ut, int64_t اشاریہ, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    ثابت uint8_t *s8=(ثابت uint8_t *)ut->context;
    UTF8Buf *u8b = خالی_سوچک;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)اشاریہ;     // Requested index, trimmed to 32 bits.
    int32_t  map_اشاریہ = 0;
    إذا (اشاریہ<0) {
        ix=0;
    } وإلا إذا (اشاریہ > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    إذا (ix>length) {
        إذا (length>=0) {
            ix=length;
        } وإلا إذا (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            طالما (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            إذا (s8[ut->c] == 0) {
                // We just found the actual length of the string.
                //  Trim the requested index back to that.
                ix     = ut->c;
                ut->b  = ut->c;
                length = ut->c;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    إذا (forward) {
        إذا (ix==ut->chunk_Native_حد_بندی) {
            // Check for normal sequential iteration cases first.
            إذا (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunk_وتھ = ut->chunk_لمائی;
                أرجع خطأ;
            } وإلا {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                إذا (ix>=altB->buf_Native_شروع && ix<altB->buf_Native_حد_بندی) {
                    اذهب وٹاندرا_Buffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            إذا (ix>=u8b->buf_Native_شروع && ix<u8b->buf_Native_حد_بندی) {
                // Requested index is in the other buffer.
                اذهب وٹاندرا_Buffers;
            }
            إذا (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                إذا (ix == ut->chunk_Native_حد_بندی) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunk_وتھ = ut->chunk_لمائی;
                    أرجع خطأ;
                }
                إذا (ix == u8b->buf_Native_حد_بندی) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    اذهب وٹاندرا_Buffers_And_Fail;
                }

                // Neither existing buffer extends to the end of the string.
                اذهب make_Stub_بفر;
            }

            إذا (ix<ut->chunk_Native_شروع || ix>=ut->chunk_Native_حد_بندی) {
                // Requested index is in neither buffer.
                اذهب fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            map_اشاریہ = ix - u8b->منزل_UChars_Map_شروع;
            U_ASSERT(map_اشاریہ < (int32_t)حجم(UTF8Buf::map_منزل_UChars));
            ut->chunk_وتھ = u8b->map_منزل_UChars[map_اشاریہ] - u8b->buf_شروع_Idx;
            أرجع سچ;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    إذا (ix==ut->chunk_Native_شروع) {
        // Check for normal sequential iteration cases first.
        إذا (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunk_وتھ = 0;
            أرجع خطأ;
        } وإلا {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            إذا (ix>altB->buf_Native_شروع && ix<=altB->buf_Native_حد_بندی) {
                اذهب وٹاندرا_Buffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    إذا (ix>u8b->buf_Native_شروع && ix<=u8b->buf_Native_حد_بندی) {
        // Requested index is in the other buffer.
        اذهب وٹاندرا_Buffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    إذا (ix==0) {
        إذا (u8b->buf_Native_شروع==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            اذهب وٹاندرا_Buffers_And_Fail;
        } وإلا {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            اذهب make_Stub_بفر;
        }
    }

    إذا (ix<=ut->chunk_Native_شروع || ix>ut->chunk_Native_حد_بندی) {
        // Requested index is in neither buffer.
        اذهب fill_الٹ;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    map_اشاریہ = ix - u8b->منزل_UChars_Map_شروع;
    ut->chunk_وتھ = u8b->map_منزل_UChars[map_اشاریہ] - u8b->buf_شروع_Idx;
    إذا (ut->chunk_وتھ==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        أرجع خطأ;
    } وإلا {
        أرجع سچ;
    }



وٹاندرا_Buffers:
    //  The alternate buffer (ut->q) has the string data that was requested.
    //  Swap the primary and alternate buffers, and set the
    //   chunk index into the new primary buffer.
    {
        u8b   = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b;
        ut->chunkContents       = &u8b->buf[u8b->buf_شروع_Idx];
        ut->chunk_لمائی         = u8b->buf_حد_بندی_Idx - u8b->buf_شروع_Idx;
        ut->chunk_Native_شروع    = u8b->buf_Native_شروع;
        ut->chunk_Native_حد_بندی    = u8b->buf_Native_حد_بندی;
        ut->native_Indexing_حد_بندی = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->buf_Native_شروع);
        U_ASSERT(ix<=u8b->buf_Native_حد_بندی);
        map_اشاریہ = ix - u8b->منزل_UChars_Map_شروع;
        U_ASSERT(map_اشاریہ>=0);
        U_ASSERT(map_اشاریہ<(int32_t)حجم(u8b->map_منزل_UChars));
        ut->chunk_وتھ = u8b->map_منزل_UChars[map_اشاریہ] - u8b->buf_شروع_Idx;

        أرجع سچ;
    }


 وٹاندرا_Buffers_And_Fail:
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
    ut->chunkContents       = &u8b->buf[u8b->buf_شروع_Idx];
    ut->chunk_لمائی         = u8b->buf_حد_بندی_Idx - u8b->buf_شروع_Idx;
    ut->chunk_Native_شروع    = u8b->buf_Native_شروع;
    ut->chunk_Native_حد_بندی    = u8b->buf_Native_حد_بندی;
    ut->native_Indexing_حد_بندی = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    إذا (ix==u8b->buf_Native_حد_بندی) {
        ut->chunk_وتھ = ut->chunk_لمائی;
    } وإلا  {
        ut->chunk_وتھ = 0;
        U_ASSERT(ix == u8b->buf_Native_شروع);
    }
    أرجع خطأ;

make_Stub_بفر:
    //   The user has done a seek/access past the start or end
    //   of the string.  Rather than loading data that is likely
    //   to never be used, just set up a zero-length buffer at
    //   the position.
    u8b = (UTF8Buf *)ut->q;
    u8b->buf_Native_شروع   = ix;
    u8b->buf_Native_حد_بندی   = ix;
    u8b->buf_شروع_Idx      = 0;
    u8b->buf_حد_بندی_Idx      = 0;
    u8b->bufNILimit       = 0;
    u8b->منزل_UChars_Map_شروع = ix;
    u8b->map_منزل_Native[0]   = 0;
    u8b->map_منزل_UChars[0]   = 0;
    اذهب وٹاندرا_Buffers_And_Fail;



fillForward:
    {
        // Move the incoming index to a code point boundary.
        U8_سیٹ_CP_شروع(s8, 0, ix);

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_وٹاندرا = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_وٹاندرا;

        int32_t strLen = ut->b;
        UBool   nulTerminated = خطأ;
        إذا (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = سچ;
        }

        char16_t   *buf = u8b_وٹاندرا->buf;
        uint8_t *map_منزل_Native  = u8b_وٹاندرا->map_منزل_Native;
        uint8_t *map_منزل_UChars  = u8b_وٹاندرا->map_منزل_UChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    تکیا_Non_Ascii = خطأ;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        طالما (destIx<UTF8_متن_CHUNK_ناپ) {
            c = s8[srcIx];
            إذا (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                map_منزل_Native[destIx]    = (uint8_t)(srcIx - ix);
                map_منزل_UChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } وإلا {
                // General case, handle everything.
                إذا (تکیا_Non_Ascii == خطأ) {
                    تکیا_Non_Ascii = سچ;
                    u8b_وٹاندرا->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_اگلا_OR_FFFD(s8, srcIx, strLen, c);
                إذا (c==0 && nulTerminated) {
                    srcIx--;
                    اكسر;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                افعل {
                    map_منزل_Native[dIx++] = (uint8_t)(cIx - ix);
                } طالما (dIx < destIx);

                افعل {
                    map_منزل_UChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } طالما (cIx < srcIx);
            }
            إذا (srcIx>=strLen) {
                اكسر;
            }

        }

        //  store Native <--> Chunk Map entries for the end of the buffer.
        //    There is no actual character here, but the index position is valid.
        map_منزل_Native[destIx]     = (uint8_t)(srcIx - ix);
        map_منزل_UChars[srcIx - ix] = (uint8_t)destIx;

        //  fill in Buffer descriptor
        u8b_وٹاندرا->buf_Native_شروع     = ix;
        u8b_وٹاندرا->buf_Native_حد_بندی     = srcIx;
        u8b_وٹاندرا->buf_شروع_Idx        = 0;
        u8b_وٹاندرا->buf_حد_بندی_Idx        = destIx;
        إذا (تکیا_Non_Ascii == خطأ) {
            u8b_وٹاندرا->bufNILimit     = destIx;
        }
        u8b_وٹاندرا->منزل_UChars_Map_شروع   = u8b_وٹاندرا->buf_Native_شروع;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunk_وتھ         = 0;
        ut->chunk_لمائی         = u8b_وٹاندرا->buf_حد_بندی_Idx;
        ut->chunk_Native_شروع    = u8b_وٹاندرا->buf_Native_شروع;
        ut->chunk_Native_حد_بندی    = u8b_وٹاندرا->buf_Native_حد_بندی;
        ut->native_Indexing_حد_بندی = u8b_وٹاندرا->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        إذا (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            إذا (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE);
            }
        }
        أرجع سچ;
    }


fill_الٹ:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        إذا (ix != ut->b) {
            // Note: this function will only move the index back if it is on a trail byte
            //       and there is a preceding lead byte and the sequence from the lead 
            //       through this trail could be part of a valid UTF-8 sequence
            //       Otherwise the index remains unchanged.
            U8_سیٹ_CP_شروع(s8, 0, ix);
        }

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_وٹاندرا = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_وٹاندرا;

        char16_t   *buf = u8b_وٹاندرا->buf;
        uint8_t *map_منزل_Native = u8b_وٹاندرا->map_منزل_Native;
        uint8_t *map_منزل_UChars = u8b_وٹاندرا->map_منزل_UChars;
        int32_t  منزل_UChars_Map_شروع = ix - حجم(UTF8Buf::map_منزل_UChars) + 1;
        // Note that toUCharsMapStart can be negative. Happens when the remaining
        // text from current position to the beginning is less than the buffer size.
        // + 1 because mapToUChars must have a slot at the end for the bufNativeLimit entry.
        int32_t  destIx = UTF8_متن_CHUNK_ناپ+2;   // Start in the overflow region
                                                    //   at end of buffer to leave room
                                                    //   for a surrogate pair at the
                                                    //   buffer start.
        int32_t  srcIx  = ix;
        int32_t  bufNILimit = destIx;
        UChar32   c;

        // Map to/from Native Indexes, fill in for the position at the end of
        //   the buffer.
        //
        map_منزل_Native[destIx] = (uint8_t)(srcIx - منزل_UChars_Map_شروع);
        map_منزل_UChars[srcIx - منزل_UChars_Map_شروع] = (uint8_t)destIx;

        // Fill the chunk buffer
        // Work backwards, filling from the end of the buffer towards the front.
        //
        طالما (destIx>2 && (srcIx - منزل_UChars_Map_شروع > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            إذا (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(منزل_UChars_Map_شروع <= srcIx);
                map_منزل_UChars[srcIx - منزل_UChars_Map_شروع] = (uint8_t)destIx;
                map_منزل_Native[destIx] = (uint8_t)(srcIx - منزل_UChars_Map_شروع);
            } وإلا {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_prev_Char_محفوظ_Body(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                إذا (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    map_منزل_Native[destIx] = (uint8_t)(srcIx - منزل_UChars_Map_شروع);
                } وإلا {
                    buf[destIx]         = U16_TRAIL(c);
                    map_منزل_Native[destIx] = (uint8_t)(srcIx - منزل_UChars_Map_شروع);
                    buf[--destIx]       = U16_LEAD(c);
                    map_منزل_Native[destIx] = (uint8_t)(srcIx - منزل_UChars_Map_شروع);
                }

                // Fill in the map from native indexes to UChars buf index.
                افعل {
                    map_منزل_UChars[sIx-- - منزل_UChars_Map_شروع] = (uint8_t)destIx;
                } طالما (sIx >= srcIx);
                U_ASSERT(منزل_UChars_Map_شروع <= (srcIx+1));

                // Set native indexing limit to be the current position.
                //   We are processing a non-ascii, non-native-indexing char now;
                //     the limit will be here if the rest of the chars to be
                //     added to this buffer are ascii.
                bufNILimit = destIx;
            }
        }
        u8b_وٹاندرا->buf_Native_شروع     = srcIx;
        u8b_وٹاندرا->buf_Native_حد_بندی     = ix;
        u8b_وٹاندرا->buf_شروع_Idx        = destIx;
        u8b_وٹاندرا->buf_حد_بندی_Idx        = UTF8_متن_CHUNK_ناپ+2;
        u8b_وٹاندرا->bufNILimit         = bufNILimit - u8b_وٹاندرا->buf_شروع_Idx;
        u8b_وٹاندرا->منزل_UChars_Map_شروع   = منزل_UChars_Map_شروع;

        ut->chunkContents       = &buf[u8b_وٹاندرا->buf_شروع_Idx];
        ut->chunk_لمائی         = u8b_وٹاندرا->buf_حد_بندی_Idx - u8b_وٹاندرا->buf_شروع_Idx;
        ut->chunk_وتھ         = ut->chunk_لمائی;
        ut->chunk_Native_شروع    = u8b_وٹاندرا->buf_Native_شروع;
        ut->chunk_Native_حد_بندی    = u8b_وٹاندرا->buf_Native_حد_بندی;
        ut->native_Indexing_حد_بندی = u8b_وٹاندرا->bufNILimit;
        أرجع سچ;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
ساكن char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t dest_سمرتھا,
              int32_t *p_Dest_لمائی,
              ثابت حرف* src,
              int32_t src_لمائی,        // required.  NUL terminated not supported.
              UError_کوڈ *p_Error_کوڈ
              )
{

    char16_t *pDest = dest;
    char16_t *p_Dest_حد_بندی = (dest!=خالی_سوچک)?(dest+dest_سمرتھا):خالی_سوچک;
    UChar32 ch=0;
    int32_t اشاریہ = 0;
    int32_t req_لمائی = 0;
    uint8_t* pSrc = (uint8_t*) src;


    طالما((اشاریہ < src_لمائی)&&(pDest<p_Dest_حد_بندی)){
        ch = pSrc[اشاریہ++];
        إذا(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }وإلا{
            ch=utf8_اگلا_Char_محفوظ_Body(pSrc, &اشاریہ, src_لمائی, ch, -3);
            إذا(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }وإلا{
                *(pDest++)=U16_LEAD(ch);
                إذا(pDest<p_Dest_حد_بندی){
                    *(pDest++)=U16_TRAIL(ch);
                }وإلا{
                    req_لمائی++;
                    اكسر;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    طالما(اشاریہ < src_لمائی){
        ch = pSrc[اشاریہ++];
        إذا(ch <= 0x7f){
            req_لمائی++;
        }وإلا{
            ch=utf8_اگلا_Char_محفوظ_Body(pSrc, &اشاریہ, src_لمائی, ch, -3);
            req_لمائی+=U16_لمائی(ch);
        }
    }

    req_لمائی+=(int32_t)(pDest - dest);

    إذا(p_Dest_لمائی){
        *p_Dest_لمائی = req_لمائی;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_سمرتھا,req_لمائی,p_Error_کوڈ);

    أرجع dest;
}



ساكن int32_t U_CALLCONV
utf8_متن_کڈھو(UText *ut,
                int64_t شروع, int64_t حد_بندی,
                char16_t *dest, int32_t dest_سمرتھا,
                UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(dest_سمرتھا<0 || (dest==خالی_سوچک && dest_سمرتھا>0)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pin_اشاریہ(شروع, length);
    int32_t  limit32 = pin_اشاریہ(حد_بندی, length);

    إذا(start32>limit32) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    ثابت uint8_t *buf = (ثابت uint8_t *)ut->context;
    صحيح i;
    إذا (start32 < ut->chunk_Native_حد_بندی) {
        لكل (i=0; i<3; i++) {
            إذا (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                اكسر;
            }
            start32--;
        }
    }

    إذا (limit32 < ut->chunk_Native_حد_بندی) {
        لكل (i=0; i<3; i++) {
            إذا (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                اكسر;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t dest_لمائی=0;
    utext_strFromUTF8(dest, dest_سمرتھا, &dest_لمائی,
                    (ثابت حرف *)ut->context+start32, limit32-start32,
                    p_Error_کوڈ);
    utf8_متن_Access(ut, limit32, سچ);
    أرجع dest_لمائی;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
ساكن int64_t U_CALLCONV
utf8_متن_Map_وتھ_منزل_Native(ثابت UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunk_وتھ>ut->native_Indexing_حد_بندی && ut->chunk_وتھ<=ut->chunk_لمائی);
    int32_t native_وتھ = u8b->map_منزل_Native[ut->chunk_وتھ + u8b->buf_شروع_Idx] + u8b->منزل_UChars_Map_شروع;
    U_ASSERT(native_وتھ >= ut->chunk_Native_شروع && native_وتھ <= ut->chunk_Native_حد_بندی);
    أرجع native_وتھ;
}

//
// Map a native index to the corresponding chunk offset
//
ساكن int32_t U_CALLCONV
utf8_متن_Map_اشاریہ_منزل_UTF16(ثابت UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t اشاریہ = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(اشاریہ>=ut->chunk_Native_شروع+ut->native_Indexing_حد_بندی);
    U_ASSERT(اشاریہ<=ut->chunk_Native_حد_بندی);
    int32_t map_اشاریہ = اشاریہ - u8b->منزل_UChars_Map_شروع;
    U_ASSERT(map_اشاریہ < (int32_t)حجم(UTF8Buf::map_منزل_UChars));
    int32_t وتھ = u8b->map_منزل_UChars[map_اشاریہ] - u8b->buf_شروع_Idx;
    U_ASSERT(وتھ>=0 && وتھ<=ut->chunk_لمائی);
    أرجع وتھ;
}

ساكن UText * U_CALLCONV
utf8_متن_Clone(UText *dest, ثابت UText *src, UBool deep, UError_کوڈ *حالت)
{
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_متن_Clone(dest, src, حالت);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //
    // TODO:  There is an issue with using utext_nativeLength().
    //        That function is non-const in cases where the input was NUL terminated
    //          and the length has not yet been determined.
    //        This function (clone()) is const.
    //        There potentially a thread safety issue lurking here.
    //
    إذا (deep && U_SUCCESS(*حالت)) {
        int32_t  len = (int32_t)utext_native_لمائی((UText *)src);
        حرف *copyStr = (حرف *)uprv_malloc(len+1);
        إذا (copyStr == خالی_سوچک) {
            *حالت = U_حافظہ_ونڈ_ERROR;
        } وإلا {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_متن);
        }
    }
    أرجع dest;
}


ساكن فراغ U_CALLCONV
utf8_متن_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    إذا (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_متن)) {
        حرف *s = (حرف *)ut->context;
        uprv_free(s);
        ut->context = خالی_سوچک;
    }
}

U_CDECL_سرا


ساكن ثابت بنية UTextFuncs utf8Funcs =
{
    حجم(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8_متن_Clone,
    utf8_متن_لمائی,
    utf8_متن_Access,
    utf8_متن_کڈھو,
    خالی_سوچک,                /* replace*/
    خالی_سوچک,                /* copy   */
    utf8_متن_Map_وتھ_منزل_Native,
    utf8_متن_Map_اشاریہ_منزل_UTF16,
    utf8_متن_Close,
    خالی_سوچک,                // spare 1
    خالی_سوچک,                // spare 2
    خالی_سوچک                 // spare 3
};


ساكن ثابت حرف g_خالی_تھاواں_String[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               ثابت حرف* s U_LIFETIME_BOUND,
               int64_t length,
               UError_کوڈ* حالت) {
    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا(s==خالی_سوچک && length==0) {
        s = g_خالی_تھاواں_String;
    }

    إذا(s==خالی_سوچک || length<-1 || length>INT32_MAX) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    ut = utext_تیاری(ut, حجم(UTF8Buf) * 2, حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    إذا (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (حرف *)ut->pExtra + حجم(UTF8Buf);
    أرجع ut;

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
تعداد { REP_متن_CHUNK_ناپ=10 };

بنية ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_متن_CHUNK_ناپ+1];
};


U_CDECL_BEGIN

ساكن UText * U_CALLCONV
rep_متن_Clone(UText *dest, ثابت UText *src, UBool deep, UError_کوڈ *حالت) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_متن_Clone(dest, src, حالت);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    إذا (deep && U_SUCCESS(*حالت)) {
        ثابت Replaceable *replSrc = (ثابت Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_متن);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    أرجع dest;
}


ساكن فراغ U_CALLCONV
rep_متن_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    إذا (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_متن)) {
        Replaceable *rep = (Replaceable *)ut->context;
        مٹاؤ rep;
        ut->context = خالی_سوچک;
    }
}


ساكن int64_t U_CALLCONV
rep_متن_لمائی(UText *ut) {
    ثابت Replaceable *replSrc = (ثابت Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    أرجع len;
}


ساكن UBool U_CALLCONV
rep_متن_Access(UText *ut, int64_t اشاریہ, UBool forward) {
    ثابت Replaceable *rep=(ثابت Replaceable *)ut->context;
    int32_t length=rep->length();   // Full length of the input text (bigger than a chunk)

    // clip the requested index to the limits of the text.
    int32_t index32 = pin_اشاریہ(اشاریہ, length);
    U_ASSERT(اشاریہ<=INT32_MAX);


    /*
     * Compute start/limit boundaries around index, for a segment of text
     * to be extracted.
     * To allow for the possibility that our user gave an index to the trailing
     * half of a surrogate pair, we must request one extra preceding char16_t when
     * going in the forward direction.  This will ensure that the buffer has the
     * entire code point at the specified index.
     */
    إذا(forward) {

        إذا (index32>=ut->chunk_Native_شروع && index32<ut->chunk_Native_حد_بندی) {
            // Buffer already contains the requested position.
            ut->chunk_وتھ = (int32_t)(اشاریہ - ut->chunk_Native_شروع);
            أرجع سچ;
        }
        إذا (index32>=length && ut->chunk_Native_حد_بندی==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunk_وتھ = length - (int32_t)ut->chunk_Native_شروع;
            أرجع خطأ;
        }

        ut->chunk_Native_حد_بندی = اشاریہ + REP_متن_CHUNK_ناپ - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        إذا(ut->chunk_Native_حد_بندی > length) {
            ut->chunk_Native_حد_بندی = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunk_Native_شروع = ut->chunk_Native_حد_بندی - REP_متن_CHUNK_ناپ;
        إذا(ut->chunk_Native_شروع < 0) {
            ut->chunk_Native_شروع = 0;
        }
    } وإلا {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        إذا (index32>ut->chunk_Native_شروع && index32<=ut->chunk_Native_حد_بندی) {
            // Requested position already in buffer.
            ut->chunk_وتھ = index32 - (int32_t)ut->chunk_Native_شروع;
            أرجع سچ;
        }
        إذا (index32==0 && ut->chunk_Native_شروع==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunk_وتھ = 0;
            أرجع خطأ;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunk_Native_شروع = index32 + 1 - REP_متن_CHUNK_ناپ;
        إذا (ut->chunk_Native_شروع < 0) {
            ut->chunk_Native_شروع = 0;
        }

        ut->chunk_Native_حد_بندی = index32 + 1;
        إذا (ut->chunk_Native_حد_بندی > length) {
            ut->chunk_Native_حد_بندی = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString بفر(ex->s, 0 /*buffer length*/, REP_متن_CHUNK_ناپ /*buffer capacity*/);
    rep->کڈھو_Between((int32_t)ut->chunk_Native_شروع, (int32_t)ut->chunk_Native_حد_بندی, بفر);

    ut->chunkContents  = ex->s;
    ut->chunk_لمائی    = (int32_t)(ut->chunk_Native_حد_بندی - ut->chunk_Native_شروع);
    ut->chunk_وتھ    = (int32_t)(index32 - ut->chunk_Native_شروع);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    إذا (ut->chunk_Native_حد_بندی < length &&
        U16_IS_LEAD(ex->s[ut->chunk_لمائی-1])) {
            ut->chunk_لمائی--;
            ut->chunk_Native_حد_بندی--;
            إذا (ut->chunk_وتھ > ut->chunk_لمائی) {
                ut->chunk_وتھ = ut->chunk_لمائی;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    إذا(ut->chunk_Native_شروع>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunk_Native_شروع);
        --(ut->chunk_لمائی);
        --(ut->chunk_وتھ);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_سیٹ_CP_شروع(ut->chunkContents, 0, ut->chunk_وتھ);

    // Use fast indexing for get/setNativeIndex()
    ut->native_Indexing_حد_بندی = ut->chunk_لمائی;

    أرجع سچ;
}



ساكن int32_t U_CALLCONV
rep_متن_کڈھو(UText *ut,
               int64_t شروع, int64_t حد_بندی,
               char16_t *dest, int32_t dest_سمرتھا,
               UError_کوڈ *حالت) {
    ثابت Replaceable *rep=(ثابت Replaceable *)ut->context;
    int32_t  length=rep->length();

    إذا(U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(dest_سمرتھا<0 || (dest==خالی_سوچک && dest_سمرتھا>0)) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
    }
    إذا(شروع>حد_بندی) {
        *حالت=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t  start32 = pin_اشاریہ(شروع, length);
    int32_t  limit32 = pin_اشاریہ(حد_بندی, length);

    // adjust start, limit if they point to trail half of surrogates
    إذا (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    إذا (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    إذا(length>dest_سمرتھا) {
        limit32 = start32 + dest_سمرتھا;
    }
    UnicodeString بفر(dest, 0, dest_سمرتھا); // writable alias
    rep->کڈھو_Between(start32, limit32, بفر);
    rep_متن_Access(ut, limit32, سچ);

    أرجع u_terminateUChars(dest, dest_سمرتھا, length, حالت);
}

ساكن int32_t U_CALLCONV
rep_متن_Replace(UText *ut,
               int64_t شروع, int64_t حد_بندی,
               ثابت char16_t *src, int32_t length,
               UError_کوڈ *حالت) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t old_لمائی;

    إذا(U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(src==خالی_سوچک && length!=0) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    old_لمائی=rep->length(); // will subtract from new length
    إذا(شروع>حد_بندی ) {
        *حالت=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t start32 = pin_اشاریہ(شروع, old_لمائی);
    int32_t limit32 = pin_اشاریہ(حد_بندی, old_لمائی);

    // Snap start & limit to code point boundaries.
    إذا (start32<old_لمائی && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    إذا (limit32<old_لمائی && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t new_لمائی = rep->length();
    int32_t لمائی_Delta = new_لمائی - old_لمائی;

    // Is the UText chunk buffer OK?
    إذا (ut->chunk_Native_حد_بندی > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t new_اشاریہ_Pos = limit32 + لمائی_Delta;
    rep_متن_Access(ut, new_اشاریہ_Pos, سچ);

    أرجع لمائی_Delta;
}


ساكن فراغ U_CALLCONV
rep_متن_Copy(UText *ut,
                int64_t شروع, int64_t حد_بندی,
                int64_t dest_اشاریہ,
                UBool move,
                UError_کوڈ *حالت)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    إذا(U_FAILURE(*حالت)) {
        أرجع;
    }
    إذا (شروع>حد_بندی || (شروع<dest_اشاریہ && dest_اشاریہ<حد_بندی))
    {
        *حالت=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    int32_t start32     = pin_اشاریہ(شروع, length);
    int32_t limit32     = pin_اشاریہ(حد_بندی, length);
    int32_t destIndex32 = pin_اشاریہ(dest_اشاریہ, length);

    // TODO:  snap input parameters to code point boundaries.

    إذا(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t seg_لمائی=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        إذا(destIndex32<start32) {
            start32+=seg_لمائی;
            limit32+=seg_لمائی;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } وإلا {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t پہلا_Affected_اشاریہ = destIndex32;
    إذا (move && start32<پہلا_Affected_اشاریہ) {
        پہلا_Affected_اشاریہ = start32;
    }
    إذا (پہلا_Affected_اشاریہ < ut->chunk_Native_حد_بندی) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  native_Iter_اشاریہ = destIndex32 + limit32 - start32;
    إذا (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        native_Iter_اشاریہ = destIndex32;
    }

    // Set position, reload chunk if needed.
    rep_متن_Access(ut, native_Iter_اشاریہ, سچ);
}

ساكن ثابت بنية UTextFuncs repFuncs =
{
    حجم(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    rep_متن_Clone,
    rep_متن_لمائی,
    rep_متن_Access,
    rep_متن_کڈھو,
    rep_متن_Replace,
    rep_متن_Copy,
    خالی_سوچک,              // MapOffsetToNative,
    خالی_سوچک,              // MapIndexToUTF16,
    rep_متن_Close,
    خالی_سوچک,              // spare 1
    خالی_سوچک,              // spare 2
    خالی_سوچک               // spare 3
};


U_CAPI UText* U_EXPORT2
utext_openReplaceable(UText* ut U_LIFETIME_BOUND,
                      Replaceable* rep U_LIFETIME_BOUND,
                      UError_کوڈ* حالت) {
    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا(rep==خالی_سوچک) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    ut = utext_تیاری(ut, حجم(ReplExtra), حالت);
    إذا(U_FAILURE(*حالت)) {
        أرجع ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    إذا(rep->has_Meta_انکڑے()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_انکڑے);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    أرجع ut;
}

U_CDECL_سرا








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


ساكن UText * U_CALLCONV
unistr_متن_Clone(UText *dest, ثابت UText *src, UBool deep, UError_کوڈ *حالت) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_متن_Clone(dest, src, حالت);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    إذا (deep && U_SUCCESS(*حالت)) {
        ثابت UnicodeString *srcString = (ثابت UnicodeString *)src->context;
        dest->context = جديد UnicodeString(*srcString);
        dest->chunkContents = ((ثابت UnicodeString *)dest->context)->get_بفر();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_متن);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    أرجع dest;
}

ساكن فراغ U_CALLCONV
unistr_متن_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    إذا (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_متن)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        مٹاؤ str;
        ut->context = خالی_سوچک;
    }
}


ساكن int64_t U_CALLCONV
unistr_متن_لمائی(UText *t) {
    أرجع ((ثابت UnicodeString *)t->context)->length();
}


ساكن UBool U_CALLCONV
unistr_متن_Access(UText *ut, int64_t اشاریہ, UBool  forward) {
    int32_t length  = ut->chunk_لمائی;
    ut->chunk_وتھ = pin_اشاریہ(اشاریہ, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && اشاریہ<length) || (!forward && اشاریہ>0);
    أرجع retVal;
}



ساكن int32_t U_CALLCONV
unistr_متن_کڈھو(UText *t,
                  int64_t شروع, int64_t حد_بندی,
                  char16_t *dest, int32_t dest_سمرتھا,
                  UError_کوڈ *p_Error_کوڈ) {
    ثابت UnicodeString *us=(ثابت UnicodeString *)t->context;
    int32_t length=us->length();

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(dest_سمرتھا<0 || (dest==خالی_سوچک && dest_سمرتھا>0)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
    }
    إذا(شروع<0 || شروع>حد_بندی) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t start32 = شروع<length ? us->get_Char32_شروع((int32_t)شروع) : length;
    int32_t limit32 = حد_بندی<length ? us->get_Char32_شروع((int32_t)حد_بندی) : length;

    length=limit32-start32;
    إذا (dest_سمرتھا>0 && dest!=خالی_سوچک) {
        int32_t trimmed_لمائی = length;
        إذا(trimmed_لمائی>dest_سمرتھا) {
            trimmed_لمائی=dest_سمرتھا;
        }
        us->extract(start32, trimmed_لمائی, dest);
        t->chunk_وتھ = start32+trimmed_لمائی;
    } وإلا {
        t->chunk_وتھ = start32;
    }
    u_terminateUChars(dest, dest_سمرتھا, length, p_Error_کوڈ);
    أرجع length;
}

ساكن int32_t U_CALLCONV
unistr_متن_Replace(UText *ut,
                  int64_t شروع, int64_t حد_بندی,
                  ثابت char16_t *src, int32_t length,
                  UError_کوڈ *p_Error_کوڈ) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t old_لمائی;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(src==خالی_سوچک && length!=0) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
    }
    إذا(شروع>حد_بندی) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    old_لمائی=us->length();
    int32_t start32 = pin_اشاریہ(شروع, old_لمائی);
    int32_t limit32 = pin_اشاریہ(حد_بندی, old_لمائی);
    إذا (start32 < old_لمائی) {
        start32 = us->get_Char32_شروع(start32);
    }
    إذا (limit32 < old_لمائی) {
        limit32 = us->get_Char32_شروع(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t new_لمائی = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->get_بفر();
    ut->chunk_لمائی      = new_لمائی;
    ut->chunk_Native_حد_بندی = new_لمائی;
    ut->native_Indexing_حد_بندی = new_لمائی;

    // Set iteration position to the point just following the newly inserted text.
    int32_t لمائی_Delta = new_لمائی - old_لمائی;
    ut->chunk_وتھ = limit32 + لمائی_Delta;

    أرجع لمائی_Delta;
}

ساكن فراغ U_CALLCONV
unistr_متن_Copy(UText *ut,
               int64_t شروع, int64_t حد_بندی,
               int64_t dest_اشاریہ,
               UBool move,
               UError_کوڈ *p_Error_کوڈ) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    int32_t start32 = pin_اشاریہ(شروع, length);
    int32_t limit32 = pin_اشاریہ(حد_بندی, length);
    int32_t destIndex32 = pin_اشاریہ(dest_اشاریہ, length);

    إذا( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    إذا(move) {
        // move: copy to destIndex, then remove original
        int32_t seg_لمائی=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        إذا(destIndex32<start32) {
            start32+=seg_لمائی;
        }
        us->remove(start32, seg_لمائی);
    } وإلا {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->get_بفر();
    إذا (move==خطأ) {
        // copy operation, string length grows
        ut->chunk_لمائی += limit32-start32;
        ut->chunk_Native_حد_بندی = ut->chunk_لمائی;
        ut->native_Indexing_حد_بندی = ut->chunk_لمائی;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunk_وتھ = destIndex32+limit32-start32;
    إذا (move && destIndex32>start32) {
        ut->chunk_وتھ = destIndex32;
    }

}

ساكن ثابت بنية UTextFuncs unistrFuncs =
{
    حجم(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistr_متن_Clone,
    unistr_متن_لمائی,
    unistr_متن_Access,
    unistr_متن_کڈھو,
    unistr_متن_Replace,
    unistr_متن_Copy,
    خالی_سوچک,                // MapOffsetToNative,
    خالی_سوچک,                // MapIndexToUTF16,
    unistr_متن_Close,
    خالی_سوچک,                // spare 1
    خالی_سوچک,                // spare 2
    خالی_سوچک                 // spare 3
};



U_CDECL_سرا


U_CAPI UText* U_EXPORT2
utext_openUnicodeString(UText* ut U_LIFETIME_BOUND,
                        UnicodeString* s U_LIFETIME_BOUND,
                        UError_کوڈ* حالت) {
    ut = utext_openConstUnicodeString(ut, s, حالت);
    إذا (U_SUCCESS(*حالت)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    أرجع ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             ثابت UnicodeString* s U_LIFETIME_BOUND,
                             UError_کوڈ* حالت) {
    إذا (U_SUCCESS(*حالت) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, خالی_سوچک, 0, حالت);
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع ut;
    }
    ut = utext_تیاری(ut, 0, حالت);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    إذا (U_SUCCESS(*حالت)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->get_بفر();
        ut->chunk_لمائی         = s->length();
        ut->chunk_Native_شروع    = 0;
        ut->chunk_Native_حد_بندی    = ut->chunk_لمائی;
        ut->native_Indexing_حد_بندی = ut->chunk_لمائی;
    }
    أرجع ut;
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


ساكن UText * U_CALLCONV
ucstr_متن_Clone(UText *dest, ثابت UText * src, UBool deep, UError_کوڈ * حالت) {
    // First do a generic shallow clone.
    dest = shallow_متن_Clone(dest, src, حالت);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    إذا (deep && U_SUCCESS(*حالت)) {
        U_ASSERT(utext_native_لمائی(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_native_لمائی(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        ثابت char16_t *srcStr = (ثابت char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * حجم(char16_t));
        إذا (copyStr == خالی_سوچک) {
            *حالت = U_حافظہ_ونڈ_ERROR;
        } وإلا {
            int64_t i;
            لكل (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_متن);
        }
    }
    أرجع dest;
}


ساكن فراغ U_CALLCONV
ucstr_متن_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    إذا (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_متن)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = خالی_سوچک;
    }
}



ساكن int64_t U_CALLCONV
ucstr_متن_لمائی(UText *ut) {
    إذا (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        ثابت char16_t  *str = (ثابت char16_t *)ut->context;
        لكل (;;) {
            إذا (str[ut->chunk_Native_حد_بندی] == 0) {
                اكسر;
            }
            ut->chunk_Native_حد_بندی++;
        }
        ut->a = ut->chunk_Native_حد_بندی;
        ut->chunk_لمائی = (int32_t)ut->chunk_Native_حد_بندی;
        ut->native_Indexing_حد_بندی = ut->chunk_لمائی;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE);
    }
    أرجع ut->a;
}


ساكن UBool U_CALLCONV
ucstr_متن_Access(UText *ut, int64_t اشاریہ, UBool  forward) {
    ثابت char16_t *str   = (ثابت char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    إذا (اشاریہ<0) {
        اشاریہ = 0;
    } وإلا إذا (اشاریہ < ut->chunk_Native_حد_بندی) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_سیٹ_CP_شروع(str, 0, اشاریہ);
    } وإلا إذا (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        اشاریہ = ut->a;
    } وإلا {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t جانچ_حد_بندی = (int32_t)اشاریہ + 32;
        إذا ((اشاریہ + 32)>INT32_MAX || (اشاریہ + 32)<0 ) {   // note: int64 expression
            جانچ_حد_بندی = INT32_MAX;
        }

        int32_t chunk_حد_بندی = (int32_t)ut->chunk_Native_حد_بندی;
        لكل (; chunk_حد_بندی<جانچ_حد_بندی; chunk_حد_بندی++) {
            إذا (str[chunk_حد_بندی] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunk_حد_بندی;
                ut->chunk_لمائی = chunk_حد_بندی;
                ut->native_Indexing_حد_بندی = chunk_حد_بندی;
                إذا (اشاریہ >= chunk_حد_بندی) {
                    اشاریہ = chunk_حد_بندی;
                } وإلا {
                    U16_سیٹ_CP_شروع(str, 0, اشاریہ);
                }

                ut->chunk_Native_حد_بندی = chunk_حد_بندی;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE);
                اذهب breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_سیٹ_CP_شروع(str, 0, اشاریہ);
        إذا (chunk_حد_بندی == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunk_حد_بندی;
            ut->chunk_لمائی = chunk_حد_بندی;
            ut->native_Indexing_حد_بندی = chunk_حد_بندی;
            إذا (اشاریہ > chunk_حد_بندی) {
                اشاریہ = chunk_حد_بندی;
            }
            ut->chunk_Native_حد_بندی = chunk_حد_بندی;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE);
        } وإلا {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            إذا (U16_IS_LEAD(str[chunk_حد_بندی-1])) {
                --chunk_حد_بندی;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunk_Native_حد_بندی = chunk_حد_بندی;
            ut->native_Indexing_حد_بندی = chunk_حد_بندی;
            ut->chunk_لمائی = chunk_حد_بندی;
        }

    }
breakout:
    U_ASSERT(اشاریہ<=INT32_MAX);
    ut->chunk_وتھ = (int32_t)اشاریہ;

    // Check whether request is at the start or end
    UBool retVal = (forward && اشاریہ<ut->chunk_Native_حد_بندی) || (!forward && اشاریہ>0);
    أرجع retVal;
}



ساكن int32_t U_CALLCONV
ucstr_متن_کڈھو(UText *ut,
                  int64_t شروع, int64_t حد_بندی,
                  char16_t *dest, int32_t dest_سمرتھا,
                  UError_کوڈ *p_Error_کوڈ)
{
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(dest_سمرتھا<0 || (dest==خالی_سوچک && dest_سمرتھا>0) || شروع>حد_بندی) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstr_متن_Access(ut, شروع, سچ);
    ثابت char16_t *s=ut->chunkContents;
    start32 = ut->chunk_وتھ;

    int32_t str_لمائی=(int32_t)ut->a;
    إذا (str_لمائی >= 0) {
        limit32 = pin_اشاریہ(حد_بندی, str_لمائی);
    } وإلا {
        limit32 = pin_اشاریہ(حد_بندی, INT32_MAX);
    }
    di = 0;
    لكل (si=start32; si<limit32; si++) {
        إذا (str_لمائی<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunk_Native_حد_بندی    = si;
            ut->chunk_لمائی         = si;
            ut->native_Indexing_حد_بندی = si;
            str_لمائی               = si;
            limit32                 = si;
            اكسر;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        إذا (di<dest_سمرتھا) {
            // only store if there is space.
            dest[di] = s[si];
        } وإلا {
            إذا (str_لمائی>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                اكسر;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    إذا (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<str_لمائی || str_لمائی<0)  && U16_IS_TRAIL(s[si])))
    {
        إذا (di<dest_سمرتھا) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    إذا (si <= ut->chunk_Native_حد_بندی) {
        ut->chunk_وتھ = si;
    } وإلا {
        ucstr_متن_Access(ut, si, سچ);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, dest_سمرتھا, di, p_Error_کوڈ);
    أرجع di;
}

ساكن ثابت بنية UTextFuncs ucstrFuncs =
{
    حجم(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstr_متن_Clone,
    ucstr_متن_لمائی,
    ucstr_متن_Access,
    ucstr_متن_کڈھو,
    خالی_سوچک,              // Replace
    خالی_سوچک,              // Copy
    خالی_سوچک,              // MapOffsetToNative,
    خالی_سوچک,              // MapIndexToUTF16,
    ucstr_متن_Close,
    خالی_سوچک,              // spare 1
    خالی_سوچک,              // spare 2
    خالی_سوچک,              // spare 3
};

U_CDECL_سرا

ساكن ثابت char16_t g_خالی_تھاواں_UString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 ثابت char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UError_کوڈ* حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا(s==خالی_سوچک && length==0) {
        s = g_خالی_تھاواں_UString;
    }
    إذا (s==خالی_سوچک || length < -1 || length>INT32_MAX) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    ut = utext_تیاری(ut, 0, حالت);
    إذا (U_SUCCESS(*حالت)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        إذا (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_لمائی_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunk_Native_شروع     = 0;
        ut->chunk_Native_حد_بندی     = length>=0? length : 0;
        ut->chunk_لمائی          = (int32_t)ut->chunk_Native_حد_بندی;
        ut->chunk_وتھ          = 0;
        ut->native_Indexing_حد_بندی  = ut->chunk_لمائی;
    }
    أرجع ut;
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
#تعریف_کرو CIBuf_ناپ 16

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
char_Iter_متن_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    مٹاؤ ci;
    ut->r = خالی_سوچک;
}

ساكن int64_t U_CALLCONV
char_Iter_متن_لمائی(UText *ut) {
    أرجع (int32_t)ut->a;
}

ساكن UBool U_CALLCONV
char_Iter_متن_Access(UText *ut, int64_t اشاریہ, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clipped_اشاریہ = (int32_t)اشاریہ;
    إذا (clipped_اشاریہ<0) {
        clipped_اشاریہ=0;
    } وإلا إذا (clipped_اشاریہ>=ut->a) {
        clipped_اشاریہ=(int32_t)ut->a;
    }
    int32_t needed_اشاریہ = clipped_اشاریہ;
    إذا (!forward && needed_اشاریہ>0) {
        // reverse iteration, want the position just before what was asked for.
        needed_اشاریہ--;
    } وإلا إذا (forward && needed_اشاریہ==ut->a && needed_اشاریہ>0) {
        // Forward iteration, don't ask for something past the end of the text.
        needed_اشاریہ--;
    }

    // Find the native index of the start of the buffer containing what we want.
    needed_اشاریہ -= needed_اشاریہ % CIBuf_ناپ;

    char16_t *buf = خالی_سوچک;
    UBool  need_Chunk_تیاری = سچ;
    صحيح    i;
    إذا (ut->chunk_Native_شروع == needed_اشاریہ) {
        // The buffer we want is already the current chunk.
        need_Chunk_تیاری = خطأ;
    } وإلا إذا (ut->b == needed_اشاریہ) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } وإلا إذا (ut->c == needed_اشاریہ) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } وإلا {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        إذا (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->سیٹ_اشاریہ(needed_اشاریہ);
        لكل (i=0; i<CIBuf_ناپ; i++) {
            buf[i] = ci->اگلا_Post_Inc();
            إذا (i+needed_اشاریہ > ut->a) {
                اكسر;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    إذا (need_Chunk_تیاری) {
        ut->chunkContents = buf;
        ut->chunk_لمائی   = CIBuf_ناپ;
        ut->chunk_Native_شروع = needed_اشاریہ;
        ut->chunk_Native_حد_بندی = needed_اشاریہ + CIBuf_ناپ;
        إذا (ut->chunk_Native_حد_بندی > ut->a) {
            ut->chunk_Native_حد_بندی = ut->a;
            ut->chunk_لمائی  = (int32_t)(ut->chunk_Native_حد_بندی)-(int32_t)(ut->chunk_Native_شروع);
        }
        ut->native_Indexing_حد_بندی = ut->chunk_لمائی;
        U_ASSERT(ut->chunk_وتھ>=0 && ut->chunk_وتھ<=CIBuf_ناپ);
    }
    ut->chunk_وتھ = clipped_اشاریہ - (int32_t)ut->chunk_Native_شروع;
    UBool success = (forward? ut->chunk_وتھ<ut->chunk_لمائی : ut->chunk_وتھ>0);
    أرجع success;
}

ساكن UText * U_CALLCONV
char_Iter_متن_Clone(UText *dest, ثابت UText *src, UBool deep, UError_کوڈ * حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    إذا (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع خالی_سوچک;
    } وإلا {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, حالت);
        إذا (U_FAILURE(*حالت)) {
            أرجع dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_get_Native_اشاریہ((UText *)src);
        utext_سیٹ_Native_اشاریہ(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    أرجع dest;
}

ساكن int32_t U_CALLCONV
char_Iter_متن_کڈھو(UText *ut,
                  int64_t شروع, int64_t حد_بندی,
                  char16_t *dest, int32_t dest_سمرتھا,
                  UError_کوڈ *حالت)
{
    إذا(U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(dest_سمرتھا<0 || (dest==خالی_سوچک && dest_سمرتھا>0) || شروع>حد_بندی) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pin_اشاریہ(شروع, length);
    int32_t  limit32 = pin_اشاریہ(حد_بندی, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copy_حد_بندی;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->سیٹ_Index32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->get_اشاریہ();
    copy_حد_بندی = srci;
    طالما (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_لمائی(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        إذا (desti+len <= dest_سمرتھا) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copy_حد_بندی = srci+len;
        } وإلا {
            desti += len;
            *حالت = U_بفر_حد_توں_ودھ_ERROR;
        }
        srci += len;
    }

    char_Iter_متن_Access(ut, copy_حد_بندی, سچ);

    u_terminateUChars(dest, dest_سمرتھا, desti, حالت);
    أرجع desti;
}

ساكن ثابت بنية UTextFuncs charIterFuncs =
{
    حجم(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    char_Iter_متن_Clone,
    char_Iter_متن_لمائی,
    char_Iter_متن_Access,
    char_Iter_متن_کڈھو,
    خالی_سوچک,                // Replace
    خالی_سوچک,                // Copy
    خالی_سوچک,                // MapOffsetToNative,
    خالی_سوچک,                // MapIndexToUTF16,
    char_Iter_متن_Close,
    خالی_سوچک,                // spare 1
    خالی_سوچک,                // spare 2
    خالی_سوچک                 // spare 3
};
U_CDECL_سرا

U_CAPI UText* U_EXPORT2
utext_openCharacterIterator(UText* ut U_LIFETIME_BOUND,
                            CharacterIterator* ci U_LIFETIME_BOUND,
                            UError_کوڈ* حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    إذا (ci->شروع_اشاریہ() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع خالی_سوچک;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBuf_ناپ * حجم(char16_t);
    ut = utext_تیاری(ut, extraSpace, حالت);
    إذا (U_SUCCESS(*حالت)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->سرا_اشاریہ();        // Length of text
        ut->p                    = ut->pExtra;            // First buffer
        ut->b                    = -1;                    // Native index of first buffer contents
        ut->q                    = (char16_t*)ut->pExtra+CIBuf_ناپ;  // Second buffer
        ut->c                    = -1;                    // Native index of second buffer contents

        // Initialize current chunk contents to be empty.
        //   First access will fault something in.
        //   Note:  The initial nativeStart and chunkOffset must sum to zero
        //          so that getNativeIndex() will correctly compute to zero
        //          if no call to Access() has ever been made.  They can't be both
        //          zero without Access() thinking that the chunk is valid.
        ut->chunkContents        = (char16_t *)ut->p;
        ut->chunk_Native_شروع     = -1;
        ut->chunk_وتھ          = 1;
        ut->chunk_Native_حد_بندی     = 0;
        ut->chunk_لمائی          = 0;
        ut->native_Indexing_حد_بندی  = ut->chunk_وتھ;  // enables native indexing
    }
    أرجع ut;
}

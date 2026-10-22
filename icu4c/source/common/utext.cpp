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

#تعریف_کن I32_FLAG(بیت_شاخص) ((int32_t)1<<(بیت_شاخص))


ساكن UBool
utext_access(UText *ut, int64_t شاخص, UBool forward) {
    أرجع ut->pFuncs->access(ut, شاخص, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    إذا (delta > 0) {
        افعل {
            إذا(ut->chunk_جابه_جایی>=ut->chunk_درازی && !utext_access(ut, ut->chunk_Native_حد, درست)) {
                أرجع خطأ;
            }
            c = ut->chunkContents[ut->chunk_جابه_جایی];
            إذا (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                إذا (c == U_SENTINEL) {
                    أرجع خطأ;
                }
            } وإلا {
                ut->chunk_جابه_جایی++;
            }
        } طالما(--delta>0);

    } وإلا إذا (delta<0) {
        افعل {
            إذا(ut->chunk_جابه_جایی<=0 && !utext_access(ut, ut->chunk_Native_شروع, خطأ)) {
                أرجع خطأ;
            }
            c = ut->chunkContents[ut->chunk_جابه_جایی-1];
            إذا (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                إذا (c == U_SENTINEL) {
                    أرجع خطأ;
                }
            } وإلا {
                ut->chunk_جابه_جایی--;
            }
        } طالما(++delta<0);
    }

    أرجع درست;
}


U_CAPI int64_t U_EXPORT2
utext_native_درازی(UText *ut) {
    أرجع ut->pFuncs->native_درازی(ut);
}


U_CAPI UBool U_EXPORT2
utext_is_درازی_Expensive(ثابت UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE)) != 0;
    أرجع r;
}


U_CAPI int64_t U_EXPORT2
utext_get_Native_شاخص(ثابت UText *ut) {
    إذا(ut->chunk_جابه_جایی <= ut->native_Indexing_حد) {
        أرجع ut->chunk_Native_شروع+ut->chunk_جابه_جایی;
    } وإلا {
        أرجع ut->pFuncs->map_جابه_جایی_مقصد_Native(ut);
    }
}


U_CAPI فراغ U_EXPORT2
utext_مجموعه_Native_شاخص(UText *ut, int64_t شاخص) {
    إذا(شاخص<ut->chunk_Native_شروع || شاخص>=ut->chunk_Native_حد) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, شاخص, درست);
    } وإلا إذا((int32_t)(شاخص - ut->chunk_Native_شروع) <= ut->native_Indexing_حد) {
        // utf-16 indexing.
        ut->chunk_جابه_جایی=(int32_t)(شاخص-ut->chunk_Native_شروع);
    } وإلا {
         ut->chunk_جابه_جایی=ut->pFuncs->map_Native_شاخص_مقصد_UTF16(ut, شاخص);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    إذا (ut->chunk_جابه_جایی<ut->chunk_درازی) {
        char16_t c= ut->chunkContents[ut->chunk_جابه_جایی];
        إذا (U16_IS_TRAIL(c)) {
            إذا (ut->chunk_جابه_جایی==0) {
                ut->pFuncs->access(ut, ut->chunk_Native_شروع, خطأ);
            }
            إذا (ut->chunk_جابه_جایی>0) {
                char16_t lead = ut->chunkContents[ut->chunk_جابه_جایی-1];
                إذا (U16_IS_LEAD(lead)) {
                    ut->chunk_جابه_جایی--;
                }
            }
        }
    }
}



U_CAPI int64_t U_EXPORT2
utext_get_قبلی_Native_شاخص(UText *ut) {
    //
    //  Fast-path the common case.
    //     Common means current position is not at the beginning of a chunk
    //     and the preceding character is not supplementary.
    //
    int32_t i = ut->chunk_جابه_جایی - 1;
    int64_t نتیجه;
    إذا (i >= 0) {
        char16_t c = ut->chunkContents[i];
        إذا (U16_IS_TRAIL(c) == خطأ) {
            إذا (i <= ut->native_Indexing_حد) {
                نتیجه = ut->chunk_Native_شروع + i;
            } وإلا {
                ut->chunk_جابه_جایی = i;
                نتیجه = ut->pFuncs->map_جابه_جایی_مقصد_Native(ut);
                ut->chunk_جابه_جایی++;
            }
            أرجع نتیجه;
        }
    }

    // If at the start of text, simply return 0.
    إذا (ut->chunk_جابه_جایی==0 && ut->chunk_Native_شروع==0) {
        أرجع 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    نتیجه = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    أرجع نتیجه;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    إذا (ut->chunk_جابه_جایی==ut->chunk_درازی) {
        // Current position is just off the end of the chunk.
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_حد, درست) == خطأ) {
            // Off the end of the text.
            أرجع U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_جابه_جایی];
    إذا (U16_IS_LEAD(c) == خطأ) {
        // Normal, non-supplementary case.
        أرجع c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    إذا ((ut->chunk_جابه_جایی+1) < ut->chunk_درازی) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunk_جابه_جایی+1];
    } وإلا {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunk_Native_حد;
        إذا (ut->pFuncs->access(ut, nativePosition, درست)) {
            trail = ut->chunkContents[ut->chunk_جابه_جایی];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, خطأ);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunk_جابه_جایی = ut->chunk_درازی - 1;
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
utext_char32At(UText *ut, int64_t native_شاخص) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    إذا (native_شاخص>=ut->chunk_Native_شروع && native_شاخص < ut->chunk_Native_شروع + ut->native_Indexing_حد) {
        ut->chunk_جابه_جایی = (int32_t)(native_شاخص - ut->chunk_Native_شروع);
        c = ut->chunkContents[ut->chunk_جابه_جایی];
        إذا (U16_IS_SURROGATE(c) == خطأ) {
            أرجع c;
        }
    }


    utext_مجموعه_Native_شاخص(ut, native_شاخص);
    إذا (native_شاخص>=ut->chunk_Native_شروع && ut->chunk_جابه_جایی<ut->chunk_درازی) {
        c = ut->chunkContents[ut->chunk_جابه_جایی];
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

    إذا (ut->chunk_جابه_جایی >= ut->chunk_درازی) {
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_حد, درست) == خطأ) {
            أرجع U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_جابه_جایی++];
    إذا (U16_IS_LEAD(c) == خطأ) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        أرجع c;
    }

    إذا (ut->chunk_جابه_جایی >= ut->chunk_درازی) {
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_حد, درست) == خطأ) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            أرجع c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunk_جابه_جایی];
    إذا (U16_IS_TRAIL(trail) == خطأ) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        أرجع c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunk_جابه_جایی++;   // move iteration position over the trail surrogate.
    أرجع supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    إذا (ut->chunk_جابه_جایی <= 0) {
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_شروع, خطأ) == خطأ) {
            أرجع U_SENTINEL;
        }
    }
    ut->chunk_جابه_جایی--;
    c = ut->chunkContents[ut->chunk_جابه_جایی];
    إذا (U16_IS_TRAIL(c) == خطأ) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        أرجع c;
    }

    إذا (ut->chunk_جابه_جایی <= 0) {
        إذا (ut->pFuncs->access(ut, ut->chunk_Native_شروع, خطأ) == خطأ) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            أرجع c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunk_جابه_جایی-1];
    إذا (U16_IS_LEAD(lead) == خطأ) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        أرجع c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunk_جابه_جایی--;   // move iteration position over the lead surrogate.
    أرجع supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t شاخص) {
    UChar32       c      = U_SENTINEL;

    إذا(شاخص<ut->chunk_Native_شروع || شاخص>=ut->chunk_Native_حد) {
        // Desired position is outside of the current chunk.
        إذا(!ut->pFuncs->access(ut, شاخص, درست)) {
            // no chunk available here
            أرجع U_SENTINEL;
        }
    } وإلا إذا (شاخص - ut->chunk_Native_شروع  <= (int64_t)ut->native_Indexing_حد) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunk_جابه_جایی = (int32_t)(شاخص - ut->chunk_Native_شروع);
    } وإلا {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunk_جابه_جایی = ut->pFuncs->map_Native_شاخص_مقصد_UTF16(ut, شاخص);
    }

    c = ut->chunkContents[ut->chunk_جابه_جایی++];
    إذا (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_مجموعه_Native_شاخص(ut, شاخص);
        c = utext_next32(ut);
    }
    أرجع c;
}


U_CAPI UChar32 U_EXPORT2
utext_previous32From(UText *ut, int64_t شاخص) {
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
    إذا(شاخص<=ut->chunk_Native_شروع || شاخص>ut->chunk_Native_حد) {
        // Requested native index is outside of the current chunk.
        إذا(!ut->pFuncs->access(ut, شاخص, خطأ)) {
            // no chunk available here
            أرجع U_SENTINEL;
        }
    } وإلا إذا(شاخص - ut->chunk_Native_شروع <= (int64_t)ut->native_Indexing_حد) {
        // Direct UTF-16 indexing.
        ut->chunk_جابه_جایی = (int32_t)(شاخص - ut->chunk_Native_شروع);
    } وإلا {
        ut->chunk_جابه_جایی=ut->pFuncs->map_Native_شاخص_مقصد_UTF16(ut, شاخص);
        إذا (ut->chunk_جابه_جایی==0 && !ut->pFuncs->access(ut, شاخص, خطأ)) {
            // no chunk available here
            أرجع U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunk_جابه_جایی--;
    cPrev = ut->chunkContents[ut->chunk_جابه_جایی];

    إذا (U16_IS_SURROGATE(cPrev)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_مجموعه_Native_شاخص(ut, شاخص);
        cPrev = utext_previous32(ut);
    }
    أرجع cPrev;
}


U_CAPI int32_t U_EXPORT2
utext_استخراج(UText *ut,
             int64_t شروع, int64_t حد,
             char16_t *dest, int32_t dest_ظرفیت,
             UError_کد *وضعیت) {
                 أرجع ut->pFuncs->extract(ut, شروع, حد, dest, dest_ظرفیت, وضعیت);
             }



U_CAPI UBool U_EXPORT2
utext_equals(ثابت UText *a, ثابت UText *b) {
    إذا (a==اشاره_تهی || b==اشاره_تهی ||
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
    إذا (utext_get_Native_شاخص(a) != utext_get_Native_شاخص(b)) {
        // Different current position in the string.
        أرجع خطأ;
    }

    أرجع درست;
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
utext_has_Meta_داده(ثابت UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_داده)) != 0;
    أرجع b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t native_شروع, int64_t native_حد,
             ثابت char16_t *replacement_متن, int32_t replacement_درازی,
             UError_کد *وضعیت)
{
    إذا (U_FAILURE(*وضعیت)) {
        أرجع 0;
    }
    إذا ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *وضعیت = U_NO_WRITE_PERMISSION;
        أرجع 0;
    }
    int32_t i = ut->pFuncs->replace(ut, native_شروع, native_حد, replacement_متن, replacement_درازی, وضعیت);
    أرجع i;
}

U_CAPI فراغ U_EXPORT2
utext_copy(UText *ut,
          int64_t native_شروع, int64_t native_حد,
          int64_t dest_شاخص,
          UBool move,
          UError_کد *وضعیت)
{
    إذا (U_FAILURE(*وضعیت)) {
        أرجع;
    }
    إذا ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *وضعیت = U_NO_WRITE_PERMISSION;
        أرجع;
    }
    ut->pFuncs->copy(ut, native_شروع, native_حد, dest_شاخص, move, وضعیت);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            ثابت UText* src,
            UBool deep,
            UBool readOnly,
            UError_کد* وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع dest;
    }
    UText *نتیجه = src->pFuncs->clone(dest, src, deep, وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
        أرجع نتیجه;
    }
    إذا (نتیجه == اشاره_تهی) {
        *وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع نتیجه;
    }
    إذا (readOnly) {
        utext_freeze(نتیجه);
    }
    أرجع نتیجه;
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
    UTEXT_انباشته_ALLOCATED  = 1,      //  1 if ICU has allocated this UText struct on the heap.
                                    //  0 if caller provided storage for the UText.

    UTEXT_EXTRA_انباشته_ALLOCATED = 2, //  1 if ICU has allocated extra storage as a separate
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

ساكن ثابت UText خالی_متن = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_راهاندازی(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UError_کد* وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع ut;
    }

    إذا (ut == اشاره_تهی) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = حجم(UText);
        إذا (extraSpace > 0) {
            spaceRequired = حجم(ExtendedUText) + extraSpace - حجم(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        إذا (ut == اشاره_تهی) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع اشاره_تهی;
        } وإلا {
            *ut = خالی_متن;
            ut->flags |= UTEXT_انباشته_ALLOCATED;
            إذا (spaceRequired>0) {
                ut->extra_اندازه = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } وإلا {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        إذا (ut->magic != UTEXT_MAGIC) {
            *وضعیت = U_ILLEGAL_آرگومان_ERROR;
            أرجع ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        إذا ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != اشاره_تهی)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        إذا (extraSpace > ut->extra_اندازه) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            إذا (ut->flags & UTEXT_EXTRA_انباشته_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extra_اندازه = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            إذا (ut->pExtra == اشاره_تهی) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
            } وإلا {
                ut->extra_اندازه = extraSpace;
                ut->flags |= UTEXT_EXTRA_انباشته_ALLOCATED;
            }
        }
    }
    إذا (U_SUCCESS(*وضعیت)) {
        ut->flags |= UTEXT_OPEN;

        // Initialize all remaining fields of the UText.
        //
        ut->context             = اشاره_تهی;
        ut->chunkContents       = اشاره_تهی;
        ut->p                   = اشاره_تهی;
        ut->q                   = اشاره_تهی;
        ut->r                   = اشاره_تهی;
        ut->a                   = 0;
        ut->b                   = 0;
        ut->c                   = 0;
        ut->chunk_جابه_جایی         = 0;
        ut->chunk_درازی         = 0;
        ut->chunk_Native_شروع    = 0;
        ut->chunk_Native_حد    = 0;
        ut->native_Indexing_حد = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = اشاره_تهی;
        إذا (ut->pExtra!=اشاره_تهی && ut->extra_اندازه>0)
            uprv_memset(ut->pExtra, 0, ut->extra_اندازه);

    }
    أرجع ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    إذا (ut==اشاره_تهی ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        أرجع ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    إذا (ut->pFuncs->close != اشاره_تهی) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    إذا (ut->flags & UTEXT_EXTRA_انباشته_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = اشاره_تهی;
        ut->flags &= ~UTEXT_EXTRA_انباشته_ALLOCATED;
        ut->extra_اندازه = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = اشاره_تهی;

    إذا (ut->flags & UTEXT_انباشته_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = اشاره_تهی;
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
    ut->chunk_درازی = 0;
    ut->chunk_Native_حد = 0;
    ut->chunk_Native_شروع = 0;
    ut->chunk_جابه_جایی = 0;
    ut->native_Indexing_حد = 0;
}

//
// pinIndex        Do range pinning on a native index parameter.
//                 64 bit pinning is done in place.
//                 32 bit truncated result is returned as a convenience for
//                        use in providers that don't need 64 bits.
ساكن int32_t
pin_شاخص(int64_t &شاخص, int64_t حد) {
    إذا (شاخص<0) {
        شاخص = 0;
    } وإلا إذا (شاخص > حد) {
        شاخص = حد;
    }
    أرجع static_cast<int32_t>(شاخص);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
ساكن فراغ adjust_اشاره_گر(UText *dest, ثابت فراغ **destPtr, ثابت UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    حرف  *dptr = (حرف *)*destPtr;
    حرف  *dUText = (حرف *)dest;
    حرف  *sUText = (حرف *)src;

    إذا (dptr >= (حرف *)src->pExtra && dptr < ((حرف*)src->pExtra)+src->extra_اندازه) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((حرف *)dest->pExtra) + (dptr - (حرف *)src->pExtra);
    } وإلا إذا (dptr>=sUText && dptr < sUText+src->اندازه_از_Struct) {
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
shallow_متن_Clone(UText * dest, ثابت UText * src, UError_کد * وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    int32_t  src_Extra_اندازه = src->extra_اندازه;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_راهاندازی(dest, src_Extra_اندازه, وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
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
    صحيح اندازه_مقصد_Copy = src->اندازه_از_Struct;
    إذا (اندازه_مقصد_Copy > dest->اندازه_از_Struct) {
        اندازه_مقصد_Copy = dest->اندازه_از_Struct;
    }
    uprv_memcpy(dest, src, اندازه_مقصد_Copy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    إذا (src_Extra_اندازه > 0) {
        uprv_memcpy(dest->pExtra, src->pExtra, src_Extra_اندازه);
    }

    //
    // Relocate any pointers in the target that refer to the UText itself
    //   to point to the cloned copy rather than the original source.
    //
    adjust_اشاره_گر(dest, &dest->context, src);
    adjust_اشاره_گر(dest, &dest->p, src);
    adjust_اشاره_گر(dest, &dest->q, src);
    adjust_اشاره_گر(dest, &dest->r, src);
    adjust_اشاره_گر(dest, (ثابت فراغ **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_متن);

    أرجع dest;
}


U_CDECL_انتها



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
تعداد { UTF8_متن_CHUNK_اندازه=32 };

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
    int32_t   buf_Native_حد;                        // Native index following last char in buf.
    int32_t   buf_شروع_Idx;                           // First filled position in buf.
    int32_t   buf_حد_Idx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   مقصد_UChars_Map_شروع;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_متن_CHUNK_اندازه+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   map_مقصد_Native[UTF8_متن_CHUNK_اندازه+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   map_مقصد_UChars[UTF8_متن_CHUNK_اندازه*3+6]; // Map native offset from bufNativeStart to
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
utf8_متن_درازی(UText *ut) {
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
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE);
    }
    أرجع ut->b;
}






ساكن UBool U_CALLCONV
utf8_متن_Access(UText *ut, int64_t شاخص, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    ثابت uint8_t *s8=(ثابت uint8_t *)ut->context;
    UTF8Buf *u8b = اشاره_تهی;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)شاخص;     // Requested index, trimmed to 32 bits.
    int32_t  map_شاخص = 0;
    إذا (شاخص<0) {
        ix=0;
    } وإلا إذا (شاخص > 0x7fffffff) {
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
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    إذا (forward) {
        إذا (ix==ut->chunk_Native_حد) {
            // Check for normal sequential iteration cases first.
            إذا (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunk_جابه_جایی = ut->chunk_درازی;
                أرجع خطأ;
            } وإلا {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                إذا (ix>=altB->buf_Native_شروع && ix<altB->buf_Native_حد) {
                    اذهب جابجا_Buffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            إذا (ix>=u8b->buf_Native_شروع && ix<u8b->buf_Native_حد) {
                // Requested index is in the other buffer.
                اذهب جابجا_Buffers;
            }
            إذا (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                إذا (ix == ut->chunk_Native_حد) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunk_جابه_جایی = ut->chunk_درازی;
                    أرجع خطأ;
                }
                إذا (ix == u8b->buf_Native_حد) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    اذهب جابجا_Buffers_And_Fail;
                }

                // Neither existing buffer extends to the end of the string.
                اذهب make_Stub_میانگیر;
            }

            إذا (ix<ut->chunk_Native_شروع || ix>=ut->chunk_Native_حد) {
                // Requested index is in neither buffer.
                اذهب fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            map_شاخص = ix - u8b->مقصد_UChars_Map_شروع;
            U_ASSERT(map_شاخص < (int32_t)حجم(UTF8Buf::map_مقصد_UChars));
            ut->chunk_جابه_جایی = u8b->map_مقصد_UChars[map_شاخص] - u8b->buf_شروع_Idx;
            أرجع درست;

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
            ut->chunk_جابه_جایی = 0;
            أرجع خطأ;
        } وإلا {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            إذا (ix>altB->buf_Native_شروع && ix<=altB->buf_Native_حد) {
                اذهب جابجا_Buffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    إذا (ix>u8b->buf_Native_شروع && ix<=u8b->buf_Native_حد) {
        // Requested index is in the other buffer.
        اذهب جابجا_Buffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    إذا (ix==0) {
        إذا (u8b->buf_Native_شروع==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            اذهب جابجا_Buffers_And_Fail;
        } وإلا {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            اذهب make_Stub_میانگیر;
        }
    }

    إذا (ix<=ut->chunk_Native_شروع || ix>ut->chunk_Native_حد) {
        // Requested index is in neither buffer.
        اذهب fill_معکوس;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    map_شاخص = ix - u8b->مقصد_UChars_Map_شروع;
    ut->chunk_جابه_جایی = u8b->map_مقصد_UChars[map_شاخص] - u8b->buf_شروع_Idx;
    إذا (ut->chunk_جابه_جایی==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        أرجع خطأ;
    } وإلا {
        أرجع درست;
    }



جابجا_Buffers:
    //  The alternate buffer (ut->q) has the string data that was requested.
    //  Swap the primary and alternate buffers, and set the
    //   chunk index into the new primary buffer.
    {
        u8b   = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b;
        ut->chunkContents       = &u8b->buf[u8b->buf_شروع_Idx];
        ut->chunk_درازی         = u8b->buf_حد_Idx - u8b->buf_شروع_Idx;
        ut->chunk_Native_شروع    = u8b->buf_Native_شروع;
        ut->chunk_Native_حد    = u8b->buf_Native_حد;
        ut->native_Indexing_حد = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->buf_Native_شروع);
        U_ASSERT(ix<=u8b->buf_Native_حد);
        map_شاخص = ix - u8b->مقصد_UChars_Map_شروع;
        U_ASSERT(map_شاخص>=0);
        U_ASSERT(map_شاخص<(int32_t)حجم(u8b->map_مقصد_UChars));
        ut->chunk_جابه_جایی = u8b->map_مقصد_UChars[map_شاخص] - u8b->buf_شروع_Idx;

        أرجع درست;
    }


 جابجا_Buffers_And_Fail:
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
    ut->chunk_درازی         = u8b->buf_حد_Idx - u8b->buf_شروع_Idx;
    ut->chunk_Native_شروع    = u8b->buf_Native_شروع;
    ut->chunk_Native_حد    = u8b->buf_Native_حد;
    ut->native_Indexing_حد = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    إذا (ix==u8b->buf_Native_حد) {
        ut->chunk_جابه_جایی = ut->chunk_درازی;
    } وإلا  {
        ut->chunk_جابه_جایی = 0;
        U_ASSERT(ix == u8b->buf_Native_شروع);
    }
    أرجع خطأ;

make_Stub_میانگیر:
    //   The user has done a seek/access past the start or end
    //   of the string.  Rather than loading data that is likely
    //   to never be used, just set up a zero-length buffer at
    //   the position.
    u8b = (UTF8Buf *)ut->q;
    u8b->buf_Native_شروع   = ix;
    u8b->buf_Native_حد   = ix;
    u8b->buf_شروع_Idx      = 0;
    u8b->buf_حد_Idx      = 0;
    u8b->bufNILimit       = 0;
    u8b->مقصد_UChars_Map_شروع = ix;
    u8b->map_مقصد_Native[0]   = 0;
    u8b->map_مقصد_UChars[0]   = 0;
    اذهب جابجا_Buffers_And_Fail;



fillForward:
    {
        // Move the incoming index to a code point boundary.
        U8_مجموعه_CP_شروع(s8, 0, ix);

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_جابجا = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_جابجا;

        int32_t strLen = ut->b;
        UBool   nulTerminated = خطأ;
        إذا (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = درست;
        }

        char16_t   *buf = u8b_جابجا->buf;
        uint8_t *map_مقصد_Native  = u8b_جابجا->map_مقصد_Native;
        uint8_t *map_مقصد_UChars  = u8b_جابجا->map_مقصد_UChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    دیده_Non_Ascii = خطأ;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        طالما (destIx<UTF8_متن_CHUNK_اندازه) {
            c = s8[srcIx];
            إذا (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                map_مقصد_Native[destIx]    = (uint8_t)(srcIx - ix);
                map_مقصد_UChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } وإلا {
                // General case, handle everything.
                إذا (دیده_Non_Ascii == خطأ) {
                    دیده_Non_Ascii = درست;
                    u8b_جابجا->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_بعدی_OR_FFFD(s8, srcIx, strLen, c);
                إذا (c==0 && nulTerminated) {
                    srcIx--;
                    اكسر;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                افعل {
                    map_مقصد_Native[dIx++] = (uint8_t)(cIx - ix);
                } طالما (dIx < destIx);

                افعل {
                    map_مقصد_UChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } طالما (cIx < srcIx);
            }
            إذا (srcIx>=strLen) {
                اكسر;
            }

        }

        //  store Native <--> Chunk Map entries for the end of the buffer.
        //    There is no actual character here, but the index position is valid.
        map_مقصد_Native[destIx]     = (uint8_t)(srcIx - ix);
        map_مقصد_UChars[srcIx - ix] = (uint8_t)destIx;

        //  fill in Buffer descriptor
        u8b_جابجا->buf_Native_شروع     = ix;
        u8b_جابجا->buf_Native_حد     = srcIx;
        u8b_جابجا->buf_شروع_Idx        = 0;
        u8b_جابجا->buf_حد_Idx        = destIx;
        إذا (دیده_Non_Ascii == خطأ) {
            u8b_جابجا->bufNILimit     = destIx;
        }
        u8b_جابجا->مقصد_UChars_Map_شروع   = u8b_جابجا->buf_Native_شروع;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunk_جابه_جایی         = 0;
        ut->chunk_درازی         = u8b_جابجا->buf_حد_Idx;
        ut->chunk_Native_شروع    = u8b_جابجا->buf_Native_شروع;
        ut->chunk_Native_حد    = u8b_جابجا->buf_Native_حد;
        ut->native_Indexing_حد = u8b_جابجا->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        إذا (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            إذا (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE);
            }
        }
        أرجع درست;
    }


fill_معکوس:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        إذا (ix != ut->b) {
            // Note: this function will only move the index back if it is on a trail byte
            //       and there is a preceding lead byte and the sequence from the lead 
            //       through this trail could be part of a valid UTF-8 sequence
            //       Otherwise the index remains unchanged.
            U8_مجموعه_CP_شروع(s8, 0, ix);
        }

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_جابجا = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_جابجا;

        char16_t   *buf = u8b_جابجا->buf;
        uint8_t *map_مقصد_Native = u8b_جابجا->map_مقصد_Native;
        uint8_t *map_مقصد_UChars = u8b_جابجا->map_مقصد_UChars;
        int32_t  مقصد_UChars_Map_شروع = ix - حجم(UTF8Buf::map_مقصد_UChars) + 1;
        // Note that toUCharsMapStart can be negative. Happens when the remaining
        // text from current position to the beginning is less than the buffer size.
        // + 1 because mapToUChars must have a slot at the end for the bufNativeLimit entry.
        int32_t  destIx = UTF8_متن_CHUNK_اندازه+2;   // Start in the overflow region
                                                    //   at end of buffer to leave room
                                                    //   for a surrogate pair at the
                                                    //   buffer start.
        int32_t  srcIx  = ix;
        int32_t  bufNILimit = destIx;
        UChar32   c;

        // Map to/from Native Indexes, fill in for the position at the end of
        //   the buffer.
        //
        map_مقصد_Native[destIx] = (uint8_t)(srcIx - مقصد_UChars_Map_شروع);
        map_مقصد_UChars[srcIx - مقصد_UChars_Map_شروع] = (uint8_t)destIx;

        // Fill the chunk buffer
        // Work backwards, filling from the end of the buffer towards the front.
        //
        طالما (destIx>2 && (srcIx - مقصد_UChars_Map_شروع > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            إذا (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(مقصد_UChars_Map_شروع <= srcIx);
                map_مقصد_UChars[srcIx - مقصد_UChars_Map_شروع] = (uint8_t)destIx;
                map_مقصد_Native[destIx] = (uint8_t)(srcIx - مقصد_UChars_Map_شروع);
            } وإلا {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_prev_Char_امن_Body(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                إذا (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    map_مقصد_Native[destIx] = (uint8_t)(srcIx - مقصد_UChars_Map_شروع);
                } وإلا {
                    buf[destIx]         = U16_TRAIL(c);
                    map_مقصد_Native[destIx] = (uint8_t)(srcIx - مقصد_UChars_Map_شروع);
                    buf[--destIx]       = U16_LEAD(c);
                    map_مقصد_Native[destIx] = (uint8_t)(srcIx - مقصد_UChars_Map_شروع);
                }

                // Fill in the map from native indexes to UChars buf index.
                افعل {
                    map_مقصد_UChars[sIx-- - مقصد_UChars_Map_شروع] = (uint8_t)destIx;
                } طالما (sIx >= srcIx);
                U_ASSERT(مقصد_UChars_Map_شروع <= (srcIx+1));

                // Set native indexing limit to be the current position.
                //   We are processing a non-ascii, non-native-indexing char now;
                //     the limit will be here if the rest of the chars to be
                //     added to this buffer are ascii.
                bufNILimit = destIx;
            }
        }
        u8b_جابجا->buf_Native_شروع     = srcIx;
        u8b_جابجا->buf_Native_حد     = ix;
        u8b_جابجا->buf_شروع_Idx        = destIx;
        u8b_جابجا->buf_حد_Idx        = UTF8_متن_CHUNK_اندازه+2;
        u8b_جابجا->bufNILimit         = bufNILimit - u8b_جابجا->buf_شروع_Idx;
        u8b_جابجا->مقصد_UChars_Map_شروع   = مقصد_UChars_Map_شروع;

        ut->chunkContents       = &buf[u8b_جابجا->buf_شروع_Idx];
        ut->chunk_درازی         = u8b_جابجا->buf_حد_Idx - u8b_جابجا->buf_شروع_Idx;
        ut->chunk_جابه_جایی         = ut->chunk_درازی;
        ut->chunk_Native_شروع    = u8b_جابجا->buf_Native_شروع;
        ut->chunk_Native_حد    = u8b_جابجا->buf_Native_حد;
        ut->native_Indexing_حد = u8b_جابجا->bufNILimit;
        أرجع درست;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
ساكن char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t dest_ظرفیت,
              int32_t *p_Dest_درازی,
              ثابت حرف* src,
              int32_t src_درازی,        // required.  NUL terminated not supported.
              UError_کد *p_Error_کد
              )
{

    char16_t *pDest = dest;
    char16_t *p_Dest_حد = (dest!=اشاره_تهی)?(dest+dest_ظرفیت):اشاره_تهی;
    UChar32 ch=0;
    int32_t شاخص = 0;
    int32_t req_درازی = 0;
    uint8_t* pSrc = (uint8_t*) src;


    طالما((شاخص < src_درازی)&&(pDest<p_Dest_حد)){
        ch = pSrc[شاخص++];
        إذا(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }وإلا{
            ch=utf8_بعدی_Char_امن_Body(pSrc, &شاخص, src_درازی, ch, -3);
            إذا(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }وإلا{
                *(pDest++)=U16_LEAD(ch);
                إذا(pDest<p_Dest_حد){
                    *(pDest++)=U16_TRAIL(ch);
                }وإلا{
                    req_درازی++;
                    اكسر;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    طالما(شاخص < src_درازی){
        ch = pSrc[شاخص++];
        إذا(ch <= 0x7f){
            req_درازی++;
        }وإلا{
            ch=utf8_بعدی_Char_امن_Body(pSrc, &شاخص, src_درازی, ch, -3);
            req_درازی+=U16_درازی(ch);
        }
    }

    req_درازی+=(int32_t)(pDest - dest);

    إذا(p_Dest_درازی){
        *p_Dest_درازی = req_درازی;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_ظرفیت,req_درازی,p_Error_کد);

    أرجع dest;
}



ساكن int32_t U_CALLCONV
utf8_متن_استخراج(UText *ut,
                int64_t شروع, int64_t حد,
                char16_t *dest, int32_t dest_ظرفیت,
                UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==اشاره_تهی && dest_ظرفیت>0)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pin_شاخص(شروع, length);
    int32_t  limit32 = pin_شاخص(حد, length);

    إذا(start32>limit32) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    ثابت uint8_t *buf = (ثابت uint8_t *)ut->context;
    صحيح i;
    إذا (start32 < ut->chunk_Native_حد) {
        لكل (i=0; i<3; i++) {
            إذا (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                اكسر;
            }
            start32--;
        }
    }

    إذا (limit32 < ut->chunk_Native_حد) {
        لكل (i=0; i<3; i++) {
            إذا (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                اكسر;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t dest_درازی=0;
    utext_strFromUTF8(dest, dest_ظرفیت, &dest_درازی,
                    (ثابت حرف *)ut->context+start32, limit32-start32,
                    p_Error_کد);
    utf8_متن_Access(ut, limit32, درست);
    أرجع dest_درازی;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
ساكن int64_t U_CALLCONV
utf8_متن_Map_جابه_جایی_مقصد_Native(ثابت UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunk_جابه_جایی>ut->native_Indexing_حد && ut->chunk_جابه_جایی<=ut->chunk_درازی);
    int32_t native_جابه_جایی = u8b->map_مقصد_Native[ut->chunk_جابه_جایی + u8b->buf_شروع_Idx] + u8b->مقصد_UChars_Map_شروع;
    U_ASSERT(native_جابه_جایی >= ut->chunk_Native_شروع && native_جابه_جایی <= ut->chunk_Native_حد);
    أرجع native_جابه_جایی;
}

//
// Map a native index to the corresponding chunk offset
//
ساكن int32_t U_CALLCONV
utf8_متن_Map_شاخص_مقصد_UTF16(ثابت UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t شاخص = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(شاخص>=ut->chunk_Native_شروع+ut->native_Indexing_حد);
    U_ASSERT(شاخص<=ut->chunk_Native_حد);
    int32_t map_شاخص = شاخص - u8b->مقصد_UChars_Map_شروع;
    U_ASSERT(map_شاخص < (int32_t)حجم(UTF8Buf::map_مقصد_UChars));
    int32_t جابه_جایی = u8b->map_مقصد_UChars[map_شاخص] - u8b->buf_شروع_Idx;
    U_ASSERT(جابه_جایی>=0 && جابه_جایی<=ut->chunk_درازی);
    أرجع جابه_جایی;
}

ساكن UText * U_CALLCONV
utf8_متن_Clone(UText *dest, ثابت UText *src, UBool deep, UError_کد *وضعیت)
{
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_متن_Clone(dest, src, وضعیت);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //
    // TODO:  There is an issue with using utext_nativeLength().
    //        That function is non-const in cases where the input was NUL terminated
    //          and the length has not yet been determined.
    //        This function (clone()) is const.
    //        There potentially a thread safety issue lurking here.
    //
    إذا (deep && U_SUCCESS(*وضعیت)) {
        int32_t  len = (int32_t)utext_native_درازی((UText *)src);
        حرف *copyStr = (حرف *)uprv_malloc(len+1);
        إذا (copyStr == اشاره_تهی) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
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
        ut->context = اشاره_تهی;
    }
}

U_CDECL_انتها


ساكن ثابت بنية UTextFuncs utf8Funcs =
{
    حجم(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8_متن_Clone,
    utf8_متن_درازی,
    utf8_متن_Access,
    utf8_متن_استخراج,
    اشاره_تهی,                /* replace*/
    اشاره_تهی,                /* copy   */
    utf8_متن_Map_جابه_جایی_مقصد_Native,
    utf8_متن_Map_شاخص_مقصد_UTF16,
    utf8_متن_Close,
    اشاره_تهی,                // spare 1
    اشاره_تهی,                // spare 2
    اشاره_تهی                 // spare 3
};


ساكن ثابت حرف g_خالی_String[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               ثابت حرف* s U_LIFETIME_BOUND,
               int64_t length,
               UError_کد* وضعیت) {
    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا(s==اشاره_تهی && length==0) {
        s = g_خالی_String;
    }

    إذا(s==اشاره_تهی || length<-1 || length>INT32_MAX) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    ut = utext_راهاندازی(ut, حجم(UTF8Buf) * 2, وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
        أرجع ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    إذا (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE);
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
تعداد { REP_متن_CHUNK_اندازه=10 };

بنية ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_متن_CHUNK_اندازه+1];
};


U_CDECL_BEGIN

ساكن UText * U_CALLCONV
rep_متن_Clone(UText *dest, ثابت UText *src, UBool deep, UError_کد *وضعیت) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_متن_Clone(dest, src, وضعیت);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    إذا (deep && U_SUCCESS(*وضعیت)) {
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
        حذف rep;
        ut->context = اشاره_تهی;
    }
}


ساكن int64_t U_CALLCONV
rep_متن_درازی(UText *ut) {
    ثابت Replaceable *replSrc = (ثابت Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    أرجع len;
}


ساكن UBool U_CALLCONV
rep_متن_Access(UText *ut, int64_t شاخص, UBool forward) {
    ثابت Replaceable *rep=(ثابت Replaceable *)ut->context;
    int32_t length=rep->length();   // Full length of the input text (bigger than a chunk)

    // clip the requested index to the limits of the text.
    int32_t index32 = pin_شاخص(شاخص, length);
    U_ASSERT(شاخص<=INT32_MAX);


    /*
     * Compute start/limit boundaries around index, for a segment of text
     * to be extracted.
     * To allow for the possibility that our user gave an index to the trailing
     * half of a surrogate pair, we must request one extra preceding char16_t when
     * going in the forward direction.  This will ensure that the buffer has the
     * entire code point at the specified index.
     */
    إذا(forward) {

        إذا (index32>=ut->chunk_Native_شروع && index32<ut->chunk_Native_حد) {
            // Buffer already contains the requested position.
            ut->chunk_جابه_جایی = (int32_t)(شاخص - ut->chunk_Native_شروع);
            أرجع درست;
        }
        إذا (index32>=length && ut->chunk_Native_حد==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunk_جابه_جایی = length - (int32_t)ut->chunk_Native_شروع;
            أرجع خطأ;
        }

        ut->chunk_Native_حد = شاخص + REP_متن_CHUNK_اندازه - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        إذا(ut->chunk_Native_حد > length) {
            ut->chunk_Native_حد = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunk_Native_شروع = ut->chunk_Native_حد - REP_متن_CHUNK_اندازه;
        إذا(ut->chunk_Native_شروع < 0) {
            ut->chunk_Native_شروع = 0;
        }
    } وإلا {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        إذا (index32>ut->chunk_Native_شروع && index32<=ut->chunk_Native_حد) {
            // Requested position already in buffer.
            ut->chunk_جابه_جایی = index32 - (int32_t)ut->chunk_Native_شروع;
            أرجع درست;
        }
        إذا (index32==0 && ut->chunk_Native_شروع==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunk_جابه_جایی = 0;
            أرجع خطأ;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunk_Native_شروع = index32 + 1 - REP_متن_CHUNK_اندازه;
        إذا (ut->chunk_Native_شروع < 0) {
            ut->chunk_Native_شروع = 0;
        }

        ut->chunk_Native_حد = index32 + 1;
        إذا (ut->chunk_Native_حد > length) {
            ut->chunk_Native_حد = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString میانگیر(ex->s, 0 /*buffer length*/, REP_متن_CHUNK_اندازه /*buffer capacity*/);
    rep->استخراج_Between((int32_t)ut->chunk_Native_شروع, (int32_t)ut->chunk_Native_حد, میانگیر);

    ut->chunkContents  = ex->s;
    ut->chunk_درازی    = (int32_t)(ut->chunk_Native_حد - ut->chunk_Native_شروع);
    ut->chunk_جابه_جایی    = (int32_t)(index32 - ut->chunk_Native_شروع);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    إذا (ut->chunk_Native_حد < length &&
        U16_IS_LEAD(ex->s[ut->chunk_درازی-1])) {
            ut->chunk_درازی--;
            ut->chunk_Native_حد--;
            إذا (ut->chunk_جابه_جایی > ut->chunk_درازی) {
                ut->chunk_جابه_جایی = ut->chunk_درازی;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    إذا(ut->chunk_Native_شروع>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunk_Native_شروع);
        --(ut->chunk_درازی);
        --(ut->chunk_جابه_جایی);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_مجموعه_CP_شروع(ut->chunkContents, 0, ut->chunk_جابه_جایی);

    // Use fast indexing for get/setNativeIndex()
    ut->native_Indexing_حد = ut->chunk_درازی;

    أرجع درست;
}



ساكن int32_t U_CALLCONV
rep_متن_استخراج(UText *ut,
               int64_t شروع, int64_t حد,
               char16_t *dest, int32_t dest_ظرفیت,
               UError_کد *وضعیت) {
    ثابت Replaceable *rep=(ثابت Replaceable *)ut->context;
    int32_t  length=rep->length();

    إذا(U_FAILURE(*وضعیت)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==اشاره_تهی && dest_ظرفیت>0)) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
    }
    إذا(شروع>حد) {
        *وضعیت=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t  start32 = pin_شاخص(شروع, length);
    int32_t  limit32 = pin_شاخص(حد, length);

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
    إذا(length>dest_ظرفیت) {
        limit32 = start32 + dest_ظرفیت;
    }
    UnicodeString میانگیر(dest, 0, dest_ظرفیت); // writable alias
    rep->استخراج_Between(start32, limit32, میانگیر);
    rep_متن_Access(ut, limit32, درست);

    أرجع u_terminateUChars(dest, dest_ظرفیت, length, وضعیت);
}

ساكن int32_t U_CALLCONV
rep_متن_Replace(UText *ut,
               int64_t شروع, int64_t حد,
               ثابت char16_t *src, int32_t length,
               UError_کد *وضعیت) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t old_درازی;

    إذا(U_FAILURE(*وضعیت)) {
        أرجع 0;
    }
    إذا(src==اشاره_تهی && length!=0) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    old_درازی=rep->length(); // will subtract from new length
    إذا(شروع>حد ) {
        *وضعیت=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t start32 = pin_شاخص(شروع, old_درازی);
    int32_t limit32 = pin_شاخص(حد, old_درازی);

    // Snap start & limit to code point boundaries.
    إذا (start32<old_درازی && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    إذا (limit32<old_درازی && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t new_درازی = rep->length();
    int32_t درازی_Delta = new_درازی - old_درازی;

    // Is the UText chunk buffer OK?
    إذا (ut->chunk_Native_حد > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t new_شاخص_Pos = limit32 + درازی_Delta;
    rep_متن_Access(ut, new_شاخص_Pos, درست);

    أرجع درازی_Delta;
}


ساكن فراغ U_CALLCONV
rep_متن_Copy(UText *ut,
                int64_t شروع, int64_t حد,
                int64_t dest_شاخص,
                UBool move,
                UError_کد *وضعیت)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    إذا(U_FAILURE(*وضعیت)) {
        أرجع;
    }
    إذا (شروع>حد || (شروع<dest_شاخص && dest_شاخص<حد))
    {
        *وضعیت=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    int32_t start32     = pin_شاخص(شروع, length);
    int32_t limit32     = pin_شاخص(حد, length);
    int32_t destIndex32 = pin_شاخص(dest_شاخص, length);

    // TODO:  snap input parameters to code point boundaries.

    إذا(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t seg_درازی=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        إذا(destIndex32<start32) {
            start32+=seg_درازی;
            limit32+=seg_درازی;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } وإلا {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t اولی_Affected_شاخص = destIndex32;
    إذا (move && start32<اولی_Affected_شاخص) {
        اولی_Affected_شاخص = start32;
    }
    إذا (اولی_Affected_شاخص < ut->chunk_Native_حد) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  native_Iter_شاخص = destIndex32 + limit32 - start32;
    إذا (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        native_Iter_شاخص = destIndex32;
    }

    // Set position, reload chunk if needed.
    rep_متن_Access(ut, native_Iter_شاخص, درست);
}

ساكن ثابت بنية UTextFuncs repFuncs =
{
    حجم(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    rep_متن_Clone,
    rep_متن_درازی,
    rep_متن_Access,
    rep_متن_استخراج,
    rep_متن_Replace,
    rep_متن_Copy,
    اشاره_تهی,              // MapOffsetToNative,
    اشاره_تهی,              // MapIndexToUTF16,
    rep_متن_Close,
    اشاره_تهی,              // spare 1
    اشاره_تهی,              // spare 2
    اشاره_تهی               // spare 3
};


U_CAPI UText* U_EXPORT2
utext_openReplaceable(UText* ut U_LIFETIME_BOUND,
                      Replaceable* rep U_LIFETIME_BOUND,
                      UError_کد* وضعیت) {
    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا(rep==اشاره_تهی) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    ut = utext_راهاندازی(ut, حجم(ReplExtra), وضعیت);
    إذا(U_FAILURE(*وضعیت)) {
        أرجع ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    إذا(rep->has_Meta_داده()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_داده);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    أرجع ut;
}

U_CDECL_انتها








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
unistr_متن_Clone(UText *dest, ثابت UText *src, UBool deep, UError_کد *وضعیت) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallow_متن_Clone(dest, src, وضعیت);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    إذا (deep && U_SUCCESS(*وضعیت)) {
        ثابت UnicodeString *srcString = (ثابت UnicodeString *)src->context;
        dest->context = جديد UnicodeString(*srcString);
        dest->chunkContents = ((ثابت UnicodeString *)dest->context)->get_میانگیر();
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
        حذف str;
        ut->context = اشاره_تهی;
    }
}


ساكن int64_t U_CALLCONV
unistr_متن_درازی(UText *t) {
    أرجع ((ثابت UnicodeString *)t->context)->length();
}


ساكن UBool U_CALLCONV
unistr_متن_Access(UText *ut, int64_t شاخص, UBool  forward) {
    int32_t length  = ut->chunk_درازی;
    ut->chunk_جابه_جایی = pin_شاخص(شاخص, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && شاخص<length) || (!forward && شاخص>0);
    أرجع retVal;
}



ساكن int32_t U_CALLCONV
unistr_متن_استخراج(UText *t,
                  int64_t شروع, int64_t حد,
                  char16_t *dest, int32_t dest_ظرفیت,
                  UError_کد *p_Error_کد) {
    ثابت UnicodeString *us=(ثابت UnicodeString *)t->context;
    int32_t length=us->length();

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==اشاره_تهی && dest_ظرفیت>0)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
    }
    إذا(شروع<0 || شروع>حد) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t start32 = شروع<length ? us->get_Char32_شروع((int32_t)شروع) : length;
    int32_t limit32 = حد<length ? us->get_Char32_شروع((int32_t)حد) : length;

    length=limit32-start32;
    إذا (dest_ظرفیت>0 && dest!=اشاره_تهی) {
        int32_t trimmed_درازی = length;
        إذا(trimmed_درازی>dest_ظرفیت) {
            trimmed_درازی=dest_ظرفیت;
        }
        us->extract(start32, trimmed_درازی, dest);
        t->chunk_جابه_جایی = start32+trimmed_درازی;
    } وإلا {
        t->chunk_جابه_جایی = start32;
    }
    u_terminateUChars(dest, dest_ظرفیت, length, p_Error_کد);
    أرجع length;
}

ساكن int32_t U_CALLCONV
unistr_متن_Replace(UText *ut,
                  int64_t شروع, int64_t حد,
                  ثابت char16_t *src, int32_t length,
                  UError_کد *p_Error_کد) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t old_درازی;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(src==اشاره_تهی && length!=0) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
    }
    إذا(شروع>حد) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    old_درازی=us->length();
    int32_t start32 = pin_شاخص(شروع, old_درازی);
    int32_t limit32 = pin_شاخص(حد, old_درازی);
    إذا (start32 < old_درازی) {
        start32 = us->get_Char32_شروع(start32);
    }
    إذا (limit32 < old_درازی) {
        limit32 = us->get_Char32_شروع(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t new_درازی = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->get_میانگیر();
    ut->chunk_درازی      = new_درازی;
    ut->chunk_Native_حد = new_درازی;
    ut->native_Indexing_حد = new_درازی;

    // Set iteration position to the point just following the newly inserted text.
    int32_t درازی_Delta = new_درازی - old_درازی;
    ut->chunk_جابه_جایی = limit32 + درازی_Delta;

    أرجع درازی_Delta;
}

ساكن فراغ U_CALLCONV
unistr_متن_Copy(UText *ut,
               int64_t شروع, int64_t حد,
               int64_t dest_شاخص,
               UBool move,
               UError_کد *p_Error_کد) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    int32_t start32 = pin_شاخص(شروع, length);
    int32_t limit32 = pin_شاخص(حد, length);
    int32_t destIndex32 = pin_شاخص(dest_شاخص, length);

    إذا( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    إذا(move) {
        // move: copy to destIndex, then remove original
        int32_t seg_درازی=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        إذا(destIndex32<start32) {
            start32+=seg_درازی;
        }
        us->remove(start32, seg_درازی);
    } وإلا {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->get_میانگیر();
    إذا (move==خطأ) {
        // copy operation, string length grows
        ut->chunk_درازی += limit32-start32;
        ut->chunk_Native_حد = ut->chunk_درازی;
        ut->native_Indexing_حد = ut->chunk_درازی;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunk_جابه_جایی = destIndex32+limit32-start32;
    إذا (move && destIndex32>start32) {
        ut->chunk_جابه_جایی = destIndex32;
    }

}

ساكن ثابت بنية UTextFuncs unistrFuncs =
{
    حجم(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistr_متن_Clone,
    unistr_متن_درازی,
    unistr_متن_Access,
    unistr_متن_استخراج,
    unistr_متن_Replace,
    unistr_متن_Copy,
    اشاره_تهی,                // MapOffsetToNative,
    اشاره_تهی,                // MapIndexToUTF16,
    unistr_متن_Close,
    اشاره_تهی,                // spare 1
    اشاره_تهی,                // spare 2
    اشاره_تهی                 // spare 3
};



U_CDECL_انتها


U_CAPI UText* U_EXPORT2
utext_openUnicodeString(UText* ut U_LIFETIME_BOUND,
                        UnicodeString* s U_LIFETIME_BOUND,
                        UError_کد* وضعیت) {
    ut = utext_openConstUnicodeString(ut, s, وضعیت);
    إذا (U_SUCCESS(*وضعیت)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    أرجع ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             ثابت UnicodeString* s U_LIFETIME_BOUND,
                             UError_کد* وضعیت) {
    إذا (U_SUCCESS(*وضعیت) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, اشاره_تهی, 0, وضعیت);
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع ut;
    }
    ut = utext_راهاندازی(ut, 0, وضعیت);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    إذا (U_SUCCESS(*وضعیت)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->get_میانگیر();
        ut->chunk_درازی         = s->length();
        ut->chunk_Native_شروع    = 0;
        ut->chunk_Native_حد    = ut->chunk_درازی;
        ut->native_Indexing_حد = ut->chunk_درازی;
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
ucstr_متن_Clone(UText *dest, ثابت UText * src, UBool deep, UError_کد * وضعیت) {
    // First do a generic shallow clone.
    dest = shallow_متن_Clone(dest, src, وضعیت);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    إذا (deep && U_SUCCESS(*وضعیت)) {
        U_ASSERT(utext_native_درازی(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_native_درازی(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        ثابت char16_t *srcStr = (ثابت char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * حجم(char16_t));
        إذا (copyStr == اشاره_تهی) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
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
        ut->context = اشاره_تهی;
    }
}



ساكن int64_t U_CALLCONV
ucstr_متن_درازی(UText *ut) {
    إذا (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        ثابت char16_t  *str = (ثابت char16_t *)ut->context;
        لكل (;;) {
            إذا (str[ut->chunk_Native_حد] == 0) {
                اكسر;
            }
            ut->chunk_Native_حد++;
        }
        ut->a = ut->chunk_Native_حد;
        ut->chunk_درازی = (int32_t)ut->chunk_Native_حد;
        ut->native_Indexing_حد = ut->chunk_درازی;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE);
    }
    أرجع ut->a;
}


ساكن UBool U_CALLCONV
ucstr_متن_Access(UText *ut, int64_t شاخص, UBool  forward) {
    ثابت char16_t *str   = (ثابت char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    إذا (شاخص<0) {
        شاخص = 0;
    } وإلا إذا (شاخص < ut->chunk_Native_حد) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_مجموعه_CP_شروع(str, 0, شاخص);
    } وإلا إذا (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        شاخص = ut->a;
    } وإلا {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t پیمایش_حد = (int32_t)شاخص + 32;
        إذا ((شاخص + 32)>INT32_MAX || (شاخص + 32)<0 ) {   // note: int64 expression
            پیمایش_حد = INT32_MAX;
        }

        int32_t chunk_حد = (int32_t)ut->chunk_Native_حد;
        لكل (; chunk_حد<پیمایش_حد; chunk_حد++) {
            إذا (str[chunk_حد] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunk_حد;
                ut->chunk_درازی = chunk_حد;
                ut->native_Indexing_حد = chunk_حد;
                إذا (شاخص >= chunk_حد) {
                    شاخص = chunk_حد;
                } وإلا {
                    U16_مجموعه_CP_شروع(str, 0, شاخص);
                }

                ut->chunk_Native_حد = chunk_حد;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE);
                اذهب breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_مجموعه_CP_شروع(str, 0, شاخص);
        إذا (chunk_حد == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunk_حد;
            ut->chunk_درازی = chunk_حد;
            ut->native_Indexing_حد = chunk_حد;
            إذا (شاخص > chunk_حد) {
                شاخص = chunk_حد;
            }
            ut->chunk_Native_حد = chunk_حد;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE);
        } وإلا {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            إذا (U16_IS_LEAD(str[chunk_حد-1])) {
                --chunk_حد;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunk_Native_حد = chunk_حد;
            ut->native_Indexing_حد = chunk_حد;
            ut->chunk_درازی = chunk_حد;
        }

    }
breakout:
    U_ASSERT(شاخص<=INT32_MAX);
    ut->chunk_جابه_جایی = (int32_t)شاخص;

    // Check whether request is at the start or end
    UBool retVal = (forward && شاخص<ut->chunk_Native_حد) || (!forward && شاخص>0);
    أرجع retVal;
}



ساكن int32_t U_CALLCONV
ucstr_متن_استخراج(UText *ut,
                  int64_t شروع, int64_t حد,
                  char16_t *dest, int32_t dest_ظرفیت,
                  UError_کد *p_Error_کد)
{
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==اشاره_تهی && dest_ظرفیت>0) || شروع>حد) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstr_متن_Access(ut, شروع, درست);
    ثابت char16_t *s=ut->chunkContents;
    start32 = ut->chunk_جابه_جایی;

    int32_t str_درازی=(int32_t)ut->a;
    إذا (str_درازی >= 0) {
        limit32 = pin_شاخص(حد, str_درازی);
    } وإلا {
        limit32 = pin_شاخص(حد, INT32_MAX);
    }
    di = 0;
    لكل (si=start32; si<limit32; si++) {
        إذا (str_درازی<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunk_Native_حد    = si;
            ut->chunk_درازی         = si;
            ut->native_Indexing_حد = si;
            str_درازی               = si;
            limit32                 = si;
            اكسر;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        إذا (di<dest_ظرفیت) {
            // only store if there is space.
            dest[di] = s[si];
        } وإلا {
            إذا (str_درازی>=0) {
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
            ((si<str_درازی || str_درازی<0)  && U16_IS_TRAIL(s[si])))
    {
        إذا (di<dest_ظرفیت) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    إذا (si <= ut->chunk_Native_حد) {
        ut->chunk_جابه_جایی = si;
    } وإلا {
        ucstr_متن_Access(ut, si, درست);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, dest_ظرفیت, di, p_Error_کد);
    أرجع di;
}

ساكن ثابت بنية UTextFuncs ucstrFuncs =
{
    حجم(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstr_متن_Clone,
    ucstr_متن_درازی,
    ucstr_متن_Access,
    ucstr_متن_استخراج,
    اشاره_تهی,              // Replace
    اشاره_تهی,              // Copy
    اشاره_تهی,              // MapOffsetToNative,
    اشاره_تهی,              // MapIndexToUTF16,
    ucstr_متن_Close,
    اشاره_تهی,              // spare 1
    اشاره_تهی,              // spare 2
    اشاره_تهی,              // spare 3
};

U_CDECL_انتها

ساكن ثابت char16_t g_خالی_UString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 ثابت char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UError_کد* وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا(s==اشاره_تهی && length==0) {
        s = g_خالی_UString;
    }
    إذا (s==اشاره_تهی || length < -1 || length>INT32_MAX) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    ut = utext_راهاندازی(ut, 0, وضعیت);
    إذا (U_SUCCESS(*وضعیت)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        إذا (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_درازی_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunk_Native_شروع     = 0;
        ut->chunk_Native_حد     = length>=0? length : 0;
        ut->chunk_درازی          = (int32_t)ut->chunk_Native_حد;
        ut->chunk_جابه_جایی          = 0;
        ut->native_Indexing_حد  = ut->chunk_درازی;
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
#تعریف_کن CIBuf_اندازه 16

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
char_Iter_متن_Close(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    حذف ci;
    ut->r = اشاره_تهی;
}

ساكن int64_t U_CALLCONV
char_Iter_متن_درازی(UText *ut) {
    أرجع (int32_t)ut->a;
}

ساكن UBool U_CALLCONV
char_Iter_متن_Access(UText *ut, int64_t شاخص, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clipped_شاخص = (int32_t)شاخص;
    إذا (clipped_شاخص<0) {
        clipped_شاخص=0;
    } وإلا إذا (clipped_شاخص>=ut->a) {
        clipped_شاخص=(int32_t)ut->a;
    }
    int32_t needed_شاخص = clipped_شاخص;
    إذا (!forward && needed_شاخص>0) {
        // reverse iteration, want the position just before what was asked for.
        needed_شاخص--;
    } وإلا إذا (forward && needed_شاخص==ut->a && needed_شاخص>0) {
        // Forward iteration, don't ask for something past the end of the text.
        needed_شاخص--;
    }

    // Find the native index of the start of the buffer containing what we want.
    needed_شاخص -= needed_شاخص % CIBuf_اندازه;

    char16_t *buf = اشاره_تهی;
    UBool  need_Chunk_راهاندازی = درست;
    صحيح    i;
    إذا (ut->chunk_Native_شروع == needed_شاخص) {
        // The buffer we want is already the current chunk.
        need_Chunk_راهاندازی = خطأ;
    } وإلا إذا (ut->b == needed_شاخص) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } وإلا إذا (ut->c == needed_شاخص) {
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
        ci->مجموعه_شاخص(needed_شاخص);
        لكل (i=0; i<CIBuf_اندازه; i++) {
            buf[i] = ci->بعدی_Post_Inc();
            إذا (i+needed_شاخص > ut->a) {
                اكسر;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    إذا (need_Chunk_راهاندازی) {
        ut->chunkContents = buf;
        ut->chunk_درازی   = CIBuf_اندازه;
        ut->chunk_Native_شروع = needed_شاخص;
        ut->chunk_Native_حد = needed_شاخص + CIBuf_اندازه;
        إذا (ut->chunk_Native_حد > ut->a) {
            ut->chunk_Native_حد = ut->a;
            ut->chunk_درازی  = (int32_t)(ut->chunk_Native_حد)-(int32_t)(ut->chunk_Native_شروع);
        }
        ut->native_Indexing_حد = ut->chunk_درازی;
        U_ASSERT(ut->chunk_جابه_جایی>=0 && ut->chunk_جابه_جایی<=CIBuf_اندازه);
    }
    ut->chunk_جابه_جایی = clipped_شاخص - (int32_t)ut->chunk_Native_شروع;
    UBool success = (forward? ut->chunk_جابه_جایی<ut->chunk_درازی : ut->chunk_جابه_جایی>0);
    أرجع success;
}

ساكن UText * U_CALLCONV
char_Iter_متن_Clone(UText *dest, ثابت UText *src, UBool deep, UError_کد * وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    إذا (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *وضعیت = U_UNSUPPORTED_ERROR;
        أرجع اشاره_تهی;
    } وإلا {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, وضعیت);
        إذا (U_FAILURE(*وضعیت)) {
            أرجع dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_get_Native_شاخص((UText *)src);
        utext_مجموعه_Native_شاخص(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    أرجع dest;
}

ساكن int32_t U_CALLCONV
char_Iter_متن_استخراج(UText *ut,
                  int64_t شروع, int64_t حد,
                  char16_t *dest, int32_t dest_ظرفیت,
                  UError_کد *وضعیت)
{
    إذا(U_FAILURE(*وضعیت)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==اشاره_تهی && dest_ظرفیت>0) || شروع>حد) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pin_شاخص(شروع, length);
    int32_t  limit32 = pin_شاخص(حد, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copy_حد;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->مجموعه_Index32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->get_شاخص();
    copy_حد = srci;
    طالما (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_درازی(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        إذا (desti+len <= dest_ظرفیت) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copy_حد = srci+len;
        } وإلا {
            desti += len;
            *وضعیت = U_میانگیر_سرریز_ERROR;
        }
        srci += len;
    }

    char_Iter_متن_Access(ut, copy_حد, درست);

    u_terminateUChars(dest, dest_ظرفیت, desti, وضعیت);
    أرجع desti;
}

ساكن ثابت بنية UTextFuncs charIterFuncs =
{
    حجم(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    char_Iter_متن_Clone,
    char_Iter_متن_درازی,
    char_Iter_متن_Access,
    char_Iter_متن_استخراج,
    اشاره_تهی,                // Replace
    اشاره_تهی,                // Copy
    اشاره_تهی,                // MapOffsetToNative,
    اشاره_تهی,                // MapIndexToUTF16,
    char_Iter_متن_Close,
    اشاره_تهی,                // spare 1
    اشاره_تهی,                // spare 2
    اشاره_تهی                 // spare 3
};
U_CDECL_انتها

U_CAPI UText* U_EXPORT2
utext_openCharacterIterator(UText* ut U_LIFETIME_BOUND,
                            CharacterIterator* ci U_LIFETIME_BOUND,
                            UError_کد* وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    إذا (ci->شروع_شاخص() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *وضعیت = U_UNSUPPORTED_ERROR;
        أرجع اشاره_تهی;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBuf_اندازه * حجم(char16_t);
    ut = utext_راهاندازی(ut, extraSpace, وضعیت);
    إذا (U_SUCCESS(*وضعیت)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->انتها_شاخص();        // Length of text
        ut->p                    = ut->pExtra;            // First buffer
        ut->b                    = -1;                    // Native index of first buffer contents
        ut->q                    = (char16_t*)ut->pExtra+CIBuf_اندازه;  // Second buffer
        ut->c                    = -1;                    // Native index of second buffer contents

        // Initialize current chunk contents to be empty.
        //   First access will fault something in.
        //   Note:  The initial nativeStart and chunkOffset must sum to zero
        //          so that getNativeIndex() will correctly compute to zero
        //          if no call to Access() has ever been made.  They can't be both
        //          zero without Access() thinking that the chunk is valid.
        ut->chunkContents        = (char16_t *)ut->p;
        ut->chunk_Native_شروع     = -1;
        ut->chunk_جابه_جایی          = 1;
        ut->chunk_Native_حد     = 0;
        ut->chunk_درازی          = 0;
        ut->native_Indexing_حد  = ut->chunk_جابه_جایی;  // enables native indexing
    }
    أرجع ut;
}

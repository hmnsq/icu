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

#অন্তর্ভাব <cstddef>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/chariter.h"
#অন্তর্ভাব "unicode/utext.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "putilimp.h"

U_NAMESPACE_USE

#define I32_FLAG(bit_সূচক) ((int32_t)1<<(bit_সূচক))


স্থির UBool
utext_access(UText *ut, int64_t সূচক, UBool forward) {
    ফেরত ut->pFuncs->access(ut, সূচক, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    যদি (delta > 0) {
        করো {
            যদি(ut->chunkOffset>=ut->chunk_দৈর্ঘ্য && !utext_access(ut, ut->chunkNativeLimit, সত্য)) {
                ফেরত মিথ্যা;
            }
            c = ut->chunkContents[ut->chunkOffset];
            যদি (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                যদি (c == U_SENTINEL) {
                    ফেরত মিথ্যা;
                }
            } অন্যথা {
                ut->chunkOffset++;
            }
        } যখন(--delta>0);

    } অন্যথা যদি (delta<0) {
        করো {
            যদি(ut->chunkOffset<=0 && !utext_access(ut, ut->chunkNativeStart, মিথ্যা)) {
                ফেরত মিথ্যা;
            }
            c = ut->chunkContents[ut->chunkOffset-1];
            যদি (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                যদি (c == U_SENTINEL) {
                    ফেরত মিথ্যা;
                }
            } অন্যথা {
                ut->chunkOffset--;
            }
        } যখন(++delta<0);
    }

    ফেরত সত্য;
}


U_CAPI int64_t U_EXPORT2
utext_native_দৈর্ঘ্য(UText *ut) {
    ফেরত ut->pFuncs->native_দৈর্ঘ্য(ut);
}


U_CAPI UBool U_EXPORT2
utext_is_দৈর্ঘ্য_Expensive(ধ্রুবক UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE)) != 0;
    ফেরত r;
}


U_CAPI int64_t U_EXPORT2
utext_get_Native_সূচক(ধ্রুবক UText *ut) {
    যদি(ut->chunkOffset <= ut->nativeIndexingLimit) {
        ফেরত ut->chunkNativeStart+ut->chunkOffset;
    } অন্যথা {
        ফেরত ut->pFuncs->mapOffsetToNative(ut);
    }
}


U_CAPI শূন্য U_EXPORT2
utext_set_Native_সূচক(UText *ut, int64_t সূচক) {
    যদি(সূচক<ut->chunkNativeStart || সূচক>=ut->chunkNativeLimit) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, সূচক, সত্য);
    } অন্যথা যদি((int32_t)(সূচক - ut->chunkNativeStart) <= ut->nativeIndexingLimit) {
        // utf-16 indexing.
        ut->chunkOffset=(int32_t)(সূচক-ut->chunkNativeStart);
    } অন্যথা {
         ut->chunkOffset=ut->pFuncs->map_Native_সূচক_To_UTF16(ut, সূচক);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    যদি (ut->chunkOffset<ut->chunk_দৈর্ঘ্য) {
        char16_t c= ut->chunkContents[ut->chunkOffset];
        যদি (U16_IS_TRAIL(c)) {
            যদি (ut->chunkOffset==0) {
                ut->pFuncs->access(ut, ut->chunkNativeStart, মিথ্যা);
            }
            যদি (ut->chunkOffset>0) {
                char16_t lead = ut->chunkContents[ut->chunkOffset-1];
                যদি (U16_IS_LEAD(lead)) {
                    ut->chunkOffset--;
                }
            }
        }
    }
}



U_CAPI int64_t U_EXPORT2
utext_get_Previous_Native_সূচক(UText *ut) {
    //
    //  Fast-path the common case.
    //     Common means current position is not at the beginning of a chunk
    //     and the preceding character is not supplementary.
    //
    int32_t i = ut->chunkOffset - 1;
    int64_t ফলাফল;
    যদি (i >= 0) {
        char16_t c = ut->chunkContents[i];
        যদি (U16_IS_TRAIL(c) == মিথ্যা) {
            যদি (i <= ut->nativeIndexingLimit) {
                ফলাফল = ut->chunkNativeStart + i;
            } অন্যথা {
                ut->chunkOffset = i;
                ফলাফল = ut->pFuncs->mapOffsetToNative(ut);
                ut->chunkOffset++;
            }
            ফেরত ফলাফল;
        }
    }

    // If at the start of text, simply return 0.
    যদি (ut->chunkOffset==0 && ut->chunkNativeStart==0) {
        ফেরত 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    ফলাফল = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    ফেরত ফলাফল;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    যদি (ut->chunkOffset==ut->chunk_দৈর্ঘ্য) {
        // Current position is just off the end of the chunk.
        যদি (ut->pFuncs->access(ut, ut->chunkNativeLimit, সত্য) == মিথ্যা) {
            // Off the end of the text.
            ফেরত U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunkOffset];
    যদি (U16_IS_LEAD(c) == মিথ্যা) {
        // Normal, non-supplementary case.
        ফেরত c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    যদি ((ut->chunkOffset+1) < ut->chunk_দৈর্ঘ্য) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunkOffset+1];
    } অন্যথা {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunkNativeLimit;
        যদি (ut->pFuncs->access(ut, nativePosition, সত্য)) {
            trail = ut->chunkContents[ut->chunkOffset];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, মিথ্যা);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunkOffset = ut->chunk_দৈর্ঘ্য - 1;
        যদি(!r) {
            ফেরত U_SENTINEL;
        }
    }

    যদি (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    ফেরত supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t native_সূচক) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    যদি (native_সূচক>=ut->chunkNativeStart && native_সূচক < ut->chunkNativeStart + ut->nativeIndexingLimit) {
        ut->chunkOffset = (int32_t)(native_সূচক - ut->chunkNativeStart);
        c = ut->chunkContents[ut->chunkOffset];
        যদি (U16_IS_SURROGATE(c) == মিথ্যা) {
            ফেরত c;
        }
    }


    utext_set_Native_সূচক(ut, native_সূচক);
    যদি (native_সূচক>=ut->chunkNativeStart && ut->chunkOffset<ut->chunk_দৈর্ঘ্য) {
        c = ut->chunkContents[ut->chunkOffset];
        যদি (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    ফেরত c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    যদি (ut->chunkOffset >= ut->chunk_দৈর্ঘ্য) {
        যদি (ut->pFuncs->access(ut, ut->chunkNativeLimit, সত্য) == মিথ্যা) {
            ফেরত U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunkOffset++];
    যদি (U16_IS_LEAD(c) == মিথ্যা) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        ফেরত c;
    }

    যদি (ut->chunkOffset >= ut->chunk_দৈর্ঘ্য) {
        যদি (ut->pFuncs->access(ut, ut->chunkNativeLimit, সত্য) == মিথ্যা) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            ফেরত c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunkOffset];
    যদি (U16_IS_TRAIL(trail) == মিথ্যা) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        ফেরত c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunkOffset++;   // move iteration position over the trail surrogate.
    ফেরত supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    যদি (ut->chunkOffset <= 0) {
        যদি (ut->pFuncs->access(ut, ut->chunkNativeStart, মিথ্যা) == মিথ্যা) {
            ফেরত U_SENTINEL;
        }
    }
    ut->chunkOffset--;
    c = ut->chunkContents[ut->chunkOffset];
    যদি (U16_IS_TRAIL(c) == মিথ্যা) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        ফেরত c;
    }

    যদি (ut->chunkOffset <= 0) {
        যদি (ut->pFuncs->access(ut, ut->chunkNativeStart, মিথ্যা) == মিথ্যা) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            ফেরত c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunkOffset-1];
    যদি (U16_IS_LEAD(lead) == মিথ্যা) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        ফেরত c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunkOffset--;   // move iteration position over the lead surrogate.
    ফেরত supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t সূচক) {
    UChar32       c      = U_SENTINEL;

    যদি(সূচক<ut->chunkNativeStart || সূচক>=ut->chunkNativeLimit) {
        // Desired position is outside of the current chunk.
        যদি(!ut->pFuncs->access(ut, সূচক, সত্য)) {
            // no chunk available here
            ফেরত U_SENTINEL;
        }
    } অন্যথা যদি (সূচক - ut->chunkNativeStart  <= (int64_t)ut->nativeIndexingLimit) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunkOffset = (int32_t)(সূচক - ut->chunkNativeStart);
    } অন্যথা {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunkOffset = ut->pFuncs->map_Native_সূচক_To_UTF16(ut, সূচক);
    }

    c = ut->chunkContents[ut->chunkOffset++];
    যদি (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_set_Native_সূচক(ut, সূচক);
        c = utext_next32(ut);
    }
    ফেরত c;
}


U_CAPI UChar32 U_EXPORT2
utext_previous32From(UText *ut, int64_t সূচক) {
    //
    //  Return the character preceding the specified index.
    //  Leave the iteration position at the start of the character that was returned.
    //
    UChar32     c_পূর্ব;    // The character preceding cCurr, which is what we will return.

    // Address the chunk containing the position preceding the incoming index
    // A tricky edge case:
    //   We try to test the requested native index against the chunkNativeStart to determine
    //    whether the character preceding the one at the index is in the current chunk.
    //    BUT, this test can fail with UTF-8 (or any other multibyte encoding), when the
    //    requested index is on something other than the first position of the first char.
    //
    যদি(সূচক<=ut->chunkNativeStart || সূচক>ut->chunkNativeLimit) {
        // Requested native index is outside of the current chunk.
        যদি(!ut->pFuncs->access(ut, সূচক, মিথ্যা)) {
            // no chunk available here
            ফেরত U_SENTINEL;
        }
    } অন্যথা যদি(সূচক - ut->chunkNativeStart <= (int64_t)ut->nativeIndexingLimit) {
        // Direct UTF-16 indexing.
        ut->chunkOffset = (int32_t)(সূচক - ut->chunkNativeStart);
    } অন্যথা {
        ut->chunkOffset=ut->pFuncs->map_Native_সূচক_To_UTF16(ut, সূচক);
        যদি (ut->chunkOffset==0 && !ut->pFuncs->access(ut, সূচক, মিথ্যা)) {
            // no chunk available here
            ফেরত U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunkOffset--;
    c_পূর্ব = ut->chunkContents[ut->chunkOffset];

    যদি (U16_IS_SURROGATE(c_পূর্ব)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_set_Native_সূচক(ut, সূচক);
        c_পূর্ব = utext_previous32(ut);
    }
    ফেরত c_পূর্ব;
}


U_CAPI int32_t U_EXPORT2
utext_extract(UText *ut,
             int64_t start, int64_t limit,
             char16_t *dest, int32_t destCapacity,
             UErrorCode *status) {
                 ফেরত ut->pFuncs->extract(ut, start, limit, dest, destCapacity, status);
             }



U_CAPI UBool U_EXPORT2
utext_equals(ধ্রুবক UText *a, ধ্রুবক UText *b) {
    যদি (a==nullptr || b==nullptr ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            ফেরত মিথ্যা;
    }

    যদি (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        ফেরত মিথ্যা;
    }

    যদি (a->context != b->context) {
        // Different sources (different strings)
        ফেরত মিথ্যা;
    }
    যদি (utext_get_Native_সূচক(a) != utext_get_Native_সূচক(b)) {
        // Different current position in the string.
        ফেরত মিথ্যা;
    }

    ফেরত সত্য;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(ধ্রুবক UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    ফেরত b;
}


U_CAPI শূন্য U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_has_Meta_তথ্য(ধ্রুবক UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_তথ্য)) != 0;
    ফেরত b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t nativeStart, int64_t nativeLimit,
             ধ্রুবক char16_t *replacementText, int32_t replacement_দৈর্ঘ্য,
             UErrorCode *status)
{
    যদি (U_FAILURE(*status)) {
        ফেরত 0;
    }
    যদি ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *status = U_NO_WRITE_PERMISSION;
        ফেরত 0;
    }
    int32_t i = ut->pFuncs->replace(ut, nativeStart, nativeLimit, replacementText, replacement_দৈর্ঘ্য, status);
    ফেরত i;
}

U_CAPI শূন্য U_EXPORT2
utext_copy(UText *ut,
          int64_t nativeStart, int64_t nativeLimit,
          int64_t dest_সূচক,
          UBool move,
          UErrorCode *status)
{
    যদি (U_FAILURE(*status)) {
        ফেরত;
    }
    যদি ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *status = U_NO_WRITE_PERMISSION;
        ফেরত;
    }
    ut->pFuncs->copy(ut, nativeStart, nativeLimit, dest_সূচক, move, status);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            ধ্রুবক UText* src,
            UBool deep,
            UBool readOnly,
            UErrorCode* status) {
    যদি (U_FAILURE(*status)) {
        ফেরত dest;
    }
    UText *ফলাফল = src->pFuncs->clone(dest, src, deep, status);
    যদি (U_FAILURE(*status)) {
        ফেরত ফলাফল;
    }
    যদি (ফলাফল == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত ফলাফল;
    }
    যদি (readOnly) {
        utext_freeze(ফলাফল);
    }
    ফেরত ফলাফল;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
গণনা {
    UTEXT_HEAP_ALLOCATED  = 1,      //  1 if ICU has allocated this UText struct on the heap.
                                    //  0 if caller provided storage for the UText.

    UTEXT_EXTRA_HEAP_ALLOCATED = 2, //  1 if ICU has allocated extra storage as a separate
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

কাঠামো ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

স্থির ধ্রুবক UText emptyText = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_setup(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UErrorCode* status) {
    যদি (U_FAILURE(*status)) {
        ফেরত ut;
    }

    যদি (ut == nullptr) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = আকার(UText);
        যদি (extraSpace > 0) {
            spaceRequired = আকার(ExtendedUText) + extraSpace - আকার(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        যদি (ut == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        } অন্যথা {
            *ut = emptyText;
            ut->flags |= UTEXT_HEAP_ALLOCATED;
            যদি (spaceRequired>0) {
                ut->extra_আকার = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } অন্যথা {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        যদি (ut->magic != UTEXT_MAGIC) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        যদি ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != nullptr)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        যদি (extraSpace > ut->extra_আকার) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            যদি (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extra_আকার = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            যদি (ut->pExtra == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } অন্যথা {
                ut->extra_আকার = extraSpace;
                ut->flags |= UTEXT_EXTRA_HEAP_ALLOCATED;
            }
        }
    }
    যদি (U_SUCCESS(*status)) {
        ut->flags |= UTEXT_OPEN;

        // Initialize all remaining fields of the UText.
        //
        ut->context             = nullptr;
        ut->chunkContents       = nullptr;
        ut->p                   = nullptr;
        ut->q                   = nullptr;
        ut->r                   = nullptr;
        ut->a                   = 0;
        ut->b                   = 0;
        ut->c                   = 0;
        ut->chunkOffset         = 0;
        ut->chunk_দৈর্ঘ্য         = 0;
        ut->chunkNativeStart    = 0;
        ut->chunkNativeLimit    = 0;
        ut->nativeIndexingLimit = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = nullptr;
        যদি (ut->pExtra!=nullptr && ut->extra_আকার>0)
            uprv_memset(ut->pExtra, 0, ut->extra_আকার);

    }
    ফেরত ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    যদি (ut==nullptr ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        ফেরত ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    যদি (ut->pFuncs->close != nullptr) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    যদি (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = nullptr;
        ut->flags &= ~UTEXT_EXTRA_HEAP_ALLOCATED;
        ut->extra_আকার = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = nullptr;

    যদি (ut->flags & UTEXT_HEAP_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = nullptr;
    }
    ফেরত ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
স্থির শূন্য
invalidateChunk(UText *ut) {
    ut->chunk_দৈর্ঘ্য = 0;
    ut->chunkNativeLimit = 0;
    ut->chunkNativeStart = 0;
    ut->chunkOffset = 0;
    ut->nativeIndexingLimit = 0;
}

//
// pinIndex        Do range pinning on a native index parameter.
//                 64 bit pinning is done in place.
//                 32 bit truncated result is returned as a convenience for
//                        use in providers that don't need 64 bits.
স্থির int32_t
pin_সূচক(int64_t &সূচক, int64_t limit) {
    যদি (সূচক<0) {
        সূচক = 0;
    } অন্যথা যদি (সূচক > limit) {
        সূচক = limit;
    }
    ফেরত static_cast<int32_t>(সূচক);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
স্থির শূন্য adjustPointer(UText *dest, ধ্রুবক শূন্য **destPtr, ধ্রুবক UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    অক্ষর  *dptr = (অক্ষর *)*destPtr;
    অক্ষর  *dUText = (অক্ষর *)dest;
    অক্ষর  *sUText = (অক্ষর *)src;

    যদি (dptr >= (অক্ষর *)src->pExtra && dptr < ((অক্ষর*)src->pExtra)+src->extra_আকার) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((অক্ষর *)dest->pExtra) + (dptr - (অক্ষর *)src->pExtra);
    } অন্যথা যদি (dptr>=sUText && dptr < sUText+src->আকার_Of_Struct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
স্থির UText * U_CALLCONV
shallowTextClone(UText * dest, ধ্রুবক UText * src, UErrorCode * status) {
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    int32_t  src_Extra_আকার = src->extra_আকার;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_setup(dest, src_Extra_আকার, status);
    যদি (U_FAILURE(*status)) {
        ফেরত dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    শূন্য *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    পূর্ণসংখ্যা আকার_To_Copy = src->আকার_Of_Struct;
    যদি (আকার_To_Copy > dest->আকার_Of_Struct) {
        আকার_To_Copy = dest->আকার_Of_Struct;
    }
    uprv_memcpy(dest, src, আকার_To_Copy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    যদি (src_Extra_আকার > 0) {
        uprv_memcpy(dest->pExtra, src->pExtra, src_Extra_আকার);
    }

    //
    // Relocate any pointers in the target that refer to the UText itself
    //   to point to the cloned copy rather than the original source.
    //
    adjustPointer(dest, &dest->context, src);
    adjustPointer(dest, &dest->p, src);
    adjustPointer(dest, &dest->q, src);
    adjustPointer(dest, &dest->r, src);
    adjustPointer(dest, (ধ্রুবক শূন্য **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

    ফেরত dest;
}


U_CDECL_END



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
গণনা { UTF8_TEXT_CHUNK_আকার=32 };

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

কাঠামো UTF8Buf {
    int32_t   bufNativeStart;                        // Native index of first char in char16_t buf
    int32_t   bufNativeLimit;                        // Native index following last char in buf.
    int32_t   bufStartIdx;                           // First filled position in buf.
    int32_t   bufLimitIdx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   toUCharsMapStart;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_TEXT_CHUNK_আকার+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   mapToNative[UTF8_TEXT_CHUNK_আকার+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   mapToUChars[UTF8_TEXT_CHUNK_আকার*3+6]; // Map native offset from bufNativeStart to
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
স্থির int64_t U_CALLCONV
utf8_Text_দৈর্ঘ্য(UText *ut) {
    যদি (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        ধ্রুবক অক্ষর *r = (ধ্রুবক অক্ষর *)ut->context + ut->c;
        যখন (*r != 0) {
            r++;
        }
        যদি ((r - (ধ্রুবক অক্ষর *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (ধ্রুবক অক্ষর *)ut->context);
        } অন্যথা {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE);
    }
    ফেরত ut->b;
}






স্থির UBool U_CALLCONV
utf8TextAccess(UText *ut, int64_t সূচক, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    ধ্রুবক uint8_t *s8=(ধ্রুবক uint8_t *)ut->context;
    UTF8Buf *u8b = nullptr;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)সূচক;     // Requested index, trimmed to 32 bits.
    int32_t  map_সূচক = 0;
    যদি (সূচক<0) {
        ix=0;
    } অন্যথা যদি (সূচক > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    যদি (ix>length) {
        যদি (length>=0) {
            ix=length;
        } অন্যথা যদি (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            যখন (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            যদি (s8[ut->c] == 0) {
                // We just found the actual length of the string.
                //  Trim the requested index back to that.
                ix     = ut->c;
                ut->b  = ut->c;
                length = ut->c;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    যদি (forward) {
        যদি (ix==ut->chunkNativeLimit) {
            // Check for normal sequential iteration cases first.
            যদি (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunkOffset = ut->chunk_দৈর্ঘ্য;
                ফেরত মিথ্যা;
            } অন্যথা {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                যদি (ix>=altB->bufNativeStart && ix<altB->bufNativeLimit) {
                    যাও swapBuffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            যদি (ix>=u8b->bufNativeStart && ix<u8b->bufNativeLimit) {
                // Requested index is in the other buffer.
                যাও swapBuffers;
            }
            যদি (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                যদি (ix == ut->chunkNativeLimit) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunkOffset = ut->chunk_দৈর্ঘ্য;
                    ফেরত মিথ্যা;
                }
                যদি (ix == u8b->bufNativeLimit) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    যাও swapBuffersAndFail;
                }

                // Neither existing buffer extends to the end of the string.
                যাও make_Stub_বাফার;
            }

            যদি (ix<ut->chunkNativeStart || ix>=ut->chunkNativeLimit) {
                // Requested index is in neither buffer.
                যাও fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            map_সূচক = ix - u8b->toUCharsMapStart;
            U_ASSERT(map_সূচক < (int32_t)আকার(UTF8Buf::mapToUChars));
            ut->chunkOffset = u8b->mapToUChars[map_সূচক] - u8b->bufStartIdx;
            ফেরত সত্য;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    যদি (ix==ut->chunkNativeStart) {
        // Check for normal sequential iteration cases first.
        যদি (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunkOffset = 0;
            ফেরত মিথ্যা;
        } অন্যথা {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            যদি (ix>altB->bufNativeStart && ix<=altB->bufNativeLimit) {
                যাও swapBuffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    যদি (ix>u8b->bufNativeStart && ix<=u8b->bufNativeLimit) {
        // Requested index is in the other buffer.
        যাও swapBuffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    যদি (ix==0) {
        যদি (u8b->bufNativeStart==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            যাও swapBuffersAndFail;
        } অন্যথা {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            যাও make_Stub_বাফার;
        }
    }

    যদি (ix<=ut->chunkNativeStart || ix>ut->chunkNativeLimit) {
        // Requested index is in neither buffer.
        যাও fillReverse;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    map_সূচক = ix - u8b->toUCharsMapStart;
    ut->chunkOffset = u8b->mapToUChars[map_সূচক] - u8b->bufStartIdx;
    যদি (ut->chunkOffset==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        ফেরত মিথ্যা;
    } অন্যথা {
        ফেরত সত্য;
    }



swapBuffers:
    //  The alternate buffer (ut->q) has the string data that was requested.
    //  Swap the primary and alternate buffers, and set the
    //   chunk index into the new primary buffer.
    {
        u8b   = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b;
        ut->chunkContents       = &u8b->buf[u8b->bufStartIdx];
        ut->chunk_দৈর্ঘ্য         = u8b->bufLimitIdx - u8b->bufStartIdx;
        ut->chunkNativeStart    = u8b->bufNativeStart;
        ut->chunkNativeLimit    = u8b->bufNativeLimit;
        ut->nativeIndexingLimit = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->bufNativeStart);
        U_ASSERT(ix<=u8b->bufNativeLimit);
        map_সূচক = ix - u8b->toUCharsMapStart;
        U_ASSERT(map_সূচক>=0);
        U_ASSERT(map_সূচক<(int32_t)আকার(u8b->mapToUChars));
        ut->chunkOffset = u8b->mapToUChars[map_সূচক] - u8b->bufStartIdx;

        ফেরত সত্য;
    }


 swapBuffersAndFail:
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
    ut->chunkContents       = &u8b->buf[u8b->bufStartIdx];
    ut->chunk_দৈর্ঘ্য         = u8b->bufLimitIdx - u8b->bufStartIdx;
    ut->chunkNativeStart    = u8b->bufNativeStart;
    ut->chunkNativeLimit    = u8b->bufNativeLimit;
    ut->nativeIndexingLimit = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    যদি (ix==u8b->bufNativeLimit) {
        ut->chunkOffset = ut->chunk_দৈর্ঘ্য;
    } অন্যথা  {
        ut->chunkOffset = 0;
        U_ASSERT(ix == u8b->bufNativeStart);
    }
    ফেরত মিথ্যা;

make_Stub_বাফার:
    //   The user has done a seek/access past the start or end
    //   of the string.  Rather than loading data that is likely
    //   to never be used, just set up a zero-length buffer at
    //   the position.
    u8b = (UTF8Buf *)ut->q;
    u8b->bufNativeStart   = ix;
    u8b->bufNativeLimit   = ix;
    u8b->bufStartIdx      = 0;
    u8b->bufLimitIdx      = 0;
    u8b->bufNILimit       = 0;
    u8b->toUCharsMapStart = ix;
    u8b->mapToNative[0]   = 0;
    u8b->mapToUChars[0]   = 0;
    যাও swapBuffersAndFail;



fillForward:
    {
        // Move the incoming index to a code point boundary.
        U8_SET_CP_START(s8, 0, ix);

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_swap = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_swap;

        int32_t strLen = ut->b;
        UBool   nulTerminated = মিথ্যা;
        যদি (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = সত্য;
        }

        char16_t   *buf = u8b_swap->buf;
        uint8_t *mapToNative  = u8b_swap->mapToNative;
        uint8_t *mapToUChars  = u8b_swap->mapToUChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    seenNonAscii = মিথ্যা;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        যখন (destIx<UTF8_TEXT_CHUNK_আকার) {
            c = s8[srcIx];
            যদি (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                mapToNative[destIx]    = (uint8_t)(srcIx - ix);
                mapToUChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } অন্যথা {
                // General case, handle everything.
                যদি (seenNonAscii == মিথ্যা) {
                    seenNonAscii = সত্য;
                    u8b_swap->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_পরবর্তী_OR_FFFD(s8, srcIx, strLen, c);
                যদি (c==0 && nulTerminated) {
                    srcIx--;
                    বিরতি;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                করো {
                    mapToNative[dIx++] = (uint8_t)(cIx - ix);
                } যখন (dIx < destIx);

                করো {
                    mapToUChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } যখন (cIx < srcIx);
            }
            যদি (srcIx>=strLen) {
                বিরতি;
            }

        }

        //  store Native <--> Chunk Map entries for the end of the buffer.
        //    There is no actual character here, but the index position is valid.
        mapToNative[destIx]     = (uint8_t)(srcIx - ix);
        mapToUChars[srcIx - ix] = (uint8_t)destIx;

        //  fill in Buffer descriptor
        u8b_swap->bufNativeStart     = ix;
        u8b_swap->bufNativeLimit     = srcIx;
        u8b_swap->bufStartIdx        = 0;
        u8b_swap->bufLimitIdx        = destIx;
        যদি (seenNonAscii == মিথ্যা) {
            u8b_swap->bufNILimit     = destIx;
        }
        u8b_swap->toUCharsMapStart   = u8b_swap->bufNativeStart;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunkOffset         = 0;
        ut->chunk_দৈর্ঘ্য         = u8b_swap->bufLimitIdx;
        ut->chunkNativeStart    = u8b_swap->bufNativeStart;
        ut->chunkNativeLimit    = u8b_swap->bufNativeLimit;
        ut->nativeIndexingLimit = u8b_swap->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        যদি (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            যদি (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE);
            }
        }
        ফেরত সত্য;
    }


fillReverse:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        যদি (ix != ut->b) {
            // Note: this function will only move the index back if it is on a trail byte
            //       and there is a preceding lead byte and the sequence from the lead 
            //       through this trail could be part of a valid UTF-8 sequence
            //       Otherwise the index remains unchanged.
            U8_SET_CP_START(s8, 0, ix);
        }

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_swap = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_swap;

        char16_t   *buf = u8b_swap->buf;
        uint8_t *mapToNative = u8b_swap->mapToNative;
        uint8_t *mapToUChars = u8b_swap->mapToUChars;
        int32_t  toUCharsMapStart = ix - আকার(UTF8Buf::mapToUChars) + 1;
        // Note that toUCharsMapStart can be negative. Happens when the remaining
        // text from current position to the beginning is less than the buffer size.
        // + 1 because mapToUChars must have a slot at the end for the bufNativeLimit entry.
        int32_t  destIx = UTF8_TEXT_CHUNK_আকার+2;   // Start in the overflow region
                                                    //   at end of buffer to leave room
                                                    //   for a surrogate pair at the
                                                    //   buffer start.
        int32_t  srcIx  = ix;
        int32_t  bufNILimit = destIx;
        UChar32   c;

        // Map to/from Native Indexes, fill in for the position at the end of
        //   the buffer.
        //
        mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
        mapToUChars[srcIx - toUCharsMapStart] = (uint8_t)destIx;

        // Fill the chunk buffer
        // Work backwards, filling from the end of the buffer towards the front.
        //
        যখন (destIx>2 && (srcIx - toUCharsMapStart > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            যদি (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(toUCharsMapStart <= srcIx);
                mapToUChars[srcIx - toUCharsMapStart] = (uint8_t)destIx;
                mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
            } অন্যথা {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_পূর্ব_Char_Safe_Body(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                যদি (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                } অন্যথা {
                    buf[destIx]         = U16_TRAIL(c);
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                    buf[--destIx]       = U16_LEAD(c);
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                }

                // Fill in the map from native indexes to UChars buf index.
                করো {
                    mapToUChars[sIx-- - toUCharsMapStart] = (uint8_t)destIx;
                } যখন (sIx >= srcIx);
                U_ASSERT(toUCharsMapStart <= (srcIx+1));

                // Set native indexing limit to be the current position.
                //   We are processing a non-ascii, non-native-indexing char now;
                //     the limit will be here if the rest of the chars to be
                //     added to this buffer are ascii.
                bufNILimit = destIx;
            }
        }
        u8b_swap->bufNativeStart     = srcIx;
        u8b_swap->bufNativeLimit     = ix;
        u8b_swap->bufStartIdx        = destIx;
        u8b_swap->bufLimitIdx        = UTF8_TEXT_CHUNK_আকার+2;
        u8b_swap->bufNILimit         = bufNILimit - u8b_swap->bufStartIdx;
        u8b_swap->toUCharsMapStart   = toUCharsMapStart;

        ut->chunkContents       = &buf[u8b_swap->bufStartIdx];
        ut->chunk_দৈর্ঘ্য         = u8b_swap->bufLimitIdx - u8b_swap->bufStartIdx;
        ut->chunkOffset         = ut->chunk_দৈর্ঘ্য;
        ut->chunkNativeStart    = u8b_swap->bufNativeStart;
        ut->chunkNativeLimit    = u8b_swap->bufNativeLimit;
        ut->nativeIndexingLimit = u8b_swap->bufNILimit;
        ফেরত সত্য;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
স্থির char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t destCapacity,
              int32_t *p_Dest_দৈর্ঘ্য,
              ধ্রুবক অক্ষর* src,
              int32_t src_দৈর্ঘ্য,        // required.  NUL terminated not supported.
              UErrorCode *pErrorCode
              )
{

    char16_t *pDest = dest;
    char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
    UChar32 ch=0;
    int32_t সূচক = 0;
    int32_t req_দৈর্ঘ্য = 0;
    uint8_t* pSrc = (uint8_t*) src;


    যখন((সূচক < src_দৈর্ঘ্য)&&(pDest<pDestLimit)){
        ch = pSrc[সূচক++];
        যদি(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }অন্যথা{
            ch=utf8_পরবর্তী_Char_Safe_Body(pSrc, &সূচক, src_দৈর্ঘ্য, ch, -3);
            যদি(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }অন্যথা{
                *(pDest++)=U16_LEAD(ch);
                যদি(pDest<pDestLimit){
                    *(pDest++)=U16_TRAIL(ch);
                }অন্যথা{
                    req_দৈর্ঘ্য++;
                    বিরতি;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    যখন(সূচক < src_দৈর্ঘ্য){
        ch = pSrc[সূচক++];
        যদি(ch <= 0x7f){
            req_দৈর্ঘ্য++;
        }অন্যথা{
            ch=utf8_পরবর্তী_Char_Safe_Body(pSrc, &সূচক, src_দৈর্ঘ্য, ch, -3);
            req_দৈর্ঘ্য+=U16_দৈর্ঘ্য(ch);
        }
    }

    req_দৈর্ঘ্য+=(int32_t)(pDest - dest);

    যদি(p_Dest_দৈর্ঘ্য){
        *p_Dest_দৈর্ঘ্য = req_দৈর্ঘ্য;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,req_দৈর্ঘ্য,pErrorCode);

    ফেরত dest;
}



স্থির int32_t U_CALLCONV
utf8TextExtract(UText *ut,
                int64_t start, int64_t limit,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pin_সূচক(start, length);
    int32_t  limit32 = pin_সূচক(limit, length);

    যদি(start32>limit32) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    ধ্রুবক uint8_t *buf = (ধ্রুবক uint8_t *)ut->context;
    পূর্ণসংখ্যা i;
    যদি (start32 < ut->chunkNativeLimit) {
        জন্য (i=0; i<3; i++) {
            যদি (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                বিরতি;
            }
            start32--;
        }
    }

    যদি (limit32 < ut->chunkNativeLimit) {
        জন্য (i=0; i<3; i++) {
            যদি (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                বিরতি;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t dest_দৈর্ঘ্য=0;
    utext_strFromUTF8(dest, destCapacity, &dest_দৈর্ঘ্য,
                    (ধ্রুবক অক্ষর *)ut->context+start32, limit32-start32,
                    pErrorCode);
    utf8TextAccess(ut, limit32, সত্য);
    ফেরত dest_দৈর্ঘ্য;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
স্থির int64_t U_CALLCONV
utf8TextMapOffsetToNative(ধ্রুবক UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunkOffset>ut->nativeIndexingLimit && ut->chunkOffset<=ut->chunk_দৈর্ঘ্য);
    int32_t nativeOffset = u8b->mapToNative[ut->chunkOffset + u8b->bufStartIdx] + u8b->toUCharsMapStart;
    U_ASSERT(nativeOffset >= ut->chunkNativeStart && nativeOffset <= ut->chunkNativeLimit);
    ফেরত nativeOffset;
}

//
// Map a native index to the corresponding chunk offset
//
স্থির int32_t U_CALLCONV
utf8_Text_Map_সূচক_To_UTF16(ধ্রুবক UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t সূচক = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(সূচক>=ut->chunkNativeStart+ut->nativeIndexingLimit);
    U_ASSERT(সূচক<=ut->chunkNativeLimit);
    int32_t map_সূচক = সূচক - u8b->toUCharsMapStart;
    U_ASSERT(map_সূচক < (int32_t)আকার(UTF8Buf::mapToUChars));
    int32_t offset = u8b->mapToUChars[map_সূচক] - u8b->bufStartIdx;
    U_ASSERT(offset>=0 && offset<=ut->chunk_দৈর্ঘ্য);
    ফেরত offset;
}

স্থির UText * U_CALLCONV
utf8TextClone(UText *dest, ধ্রুবক UText *src, UBool deep, UErrorCode *status)
{
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //
    // TODO:  There is an issue with using utext_nativeLength().
    //        That function is non-const in cases where the input was NUL terminated
    //          and the length has not yet been determined.
    //        This function (clone()) is const.
    //        There potentially a thread safety issue lurking here.
    //
    যদি (deep && U_SUCCESS(*status)) {
        int32_t  len = (int32_t)utext_native_দৈর্ঘ্য((UText *)src);
        অক্ষর *copyStr = (অক্ষর *)uprv_malloc(len+1);
        যদি (copyStr == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    ফেরত dest;
}


স্থির শূন্য U_CALLCONV
utf8TextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    যদি (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        অক্ষর *s = (অক্ষর *)ut->context;
        uprv_free(s);
        ut->context = nullptr;
    }
}

U_CDECL_END


স্থির ধ্রুবক কাঠামো UTextFuncs utf8Funcs =
{
    আকার(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8TextClone,
    utf8_Text_দৈর্ঘ্য,
    utf8TextAccess,
    utf8TextExtract,
    nullptr,                /* replace*/
    nullptr,                /* copy   */
    utf8TextMapOffsetToNative,
    utf8_Text_Map_সূচক_To_UTF16,
    utf8TextClose,
    nullptr,                // spare 1
    nullptr,                // spare 2
    nullptr                 // spare 3
};


স্থির ধ্রুবক অক্ষর gEmptyString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               ধ্রুবক অক্ষর* s U_LIFETIME_BOUND,
               int64_t length,
               UErrorCode* status) {
    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি(s==nullptr && length==0) {
        s = gEmptyString;
    }

    যদি(s==nullptr || length<-1 || length>INT32_MAX) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    ut = utext_setup(ut, আকার(UTF8Buf) * 2, status);
    যদি (U_FAILURE(*status)) {
        ফেরত ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    যদি (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (অক্ষর *)ut->pExtra + আকার(UTF8Buf);
    ফেরত ut;

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
গণনা { REP_TEXT_CHUNK_আকার=10 };

কাঠামো ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_TEXT_CHUNK_আকার+1];
};


U_CDECL_BEGIN

স্থির UText * U_CALLCONV
repTextClone(UText *dest, ধ্রুবক UText *src, UBool deep, UErrorCode *status) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    যদি (deep && U_SUCCESS(*status)) {
        ধ্রুবক Replaceable *replSrc = (ধ্রুবক Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ফেরত dest;
}


স্থির শূন্য U_CALLCONV
repTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    যদি (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        Replaceable *rep = (Replaceable *)ut->context;
        delete rep;
        ut->context = nullptr;
    }
}


স্থির int64_t U_CALLCONV
rep_Text_দৈর্ঘ্য(UText *ut) {
    ধ্রুবক Replaceable *replSrc = (ধ্রুবক Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    ফেরত len;
}


স্থির UBool U_CALLCONV
repTextAccess(UText *ut, int64_t সূচক, UBool forward) {
    ধ্রুবক Replaceable *rep=(ধ্রুবক Replaceable *)ut->context;
    int32_t length=rep->length();   // Full length of the input text (bigger than a chunk)

    // clip the requested index to the limits of the text.
    int32_t index32 = pin_সূচক(সূচক, length);
    U_ASSERT(সূচক<=INT32_MAX);


    /*
     * Compute start/limit boundaries around index, for a segment of text
     * to be extracted.
     * To allow for the possibility that our user gave an index to the trailing
     * half of a surrogate pair, we must request one extra preceding char16_t when
     * going in the forward direction.  This will ensure that the buffer has the
     * entire code point at the specified index.
     */
    যদি(forward) {

        যদি (index32>=ut->chunkNativeStart && index32<ut->chunkNativeLimit) {
            // Buffer already contains the requested position.
            ut->chunkOffset = (int32_t)(সূচক - ut->chunkNativeStart);
            ফেরত সত্য;
        }
        যদি (index32>=length && ut->chunkNativeLimit==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunkOffset = length - (int32_t)ut->chunkNativeStart;
            ফেরত মিথ্যা;
        }

        ut->chunkNativeLimit = সূচক + REP_TEXT_CHUNK_আকার - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        যদি(ut->chunkNativeLimit > length) {
            ut->chunkNativeLimit = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunkNativeStart = ut->chunkNativeLimit - REP_TEXT_CHUNK_আকার;
        যদি(ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }
    } অন্যথা {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        যদি (index32>ut->chunkNativeStart && index32<=ut->chunkNativeLimit) {
            // Requested position already in buffer.
            ut->chunkOffset = index32 - (int32_t)ut->chunkNativeStart;
            ফেরত সত্য;
        }
        যদি (index32==0 && ut->chunkNativeStart==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunkOffset = 0;
            ফেরত মিথ্যা;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunkNativeStart = index32 + 1 - REP_TEXT_CHUNK_আকার;
        যদি (ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }

        ut->chunkNativeLimit = index32 + 1;
        যদি (ut->chunkNativeLimit > length) {
            ut->chunkNativeLimit = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString বাফার(ex->s, 0 /*buffer length*/, REP_TEXT_CHUNK_আকার /*buffer capacity*/);
    rep->extractBetween((int32_t)ut->chunkNativeStart, (int32_t)ut->chunkNativeLimit, বাফার);

    ut->chunkContents  = ex->s;
    ut->chunk_দৈর্ঘ্য    = (int32_t)(ut->chunkNativeLimit - ut->chunkNativeStart);
    ut->chunkOffset    = (int32_t)(index32 - ut->chunkNativeStart);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    যদি (ut->chunkNativeLimit < length &&
        U16_IS_LEAD(ex->s[ut->chunk_দৈর্ঘ্য-1])) {
            ut->chunk_দৈর্ঘ্য--;
            ut->chunkNativeLimit--;
            যদি (ut->chunkOffset > ut->chunk_দৈর্ঘ্য) {
                ut->chunkOffset = ut->chunk_দৈর্ঘ্য;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    যদি(ut->chunkNativeStart>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunkNativeStart);
        --(ut->chunk_দৈর্ঘ্য);
        --(ut->chunkOffset);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_SET_CP_START(ut->chunkContents, 0, ut->chunkOffset);

    // Use fast indexing for get/setNativeIndex()
    ut->nativeIndexingLimit = ut->chunk_দৈর্ঘ্য;

    ফেরত সত্য;
}



স্থির int32_t U_CALLCONV
repTextExtract(UText *ut,
               int64_t start, int64_t limit,
               char16_t *dest, int32_t destCapacity,
               UErrorCode *status) {
    ধ্রুবক Replaceable *rep=(ধ্রুবক Replaceable *)ut->context;
    int32_t  length=rep->length();

    যদি(U_FAILURE(*status)) {
        ফেরত 0;
    }
    যদি(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
    }
    যদি(start>limit) {
        *status=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    int32_t  start32 = pin_সূচক(start, length);
    int32_t  limit32 = pin_সূচক(limit, length);

    // adjust start, limit if they point to trail half of surrogates
    যদি (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    যদি (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    যদি(length>destCapacity) {
        limit32 = start32 + destCapacity;
    }
    UnicodeString বাফার(dest, 0, destCapacity); // writable alias
    rep->extractBetween(start32, limit32, বাফার);
    repTextAccess(ut, limit32, সত্য);

    ফেরত u_terminateUChars(dest, destCapacity, length, status);
}

স্থির int32_t U_CALLCONV
repTextReplace(UText *ut,
               int64_t start, int64_t limit,
               ধ্রুবক char16_t *src, int32_t length,
               UErrorCode *status) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t old_দৈর্ঘ্য;

    যদি(U_FAILURE(*status)) {
        ফেরত 0;
    }
    যদি(src==nullptr && length!=0) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    old_দৈর্ঘ্য=rep->length(); // will subtract from new length
    যদি(start>limit ) {
        *status=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    int32_t start32 = pin_সূচক(start, old_দৈর্ঘ্য);
    int32_t limit32 = pin_সূচক(limit, old_দৈর্ঘ্য);

    // Snap start & limit to code point boundaries.
    যদি (start32<old_দৈর্ঘ্য && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    যদি (limit32<old_দৈর্ঘ্য && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t new_দৈর্ঘ্য = rep->length();
    int32_t দৈর্ঘ্য_Delta = new_দৈর্ঘ্য - old_দৈর্ঘ্য;

    // Is the UText chunk buffer OK?
    যদি (ut->chunkNativeLimit > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t new_সূচক_Pos = limit32 + দৈর্ঘ্য_Delta;
    repTextAccess(ut, new_সূচক_Pos, সত্য);

    ফেরত দৈর্ঘ্য_Delta;
}


স্থির শূন্য U_CALLCONV
repTextCopy(UText *ut,
                int64_t start, int64_t limit,
                int64_t dest_সূচক,
                UBool move,
                UErrorCode *status)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    যদি(U_FAILURE(*status)) {
        ফেরত;
    }
    যদি (start>limit || (start<dest_সূচক && dest_সূচক<limit))
    {
        *status=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }

    int32_t start32     = pin_সূচক(start, length);
    int32_t limit32     = pin_সূচক(limit, length);
    int32_t destIndex32 = pin_সূচক(dest_সূচক, length);

    // TODO:  snap input parameters to code point boundaries.

    যদি(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t seg_দৈর্ঘ্য=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        যদি(destIndex32<start32) {
            start32+=seg_দৈর্ঘ্য;
            limit32+=seg_দৈর্ঘ্য;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } অন্যথা {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t first_Affected_সূচক = destIndex32;
    যদি (move && start32<first_Affected_সূচক) {
        first_Affected_সূচক = start32;
    }
    যদি (first_Affected_সূচক < ut->chunkNativeLimit) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  native_Iter_সূচক = destIndex32 + limit32 - start32;
    যদি (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        native_Iter_সূচক = destIndex32;
    }

    // Set position, reload chunk if needed.
    repTextAccess(ut, native_Iter_সূচক, সত্য);
}

স্থির ধ্রুবক কাঠামো UTextFuncs repFuncs =
{
    আকার(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    repTextClone,
    rep_Text_দৈর্ঘ্য,
    repTextAccess,
    repTextExtract,
    repTextReplace,
    repTextCopy,
    nullptr,              // MapOffsetToNative,
    nullptr,              // MapIndexToUTF16,
    repTextClose,
    nullptr,              // spare 1
    nullptr,              // spare 2
    nullptr               // spare 3
};


U_CAPI UText* U_EXPORT2
utext_openReplaceable(UText* ut U_LIFETIME_BOUND,
                      Replaceable* rep U_LIFETIME_BOUND,
                      UErrorCode* status) {
    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি(rep==nullptr) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    ut = utext_setup(ut, আকার(ReplExtra), status);
    যদি(U_FAILURE(*status)) {
        ফেরত ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    যদি(rep->has_Meta_তথ্য()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_তথ্য);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    ফেরত ut;
}

U_CDECL_END








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


স্থির UText * U_CALLCONV
unistrTextClone(UText *dest, ধ্রুবক UText *src, UBool deep, UErrorCode *status) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    যদি (deep && U_SUCCESS(*status)) {
        ধ্রুবক UnicodeString *srcString = (ধ্রুবক UnicodeString *)src->context;
        dest->context = নতুন UnicodeString(*srcString);
        dest->chunkContents = ((ধ্রুবক UnicodeString *)dest->context)->get_বাফার();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ফেরত dest;
}

স্থির শূন্য U_CALLCONV
unistrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    যদি (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        delete str;
        ut->context = nullptr;
    }
}


স্থির int64_t U_CALLCONV
unistr_Text_দৈর্ঘ্য(UText *t) {
    ফেরত ((ধ্রুবক UnicodeString *)t->context)->length();
}


স্থির UBool U_CALLCONV
unistrTextAccess(UText *ut, int64_t সূচক, UBool  forward) {
    int32_t length  = ut->chunk_দৈর্ঘ্য;
    ut->chunkOffset = pin_সূচক(সূচক, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && সূচক<length) || (!forward && সূচক>0);
    ফেরত retVal;
}



স্থির int32_t U_CALLCONV
unistrTextExtract(UText *t,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *pErrorCode) {
    ধ্রুবক UnicodeString *us=(ধ্রুবক UnicodeString *)t->context;
    int32_t length=us->length();

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
    যদি(start<0 || start>limit) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    int32_t start32 = start<length ? us->getChar32Start((int32_t)start) : length;
    int32_t limit32 = limit<length ? us->getChar32Start((int32_t)limit) : length;

    length=limit32-start32;
    যদি (destCapacity>0 && dest!=nullptr) {
        int32_t trimmed_দৈর্ঘ্য = length;
        যদি(trimmed_দৈর্ঘ্য>destCapacity) {
            trimmed_দৈর্ঘ্য=destCapacity;
        }
        us->extract(start32, trimmed_দৈর্ঘ্য, dest);
        t->chunkOffset = start32+trimmed_দৈর্ঘ্য;
    } অন্যথা {
        t->chunkOffset = start32;
    }
    u_terminateUChars(dest, destCapacity, length, pErrorCode);
    ফেরত length;
}

স্থির int32_t U_CALLCONV
unistrTextReplace(UText *ut,
                  int64_t start, int64_t limit,
                  ধ্রুবক char16_t *src, int32_t length,
                  UErrorCode *pErrorCode) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t old_দৈর্ঘ্য;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(src==nullptr && length!=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
    যদি(start>limit) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }
    old_দৈর্ঘ্য=us->length();
    int32_t start32 = pin_সূচক(start, old_দৈর্ঘ্য);
    int32_t limit32 = pin_সূচক(limit, old_দৈর্ঘ্য);
    যদি (start32 < old_দৈর্ঘ্য) {
        start32 = us->getChar32Start(start32);
    }
    যদি (limit32 < old_দৈর্ঘ্য) {
        limit32 = us->getChar32Start(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t new_দৈর্ঘ্য = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->get_বাফার();
    ut->chunk_দৈর্ঘ্য      = new_দৈর্ঘ্য;
    ut->chunkNativeLimit = new_দৈর্ঘ্য;
    ut->nativeIndexingLimit = new_দৈর্ঘ্য;

    // Set iteration position to the point just following the newly inserted text.
    int32_t দৈর্ঘ্য_Delta = new_দৈর্ঘ্য - old_দৈর্ঘ্য;
    ut->chunkOffset = limit32 + দৈর্ঘ্য_Delta;

    ফেরত দৈর্ঘ্য_Delta;
}

স্থির শূন্য U_CALLCONV
unistrTextCopy(UText *ut,
               int64_t start, int64_t limit,
               int64_t dest_সূচক,
               UBool move,
               UErrorCode *pErrorCode) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    int32_t start32 = pin_সূচক(start, length);
    int32_t limit32 = pin_সূচক(limit, length);
    int32_t destIndex32 = pin_সূচক(dest_সূচক, length);

    যদি( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }

    যদি(move) {
        // move: copy to destIndex, then remove original
        int32_t seg_দৈর্ঘ্য=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        যদি(destIndex32<start32) {
            start32+=seg_দৈর্ঘ্য;
        }
        us->remove(start32, seg_দৈর্ঘ্য);
    } অন্যথা {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->get_বাফার();
    যদি (move==মিথ্যা) {
        // copy operation, string length grows
        ut->chunk_দৈর্ঘ্য += limit32-start32;
        ut->chunkNativeLimit = ut->chunk_দৈর্ঘ্য;
        ut->nativeIndexingLimit = ut->chunk_দৈর্ঘ্য;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunkOffset = destIndex32+limit32-start32;
    যদি (move && destIndex32>start32) {
        ut->chunkOffset = destIndex32;
    }

}

স্থির ধ্রুবক কাঠামো UTextFuncs unistrFuncs =
{
    আকার(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistrTextClone,
    unistr_Text_দৈর্ঘ্য,
    unistrTextAccess,
    unistrTextExtract,
    unistrTextReplace,
    unistrTextCopy,
    nullptr,                // MapOffsetToNative,
    nullptr,                // MapIndexToUTF16,
    unistrTextClose,
    nullptr,                // spare 1
    nullptr,                // spare 2
    nullptr                 // spare 3
};



U_CDECL_END


U_CAPI UText* U_EXPORT2
utext_openUnicodeString(UText* ut U_LIFETIME_BOUND,
                        UnicodeString* s U_LIFETIME_BOUND,
                        UErrorCode* status) {
    ut = utext_openConstUnicodeString(ut, s, status);
    যদি (U_SUCCESS(*status)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    ফেরত ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             ধ্রুবক UnicodeString* s U_LIFETIME_BOUND,
                             UErrorCode* status) {
    যদি (U_SUCCESS(*status) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, nullptr, 0, status);
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত ut;
    }
    ut = utext_setup(ut, 0, status);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    যদি (U_SUCCESS(*status)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->get_বাফার();
        ut->chunk_দৈর্ঘ্য         = s->length();
        ut->chunkNativeStart    = 0;
        ut->chunkNativeLimit    = ut->chunk_দৈর্ঘ্য;
        ut->nativeIndexingLimit = ut->chunk_দৈর্ঘ্য;
    }
    ফেরত ut;
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


স্থির UText * U_CALLCONV
ucstrTextClone(UText *dest, ধ্রুবক UText * src, UBool deep, UErrorCode * status) {
    // First do a generic shallow clone.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    যদি (deep && U_SUCCESS(*status)) {
        U_ASSERT(utext_native_দৈর্ঘ্য(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_native_দৈর্ঘ্য(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        ধ্রুবক char16_t *srcStr = (ধ্রুবক char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * আকার(char16_t));
        যদি (copyStr == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            int64_t i;
            জন্য (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    ফেরত dest;
}


স্থির শূন্য U_CALLCONV
ucstrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    যদি (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = nullptr;
    }
}



স্থির int64_t U_CALLCONV
ucstr_Text_দৈর্ঘ্য(UText *ut) {
    যদি (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        ধ্রুবক char16_t  *str = (ধ্রুবক char16_t *)ut->context;
        জন্য (;;) {
            যদি (str[ut->chunkNativeLimit] == 0) {
                বিরতি;
            }
            ut->chunkNativeLimit++;
        }
        ut->a = ut->chunkNativeLimit;
        ut->chunk_দৈর্ঘ্য = (int32_t)ut->chunkNativeLimit;
        ut->nativeIndexingLimit = ut->chunk_দৈর্ঘ্য;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE);
    }
    ফেরত ut->a;
}


স্থির UBool U_CALLCONV
ucstrTextAccess(UText *ut, int64_t সূচক, UBool  forward) {
    ধ্রুবক char16_t *str   = (ধ্রুবক char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    যদি (সূচক<0) {
        সূচক = 0;
    } অন্যথা যদি (সূচক < ut->chunkNativeLimit) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_SET_CP_START(str, 0, সূচক);
    } অন্যথা যদি (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        সূচক = ut->a;
    } অন্যথা {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t scanLimit = (int32_t)সূচক + 32;
        যদি ((সূচক + 32)>INT32_MAX || (সূচক + 32)<0 ) {   // note: int64 expression
            scanLimit = INT32_MAX;
        }

        int32_t chunkLimit = (int32_t)ut->chunkNativeLimit;
        জন্য (; chunkLimit<scanLimit; chunkLimit++) {
            যদি (str[chunkLimit] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunkLimit;
                ut->chunk_দৈর্ঘ্য = chunkLimit;
                ut->nativeIndexingLimit = chunkLimit;
                যদি (সূচক >= chunkLimit) {
                    সূচক = chunkLimit;
                } অন্যথা {
                    U16_SET_CP_START(str, 0, সূচক);
                }

                ut->chunkNativeLimit = chunkLimit;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE);
                যাও breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_SET_CP_START(str, 0, সূচক);
        যদি (chunkLimit == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunkLimit;
            ut->chunk_দৈর্ঘ্য = chunkLimit;
            ut->nativeIndexingLimit = chunkLimit;
            যদি (সূচক > chunkLimit) {
                সূচক = chunkLimit;
            }
            ut->chunkNativeLimit = chunkLimit;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE);
        } অন্যথা {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            যদি (U16_IS_LEAD(str[chunkLimit-1])) {
                --chunkLimit;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunkNativeLimit = chunkLimit;
            ut->nativeIndexingLimit = chunkLimit;
            ut->chunk_দৈর্ঘ্য = chunkLimit;
        }

    }
breakout:
    U_ASSERT(সূচক<=INT32_MAX);
    ut->chunkOffset = (int32_t)সূচক;

    // Check whether request is at the start or end
    UBool retVal = (forward && সূচক<ut->chunkNativeLimit) || (!forward && সূচক>0);
    ফেরত retVal;
}



স্থির int32_t U_CALLCONV
ucstrTextExtract(UText *ut,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *pErrorCode)
{
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(destCapacity<0 || (dest==nullptr && destCapacity>0) || start>limit) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstrTextAccess(ut, start, সত্য);
    ধ্রুবক char16_t *s=ut->chunkContents;
    start32 = ut->chunkOffset;

    int32_t str_দৈর্ঘ্য=(int32_t)ut->a;
    যদি (str_দৈর্ঘ্য >= 0) {
        limit32 = pin_সূচক(limit, str_দৈর্ঘ্য);
    } অন্যথা {
        limit32 = pin_সূচক(limit, INT32_MAX);
    }
    di = 0;
    জন্য (si=start32; si<limit32; si++) {
        যদি (str_দৈর্ঘ্য<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunkNativeLimit    = si;
            ut->chunk_দৈর্ঘ্য         = si;
            ut->nativeIndexingLimit = si;
            str_দৈর্ঘ্য               = si;
            limit32                 = si;
            বিরতি;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        যদি (di<destCapacity) {
            // only store if there is space.
            dest[di] = s[si];
        } অন্যথা {
            যদি (str_দৈর্ঘ্য>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                বিরতি;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    যদি (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<str_দৈর্ঘ্য || str_দৈর্ঘ্য<0)  && U16_IS_TRAIL(s[si])))
    {
        যদি (di<destCapacity) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    যদি (si <= ut->chunkNativeLimit) {
        ut->chunkOffset = si;
    } অন্যথা {
        ucstrTextAccess(ut, si, সত্য);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, destCapacity, di, pErrorCode);
    ফেরত di;
}

স্থির ধ্রুবক কাঠামো UTextFuncs ucstrFuncs =
{
    আকার(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstrTextClone,
    ucstr_Text_দৈর্ঘ্য,
    ucstrTextAccess,
    ucstrTextExtract,
    nullptr,              // Replace
    nullptr,              // Copy
    nullptr,              // MapOffsetToNative,
    nullptr,              // MapIndexToUTF16,
    ucstrTextClose,
    nullptr,              // spare 1
    nullptr,              // spare 2
    nullptr,              // spare 3
};

U_CDECL_END

স্থির ধ্রুবক char16_t gEmptyUString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 ধ্রুবক char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UErrorCode* status) {
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি(s==nullptr && length==0) {
        s = gEmptyUString;
    }
    যদি (s==nullptr || length < -1 || length>INT32_MAX) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    ut = utext_setup(ut, 0, status);
    যদি (U_SUCCESS(*status)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        যদি (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_দৈর্ঘ্য_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunkNativeStart     = 0;
        ut->chunkNativeLimit     = length>=0? length : 0;
        ut->chunk_দৈর্ঘ্য          = (int32_t)ut->chunkNativeLimit;
        ut->chunkOffset          = 0;
        ut->nativeIndexingLimit  = ut->chunk_দৈর্ঘ্য;
    }
    ফেরত ut;
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
#define CIBuf_আকার 16

U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
charIterTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    delete ci;
    ut->r = nullptr;
}

স্থির int64_t U_CALLCONV
char_Iter_Text_দৈর্ঘ্য(UText *ut) {
    ফেরত (int32_t)ut->a;
}

স্থির UBool U_CALLCONV
charIterTextAccess(UText *ut, int64_t সূচক, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clipped_সূচক = (int32_t)সূচক;
    যদি (clipped_সূচক<0) {
        clipped_সূচক=0;
    } অন্যথা যদি (clipped_সূচক>=ut->a) {
        clipped_সূচক=(int32_t)ut->a;
    }
    int32_t needed_সূচক = clipped_সূচক;
    যদি (!forward && needed_সূচক>0) {
        // reverse iteration, want the position just before what was asked for.
        needed_সূচক--;
    } অন্যথা যদি (forward && needed_সূচক==ut->a && needed_সূচক>0) {
        // Forward iteration, don't ask for something past the end of the text.
        needed_সূচক--;
    }

    // Find the native index of the start of the buffer containing what we want.
    needed_সূচক -= needed_সূচক % CIBuf_আকার;

    char16_t *buf = nullptr;
    UBool  needChunkSetup = সত্য;
    পূর্ণসংখ্যা    i;
    যদি (ut->chunkNativeStart == needed_সূচক) {
        // The buffer we want is already the current chunk.
        needChunkSetup = মিথ্যা;
    } অন্যথা যদি (ut->b == needed_সূচক) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } অন্যথা যদি (ut->c == needed_সূচক) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } অন্যথা {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        যদি (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->set_সূচক(needed_সূচক);
        জন্য (i=0; i<CIBuf_আকার; i++) {
            buf[i] = ci->পরবর্তী_Post_Inc();
            যদি (i+needed_সূচক > ut->a) {
                বিরতি;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    যদি (needChunkSetup) {
        ut->chunkContents = buf;
        ut->chunk_দৈর্ঘ্য   = CIBuf_আকার;
        ut->chunkNativeStart = needed_সূচক;
        ut->chunkNativeLimit = needed_সূচক + CIBuf_আকার;
        যদি (ut->chunkNativeLimit > ut->a) {
            ut->chunkNativeLimit = ut->a;
            ut->chunk_দৈর্ঘ্য  = (int32_t)(ut->chunkNativeLimit)-(int32_t)(ut->chunkNativeStart);
        }
        ut->nativeIndexingLimit = ut->chunk_দৈর্ঘ্য;
        U_ASSERT(ut->chunkOffset>=0 && ut->chunkOffset<=CIBuf_আকার);
    }
    ut->chunkOffset = clipped_সূচক - (int32_t)ut->chunkNativeStart;
    UBool success = (forward? ut->chunkOffset<ut->chunk_দৈর্ঘ্য : ut->chunkOffset>0);
    ফেরত success;
}

স্থির UText * U_CALLCONV
charIterTextClone(UText *dest, ধ্রুবক UText *src, UBool deep, UErrorCode * status) {
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    যদি (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *status = U_UNSUPPORTED_ERROR;
        ফেরত nullptr;
    } অন্যথা {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, status);
        যদি (U_FAILURE(*status)) {
            ফেরত dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_get_Native_সূচক((UText *)src);
        utext_set_Native_সূচক(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    ফেরত dest;
}

স্থির int32_t U_CALLCONV
charIterTextExtract(UText *ut,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *status)
{
    যদি(U_FAILURE(*status)) {
        ফেরত 0;
    }
    যদি(destCapacity<0 || (dest==nullptr && destCapacity>0) || start>limit) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pin_সূচক(start, length);
    int32_t  limit32 = pin_সূচক(limit, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copyLimit;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->setIndex32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->get_সূচক();
    copyLimit = srci;
    যখন (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_দৈর্ঘ্য(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        যদি (desti+len <= destCapacity) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copyLimit = srci+len;
        } অন্যথা {
            desti += len;
            *status = U_বাফার_OVERFLOW_ERROR;
        }
        srci += len;
    }

    charIterTextAccess(ut, copyLimit, সত্য);

    u_terminateUChars(dest, destCapacity, desti, status);
    ফেরত desti;
}

স্থির ধ্রুবক কাঠামো UTextFuncs charIterFuncs =
{
    আকার(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    charIterTextClone,
    char_Iter_Text_দৈর্ঘ্য,
    charIterTextAccess,
    charIterTextExtract,
    nullptr,                // Replace
    nullptr,                // Copy
    nullptr,                // MapOffsetToNative,
    nullptr,                // MapIndexToUTF16,
    charIterTextClose,
    nullptr,                // spare 1
    nullptr,                // spare 2
    nullptr                 // spare 3
};
U_CDECL_END

U_CAPI UText* U_EXPORT2
utext_openCharacterIterator(UText* ut U_LIFETIME_BOUND,
                            CharacterIterator* ci U_LIFETIME_BOUND,
                            UErrorCode* status) {
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    যদি (ci->start_সূচক() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *status = U_UNSUPPORTED_ERROR;
        ফেরত nullptr;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBuf_আকার * আকার(char16_t);
    ut = utext_setup(ut, extraSpace, status);
    যদি (U_SUCCESS(*status)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->end_সূচক();        // Length of text
        ut->p                    = ut->pExtra;            // First buffer
        ut->b                    = -1;                    // Native index of first buffer contents
        ut->q                    = (char16_t*)ut->pExtra+CIBuf_আকার;  // Second buffer
        ut->c                    = -1;                    // Native index of second buffer contents

        // Initialize current chunk contents to be empty.
        //   First access will fault something in.
        //   Note:  The initial nativeStart and chunkOffset must sum to zero
        //          so that getNativeIndex() will correctly compute to zero
        //          if no call to Access() has ever been made.  They can't be both
        //          zero without Access() thinking that the chunk is valid.
        ut->chunkContents        = (char16_t *)ut->p;
        ut->chunkNativeStart     = -1;
        ut->chunkOffset          = 1;
        ut->chunkNativeLimit     = 0;
        ut->chunk_দৈর্ঘ্য          = 0;
        ut->nativeIndexingLimit  = ut->chunkOffset;  // enables native indexing
    }
    ফেরত ut;
}

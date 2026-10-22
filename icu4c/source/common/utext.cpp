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

#அந்தர்பாவ <cstddef>

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/chariter.h"
#அந்தர்பாவ "unicode/utext.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "putilimp.h"

U_NAMESPACE_USE

#define I32_FLAG(bitIndex) ((int32_t)1<<(bitIndex))


நிலையான UBool
utext_access(UText *ut, int64_t index, UBool forward) {
    திரும்பு ut->pFuncs->access(ut, index, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    எனில் (delta > 0) {
        செய் {
            எனில்(ut->chunkOffset>=ut->chunkLength && !utext_access(ut, ut->chunkNativeLimit, உண்மை)) {
                திரும்பு பொய்;
            }
            c = ut->chunkContents[ut->chunkOffset];
            எனில் (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                எனில் (c == U_SENTINEL) {
                    திரும்பு பொய்;
                }
            } இல்லையெனில் {
                ut->chunkOffset++;
            }
        } போது(--delta>0);

    } இல்லையெனில் எனில் (delta<0) {
        செய் {
            எனில்(ut->chunkOffset<=0 && !utext_access(ut, ut->chunkNativeStart, பொய்)) {
                திரும்பு பொய்;
            }
            c = ut->chunkContents[ut->chunkOffset-1];
            எனில் (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                எனில் (c == U_SENTINEL) {
                    திரும்பு பொய்;
                }
            } இல்லையெனில் {
                ut->chunkOffset--;
            }
        } போது(++delta<0);
    }

    திரும்பு உண்மை;
}


U_CAPI int64_t U_EXPORT2
utext_nativeLength(UText *ut) {
    திரும்பு ut->pFuncs->nativeLength(ut);
}


U_CAPI UBool U_EXPORT2
utext_isLengthExpensive(மாறிலி UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE)) != 0;
    திரும்பு r;
}


U_CAPI int64_t U_EXPORT2
utext_getNativeIndex(மாறிலி UText *ut) {
    எனில்(ut->chunkOffset <= ut->nativeIndexingLimit) {
        திரும்பு ut->chunkNativeStart+ut->chunkOffset;
    } இல்லையெனில் {
        திரும்பு ut->pFuncs->mapOffsetToNative(ut);
    }
}


U_CAPI வெறுமை U_EXPORT2
utext_setNativeIndex(UText *ut, int64_t index) {
    எனில்(index<ut->chunkNativeStart || index>=ut->chunkNativeLimit) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, index, உண்மை);
    } இல்லையெனில் எனில்((int32_t)(index - ut->chunkNativeStart) <= ut->nativeIndexingLimit) {
        // utf-16 indexing.
        ut->chunkOffset=(int32_t)(index-ut->chunkNativeStart);
    } இல்லையெனில் {
         ut->chunkOffset=ut->pFuncs->mapNativeIndexToUTF16(ut, index);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    எனில் (ut->chunkOffset<ut->chunkLength) {
        char16_t c= ut->chunkContents[ut->chunkOffset];
        எனில் (U16_IS_TRAIL(c)) {
            எனில் (ut->chunkOffset==0) {
                ut->pFuncs->access(ut, ut->chunkNativeStart, பொய்);
            }
            எனில் (ut->chunkOffset>0) {
                char16_t lead = ut->chunkContents[ut->chunkOffset-1];
                எனில் (U16_IS_LEAD(lead)) {
                    ut->chunkOffset--;
                }
            }
        }
    }
}



U_CAPI int64_t U_EXPORT2
utext_getPreviousNativeIndex(UText *ut) {
    //
    //  Fast-path the common case.
    //     Common means current position is not at the beginning of a chunk
    //     and the preceding character is not supplementary.
    //
    int32_t i = ut->chunkOffset - 1;
    int64_t result;
    எனில் (i >= 0) {
        char16_t c = ut->chunkContents[i];
        எனில் (U16_IS_TRAIL(c) == பொய்) {
            எனில் (i <= ut->nativeIndexingLimit) {
                result = ut->chunkNativeStart + i;
            } இல்லையெனில் {
                ut->chunkOffset = i;
                result = ut->pFuncs->mapOffsetToNative(ut);
                ut->chunkOffset++;
            }
            திரும்பு result;
        }
    }

    // If at the start of text, simply return 0.
    எனில் (ut->chunkOffset==0 && ut->chunkNativeStart==0) {
        திரும்பு 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    result = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    திரும்பு result;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    எனில் (ut->chunkOffset==ut->chunkLength) {
        // Current position is just off the end of the chunk.
        எனில் (ut->pFuncs->access(ut, ut->chunkNativeLimit, உண்மை) == பொய்) {
            // Off the end of the text.
            திரும்பு U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunkOffset];
    எனில் (U16_IS_LEAD(c) == பொய்) {
        // Normal, non-supplementary case.
        திரும்பு c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    எனில் ((ut->chunkOffset+1) < ut->chunkLength) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunkOffset+1];
    } இல்லையெனில் {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunkNativeLimit;
        எனில் (ut->pFuncs->access(ut, nativePosition, உண்மை)) {
            trail = ut->chunkContents[ut->chunkOffset];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, பொய்);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunkOffset = ut->chunkLength - 1;
        எனில்(!r) {
            திரும்பு U_SENTINEL;
        }
    }

    எனில் (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    திரும்பு supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t nativeIndex) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    எனில் (nativeIndex>=ut->chunkNativeStart && nativeIndex < ut->chunkNativeStart + ut->nativeIndexingLimit) {
        ut->chunkOffset = (int32_t)(nativeIndex - ut->chunkNativeStart);
        c = ut->chunkContents[ut->chunkOffset];
        எனில் (U16_IS_SURROGATE(c) == பொய்) {
            திரும்பு c;
        }
    }


    utext_setNativeIndex(ut, nativeIndex);
    எனில் (nativeIndex>=ut->chunkNativeStart && ut->chunkOffset<ut->chunkLength) {
        c = ut->chunkContents[ut->chunkOffset];
        எனில் (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    திரும்பு c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    எனில் (ut->chunkOffset >= ut->chunkLength) {
        எனில் (ut->pFuncs->access(ut, ut->chunkNativeLimit, உண்மை) == பொய்) {
            திரும்பு U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunkOffset++];
    எனில் (U16_IS_LEAD(c) == பொய்) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        திரும்பு c;
    }

    எனில் (ut->chunkOffset >= ut->chunkLength) {
        எனில் (ut->pFuncs->access(ut, ut->chunkNativeLimit, உண்மை) == பொய்) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            திரும்பு c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunkOffset];
    எனில் (U16_IS_TRAIL(trail) == பொய்) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        திரும்பு c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunkOffset++;   // move iteration position over the trail surrogate.
    திரும்பு supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    எனில் (ut->chunkOffset <= 0) {
        எனில் (ut->pFuncs->access(ut, ut->chunkNativeStart, பொய்) == பொய்) {
            திரும்பு U_SENTINEL;
        }
    }
    ut->chunkOffset--;
    c = ut->chunkContents[ut->chunkOffset];
    எனில் (U16_IS_TRAIL(c) == பொய்) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        திரும்பு c;
    }

    எனில் (ut->chunkOffset <= 0) {
        எனில் (ut->pFuncs->access(ut, ut->chunkNativeStart, பொய்) == பொய்) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            திரும்பு c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunkOffset-1];
    எனில் (U16_IS_LEAD(lead) == பொய்) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        திரும்பு c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunkOffset--;   // move iteration position over the lead surrogate.
    திரும்பு supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t index) {
    UChar32       c      = U_SENTINEL;

    எனில்(index<ut->chunkNativeStart || index>=ut->chunkNativeLimit) {
        // Desired position is outside of the current chunk.
        எனில்(!ut->pFuncs->access(ut, index, உண்மை)) {
            // no chunk available here
            திரும்பு U_SENTINEL;
        }
    } இல்லையெனில் எனில் (index - ut->chunkNativeStart  <= (int64_t)ut->nativeIndexingLimit) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunkOffset = (int32_t)(index - ut->chunkNativeStart);
    } இல்லையெனில் {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunkOffset = ut->pFuncs->mapNativeIndexToUTF16(ut, index);
    }

    c = ut->chunkContents[ut->chunkOffset++];
    எனில் (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_setNativeIndex(ut, index);
        c = utext_next32(ut);
    }
    திரும்பு c;
}


U_CAPI UChar32 U_EXPORT2
utext_previous32From(UText *ut, int64_t index) {
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
    எனில்(index<=ut->chunkNativeStart || index>ut->chunkNativeLimit) {
        // Requested native index is outside of the current chunk.
        எனில்(!ut->pFuncs->access(ut, index, பொய்)) {
            // no chunk available here
            திரும்பு U_SENTINEL;
        }
    } இல்லையெனில் எனில்(index - ut->chunkNativeStart <= (int64_t)ut->nativeIndexingLimit) {
        // Direct UTF-16 indexing.
        ut->chunkOffset = (int32_t)(index - ut->chunkNativeStart);
    } இல்லையெனில் {
        ut->chunkOffset=ut->pFuncs->mapNativeIndexToUTF16(ut, index);
        எனில் (ut->chunkOffset==0 && !ut->pFuncs->access(ut, index, பொய்)) {
            // no chunk available here
            திரும்பு U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunkOffset--;
    cPrev = ut->chunkContents[ut->chunkOffset];

    எனில் (U16_IS_SURROGATE(cPrev)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_setNativeIndex(ut, index);
        cPrev = utext_previous32(ut);
    }
    திரும்பு cPrev;
}


U_CAPI int32_t U_EXPORT2
utext_extract(UText *ut,
             int64_t start, int64_t limit,
             char16_t *dest, int32_t destCapacity,
             UErrorCode *status) {
                 திரும்பு ut->pFuncs->extract(ut, start, limit, dest, destCapacity, status);
             }



U_CAPI UBool U_EXPORT2
utext_equals(மாறிலி UText *a, மாறிலி UText *b) {
    எனில் (a==nullptr || b==nullptr ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            திரும்பு பொய்;
    }

    எனில் (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        திரும்பு பொய்;
    }

    எனில் (a->context != b->context) {
        // Different sources (different strings)
        திரும்பு பொய்;
    }
    எனில் (utext_getNativeIndex(a) != utext_getNativeIndex(b)) {
        // Different current position in the string.
        திரும்பு பொய்;
    }

    திரும்பு உண்மை;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(மாறிலி UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    திரும்பு b;
}


U_CAPI வெறுமை U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_hasMetaData(மாறிலி UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_DATA)) != 0;
    திரும்பு b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t nativeStart, int64_t nativeLimit,
             மாறிலி char16_t *replacementText, int32_t replacementLength,
             UErrorCode *status)
{
    எனில் (U_FAILURE(*status)) {
        திரும்பு 0;
    }
    எனில் ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *status = U_NO_WRITE_PERMISSION;
        திரும்பு 0;
    }
    int32_t i = ut->pFuncs->replace(ut, nativeStart, nativeLimit, replacementText, replacementLength, status);
    திரும்பு i;
}

U_CAPI வெறுமை U_EXPORT2
utext_copy(UText *ut,
          int64_t nativeStart, int64_t nativeLimit,
          int64_t destIndex,
          UBool move,
          UErrorCode *status)
{
    எனில் (U_FAILURE(*status)) {
        திரும்பு;
    }
    எனில் ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *status = U_NO_WRITE_PERMISSION;
        திரும்பு;
    }
    ut->pFuncs->copy(ut, nativeStart, nativeLimit, destIndex, move, status);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            மாறிலி UText* src,
            UBool deep,
            UBool readOnly,
            UErrorCode* status) {
    எனில் (U_FAILURE(*status)) {
        திரும்பு dest;
    }
    UText *result = src->pFuncs->clone(dest, src, deep, status);
    எனில் (U_FAILURE(*status)) {
        திரும்பு result;
    }
    எனில் (result == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு result;
    }
    எனில் (readOnly) {
        utext_freeze(result);
    }
    திரும்பு result;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
எண்ணிக்கை {
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

கட்டமைப்பு ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

நிலையான மாறிலி UText emptyText = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_setup(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UErrorCode* status) {
    எனில் (U_FAILURE(*status)) {
        திரும்பு ut;
    }

    எனில் (ut == nullptr) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = அளவு(UText);
        எனில் (extraSpace > 0) {
            spaceRequired = அளவு(ExtendedUText) + extraSpace - அளவு(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        எனில் (ut == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு nullptr;
        } இல்லையெனில் {
            *ut = emptyText;
            ut->flags |= UTEXT_HEAP_ALLOCATED;
            எனில் (spaceRequired>0) {
                ut->extraSize = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } இல்லையெனில் {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        எனில் (ut->magic != UTEXT_MAGIC) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        எனில் ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != nullptr)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        எனில் (extraSpace > ut->extraSize) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            எனில் (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extraSize = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            எனில் (ut->pExtra == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } இல்லையெனில் {
                ut->extraSize = extraSpace;
                ut->flags |= UTEXT_EXTRA_HEAP_ALLOCATED;
            }
        }
    }
    எனில் (U_SUCCESS(*status)) {
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
        ut->chunkLength         = 0;
        ut->chunkNativeStart    = 0;
        ut->chunkNativeLimit    = 0;
        ut->nativeIndexingLimit = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = nullptr;
        எனில் (ut->pExtra!=nullptr && ut->extraSize>0)
            uprv_memset(ut->pExtra, 0, ut->extraSize);

    }
    திரும்பு ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    எனில் (ut==nullptr ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        திரும்பு ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    எனில் (ut->pFuncs->close != nullptr) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    எனில் (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = nullptr;
        ut->flags &= ~UTEXT_EXTRA_HEAP_ALLOCATED;
        ut->extraSize = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = nullptr;

    எனில் (ut->flags & UTEXT_HEAP_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = nullptr;
    }
    திரும்பு ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
நிலையான வெறுமை
invalidateChunk(UText *ut) {
    ut->chunkLength = 0;
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
நிலையான int32_t
pinIndex(int64_t &index, int64_t limit) {
    எனில் (index<0) {
        index = 0;
    } இல்லையெனில் எனில் (index > limit) {
        index = limit;
    }
    திரும்பு static_cast<int32_t>(index);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
நிலையான வெறுமை adjustPointer(UText *dest, மாறிலி வெறுமை **destPtr, மாறிலி UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    எழுத்து  *dptr = (எழுத்து *)*destPtr;
    எழுத்து  *dUText = (எழுத்து *)dest;
    எழுத்து  *sUText = (எழுத்து *)src;

    எனில் (dptr >= (எழுத்து *)src->pExtra && dptr < ((எழுத்து*)src->pExtra)+src->extraSize) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((எழுத்து *)dest->pExtra) + (dptr - (எழுத்து *)src->pExtra);
    } இல்லையெனில் எனில் (dptr>=sUText && dptr < sUText+src->sizeOfStruct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
நிலையான UText * U_CALLCONV
shallowTextClone(UText * dest, மாறிலி UText * src, UErrorCode * status) {
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    int32_t  srcExtraSize = src->extraSize;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_setup(dest, srcExtraSize, status);
    எனில் (U_FAILURE(*status)) {
        திரும்பு dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    வெறுமை *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    முழு sizeToCopy = src->sizeOfStruct;
    எனில் (sizeToCopy > dest->sizeOfStruct) {
        sizeToCopy = dest->sizeOfStruct;
    }
    uprv_memcpy(dest, src, sizeToCopy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    எனில் (srcExtraSize > 0) {
        uprv_memcpy(dest->pExtra, src->pExtra, srcExtraSize);
    }

    //
    // Relocate any pointers in the target that refer to the UText itself
    //   to point to the cloned copy rather than the original source.
    //
    adjustPointer(dest, &dest->context, src);
    adjustPointer(dest, &dest->p, src);
    adjustPointer(dest, &dest->q, src);
    adjustPointer(dest, &dest->r, src);
    adjustPointer(dest, (மாறிலி வெறுமை **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

    திரும்பு dest;
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
எண்ணிக்கை { UTF8_TEXT_CHUNK_SIZE=32 };

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

கட்டமைப்பு UTF8Buf {
    int32_t   bufNativeStart;                        // Native index of first char in char16_t buf
    int32_t   bufNativeLimit;                        // Native index following last char in buf.
    int32_t   bufStartIdx;                           // First filled position in buf.
    int32_t   bufLimitIdx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   toUCharsMapStart;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_TEXT_CHUNK_SIZE+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   mapToNative[UTF8_TEXT_CHUNK_SIZE+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   mapToUChars[UTF8_TEXT_CHUNK_SIZE*3+6]; // Map native offset from bufNativeStart to
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
நிலையான int64_t U_CALLCONV
utf8TextLength(UText *ut) {
    எனில் (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        மாறிலி எழுத்து *r = (மாறிலி எழுத்து *)ut->context + ut->c;
        போது (*r != 0) {
            r++;
        }
        எனில் ((r - (மாறிலி எழுத்து *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (மாறிலி எழுத்து *)ut->context);
        } இல்லையெனில் {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
    }
    திரும்பு ut->b;
}






நிலையான UBool U_CALLCONV
utf8TextAccess(UText *ut, int64_t index, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    மாறிலி uint8_t *s8=(மாறிலி uint8_t *)ut->context;
    UTF8Buf *u8b = nullptr;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)index;     // Requested index, trimmed to 32 bits.
    int32_t  mapIndex = 0;
    எனில் (index<0) {
        ix=0;
    } இல்லையெனில் எனில் (index > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    எனில் (ix>length) {
        எனில் (length>=0) {
            ix=length;
        } இல்லையெனில் எனில் (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            போது (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            எனில் (s8[ut->c] == 0) {
                // We just found the actual length of the string.
                //  Trim the requested index back to that.
                ix     = ut->c;
                ut->b  = ut->c;
                length = ut->c;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    எனில் (forward) {
        எனில் (ix==ut->chunkNativeLimit) {
            // Check for normal sequential iteration cases first.
            எனில் (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunkOffset = ut->chunkLength;
                திரும்பு பொய்;
            } இல்லையெனில் {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                எனில் (ix>=altB->bufNativeStart && ix<altB->bufNativeLimit) {
                    செல் swapBuffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            எனில் (ix>=u8b->bufNativeStart && ix<u8b->bufNativeLimit) {
                // Requested index is in the other buffer.
                செல் swapBuffers;
            }
            எனில் (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                எனில் (ix == ut->chunkNativeLimit) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunkOffset = ut->chunkLength;
                    திரும்பு பொய்;
                }
                எனில் (ix == u8b->bufNativeLimit) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    செல் swapBuffersAndFail;
                }

                // Neither existing buffer extends to the end of the string.
                செல் makeStubBuffer;
            }

            எனில் (ix<ut->chunkNativeStart || ix>=ut->chunkNativeLimit) {
                // Requested index is in neither buffer.
                செல் fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            mapIndex = ix - u8b->toUCharsMapStart;
            U_ASSERT(mapIndex < (int32_t)அளவு(UTF8Buf::mapToUChars));
            ut->chunkOffset = u8b->mapToUChars[mapIndex] - u8b->bufStartIdx;
            திரும்பு உண்மை;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    எனில் (ix==ut->chunkNativeStart) {
        // Check for normal sequential iteration cases first.
        எனில் (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunkOffset = 0;
            திரும்பு பொய்;
        } இல்லையெனில் {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            எனில் (ix>altB->bufNativeStart && ix<=altB->bufNativeLimit) {
                செல் swapBuffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    எனில் (ix>u8b->bufNativeStart && ix<=u8b->bufNativeLimit) {
        // Requested index is in the other buffer.
        செல் swapBuffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    எனில் (ix==0) {
        எனில் (u8b->bufNativeStart==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            செல் swapBuffersAndFail;
        } இல்லையெனில் {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            செல் makeStubBuffer;
        }
    }

    எனில் (ix<=ut->chunkNativeStart || ix>ut->chunkNativeLimit) {
        // Requested index is in neither buffer.
        செல் fillReverse;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    mapIndex = ix - u8b->toUCharsMapStart;
    ut->chunkOffset = u8b->mapToUChars[mapIndex] - u8b->bufStartIdx;
    எனில் (ut->chunkOffset==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        திரும்பு பொய்;
    } இல்லையெனில் {
        திரும்பு உண்மை;
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
        ut->chunkLength         = u8b->bufLimitIdx - u8b->bufStartIdx;
        ut->chunkNativeStart    = u8b->bufNativeStart;
        ut->chunkNativeLimit    = u8b->bufNativeLimit;
        ut->nativeIndexingLimit = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->bufNativeStart);
        U_ASSERT(ix<=u8b->bufNativeLimit);
        mapIndex = ix - u8b->toUCharsMapStart;
        U_ASSERT(mapIndex>=0);
        U_ASSERT(mapIndex<(int32_t)அளவு(u8b->mapToUChars));
        ut->chunkOffset = u8b->mapToUChars[mapIndex] - u8b->bufStartIdx;

        திரும்பு உண்மை;
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
    ut->chunkLength         = u8b->bufLimitIdx - u8b->bufStartIdx;
    ut->chunkNativeStart    = u8b->bufNativeStart;
    ut->chunkNativeLimit    = u8b->bufNativeLimit;
    ut->nativeIndexingLimit = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    எனில் (ix==u8b->bufNativeLimit) {
        ut->chunkOffset = ut->chunkLength;
    } இல்லையெனில்  {
        ut->chunkOffset = 0;
        U_ASSERT(ix == u8b->bufNativeStart);
    }
    திரும்பு பொய்;

makeStubBuffer:
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
    செல் swapBuffersAndFail;



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
        UBool   nulTerminated = பொய்;
        எனில் (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = உண்மை;
        }

        char16_t   *buf = u8b_swap->buf;
        uint8_t *mapToNative  = u8b_swap->mapToNative;
        uint8_t *mapToUChars  = u8b_swap->mapToUChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    seenNonAscii = பொய்;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        போது (destIx<UTF8_TEXT_CHUNK_SIZE) {
            c = s8[srcIx];
            எனில் (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                mapToNative[destIx]    = (uint8_t)(srcIx - ix);
                mapToUChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } இல்லையெனில் {
                // General case, handle everything.
                எனில் (seenNonAscii == பொய்) {
                    seenNonAscii = உண்மை;
                    u8b_swap->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_NEXT_OR_FFFD(s8, srcIx, strLen, c);
                எனில் (c==0 && nulTerminated) {
                    srcIx--;
                    நிறுத்து;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                செய் {
                    mapToNative[dIx++] = (uint8_t)(cIx - ix);
                } போது (dIx < destIx);

                செய் {
                    mapToUChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } போது (cIx < srcIx);
            }
            எனில் (srcIx>=strLen) {
                நிறுத்து;
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
        எனில் (seenNonAscii == பொய்) {
            u8b_swap->bufNILimit     = destIx;
        }
        u8b_swap->toUCharsMapStart   = u8b_swap->bufNativeStart;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunkOffset         = 0;
        ut->chunkLength         = u8b_swap->bufLimitIdx;
        ut->chunkNativeStart    = u8b_swap->bufNativeStart;
        ut->chunkNativeLimit    = u8b_swap->bufNativeLimit;
        ut->nativeIndexingLimit = u8b_swap->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        எனில் (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            எனில் (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
            }
        }
        திரும்பு உண்மை;
    }


fillReverse:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        எனில் (ix != ut->b) {
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
        int32_t  toUCharsMapStart = ix - அளவு(UTF8Buf::mapToUChars) + 1;
        // Note that toUCharsMapStart can be negative. Happens when the remaining
        // text from current position to the beginning is less than the buffer size.
        // + 1 because mapToUChars must have a slot at the end for the bufNativeLimit entry.
        int32_t  destIx = UTF8_TEXT_CHUNK_SIZE+2;   // Start in the overflow region
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
        போது (destIx>2 && (srcIx - toUCharsMapStart > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            எனில் (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(toUCharsMapStart <= srcIx);
                mapToUChars[srcIx - toUCharsMapStart] = (uint8_t)destIx;
                mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
            } இல்லையெனில் {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_prevCharSafeBody(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                எனில் (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                } இல்லையெனில் {
                    buf[destIx]         = U16_TRAIL(c);
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                    buf[--destIx]       = U16_LEAD(c);
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                }

                // Fill in the map from native indexes to UChars buf index.
                செய் {
                    mapToUChars[sIx-- - toUCharsMapStart] = (uint8_t)destIx;
                } போது (sIx >= srcIx);
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
        u8b_swap->bufLimitIdx        = UTF8_TEXT_CHUNK_SIZE+2;
        u8b_swap->bufNILimit         = bufNILimit - u8b_swap->bufStartIdx;
        u8b_swap->toUCharsMapStart   = toUCharsMapStart;

        ut->chunkContents       = &buf[u8b_swap->bufStartIdx];
        ut->chunkLength         = u8b_swap->bufLimitIdx - u8b_swap->bufStartIdx;
        ut->chunkOffset         = ut->chunkLength;
        ut->chunkNativeStart    = u8b_swap->bufNativeStart;
        ut->chunkNativeLimit    = u8b_swap->bufNativeLimit;
        ut->nativeIndexingLimit = u8b_swap->bufNILimit;
        திரும்பு உண்மை;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
நிலையான char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t destCapacity,
              int32_t *pDestLength,
              மாறிலி எழுத்து* src,
              int32_t srcLength,        // required.  NUL terminated not supported.
              UErrorCode *pErrorCode
              )
{

    char16_t *pDest = dest;
    char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
    UChar32 ch=0;
    int32_t index = 0;
    int32_t reqLength = 0;
    uint8_t* pSrc = (uint8_t*) src;


    போது((index < srcLength)&&(pDest<pDestLimit)){
        ch = pSrc[index++];
        எனில்(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }இல்லையெனில்{
            ch=utf8_nextCharSafeBody(pSrc, &index, srcLength, ch, -3);
            எனில்(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }இல்லையெனில்{
                *(pDest++)=U16_LEAD(ch);
                எனில்(pDest<pDestLimit){
                    *(pDest++)=U16_TRAIL(ch);
                }இல்லையெனில்{
                    reqLength++;
                    நிறுத்து;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    போது(index < srcLength){
        ch = pSrc[index++];
        எனில்(ch <= 0x7f){
            reqLength++;
        }இல்லையெனில்{
            ch=utf8_nextCharSafeBody(pSrc, &index, srcLength, ch, -3);
            reqLength+=U16_LENGTH(ch);
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    எனில்(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    திரும்பு dest;
}



நிலையான int32_t U_CALLCONV
utf8TextExtract(UText *ut,
                int64_t start, int64_t limit,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pinIndex(start, length);
    int32_t  limit32 = pinIndex(limit, length);

    எனில்(start32>limit32) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    மாறிலி uint8_t *buf = (மாறிலி uint8_t *)ut->context;
    முழு i;
    எனில் (start32 < ut->chunkNativeLimit) {
        க்கு (i=0; i<3; i++) {
            எனில் (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                நிறுத்து;
            }
            start32--;
        }
    }

    எனில் (limit32 < ut->chunkNativeLimit) {
        க்கு (i=0; i<3; i++) {
            எனில் (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                நிறுத்து;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t destLength=0;
    utext_strFromUTF8(dest, destCapacity, &destLength,
                    (மாறிலி எழுத்து *)ut->context+start32, limit32-start32,
                    pErrorCode);
    utf8TextAccess(ut, limit32, உண்மை);
    திரும்பு destLength;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
நிலையான int64_t U_CALLCONV
utf8TextMapOffsetToNative(மாறிலி UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunkOffset>ut->nativeIndexingLimit && ut->chunkOffset<=ut->chunkLength);
    int32_t nativeOffset = u8b->mapToNative[ut->chunkOffset + u8b->bufStartIdx] + u8b->toUCharsMapStart;
    U_ASSERT(nativeOffset >= ut->chunkNativeStart && nativeOffset <= ut->chunkNativeLimit);
    திரும்பு nativeOffset;
}

//
// Map a native index to the corresponding chunk offset
//
நிலையான int32_t U_CALLCONV
utf8TextMapIndexToUTF16(மாறிலி UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t index = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(index>=ut->chunkNativeStart+ut->nativeIndexingLimit);
    U_ASSERT(index<=ut->chunkNativeLimit);
    int32_t mapIndex = index - u8b->toUCharsMapStart;
    U_ASSERT(mapIndex < (int32_t)அளவு(UTF8Buf::mapToUChars));
    int32_t offset = u8b->mapToUChars[mapIndex] - u8b->bufStartIdx;
    U_ASSERT(offset>=0 && offset<=ut->chunkLength);
    திரும்பு offset;
}

நிலையான UText * U_CALLCONV
utf8TextClone(UText *dest, மாறிலி UText *src, UBool deep, UErrorCode *status)
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
    எனில் (deep && U_SUCCESS(*status)) {
        int32_t  len = (int32_t)utext_nativeLength((UText *)src);
        எழுத்து *copyStr = (எழுத்து *)uprv_malloc(len+1);
        எனில் (copyStr == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    திரும்பு dest;
}


நிலையான வெறுமை U_CALLCONV
utf8TextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    எனில் (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        எழுத்து *s = (எழுத்து *)ut->context;
        uprv_free(s);
        ut->context = nullptr;
    }
}

U_CDECL_END


நிலையான மாறிலி கட்டமைப்பு UTextFuncs utf8Funcs =
{
    அளவு(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8TextClone,
    utf8TextLength,
    utf8TextAccess,
    utf8TextExtract,
    nullptr,                /* replace*/
    nullptr,                /* copy   */
    utf8TextMapOffsetToNative,
    utf8TextMapIndexToUTF16,
    utf8TextClose,
    nullptr,                // spare 1
    nullptr,                // spare 2
    nullptr                 // spare 3
};


நிலையான மாறிலி எழுத்து gEmptyString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               மாறிலி எழுத்து* s U_LIFETIME_BOUND,
               int64_t length,
               UErrorCode* status) {
    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில்(s==nullptr && length==0) {
        s = gEmptyString;
    }

    எனில்(s==nullptr || length<-1 || length>INT32_MAX) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    ut = utext_setup(ut, அளவு(UTF8Buf) * 2, status);
    எனில் (U_FAILURE(*status)) {
        திரும்பு ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    எனில் (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (எழுத்து *)ut->pExtra + அளவு(UTF8Buf);
    திரும்பு ut;

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
எண்ணிக்கை { REP_TEXT_CHUNK_SIZE=10 };

கட்டமைப்பு ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_TEXT_CHUNK_SIZE+1];
};


U_CDECL_BEGIN

நிலையான UText * U_CALLCONV
repTextClone(UText *dest, மாறிலி UText *src, UBool deep, UErrorCode *status) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    எனில் (deep && U_SUCCESS(*status)) {
        மாறிலி Replaceable *replSrc = (மாறிலி Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    திரும்பு dest;
}


நிலையான வெறுமை U_CALLCONV
repTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    எனில் (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        Replaceable *rep = (Replaceable *)ut->context;
        delete rep;
        ut->context = nullptr;
    }
}


நிலையான int64_t U_CALLCONV
repTextLength(UText *ut) {
    மாறிலி Replaceable *replSrc = (மாறிலி Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    திரும்பு len;
}


நிலையான UBool U_CALLCONV
repTextAccess(UText *ut, int64_t index, UBool forward) {
    மாறிலி Replaceable *rep=(மாறிலி Replaceable *)ut->context;
    int32_t length=rep->length();   // Full length of the input text (bigger than a chunk)

    // clip the requested index to the limits of the text.
    int32_t index32 = pinIndex(index, length);
    U_ASSERT(index<=INT32_MAX);


    /*
     * Compute start/limit boundaries around index, for a segment of text
     * to be extracted.
     * To allow for the possibility that our user gave an index to the trailing
     * half of a surrogate pair, we must request one extra preceding char16_t when
     * going in the forward direction.  This will ensure that the buffer has the
     * entire code point at the specified index.
     */
    எனில்(forward) {

        எனில் (index32>=ut->chunkNativeStart && index32<ut->chunkNativeLimit) {
            // Buffer already contains the requested position.
            ut->chunkOffset = (int32_t)(index - ut->chunkNativeStart);
            திரும்பு உண்மை;
        }
        எனில் (index32>=length && ut->chunkNativeLimit==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunkOffset = length - (int32_t)ut->chunkNativeStart;
            திரும்பு பொய்;
        }

        ut->chunkNativeLimit = index + REP_TEXT_CHUNK_SIZE - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        எனில்(ut->chunkNativeLimit > length) {
            ut->chunkNativeLimit = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunkNativeStart = ut->chunkNativeLimit - REP_TEXT_CHUNK_SIZE;
        எனில்(ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }
    } இல்லையெனில் {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        எனில் (index32>ut->chunkNativeStart && index32<=ut->chunkNativeLimit) {
            // Requested position already in buffer.
            ut->chunkOffset = index32 - (int32_t)ut->chunkNativeStart;
            திரும்பு உண்மை;
        }
        எனில் (index32==0 && ut->chunkNativeStart==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunkOffset = 0;
            திரும்பு பொய்;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunkNativeStart = index32 + 1 - REP_TEXT_CHUNK_SIZE;
        எனில் (ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }

        ut->chunkNativeLimit = index32 + 1;
        எனில் (ut->chunkNativeLimit > length) {
            ut->chunkNativeLimit = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString buffer(ex->s, 0 /*buffer length*/, REP_TEXT_CHUNK_SIZE /*buffer capacity*/);
    rep->extractBetween((int32_t)ut->chunkNativeStart, (int32_t)ut->chunkNativeLimit, buffer);

    ut->chunkContents  = ex->s;
    ut->chunkLength    = (int32_t)(ut->chunkNativeLimit - ut->chunkNativeStart);
    ut->chunkOffset    = (int32_t)(index32 - ut->chunkNativeStart);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    எனில் (ut->chunkNativeLimit < length &&
        U16_IS_LEAD(ex->s[ut->chunkLength-1])) {
            ut->chunkLength--;
            ut->chunkNativeLimit--;
            எனில் (ut->chunkOffset > ut->chunkLength) {
                ut->chunkOffset = ut->chunkLength;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    எனில்(ut->chunkNativeStart>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunkNativeStart);
        --(ut->chunkLength);
        --(ut->chunkOffset);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_SET_CP_START(ut->chunkContents, 0, ut->chunkOffset);

    // Use fast indexing for get/setNativeIndex()
    ut->nativeIndexingLimit = ut->chunkLength;

    திரும்பு உண்மை;
}



நிலையான int32_t U_CALLCONV
repTextExtract(UText *ut,
               int64_t start, int64_t limit,
               char16_t *dest, int32_t destCapacity,
               UErrorCode *status) {
    மாறிலி Replaceable *rep=(மாறிலி Replaceable *)ut->context;
    int32_t  length=rep->length();

    எனில்(U_FAILURE(*status)) {
        திரும்பு 0;
    }
    எனில்(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
    }
    எனில்(start>limit) {
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    int32_t  start32 = pinIndex(start, length);
    int32_t  limit32 = pinIndex(limit, length);

    // adjust start, limit if they point to trail half of surrogates
    எனில் (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    எனில் (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    எனில்(length>destCapacity) {
        limit32 = start32 + destCapacity;
    }
    UnicodeString buffer(dest, 0, destCapacity); // writable alias
    rep->extractBetween(start32, limit32, buffer);
    repTextAccess(ut, limit32, உண்மை);

    திரும்பு u_terminateUChars(dest, destCapacity, length, status);
}

நிலையான int32_t U_CALLCONV
repTextReplace(UText *ut,
               int64_t start, int64_t limit,
               மாறிலி char16_t *src, int32_t length,
               UErrorCode *status) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t oldLength;

    எனில்(U_FAILURE(*status)) {
        திரும்பு 0;
    }
    எனில்(src==nullptr && length!=0) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    oldLength=rep->length(); // will subtract from new length
    எனில்(start>limit ) {
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    int32_t start32 = pinIndex(start, oldLength);
    int32_t limit32 = pinIndex(limit, oldLength);

    // Snap start & limit to code point boundaries.
    எனில் (start32<oldLength && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    எனில் (limit32<oldLength && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t newLength = rep->length();
    int32_t lengthDelta = newLength - oldLength;

    // Is the UText chunk buffer OK?
    எனில் (ut->chunkNativeLimit > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t newIndexPos = limit32 + lengthDelta;
    repTextAccess(ut, newIndexPos, உண்மை);

    திரும்பு lengthDelta;
}


நிலையான வெறுமை U_CALLCONV
repTextCopy(UText *ut,
                int64_t start, int64_t limit,
                int64_t destIndex,
                UBool move,
                UErrorCode *status)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    எனில்(U_FAILURE(*status)) {
        திரும்பு;
    }
    எனில் (start>limit || (start<destIndex && destIndex<limit))
    {
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு;
    }

    int32_t start32     = pinIndex(start, length);
    int32_t limit32     = pinIndex(limit, length);
    int32_t destIndex32 = pinIndex(destIndex, length);

    // TODO:  snap input parameters to code point boundaries.

    எனில்(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t segLength=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        எனில்(destIndex32<start32) {
            start32+=segLength;
            limit32+=segLength;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } இல்லையெனில் {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t firstAffectedIndex = destIndex32;
    எனில் (move && start32<firstAffectedIndex) {
        firstAffectedIndex = start32;
    }
    எனில் (firstAffectedIndex < ut->chunkNativeLimit) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  nativeIterIndex = destIndex32 + limit32 - start32;
    எனில் (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        nativeIterIndex = destIndex32;
    }

    // Set position, reload chunk if needed.
    repTextAccess(ut, nativeIterIndex, உண்மை);
}

நிலையான மாறிலி கட்டமைப்பு UTextFuncs repFuncs =
{
    அளவு(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    repTextClone,
    repTextLength,
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
    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில்(rep==nullptr) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    ut = utext_setup(ut, அளவு(ReplExtra), status);
    எனில்(U_FAILURE(*status)) {
        திரும்பு ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    எனில்(rep->hasMetaData()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_DATA);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    திரும்பு ut;
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


நிலையான UText * U_CALLCONV
unistrTextClone(UText *dest, மாறிலி UText *src, UBool deep, UErrorCode *status) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    எனில் (deep && U_SUCCESS(*status)) {
        மாறிலி UnicodeString *srcString = (மாறிலி UnicodeString *)src->context;
        dest->context = புதிய UnicodeString(*srcString);
        dest->chunkContents = ((மாறிலி UnicodeString *)dest->context)->getBuffer();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    திரும்பு dest;
}

நிலையான வெறுமை U_CALLCONV
unistrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    எனில் (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        delete str;
        ut->context = nullptr;
    }
}


நிலையான int64_t U_CALLCONV
unistrTextLength(UText *t) {
    திரும்பு ((மாறிலி UnicodeString *)t->context)->length();
}


நிலையான UBool U_CALLCONV
unistrTextAccess(UText *ut, int64_t index, UBool  forward) {
    int32_t length  = ut->chunkLength;
    ut->chunkOffset = pinIndex(index, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && index<length) || (!forward && index>0);
    திரும்பு retVal;
}



நிலையான int32_t U_CALLCONV
unistrTextExtract(UText *t,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *pErrorCode) {
    மாறிலி UnicodeString *us=(மாறிலி UnicodeString *)t->context;
    int32_t length=us->length();

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
    எனில்(start<0 || start>limit) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    int32_t start32 = start<length ? us->getChar32Start((int32_t)start) : length;
    int32_t limit32 = limit<length ? us->getChar32Start((int32_t)limit) : length;

    length=limit32-start32;
    எனில் (destCapacity>0 && dest!=nullptr) {
        int32_t trimmedLength = length;
        எனில்(trimmedLength>destCapacity) {
            trimmedLength=destCapacity;
        }
        us->extract(start32, trimmedLength, dest);
        t->chunkOffset = start32+trimmedLength;
    } இல்லையெனில் {
        t->chunkOffset = start32;
    }
    u_terminateUChars(dest, destCapacity, length, pErrorCode);
    திரும்பு length;
}

நிலையான int32_t U_CALLCONV
unistrTextReplace(UText *ut,
                  int64_t start, int64_t limit,
                  மாறிலி char16_t *src, int32_t length,
                  UErrorCode *pErrorCode) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t oldLength;

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(src==nullptr && length!=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
    எனில்(start>limit) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }
    oldLength=us->length();
    int32_t start32 = pinIndex(start, oldLength);
    int32_t limit32 = pinIndex(limit, oldLength);
    எனில் (start32 < oldLength) {
        start32 = us->getChar32Start(start32);
    }
    எனில் (limit32 < oldLength) {
        limit32 = us->getChar32Start(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t newLength = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->getBuffer();
    ut->chunkLength      = newLength;
    ut->chunkNativeLimit = newLength;
    ut->nativeIndexingLimit = newLength;

    // Set iteration position to the point just following the newly inserted text.
    int32_t lengthDelta = newLength - oldLength;
    ut->chunkOffset = limit32 + lengthDelta;

    திரும்பு lengthDelta;
}

நிலையான வெறுமை U_CALLCONV
unistrTextCopy(UText *ut,
               int64_t start, int64_t limit,
               int64_t destIndex,
               UBool move,
               UErrorCode *pErrorCode) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    int32_t start32 = pinIndex(start, length);
    int32_t limit32 = pinIndex(limit, length);
    int32_t destIndex32 = pinIndex(destIndex, length);

    எனில்( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு;
    }

    எனில்(move) {
        // move: copy to destIndex, then remove original
        int32_t segLength=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        எனில்(destIndex32<start32) {
            start32+=segLength;
        }
        us->remove(start32, segLength);
    } இல்லையெனில் {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->getBuffer();
    எனில் (move==பொய்) {
        // copy operation, string length grows
        ut->chunkLength += limit32-start32;
        ut->chunkNativeLimit = ut->chunkLength;
        ut->nativeIndexingLimit = ut->chunkLength;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunkOffset = destIndex32+limit32-start32;
    எனில் (move && destIndex32>start32) {
        ut->chunkOffset = destIndex32;
    }

}

நிலையான மாறிலி கட்டமைப்பு UTextFuncs unistrFuncs =
{
    அளவு(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistrTextClone,
    unistrTextLength,
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
    எனில் (U_SUCCESS(*status)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    திரும்பு ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             மாறிலி UnicodeString* s U_LIFETIME_BOUND,
                             UErrorCode* status) {
    எனில் (U_SUCCESS(*status) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, nullptr, 0, status);
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு ut;
    }
    ut = utext_setup(ut, 0, status);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    எனில் (U_SUCCESS(*status)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->getBuffer();
        ut->chunkLength         = s->length();
        ut->chunkNativeStart    = 0;
        ut->chunkNativeLimit    = ut->chunkLength;
        ut->nativeIndexingLimit = ut->chunkLength;
    }
    திரும்பு ut;
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


நிலையான UText * U_CALLCONV
ucstrTextClone(UText *dest, மாறிலி UText * src, UBool deep, UErrorCode * status) {
    // First do a generic shallow clone.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    எனில் (deep && U_SUCCESS(*status)) {
        U_ASSERT(utext_nativeLength(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_nativeLength(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        மாறிலி char16_t *srcStr = (மாறிலி char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * அளவு(char16_t));
        எனில் (copyStr == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            int64_t i;
            க்கு (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    திரும்பு dest;
}


நிலையான வெறுமை U_CALLCONV
ucstrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    எனில் (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = nullptr;
    }
}



நிலையான int64_t U_CALLCONV
ucstrTextLength(UText *ut) {
    எனில் (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        மாறிலி char16_t  *str = (மாறிலி char16_t *)ut->context;
        க்கு (;;) {
            எனில் (str[ut->chunkNativeLimit] == 0) {
                நிறுத்து;
            }
            ut->chunkNativeLimit++;
        }
        ut->a = ut->chunkNativeLimit;
        ut->chunkLength = (int32_t)ut->chunkNativeLimit;
        ut->nativeIndexingLimit = ut->chunkLength;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
    }
    திரும்பு ut->a;
}


நிலையான UBool U_CALLCONV
ucstrTextAccess(UText *ut, int64_t index, UBool  forward) {
    மாறிலி char16_t *str   = (மாறிலி char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    எனில் (index<0) {
        index = 0;
    } இல்லையெனில் எனில் (index < ut->chunkNativeLimit) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_SET_CP_START(str, 0, index);
    } இல்லையெனில் எனில் (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        index = ut->a;
    } இல்லையெனில் {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t scanLimit = (int32_t)index + 32;
        எனில் ((index + 32)>INT32_MAX || (index + 32)<0 ) {   // note: int64 expression
            scanLimit = INT32_MAX;
        }

        int32_t chunkLimit = (int32_t)ut->chunkNativeLimit;
        க்கு (; chunkLimit<scanLimit; chunkLimit++) {
            எனில் (str[chunkLimit] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunkLimit;
                ut->chunkLength = chunkLimit;
                ut->nativeIndexingLimit = chunkLimit;
                எனில் (index >= chunkLimit) {
                    index = chunkLimit;
                } இல்லையெனில் {
                    U16_SET_CP_START(str, 0, index);
                }

                ut->chunkNativeLimit = chunkLimit;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
                செல் breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_SET_CP_START(str, 0, index);
        எனில் (chunkLimit == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunkLimit;
            ut->chunkLength = chunkLimit;
            ut->nativeIndexingLimit = chunkLimit;
            எனில் (index > chunkLimit) {
                index = chunkLimit;
            }
            ut->chunkNativeLimit = chunkLimit;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
        } இல்லையெனில் {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            எனில் (U16_IS_LEAD(str[chunkLimit-1])) {
                --chunkLimit;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunkNativeLimit = chunkLimit;
            ut->nativeIndexingLimit = chunkLimit;
            ut->chunkLength = chunkLimit;
        }

    }
breakout:
    U_ASSERT(index<=INT32_MAX);
    ut->chunkOffset = (int32_t)index;

    // Check whether request is at the start or end
    UBool retVal = (forward && index<ut->chunkNativeLimit) || (!forward && index>0);
    திரும்பு retVal;
}



நிலையான int32_t U_CALLCONV
ucstrTextExtract(UText *ut,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *pErrorCode)
{
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(destCapacity<0 || (dest==nullptr && destCapacity>0) || start>limit) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstrTextAccess(ut, start, உண்மை);
    மாறிலி char16_t *s=ut->chunkContents;
    start32 = ut->chunkOffset;

    int32_t strLength=(int32_t)ut->a;
    எனில் (strLength >= 0) {
        limit32 = pinIndex(limit, strLength);
    } இல்லையெனில் {
        limit32 = pinIndex(limit, INT32_MAX);
    }
    di = 0;
    க்கு (si=start32; si<limit32; si++) {
        எனில் (strLength<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunkNativeLimit    = si;
            ut->chunkLength         = si;
            ut->nativeIndexingLimit = si;
            strLength               = si;
            limit32                 = si;
            நிறுத்து;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        எனில் (di<destCapacity) {
            // only store if there is space.
            dest[di] = s[si];
        } இல்லையெனில் {
            எனில் (strLength>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                நிறுத்து;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    எனில் (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<strLength || strLength<0)  && U16_IS_TRAIL(s[si])))
    {
        எனில் (di<destCapacity) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    எனில் (si <= ut->chunkNativeLimit) {
        ut->chunkOffset = si;
    } இல்லையெனில் {
        ucstrTextAccess(ut, si, உண்மை);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, destCapacity, di, pErrorCode);
    திரும்பு di;
}

நிலையான மாறிலி கட்டமைப்பு UTextFuncs ucstrFuncs =
{
    அளவு(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstrTextClone,
    ucstrTextLength,
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

நிலையான மாறிலி char16_t gEmptyUString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 மாறிலி char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UErrorCode* status) {
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில்(s==nullptr && length==0) {
        s = gEmptyUString;
    }
    எனில் (s==nullptr || length < -1 || length>INT32_MAX) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    ut = utext_setup(ut, 0, status);
    எனில் (U_SUCCESS(*status)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        எனில் (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunkNativeStart     = 0;
        ut->chunkNativeLimit     = length>=0? length : 0;
        ut->chunkLength          = (int32_t)ut->chunkNativeLimit;
        ut->chunkOffset          = 0;
        ut->nativeIndexingLimit  = ut->chunkLength;
    }
    திரும்பு ut;
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
#define CIBufSize 16

U_CDECL_BEGIN
நிலையான வெறுமை U_CALLCONV
charIterTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    delete ci;
    ut->r = nullptr;
}

நிலையான int64_t U_CALLCONV
charIterTextLength(UText *ut) {
    திரும்பு (int32_t)ut->a;
}

நிலையான UBool U_CALLCONV
charIterTextAccess(UText *ut, int64_t index, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clippedIndex = (int32_t)index;
    எனில் (clippedIndex<0) {
        clippedIndex=0;
    } இல்லையெனில் எனில் (clippedIndex>=ut->a) {
        clippedIndex=(int32_t)ut->a;
    }
    int32_t neededIndex = clippedIndex;
    எனில் (!forward && neededIndex>0) {
        // reverse iteration, want the position just before what was asked for.
        neededIndex--;
    } இல்லையெனில் எனில் (forward && neededIndex==ut->a && neededIndex>0) {
        // Forward iteration, don't ask for something past the end of the text.
        neededIndex--;
    }

    // Find the native index of the start of the buffer containing what we want.
    neededIndex -= neededIndex % CIBufSize;

    char16_t *buf = nullptr;
    UBool  needChunkSetup = உண்மை;
    முழு    i;
    எனில் (ut->chunkNativeStart == neededIndex) {
        // The buffer we want is already the current chunk.
        needChunkSetup = பொய்;
    } இல்லையெனில் எனில் (ut->b == neededIndex) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } இல்லையெனில் எனில் (ut->c == neededIndex) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } இல்லையெனில் {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        எனில் (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->setIndex(neededIndex);
        க்கு (i=0; i<CIBufSize; i++) {
            buf[i] = ci->nextPostInc();
            எனில் (i+neededIndex > ut->a) {
                நிறுத்து;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    எனில் (needChunkSetup) {
        ut->chunkContents = buf;
        ut->chunkLength   = CIBufSize;
        ut->chunkNativeStart = neededIndex;
        ut->chunkNativeLimit = neededIndex + CIBufSize;
        எனில் (ut->chunkNativeLimit > ut->a) {
            ut->chunkNativeLimit = ut->a;
            ut->chunkLength  = (int32_t)(ut->chunkNativeLimit)-(int32_t)(ut->chunkNativeStart);
        }
        ut->nativeIndexingLimit = ut->chunkLength;
        U_ASSERT(ut->chunkOffset>=0 && ut->chunkOffset<=CIBufSize);
    }
    ut->chunkOffset = clippedIndex - (int32_t)ut->chunkNativeStart;
    UBool success = (forward? ut->chunkOffset<ut->chunkLength : ut->chunkOffset>0);
    திரும்பு success;
}

நிலையான UText * U_CALLCONV
charIterTextClone(UText *dest, மாறிலி UText *src, UBool deep, UErrorCode * status) {
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    எனில் (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *status = U_UNSUPPORTED_ERROR;
        திரும்பு nullptr;
    } இல்லையெனில் {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, status);
        எனில் (U_FAILURE(*status)) {
            திரும்பு dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_getNativeIndex((UText *)src);
        utext_setNativeIndex(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    திரும்பு dest;
}

நிலையான int32_t U_CALLCONV
charIterTextExtract(UText *ut,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *status)
{
    எனில்(U_FAILURE(*status)) {
        திரும்பு 0;
    }
    எனில்(destCapacity<0 || (dest==nullptr && destCapacity>0) || start>limit) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pinIndex(start, length);
    int32_t  limit32 = pinIndex(limit, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copyLimit;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->setIndex32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->getIndex();
    copyLimit = srci;
    போது (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_LENGTH(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        எனில் (desti+len <= destCapacity) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copyLimit = srci+len;
        } இல்லையெனில் {
            desti += len;
            *status = U_BUFFER_OVERFLOW_ERROR;
        }
        srci += len;
    }

    charIterTextAccess(ut, copyLimit, உண்மை);

    u_terminateUChars(dest, destCapacity, desti, status);
    திரும்பு desti;
}

நிலையான மாறிலி கட்டமைப்பு UTextFuncs charIterFuncs =
{
    அளவு(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    charIterTextClone,
    charIterTextLength,
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
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    எனில் (ci->startIndex() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *status = U_UNSUPPORTED_ERROR;
        திரும்பு nullptr;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBufSize * அளவு(char16_t);
    ut = utext_setup(ut, extraSpace, status);
    எனில் (U_SUCCESS(*status)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->endIndex();        // Length of text
        ut->p                    = ut->pExtra;            // First buffer
        ut->b                    = -1;                    // Native index of first buffer contents
        ut->q                    = (char16_t*)ut->pExtra+CIBufSize;  // Second buffer
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
        ut->chunkLength          = 0;
        ut->nativeIndexingLimit  = ut->chunkOffset;  // enables native indexing
    }
    திரும்பு ut;
}

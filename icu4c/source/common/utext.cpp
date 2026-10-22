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

#شامل <cstddef>

#شامل "unicode/utypes.h"
#شامل "unicode/ustring.h"
#شامل "unicode/unistr.h"
#شامل "unicode/chariter.h"
#شامل "unicode/utext.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "ustr_imp.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "uassert.h"
#شامل "putilimp.h"

U_NAMESPACE_USE

#define I32_FLAG(bitIndex) ((int32_t)1<<(bitIndex))


ایستا UBool
utext_access(UText *ut, int64_t index, UBool forward) {
    بازگشت ut->pFuncs->access(ut, index, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    اگر (delta > 0) {
        انجامبده {
            اگر(ut->chunkOffset>=ut->chunkLength && !utext_access(ut, ut->chunkNativeLimit, درست)) {
                بازگشت نادرست;
            }
            c = ut->chunkContents[ut->chunkOffset];
            اگر (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                اگر (c == U_SENTINEL) {
                    بازگشت نادرست;
                }
            } وگرنه {
                ut->chunkOffset++;
            }
        } وقتی(--delta>0);

    } وگرنه اگر (delta<0) {
        انجامبده {
            اگر(ut->chunkOffset<=0 && !utext_access(ut, ut->chunkNativeStart, نادرست)) {
                بازگشت نادرست;
            }
            c = ut->chunkContents[ut->chunkOffset-1];
            اگر (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                اگر (c == U_SENTINEL) {
                    بازگشت نادرست;
                }
            } وگرنه {
                ut->chunkOffset--;
            }
        } وقتی(++delta<0);
    }

    بازگشت درست;
}


U_CAPI int64_t U_EXPORT2
utext_nativeLength(UText *ut) {
    بازگشت ut->pFuncs->nativeLength(ut);
}


U_CAPI UBool U_EXPORT2
utext_isLengthExpensive(ثابت UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE)) != 0;
    بازگشت r;
}


U_CAPI int64_t U_EXPORT2
utext_getNativeIndex(ثابت UText *ut) {
    اگر(ut->chunkOffset <= ut->nativeIndexingLimit) {
        بازگشت ut->chunkNativeStart+ut->chunkOffset;
    } وگرنه {
        بازگشت ut->pFuncs->mapOffsetToNative(ut);
    }
}


U_CAPI تهی U_EXPORT2
utext_setNativeIndex(UText *ut, int64_t index) {
    اگر(index<ut->chunkNativeStart || index>=ut->chunkNativeLimit) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, index, درست);
    } وگرنه اگر((int32_t)(index - ut->chunkNativeStart) <= ut->nativeIndexingLimit) {
        // utf-16 indexing.
        ut->chunkOffset=(int32_t)(index-ut->chunkNativeStart);
    } وگرنه {
         ut->chunkOffset=ut->pFuncs->mapNativeIndexToUTF16(ut, index);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    اگر (ut->chunkOffset<ut->chunkLength) {
        char16_t c= ut->chunkContents[ut->chunkOffset];
        اگر (U16_IS_TRAIL(c)) {
            اگر (ut->chunkOffset==0) {
                ut->pFuncs->access(ut, ut->chunkNativeStart, نادرست);
            }
            اگر (ut->chunkOffset>0) {
                char16_t lead = ut->chunkContents[ut->chunkOffset-1];
                اگر (U16_IS_LEAD(lead)) {
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
    اگر (i >= 0) {
        char16_t c = ut->chunkContents[i];
        اگر (U16_IS_TRAIL(c) == نادرست) {
            اگر (i <= ut->nativeIndexingLimit) {
                result = ut->chunkNativeStart + i;
            } وگرنه {
                ut->chunkOffset = i;
                result = ut->pFuncs->mapOffsetToNative(ut);
                ut->chunkOffset++;
            }
            بازگشت result;
        }
    }

    // If at the start of text, simply return 0.
    اگر (ut->chunkOffset==0 && ut->chunkNativeStart==0) {
        بازگشت 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    result = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    بازگشت result;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    اگر (ut->chunkOffset==ut->chunkLength) {
        // Current position is just off the end of the chunk.
        اگر (ut->pFuncs->access(ut, ut->chunkNativeLimit, درست) == نادرست) {
            // Off the end of the text.
            بازگشت U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunkOffset];
    اگر (U16_IS_LEAD(c) == نادرست) {
        // Normal, non-supplementary case.
        بازگشت c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    اگر ((ut->chunkOffset+1) < ut->chunkLength) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunkOffset+1];
    } وگرنه {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunkNativeLimit;
        اگر (ut->pFuncs->access(ut, nativePosition, درست)) {
            trail = ut->chunkContents[ut->chunkOffset];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, نادرست);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunkOffset = ut->chunkLength - 1;
        اگر(!r) {
            بازگشت U_SENTINEL;
        }
    }

    اگر (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    بازگشت supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t nativeIndex) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    اگر (nativeIndex>=ut->chunkNativeStart && nativeIndex < ut->chunkNativeStart + ut->nativeIndexingLimit) {
        ut->chunkOffset = (int32_t)(nativeIndex - ut->chunkNativeStart);
        c = ut->chunkContents[ut->chunkOffset];
        اگر (U16_IS_SURROGATE(c) == نادرست) {
            بازگشت c;
        }
    }


    utext_setNativeIndex(ut, nativeIndex);
    اگر (nativeIndex>=ut->chunkNativeStart && ut->chunkOffset<ut->chunkLength) {
        c = ut->chunkContents[ut->chunkOffset];
        اگر (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    بازگشت c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    اگر (ut->chunkOffset >= ut->chunkLength) {
        اگر (ut->pFuncs->access(ut, ut->chunkNativeLimit, درست) == نادرست) {
            بازگشت U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunkOffset++];
    اگر (U16_IS_LEAD(c) == نادرست) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        بازگشت c;
    }

    اگر (ut->chunkOffset >= ut->chunkLength) {
        اگر (ut->pFuncs->access(ut, ut->chunkNativeLimit, درست) == نادرست) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            بازگشت c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunkOffset];
    اگر (U16_IS_TRAIL(trail) == نادرست) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        بازگشت c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunkOffset++;   // move iteration position over the trail surrogate.
    بازگشت supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    اگر (ut->chunkOffset <= 0) {
        اگر (ut->pFuncs->access(ut, ut->chunkNativeStart, نادرست) == نادرست) {
            بازگشت U_SENTINEL;
        }
    }
    ut->chunkOffset--;
    c = ut->chunkContents[ut->chunkOffset];
    اگر (U16_IS_TRAIL(c) == نادرست) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        بازگشت c;
    }

    اگر (ut->chunkOffset <= 0) {
        اگر (ut->pFuncs->access(ut, ut->chunkNativeStart, نادرست) == نادرست) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            بازگشت c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunkOffset-1];
    اگر (U16_IS_LEAD(lead) == نادرست) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        بازگشت c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunkOffset--;   // move iteration position over the lead surrogate.
    بازگشت supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t index) {
    UChar32       c      = U_SENTINEL;

    اگر(index<ut->chunkNativeStart || index>=ut->chunkNativeLimit) {
        // Desired position is outside of the current chunk.
        اگر(!ut->pFuncs->access(ut, index, درست)) {
            // no chunk available here
            بازگشت U_SENTINEL;
        }
    } وگرنه اگر (index - ut->chunkNativeStart  <= (int64_t)ut->nativeIndexingLimit) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunkOffset = (int32_t)(index - ut->chunkNativeStart);
    } وگرنه {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunkOffset = ut->pFuncs->mapNativeIndexToUTF16(ut, index);
    }

    c = ut->chunkContents[ut->chunkOffset++];
    اگر (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_setNativeIndex(ut, index);
        c = utext_next32(ut);
    }
    بازگشت c;
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
    اگر(index<=ut->chunkNativeStart || index>ut->chunkNativeLimit) {
        // Requested native index is outside of the current chunk.
        اگر(!ut->pFuncs->access(ut, index, نادرست)) {
            // no chunk available here
            بازگشت U_SENTINEL;
        }
    } وگرنه اگر(index - ut->chunkNativeStart <= (int64_t)ut->nativeIndexingLimit) {
        // Direct UTF-16 indexing.
        ut->chunkOffset = (int32_t)(index - ut->chunkNativeStart);
    } وگرنه {
        ut->chunkOffset=ut->pFuncs->mapNativeIndexToUTF16(ut, index);
        اگر (ut->chunkOffset==0 && !ut->pFuncs->access(ut, index, نادرست)) {
            // no chunk available here
            بازگشت U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunkOffset--;
    cPrev = ut->chunkContents[ut->chunkOffset];

    اگر (U16_IS_SURROGATE(cPrev)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_setNativeIndex(ut, index);
        cPrev = utext_previous32(ut);
    }
    بازگشت cPrev;
}


U_CAPI int32_t U_EXPORT2
utext_extract(UText *ut,
             int64_t start, int64_t limit,
             char16_t *dest, int32_t destCapacity,
             UErrorCode *status) {
                 بازگشت ut->pFuncs->extract(ut, start, limit, dest, destCapacity, status);
             }



U_CAPI UBool U_EXPORT2
utext_equals(ثابت UText *a, ثابت UText *b) {
    اگر (a==nullptr || b==nullptr ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            بازگشت نادرست;
    }

    اگر (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        بازگشت نادرست;
    }

    اگر (a->context != b->context) {
        // Different sources (different strings)
        بازگشت نادرست;
    }
    اگر (utext_getNativeIndex(a) != utext_getNativeIndex(b)) {
        // Different current position in the string.
        بازگشت نادرست;
    }

    بازگشت درست;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(ثابت UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    بازگشت b;
}


U_CAPI تهی U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_hasMetaData(ثابت UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_DATA)) != 0;
    بازگشت b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t nativeStart, int64_t nativeLimit,
             ثابت char16_t *replacementText, int32_t replacementLength,
             UErrorCode *status)
{
    اگر (U_FAILURE(*status)) {
        بازگشت 0;
    }
    اگر ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *status = U_NO_WRITE_PERMISSION;
        بازگشت 0;
    }
    int32_t i = ut->pFuncs->replace(ut, nativeStart, nativeLimit, replacementText, replacementLength, status);
    بازگشت i;
}

U_CAPI تهی U_EXPORT2
utext_copy(UText *ut,
          int64_t nativeStart, int64_t nativeLimit,
          int64_t destIndex,
          UBool move,
          UErrorCode *status)
{
    اگر (U_FAILURE(*status)) {
        بازگشت;
    }
    اگر ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *status = U_NO_WRITE_PERMISSION;
        بازگشت;
    }
    ut->pFuncs->copy(ut, nativeStart, nativeLimit, destIndex, move, status);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            ثابت UText* src,
            UBool deep,
            UBool readOnly,
            UErrorCode* status) {
    اگر (U_FAILURE(*status)) {
        بازگشت dest;
    }
    UText *result = src->pFuncs->clone(dest, src, deep, status);
    اگر (U_FAILURE(*status)) {
        بازگشت result;
    }
    اگر (result == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت result;
    }
    اگر (readOnly) {
        utext_freeze(result);
    }
    بازگشت result;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
شمارش {
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

ساختار ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

ایستا ثابت UText emptyText = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_setup(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UErrorCode* status) {
    اگر (U_FAILURE(*status)) {
        بازگشت ut;
    }

    اگر (ut == nullptr) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = اندازه(UText);
        اگر (extraSpace > 0) {
            spaceRequired = اندازه(ExtendedUText) + extraSpace - اندازه(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        اگر (ut == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت nullptr;
        } وگرنه {
            *ut = emptyText;
            ut->flags |= UTEXT_HEAP_ALLOCATED;
            اگر (spaceRequired>0) {
                ut->extraSize = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } وگرنه {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        اگر (ut->magic != UTEXT_MAGIC) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        اگر ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != nullptr)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        اگر (extraSpace > ut->extraSize) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            اگر (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extraSize = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            اگر (ut->pExtra == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } وگرنه {
                ut->extraSize = extraSpace;
                ut->flags |= UTEXT_EXTRA_HEAP_ALLOCATED;
            }
        }
    }
    اگر (U_SUCCESS(*status)) {
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
        اگر (ut->pExtra!=nullptr && ut->extraSize>0)
            uprv_memset(ut->pExtra, 0, ut->extraSize);

    }
    بازگشت ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    اگر (ut==nullptr ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        بازگشت ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    اگر (ut->pFuncs->close != nullptr) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    اگر (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = nullptr;
        ut->flags &= ~UTEXT_EXTRA_HEAP_ALLOCATED;
        ut->extraSize = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = nullptr;

    اگر (ut->flags & UTEXT_HEAP_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = nullptr;
    }
    بازگشت ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
ایستا تهی
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
ایستا int32_t
pinIndex(int64_t &index, int64_t limit) {
    اگر (index<0) {
        index = 0;
    } وگرنه اگر (index > limit) {
        index = limit;
    }
    بازگشت static_cast<int32_t>(index);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
ایستا تهی adjustPointer(UText *dest, ثابت تهی **destPtr, ثابت UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    نویسه  *dptr = (نویسه *)*destPtr;
    نویسه  *dUText = (نویسه *)dest;
    نویسه  *sUText = (نویسه *)src;

    اگر (dptr >= (نویسه *)src->pExtra && dptr < ((نویسه*)src->pExtra)+src->extraSize) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((نویسه *)dest->pExtra) + (dptr - (نویسه *)src->pExtra);
    } وگرنه اگر (dptr>=sUText && dptr < sUText+src->sizeOfStruct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
ایستا UText * U_CALLCONV
shallowTextClone(UText * dest, ثابت UText * src, UErrorCode * status) {
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    int32_t  srcExtraSize = src->extraSize;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_setup(dest, srcExtraSize, status);
    اگر (U_FAILURE(*status)) {
        بازگشت dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    تهی *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    صحیح sizeToCopy = src->sizeOfStruct;
    اگر (sizeToCopy > dest->sizeOfStruct) {
        sizeToCopy = dest->sizeOfStruct;
    }
    uprv_memcpy(dest, src, sizeToCopy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    اگر (srcExtraSize > 0) {
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
    adjustPointer(dest, (ثابت تهی **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

    بازگشت dest;
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
شمارش { UTF8_TEXT_CHUNK_SIZE=32 };

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

ساختار UTF8Buf {
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
ایستا int64_t U_CALLCONV
utf8TextLength(UText *ut) {
    اگر (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        ثابت نویسه *r = (ثابت نویسه *)ut->context + ut->c;
        وقتی (*r != 0) {
            r++;
        }
        اگر ((r - (ثابت نویسه *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (ثابت نویسه *)ut->context);
        } وگرنه {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
    }
    بازگشت ut->b;
}






ایستا UBool U_CALLCONV
utf8TextAccess(UText *ut, int64_t index, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    ثابت uint8_t *s8=(ثابت uint8_t *)ut->context;
    UTF8Buf *u8b = nullptr;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)index;     // Requested index, trimmed to 32 bits.
    int32_t  mapIndex = 0;
    اگر (index<0) {
        ix=0;
    } وگرنه اگر (index > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    اگر (ix>length) {
        اگر (length>=0) {
            ix=length;
        } وگرنه اگر (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            وقتی (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            اگر (s8[ut->c] == 0) {
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
    اگر (forward) {
        اگر (ix==ut->chunkNativeLimit) {
            // Check for normal sequential iteration cases first.
            اگر (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunkOffset = ut->chunkLength;
                بازگشت نادرست;
            } وگرنه {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                اگر (ix>=altB->bufNativeStart && ix<altB->bufNativeLimit) {
                    برو swapBuffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            اگر (ix>=u8b->bufNativeStart && ix<u8b->bufNativeLimit) {
                // Requested index is in the other buffer.
                برو swapBuffers;
            }
            اگر (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                اگر (ix == ut->chunkNativeLimit) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunkOffset = ut->chunkLength;
                    بازگشت نادرست;
                }
                اگر (ix == u8b->bufNativeLimit) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    برو swapBuffersAndFail;
                }

                // Neither existing buffer extends to the end of the string.
                برو makeStubBuffer;
            }

            اگر (ix<ut->chunkNativeStart || ix>=ut->chunkNativeLimit) {
                // Requested index is in neither buffer.
                برو fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            mapIndex = ix - u8b->toUCharsMapStart;
            U_ASSERT(mapIndex < (int32_t)اندازه(UTF8Buf::mapToUChars));
            ut->chunkOffset = u8b->mapToUChars[mapIndex] - u8b->bufStartIdx;
            بازگشت درست;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    اگر (ix==ut->chunkNativeStart) {
        // Check for normal sequential iteration cases first.
        اگر (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunkOffset = 0;
            بازگشت نادرست;
        } وگرنه {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            اگر (ix>altB->bufNativeStart && ix<=altB->bufNativeLimit) {
                برو swapBuffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    اگر (ix>u8b->bufNativeStart && ix<=u8b->bufNativeLimit) {
        // Requested index is in the other buffer.
        برو swapBuffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    اگر (ix==0) {
        اگر (u8b->bufNativeStart==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            برو swapBuffersAndFail;
        } وگرنه {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            برو makeStubBuffer;
        }
    }

    اگر (ix<=ut->chunkNativeStart || ix>ut->chunkNativeLimit) {
        // Requested index is in neither buffer.
        برو fillReverse;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    mapIndex = ix - u8b->toUCharsMapStart;
    ut->chunkOffset = u8b->mapToUChars[mapIndex] - u8b->bufStartIdx;
    اگر (ut->chunkOffset==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        بازگشت نادرست;
    } وگرنه {
        بازگشت درست;
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
        U_ASSERT(mapIndex<(int32_t)اندازه(u8b->mapToUChars));
        ut->chunkOffset = u8b->mapToUChars[mapIndex] - u8b->bufStartIdx;

        بازگشت درست;
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
    اگر (ix==u8b->bufNativeLimit) {
        ut->chunkOffset = ut->chunkLength;
    } وگرنه  {
        ut->chunkOffset = 0;
        U_ASSERT(ix == u8b->bufNativeStart);
    }
    بازگشت نادرست;

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
    برو swapBuffersAndFail;



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
        UBool   nulTerminated = نادرست;
        اگر (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = درست;
        }

        char16_t   *buf = u8b_swap->buf;
        uint8_t *mapToNative  = u8b_swap->mapToNative;
        uint8_t *mapToUChars  = u8b_swap->mapToUChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    seenNonAscii = نادرست;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        وقتی (destIx<UTF8_TEXT_CHUNK_SIZE) {
            c = s8[srcIx];
            اگر (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                mapToNative[destIx]    = (uint8_t)(srcIx - ix);
                mapToUChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } وگرنه {
                // General case, handle everything.
                اگر (seenNonAscii == نادرست) {
                    seenNonAscii = درست;
                    u8b_swap->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_NEXT_OR_FFFD(s8, srcIx, strLen, c);
                اگر (c==0 && nulTerminated) {
                    srcIx--;
                    بشکن;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                انجامبده {
                    mapToNative[dIx++] = (uint8_t)(cIx - ix);
                } وقتی (dIx < destIx);

                انجامبده {
                    mapToUChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } وقتی (cIx < srcIx);
            }
            اگر (srcIx>=strLen) {
                بشکن;
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
        اگر (seenNonAscii == نادرست) {
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
        اگر (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            اگر (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
            }
        }
        بازگشت درست;
    }


fillReverse:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        اگر (ix != ut->b) {
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
        int32_t  toUCharsMapStart = ix - اندازه(UTF8Buf::mapToUChars) + 1;
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
        وقتی (destIx>2 && (srcIx - toUCharsMapStart > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            اگر (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(toUCharsMapStart <= srcIx);
                mapToUChars[srcIx - toUCharsMapStart] = (uint8_t)destIx;
                mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
            } وگرنه {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_prevCharSafeBody(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                اگر (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                } وگرنه {
                    buf[destIx]         = U16_TRAIL(c);
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                    buf[--destIx]       = U16_LEAD(c);
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                }

                // Fill in the map from native indexes to UChars buf index.
                انجامبده {
                    mapToUChars[sIx-- - toUCharsMapStart] = (uint8_t)destIx;
                } وقتی (sIx >= srcIx);
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
        بازگشت درست;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
ایستا char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t destCapacity,
              int32_t *pDestLength,
              ثابت نویسه* src,
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


    وقتی((index < srcLength)&&(pDest<pDestLimit)){
        ch = pSrc[index++];
        اگر(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }وگرنه{
            ch=utf8_nextCharSafeBody(pSrc, &index, srcLength, ch, -3);
            اگر(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }وگرنه{
                *(pDest++)=U16_LEAD(ch);
                اگر(pDest<pDestLimit){
                    *(pDest++)=U16_TRAIL(ch);
                }وگرنه{
                    reqLength++;
                    بشکن;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    وقتی(index < srcLength){
        ch = pSrc[index++];
        اگر(ch <= 0x7f){
            reqLength++;
        }وگرنه{
            ch=utf8_nextCharSafeBody(pSrc, &index, srcLength, ch, -3);
            reqLength+=U16_LENGTH(ch);
        }
    }

    reqLength+=(int32_t)(pDest - dest);

    اگر(pDestLength){
        *pDestLength = reqLength;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,reqLength,pErrorCode);

    بازگشت dest;
}



ایستا int32_t U_CALLCONV
utf8TextExtract(UText *ut,
                int64_t start, int64_t limit,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }
    اگر(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pinIndex(start, length);
    int32_t  limit32 = pinIndex(limit, length);

    اگر(start32>limit32) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    ثابت uint8_t *buf = (ثابت uint8_t *)ut->context;
    صحیح i;
    اگر (start32 < ut->chunkNativeLimit) {
        برای (i=0; i<3; i++) {
            اگر (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                بشکن;
            }
            start32--;
        }
    }

    اگر (limit32 < ut->chunkNativeLimit) {
        برای (i=0; i<3; i++) {
            اگر (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                بشکن;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t destLength=0;
    utext_strFromUTF8(dest, destCapacity, &destLength,
                    (ثابت نویسه *)ut->context+start32, limit32-start32,
                    pErrorCode);
    utf8TextAccess(ut, limit32, درست);
    بازگشت destLength;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
ایستا int64_t U_CALLCONV
utf8TextMapOffsetToNative(ثابت UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunkOffset>ut->nativeIndexingLimit && ut->chunkOffset<=ut->chunkLength);
    int32_t nativeOffset = u8b->mapToNative[ut->chunkOffset + u8b->bufStartIdx] + u8b->toUCharsMapStart;
    U_ASSERT(nativeOffset >= ut->chunkNativeStart && nativeOffset <= ut->chunkNativeLimit);
    بازگشت nativeOffset;
}

//
// Map a native index to the corresponding chunk offset
//
ایستا int32_t U_CALLCONV
utf8TextMapIndexToUTF16(ثابت UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t index = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(index>=ut->chunkNativeStart+ut->nativeIndexingLimit);
    U_ASSERT(index<=ut->chunkNativeLimit);
    int32_t mapIndex = index - u8b->toUCharsMapStart;
    U_ASSERT(mapIndex < (int32_t)اندازه(UTF8Buf::mapToUChars));
    int32_t offset = u8b->mapToUChars[mapIndex] - u8b->bufStartIdx;
    U_ASSERT(offset>=0 && offset<=ut->chunkLength);
    بازگشت offset;
}

ایستا UText * U_CALLCONV
utf8TextClone(UText *dest, ثابت UText *src, UBool deep, UErrorCode *status)
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
    اگر (deep && U_SUCCESS(*status)) {
        int32_t  len = (int32_t)utext_nativeLength((UText *)src);
        نویسه *copyStr = (نویسه *)uprv_malloc(len+1);
        اگر (copyStr == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } وگرنه {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    بازگشت dest;
}


ایستا تهی U_CALLCONV
utf8TextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    اگر (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        نویسه *s = (نویسه *)ut->context;
        uprv_free(s);
        ut->context = nullptr;
    }
}

U_CDECL_END


ایستا ثابت ساختار UTextFuncs utf8Funcs =
{
    اندازه(UTextFuncs),
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


ایستا ثابت نویسه gEmptyString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               ثابت نویسه* s U_LIFETIME_BOUND,
               int64_t length,
               UErrorCode* status) {
    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر(s==nullptr && length==0) {
        s = gEmptyString;
    }

    اگر(s==nullptr || length<-1 || length>INT32_MAX) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    ut = utext_setup(ut, اندازه(UTF8Buf) * 2, status);
    اگر (U_FAILURE(*status)) {
        بازگشت ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    اگر (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (نویسه *)ut->pExtra + اندازه(UTF8Buf);
    بازگشت ut;

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
شمارش { REP_TEXT_CHUNK_SIZE=10 };

ساختار ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_TEXT_CHUNK_SIZE+1];
};


U_CDECL_BEGIN

ایستا UText * U_CALLCONV
repTextClone(UText *dest, ثابت UText *src, UBool deep, UErrorCode *status) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    اگر (deep && U_SUCCESS(*status)) {
        ثابت Replaceable *replSrc = (ثابت Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    بازگشت dest;
}


ایستا تهی U_CALLCONV
repTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    اگر (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        Replaceable *rep = (Replaceable *)ut->context;
        delete rep;
        ut->context = nullptr;
    }
}


ایستا int64_t U_CALLCONV
repTextLength(UText *ut) {
    ثابت Replaceable *replSrc = (ثابت Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    بازگشت len;
}


ایستا UBool U_CALLCONV
repTextAccess(UText *ut, int64_t index, UBool forward) {
    ثابت Replaceable *rep=(ثابت Replaceable *)ut->context;
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
    اگر(forward) {

        اگر (index32>=ut->chunkNativeStart && index32<ut->chunkNativeLimit) {
            // Buffer already contains the requested position.
            ut->chunkOffset = (int32_t)(index - ut->chunkNativeStart);
            بازگشت درست;
        }
        اگر (index32>=length && ut->chunkNativeLimit==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunkOffset = length - (int32_t)ut->chunkNativeStart;
            بازگشت نادرست;
        }

        ut->chunkNativeLimit = index + REP_TEXT_CHUNK_SIZE - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        اگر(ut->chunkNativeLimit > length) {
            ut->chunkNativeLimit = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunkNativeStart = ut->chunkNativeLimit - REP_TEXT_CHUNK_SIZE;
        اگر(ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }
    } وگرنه {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        اگر (index32>ut->chunkNativeStart && index32<=ut->chunkNativeLimit) {
            // Requested position already in buffer.
            ut->chunkOffset = index32 - (int32_t)ut->chunkNativeStart;
            بازگشت درست;
        }
        اگر (index32==0 && ut->chunkNativeStart==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunkOffset = 0;
            بازگشت نادرست;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunkNativeStart = index32 + 1 - REP_TEXT_CHUNK_SIZE;
        اگر (ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }

        ut->chunkNativeLimit = index32 + 1;
        اگر (ut->chunkNativeLimit > length) {
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
    اگر (ut->chunkNativeLimit < length &&
        U16_IS_LEAD(ex->s[ut->chunkLength-1])) {
            ut->chunkLength--;
            ut->chunkNativeLimit--;
            اگر (ut->chunkOffset > ut->chunkLength) {
                ut->chunkOffset = ut->chunkLength;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    اگر(ut->chunkNativeStart>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunkNativeStart);
        --(ut->chunkLength);
        --(ut->chunkOffset);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_SET_CP_START(ut->chunkContents, 0, ut->chunkOffset);

    // Use fast indexing for get/setNativeIndex()
    ut->nativeIndexingLimit = ut->chunkLength;

    بازگشت درست;
}



ایستا int32_t U_CALLCONV
repTextExtract(UText *ut,
               int64_t start, int64_t limit,
               char16_t *dest, int32_t destCapacity,
               UErrorCode *status) {
    ثابت Replaceable *rep=(ثابت Replaceable *)ut->context;
    int32_t  length=rep->length();

    اگر(U_FAILURE(*status)) {
        بازگشت 0;
    }
    اگر(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
    }
    اگر(start>limit) {
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }

    int32_t  start32 = pinIndex(start, length);
    int32_t  limit32 = pinIndex(limit, length);

    // adjust start, limit if they point to trail half of surrogates
    اگر (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    اگر (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    اگر(length>destCapacity) {
        limit32 = start32 + destCapacity;
    }
    UnicodeString buffer(dest, 0, destCapacity); // writable alias
    rep->extractBetween(start32, limit32, buffer);
    repTextAccess(ut, limit32, درست);

    بازگشت u_terminateUChars(dest, destCapacity, length, status);
}

ایستا int32_t U_CALLCONV
repTextReplace(UText *ut,
               int64_t start, int64_t limit,
               ثابت char16_t *src, int32_t length,
               UErrorCode *status) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t oldLength;

    اگر(U_FAILURE(*status)) {
        بازگشت 0;
    }
    اگر(src==nullptr && length!=0) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    oldLength=rep->length(); // will subtract from new length
    اگر(start>limit ) {
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }

    int32_t start32 = pinIndex(start, oldLength);
    int32_t limit32 = pinIndex(limit, oldLength);

    // Snap start & limit to code point boundaries.
    اگر (start32<oldLength && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    اگر (limit32<oldLength && U16_IS_LEAD(rep->charAt(limit32-1)) &&
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
    اگر (ut->chunkNativeLimit > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t newIndexPos = limit32 + lengthDelta;
    repTextAccess(ut, newIndexPos, درست);

    بازگشت lengthDelta;
}


ایستا تهی U_CALLCONV
repTextCopy(UText *ut,
                int64_t start, int64_t limit,
                int64_t destIndex,
                UBool move,
                UErrorCode *status)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    اگر(U_FAILURE(*status)) {
        بازگشت;
    }
    اگر (start>limit || (start<destIndex && destIndex<limit))
    {
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت;
    }

    int32_t start32     = pinIndex(start, length);
    int32_t limit32     = pinIndex(limit, length);
    int32_t destIndex32 = pinIndex(destIndex, length);

    // TODO:  snap input parameters to code point boundaries.

    اگر(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t segLength=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        اگر(destIndex32<start32) {
            start32+=segLength;
            limit32+=segLength;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } وگرنه {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t firstAffectedIndex = destIndex32;
    اگر (move && start32<firstAffectedIndex) {
        firstAffectedIndex = start32;
    }
    اگر (firstAffectedIndex < ut->chunkNativeLimit) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  nativeIterIndex = destIndex32 + limit32 - start32;
    اگر (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        nativeIterIndex = destIndex32;
    }

    // Set position, reload chunk if needed.
    repTextAccess(ut, nativeIterIndex, درست);
}

ایستا ثابت ساختار UTextFuncs repFuncs =
{
    اندازه(UTextFuncs),
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
    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر(rep==nullptr) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    ut = utext_setup(ut, اندازه(ReplExtra), status);
    اگر(U_FAILURE(*status)) {
        بازگشت ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    اگر(rep->hasMetaData()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_DATA);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    بازگشت ut;
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


ایستا UText * U_CALLCONV
unistrTextClone(UText *dest, ثابت UText *src, UBool deep, UErrorCode *status) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    اگر (deep && U_SUCCESS(*status)) {
        ثابت UnicodeString *srcString = (ثابت UnicodeString *)src->context;
        dest->context = جدید UnicodeString(*srcString);
        dest->chunkContents = ((ثابت UnicodeString *)dest->context)->getBuffer();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    بازگشت dest;
}

ایستا تهی U_CALLCONV
unistrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    اگر (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        delete str;
        ut->context = nullptr;
    }
}


ایستا int64_t U_CALLCONV
unistrTextLength(UText *t) {
    بازگشت ((ثابت UnicodeString *)t->context)->length();
}


ایستا UBool U_CALLCONV
unistrTextAccess(UText *ut, int64_t index, UBool  forward) {
    int32_t length  = ut->chunkLength;
    ut->chunkOffset = pinIndex(index, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && index<length) || (!forward && index>0);
    بازگشت retVal;
}



ایستا int32_t U_CALLCONV
unistrTextExtract(UText *t,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *pErrorCode) {
    ثابت UnicodeString *us=(ثابت UnicodeString *)t->context;
    int32_t length=us->length();

    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }
    اگر(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
    اگر(start<0 || start>limit) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }

    int32_t start32 = start<length ? us->getChar32Start((int32_t)start) : length;
    int32_t limit32 = limit<length ? us->getChar32Start((int32_t)limit) : length;

    length=limit32-start32;
    اگر (destCapacity>0 && dest!=nullptr) {
        int32_t trimmedLength = length;
        اگر(trimmedLength>destCapacity) {
            trimmedLength=destCapacity;
        }
        us->extract(start32, trimmedLength, dest);
        t->chunkOffset = start32+trimmedLength;
    } وگرنه {
        t->chunkOffset = start32;
    }
    u_terminateUChars(dest, destCapacity, length, pErrorCode);
    بازگشت length;
}

ایستا int32_t U_CALLCONV
unistrTextReplace(UText *ut,
                  int64_t start, int64_t limit,
                  ثابت char16_t *src, int32_t length,
                  UErrorCode *pErrorCode) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t oldLength;

    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }
    اگر(src==nullptr && length!=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
    اگر(start>limit) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }
    oldLength=us->length();
    int32_t start32 = pinIndex(start, oldLength);
    int32_t limit32 = pinIndex(limit, oldLength);
    اگر (start32 < oldLength) {
        start32 = us->getChar32Start(start32);
    }
    اگر (limit32 < oldLength) {
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

    بازگشت lengthDelta;
}

ایستا تهی U_CALLCONV
unistrTextCopy(UText *ut,
               int64_t start, int64_t limit,
               int64_t destIndex,
               UBool move,
               UErrorCode *pErrorCode) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت;
    }
    int32_t start32 = pinIndex(start, length);
    int32_t limit32 = pinIndex(limit, length);
    int32_t destIndex32 = pinIndex(destIndex, length);

    اگر( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت;
    }

    اگر(move) {
        // move: copy to destIndex, then remove original
        int32_t segLength=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        اگر(destIndex32<start32) {
            start32+=segLength;
        }
        us->remove(start32, segLength);
    } وگرنه {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->getBuffer();
    اگر (move==نادرست) {
        // copy operation, string length grows
        ut->chunkLength += limit32-start32;
        ut->chunkNativeLimit = ut->chunkLength;
        ut->nativeIndexingLimit = ut->chunkLength;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunkOffset = destIndex32+limit32-start32;
    اگر (move && destIndex32>start32) {
        ut->chunkOffset = destIndex32;
    }

}

ایستا ثابت ساختار UTextFuncs unistrFuncs =
{
    اندازه(UTextFuncs),
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
    اگر (U_SUCCESS(*status)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    بازگشت ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             ثابت UnicodeString* s U_LIFETIME_BOUND,
                             UErrorCode* status) {
    اگر (U_SUCCESS(*status) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, nullptr, 0, status);
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت ut;
    }
    ut = utext_setup(ut, 0, status);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    اگر (U_SUCCESS(*status)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->getBuffer();
        ut->chunkLength         = s->length();
        ut->chunkNativeStart    = 0;
        ut->chunkNativeLimit    = ut->chunkLength;
        ut->nativeIndexingLimit = ut->chunkLength;
    }
    بازگشت ut;
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


ایستا UText * U_CALLCONV
ucstrTextClone(UText *dest, ثابت UText * src, UBool deep, UErrorCode * status) {
    // First do a generic shallow clone.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    اگر (deep && U_SUCCESS(*status)) {
        U_ASSERT(utext_nativeLength(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_nativeLength(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        ثابت char16_t *srcStr = (ثابت char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * اندازه(char16_t));
        اگر (copyStr == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } وگرنه {
            int64_t i;
            برای (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    بازگشت dest;
}


ایستا تهی U_CALLCONV
ucstrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    اگر (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = nullptr;
    }
}



ایستا int64_t U_CALLCONV
ucstrTextLength(UText *ut) {
    اگر (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        ثابت char16_t  *str = (ثابت char16_t *)ut->context;
        برای (;;) {
            اگر (str[ut->chunkNativeLimit] == 0) {
                بشکن;
            }
            ut->chunkNativeLimit++;
        }
        ut->a = ut->chunkNativeLimit;
        ut->chunkLength = (int32_t)ut->chunkNativeLimit;
        ut->nativeIndexingLimit = ut->chunkLength;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
    }
    بازگشت ut->a;
}


ایستا UBool U_CALLCONV
ucstrTextAccess(UText *ut, int64_t index, UBool  forward) {
    ثابت char16_t *str   = (ثابت char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    اگر (index<0) {
        index = 0;
    } وگرنه اگر (index < ut->chunkNativeLimit) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_SET_CP_START(str, 0, index);
    } وگرنه اگر (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        index = ut->a;
    } وگرنه {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t scanLimit = (int32_t)index + 32;
        اگر ((index + 32)>INT32_MAX || (index + 32)<0 ) {   // note: int64 expression
            scanLimit = INT32_MAX;
        }

        int32_t chunkLimit = (int32_t)ut->chunkNativeLimit;
        برای (; chunkLimit<scanLimit; chunkLimit++) {
            اگر (str[chunkLimit] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunkLimit;
                ut->chunkLength = chunkLimit;
                ut->nativeIndexingLimit = chunkLimit;
                اگر (index >= chunkLimit) {
                    index = chunkLimit;
                } وگرنه {
                    U16_SET_CP_START(str, 0, index);
                }

                ut->chunkNativeLimit = chunkLimit;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
                برو breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_SET_CP_START(str, 0, index);
        اگر (chunkLimit == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunkLimit;
            ut->chunkLength = chunkLimit;
            ut->nativeIndexingLimit = chunkLimit;
            اگر (index > chunkLimit) {
                index = chunkLimit;
            }
            ut->chunkNativeLimit = chunkLimit;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE);
        } وگرنه {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            اگر (U16_IS_LEAD(str[chunkLimit-1])) {
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
    بازگشت retVal;
}



ایستا int32_t U_CALLCONV
ucstrTextExtract(UText *ut,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *pErrorCode)
{
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }
    اگر(destCapacity<0 || (dest==nullptr && destCapacity>0) || start>limit) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstrTextAccess(ut, start, درست);
    ثابت char16_t *s=ut->chunkContents;
    start32 = ut->chunkOffset;

    int32_t strLength=(int32_t)ut->a;
    اگر (strLength >= 0) {
        limit32 = pinIndex(limit, strLength);
    } وگرنه {
        limit32 = pinIndex(limit, INT32_MAX);
    }
    di = 0;
    برای (si=start32; si<limit32; si++) {
        اگر (strLength<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunkNativeLimit    = si;
            ut->chunkLength         = si;
            ut->nativeIndexingLimit = si;
            strLength               = si;
            limit32                 = si;
            بشکن;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        اگر (di<destCapacity) {
            // only store if there is space.
            dest[di] = s[si];
        } وگرنه {
            اگر (strLength>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                بشکن;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    اگر (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<strLength || strLength<0)  && U16_IS_TRAIL(s[si])))
    {
        اگر (di<destCapacity) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    اگر (si <= ut->chunkNativeLimit) {
        ut->chunkOffset = si;
    } وگرنه {
        ucstrTextAccess(ut, si, درست);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, destCapacity, di, pErrorCode);
    بازگشت di;
}

ایستا ثابت ساختار UTextFuncs ucstrFuncs =
{
    اندازه(UTextFuncs),
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

ایستا ثابت char16_t gEmptyUString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 ثابت char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UErrorCode* status) {
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر(s==nullptr && length==0) {
        s = gEmptyUString;
    }
    اگر (s==nullptr || length < -1 || length>INT32_MAX) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    ut = utext_setup(ut, 0, status);
    اگر (U_SUCCESS(*status)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        اگر (length==-1) {
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
    بازگشت ut;
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
ایستا تهی U_CALLCONV
charIterTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    delete ci;
    ut->r = nullptr;
}

ایستا int64_t U_CALLCONV
charIterTextLength(UText *ut) {
    بازگشت (int32_t)ut->a;
}

ایستا UBool U_CALLCONV
charIterTextAccess(UText *ut, int64_t index, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clippedIndex = (int32_t)index;
    اگر (clippedIndex<0) {
        clippedIndex=0;
    } وگرنه اگر (clippedIndex>=ut->a) {
        clippedIndex=(int32_t)ut->a;
    }
    int32_t neededIndex = clippedIndex;
    اگر (!forward && neededIndex>0) {
        // reverse iteration, want the position just before what was asked for.
        neededIndex--;
    } وگرنه اگر (forward && neededIndex==ut->a && neededIndex>0) {
        // Forward iteration, don't ask for something past the end of the text.
        neededIndex--;
    }

    // Find the native index of the start of the buffer containing what we want.
    neededIndex -= neededIndex % CIBufSize;

    char16_t *buf = nullptr;
    UBool  needChunkSetup = درست;
    صحیح    i;
    اگر (ut->chunkNativeStart == neededIndex) {
        // The buffer we want is already the current chunk.
        needChunkSetup = نادرست;
    } وگرنه اگر (ut->b == neededIndex) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } وگرنه اگر (ut->c == neededIndex) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } وگرنه {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        اگر (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->setIndex(neededIndex);
        برای (i=0; i<CIBufSize; i++) {
            buf[i] = ci->nextPostInc();
            اگر (i+neededIndex > ut->a) {
                بشکن;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    اگر (needChunkSetup) {
        ut->chunkContents = buf;
        ut->chunkLength   = CIBufSize;
        ut->chunkNativeStart = neededIndex;
        ut->chunkNativeLimit = neededIndex + CIBufSize;
        اگر (ut->chunkNativeLimit > ut->a) {
            ut->chunkNativeLimit = ut->a;
            ut->chunkLength  = (int32_t)(ut->chunkNativeLimit)-(int32_t)(ut->chunkNativeStart);
        }
        ut->nativeIndexingLimit = ut->chunkLength;
        U_ASSERT(ut->chunkOffset>=0 && ut->chunkOffset<=CIBufSize);
    }
    ut->chunkOffset = clippedIndex - (int32_t)ut->chunkNativeStart;
    UBool success = (forward? ut->chunkOffset<ut->chunkLength : ut->chunkOffset>0);
    بازگشت success;
}

ایستا UText * U_CALLCONV
charIterTextClone(UText *dest, ثابت UText *src, UBool deep, UErrorCode * status) {
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    اگر (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *status = U_UNSUPPORTED_ERROR;
        بازگشت nullptr;
    } وگرنه {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, status);
        اگر (U_FAILURE(*status)) {
            بازگشت dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_getNativeIndex((UText *)src);
        utext_setNativeIndex(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    بازگشت dest;
}

ایستا int32_t U_CALLCONV
charIterTextExtract(UText *ut,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *status)
{
    اگر(U_FAILURE(*status)) {
        بازگشت 0;
    }
    اگر(destCapacity<0 || (dest==nullptr && destCapacity>0) || start>limit) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
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
    وقتی (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_LENGTH(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        اگر (desti+len <= destCapacity) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copyLimit = srci+len;
        } وگرنه {
            desti += len;
            *status = U_BUFFER_OVERFLOW_ERROR;
        }
        srci += len;
    }

    charIterTextAccess(ut, copyLimit, درست);

    u_terminateUChars(dest, destCapacity, desti, status);
    بازگشت desti;
}

ایستا ثابت ساختار UTextFuncs charIterFuncs =
{
    اندازه(UTextFuncs),
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
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    اگر (ci->startIndex() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *status = U_UNSUPPORTED_ERROR;
        بازگشت nullptr;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBufSize * اندازه(char16_t);
    ut = utext_setup(ut, extraSpace, status);
    اگر (U_SUCCESS(*status)) {
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
    بازگشت ut;
}

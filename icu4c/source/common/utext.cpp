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

#समावेश <cstddef>

#समावेश "unicode/utypes.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/chariter.h"
#समावेश "unicode/utext.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "ustr_imp.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "uassert.h"
#समावेश "putilimp.h"

U_NAMESPACE_USE

#घोषणा I32_FLAG(bit_सूचकांक) ((int32_t)1<<(bit_सूचकांक))


अटल UBool
utext_access(UText *ut, int64_t सूचकांक, UBool forward) {
    वापस ut->pFuncs->access(ut, सूचकांक, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    अगर (delta > 0) {
        करो {
            अगर(ut->chunkOffset>=ut->chunk_लंबाई && !utext_access(ut, ut->chunkNativeLimit, true)) {
                वापस false;
            }
            c = ut->chunkContents[ut->chunkOffset];
            अगर (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                अगर (c == U_SENTINEL) {
                    वापस false;
                }
            } अन्यथा {
                ut->chunkOffset++;
            }
        } जबतक(--delta>0);

    } अन्यथा अगर (delta<0) {
        करो {
            अगर(ut->chunkOffset<=0 && !utext_access(ut, ut->chunkNativeStart, false)) {
                वापस false;
            }
            c = ut->chunkContents[ut->chunkOffset-1];
            अगर (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                अगर (c == U_SENTINEL) {
                    वापस false;
                }
            } अन्यथा {
                ut->chunkOffset--;
            }
        } जबतक(++delta<0);
    }

    वापस true;
}


U_CAPI int64_t U_EXPORT2
utext_native_लंबाई(UText *ut) {
    वापस ut->pFuncs->native_लंबाई(ut);
}


U_CAPI UBool U_EXPORT2
utext_is_लंबाई_Expensive(स्थिर UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE)) != 0;
    वापस r;
}


U_CAPI int64_t U_EXPORT2
utext_get_Native_सूचकांक(स्थिर UText *ut) {
    अगर(ut->chunkOffset <= ut->nativeIndexingLimit) {
        वापस ut->chunkNativeStart+ut->chunkOffset;
    } अन्यथा {
        वापस ut->pFuncs->mapOffsetToNative(ut);
    }
}


U_CAPI व्योम U_EXPORT2
utext_set_Native_सूचकांक(UText *ut, int64_t सूचकांक) {
    अगर(सूचकांक<ut->chunkNativeStart || सूचकांक>=ut->chunkNativeLimit) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, सूचकांक, true);
    } अन्यथा अगर((int32_t)(सूचकांक - ut->chunkNativeStart) <= ut->nativeIndexingLimit) {
        // utf-16 indexing.
        ut->chunkOffset=(int32_t)(सूचकांक-ut->chunkNativeStart);
    } अन्यथा {
         ut->chunkOffset=ut->pFuncs->map_Native_सूचकांक_To_UTF16(ut, सूचकांक);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    अगर (ut->chunkOffset<ut->chunk_लंबाई) {
        char16_t c= ut->chunkContents[ut->chunkOffset];
        अगर (U16_IS_TRAIL(c)) {
            अगर (ut->chunkOffset==0) {
                ut->pFuncs->access(ut, ut->chunkNativeStart, false);
            }
            अगर (ut->chunkOffset>0) {
                char16_t lead = ut->chunkContents[ut->chunkOffset-1];
                अगर (U16_IS_LEAD(lead)) {
                    ut->chunkOffset--;
                }
            }
        }
    }
}



U_CAPI int64_t U_EXPORT2
utext_get_Previous_Native_सूचकांक(UText *ut) {
    //
    //  Fast-path the common case.
    //     Common means current position is not at the beginning of a chunk
    //     and the preceding character is not supplementary.
    //
    int32_t i = ut->chunkOffset - 1;
    int64_t परिणाम;
    अगर (i >= 0) {
        char16_t c = ut->chunkContents[i];
        अगर (U16_IS_TRAIL(c) == false) {
            अगर (i <= ut->nativeIndexingLimit) {
                परिणाम = ut->chunkNativeStart + i;
            } अन्यथा {
                ut->chunkOffset = i;
                परिणाम = ut->pFuncs->mapOffsetToNative(ut);
                ut->chunkOffset++;
            }
            वापस परिणाम;
        }
    }

    // If at the start of text, simply return 0.
    अगर (ut->chunkOffset==0 && ut->chunkNativeStart==0) {
        वापस 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    परिणाम = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    वापस परिणाम;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    अगर (ut->chunkOffset==ut->chunk_लंबाई) {
        // Current position is just off the end of the chunk.
        अगर (ut->pFuncs->access(ut, ut->chunkNativeLimit, true) == false) {
            // Off the end of the text.
            वापस U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunkOffset];
    अगर (U16_IS_LEAD(c) == false) {
        // Normal, non-supplementary case.
        वापस c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    अगर ((ut->chunkOffset+1) < ut->chunk_लंबाई) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunkOffset+1];
    } अन्यथा {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunkNativeLimit;
        अगर (ut->pFuncs->access(ut, nativePosition, true)) {
            trail = ut->chunkContents[ut->chunkOffset];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, false);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunkOffset = ut->chunk_लंबाई - 1;
        अगर(!r) {
            वापस U_SENTINEL;
        }
    }

    अगर (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    वापस supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t native_सूचकांक) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    अगर (native_सूचकांक>=ut->chunkNativeStart && native_सूचकांक < ut->chunkNativeStart + ut->nativeIndexingLimit) {
        ut->chunkOffset = (int32_t)(native_सूचकांक - ut->chunkNativeStart);
        c = ut->chunkContents[ut->chunkOffset];
        अगर (U16_IS_SURROGATE(c) == false) {
            वापस c;
        }
    }


    utext_set_Native_सूचकांक(ut, native_सूचकांक);
    अगर (native_सूचकांक>=ut->chunkNativeStart && ut->chunkOffset<ut->chunk_लंबाई) {
        c = ut->chunkContents[ut->chunkOffset];
        अगर (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    वापस c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    अगर (ut->chunkOffset >= ut->chunk_लंबाई) {
        अगर (ut->pFuncs->access(ut, ut->chunkNativeLimit, true) == false) {
            वापस U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunkOffset++];
    अगर (U16_IS_LEAD(c) == false) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        वापस c;
    }

    अगर (ut->chunkOffset >= ut->chunk_लंबाई) {
        अगर (ut->pFuncs->access(ut, ut->chunkNativeLimit, true) == false) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            वापस c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunkOffset];
    अगर (U16_IS_TRAIL(trail) == false) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        वापस c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunkOffset++;   // move iteration position over the trail surrogate.
    वापस supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    अगर (ut->chunkOffset <= 0) {
        अगर (ut->pFuncs->access(ut, ut->chunkNativeStart, false) == false) {
            वापस U_SENTINEL;
        }
    }
    ut->chunkOffset--;
    c = ut->chunkContents[ut->chunkOffset];
    अगर (U16_IS_TRAIL(c) == false) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        वापस c;
    }

    अगर (ut->chunkOffset <= 0) {
        अगर (ut->pFuncs->access(ut, ut->chunkNativeStart, false) == false) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            वापस c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunkOffset-1];
    अगर (U16_IS_LEAD(lead) == false) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        वापस c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunkOffset--;   // move iteration position over the lead surrogate.
    वापस supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t सूचकांक) {
    UChar32       c      = U_SENTINEL;

    अगर(सूचकांक<ut->chunkNativeStart || सूचकांक>=ut->chunkNativeLimit) {
        // Desired position is outside of the current chunk.
        अगर(!ut->pFuncs->access(ut, सूचकांक, true)) {
            // no chunk available here
            वापस U_SENTINEL;
        }
    } अन्यथा अगर (सूचकांक - ut->chunkNativeStart  <= (int64_t)ut->nativeIndexingLimit) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunkOffset = (int32_t)(सूचकांक - ut->chunkNativeStart);
    } अन्यथा {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunkOffset = ut->pFuncs->map_Native_सूचकांक_To_UTF16(ut, सूचकांक);
    }

    c = ut->chunkContents[ut->chunkOffset++];
    अगर (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_set_Native_सूचकांक(ut, सूचकांक);
        c = utext_next32(ut);
    }
    वापस c;
}


U_CAPI UChar32 U_EXPORT2
utext_previous32From(UText *ut, int64_t सूचकांक) {
    //
    //  Return the character preceding the specified index.
    //  Leave the iteration position at the start of the character that was returned.
    //
    UChar32     c_पूर्व;    // The character preceding cCurr, which is what we will return.

    // Address the chunk containing the position preceding the incoming index
    // A tricky edge case:
    //   We try to test the requested native index against the chunkNativeStart to determine
    //    whether the character preceding the one at the index is in the current chunk.
    //    BUT, this test can fail with UTF-8 (or any other multibyte encoding), when the
    //    requested index is on something other than the first position of the first char.
    //
    अगर(सूचकांक<=ut->chunkNativeStart || सूचकांक>ut->chunkNativeLimit) {
        // Requested native index is outside of the current chunk.
        अगर(!ut->pFuncs->access(ut, सूचकांक, false)) {
            // no chunk available here
            वापस U_SENTINEL;
        }
    } अन्यथा अगर(सूचकांक - ut->chunkNativeStart <= (int64_t)ut->nativeIndexingLimit) {
        // Direct UTF-16 indexing.
        ut->chunkOffset = (int32_t)(सूचकांक - ut->chunkNativeStart);
    } अन्यथा {
        ut->chunkOffset=ut->pFuncs->map_Native_सूचकांक_To_UTF16(ut, सूचकांक);
        अगर (ut->chunkOffset==0 && !ut->pFuncs->access(ut, सूचकांक, false)) {
            // no chunk available here
            वापस U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunkOffset--;
    c_पूर्व = ut->chunkContents[ut->chunkOffset];

    अगर (U16_IS_SURROGATE(c_पूर्व)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_set_Native_सूचकांक(ut, सूचकांक);
        c_पूर्व = utext_previous32(ut);
    }
    वापस c_पूर्व;
}


U_CAPI int32_t U_EXPORT2
utext_extract(UText *ut,
             int64_t start, int64_t limit,
             char16_t *dest, int32_t destCapacity,
             UErrorCode *status) {
                 वापस ut->pFuncs->extract(ut, start, limit, dest, destCapacity, status);
             }



U_CAPI UBool U_EXPORT2
utext_equals(स्थिर UText *a, स्थिर UText *b) {
    अगर (a==nullptr || b==nullptr ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            वापस false;
    }

    अगर (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        वापस false;
    }

    अगर (a->context != b->context) {
        // Different sources (different strings)
        वापस false;
    }
    अगर (utext_get_Native_सूचकांक(a) != utext_get_Native_सूचकांक(b)) {
        // Different current position in the string.
        वापस false;
    }

    वापस true;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(स्थिर UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    वापस b;
}


U_CAPI व्योम U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_has_Meta_तथ्य(स्थिर UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_तथ्य)) != 0;
    वापस b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t nativeStart, int64_t nativeLimit,
             स्थिर char16_t *replacementText, int32_t replacement_लंबाई,
             UErrorCode *status)
{
    अगर (U_FAILURE(*status)) {
        वापस 0;
    }
    अगर ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *status = U_NO_WRITE_PERMISSION;
        वापस 0;
    }
    int32_t i = ut->pFuncs->replace(ut, nativeStart, nativeLimit, replacementText, replacement_लंबाई, status);
    वापस i;
}

U_CAPI व्योम U_EXPORT2
utext_copy(UText *ut,
          int64_t nativeStart, int64_t nativeLimit,
          int64_t dest_सूचकांक,
          UBool move,
          UErrorCode *status)
{
    अगर (U_FAILURE(*status)) {
        वापस;
    }
    अगर ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *status = U_NO_WRITE_PERMISSION;
        वापस;
    }
    ut->pFuncs->copy(ut, nativeStart, nativeLimit, dest_सूचकांक, move, status);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            स्थिर UText* src,
            UBool deep,
            UBool readOnly,
            UErrorCode* status) {
    अगर (U_FAILURE(*status)) {
        वापस dest;
    }
    UText *परिणाम = src->pFuncs->clone(dest, src, deep, status);
    अगर (U_FAILURE(*status)) {
        वापस परिणाम;
    }
    अगर (परिणाम == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस परिणाम;
    }
    अगर (readOnly) {
        utext_freeze(परिणाम);
    }
    वापस परिणाम;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
क्रमागत {
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

काष्ठा ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

अटल स्थिर UText emptyText = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_setup(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UErrorCode* status) {
    अगर (U_FAILURE(*status)) {
        वापस ut;
    }

    अगर (ut == nullptr) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = माप(UText);
        अगर (extraSpace > 0) {
            spaceRequired = माप(ExtendedUText) + extraSpace - माप(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        अगर (ut == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        } अन्यथा {
            *ut = emptyText;
            ut->flags |= UTEXT_HEAP_ALLOCATED;
            अगर (spaceRequired>0) {
                ut->extra_आकार = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } अन्यथा {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        अगर (ut->magic != UTEXT_MAGIC) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        अगर ((ut->flags & UTEXT_OPEN) && ut->pFuncs->बंद != nullptr)  {
            ut->pFuncs->बंद(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        अगर (extraSpace > ut->extra_आकार) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            अगर (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extra_आकार = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            अगर (ut->pExtra == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } अन्यथा {
                ut->extra_आकार = extraSpace;
                ut->flags |= UTEXT_EXTRA_HEAP_ALLOCATED;
            }
        }
    }
    अगर (U_SUCCESS(*status)) {
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
        ut->chunk_लंबाई         = 0;
        ut->chunkNativeStart    = 0;
        ut->chunkNativeLimit    = 0;
        ut->nativeIndexingLimit = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = nullptr;
        अगर (ut->pExtra!=nullptr && ut->extra_आकार>0)
            uprv_memset(ut->pExtra, 0, ut->extra_आकार);

    }
    वापस ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    अगर (ut==nullptr ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        वापस ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    अगर (ut->pFuncs->बंद != nullptr) {
        ut->pFuncs->बंद(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    अगर (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = nullptr;
        ut->flags &= ~UTEXT_EXTRA_HEAP_ALLOCATED;
        ut->extra_आकार = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = nullptr;

    अगर (ut->flags & UTEXT_HEAP_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = nullptr;
    }
    वापस ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
अटल व्योम
invalidateChunk(UText *ut) {
    ut->chunk_लंबाई = 0;
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
अटल int32_t
pin_सूचकांक(int64_t &सूचकांक, int64_t limit) {
    अगर (सूचकांक<0) {
        सूचकांक = 0;
    } अन्यथा अगर (सूचकांक > limit) {
        सूचकांक = limit;
    }
    वापस static_cast<int32_t>(सूचकांक);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
अटल व्योम adjustPointer(UText *dest, स्थिर व्योम **destPtr, स्थिर UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    अक्षर  *dptr = (अक्षर *)*destPtr;
    अक्षर  *dUText = (अक्षर *)dest;
    अक्षर  *sUText = (अक्षर *)src;

    अगर (dptr >= (अक्षर *)src->pExtra && dptr < ((अक्षर*)src->pExtra)+src->extra_आकार) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((अक्षर *)dest->pExtra) + (dptr - (अक्षर *)src->pExtra);
    } अन्यथा अगर (dptr>=sUText && dptr < sUText+src->आकार_Of_Struct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
अटल UText * U_CALLCONV
shallowTextClone(UText * dest, स्थिर UText * src, UErrorCode * status) {
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    int32_t  src_Extra_आकार = src->extra_आकार;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_setup(dest, src_Extra_आकार, status);
    अगर (U_FAILURE(*status)) {
        वापस dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    व्योम *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    पूर्णांक आकार_To_Copy = src->आकार_Of_Struct;
    अगर (आकार_To_Copy > dest->आकार_Of_Struct) {
        आकार_To_Copy = dest->आकार_Of_Struct;
    }
    uprv_memcpy(dest, src, आकार_To_Copy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    अगर (src_Extra_आकार > 0) {
        uprv_memcpy(dest->pExtra, src->pExtra, src_Extra_आकार);
    }

    //
    // Relocate any pointers in the target that refer to the UText itself
    //   to point to the cloned copy rather than the original source.
    //
    adjustPointer(dest, &dest->context, src);
    adjustPointer(dest, &dest->p, src);
    adjustPointer(dest, &dest->q, src);
    adjustPointer(dest, &dest->r, src);
    adjustPointer(dest, (स्थिर व्योम **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

    वापस dest;
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
क्रमागत { UTF8_TEXT_CHUNK_आकार=32 };

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

काष्ठा UTF8Buf {
    int32_t   bufNativeStart;                        // Native index of first char in char16_t buf
    int32_t   bufNativeLimit;                        // Native index following last char in buf.
    int32_t   bufStartIdx;                           // First filled position in buf.
    int32_t   bufLimitIdx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   toUCharsMapStart;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_TEXT_CHUNK_आकार+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   mapToNative[UTF8_TEXT_CHUNK_आकार+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   mapToUChars[UTF8_TEXT_CHUNK_आकार*3+6]; // Map native offset from bufNativeStart to
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
अटल int64_t U_CALLCONV
utf8_Text_लंबाई(UText *ut) {
    अगर (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        स्थिर अक्षर *r = (स्थिर अक्षर *)ut->context + ut->c;
        जबतक (*r != 0) {
            r++;
        }
        अगर ((r - (स्थिर अक्षर *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (स्थिर अक्षर *)ut->context);
        } अन्यथा {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE);
    }
    वापस ut->b;
}






अटल UBool U_CALLCONV
utf8TextAccess(UText *ut, int64_t सूचकांक, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    स्थिर uint8_t *s8=(स्थिर uint8_t *)ut->context;
    UTF8Buf *u8b = nullptr;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)सूचकांक;     // Requested index, trimmed to 32 bits.
    int32_t  map_सूचकांक = 0;
    अगर (सूचकांक<0) {
        ix=0;
    } अन्यथा अगर (सूचकांक > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    अगर (ix>length) {
        अगर (length>=0) {
            ix=length;
        } अन्यथा अगर (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            जबतक (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            अगर (s8[ut->c] == 0) {
                // We just found the actual length of the string.
                //  Trim the requested index back to that.
                ix     = ut->c;
                ut->b  = ut->c;
                length = ut->c;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    अगर (forward) {
        अगर (ix==ut->chunkNativeLimit) {
            // Check for normal sequential iteration cases first.
            अगर (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunkOffset = ut->chunk_लंबाई;
                वापस false;
            } अन्यथा {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                अगर (ix>=altB->bufNativeStart && ix<altB->bufNativeLimit) {
                    जाओ swapBuffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            अगर (ix>=u8b->bufNativeStart && ix<u8b->bufNativeLimit) {
                // Requested index is in the other buffer.
                जाओ swapBuffers;
            }
            अगर (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                अगर (ix == ut->chunkNativeLimit) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunkOffset = ut->chunk_लंबाई;
                    वापस false;
                }
                अगर (ix == u8b->bufNativeLimit) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    जाओ swapBuffersAndFail;
                }

                // Neither existing buffer extends to the end of the string.
                जाओ make_Stub_बफर;
            }

            अगर (ix<ut->chunkNativeStart || ix>=ut->chunkNativeLimit) {
                // Requested index is in neither buffer.
                जाओ fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            map_सूचकांक = ix - u8b->toUCharsMapStart;
            U_ASSERT(map_सूचकांक < (int32_t)माप(UTF8Buf::mapToUChars));
            ut->chunkOffset = u8b->mapToUChars[map_सूचकांक] - u8b->bufStartIdx;
            वापस true;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    अगर (ix==ut->chunkNativeStart) {
        // Check for normal sequential iteration cases first.
        अगर (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunkOffset = 0;
            वापस false;
        } अन्यथा {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            अगर (ix>altB->bufNativeStart && ix<=altB->bufNativeLimit) {
                जाओ swapBuffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    अगर (ix>u8b->bufNativeStart && ix<=u8b->bufNativeLimit) {
        // Requested index is in the other buffer.
        जाओ swapBuffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    अगर (ix==0) {
        अगर (u8b->bufNativeStart==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            जाओ swapBuffersAndFail;
        } अन्यथा {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            जाओ make_Stub_बफर;
        }
    }

    अगर (ix<=ut->chunkNativeStart || ix>ut->chunkNativeLimit) {
        // Requested index is in neither buffer.
        जाओ fillReverse;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    map_सूचकांक = ix - u8b->toUCharsMapStart;
    ut->chunkOffset = u8b->mapToUChars[map_सूचकांक] - u8b->bufStartIdx;
    अगर (ut->chunkOffset==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        वापस false;
    } अन्यथा {
        वापस true;
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
        ut->chunk_लंबाई         = u8b->bufLimitIdx - u8b->bufStartIdx;
        ut->chunkNativeStart    = u8b->bufNativeStart;
        ut->chunkNativeLimit    = u8b->bufNativeLimit;
        ut->nativeIndexingLimit = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->bufNativeStart);
        U_ASSERT(ix<=u8b->bufNativeLimit);
        map_सूचकांक = ix - u8b->toUCharsMapStart;
        U_ASSERT(map_सूचकांक>=0);
        U_ASSERT(map_सूचकांक<(int32_t)माप(u8b->mapToUChars));
        ut->chunkOffset = u8b->mapToUChars[map_सूचकांक] - u8b->bufStartIdx;

        वापस true;
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
    ut->chunk_लंबाई         = u8b->bufLimitIdx - u8b->bufStartIdx;
    ut->chunkNativeStart    = u8b->bufNativeStart;
    ut->chunkNativeLimit    = u8b->bufNativeLimit;
    ut->nativeIndexingLimit = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    अगर (ix==u8b->bufNativeLimit) {
        ut->chunkOffset = ut->chunk_लंबाई;
    } अन्यथा  {
        ut->chunkOffset = 0;
        U_ASSERT(ix == u8b->bufNativeStart);
    }
    वापस false;

make_Stub_बफर:
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
    जाओ swapBuffersAndFail;



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
        UBool   nulTerminated = false;
        अगर (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = true;
        }

        char16_t   *buf = u8b_swap->buf;
        uint8_t *mapToNative  = u8b_swap->mapToNative;
        uint8_t *mapToUChars  = u8b_swap->mapToUChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    seenNonAscii = false;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        जबतक (destIx<UTF8_TEXT_CHUNK_आकार) {
            c = s8[srcIx];
            अगर (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                mapToNative[destIx]    = (uint8_t)(srcIx - ix);
                mapToUChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } अन्यथा {
                // General case, handle everything.
                अगर (seenNonAscii == false) {
                    seenNonAscii = true;
                    u8b_swap->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_अग्र_OR_FFFD(s8, srcIx, strLen, c);
                अगर (c==0 && nulTerminated) {
                    srcIx--;
                    अवरोध;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                करो {
                    mapToNative[dIx++] = (uint8_t)(cIx - ix);
                } जबतक (dIx < destIx);

                करो {
                    mapToUChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } जबतक (cIx < srcIx);
            }
            अगर (srcIx>=strLen) {
                अवरोध;
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
        अगर (seenNonAscii == false) {
            u8b_swap->bufNILimit     = destIx;
        }
        u8b_swap->toUCharsMapStart   = u8b_swap->bufNativeStart;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunkOffset         = 0;
        ut->chunk_लंबाई         = u8b_swap->bufLimitIdx;
        ut->chunkNativeStart    = u8b_swap->bufNativeStart;
        ut->chunkNativeLimit    = u8b_swap->bufNativeLimit;
        ut->nativeIndexingLimit = u8b_swap->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        अगर (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            अगर (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE);
            }
        }
        वापस true;
    }


fillReverse:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        अगर (ix != ut->b) {
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
        int32_t  toUCharsMapStart = ix - माप(UTF8Buf::mapToUChars) + 1;
        // Note that toUCharsMapStart can be negative. Happens when the remaining
        // text from current position to the beginning is less than the buffer size.
        // + 1 because mapToUChars must have a slot at the end for the bufNativeLimit entry.
        int32_t  destIx = UTF8_TEXT_CHUNK_आकार+2;   // Start in the overflow region
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
        जबतक (destIx>2 && (srcIx - toUCharsMapStart > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            अगर (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(toUCharsMapStart <= srcIx);
                mapToUChars[srcIx - toUCharsMapStart] = (uint8_t)destIx;
                mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
            } अन्यथा {
                // General case, handle everything non-ASCII.

                int32_t  sIx      = srcIx;  // ix of last byte of multi-byte u8 char

                // Get the full character from the UTF8 string.
                //   use code derived from the macros in utf8.h
                //   Leaves srcIx pointing at the first byte of the UTF-8 char.
                //
                c=utf8_पूर्व_Char_Safe_Body(s8, 0, &srcIx, c, -3);
                // leaves srcIx at first byte of the multi-byte char.

                // Store the character in UTF-16 buffer.
                अगर (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                } अन्यथा {
                    buf[destIx]         = U16_TRAIL(c);
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                    buf[--destIx]       = U16_LEAD(c);
                    mapToNative[destIx] = (uint8_t)(srcIx - toUCharsMapStart);
                }

                // Fill in the map from native indexes to UChars buf index.
                करो {
                    mapToUChars[sIx-- - toUCharsMapStart] = (uint8_t)destIx;
                } जबतक (sIx >= srcIx);
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
        u8b_swap->bufLimitIdx        = UTF8_TEXT_CHUNK_आकार+2;
        u8b_swap->bufNILimit         = bufNILimit - u8b_swap->bufStartIdx;
        u8b_swap->toUCharsMapStart   = toUCharsMapStart;

        ut->chunkContents       = &buf[u8b_swap->bufStartIdx];
        ut->chunk_लंबाई         = u8b_swap->bufLimitIdx - u8b_swap->bufStartIdx;
        ut->chunkOffset         = ut->chunk_लंबाई;
        ut->chunkNativeStart    = u8b_swap->bufNativeStart;
        ut->chunkNativeLimit    = u8b_swap->bufNativeLimit;
        ut->nativeIndexingLimit = u8b_swap->bufNILimit;
        वापस true;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
अटल char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t destCapacity,
              int32_t *p_Dest_लंबाई,
              स्थिर अक्षर* src,
              int32_t src_लंबाई,        // required.  NUL terminated not supported.
              UErrorCode *pErrorCode
              )
{

    char16_t *pDest = dest;
    char16_t *pDestLimit = (dest!=nullptr)?(dest+destCapacity):nullptr;
    UChar32 ch=0;
    int32_t सूचकांक = 0;
    int32_t req_लंबाई = 0;
    uint8_t* pSrc = (uint8_t*) src;


    जबतक((सूचकांक < src_लंबाई)&&(pDest<pDestLimit)){
        ch = pSrc[सूचकांक++];
        अगर(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }अन्यथा{
            ch=utf8_अग्र_Char_Safe_Body(pSrc, &सूचकांक, src_लंबाई, ch, -3);
            अगर(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }अन्यथा{
                *(pDest++)=U16_LEAD(ch);
                अगर(pDest<pDestLimit){
                    *(pDest++)=U16_TRAIL(ch);
                }अन्यथा{
                    req_लंबाई++;
                    अवरोध;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    जबतक(सूचकांक < src_लंबाई){
        ch = pSrc[सूचकांक++];
        अगर(ch <= 0x7f){
            req_लंबाई++;
        }अन्यथा{
            ch=utf8_अग्र_Char_Safe_Body(pSrc, &सूचकांक, src_लंबाई, ch, -3);
            req_लंबाई+=U16_लंबाई(ch);
        }
    }

    req_लंबाई+=(int32_t)(pDest - dest);

    अगर(p_Dest_लंबाई){
        *p_Dest_लंबाई = req_लंबाई;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,destCapacity,req_लंबाई,pErrorCode);

    वापस dest;
}



अटल int32_t U_CALLCONV
utf8TextExtract(UText *ut,
                int64_t start, int64_t limit,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pin_सूचकांक(start, length);
    int32_t  limit32 = pin_सूचकांक(limit, length);

    अगर(start32>limit32) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    स्थिर uint8_t *buf = (स्थिर uint8_t *)ut->context;
    पूर्णांक i;
    अगर (start32 < ut->chunkNativeLimit) {
        क्रम (i=0; i<3; i++) {
            अगर (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                अवरोध;
            }
            start32--;
        }
    }

    अगर (limit32 < ut->chunkNativeLimit) {
        क्रम (i=0; i<3; i++) {
            अगर (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                अवरोध;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t dest_लंबाई=0;
    utext_strFromUTF8(dest, destCapacity, &dest_लंबाई,
                    (स्थिर अक्षर *)ut->context+start32, limit32-start32,
                    pErrorCode);
    utf8TextAccess(ut, limit32, true);
    वापस dest_लंबाई;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
अटल int64_t U_CALLCONV
utf8TextMapOffsetToNative(स्थिर UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunkOffset>ut->nativeIndexingLimit && ut->chunkOffset<=ut->chunk_लंबाई);
    int32_t nativeOffset = u8b->mapToNative[ut->chunkOffset + u8b->bufStartIdx] + u8b->toUCharsMapStart;
    U_ASSERT(nativeOffset >= ut->chunkNativeStart && nativeOffset <= ut->chunkNativeLimit);
    वापस nativeOffset;
}

//
// Map a native index to the corresponding chunk offset
//
अटल int32_t U_CALLCONV
utf8_Text_Map_सूचकांक_To_UTF16(स्थिर UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t सूचकांक = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(सूचकांक>=ut->chunkNativeStart+ut->nativeIndexingLimit);
    U_ASSERT(सूचकांक<=ut->chunkNativeLimit);
    int32_t map_सूचकांक = सूचकांक - u8b->toUCharsMapStart;
    U_ASSERT(map_सूचकांक < (int32_t)माप(UTF8Buf::mapToUChars));
    int32_t offset = u8b->mapToUChars[map_सूचकांक] - u8b->bufStartIdx;
    U_ASSERT(offset>=0 && offset<=ut->chunk_लंबाई);
    वापस offset;
}

अटल UText * U_CALLCONV
utf8TextClone(UText *dest, स्थिर UText *src, UBool deep, UErrorCode *status)
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
    अगर (deep && U_SUCCESS(*status)) {
        int32_t  len = (int32_t)utext_native_लंबाई((UText *)src);
        अक्षर *copyStr = (अक्षर *)uprv_malloc(len+1);
        अगर (copyStr == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    वापस dest;
}


अटल व्योम U_CALLCONV
utf8TextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    अगर (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        अक्षर *s = (अक्षर *)ut->context;
        uprv_free(s);
        ut->context = nullptr;
    }
}

U_CDECL_END


अटल स्थिर काष्ठा UTextFuncs utf8Funcs =
{
    माप(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8TextClone,
    utf8_Text_लंबाई,
    utf8TextAccess,
    utf8TextExtract,
    nullptr,                /* replace*/
    nullptr,                /* copy   */
    utf8TextMapOffsetToNative,
    utf8_Text_Map_सूचकांक_To_UTF16,
    utf8TextClose,
    nullptr,                // spare 1
    nullptr,                // spare 2
    nullptr                 // spare 3
};


अटल स्थिर अक्षर gEmptyString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               स्थिर अक्षर* s U_LIFETIME_BOUND,
               int64_t length,
               UErrorCode* status) {
    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर(s==nullptr && length==0) {
        s = gEmptyString;
    }

    अगर(s==nullptr || length<-1 || length>INT32_MAX) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    ut = utext_setup(ut, माप(UTF8Buf) * 2, status);
    अगर (U_FAILURE(*status)) {
        वापस ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    अगर (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (अक्षर *)ut->pExtra + माप(UTF8Buf);
    वापस ut;

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
क्रमागत { REP_TEXT_CHUNK_आकार=10 };

काष्ठा ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_TEXT_CHUNK_आकार+1];
};


U_CDECL_BEGIN

अटल UText * U_CALLCONV
repTextClone(UText *dest, स्थिर UText *src, UBool deep, UErrorCode *status) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    अगर (deep && U_SUCCESS(*status)) {
        स्थिर Replaceable *replSrc = (स्थिर Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    वापस dest;
}


अटल व्योम U_CALLCONV
repTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    अगर (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        Replaceable *rep = (Replaceable *)ut->context;
        मिटाओ rep;
        ut->context = nullptr;
    }
}


अटल int64_t U_CALLCONV
rep_Text_लंबाई(UText *ut) {
    स्थिर Replaceable *replSrc = (स्थिर Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    वापस len;
}


अटल UBool U_CALLCONV
repTextAccess(UText *ut, int64_t सूचकांक, UBool forward) {
    स्थिर Replaceable *rep=(स्थिर Replaceable *)ut->context;
    int32_t length=rep->length();   // Full length of the input text (bigger than a chunk)

    // clip the requested index to the limits of the text.
    int32_t index32 = pin_सूचकांक(सूचकांक, length);
    U_ASSERT(सूचकांक<=INT32_MAX);


    /*
     * Compute start/limit boundaries around index, for a segment of text
     * to be extracted.
     * To allow for the possibility that our user gave an index to the trailing
     * half of a surrogate pair, we must request one extra preceding char16_t when
     * going in the forward direction.  This will ensure that the buffer has the
     * entire code point at the specified index.
     */
    अगर(forward) {

        अगर (index32>=ut->chunkNativeStart && index32<ut->chunkNativeLimit) {
            // Buffer already contains the requested position.
            ut->chunkOffset = (int32_t)(सूचकांक - ut->chunkNativeStart);
            वापस true;
        }
        अगर (index32>=length && ut->chunkNativeLimit==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunkOffset = length - (int32_t)ut->chunkNativeStart;
            वापस false;
        }

        ut->chunkNativeLimit = सूचकांक + REP_TEXT_CHUNK_आकार - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        अगर(ut->chunkNativeLimit > length) {
            ut->chunkNativeLimit = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunkNativeStart = ut->chunkNativeLimit - REP_TEXT_CHUNK_आकार;
        अगर(ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }
    } अन्यथा {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        अगर (index32>ut->chunkNativeStart && index32<=ut->chunkNativeLimit) {
            // Requested position already in buffer.
            ut->chunkOffset = index32 - (int32_t)ut->chunkNativeStart;
            वापस true;
        }
        अगर (index32==0 && ut->chunkNativeStart==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunkOffset = 0;
            वापस false;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunkNativeStart = index32 + 1 - REP_TEXT_CHUNK_आकार;
        अगर (ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }

        ut->chunkNativeLimit = index32 + 1;
        अगर (ut->chunkNativeLimit > length) {
            ut->chunkNativeLimit = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString बफर(ex->s, 0 /*buffer length*/, REP_TEXT_CHUNK_आकार /*buffer capacity*/);
    rep->extractBetween((int32_t)ut->chunkNativeStart, (int32_t)ut->chunkNativeLimit, बफर);

    ut->chunkContents  = ex->s;
    ut->chunk_लंबाई    = (int32_t)(ut->chunkNativeLimit - ut->chunkNativeStart);
    ut->chunkOffset    = (int32_t)(index32 - ut->chunkNativeStart);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    अगर (ut->chunkNativeLimit < length &&
        U16_IS_LEAD(ex->s[ut->chunk_लंबाई-1])) {
            ut->chunk_लंबाई--;
            ut->chunkNativeLimit--;
            अगर (ut->chunkOffset > ut->chunk_लंबाई) {
                ut->chunkOffset = ut->chunk_लंबाई;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    अगर(ut->chunkNativeStart>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunkNativeStart);
        --(ut->chunk_लंबाई);
        --(ut->chunkOffset);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_SET_CP_START(ut->chunkContents, 0, ut->chunkOffset);

    // Use fast indexing for get/setNativeIndex()
    ut->nativeIndexingLimit = ut->chunk_लंबाई;

    वापस true;
}



अटल int32_t U_CALLCONV
repTextExtract(UText *ut,
               int64_t start, int64_t limit,
               char16_t *dest, int32_t destCapacity,
               UErrorCode *status) {
    स्थिर Replaceable *rep=(स्थिर Replaceable *)ut->context;
    int32_t  length=rep->length();

    अगर(U_FAILURE(*status)) {
        वापस 0;
    }
    अगर(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
    }
    अगर(start>limit) {
        *status=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    int32_t  start32 = pin_सूचकांक(start, length);
    int32_t  limit32 = pin_सूचकांक(limit, length);

    // adjust start, limit if they point to trail half of surrogates
    अगर (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    अगर (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    अगर(length>destCapacity) {
        limit32 = start32 + destCapacity;
    }
    UnicodeString बफर(dest, 0, destCapacity); // writable alias
    rep->extractBetween(start32, limit32, बफर);
    repTextAccess(ut, limit32, true);

    वापस u_terminateUChars(dest, destCapacity, length, status);
}

अटल int32_t U_CALLCONV
repTextReplace(UText *ut,
               int64_t start, int64_t limit,
               स्थिर char16_t *src, int32_t length,
               UErrorCode *status) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t old_लंबाई;

    अगर(U_FAILURE(*status)) {
        वापस 0;
    }
    अगर(src==nullptr && length!=0) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    old_लंबाई=rep->length(); // will subtract from new length
    अगर(start>limit ) {
        *status=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    int32_t start32 = pin_सूचकांक(start, old_लंबाई);
    int32_t limit32 = pin_सूचकांक(limit, old_लंबाई);

    // Snap start & limit to code point boundaries.
    अगर (start32<old_लंबाई && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    अगर (limit32<old_लंबाई && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t new_लंबाई = rep->length();
    int32_t लंबाई_Delta = new_लंबाई - old_लंबाई;

    // Is the UText chunk buffer OK?
    अगर (ut->chunkNativeLimit > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t new_सूचकांक_Pos = limit32 + लंबाई_Delta;
    repTextAccess(ut, new_सूचकांक_Pos, true);

    वापस लंबाई_Delta;
}


अटल व्योम U_CALLCONV
repTextCopy(UText *ut,
                int64_t start, int64_t limit,
                int64_t dest_सूचकांक,
                UBool move,
                UErrorCode *status)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    अगर(U_FAILURE(*status)) {
        वापस;
    }
    अगर (start>limit || (start<dest_सूचकांक && dest_सूचकांक<limit))
    {
        *status=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }

    int32_t start32     = pin_सूचकांक(start, length);
    int32_t limit32     = pin_सूचकांक(limit, length);
    int32_t destIndex32 = pin_सूचकांक(dest_सूचकांक, length);

    // TODO:  snap input parameters to code point boundaries.

    अगर(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t seg_लंबाई=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        अगर(destIndex32<start32) {
            start32+=seg_लंबाई;
            limit32+=seg_लंबाई;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } अन्यथा {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t first_Affected_सूचकांक = destIndex32;
    अगर (move && start32<first_Affected_सूचकांक) {
        first_Affected_सूचकांक = start32;
    }
    अगर (first_Affected_सूचकांक < ut->chunkNativeLimit) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  native_Iter_सूचकांक = destIndex32 + limit32 - start32;
    अगर (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        native_Iter_सूचकांक = destIndex32;
    }

    // Set position, reload chunk if needed.
    repTextAccess(ut, native_Iter_सूचकांक, true);
}

अटल स्थिर काष्ठा UTextFuncs repFuncs =
{
    माप(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    repTextClone,
    rep_Text_लंबाई,
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
    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर(rep==nullptr) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    ut = utext_setup(ut, माप(ReplExtra), status);
    अगर(U_FAILURE(*status)) {
        वापस ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    अगर(rep->has_Meta_तथ्य()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_तथ्य);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    वापस ut;
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


अटल UText * U_CALLCONV
unistrTextClone(UText *dest, स्थिर UText *src, UBool deep, UErrorCode *status) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    अगर (deep && U_SUCCESS(*status)) {
        स्थिर UnicodeString *srcString = (स्थिर UnicodeString *)src->context;
        dest->context = नया UnicodeString(*srcString);
        dest->chunkContents = ((स्थिर UnicodeString *)dest->context)->get_बफर();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    वापस dest;
}

अटल व्योम U_CALLCONV
unistrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    अगर (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        मिटाओ str;
        ut->context = nullptr;
    }
}


अटल int64_t U_CALLCONV
unistr_Text_लंबाई(UText *t) {
    वापस ((स्थिर UnicodeString *)t->context)->length();
}


अटल UBool U_CALLCONV
unistrTextAccess(UText *ut, int64_t सूचकांक, UBool  forward) {
    int32_t length  = ut->chunk_लंबाई;
    ut->chunkOffset = pin_सूचकांक(सूचकांक, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && सूचकांक<length) || (!forward && सूचकांक>0);
    वापस retVal;
}



अटल int32_t U_CALLCONV
unistrTextExtract(UText *t,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *pErrorCode) {
    स्थिर UnicodeString *us=(स्थिर UnicodeString *)t->context;
    int32_t length=us->length();

    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(destCapacity<0 || (dest==nullptr && destCapacity>0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
    अगर(start<0 || start>limit) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    int32_t start32 = start<length ? us->getChar32Start((int32_t)start) : length;
    int32_t limit32 = limit<length ? us->getChar32Start((int32_t)limit) : length;

    length=limit32-start32;
    अगर (destCapacity>0 && dest!=nullptr) {
        int32_t trimmed_लंबाई = length;
        अगर(trimmed_लंबाई>destCapacity) {
            trimmed_लंबाई=destCapacity;
        }
        us->extract(start32, trimmed_लंबाई, dest);
        t->chunkOffset = start32+trimmed_लंबाई;
    } अन्यथा {
        t->chunkOffset = start32;
    }
    u_terminateUChars(dest, destCapacity, length, pErrorCode);
    वापस length;
}

अटल int32_t U_CALLCONV
unistrTextReplace(UText *ut,
                  int64_t start, int64_t limit,
                  स्थिर char16_t *src, int32_t length,
                  UErrorCode *pErrorCode) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t old_लंबाई;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(src==nullptr && length!=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
    अगर(start>limit) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }
    old_लंबाई=us->length();
    int32_t start32 = pin_सूचकांक(start, old_लंबाई);
    int32_t limit32 = pin_सूचकांक(limit, old_लंबाई);
    अगर (start32 < old_लंबाई) {
        start32 = us->getChar32Start(start32);
    }
    अगर (limit32 < old_लंबाई) {
        limit32 = us->getChar32Start(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t new_लंबाई = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->get_बफर();
    ut->chunk_लंबाई      = new_लंबाई;
    ut->chunkNativeLimit = new_लंबाई;
    ut->nativeIndexingLimit = new_लंबाई;

    // Set iteration position to the point just following the newly inserted text.
    int32_t लंबाई_Delta = new_लंबाई - old_लंबाई;
    ut->chunkOffset = limit32 + लंबाई_Delta;

    वापस लंबाई_Delta;
}

अटल व्योम U_CALLCONV
unistrTextCopy(UText *ut,
               int64_t start, int64_t limit,
               int64_t dest_सूचकांक,
               UBool move,
               UErrorCode *pErrorCode) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    int32_t start32 = pin_सूचकांक(start, length);
    int32_t limit32 = pin_सूचकांक(limit, length);
    int32_t destIndex32 = pin_सूचकांक(dest_सूचकांक, length);

    अगर( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }

    अगर(move) {
        // move: copy to destIndex, then remove original
        int32_t seg_लंबाई=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        अगर(destIndex32<start32) {
            start32+=seg_लंबाई;
        }
        us->हटाओ(start32, seg_लंबाई);
    } अन्यथा {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->get_बफर();
    अगर (move==false) {
        // copy operation, string length grows
        ut->chunk_लंबाई += limit32-start32;
        ut->chunkNativeLimit = ut->chunk_लंबाई;
        ut->nativeIndexingLimit = ut->chunk_लंबाई;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunkOffset = destIndex32+limit32-start32;
    अगर (move && destIndex32>start32) {
        ut->chunkOffset = destIndex32;
    }

}

अटल स्थिर काष्ठा UTextFuncs unistrFuncs =
{
    माप(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistrTextClone,
    unistr_Text_लंबाई,
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
    अगर (U_SUCCESS(*status)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    वापस ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             स्थिर UnicodeString* s U_LIFETIME_BOUND,
                             UErrorCode* status) {
    अगर (U_SUCCESS(*status) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, nullptr, 0, status);
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस ut;
    }
    ut = utext_setup(ut, 0, status);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    अगर (U_SUCCESS(*status)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->get_बफर();
        ut->chunk_लंबाई         = s->length();
        ut->chunkNativeStart    = 0;
        ut->chunkNativeLimit    = ut->chunk_लंबाई;
        ut->nativeIndexingLimit = ut->chunk_लंबाई;
    }
    वापस ut;
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


अटल UText * U_CALLCONV
ucstrTextClone(UText *dest, स्थिर UText * src, UBool deep, UErrorCode * status) {
    // First do a generic shallow clone.
    dest = shallowTextClone(dest, src, status);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    अगर (deep && U_SUCCESS(*status)) {
        U_ASSERT(utext_native_लंबाई(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_native_लंबाई(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        स्थिर char16_t *srcStr = (स्थिर char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * माप(char16_t));
        अगर (copyStr == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            int64_t i;
            क्रम (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    वापस dest;
}


अटल व्योम U_CALLCONV
ucstrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    अगर (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = nullptr;
    }
}



अटल int64_t U_CALLCONV
ucstr_Text_लंबाई(UText *ut) {
    अगर (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        स्थिर char16_t  *str = (स्थिर char16_t *)ut->context;
        क्रम (;;) {
            अगर (str[ut->chunkNativeLimit] == 0) {
                अवरोध;
            }
            ut->chunkNativeLimit++;
        }
        ut->a = ut->chunkNativeLimit;
        ut->chunk_लंबाई = (int32_t)ut->chunkNativeLimit;
        ut->nativeIndexingLimit = ut->chunk_लंबाई;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE);
    }
    वापस ut->a;
}


अटल UBool U_CALLCONV
ucstrTextAccess(UText *ut, int64_t सूचकांक, UBool  forward) {
    स्थिर char16_t *str   = (स्थिर char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    अगर (सूचकांक<0) {
        सूचकांक = 0;
    } अन्यथा अगर (सूचकांक < ut->chunkNativeLimit) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_SET_CP_START(str, 0, सूचकांक);
    } अन्यथा अगर (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        सूचकांक = ut->a;
    } अन्यथा {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t scanLimit = (int32_t)सूचकांक + 32;
        अगर ((सूचकांक + 32)>INT32_MAX || (सूचकांक + 32)<0 ) {   // note: int64 expression
            scanLimit = INT32_MAX;
        }

        int32_t chunkLimit = (int32_t)ut->chunkNativeLimit;
        क्रम (; chunkLimit<scanLimit; chunkLimit++) {
            अगर (str[chunkLimit] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunkLimit;
                ut->chunk_लंबाई = chunkLimit;
                ut->nativeIndexingLimit = chunkLimit;
                अगर (सूचकांक >= chunkLimit) {
                    सूचकांक = chunkLimit;
                } अन्यथा {
                    U16_SET_CP_START(str, 0, सूचकांक);
                }

                ut->chunkNativeLimit = chunkLimit;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE);
                जाओ breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_SET_CP_START(str, 0, सूचकांक);
        अगर (chunkLimit == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunkLimit;
            ut->chunk_लंबाई = chunkLimit;
            ut->nativeIndexingLimit = chunkLimit;
            अगर (सूचकांक > chunkLimit) {
                सूचकांक = chunkLimit;
            }
            ut->chunkNativeLimit = chunkLimit;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE);
        } अन्यथा {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            अगर (U16_IS_LEAD(str[chunkLimit-1])) {
                --chunkLimit;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunkNativeLimit = chunkLimit;
            ut->nativeIndexingLimit = chunkLimit;
            ut->chunk_लंबाई = chunkLimit;
        }

    }
breakout:
    U_ASSERT(सूचकांक<=INT32_MAX);
    ut->chunkOffset = (int32_t)सूचकांक;

    // Check whether request is at the start or end
    UBool retVal = (forward && सूचकांक<ut->chunkNativeLimit) || (!forward && सूचकांक>0);
    वापस retVal;
}



अटल int32_t U_CALLCONV
ucstrTextExtract(UText *ut,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *pErrorCode)
{
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(destCapacity<0 || (dest==nullptr && destCapacity>0) || start>limit) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstrTextAccess(ut, start, true);
    स्थिर char16_t *s=ut->chunkContents;
    start32 = ut->chunkOffset;

    int32_t str_लंबाई=(int32_t)ut->a;
    अगर (str_लंबाई >= 0) {
        limit32 = pin_सूचकांक(limit, str_लंबाई);
    } अन्यथा {
        limit32 = pin_सूचकांक(limit, INT32_MAX);
    }
    di = 0;
    क्रम (si=start32; si<limit32; si++) {
        अगर (str_लंबाई<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunkNativeLimit    = si;
            ut->chunk_लंबाई         = si;
            ut->nativeIndexingLimit = si;
            str_लंबाई               = si;
            limit32                 = si;
            अवरोध;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        अगर (di<destCapacity) {
            // only store if there is space.
            dest[di] = s[si];
        } अन्यथा {
            अगर (str_लंबाई>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                अवरोध;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    अगर (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<str_लंबाई || str_लंबाई<0)  && U16_IS_TRAIL(s[si])))
    {
        अगर (di<destCapacity) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    अगर (si <= ut->chunkNativeLimit) {
        ut->chunkOffset = si;
    } अन्यथा {
        ucstrTextAccess(ut, si, true);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, destCapacity, di, pErrorCode);
    वापस di;
}

अटल स्थिर काष्ठा UTextFuncs ucstrFuncs =
{
    माप(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstrTextClone,
    ucstr_Text_लंबाई,
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

अटल स्थिर char16_t gEmptyUString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 स्थिर char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UErrorCode* status) {
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर(s==nullptr && length==0) {
        s = gEmptyUString;
    }
    अगर (s==nullptr || length < -1 || length>INT32_MAX) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    ut = utext_setup(ut, 0, status);
    अगर (U_SUCCESS(*status)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        अगर (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_लंबाई_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunkNativeStart     = 0;
        ut->chunkNativeLimit     = length>=0? length : 0;
        ut->chunk_लंबाई          = (int32_t)ut->chunkNativeLimit;
        ut->chunkOffset          = 0;
        ut->nativeIndexingLimit  = ut->chunk_लंबाई;
    }
    वापस ut;
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
#घोषणा CIBuf_आकार 16

U_CDECL_BEGIN
अटल व्योम U_CALLCONV
charIterTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    मिटाओ ci;
    ut->r = nullptr;
}

अटल int64_t U_CALLCONV
char_Iter_Text_लंबाई(UText *ut) {
    वापस (int32_t)ut->a;
}

अटल UBool U_CALLCONV
charIterTextAccess(UText *ut, int64_t सूचकांक, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clipped_सूचकांक = (int32_t)सूचकांक;
    अगर (clipped_सूचकांक<0) {
        clipped_सूचकांक=0;
    } अन्यथा अगर (clipped_सूचकांक>=ut->a) {
        clipped_सूचकांक=(int32_t)ut->a;
    }
    int32_t needed_सूचकांक = clipped_सूचकांक;
    अगर (!forward && needed_सूचकांक>0) {
        // reverse iteration, want the position just before what was asked for.
        needed_सूचकांक--;
    } अन्यथा अगर (forward && needed_सूचकांक==ut->a && needed_सूचकांक>0) {
        // Forward iteration, don't ask for something past the end of the text.
        needed_सूचकांक--;
    }

    // Find the native index of the start of the buffer containing what we want.
    needed_सूचकांक -= needed_सूचकांक % CIBuf_आकार;

    char16_t *buf = nullptr;
    UBool  needChunkSetup = true;
    पूर्णांक    i;
    अगर (ut->chunkNativeStart == needed_सूचकांक) {
        // The buffer we want is already the current chunk.
        needChunkSetup = false;
    } अन्यथा अगर (ut->b == needed_सूचकांक) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } अन्यथा अगर (ut->c == needed_सूचकांक) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } अन्यथा {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        अगर (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->set_सूचकांक(needed_सूचकांक);
        क्रम (i=0; i<CIBuf_आकार; i++) {
            buf[i] = ci->अग्र_Post_Inc();
            अगर (i+needed_सूचकांक > ut->a) {
                अवरोध;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    अगर (needChunkSetup) {
        ut->chunkContents = buf;
        ut->chunk_लंबाई   = CIBuf_आकार;
        ut->chunkNativeStart = needed_सूचकांक;
        ut->chunkNativeLimit = needed_सूचकांक + CIBuf_आकार;
        अगर (ut->chunkNativeLimit > ut->a) {
            ut->chunkNativeLimit = ut->a;
            ut->chunk_लंबाई  = (int32_t)(ut->chunkNativeLimit)-(int32_t)(ut->chunkNativeStart);
        }
        ut->nativeIndexingLimit = ut->chunk_लंबाई;
        U_ASSERT(ut->chunkOffset>=0 && ut->chunkOffset<=CIBuf_आकार);
    }
    ut->chunkOffset = clipped_सूचकांक - (int32_t)ut->chunkNativeStart;
    UBool success = (forward? ut->chunkOffset<ut->chunk_लंबाई : ut->chunkOffset>0);
    वापस success;
}

अटल UText * U_CALLCONV
charIterTextClone(UText *dest, स्थिर UText *src, UBool deep, UErrorCode * status) {
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }

    अगर (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *status = U_UNSUPPORTED_ERROR;
        वापस nullptr;
    } अन्यथा {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, status);
        अगर (U_FAILURE(*status)) {
            वापस dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_get_Native_सूचकांक((UText *)src);
        utext_set_Native_सूचकांक(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    वापस dest;
}

अटल int32_t U_CALLCONV
charIterTextExtract(UText *ut,
                  int64_t start, int64_t limit,
                  char16_t *dest, int32_t destCapacity,
                  UErrorCode *status)
{
    अगर(U_FAILURE(*status)) {
        वापस 0;
    }
    अगर(destCapacity<0 || (dest==nullptr && destCapacity>0) || start>limit) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pin_सूचकांक(start, length);
    int32_t  limit32 = pin_सूचकांक(limit, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copyLimit;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->setIndex32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->get_सूचकांक();
    copyLimit = srci;
    जबतक (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_लंबाई(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        अगर (desti+len <= destCapacity) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copyLimit = srci+len;
        } अन्यथा {
            desti += len;
            *status = U_बफर_OVERFLOW_ERROR;
        }
        srci += len;
    }

    charIterTextAccess(ut, copyLimit, true);

    u_terminateUChars(dest, destCapacity, desti, status);
    वापस desti;
}

अटल स्थिर काष्ठा UTextFuncs charIterFuncs =
{
    माप(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    charIterTextClone,
    char_Iter_Text_लंबाई,
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
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }

    अगर (ci->start_सूचकांक() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *status = U_UNSUPPORTED_ERROR;
        वापस nullptr;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBuf_आकार * माप(char16_t);
    ut = utext_setup(ut, extraSpace, status);
    अगर (U_SUCCESS(*status)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->end_सूचकांक();        // Length of text
        ut->p                    = ut->pExtra;            // First buffer
        ut->b                    = -1;                    // Native index of first buffer contents
        ut->q                    = (char16_t*)ut->pExtra+CIBuf_आकार;  // Second buffer
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
        ut->chunk_लंबाई          = 0;
        ut->nativeIndexingLimit  = ut->chunkOffset;  // enables native indexing
    }
    वापस ut;
}

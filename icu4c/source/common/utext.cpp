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

#अन्तर्भाव <cstddef>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/chariter.h"
#अन्तर्भाव "unicode/utext.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "putilimp.h"

U_NAMESPACE_USE

#परिभाषय I32_FLAG(bit_सूचकांक) ((int32_t)1<<(bit_सूचकांक))


स्थिर UBool
utext_access(UText *ut, int64_t सूचकांक, UBool forward) {
    प्रत्यागम ut->pFuncs->access(ut, सूचकांक, forward);
}



U_CAPI UBool U_EXPORT2
utext_moveIndex32(UText *ut, int32_t delta) {
    UChar32  c;
    यदि (delta > 0) {
        कुरु {
            यदि(ut->chunk_विस्थापनम्>=ut->chunk_दीर्घता && !utext_access(ut, ut->chunk_Native_सीमा, सत्य)) {
                प्रत्यागम असत्य;
            }
            c = ut->chunkContents[ut->chunk_विस्थापनम्];
            यदि (U16_IS_SURROGATE(c)) {
                c = utext_next32(ut);
                यदि (c == U_SENTINEL) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                ut->chunk_विस्थापनम्++;
            }
        } यावत्(--delta>0);

    } अन्यथा यदि (delta<0) {
        कुरु {
            यदि(ut->chunk_विस्थापनम्<=0 && !utext_access(ut, ut->chunkNativeStart, असत्य)) {
                प्रत्यागम असत्य;
            }
            c = ut->chunkContents[ut->chunk_विस्थापनम्-1];
            यदि (U16_IS_SURROGATE(c)) {
                c = utext_previous32(ut);
                यदि (c == U_SENTINEL) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                ut->chunk_विस्थापनम्--;
            }
        } यावत्(++delta<0);
    }

    प्रत्यागम सत्य;
}


U_CAPI int64_t U_EXPORT2
utext_native_दीर्घता(UText *ut) {
    प्रत्यागम ut->pFuncs->native_दीर्घता(ut);
}


U_CAPI UBool U_EXPORT2
utext_is_दीर्घता_Expensive(ध्रुव UText *ut) {
    UBool r = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE)) != 0;
    प्रत्यागम r;
}


U_CAPI int64_t U_EXPORT2
utext_get_Native_सूचकांक(ध्रुव UText *ut) {
    यदि(ut->chunk_विस्थापनम् <= ut->native_Indexing_सीमा) {
        प्रत्यागम ut->chunkNativeStart+ut->chunk_विस्थापनम्;
    } अन्यथा {
        प्रत्यागम ut->pFuncs->map_विस्थापनम्_गन्तव्यम्_Native(ut);
    }
}


U_CAPI शून्य U_EXPORT2
utext_समुच्चयः_Native_सूचकांक(UText *ut, int64_t सूचकांक) {
    यदि(सूचकांक<ut->chunkNativeStart || सूचकांक>=ut->chunk_Native_सीमा) {
        // The desired position is outside of the current chunk.
        // Access the new position.  Assume a forward iteration from here,
        // which will also be optimimum for a single random access.
        // Reverse iterations may suffer slightly.
        ut->pFuncs->access(ut, सूचकांक, सत्य);
    } अन्यथा यदि((int32_t)(सूचकांक - ut->chunkNativeStart) <= ut->native_Indexing_सीमा) {
        // utf-16 indexing.
        ut->chunk_विस्थापनम्=(int32_t)(सूचकांक-ut->chunkNativeStart);
    } अन्यथा {
         ut->chunk_विस्थापनम्=ut->pFuncs->map_Native_सूचकांक_गन्तव्यम्_UTF16(ut, सूचकांक);
    }
    // The convention is that the index must always be on a code point boundary.
    // Adjust the index position if it is in the middle of a surrogate pair.
    यदि (ut->chunk_विस्थापनम्<ut->chunk_दीर्घता) {
        char16_t c= ut->chunkContents[ut->chunk_विस्थापनम्];
        यदि (U16_IS_TRAIL(c)) {
            यदि (ut->chunk_विस्थापनम्==0) {
                ut->pFuncs->access(ut, ut->chunkNativeStart, असत्य);
            }
            यदि (ut->chunk_विस्थापनम्>0) {
                char16_t lead = ut->chunkContents[ut->chunk_विस्थापनम्-1];
                यदि (U16_IS_LEAD(lead)) {
                    ut->chunk_विस्थापनम्--;
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
    int32_t i = ut->chunk_विस्थापनम् - 1;
    int64_t परिणाम;
    यदि (i >= 0) {
        char16_t c = ut->chunkContents[i];
        यदि (U16_IS_TRAIL(c) == असत्य) {
            यदि (i <= ut->native_Indexing_सीमा) {
                परिणाम = ut->chunkNativeStart + i;
            } अन्यथा {
                ut->chunk_विस्थापनम् = i;
                परिणाम = ut->pFuncs->map_विस्थापनम्_गन्तव्यम्_Native(ut);
                ut->chunk_विस्थापनम्++;
            }
            प्रत्यागम परिणाम;
        }
    }

    // If at the start of text, simply return 0.
    यदि (ut->chunk_विस्थापनम्==0 && ut->chunkNativeStart==0) {
        प्रत्यागम 0;
    }

    // Harder, less common cases.  We are at a chunk boundary, or on a surrogate.
    //    Keep it simple, use other functions to handle the edges.
    //
    utext_previous32(ut);
    परिणाम = UTEXT_GETNATIVEINDEX(ut);
    utext_next32(ut);
    प्रत्यागम परिणाम;
}


//
//  utext_current32.  Get the UChar32 at the current position.
//                    UText iteration position is always on a code point boundary,
//                    never on the trail half of a surrogate pair.
//
U_CAPI UChar32 U_EXPORT2
utext_current32(UText *ut) {
    UChar32  c;
    यदि (ut->chunk_विस्थापनम्==ut->chunk_दीर्घता) {
        // Current position is just off the end of the chunk.
        यदि (ut->pFuncs->access(ut, ut->chunk_Native_सीमा, सत्य) == असत्य) {
            // Off the end of the text.
            प्रत्यागम U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_विस्थापनम्];
    यदि (U16_IS_LEAD(c) == असत्य) {
        // Normal, non-supplementary case.
        प्रत्यागम c;
    }

    //
    //  Possible supplementary char.
    //
    UChar32   trail = 0;
    UChar32   supplementaryC = c;
    यदि ((ut->chunk_विस्थापनम्+1) < ut->chunk_दीर्घता) {
        // The trail surrogate is in the same chunk.
        trail = ut->chunkContents[ut->chunk_विस्थापनम्+1];
    } अन्यथा {
        //  The trail surrogate is in a different chunk.
        //     Because we must maintain the iteration position, we need to switch forward
        //     into the new chunk, get the trail surrogate, then revert the chunk back to the
        //     original one.
        //     An edge case to be careful of:  the entire text may end with an unpaired
        //        leading surrogate.  The attempt to access the trail will fail, but
        //        the original position before the unpaired lead still needs to be restored.
        int64_t  nativePosition = ut->chunk_Native_सीमा;
        यदि (ut->pFuncs->access(ut, nativePosition, सत्य)) {
            trail = ut->chunkContents[ut->chunk_विस्थापनम्];
        }
        UBool r = ut->pFuncs->access(ut, nativePosition, असत्य);  // reverse iteration flag loads preceding chunk
        U_ASSERT(r);
        // Here we need to restore chunkOffset since the access functions were called with
        // chunkNativeLimit but that is not where we were (we were 1 code unit before the
        // limit). Restoring was originally added in ICU-4669 but did not support access
        // functions that changed the chunk size, the following does.
        ut->chunk_विस्थापनम् = ut->chunk_दीर्घता - 1;
        यदि(!r) {
            प्रत्यागम U_SENTINEL;
        }
    }

    यदि (U16_IS_TRAIL(trail)) {
        supplementaryC = U16_GET_SUPPLEMENTARY(c, trail);
    }
    प्रत्यागम supplementaryC;

}


U_CAPI UChar32 U_EXPORT2
utext_char32At(UText *ut, int64_t native_सूचकांक) {
    UChar32 c = U_SENTINEL;

    // Fast path the common case.
    यदि (native_सूचकांक>=ut->chunkNativeStart && native_सूचकांक < ut->chunkNativeStart + ut->native_Indexing_सीमा) {
        ut->chunk_विस्थापनम् = (int32_t)(native_सूचकांक - ut->chunkNativeStart);
        c = ut->chunkContents[ut->chunk_विस्थापनम्];
        यदि (U16_IS_SURROGATE(c) == असत्य) {
            प्रत्यागम c;
        }
    }


    utext_समुच्चयः_Native_सूचकांक(ut, native_सूचकांक);
    यदि (native_सूचकांक>=ut->chunkNativeStart && ut->chunk_विस्थापनम्<ut->chunk_दीर्घता) {
        c = ut->chunkContents[ut->chunk_विस्थापनम्];
        यदि (U16_IS_SURROGATE(c)) {
            // For surrogates, let current32() deal with the complications
            //    of supplementaries that may span chunk boundaries.
            c = utext_current32(ut);
        }
    }
    प्रत्यागम c;
}


U_CAPI UChar32 U_EXPORT2
utext_next32(UText *ut) {
    UChar32       c;

    यदि (ut->chunk_विस्थापनम् >= ut->chunk_दीर्घता) {
        यदि (ut->pFuncs->access(ut, ut->chunk_Native_सीमा, सत्य) == असत्य) {
            प्रत्यागम U_SENTINEL;
        }
    }

    c = ut->chunkContents[ut->chunk_विस्थापनम्++];
    यदि (U16_IS_LEAD(c) == असत्य) {
        // Normal case, not supplementary.
        //   (A trail surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        प्रत्यागम c;
    }

    यदि (ut->chunk_विस्थापनम् >= ut->chunk_दीर्घता) {
        यदि (ut->pFuncs->access(ut, ut->chunk_Native_सीमा, सत्य) == असत्य) {
            // c is an unpaired lead surrogate at the end of the text.
            // return it as it is.
            प्रत्यागम c;
        }
    }
    UChar32 trail = ut->chunkContents[ut->chunk_विस्थापनम्];
    यदि (U16_IS_TRAIL(trail) == असत्य) {
        // c was an unpaired lead surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is on the
        // following character, possibly in the next chunk, where the
        //  trail surrogate would have been if it had existed.
        प्रत्यागम c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(c, trail);
    ut->chunk_विस्थापनम्++;   // move iteration position over the trail surrogate.
    प्रत्यागम supplementary;
    }


U_CAPI UChar32 U_EXPORT2
utext_previous32(UText *ut) {
    UChar32       c;

    यदि (ut->chunk_विस्थापनम् <= 0) {
        यदि (ut->pFuncs->access(ut, ut->chunkNativeStart, असत्य) == असत्य) {
            प्रत्यागम U_SENTINEL;
        }
    }
    ut->chunk_विस्थापनम्--;
    c = ut->chunkContents[ut->chunk_विस्थापनम्];
    यदि (U16_IS_TRAIL(c) == असत्य) {
        // Normal case, not supplementary.
        //   (A lead surrogate seen here is just returned as is, as a surrogate value.
        //    It cannot be part of a pair.)
        प्रत्यागम c;
    }

    यदि (ut->chunk_विस्थापनम् <= 0) {
        यदि (ut->pFuncs->access(ut, ut->chunkNativeStart, असत्य) == असत्य) {
            // c is an unpaired trail surrogate at the start of the text.
            // return it as it is.
            प्रत्यागम c;
        }
    }

    UChar32 lead = ut->chunkContents[ut->chunk_विस्थापनम्-1];
    यदि (U16_IS_LEAD(lead) == असत्य) {
        // c was an unpaired trail surrogate, not at the end of the text.
        // return it as it is (unpaired).  Iteration position is at c
        प्रत्यागम c;
    }

    UChar32 supplementary = U16_GET_SUPPLEMENTARY(lead, c);
    ut->chunk_विस्थापनम्--;   // move iteration position over the lead surrogate.
    प्रत्यागम supplementary;
}



U_CAPI UChar32 U_EXPORT2
utext_next32From(UText *ut, int64_t सूचकांक) {
    UChar32       c      = U_SENTINEL;

    यदि(सूचकांक<ut->chunkNativeStart || सूचकांक>=ut->chunk_Native_सीमा) {
        // Desired position is outside of the current chunk.
        यदि(!ut->pFuncs->access(ut, सूचकांक, सत्य)) {
            // no chunk available here
            प्रत्यागम U_SENTINEL;
        }
    } अन्यथा यदि (सूचकांक - ut->chunkNativeStart  <= (int64_t)ut->native_Indexing_सीमा) {
        // Desired position is in chunk, with direct 1:1 native to UTF16 indexing
        ut->chunk_विस्थापनम् = (int32_t)(सूचकांक - ut->chunkNativeStart);
    } अन्यथा {
        // Desired position is in chunk, with non-UTF16 indexing.
        ut->chunk_विस्थापनम् = ut->pFuncs->map_Native_सूचकांक_गन्तव्यम्_UTF16(ut, सूचकांक);
    }

    c = ut->chunkContents[ut->chunk_विस्थापनम्++];
    यदि (U16_IS_SURROGATE(c)) {
        // Surrogates.  Many edge cases.  Use other functions that already
        //              deal with the problems.
        utext_समुच्चयः_Native_सूचकांक(ut, सूचकांक);
        c = utext_next32(ut);
    }
    प्रत्यागम c;
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
    यदि(सूचकांक<=ut->chunkNativeStart || सूचकांक>ut->chunk_Native_सीमा) {
        // Requested native index is outside of the current chunk.
        यदि(!ut->pFuncs->access(ut, सूचकांक, असत्य)) {
            // no chunk available here
            प्रत्यागम U_SENTINEL;
        }
    } अन्यथा यदि(सूचकांक - ut->chunkNativeStart <= (int64_t)ut->native_Indexing_सीमा) {
        // Direct UTF-16 indexing.
        ut->chunk_विस्थापनम् = (int32_t)(सूचकांक - ut->chunkNativeStart);
    } अन्यथा {
        ut->chunk_विस्थापनम्=ut->pFuncs->map_Native_सूचकांक_गन्तव्यम्_UTF16(ut, सूचकांक);
        यदि (ut->chunk_विस्थापनम्==0 && !ut->pFuncs->access(ut, सूचकांक, असत्य)) {
            // no chunk available here
            प्रत्यागम U_SENTINEL;
        }
    }

    //
    // Simple case with no surrogates.
    //
    ut->chunk_विस्थापनम्--;
    c_पूर्व = ut->chunkContents[ut->chunk_विस्थापनम्];

    यदि (U16_IS_SURROGATE(c_पूर्व)) {
        // Possible supplementary.  Many edge cases.
        // Let other functions do the heavy lifting.
        utext_समुच्चयः_Native_सूचकांक(ut, सूचकांक);
        c_पूर्व = utext_previous32(ut);
    }
    प्रत्यागम c_पूर्व;
}


U_CAPI int32_t U_EXPORT2
utext_extract(UText *ut,
             int64_t start, int64_t सीमा,
             char16_t *dest, int32_t dest_क्षमता,
             UError_संहिता *स्थितिः) {
                 प्रत्यागम ut->pFuncs->extract(ut, start, सीमा, dest, dest_क्षमता, स्थितिः);
             }



U_CAPI UBool U_EXPORT2
utext_equals(ध्रुव UText *a, ध्रुव UText *b) {
    यदि (a==शून्यसूचकः || b==शून्यसूचकः ||
        a->magic != UTEXT_MAGIC ||
        b->magic != UTEXT_MAGIC) {
            // Null or invalid arguments don't compare equal to anything.
            प्रत्यागम असत्य;
    }

    यदि (a->pFuncs != b->pFuncs) {
        // Different types of text providers.
        प्रत्यागम असत्य;
    }

    यदि (a->context != b->context) {
        // Different sources (different strings)
        प्रत्यागम असत्य;
    }
    यदि (utext_get_Native_सूचकांक(a) != utext_get_Native_सूचकांक(b)) {
        // Different current position in the string.
        प्रत्यागम असत्य;
    }

    प्रत्यागम सत्य;
}

U_CAPI UBool U_EXPORT2
utext_isWritable(ध्रुव UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) != 0;
    प्रत्यागम b;
}


U_CAPI शून्य U_EXPORT2
utext_freeze(UText *ut) {
    // Zero out the WRITABLE flag.
    ut->providerProperties &= ~(I32_FLAG(UTEXT_PROVIDER_WRITABLE));
}


U_CAPI UBool U_EXPORT2
utext_has_Meta_दत्त(ध्रुव UText *ut)
{
    UBool b = (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_HAS_META_दत्त)) != 0;
    प्रत्यागम b;
}



U_CAPI int32_t U_EXPORT2
utext_replace(UText *ut,
             int64_t nativeStart, int64_t native_सीमा,
             ध्रुव char16_t *replacementText, int32_t replacement_दीर्घता,
             UError_संहिता *स्थितिः)
{
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *स्थितिः = U_NO_WRITE_PERMISSION;
        प्रत्यागम 0;
    }
    int32_t i = ut->pFuncs->replace(ut, nativeStart, native_सीमा, replacementText, replacement_दीर्घता, स्थितिः);
    प्रत्यागम i;
}

U_CAPI शून्य U_EXPORT2
utext_copy(UText *ut,
          int64_t nativeStart, int64_t native_सीमा,
          int64_t dest_सूचकांक,
          UBool move,
          UError_संहिता *स्थितिः)
{
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम;
    }
    यदि ((ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_WRITABLE)) == 0) {
        *स्थितिः = U_NO_WRITE_PERMISSION;
        प्रत्यागम;
    }
    ut->pFuncs->copy(ut, nativeStart, native_सीमा, dest_सूचकांक, move, स्थितिः);
}

U_CAPI UText* U_EXPORT2
utext_clone(UText* dest U_LIFETIME_BOUND,
            ध्रुव UText* src,
            UBool deep,
            UBool readOnly,
            UError_संहिता* स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम dest;
    }
    UText *परिणाम = src->pFuncs->clone(dest, src, deep, स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम परिणाम;
    }
    यदि (परिणाम == शून्यसूचकः) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम परिणाम;
    }
    यदि (readOnly) {
        utext_freeze(परिणाम);
    }
    प्रत्यागम परिणाम;
}



//------------------------------------------------------------------------------
//
//   UText common functions implementation
//
//------------------------------------------------------------------------------

//
//  UText.flags bit definitions
//
गणना {
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

संरचना ExtendedUText {
    UText               ut;
    std::max_align_t    extension;
};

स्थिर ध्रुव UText रिक्तानि_Text = UTEXT_INITIALIZER;

U_CAPI UText* U_EXPORT2
utext_सज्जता(UText* ut U_LIFETIME_BOUND, int32_t extraSpace, UError_संहिता* स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम ut;
    }

    यदि (ut == शून्यसूचकः) {
        // We need to heap-allocate storage for the new UText
        int32_t spaceRequired = परिमाण(UText);
        यदि (extraSpace > 0) {
            spaceRequired = परिमाण(ExtendedUText) + extraSpace - परिमाण(std::max_align_t);
        }
        ut = (UText *)uprv_malloc(spaceRequired);
        यदि (ut == शून्यसूचकः) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        } अन्यथा {
            *ut = रिक्तानि_Text;
            ut->flags |= UTEXT_HEAP_ALLOCATED;
            यदि (spaceRequired>0) {
                ut->extra_आकार = extraSpace;
                ut->pExtra    = &((ExtendedUText *)ut)->extension;
            }
        }
    } अन्यथा {
        // We have been supplied with an already existing UText.
        // Verify that it really appears to be a UText.
        यदि (ut->magic != UTEXT_MAGIC) {
            *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम ut;
        }
        // If the ut is already open and there's a provider supplied close
        //   function, call it.
        यदि ((ut->flags & UTEXT_OPEN) && ut->pFuncs->close != शून्यसूचकः)  {
            ut->pFuncs->close(ut);
        }
        ut->flags &= ~UTEXT_OPEN;

        // If extra space was requested by our caller, check whether
        //   sufficient already exists, and allocate new if needed.
        यदि (extraSpace > ut->extra_आकार) {
            // Need more space.  If there is existing separately allocated space,
            //   delete it first, then allocate new space.
            यदि (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
                uprv_free(ut->pExtra);
                ut->extra_आकार = 0;
            }
            ut->pExtra = uprv_malloc(extraSpace);
            यदि (ut->pExtra == शून्यसूचकः) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            } अन्यथा {
                ut->extra_आकार = extraSpace;
                ut->flags |= UTEXT_EXTRA_HEAP_ALLOCATED;
            }
        }
    }
    यदि (U_SUCCESS(*स्थितिः)) {
        ut->flags |= UTEXT_OPEN;

        // Initialize all remaining fields of the UText.
        //
        ut->context             = शून्यसूचकः;
        ut->chunkContents       = शून्यसूचकः;
        ut->p                   = शून्यसूचकः;
        ut->q                   = शून्यसूचकः;
        ut->r                   = शून्यसूचकः;
        ut->a                   = 0;
        ut->b                   = 0;
        ut->c                   = 0;
        ut->chunk_विस्थापनम्         = 0;
        ut->chunk_दीर्घता         = 0;
        ut->chunkNativeStart    = 0;
        ut->chunk_Native_सीमा    = 0;
        ut->native_Indexing_सीमा = 0;
        ut->providerProperties  = 0;
        ut->privA               = 0;
        ut->privB               = 0;
        ut->privC               = 0;
        ut->privP               = शून्यसूचकः;
        यदि (ut->pExtra!=शून्यसूचकः && ut->extra_आकार>0)
            uprv_memset(ut->pExtra, 0, ut->extra_आकार);

    }
    प्रत्यागम ut;
}


U_CAPI UText* U_EXPORT2
utext_close(UText* ut U_LIFETIME_BOUND) {
    यदि (ut==शून्यसूचकः ||
        ut->magic != UTEXT_MAGIC ||
        (ut->flags & UTEXT_OPEN) == 0)
    {
        // The supplied ut is not an open UText.
        // Do nothing.
        प्रत्यागम ut;
    }

    // If the provider gave us a close function, call it now.
    // This will clean up anything allocated specifically by the provider.
    यदि (ut->pFuncs->close != शून्यसूचकः) {
        ut->pFuncs->close(ut);
    }
    ut->flags &= ~UTEXT_OPEN;

    // If we (the framework) allocated the UText or subsidiary storage,
    //   delete it.
    यदि (ut->flags & UTEXT_EXTRA_HEAP_ALLOCATED) {
        uprv_free(ut->pExtra);
        ut->pExtra = शून्यसूचकः;
        ut->flags &= ~UTEXT_EXTRA_HEAP_ALLOCATED;
        ut->extra_आकार = 0;
    }

    // Zero out function table of the closed UText.  This is a defensive move,
    //   intended to cause applications that inadvertently use a closed
    //   utext to crash with null pointer errors.
    ut->pFuncs        = शून्यसूचकः;

    यदि (ut->flags & UTEXT_HEAP_ALLOCATED) {
        // This UText was allocated by UText setup.  We need to free it.
        // Clear magic, so we can detect if the user messes up and immediately
        //  tries to reopen another UText using the deleted storage.
        ut->magic = 0;
        uprv_free(ut);
        ut = शून्यसूचकः;
    }
    प्रत्यागम ut;
}




//
// invalidateChunk   Reset a chunk to have no contents, so that the next call
//                   to access will cause new data to load.
//                   This is needed when copy/move/replace operate directly on the
//                   backing text, potentially putting it out of sync with the
//                   contents in the chunk.
//
स्थिर शून्य
invalidateChunk(UText *ut) {
    ut->chunk_दीर्घता = 0;
    ut->chunk_Native_सीमा = 0;
    ut->chunkNativeStart = 0;
    ut->chunk_विस्थापनम् = 0;
    ut->native_Indexing_सीमा = 0;
}

//
// pinIndex        Do range pinning on a native index parameter.
//                 64 bit pinning is done in place.
//                 32 bit truncated result is returned as a convenience for
//                        use in providers that don't need 64 bits.
स्थिर int32_t
pin_सूचकांक(int64_t &सूचकांक, int64_t सीमा) {
    यदि (सूचकांक<0) {
        सूचकांक = 0;
    } अन्यथा यदि (सूचकांक > सीमा) {
        सूचकांक = सीमा;
    }
    प्रत्यागम static_cast<int32_t>(सूचकांक);
}


U_CDECL_BEGIN

//
// Pointer relocation function,
//   a utility used by shallow clone.
//   Adjust a pointer that refers to something within one UText (the source)
//   to refer to the same relative offset within a another UText (the target)
//
स्थिर शून्य adjustPointer(UText *dest, ध्रुव शून्य **destPtr, ध्रुव UText *src) {
    // convert all pointers to (char *) so that byte address arithmetic will work.
    वर्ण  *dptr = (वर्ण *)*destPtr;
    वर्ण  *dUText = (वर्ण *)dest;
    वर्ण  *sUText = (वर्ण *)src;

    यदि (dptr >= (वर्ण *)src->pExtra && dptr < ((वर्ण*)src->pExtra)+src->extra_आकार) {
        // target ptr was to something within the src UText's pExtra storage.
        //   relocate it into the target UText's pExtra region.
        *destPtr = ((वर्ण *)dest->pExtra) + (dptr - (वर्ण *)src->pExtra);
    } अन्यथा यदि (dptr>=sUText && dptr < sUText+src->आकार_सम्बन्धि_Struct) {
        // target ptr was pointing to somewhere within the source UText itself.
        //   Move it to the same offset within the target UText.
        *destPtr = dUText + (dptr-sUText);
    }
}


//
//  Clone.  This is a generic copy-the-utext-by-value clone function that can be
//          used as-is with some utext types, and as a helper by other clones.
//
स्थिर UText * U_CALLCONV
shallowTextClone(UText * dest, ध्रुव UText * src, UError_संहिता * स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    int32_t  src_Extra_आकार = src->extra_आकार;

    //
    // Use the generic text_setup to allocate storage if required.
    //
    dest = utext_सज्जता(dest, src_Extra_आकार, स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम dest;
    }

    //
    //  flags (how the UText was allocated) and the pointer to the
    //   extra storage must retain the values in the cloned utext that
    //   were set up by utext_setup.  Save them separately before
    //   copying the whole struct.
    //
    शून्य *destExtra = dest->pExtra;
    int32_t flags   = dest->flags;


    //
    //  Copy the whole UText struct by value.
    //  Any "Extra" storage is copied also.
    //
    पूर्णाङ्क आकार_गन्तव्यम्_Copy = src->आकार_सम्बन्धि_Struct;
    यदि (आकार_गन्तव्यम्_Copy > dest->आकार_सम्बन्धि_Struct) {
        आकार_गन्तव्यम्_Copy = dest->आकार_सम्बन्धि_Struct;
    }
    uprv_memcpy(dest, src, आकार_गन्तव्यम्_Copy);
    dest->pExtra = destExtra;
    dest->flags  = flags;
    यदि (src_Extra_आकार > 0) {
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
    adjustPointer(dest, (ध्रुव शून्य **)&dest->chunkContents, src);

    // The newly shallow-cloned UText does _not_ own the underlying storage for the text.
    // (The source for the clone may or may not have owned the text.)

    dest->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

    प्रत्यागम dest;
}


U_CDECL_अन्तः



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
गणना { UTF8_TEXT_CHUNK_आकार=32 };

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

संरचना UTF8Buf {
    int32_t   bufNativeStart;                        // Native index of first char in char16_t buf
    int32_t   buf_Native_सीमा;                        // Native index following last char in buf.
    int32_t   bufStartIdx;                           // First filled position in buf.
    int32_t   buf_सीमा_Idx;                           // Limit of filled range in buf.
    int32_t   bufNILimit;                            // Limit of native indexing part of buf
    int32_t   गन्तव्यम्_UChars_Map_Start;                      // Native index corresponding to
                                                     //   mapToUChars[0].
                                                     //   Set to bufNativeStart when filling forwards.
                                                     //   Set to computed value when filling backwards.

    char16_t  buf[UTF8_TEXT_CHUNK_आकार+4];           // The char16_t buffer.  Requires one extra position beyond the
                                                     //   the chunk size, to allow for surrogate at the end.
                                                     //   Length must be identical to mapToNative array, below,
                                                     //   because of the way indexing works when the array is
                                                     //   filled backwards during a reverse iteration.  Thus,
                                                     //   the additional extra size.
    uint8_t   map_गन्तव्यम्_Native[UTF8_TEXT_CHUNK_आकार+4];   // map char16_t index in buf to
                                                     //  native offset from bufNativeStart.
                                                     //  Requires two extra slots,
                                                     //    one for a supplementary starting in the last normal position,
                                                     //    and one for an entry for the buffer limit position.
    uint8_t   map_गन्तव्यम्_UChars[UTF8_TEXT_CHUNK_आकार*3+6]; // Map native offset from bufNativeStart to
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
स्थिर int64_t U_CALLCONV
utf8_Text_दीर्घता(UText *ut) {
    यदि (ut->b < 0) {
        // Zero terminated string, and we haven't scanned to the end yet.
        // Scan it now.
        ध्रुव वर्ण *r = (ध्रुव वर्ण *)ut->context + ut->c;
        यावत् (*r != 0) {
            r++;
        }
        यदि ((r - (ध्रुव वर्ण *)ut->context) < 0x7fffffff) {
            ut->b = (int32_t)(r - (ध्रुव वर्ण *)ut->context);
        } अन्यथा {
            // Actual string was bigger (more than 2 gig) than we
            //   can handle.  Clip it to 2 GB.
            ut->b = 0x7fffffff;
        }
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE);
    }
    प्रत्यागम ut->b;
}






स्थिर UBool U_CALLCONV
utf8TextAccess(UText *ut, int64_t सूचकांक, UBool forward) {
    //
    //  Apologies to those who are allergic to goto statements.
    //    Consider each goto to a labelled block to be the equivalent of
    //         call the named block as if it were a function();
    //         return;
    //
    ध्रुव uint8_t *s8=(ध्रुव uint8_t *)ut->context;
    UTF8Buf *u8b = शून्यसूचकः;
    int32_t  length = ut->b;         // Length of original utf-8
    int32_t  ix= (int32_t)सूचकांक;     // Requested index, trimmed to 32 bits.
    int32_t  map_सूचकांक = 0;
    यदि (सूचकांक<0) {
        ix=0;
    } अन्यथा यदि (सूचकांक > 0x7fffffff) {
        // Strings with 64 bit lengths not supported by this UTF-8 provider.
        ix = 0x7fffffff;
    }

    // Pin requested index to the string length.
    यदि (ix>length) {
        यदि (length>=0) {
            ix=length;
        } अन्यथा यदि (ix>=ut->c) {
            // Zero terminated string, and requested index is beyond
            //   the region that has already been scanned.
            //   Scan up to either the end of the string or to the
            //   requested position, whichever comes first.
            यावत् (ut->c<ix && s8[ut->c]!=0) {
                ut->c++;
            }
            //  TODO:  support for null terminated string length > 32 bits.
            यदि (s8[ut->c] == 0) {
                // We just found the actual length of the string.
                //  Trim the requested index back to that.
                ix     = ut->c;
                ut->b  = ut->c;
                length = ut->c;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE);
            }
        }
    }

    //
    // Dispatch to the appropriate action for a forward iteration request.
    //
    यदि (forward) {
        यदि (ix==ut->chunk_Native_सीमा) {
            // Check for normal sequential iteration cases first.
            यदि (ix==length) {
                // Just reached end of string
                // Don't swap buffers, but do set the
                //   current buffer position.
                ut->chunk_विस्थापनम् = ut->chunk_दीर्घता;
                प्रत्यागम असत्य;
            } अन्यथा {
                // End of current buffer.
                //   check whether other buffer already has what we need.
                UTF8Buf *altB = (UTF8Buf *)ut->q;
                यदि (ix>=altB->bufNativeStart && ix<altB->buf_Native_सीमा) {
                    गच्छ swapBuffers;
                }
            }
        }

        // A random access.  Desired index could be in either or niether buf.
        // For optimizing the order of testing, first check for the index
        //    being in the other buffer.  This will be the case for uses that
        //    move back and forth over a fairly limited range
        {
            u8b = (UTF8Buf *)ut->q;   // the alternate buffer
            यदि (ix>=u8b->bufNativeStart && ix<u8b->buf_Native_सीमा) {
                // Requested index is in the other buffer.
                गच्छ swapBuffers;
            }
            यदि (ix == length) {
                // Requested index is end-of-string.
                //   (this is the case of randomly seeking to the end.
                //    The case of iterating off the end is handled earlier.)
                यदि (ix == ut->chunk_Native_सीमा) {
                    // Current buffer extends up to the end of the string.
                    //   Leave it as the current buffer.
                    ut->chunk_विस्थापनम् = ut->chunk_दीर्घता;
                    प्रत्यागम असत्य;
                }
                यदि (ix == u8b->buf_Native_सीमा) {
                    // Alternate buffer extends to the end of string.
                    //   Swap it in as the current buffer.
                    गच्छ swapBuffersAndFail;
                }

                // Neither existing buffer extends to the end of the string.
                गच्छ make_Stub_बफर;
            }

            यदि (ix<ut->chunkNativeStart || ix>=ut->chunk_Native_सीमा) {
                // Requested index is in neither buffer.
                गच्छ fillForward;
            }

            // Requested index is in this buffer.
            u8b = (UTF8Buf *)ut->p;   // the current buffer
            map_सूचकांक = ix - u8b->गन्तव्यम्_UChars_Map_Start;
            U_ASSERT(map_सूचकांक < (int32_t)परिमाण(UTF8Buf::map_गन्तव्यम्_UChars));
            ut->chunk_विस्थापनम् = u8b->map_गन्तव्यम्_UChars[map_सूचकांक] - u8b->bufStartIdx;
            प्रत्यागम सत्य;

        }
    }


    //
    // Dispatch to the appropriate action for a
    //   Backwards Direction iteration request.
    //
    यदि (ix==ut->chunkNativeStart) {
        // Check for normal sequential iteration cases first.
        यदि (ix==0) {
            // Just reached the start of string
            // Don't swap buffers, but do set the
            //   current buffer position.
            ut->chunk_विस्थापनम् = 0;
            प्रत्यागम असत्य;
        } अन्यथा {
            // Start of current buffer.
            //   check whether other buffer already has what we need.
            UTF8Buf *altB = (UTF8Buf *)ut->q;
            यदि (ix>altB->bufNativeStart && ix<=altB->buf_Native_सीमा) {
                गच्छ swapBuffers;
            }
        }
    }

    // A random access.  Desired index could be in either or niether buf.
    // For optimizing the order of testing,
    //    Most likely case:  in the other buffer.
    //    Second most likely: in neither buffer.
    //    Unlikely, but must work:  in the current buffer.
    u8b = (UTF8Buf *)ut->q;   // the alternate buffer
    यदि (ix>u8b->bufNativeStart && ix<=u8b->buf_Native_सीमा) {
        // Requested index is in the other buffer.
        गच्छ swapBuffers;
    }
    // Requested index is start-of-string.
    //   (this is the case of randomly seeking to the start.
    //    The case of iterating off the start is handled earlier.)
    यदि (ix==0) {
        यदि (u8b->bufNativeStart==0) {
            // Alternate buffer contains the data for the start string.
            // Make it be the current buffer.
            गच्छ swapBuffersAndFail;
        } अन्यथा {
            // Request for data before the start of string,
            //   neither buffer is usable.
            //   set up a zero-length buffer.
            गच्छ make_Stub_बफर;
        }
    }

    यदि (ix<=ut->chunkNativeStart || ix>ut->chunk_Native_सीमा) {
        // Requested index is in neither buffer.
        गच्छ fillReverse;
    }

    // Requested index is in this buffer.
    //   Set the utf16 buffer index.
    u8b = (UTF8Buf *)ut->p;
    map_सूचकांक = ix - u8b->गन्तव्यम्_UChars_Map_Start;
    ut->chunk_विस्थापनम् = u8b->map_गन्तव्यम्_UChars[map_सूचकांक] - u8b->bufStartIdx;
    यदि (ut->chunk_विस्थापनम्==0) {
        // This occurs when the first character in the text is
        //   a multi-byte UTF-8 char, and the requested index is to
        //   one of the trailing bytes.  Because there is no preceding ,
        //   character, this access fails.  We can't pick up on the
        //   situation sooner because the requested index is not zero.
        प्रत्यागम असत्य;
    } अन्यथा {
        प्रत्यागम सत्य;
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
        ut->chunk_दीर्घता         = u8b->buf_सीमा_Idx - u8b->bufStartIdx;
        ut->chunkNativeStart    = u8b->bufNativeStart;
        ut->chunk_Native_सीमा    = u8b->buf_Native_सीमा;
        ut->native_Indexing_सीमा = u8b->bufNILimit;

        // Index into the (now current) chunk
        // Use the map to set the chunk index.  It's more trouble than it's worth
        //    to check whether native indexing can be used.
        U_ASSERT(ix>=u8b->bufNativeStart);
        U_ASSERT(ix<=u8b->buf_Native_सीमा);
        map_सूचकांक = ix - u8b->गन्तव्यम्_UChars_Map_Start;
        U_ASSERT(map_सूचकांक>=0);
        U_ASSERT(map_सूचकांक<(int32_t)परिमाण(u8b->map_गन्तव्यम्_UChars));
        ut->chunk_विस्थापनम् = u8b->map_गन्तव्यम्_UChars[map_सूचकांक] - u8b->bufStartIdx;

        प्रत्यागम सत्य;
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
    ut->chunk_दीर्घता         = u8b->buf_सीमा_Idx - u8b->bufStartIdx;
    ut->chunkNativeStart    = u8b->bufNativeStart;
    ut->chunk_Native_सीमा    = u8b->buf_Native_सीमा;
    ut->native_Indexing_सीमा = u8b->bufNILimit;

    // Index into the (now current) chunk
    //  For this function  (swapBuffersAndFail), the requested index
    //    will always be at either the start or end of the chunk.
    यदि (ix==u8b->buf_Native_सीमा) {
        ut->chunk_विस्थापनम् = ut->chunk_दीर्घता;
    } अन्यथा  {
        ut->chunk_विस्थापनम् = 0;
        U_ASSERT(ix == u8b->bufNativeStart);
    }
    प्रत्यागम असत्य;

make_Stub_बफर:
    //   The user has done a seek/access past the start or end
    //   of the string.  Rather than loading data that is likely
    //   to never be used, just set up a zero-length buffer at
    //   the position.
    u8b = (UTF8Buf *)ut->q;
    u8b->bufNativeStart   = ix;
    u8b->buf_Native_सीमा   = ix;
    u8b->bufStartIdx      = 0;
    u8b->buf_सीमा_Idx      = 0;
    u8b->bufNILimit       = 0;
    u8b->गन्तव्यम्_UChars_Map_Start = ix;
    u8b->map_गन्तव्यम्_Native[0]   = 0;
    u8b->map_गन्तव्यम्_UChars[0]   = 0;
    गच्छ swapBuffersAndFail;



fillForward:
    {
        // Move the incoming index to a code point boundary.
        U8_समुच्चयः_CP_START(s8, 0, ix);

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_swap = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_swap;

        int32_t strLen = ut->b;
        UBool   nulTerminated = असत्य;
        यदि (strLen < 0) {
            strLen = 0x7fffffff;
            nulTerminated = सत्य;
        }

        char16_t   *buf = u8b_swap->buf;
        uint8_t *map_गन्तव्यम्_Native  = u8b_swap->map_गन्तव्यम्_Native;
        uint8_t *map_गन्तव्यम्_UChars  = u8b_swap->map_गन्तव्यम्_UChars;
        int32_t  destIx       = 0;
        int32_t  srcIx        = ix;
        UBool    आलोकितम्_Non_Ascii = असत्य;
        UChar32  c = 0;

        // Fill the chunk buffer and mapping arrays.
        यावत् (destIx<UTF8_TEXT_CHUNK_आकार) {
            c = s8[srcIx];
            यदि (c>0 && c<0x80) {
                // Special case ASCII range for speed.
                //   zero is excluded to simplify bounds checking.
                buf[destIx] = (char16_t)c;
                map_गन्तव्यम्_Native[destIx]    = (uint8_t)(srcIx - ix);
                map_गन्तव्यम्_UChars[srcIx-ix]  = (uint8_t)destIx;
                srcIx++;
                destIx++;
            } अन्यथा {
                // General case, handle everything.
                यदि (आलोकितम्_Non_Ascii == असत्य) {
                    आलोकितम्_Non_Ascii = सत्य;
                    u8b_swap->bufNILimit = destIx;
                }

                int32_t  cIx      = srcIx;
                int32_t  dIx      = destIx;
                int32_t  dIxSaved = destIx;
                U8_अग्र_OR_FFFD(s8, srcIx, strLen, c);
                यदि (c==0 && nulTerminated) {
                    srcIx--;
                    भङ्ग;
                }

                U16_APPEND_UNSAFE(buf, destIx, c);
                कुरु {
                    map_गन्तव्यम्_Native[dIx++] = (uint8_t)(cIx - ix);
                } यावत् (dIx < destIx);

                कुरु {
                    map_गन्तव्यम्_UChars[cIx++ - ix] = (uint8_t)dIxSaved;
                } यावत् (cIx < srcIx);
            }
            यदि (srcIx>=strLen) {
                भङ्ग;
            }

        }

        //  store Native <--> Chunk Map entries for the end of the buffer.
        //    There is no actual character here, but the index position is valid.
        map_गन्तव्यम्_Native[destIx]     = (uint8_t)(srcIx - ix);
        map_गन्तव्यम्_UChars[srcIx - ix] = (uint8_t)destIx;

        //  fill in Buffer descriptor
        u8b_swap->bufNativeStart     = ix;
        u8b_swap->buf_Native_सीमा     = srcIx;
        u8b_swap->bufStartIdx        = 0;
        u8b_swap->buf_सीमा_Idx        = destIx;
        यदि (आलोकितम्_Non_Ascii == असत्य) {
            u8b_swap->bufNILimit     = destIx;
        }
        u8b_swap->गन्तव्यम्_UChars_Map_Start   = u8b_swap->bufNativeStart;

        // Set UText chunk to refer to this buffer.
        ut->chunkContents       = buf;
        ut->chunk_विस्थापनम्         = 0;
        ut->chunk_दीर्घता         = u8b_swap->buf_सीमा_Idx;
        ut->chunkNativeStart    = u8b_swap->bufNativeStart;
        ut->chunk_Native_सीमा    = u8b_swap->buf_Native_सीमा;
        ut->native_Indexing_सीमा = u8b_swap->bufNILimit;

        // For zero terminated strings, keep track of the maximum point
        //   scanned so far.
        यदि (nulTerminated && srcIx>ut->c) {
            ut->c = srcIx;
            यदि (c==0) {
                // We scanned to the end.
                //   Remember the actual length.
                ut->b = srcIx;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE);
            }
        }
        प्रत्यागम सत्य;
    }


fillReverse:
    {
        // Move the incoming index to a code point boundary.
        // Can only do this if the incoming index is somewhere in the interior of the string.
        //   If index is at the end, there is no character there to look at.
        यदि (ix != ut->b) {
            // Note: this function will only move the index back if it is on a trail byte
            //       and there is a preceding lead byte and the sequence from the lead 
            //       through this trail could be part of a valid UTF-8 sequence
            //       Otherwise the index remains unchanged.
            U8_समुच्चयः_CP_START(s8, 0, ix);
        }

        // Swap the UText buffers.
        //  We want to fill what was previously the alternate buffer,
        //  and make what was the current buffer be the new alternate.
        UTF8Buf *u8b_swap = (UTF8Buf *)ut->q;
        ut->q = ut->p;
        ut->p = u8b_swap;

        char16_t   *buf = u8b_swap->buf;
        uint8_t *map_गन्तव्यम्_Native = u8b_swap->map_गन्तव्यम्_Native;
        uint8_t *map_गन्तव्यम्_UChars = u8b_swap->map_गन्तव्यम्_UChars;
        int32_t  गन्तव्यम्_UChars_Map_Start = ix - परिमाण(UTF8Buf::map_गन्तव्यम्_UChars) + 1;
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
        map_गन्तव्यम्_Native[destIx] = (uint8_t)(srcIx - गन्तव्यम्_UChars_Map_Start);
        map_गन्तव्यम्_UChars[srcIx - गन्तव्यम्_UChars_Map_Start] = (uint8_t)destIx;

        // Fill the chunk buffer
        // Work backwards, filling from the end of the buffer towards the front.
        //
        यावत् (destIx>2 && (srcIx - गन्तव्यम्_UChars_Map_Start > 5) && (srcIx > 0)) {
            srcIx--;
            destIx--;

            // Get last byte of the UTF-8 character
            c = s8[srcIx];
            यदि (c<0x80) {
                // Special case ASCII range for speed.
                buf[destIx] = (char16_t)c;
                U_ASSERT(गन्तव्यम्_UChars_Map_Start <= srcIx);
                map_गन्तव्यम्_UChars[srcIx - गन्तव्यम्_UChars_Map_Start] = (uint8_t)destIx;
                map_गन्तव्यम्_Native[destIx] = (uint8_t)(srcIx - गन्तव्यम्_UChars_Map_Start);
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
                यदि (c<0x10000) {
                    buf[destIx] = (char16_t)c;
                    map_गन्तव्यम्_Native[destIx] = (uint8_t)(srcIx - गन्तव्यम्_UChars_Map_Start);
                } अन्यथा {
                    buf[destIx]         = U16_TRAIL(c);
                    map_गन्तव्यम्_Native[destIx] = (uint8_t)(srcIx - गन्तव्यम्_UChars_Map_Start);
                    buf[--destIx]       = U16_LEAD(c);
                    map_गन्तव्यम्_Native[destIx] = (uint8_t)(srcIx - गन्तव्यम्_UChars_Map_Start);
                }

                // Fill in the map from native indexes to UChars buf index.
                कुरु {
                    map_गन्तव्यम्_UChars[sIx-- - गन्तव्यम्_UChars_Map_Start] = (uint8_t)destIx;
                } यावत् (sIx >= srcIx);
                U_ASSERT(गन्तव्यम्_UChars_Map_Start <= (srcIx+1));

                // Set native indexing limit to be the current position.
                //   We are processing a non-ascii, non-native-indexing char now;
                //     the limit will be here if the rest of the chars to be
                //     added to this buffer are ascii.
                bufNILimit = destIx;
            }
        }
        u8b_swap->bufNativeStart     = srcIx;
        u8b_swap->buf_Native_सीमा     = ix;
        u8b_swap->bufStartIdx        = destIx;
        u8b_swap->buf_सीमा_Idx        = UTF8_TEXT_CHUNK_आकार+2;
        u8b_swap->bufNILimit         = bufNILimit - u8b_swap->bufStartIdx;
        u8b_swap->गन्तव्यम्_UChars_Map_Start   = गन्तव्यम्_UChars_Map_Start;

        ut->chunkContents       = &buf[u8b_swap->bufStartIdx];
        ut->chunk_दीर्घता         = u8b_swap->buf_सीमा_Idx - u8b_swap->bufStartIdx;
        ut->chunk_विस्थापनम्         = ut->chunk_दीर्घता;
        ut->chunkNativeStart    = u8b_swap->bufNativeStart;
        ut->chunk_Native_सीमा    = u8b_swap->buf_Native_सीमा;
        ut->native_Indexing_सीमा = u8b_swap->bufNILimit;
        प्रत्यागम सत्य;
    }

}



//
//  This is a slightly modified copy of u_strFromUTF8,
//     Inserts a Replacement Char rather than failing on invalid UTF-8
//     Removes unnecessary features.
//
स्थिर char16_t*
utext_strFromUTF8(char16_t *dest,
              int32_t dest_क्षमता,
              int32_t *p_Dest_दीर्घता,
              ध्रुव वर्ण* src,
              int32_t src_दीर्घता,        // required.  NUL terminated not supported.
              UError_संहिता *p_Error_संहिता
              )
{

    char16_t *pDest = dest;
    char16_t *p_Dest_सीमा = (dest!=शून्यसूचकः)?(dest+dest_क्षमता):शून्यसूचकः;
    UChar32 ch=0;
    int32_t सूचकांक = 0;
    int32_t req_दीर्घता = 0;
    uint8_t* pSrc = (uint8_t*) src;


    यावत्((सूचकांक < src_दीर्घता)&&(pDest<p_Dest_सीमा)){
        ch = pSrc[सूचकांक++];
        यदि(ch <=0x7f){
            *pDest++=(char16_t)ch;
        }अन्यथा{
            ch=utf8_अग्र_Char_Safe_Body(pSrc, &सूचकांक, src_दीर्घता, ch, -3);
            यदि(U_IS_BMP(ch)){
                *(pDest++)=(char16_t)ch;
            }अन्यथा{
                *(pDest++)=U16_LEAD(ch);
                यदि(pDest<p_Dest_सीमा){
                    *(pDest++)=U16_TRAIL(ch);
                }अन्यथा{
                    req_दीर्घता++;
                    भङ्ग;
                }
            }
        }
    }
    /* donot fill the dest buffer just count the UChars needed */
    यावत्(सूचकांक < src_दीर्घता){
        ch = pSrc[सूचकांक++];
        यदि(ch <= 0x7f){
            req_दीर्घता++;
        }अन्यथा{
            ch=utf8_अग्र_Char_Safe_Body(pSrc, &सूचकांक, src_दीर्घता, ch, -3);
            req_दीर्घता+=U16_दीर्घता(ch);
        }
    }

    req_दीर्घता+=(int32_t)(pDest - dest);

    यदि(p_Dest_दीर्घता){
        *p_Dest_दीर्घता = req_दीर्घता;
    }

    /* Terminate the buffer */
    u_terminateUChars(dest,dest_क्षमता,req_दीर्घता,p_Error_संहिता);

    प्रत्यागम dest;
}



स्थिर int32_t U_CALLCONV
utf8TextExtract(UText *ut,
                int64_t start, int64_t सीमा,
                char16_t *dest, int32_t dest_क्षमता,
                UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता>0)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    int32_t  length  = ut->b;
    int32_t  start32 = pin_सूचकांक(start, length);
    int32_t  limit32 = pin_सूचकांक(सीमा, length);

    यदि(start32>limit32) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }


    // adjust the incoming indexes to land on code point boundaries if needed.
    //    adjust by no more than three, because that is the largest number of trail bytes
    //    in a well formed UTF8 character.
    ध्रुव uint8_t *buf = (ध्रुव uint8_t *)ut->context;
    पूर्णाङ्क i;
    यदि (start32 < ut->chunk_Native_सीमा) {
        प्रति (i=0; i<3; i++) {
            यदि (U8_IS_SINGLE(buf[start32]) || U8_IS_LEAD(buf[start32]) || start32==0) {
                भङ्ग;
            }
            start32--;
        }
    }

    यदि (limit32 < ut->chunk_Native_सीमा) {
        प्रति (i=0; i<3; i++) {
            यदि (U8_IS_SINGLE(buf[limit32]) || U8_IS_LEAD(buf[limit32]) || limit32==0) {
                भङ्ग;
            }
            limit32--;
        }
    }

    // Do the actual extract.
    int32_t dest_दीर्घता=0;
    utext_strFromUTF8(dest, dest_क्षमता, &dest_दीर्घता,
                    (ध्रुव वर्ण *)ut->context+start32, limit32-start32,
                    p_Error_संहिता);
    utf8TextAccess(ut, limit32, सत्य);
    प्रत्यागम dest_दीर्घता;
}

//
// utf8TextMapOffsetToNative
//
// Map a chunk (UTF-16) offset to a native index.
स्थिर int64_t U_CALLCONV
utf8_Text_Map_विस्थापनम्_गन्तव्यम्_Native(ध्रुव UText *ut) {
    //
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(ut->chunk_विस्थापनम्>ut->native_Indexing_सीमा && ut->chunk_विस्थापनम्<=ut->chunk_दीर्घता);
    int32_t native_विस्थापनम् = u8b->map_गन्तव्यम्_Native[ut->chunk_विस्थापनम् + u8b->bufStartIdx] + u8b->गन्तव्यम्_UChars_Map_Start;
    U_ASSERT(native_विस्थापनम् >= ut->chunkNativeStart && native_विस्थापनम् <= ut->chunk_Native_सीमा);
    प्रत्यागम native_विस्थापनम्;
}

//
// Map a native index to the corresponding chunk offset
//
स्थिर int32_t U_CALLCONV
utf8_Text_Map_सूचकांक_गन्तव्यम्_UTF16(ध्रुव UText *ut, int64_t index64) {
    U_ASSERT(index64 <= 0x7fffffff);
    int32_t सूचकांक = (int32_t)index64;
    UTF8Buf *u8b = (UTF8Buf *)ut->p;
    U_ASSERT(सूचकांक>=ut->chunkNativeStart+ut->native_Indexing_सीमा);
    U_ASSERT(सूचकांक<=ut->chunk_Native_सीमा);
    int32_t map_सूचकांक = सूचकांक - u8b->गन्तव्यम्_UChars_Map_Start;
    U_ASSERT(map_सूचकांक < (int32_t)परिमाण(UTF8Buf::map_गन्तव्यम्_UChars));
    int32_t विस्थापनम् = u8b->map_गन्तव्यम्_UChars[map_सूचकांक] - u8b->bufStartIdx;
    U_ASSERT(विस्थापनम्>=0 && विस्थापनम्<=ut->chunk_दीर्घता);
    प्रत्यागम विस्थापनम्;
}

स्थिर UText * U_CALLCONV
utf8TextClone(UText *dest, ध्रुव UText *src, UBool deep, UError_संहिता *स्थितिः)
{
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, स्थितिः);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //
    // TODO:  There is an issue with using utext_nativeLength().
    //        That function is non-const in cases where the input was NUL terminated
    //          and the length has not yet been determined.
    //        This function (clone()) is const.
    //        There potentially a thread safety issue lurking here.
    //
    यदि (deep && U_SUCCESS(*स्थितिः)) {
        int32_t  len = (int32_t)utext_native_दीर्घता((UText *)src);
        वर्ण *copyStr = (वर्ण *)uprv_malloc(len+1);
        यदि (copyStr == शून्यसूचकः) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            uprv_memcpy(copyStr, src->context, len+1);
            dest->context = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    प्रत्यागम dest;
}


स्थिर शून्य U_CALLCONV
utf8TextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is to delete the UTF8 string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    यदि (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        वर्ण *s = (वर्ण *)ut->context;
        uprv_free(s);
        ut->context = शून्यसूचकः;
    }
}

U_CDECL_अन्तः


स्थिर ध्रुव संरचना UTextFuncs utf8Funcs =
{
    परिमाण(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    utf8TextClone,
    utf8_Text_दीर्घता,
    utf8TextAccess,
    utf8TextExtract,
    शून्यसूचकः,                /* replace*/
    शून्यसूचकः,                /* copy   */
    utf8_Text_Map_विस्थापनम्_गन्तव्यम्_Native,
    utf8_Text_Map_सूचकांक_गन्तव्यम्_UTF16,
    utf8TextClose,
    शून्यसूचकः,                // spare 1
    शून्यसूचकः,                // spare 2
    शून्यसूचकः                 // spare 3
};


स्थिर ध्रुव वर्ण g_रिक्तानि_String[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUTF8(UText* ut U_LIFETIME_BOUND,
               ध्रुव वर्ण* s U_LIFETIME_BOUND,
               int64_t length,
               UError_संहिता* स्थितिः) {
    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(s==शून्यसूचकः && length==0) {
        s = g_रिक्तानि_String;
    }

    यदि(s==शून्यसूचकः || length<-1 || length>INT32_MAX) {
        *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    ut = utext_सज्जता(ut, परिमाण(UTF8Buf) * 2, स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम ut;
    }

    ut->pFuncs  = &utf8Funcs;
    ut->context = s;
    ut->b       = (int32_t)length;
    ut->c       = (int32_t)length;
    यदि (ut->c < 0) {
        ut->c = 0;
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE);
    }
    ut->p = ut->pExtra;
    ut->q = (वर्ण *)ut->pExtra + परिमाण(UTF8Buf);
    प्रत्यागम ut;

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
गणना { REP_TEXT_CHUNK_आकार=10 };

संरचना ReplExtra {
    /*
     * Chunk UChars.
     * +1 to simplify filling with surrogate pair at the end.
     */
    char16_t s[REP_TEXT_CHUNK_आकार+1];
};


U_CDECL_BEGIN

स्थिर UText * U_CALLCONV
repTextClone(UText *dest, ध्रुव UText *src, UBool deep, UError_संहिता *स्थितिः) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, स्थितिः);

    // For deep clones, make a copy of the Replaceable.
    //  The copied Replaceable storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    यदि (deep && U_SUCCESS(*स्थितिः)) {
        ध्रुव Replaceable *replSrc = (ध्रुव Replaceable *)src->context;
        dest->context = replSrc->clone();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    प्रत्यागम dest;
}


स्थिर शून्य U_CALLCONV
repTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the Replaceable if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    यदि (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        Replaceable *rep = (Replaceable *)ut->context;
        विलोपय rep;
        ut->context = शून्यसूचकः;
    }
}


स्थिर int64_t U_CALLCONV
rep_Text_दीर्घता(UText *ut) {
    ध्रुव Replaceable *replSrc = (ध्रुव Replaceable *)ut->context;
    int32_t  len = replSrc->length();
    प्रत्यागम len;
}


स्थिर UBool U_CALLCONV
repTextAccess(UText *ut, int64_t सूचकांक, UBool forward) {
    ध्रुव Replaceable *rep=(ध्रुव Replaceable *)ut->context;
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
    यदि(forward) {

        यदि (index32>=ut->chunkNativeStart && index32<ut->chunk_Native_सीमा) {
            // Buffer already contains the requested position.
            ut->chunk_विस्थापनम् = (int32_t)(सूचकांक - ut->chunkNativeStart);
            प्रत्यागम सत्य;
        }
        यदि (index32>=length && ut->chunk_Native_सीमा==length) {
            // Request for end of string, and buffer already extends up to it.
            // Can't get the data, but don't change the buffer.
            ut->chunk_विस्थापनम् = length - (int32_t)ut->chunkNativeStart;
            प्रत्यागम असत्य;
        }

        ut->chunk_Native_सीमा = सूचकांक + REP_TEXT_CHUNK_आकार - 1;
        // Going forward, so we want to have the buffer with stuff at and beyond
        //   the requested index.  The -1 gets us one code point before the
        //   requested index also, to handle the case of the index being on
        //   a trail surrogate of a surrogate pair.
        यदि(ut->chunk_Native_सीमा > length) {
            ut->chunk_Native_सीमा = length;
        }
        // unless buffer ran off end, start is index-1.
        ut->chunkNativeStart = ut->chunk_Native_सीमा - REP_TEXT_CHUNK_आकार;
        यदि(ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }
    } अन्यथा {
        // Reverse iteration.  Fill buffer with data preceding the requested index.
        यदि (index32>ut->chunkNativeStart && index32<=ut->chunk_Native_सीमा) {
            // Requested position already in buffer.
            ut->chunk_विस्थापनम् = index32 - (int32_t)ut->chunkNativeStart;
            प्रत्यागम सत्य;
        }
        यदि (index32==0 && ut->chunkNativeStart==0) {
            // Request for start, buffer already begins at start.
            //  No data, but keep the buffer as is.
            ut->chunk_विस्थापनम् = 0;
            प्रत्यागम असत्य;
        }

        // Figure out the bounds of the chunk to extract for reverse iteration.
        // Need to worry about chunk not splitting surrogate pairs, and while still
        // containing the data we need.
        // Fix by requesting a chunk that includes an extra char16_t at the end.
        // If this turns out to be a lead surrogate, we can lop it off and still have
        //   the data we wanted.
        ut->chunkNativeStart = index32 + 1 - REP_TEXT_CHUNK_आकार;
        यदि (ut->chunkNativeStart < 0) {
            ut->chunkNativeStart = 0;
        }

        ut->chunk_Native_सीमा = index32 + 1;
        यदि (ut->chunk_Native_सीमा > length) {
            ut->chunk_Native_सीमा = length;
        }
    }

    // Extract the new chunk of text from the Replaceable source.
    ReplExtra *ex = (ReplExtra *)ut->pExtra;
    // UnicodeString with its buffer a writable alias to the chunk buffer
    UnicodeString बफर(ex->s, 0 /*buffer length*/, REP_TEXT_CHUNK_आकार /*buffer capacity*/);
    rep->extractBetween((int32_t)ut->chunkNativeStart, (int32_t)ut->chunk_Native_सीमा, बफर);

    ut->chunkContents  = ex->s;
    ut->chunk_दीर्घता    = (int32_t)(ut->chunk_Native_सीमा - ut->chunkNativeStart);
    ut->chunk_विस्थापनम्    = (int32_t)(index32 - ut->chunkNativeStart);

    // Surrogate pairs from the input text must not span chunk boundaries.
    // If end of chunk could be the start of a surrogate, trim it off.
    यदि (ut->chunk_Native_सीमा < length &&
        U16_IS_LEAD(ex->s[ut->chunk_दीर्घता-1])) {
            ut->chunk_दीर्घता--;
            ut->chunk_Native_सीमा--;
            यदि (ut->chunk_विस्थापनम् > ut->chunk_दीर्घता) {
                ut->chunk_विस्थापनम् = ut->chunk_दीर्घता;
            }
        }

    // if the first char16_t in the chunk could be the trailing half of a surrogate pair,
    // trim it off.
    यदि(ut->chunkNativeStart>0 && U16_IS_TRAIL(ex->s[0])) {
        ++(ut->chunkContents);
        ++(ut->chunkNativeStart);
        --(ut->chunk_दीर्घता);
        --(ut->chunk_विस्थापनम्);
    }

    // adjust the index/chunkOffset to a code point boundary
    U16_समुच्चयः_CP_START(ut->chunkContents, 0, ut->chunk_विस्थापनम्);

    // Use fast indexing for get/setNativeIndex()
    ut->native_Indexing_सीमा = ut->chunk_दीर्घता;

    प्रत्यागम सत्य;
}



स्थिर int32_t U_CALLCONV
repTextExtract(UText *ut,
               int64_t start, int64_t सीमा,
               char16_t *dest, int32_t dest_क्षमता,
               UError_संहिता *स्थितिः) {
    ध्रुव Replaceable *rep=(ध्रुव Replaceable *)ut->context;
    int32_t  length=rep->length();

    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि(dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता>0)) {
        *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
    }
    यदि(start>सीमा) {
        *स्थितिः=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    int32_t  start32 = pin_सूचकांक(start, length);
    int32_t  limit32 = pin_सूचकांक(सीमा, length);

    // adjust start, limit if they point to trail half of surrogates
    यदि (start32<length && U16_IS_TRAIL(rep->charAt(start32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(start32))){
            start32--;
    }
    यदि (limit32<length && U16_IS_TRAIL(rep->charAt(limit32)) &&
        U_IS_SUPPLEMENTARY(rep->char32At(limit32))){
            limit32--;
    }

    length=limit32-start32;
    यदि(length>dest_क्षमता) {
        limit32 = start32 + dest_क्षमता;
    }
    UnicodeString बफर(dest, 0, dest_क्षमता); // writable alias
    rep->extractBetween(start32, limit32, बफर);
    repTextAccess(ut, limit32, सत्य);

    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, length, स्थितिः);
}

स्थिर int32_t U_CALLCONV
repTextReplace(UText *ut,
               int64_t start, int64_t सीमा,
               ध्रुव char16_t *src, int32_t length,
               UError_संहिता *स्थितिः) {
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t old_दीर्घता;

    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि(src==शून्यसूचकः && length!=0) {
        *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    old_दीर्घता=rep->length(); // will subtract from new length
    यदि(start>सीमा ) {
        *स्थितिः=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    int32_t start32 = pin_सूचकांक(start, old_दीर्घता);
    int32_t limit32 = pin_सूचकांक(सीमा, old_दीर्घता);

    // Snap start & limit to code point boundaries.
    यदि (start32<old_दीर्घता && U16_IS_TRAIL(rep->charAt(start32)) &&
        start32>0 && U16_IS_LEAD(rep->charAt(start32-1)))
    {
            start32--;
    }
    यदि (limit32<old_दीर्घता && U16_IS_LEAD(rep->charAt(limit32-1)) &&
        U16_IS_TRAIL(rep->charAt(limit32)))
    {
            limit32++;
    }

    // Do the actual replace operation using methods of the Replaceable class
    UnicodeString replStr(length < 0, src, length); // read-only alias
    rep->handleReplaceBetween(start32, limit32, replStr);
    int32_t new_दीर्घता = rep->length();
    int32_t दीर्घता_Delta = new_दीर्घता - old_दीर्घता;

    // Is the UText chunk buffer OK?
    यदि (ut->chunk_Native_सीमा > start32) {
        // this replace operation may have impacted the current chunk.
        // invalidate it, which will force a reload on the next access.
        invalidateChunk(ut);
    }

    // set the iteration position to the end of the newly inserted replacement text.
    int32_t new_सूचकांक_Pos = limit32 + दीर्घता_Delta;
    repTextAccess(ut, new_सूचकांक_Pos, सत्य);

    प्रत्यागम दीर्घता_Delta;
}


स्थिर शून्य U_CALLCONV
repTextCopy(UText *ut,
                int64_t start, int64_t सीमा,
                int64_t dest_सूचकांक,
                UBool move,
                UError_संहिता *स्थितिः)
{
    Replaceable *rep=(Replaceable *)ut->context;
    int32_t length=rep->length();

    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम;
    }
    यदि (start>सीमा || (start<dest_सूचकांक && dest_सूचकांक<सीमा))
    {
        *स्थितिः=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }

    int32_t start32     = pin_सूचकांक(start, length);
    int32_t limit32     = pin_सूचकांक(सीमा, length);
    int32_t destIndex32 = pin_सूचकांक(dest_सूचकांक, length);

    // TODO:  snap input parameters to code point boundaries.

    यदि(move) {
        // move: copy to destIndex, then replace original with nothing
        int32_t seg_दीर्घता=limit32-start32;
        rep->copy(start32, limit32, destIndex32);
        यदि(destIndex32<start32) {
            start32+=seg_दीर्घता;
            limit32+=seg_दीर्घता;
        }
        rep->handleReplaceBetween(start32, limit32, UnicodeString());
    } अन्यथा {
        // copy
        rep->copy(start32, limit32, destIndex32);
    }

    // If the change to the text touched the region in the chunk buffer,
    //  invalidate the buffer.
    int32_t first_Affected_सूचकांक = destIndex32;
    यदि (move && start32<first_Affected_सूचकांक) {
        first_Affected_सूचकांक = start32;
    }
    यदि (first_Affected_सूचकांक < ut->chunk_Native_सीमा) {
        // changes may have affected range covered by the chunk
        invalidateChunk(ut);
    }

    // Put iteration position at the newly inserted (moved) block,
    int32_t  native_Iter_सूचकांक = destIndex32 + limit32 - start32;
    यदि (move && destIndex32>start32) {
        // moved a block of text towards the end of the string.
        native_Iter_सूचकांक = destIndex32;
    }

    // Set position, reload chunk if needed.
    repTextAccess(ut, native_Iter_सूचकांक, सत्य);
}

स्थिर ध्रुव संरचना UTextFuncs repFuncs =
{
    परिमाण(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    repTextClone,
    rep_Text_दीर्घता,
    repTextAccess,
    repTextExtract,
    repTextReplace,
    repTextCopy,
    शून्यसूचकः,              // MapOffsetToNative,
    शून्यसूचकः,              // MapIndexToUTF16,
    repTextClose,
    शून्यसूचकः,              // spare 1
    शून्यसूचकः,              // spare 2
    शून्यसूचकः               // spare 3
};


U_CAPI UText* U_EXPORT2
utext_openReplaceable(UText* ut U_LIFETIME_BOUND,
                      Replaceable* rep U_LIFETIME_BOUND,
                      UError_संहिता* स्थितिः) {
    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(rep==शून्यसूचकः) {
        *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    ut = utext_सज्जता(ut, परिमाण(ReplExtra), स्थितिः);
    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम ut;
    }

    ut->providerProperties = I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    यदि(rep->has_Meta_दत्त()) {
        ut->providerProperties |=I32_FLAG(UTEXT_PROVIDER_HAS_META_दत्त);
    }

    ut->pFuncs  = &repFuncs;
    ut->context =  rep;
    प्रत्यागम ut;
}

U_CDECL_अन्तः








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


स्थिर UText * U_CALLCONV
unistrTextClone(UText *dest, ध्रुव UText *src, UBool deep, UError_संहिता *स्थितिः) {
    // First do a generic shallow clone.  Does everything needed for the UText struct itself.
    dest = shallowTextClone(dest, src, स्थितिः);

    // For deep clones, make a copy of the UnicodeSring.
    //  The copied UnicodeString storage is owned by the newly created UText clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    the UText.
    //
    यदि (deep && U_SUCCESS(*स्थितिः)) {
        ध्रुव UnicodeString *srcString = (ध्रुव UnicodeString *)src->context;
        dest->context = नूतन UnicodeString(*srcString);
        dest->chunkContents = ((ध्रुव UnicodeString *)dest->context)->get_बफर();
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);

        // with deep clone, the copy is writable, even when the source is not.
        dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    प्रत्यागम dest;
}

स्थिर शून्य U_CALLCONV
unistrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the UnicodeString if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    यदि (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        UnicodeString *str = (UnicodeString *)ut->context;
        विलोपय str;
        ut->context = शून्यसूचकः;
    }
}


स्थिर int64_t U_CALLCONV
unistr_Text_दीर्घता(UText *t) {
    प्रत्यागम ((ध्रुव UnicodeString *)t->context)->length();
}


स्थिर UBool U_CALLCONV
unistrTextAccess(UText *ut, int64_t सूचकांक, UBool  forward) {
    int32_t length  = ut->chunk_दीर्घता;
    ut->chunk_विस्थापनम् = pin_सूचकांक(सूचकांक, length);

    // Check whether request is at the start or end
    UBool retVal = (forward && सूचकांक<length) || (!forward && सूचकांक>0);
    प्रत्यागम retVal;
}



स्थिर int32_t U_CALLCONV
unistrTextExtract(UText *t,
                  int64_t start, int64_t सीमा,
                  char16_t *dest, int32_t dest_क्षमता,
                  UError_संहिता *p_Error_संहिता) {
    ध्रुव UnicodeString *us=(ध्रुव UnicodeString *)t->context;
    int32_t length=us->length();

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता>0)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    }
    यदि(start<0 || start>सीमा) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    int32_t start32 = start<length ? us->getChar32Start((int32_t)start) : length;
    int32_t limit32 = सीमा<length ? us->getChar32Start((int32_t)सीमा) : length;

    length=limit32-start32;
    यदि (dest_क्षमता>0 && dest!=शून्यसूचकः) {
        int32_t trimmed_दीर्घता = length;
        यदि(trimmed_दीर्घता>dest_क्षमता) {
            trimmed_दीर्घता=dest_क्षमता;
        }
        us->extract(start32, trimmed_दीर्घता, dest);
        t->chunk_विस्थापनम् = start32+trimmed_दीर्घता;
    } अन्यथा {
        t->chunk_विस्थापनम् = start32;
    }
    u_terminateUChars(dest, dest_क्षमता, length, p_Error_संहिता);
    प्रत्यागम length;
}

स्थिर int32_t U_CALLCONV
unistrTextReplace(UText *ut,
                  int64_t start, int64_t सीमा,
                  ध्रुव char16_t *src, int32_t length,
                  UError_संहिता *p_Error_संहिता) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t old_दीर्घता;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(src==शून्यसूचकः && length!=0) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    }
    यदि(start>सीमा) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    old_दीर्घता=us->length();
    int32_t start32 = pin_सूचकांक(start, old_दीर्घता);
    int32_t limit32 = pin_सूचकांक(सीमा, old_दीर्घता);
    यदि (start32 < old_दीर्घता) {
        start32 = us->getChar32Start(start32);
    }
    यदि (limit32 < old_दीर्घता) {
        limit32 = us->getChar32Start(limit32);
    }

    // replace
    us->replace(start32, limit32-start32, src, length);
    int32_t new_दीर्घता = us->length();

    // Update the chunk description.
    ut->chunkContents    = us->get_बफर();
    ut->chunk_दीर्घता      = new_दीर्घता;
    ut->chunk_Native_सीमा = new_दीर्घता;
    ut->native_Indexing_सीमा = new_दीर्घता;

    // Set iteration position to the point just following the newly inserted text.
    int32_t दीर्घता_Delta = new_दीर्घता - old_दीर्घता;
    ut->chunk_विस्थापनम् = limit32 + दीर्घता_Delta;

    प्रत्यागम दीर्घता_Delta;
}

स्थिर शून्य U_CALLCONV
unistrTextCopy(UText *ut,
               int64_t start, int64_t सीमा,
               int64_t dest_सूचकांक,
               UBool move,
               UError_संहिता *p_Error_संहिता) {
    UnicodeString *us=(UnicodeString *)ut->context;
    int32_t length=us->length();

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    int32_t start32 = pin_सूचकांक(start, length);
    int32_t limit32 = pin_सूचकांक(सीमा, length);
    int32_t destIndex32 = pin_सूचकांक(dest_सूचकांक, length);

    यदि( start32>limit32 || (start32<destIndex32 && destIndex32<limit32)) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }

    यदि(move) {
        // move: copy to destIndex, then remove original
        int32_t seg_दीर्घता=limit32-start32;
        us->copy(start32, limit32, destIndex32);
        यदि(destIndex32<start32) {
            start32+=seg_दीर्घता;
        }
        us->remove(start32, seg_दीर्घता);
    } अन्यथा {
        // copy
        us->copy(start32, limit32, destIndex32);
    }

    // update chunk description, set iteration position.
    ut->chunkContents = us->get_बफर();
    यदि (move==असत्य) {
        // copy operation, string length grows
        ut->chunk_दीर्घता += limit32-start32;
        ut->chunk_Native_सीमा = ut->chunk_दीर्घता;
        ut->native_Indexing_सीमा = ut->chunk_दीर्घता;
    }

    // Iteration position to end of the newly inserted text.
    ut->chunk_विस्थापनम् = destIndex32+limit32-start32;
    यदि (move && destIndex32>start32) {
        ut->chunk_विस्थापनम् = destIndex32;
    }

}

स्थिर ध्रुव संरचना UTextFuncs unistrFuncs =
{
    परिमाण(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    unistrTextClone,
    unistr_Text_दीर्घता,
    unistrTextAccess,
    unistrTextExtract,
    unistrTextReplace,
    unistrTextCopy,
    शून्यसूचकः,                // MapOffsetToNative,
    शून्यसूचकः,                // MapIndexToUTF16,
    unistrTextClose,
    शून्यसूचकः,                // spare 1
    शून्यसूचकः,                // spare 2
    शून्यसूचकः                 // spare 3
};



U_CDECL_अन्तः


U_CAPI UText* U_EXPORT2
utext_openUnicodeString(UText* ut U_LIFETIME_BOUND,
                        UnicodeString* s U_LIFETIME_BOUND,
                        UError_संहिता* स्थितिः) {
    ut = utext_openConstUnicodeString(ut, s, स्थितिः);
    यदि (U_SUCCESS(*स्थितिः)) {
        ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_WRITABLE);
    }
    प्रत्यागम ut;
}



U_CAPI UText* U_EXPORT2
utext_openConstUnicodeString(UText* ut U_LIFETIME_BOUND,
                             ध्रुव UnicodeString* s U_LIFETIME_BOUND,
                             UError_संहिता* स्थितिः) {
    यदि (U_SUCCESS(*स्थितिः) && s->isBogus()) {
        // The UnicodeString is bogus, but we still need to detach the UText
        //   from whatever it was hooked to before, if anything.
        utext_openUChars(ut, शून्यसूचकः, 0, स्थितिः);
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम ut;
    }
    ut = utext_सज्जता(ut, 0, स्थितिः);
    //    note:  use the standard (writable) function table for UnicodeString.
    //           The flag settings disable writing, so having the functions in
    //           the table is harmless.
    यदि (U_SUCCESS(*स्थितिः)) {
        ut->pFuncs              = &unistrFuncs;
        ut->context             = s;
        ut->providerProperties  = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        ut->chunkContents       = s->get_बफर();
        ut->chunk_दीर्घता         = s->length();
        ut->chunkNativeStart    = 0;
        ut->chunk_Native_सीमा    = ut->chunk_दीर्घता;
        ut->native_Indexing_सीमा = ut->chunk_दीर्घता;
    }
    प्रत्यागम ut;
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


स्थिर UText * U_CALLCONV
ucstrTextClone(UText *dest, ध्रुव UText * src, UBool deep, UError_संहिता * स्थितिः) {
    // First do a generic shallow clone.
    dest = shallowTextClone(dest, src, स्थितिः);

    // For deep clones, make a copy of the string.
    //  The copied storage is owned by the newly created clone.
    //  A non-nullptr pointer in UText.p is the signal to the close() function to delete
    //    it.
    //
    यदि (deep && U_SUCCESS(*स्थितिः)) {
        U_ASSERT(utext_native_दीर्घता(dest) < INT32_MAX);
        int32_t  len = (int32_t)utext_native_दीर्घता(dest);

        // The cloned string IS going to be NUL terminated, whether or not the original was.
        ध्रुव char16_t *srcStr = (ध्रुव char16_t *)src->context;
        char16_t *copyStr = (char16_t *)uprv_malloc((len+1) * परिमाण(char16_t));
        यदि (copyStr == शून्यसूचकः) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            int64_t i;
            प्रति (i=0; i<len; i++) {
                copyStr[i] = srcStr[i];
            }
            copyStr[len] = 0;
            dest->context = copyStr;
            dest->chunkContents = copyStr;
            dest->providerProperties |= I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT);
        }
    }
    प्रत्यागम dest;
}


स्थिर शून्य U_CALLCONV
ucstrTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the string if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    यदि (ut->providerProperties & I32_FLAG(UTEXT_PROVIDER_OWNS_TEXT)) {
        char16_t *s = (char16_t *)ut->context;
        uprv_free(s);
        ut->context = शून्यसूचकः;
    }
}



स्थिर int64_t U_CALLCONV
ucstr_Text_दीर्घता(UText *ut) {
    यदि (ut->a < 0) {
        // null terminated, we don't yet know the length. Scan for it.
        //    Access is not convenient for doing this
        //    because the current iteration position can't be changed.
        ध्रुव char16_t  *str = (ध्रुव char16_t *)ut->context;
        प्रति (;;) {
            यदि (str[ut->chunk_Native_सीमा] == 0) {
                भङ्ग;
            }
            ut->chunk_Native_सीमा++;
        }
        ut->a = ut->chunk_Native_सीमा;
        ut->chunk_दीर्घता = (int32_t)ut->chunk_Native_सीमा;
        ut->native_Indexing_सीमा = ut->chunk_दीर्घता;
        ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE);
    }
    प्रत्यागम ut->a;
}


स्थिर UBool U_CALLCONV
ucstrTextAccess(UText *ut, int64_t सूचकांक, UBool  forward) {
    ध्रुव char16_t *str   = (ध्रुव char16_t *)ut->context;

    // pin the requested index to the bounds of the string,
    //  and set current iteration position.
    यदि (सूचकांक<0) {
        सूचकांक = 0;
    } अन्यथा यदि (सूचकांक < ut->chunk_Native_सीमा) {
        // The request data is within the chunk as it is known so far.
        // Put index on a code point boundary.
        U16_समुच्चयः_CP_START(str, 0, सूचकांक);
    } अन्यथा यदि (ut->a >= 0) {
        // We know the length of this string, and the user is requesting something
        // at or beyond the length.  Pin the requested index to the length.
        सूचकांक = ut->a;
    } अन्यथा {
        // Null terminated string, length not yet known, and the requested index
        //  is beyond where we have scanned so far.
        //  Scan to 32 UChars beyond the requested index.  The strategy here is
        //  to avoid fully scanning a long string when the caller only wants to
        //  see a few characters at its beginning.
        int32_t अवलोकनम्_सीमा = (int32_t)सूचकांक + 32;
        यदि ((सूचकांक + 32)>INT32_MAX || (सूचकांक + 32)<0 ) {   // note: int64 expression
            अवलोकनम्_सीमा = INT32_MAX;
        }

        int32_t chunk_सीमा = (int32_t)ut->chunk_Native_सीमा;
        प्रति (; chunk_सीमा<अवलोकनम्_सीमा; chunk_सीमा++) {
            यदि (str[chunk_सीमा] == 0) {
                // We found the end of the string.  Remember it, pin the requested index to it,
                //  and bail out of here.
                ut->a = chunk_सीमा;
                ut->chunk_दीर्घता = chunk_सीमा;
                ut->native_Indexing_सीमा = chunk_सीमा;
                यदि (सूचकांक >= chunk_सीमा) {
                    सूचकांक = chunk_सीमा;
                } अन्यथा {
                    U16_समुच्चयः_CP_START(str, 0, सूचकांक);
                }

                ut->chunk_Native_सीमा = chunk_सीमा;
                ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE);
                गच्छ breakout;
            }
        }
        // We scanned through the next batch of UChars without finding the end.
        U16_समुच्चयः_CP_START(str, 0, सूचकांक);
        यदि (chunk_सीमा == INT32_MAX) {
            // Scanned to the limit of a 32 bit length.
            // Forceably trim the overlength string back so length fits in int32
            //  TODO:  add support for 64 bit strings.
            ut->a = chunk_सीमा;
            ut->chunk_दीर्घता = chunk_सीमा;
            ut->native_Indexing_सीमा = chunk_सीमा;
            यदि (सूचकांक > chunk_सीमा) {
                सूचकांक = chunk_सीमा;
            }
            ut->chunk_Native_सीमा = chunk_सीमा;
            ut->providerProperties &= ~I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE);
        } अन्यथा {
            // The endpoint of a chunk must not be left in the middle of a surrogate pair.
            // If the current end is on a lead surrogate, back the end up by one.
            // It doesn't matter if the end char happens to be an unpaired surrogate,
            //    and it's simpler not to worry about it.
            यदि (U16_IS_LEAD(str[chunk_सीमा-1])) {
                --chunk_सीमा;
            }
            // Null-terminated chunk with end still unknown.
            // Update the chunk length to reflect what has been scanned thus far.
            // That the full length is still unknown is (still) flagged by
            //    ut->a being < 0.
            ut->chunk_Native_सीमा = chunk_सीमा;
            ut->native_Indexing_सीमा = chunk_सीमा;
            ut->chunk_दीर्घता = chunk_सीमा;
        }

    }
breakout:
    U_ASSERT(सूचकांक<=INT32_MAX);
    ut->chunk_विस्थापनम् = (int32_t)सूचकांक;

    // Check whether request is at the start or end
    UBool retVal = (forward && सूचकांक<ut->chunk_Native_सीमा) || (!forward && सूचकांक>0);
    प्रत्यागम retVal;
}



स्थिर int32_t U_CALLCONV
ucstrTextExtract(UText *ut,
                  int64_t start, int64_t सीमा,
                  char16_t *dest, int32_t dest_क्षमता,
                  UError_संहिता *p_Error_संहिता)
{
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता>0) || start>सीमा) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    //const char16_t *s=(const char16_t *)ut->context;
    int32_t si, di;

    int32_t start32;
    int32_t limit32;

    // Access the start.  Does two things we need:
    //   Pins 'start' to the length of the string, if it came in out-of-bounds.
    //   Snaps 'start' to the beginning of a code point.
    ucstrTextAccess(ut, start, सत्य);
    ध्रुव char16_t *s=ut->chunkContents;
    start32 = ut->chunk_विस्थापनम्;

    int32_t str_दीर्घता=(int32_t)ut->a;
    यदि (str_दीर्घता >= 0) {
        limit32 = pin_सूचकांक(सीमा, str_दीर्घता);
    } अन्यथा {
        limit32 = pin_सूचकांक(सीमा, INT32_MAX);
    }
    di = 0;
    प्रति (si=start32; si<limit32; si++) {
        यदि (str_दीर्घता<0 && s[si]==0) {
            // Just hit the end of a null-terminated string.
            ut->a = si;               // set string length for this UText
            ut->chunk_Native_सीमा    = si;
            ut->chunk_दीर्घता         = si;
            ut->native_Indexing_सीमा = si;
            str_दीर्घता               = si;
            limit32                 = si;
            भङ्ग;
        }
        U_ASSERT(di>=0); /* to ensure di never exceeds INT32_MAX, which must not happen logically */
        यदि (di<dest_क्षमता) {
            // only store if there is space.
            dest[di] = s[si];
        } अन्यथा {
            यदि (str_दीर्घता>=0) {
                // We have filled the destination buffer, and the string length is known.
                //  Cut the loop short.  There is no need to scan string termination.
                di = limit32 - start32;
                si = limit32;
                भङ्ग;
            }
        }
        di++;
    }

    // If the limit index points to a lead surrogate of a pair,
    //   add the corresponding trail surrogate to the destination.
    यदि (si>0 && U16_IS_LEAD(s[si-1]) &&
            ((si<str_दीर्घता || str_दीर्घता<0)  && U16_IS_TRAIL(s[si])))
    {
        यदि (di<dest_क्षमता) {
            // store only if there is space in the output buffer.
            dest[di++] = s[si];
        }
        si++;
    }

    // Put iteration position at the point just following the extracted text
    यदि (si <= ut->chunk_Native_सीमा) {
        ut->chunk_विस्थापनम् = si;
    } अन्यथा {
        ucstrTextAccess(ut, si, सत्य);
    }

    // Add a terminating NUL if space in the buffer permits,
    // and set the error status as required.
    u_terminateUChars(dest, dest_क्षमता, di, p_Error_संहिता);
    प्रत्यागम di;
}

स्थिर ध्रुव संरचना UTextFuncs ucstrFuncs =
{
    परिमाण(UTextFuncs),
    0, 0, 0,           // Reserved alignment padding
    ucstrTextClone,
    ucstr_Text_दीर्घता,
    ucstrTextAccess,
    ucstrTextExtract,
    शून्यसूचकः,              // Replace
    शून्यसूचकः,              // Copy
    शून्यसूचकः,              // MapOffsetToNative,
    शून्यसूचकः,              // MapIndexToUTF16,
    ucstrTextClose,
    शून्यसूचकः,              // spare 1
    शून्यसूचकः,              // spare 2
    शून्यसूचकः,              // spare 3
};

U_CDECL_अन्तः

स्थिर ध्रुव char16_t g_रिक्तानि_UString[] = {0};

U_CAPI UText* U_EXPORT2
utext_openUChars(UText* ut U_LIFETIME_BOUND,
                 ध्रुव char16_t* s U_LIFETIME_BOUND,
                 int64_t length,
                 UError_संहिता* स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(s==शून्यसूचकः && length==0) {
        s = g_रिक्तानि_UString;
    }
    यदि (s==शून्यसूचकः || length < -1 || length>INT32_MAX) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    ut = utext_सज्जता(ut, 0, स्थितिः);
    यदि (U_SUCCESS(*स्थितिः)) {
        ut->pFuncs               = &ucstrFuncs;
        ut->context              = s;
        ut->providerProperties   = I32_FLAG(UTEXT_PROVIDER_STABLE_CHUNKS);
        यदि (length==-1) {
            ut->providerProperties |= I32_FLAG(UTEXT_PROVIDER_दीर्घता_IS_EXPENSIVE);
        }
        ut->a                    = length;
        ut->chunkContents        = s;
        ut->chunkNativeStart     = 0;
        ut->chunk_Native_सीमा     = length>=0? length : 0;
        ut->chunk_दीर्घता          = (int32_t)ut->chunk_Native_सीमा;
        ut->chunk_विस्थापनम्          = 0;
        ut->native_Indexing_सीमा  = ut->chunk_दीर्घता;
    }
    प्रत्यागम ut;
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
#परिभाषय CIBuf_आकार 16

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
charIterTextClose(UText *ut) {
    // Most of the work of close is done by the generic UText framework close.
    // All that needs to be done here is delete the CharacterIterator if the UText
    //  owns it.  This occurs if the UText was created by cloning.
    CharacterIterator *ci = (CharacterIterator *)ut->r;
    विलोपय ci;
    ut->r = शून्यसूचकः;
}

स्थिर int64_t U_CALLCONV
char_Iter_Text_दीर्घता(UText *ut) {
    प्रत्यागम (int32_t)ut->a;
}

स्थिर UBool U_CALLCONV
charIterTextAccess(UText *ut, int64_t सूचकांक, UBool  forward) {
    CharacterIterator *ci   = (CharacterIterator *)ut->context;

    int32_t clipped_सूचकांक = (int32_t)सूचकांक;
    यदि (clipped_सूचकांक<0) {
        clipped_सूचकांक=0;
    } अन्यथा यदि (clipped_सूचकांक>=ut->a) {
        clipped_सूचकांक=(int32_t)ut->a;
    }
    int32_t needed_सूचकांक = clipped_सूचकांक;
    यदि (!forward && needed_सूचकांक>0) {
        // reverse iteration, want the position just before what was asked for.
        needed_सूचकांक--;
    } अन्यथा यदि (forward && needed_सूचकांक==ut->a && needed_सूचकांक>0) {
        // Forward iteration, don't ask for something past the end of the text.
        needed_सूचकांक--;
    }

    // Find the native index of the start of the buffer containing what we want.
    needed_सूचकांक -= needed_सूचकांक % CIBuf_आकार;

    char16_t *buf = शून्यसूचकः;
    UBool  need_Chunk_सज्जता = सत्य;
    पूर्णाङ्क    i;
    यदि (ut->chunkNativeStart == needed_सूचकांक) {
        // The buffer we want is already the current chunk.
        need_Chunk_सज्जता = असत्य;
    } अन्यथा यदि (ut->b == needed_सूचकांक) {
        // The first buffer (buffer p) has what we need.
        buf = (char16_t *)ut->p;
    } अन्यथा यदि (ut->c == needed_सूचकांक) {
        // The second buffer (buffer q) has what we need.
        buf = (char16_t *)ut->q;
    } अन्यथा {
        // Neither buffer already has what we need.
        // Load new data from the character iterator.
        // Use the buf that is not the current buffer.
        buf = (char16_t *)ut->p;
        यदि (ut->p == ut->chunkContents) {
            buf = (char16_t *)ut->q;
        }
        ci->समुच्चयः_सूचकांक(needed_सूचकांक);
        प्रति (i=0; i<CIBuf_आकार; i++) {
            buf[i] = ci->अग्र_Post_Inc();
            यदि (i+needed_सूचकांक > ut->a) {
                भङ्ग;
            }
        }
    }

    // We have a buffer with the data we need.
    // Set it up as the current chunk, if it wasn't already.
    यदि (need_Chunk_सज्जता) {
        ut->chunkContents = buf;
        ut->chunk_दीर्घता   = CIBuf_आकार;
        ut->chunkNativeStart = needed_सूचकांक;
        ut->chunk_Native_सीमा = needed_सूचकांक + CIBuf_आकार;
        यदि (ut->chunk_Native_सीमा > ut->a) {
            ut->chunk_Native_सीमा = ut->a;
            ut->chunk_दीर्घता  = (int32_t)(ut->chunk_Native_सीमा)-(int32_t)(ut->chunkNativeStart);
        }
        ut->native_Indexing_सीमा = ut->chunk_दीर्घता;
        U_ASSERT(ut->chunk_विस्थापनम्>=0 && ut->chunk_विस्थापनम्<=CIBuf_आकार);
    }
    ut->chunk_विस्थापनम् = clipped_सूचकांक - (int32_t)ut->chunkNativeStart;
    UBool success = (forward? ut->chunk_विस्थापनम्<ut->chunk_दीर्घता : ut->chunk_विस्थापनम्>0);
    प्रत्यागम success;
}

स्थिर UText * U_CALLCONV
charIterTextClone(UText *dest, ध्रुव UText *src, UBool deep, UError_संहिता * स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि (deep) {
        // There is no CharacterIterator API for cloning the underlying text storage.
        *स्थितिः = U_UNSUPPORTED_ERROR;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा {
        CharacterIterator *srcCI =(CharacterIterator *)src->context;
        srcCI = srcCI->clone();
        dest = utext_openCharacterIterator(dest, srcCI, स्थितिः);
        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम dest;
        }
        // cast off const on getNativeIndex.
        //   For CharacterIterator based UTexts, this is safe, the operation is const.
        int64_t  ix = utext_get_Native_सूचकांक((UText *)src);
        utext_समुच्चयः_Native_सूचकांक(dest, ix);
        dest->r = srcCI;    // flags that this UText owns the CharacterIterator
    }
    प्रत्यागम dest;
}

स्थिर int32_t U_CALLCONV
charIterTextExtract(UText *ut,
                  int64_t start, int64_t सीमा,
                  char16_t *dest, int32_t dest_क्षमता,
                  UError_संहिता *स्थितिः)
{
    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि(dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता>0) || start>सीमा) {
        *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    int32_t  length  = (int32_t)ut->a;
    int32_t  start32 = pin_सूचकांक(start, length);
    int32_t  limit32 = pin_सूचकांक(सीमा, length);
    int32_t  desti   = 0;
    int32_t  srci;
    int32_t  copy_सीमा;

    CharacterIterator *ci = (CharacterIterator *)ut->context;
    ci->समुच्चयः_Index32(start32);   // Moves ix to lead of surrogate pair, if needed.
    srci = ci->get_सूचकांक();
    copy_सीमा = srci;
    यावत् (srci<limit32) {
        UChar32 c = ci->next32PostInc();
        int32_t  len = U16_दीर्घता(c);
        U_ASSERT(desti+len>0); /* to ensure desti+len never exceeds MAX_INT32, which must not happen logically */
        यदि (desti+len <= dest_क्षमता) {
            U16_APPEND_UNSAFE(dest, desti, c);
            copy_सीमा = srci+len;
        } अन्यथा {
            desti += len;
            *स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
        }
        srci += len;
    }

    charIterTextAccess(ut, copy_सीमा, सत्य);

    u_terminateUChars(dest, dest_क्षमता, desti, स्थितिः);
    प्रत्यागम desti;
}

स्थिर ध्रुव संरचना UTextFuncs charIterFuncs =
{
    परिमाण(UTextFuncs),
    0, 0, 0,             // Reserved alignment padding
    charIterTextClone,
    char_Iter_Text_दीर्घता,
    charIterTextAccess,
    charIterTextExtract,
    शून्यसूचकः,                // Replace
    शून्यसूचकः,                // Copy
    शून्यसूचकः,                // MapOffsetToNative,
    शून्यसूचकः,                // MapIndexToUTF16,
    charIterTextClose,
    शून्यसूचकः,                // spare 1
    शून्यसूचकः,                // spare 2
    शून्यसूचकः                 // spare 3
};
U_CDECL_अन्तः

U_CAPI UText* U_EXPORT2
utext_openCharacterIterator(UText* ut U_LIFETIME_BOUND,
                            CharacterIterator* ci U_LIFETIME_BOUND,
                            UError_संहिता* स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि (ci->start_सूचकांक() > 0) {
        // No support for CharacterIterators that do not start indexing from zero.
        *स्थितिः = U_UNSUPPORTED_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    // Extra space in UText for 2 buffers of CIBufSize UChars each.
    int32_t  extraSpace = 2 * CIBuf_आकार * परिमाण(char16_t);
    ut = utext_सज्जता(ut, extraSpace, स्थितिः);
    यदि (U_SUCCESS(*स्थितिः)) {
        ut->pFuncs                = &charIterFuncs;
        ut->context              = ci;
        ut->providerProperties   = 0;
        ut->a                    = ci->अन्तः_सूचकांक();        // Length of text
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
        ut->chunk_विस्थापनम्          = 1;
        ut->chunk_Native_सीमा     = 0;
        ut->chunk_दीर्घता          = 0;
        ut->native_Indexing_सीमा  = ut->chunk_विस्थापनम्;  // enables native indexing
    }
    प्रत्यागम ut;
}

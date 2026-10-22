// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2016 International Business Machines Corporation
*   and others. All rights reserved.
***************************************************************************
*/
//
//  file:  rbbi.cpp  Contains the implementation of the rule based break iterator
//                   runtime engine and the API implementation for
//                   class RuleBasedBreakIterator
//

#അന്തര്ഭാവ "utypeinfo.h"  // for 'typeid' to work

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ <cinttypes>

#അന്തര്ഭാവ "unicode/rbbi.h"
#അന്തര്ഭാവ "unicode/schriter.h"
#അന്തര്ഭാവ "unicode/uchriter.h"
#അന്തര്ഭാവ "unicode/uclean.h"
#അന്തര്ഭാവ "unicode/udata.h"

#അന്തര്ഭാവ "brkeng.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "localsvc.h"
#അന്തര്ഭാവ "rbbidata.h"
#അന്തര്ഭാവ "rbbi_cache.h"
#അന്തര്ഭാവ "rbbirb.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uvectr32.h"

#ifdef RBBI_DEBUG
സ്ഥിതം UBool gTrace = അസത്യം;
#endif

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t START_STATE = 1;

// The state-transition value indicating "stop"
constexpr int32_t STOP_STATE = 0;


UOBJECT_DEFINE_RTTI_IMPLEMENTATION(RuleBasedBreakIterator)


//=======================================================================
// constructors
//=======================================================================

/**
 * Constructs a RuleBasedBreakIterator that uses the already-created
 * tables object that is passed in as a parameter.
 */
RuleBasedBreakIterator::RuleBasedBreakIterator(RBBIDataHeader* data, UErrorCode &status)
 : RuleBasedBreakIterator(&status)
{
    fData = പുതിയ RBBIDataWrapper(data, status); // status checked in constructor
    എങ്കിൽ (U_FAILURE(status)) {തിരികെ;}
    എങ്കിൽ(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    എങ്കിൽ (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * വലുപ്പം(int32_t)));
        എങ്കിൽ (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
    }
}

//-------------------------------------------------------------------------------
//
//   Constructor   from a UDataMemory handle to precompiled break rules
//                 stored in an ICU data file. This construcotr is private API,
//                 only for internal use.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator(UDataMemory* udm, UBool isPhraseBreaking,
        UErrorCode &status) : RuleBasedBreakIterator(udm, status)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
RuleBasedBreakIterator::RuleBasedBreakIterator(സ്ഥിരം uint8_t *compiledRules,
                       uint32_t       ruleLength,
                       UErrorCode     &status)
 : RuleBasedBreakIterator(&status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (compiledRules == nullptr || ruleLength < വലുപ്പം(RBBIDataHeader)) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    സ്ഥിരം RBBIDataHeader* data = reinterpret_cast<സ്ഥിരം RBBIDataHeader*>(compiledRules);
    എങ്കിൽ (data->fLength > ruleLength) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    fData = പുതിയ RBBIDataWrapper(data, RBBIDataWrapper::kDontAdopt, status);
    എങ്കിൽ (U_FAILURE(status)) {തിരികെ;}
    എങ്കിൽ(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    എങ്കിൽ (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * വലുപ്പം(int32_t)));
        എങ്കിൽ (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
    }
}


//-------------------------------------------------------------------------------
//
//   Constructor   from a UDataMemory handle to precompiled break rules
//                 stored in an ICU data file.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator(UDataMemory* udm, UErrorCode &status)
 : RuleBasedBreakIterator(&status)
{
    fData = പുതിയ RBBIDataWrapper(udm, status); // status checked in constructor
    എങ്കിൽ (U_FAILURE(status)) {തിരികെ;}
    എങ്കിൽ(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    എങ്കിൽ (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * വലുപ്പം(int32_t)));
        എങ്കിൽ (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator( സ്ഥിരം UnicodeString  &rules,
                                                UParseError          &parseError,
                                                UErrorCode           &status)
 : RuleBasedBreakIterator(&status)
{
    എങ്കിൽ (U_FAILURE(status)) {തിരികെ;}
    RuleBasedBreakIterator *bi = (RuleBasedBreakIterator *)
        RBBIRuleBuilder::createRuleBasedBreakIterator(rules, &parseError, status);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    എങ്കിൽ (U_SUCCESS(status)) {
        *ഇത് = *bi;
        delete bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator()
 : RuleBasedBreakIterator(nullptr)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
RuleBasedBreakIterator::RuleBasedBreakIterator(UErrorCode *status) {
    UErrorCode ec = U_ZERO_ERROR;
    എങ്കിൽ (status == nullptr) {
        status = &ec;
    }
    utext_openUChars(&fText, nullptr, 0, status);
    LocalPointer<DictionaryCache> lpDictionaryCache(പുതിയ DictionaryCache(ഇത്, *status), *status);
    LocalPointer<BreakCache> lpBreakCache(പുതിയ BreakCache(ഇത്, *status), *status);
    എങ്കിൽ (U_FAILURE(*status)) {
        fErrorCode = *status;
        തിരികെ;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#ifdef RBBI_DEBUG
    സ്ഥിതം UBool debugInitDone = അസത്യം;
    എങ്കിൽ (debugInitDone == അസത്യം) {
        അക്ഷരം *debugEnv = getenv("U_RBBIDEBUG");
        എങ്കിൽ (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = സത്യം;
        }
        debugInitDone = സത്യം;
    }
#endif
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator(സ്ഥിരം RuleBasedBreakIterator& other)
: RuleBasedBreakIterator()
{
    *ഇത് = other;
}


/**
 * Destructor
 */
RuleBasedBreakIterator::~RuleBasedBreakIterator() {
    എങ്കിൽ (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        delete fCharIter;
    }
    fCharIter = nullptr;

    utext_close(&fText);

    എങ്കിൽ (fData != nullptr) {
        fData->removeReference();
        fData = nullptr;
    }
    delete fBreakCache;
    fBreakCache = nullptr;

    delete fDictionaryCache;
    fDictionaryCache = nullptr;

    delete fLanguageBreakEngines;
    fLanguageBreakEngines = nullptr;

    delete fUnhandledBreakEngine;
    fUnhandledBreakEngine = nullptr;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = nullptr;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
RuleBasedBreakIterator&
RuleBasedBreakIterator::operator=(സ്ഥിരം RuleBasedBreakIterator& that) U_LIFETIME_BOUND {
    എങ്കിൽ (ഇത് == &that) {
        തിരികെ *ഇത്;
    }
    BreakIterator::operator=(that);

    എങ്കിൽ (fLanguageBreakEngines != nullptr) {
        delete fLanguageBreakEngines;
        fLanguageBreakEngines = nullptr;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UErrorCode status = U_ZERO_ERROR;
    utext_clone(&fText, &that.fText, അസത്യം, സത്യം, &status);

    എങ്കിൽ (fCharIter != &fSCharIter) {
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    എങ്കിൽ (that.fCharIter != nullptr && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    എങ്കിൽ (fCharIter == nullptr) {
        fCharIter = &fSCharIter;
    }

    എങ്കിൽ (fData != nullptr) {
        fData->removeReference();
        fData = nullptr;
    }
    എങ്കിൽ (that.fData != nullptr) {
        fData = that.fData->addReference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = nullptr;
    എങ്കിൽ (fData && fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * വലുപ്പം(int32_t)));
    }


    fPosition = that.fPosition;
    fRuleStatusIndex = that.fRuleStatusIndex;
    fDone = that.fDone;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, fRuleStatusIndex);
    fDictionaryCache->reset();

    തിരികെ *ഇത്;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
RuleBasedBreakIterator*
RuleBasedBreakIterator::clone() സ്ഥിരം {
    തിരികെ പുതിയ RuleBasedBreakIterator(*ഇത്);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
തർക്കം
RuleBasedBreakIterator::operator==(സ്ഥിരം BreakIterator& that) സ്ഥിരം {
    എങ്കിൽ (typeid(*ഇത്) != typeid(that)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (ഇത് == &that) {
        തിരികെ സത്യം;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    സ്ഥിരം RuleBasedBreakIterator& that2 = static_cast<സ്ഥിരം RuleBasedBreakIterator&>(that);

    എങ്കിൽ (!utext_equals(&fText, &that2.fText)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        തിരികെ അസത്യം;
    }

    എങ്കിൽ (!(fPosition == that2.fPosition &&
            fRuleStatusIndex == that2.fRuleStatusIndex &&
            fDone == that2.fDone &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        തിരികെ അസത്യം;
    }

    എങ്കിൽ (that2.fData == fData ||
        (fData != nullptr && that2.fData != nullptr && *that2.fData == *fData)) {
            // The two break iterators are using the same rules.
            തിരികെ സത്യം;
        }
    തിരികെ അസത്യം;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
RuleBasedBreakIterator::hashCode() സ്ഥിരം {
    int32_t   hash = 0;
    എങ്കിൽ (fData != nullptr) {
        hash = fData->hashCode();
    }
    തിരികെ hash;
}


ശൂന്യം RuleBasedBreakIterator::setText(UText *ut, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&fText, ut, അസത്യം, സത്യം, &status);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.setText(u"", 0);

    എങ്കിൽ (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    ഇത്->first();
}


UText* RuleBasedBreakIterator::getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) സ്ഥിരം {
    UText *result = utext_clone(fillIn, &fText, അസത്യം, സത്യം, &status);
    തിരികെ result;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
RuleBasedBreakIterator::getText() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
ശൂന്യം
RuleBasedBreakIterator::adoptText(CharacterIterator* newText) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    എങ്കിൽ (fCharIter != &fSCharIter) {
        delete fCharIter;
    }

    fCharIter = newText;
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    എങ്കിൽ (newText==nullptr || newText->startIndex() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&fText, nullptr, 0, &status);
    } അല്ലെങ്കിൽ {
        utext_openCharacterIterator(&fText, newText, &status);
    }
    ഇത്->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
ശൂന്യം
RuleBasedBreakIterator::setText(സ്ഥിരം UnicodeString& newText) {
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&fText, &newText, &status);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.setText(newText.getBuffer(), newText.length());

    എങ്കിൽ (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    ഇത്->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
RuleBasedBreakIterator&
RuleBasedBreakIterator::refresh_നിവേശം_Text(UText* input, UErrorCode& status) U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (input == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ *ഇത്;
    }
    int64_t pos = utext_getNativeIndex(&fText);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&fText, input, അസത്യം, സത്യം, &status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ *ഇത്;
    }
    utext_setNativeIndex(&fText, pos);
    എങ്കിൽ (utext_getNativeIndex(&fText) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    തിരികെ *ഇത്;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t RuleBasedBreakIterator::first() {
    UErrorCode status = U_ZERO_ERROR;
    എങ്കിൽ (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, status);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    തിരികെ 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t RuleBasedBreakIterator::last() {
    int32_t endPos = static_cast<int32_t>(utext_nativeLength(&fText));
    UBool endShouldBeBoundary = isBoundary(endPos);      // Has side effect of setting iterator position.
    (ശൂന്യം)endShouldBeBoundary;
    U_ASSERT(endShouldBeBoundary);
    U_ASSERT(fPosition == endPos);
    തിരികെ endPos;
}

/**
 * Advances the iterator either forward or backward the specified number of steps.
 * Negative values move backward, and positive values move forward.  This is
 * equivalent to repeatedly calling next() or previous().
 * @param n The number of steps to move.  The sign indicates the direction
 * (negative is backwards, and positive is forwards).
 * @return The character offset of the boundary position n boundaries away from
 * the current one.
 */
int32_t RuleBasedBreakIterator::next(int32_t n) {
    int32_t result = 0;
    എങ്കിൽ (n > 0) {
        വേണ്ടി (; n > 0 && result != UBRK_DONE; --n) {
            result = next();
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (n < 0) {
        വേണ്ടി (; n < 0 && result != UBRK_DONE; ++n) {
            result = previous();
        }
    } അല്ലെങ്കിൽ {
        result = current();
    }
    തിരികെ result;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t RuleBasedBreakIterator::next() {
    fBreakCache->next();
    തിരികെ fDone ? UBRK_DONE : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t RuleBasedBreakIterator::previous() {
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->previous(status);
    തിരികെ fDone ? UBRK_DONE : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t RuleBasedBreakIterator::following(int32_t startPos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    എങ്കിൽ (startPos < 0) {
        തിരികെ first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_setNativeIndex(&fText, startPos);
    startPos = static_cast<int32_t>(utext_getNativeIndex(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->following(startPos, status);
    തിരികെ fDone ? UBRK_DONE : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t RuleBasedBreakIterator::preceding(int32_t offset) {
    എങ്കിൽ (offset > utext_nativeLength(&fText)) {
        തിരികെ last();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_setNativeIndex(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_getNativeIndex(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->preceding(adjustedOffset, status);
    തിരികെ fDone ? UBRK_DONE : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool RuleBasedBreakIterator::isBoundary(int32_t offset) {
    // out-of-range indexes are never boundary positions
    എങ്കിൽ (offset < 0) {
        first();       // For side effects on current position, tag values.
        തിരികെ അസത്യം;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_setNativeIndex(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_getNativeIndex(&fText));

    തർക്കം result = അസത്യം;
    UErrorCode status = U_ZERO_ERROR;
    എങ്കിൽ (fBreakCache->seek(adjustedOffset) || fBreakCache->populateNear(adjustedOffset, status)) {
        result = (fBreakCache->current() == offset);
    }

    എങ്കിൽ (result && adjustedOffset < offset && utext_char32At(&fText, offset) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (!result) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    തിരികെ result;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t RuleBasedBreakIterator::current() സ്ഥിരം {
    തിരികെ fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
എണ്ണൽ RBBIRunMode {
    RBBI_START,     // state machine processing is before first char of input
    RBBI_RUN,       // state machine processing is in the user text
    RBBI_END        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
സ്ഥിതം വരിയിൽ uint16_t TrieFunc8(സ്ഥിരം UCPTrie *trie, UChar32 c) {
    തിരികെ UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

സ്ഥിതം വരിയിൽ uint16_t TrieFunc16(സ്ഥിരം UCPTrie *trie, UChar32 c) {
    തിരികെ UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t RuleBasedBreakIterator::handleNext() {
    സ്ഥിരം RBBIStateTable *statetable = fData->fForwardTable;
    തർക്കം use8BitsTrie = ucptrie_get_മൂല്യം_Width(fData->fTrie) == UCPTRIE_മൂല്യം_BITS_8;
    എങ്കിൽ (statetable->fFlags & RBBI_8BITS_ROWS) {
        എങ്കിൽ (use8BitsTrie) {
            തിരികെ handleNext<RBBIStateTableRow8, TrieFunc8>();
        } അല്ലെങ്കിൽ {
            തിരികെ handleNext<RBBIStateTableRow8, TrieFunc16>();
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (use8BitsTrie) {
            തിരികെ handleNext<RBBIStateTableRow16, TrieFunc8>();
        } അല്ലെങ്കിൽ {
            തിരികെ handleNext<RBBIStateTableRow16, TrieFunc16>();
        }
    }
}

int32_t RuleBasedBreakIterator::handleSafePrevious(int32_t fromPosition) {
    സ്ഥിരം RBBIStateTable *statetable = fData->fReverseTable;
    തർക്കം use8BitsTrie = ucptrie_get_മൂല്യം_Width(fData->fTrie) == UCPTRIE_മൂല്യം_BITS_8;
    എങ്കിൽ (statetable->fFlags & RBBI_8BITS_ROWS) {
        എങ്കിൽ (use8BitsTrie) {
            തിരികെ handleSafePrevious<RBBIStateTableRow8, TrieFunc8>(fromPosition);
        } അല്ലെങ്കിൽ {
            തിരികെ handleSafePrevious<RBBIStateTableRow8, TrieFunc16>(fromPosition);
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (use8BitsTrie) {
            തിരികെ handleSafePrevious<RBBIStateTableRow16, TrieFunc8>(fromPosition);
        } അല്ലെങ്കിൽ {
            തിരികെ handleSafePrevious<RBBIStateTableRow16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
template <typename RowType, RuleBasedBreakIterator::PTrieFunc trieFunc>
int32_t RuleBasedBreakIterator::handleNext() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRunMode         mode;

    RowType             *row;
    UChar32             c;
    int32_t             result             = 0;
    int32_t             initialPosition    = 0;
    സ്ഥിരം RBBIStateTable *statetable       = fData->fForwardTable;
    സ്ഥിരം അക്ഷരം         *tableData          = statetable->fTableData;
    uint32_t            tableRowLen        = statetable->fRowLen;
    uint32_t            dictStart          = statetable->fDictCategoriesStart;
    #ifdef RBBI_DEBUG
        എങ്കിൽ (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #endif

    // handleNext always sets the break tag value.
    // Set the default for it.
    fRuleStatusIndex = 0;

    f_Dictionary_Char_എണ്ണം = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&fText, initialPosition);
    result          = initialPosition;
    c               = UTEXT_NEXT32(&fText);
    എങ്കിൽ (c==U_SENTINEL) {
        fDone = സത്യം;
        തിരികെ UBRK_DONE;
    }

    //  Set the initial state for the state machine
    state = START_STATE;
    row = (RowType *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (tableData + tableRowLen * state);


    mode     = RBBI_RUN;


    // loop until we reach the end of the text or transition to state 0
    //
    വേണ്ടി (;;) {
        എങ്കിൽ (c == U_SENTINEL) {
            // Reached end of input string.
            എങ്കിൽ (mode == RBBI_END) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                നിർത്തുക;
            }
            // Run the loop one last time with the fake end-of-input character category.
            mode = RBBI_END;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        എങ്കിൽ (mode == RBBI_RUN) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(fData->fTrie, c);
            f_Dictionary_Char_എണ്ണം += (category >= dictStart);
        }

       #ifdef RBBI_DEBUG
            എങ്കിൽ (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_getNativeIndex(&fText));
                എങ്കിൽ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } അല്ലെങ്കിൽ {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<fData->fHeader->f_Cat_എണ്ണം);
        state = row->fNextState[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (tableData + tableRowLen * state);


        uint16_t accepting = row->fAccepting;
        എങ്കിൽ (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            എങ്കിൽ (mode != RBBI_START) {
                result = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            }
            fRuleStatusIndex = row->fTagsIdx;   // Remember the break status (tag) values.
        } അല്ലെങ്കിൽ എങ്കിൽ (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < fData->fForwardTable->fLookAheadResultsSize);
            int32_t lookaheadResult = fLookAheadMatches[accepting];
            എങ്കിൽ (lookaheadResult >= 0) {
                fRuleStatusIndex = row->fTagsIdx;
                fPosition = lookaheadResult;
                തിരികെ lookaheadResult;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t rule = row->fLookAhead;
        U_ASSERT(rule == 0 || rule > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(rule == 0 || rule < fData->fForwardTable->fLookAheadResultsSize);
        എങ്കിൽ (rule > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            fLookAheadMatches[rule] = pos;
        }

        എങ്കിൽ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            നിർത്തുക;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        എങ്കിൽ (mode == RBBI_RUN) {
            c = UTEXT_NEXT32(&fText);
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (mode == RBBI_START) {
                mode = RBBI_RUN;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    എങ്കിൽ (result == initialPosition) {
        utext_setNativeIndex(&fText, initialPosition);
        utext_next32(&fText);
        result = static_cast<int32_t>(utext_getNativeIndex(&fText));
        fRuleStatusIndex = 0;
    }

    // Leave the iterator at our result position.
    fPosition = result;
    #ifdef RBBI_DEBUG
        എങ്കിൽ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", result);
        }
    #endif
    തിരികെ result;
}


//-----------------------------------------------------------------------------------
//
//  handleSafePrevious()
//
//      Iterate backwards using the safe reverse rules.
//      The logic of this function is similar to handleNext(), but simpler
//      because the safe table does not require as many options.
//
//-----------------------------------------------------------------------------------
template <typename RowType, RuleBasedBreakIterator::PTrieFunc trieFunc>
int32_t RuleBasedBreakIterator::handleSafePrevious(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    RowType            *row;
    UChar32             c;
    int32_t             result          = 0;

    സ്ഥിരം RBBIStateTable *stateTable = fData->fReverseTable;
    UTEXT_SETNATIVEINDEX(&fText, fromPosition);
    #ifdef RBBI_DEBUG
        എങ്കിൽ (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #endif

    // if we're already at the start of the text, return DONE.
    എങ്കിൽ (fData == nullptr || UTEXT_GETNATIVEINDEX(&fText)==0) {
        തിരികെ BreakIterator::DONE;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&fText);
    state = START_STATE;
    row = (RowType *)
            (stateTable->fTableData + (stateTable->fRowLen * state));

    // loop until we reach the start of the text or transition to state 0
    //
    വേണ്ടി (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&fText)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(fData->fTrie, c);

        #ifdef RBBI_DEBUG
            എങ്കിൽ (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_getNativeIndex(&fText));
                എങ്കിൽ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } അല്ലെങ്കിൽ {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<fData->fHeader->f_Cat_എണ്ണം);
        state = row->fNextState[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            (stateTable->fTableData + (stateTable->fRowLen * state));

        എങ്കിൽ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            നിർത്തുക;
        }
    }

    // The state machine is done.  Check whether it found a match...
    result = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
    #ifdef RBBI_DEBUG
        എങ്കിൽ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", result);
        }
    #endif
    തിരികെ result;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  RuleBasedBreakIterator::getRuleStatus() സ്ഥിരം {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = fRuleStatusIndex + fData->fRuleStatusTable[fRuleStatusIndex];
    int32_t  tagVal = fData->fRuleStatusTable[idx];

    തിരികെ tagVal;
}


int32_t RuleBasedBreakIterator::getRuleStatusVec(
             int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ 0;
    }

    int32_t  അക്കം_Vals = fData->fRuleStatusTable[fRuleStatusIndex];
    int32_t  അക്കം_Vals_To_Copy = അക്കം_Vals;
    എങ്കിൽ (അക്കം_Vals > capacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        അക്കം_Vals_To_Copy = capacity;
    }
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i=0; i<അക്കം_Vals_To_Copy; i++) {
        fillInVec[i] = fData->fRuleStatusTable[fRuleStatusIndex + i + 1];
    }
    തിരികെ അക്കം_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
സ്ഥിരം uint8_t* RuleBasedBreakIterator::get_ദ്വിമാന_Rules(uint32_t& length) U_LIFETIME_BOUND {
    സ്ഥിരം uint8_t  *retPtr = nullptr;
    length = 0;

    എങ്കിൽ (fData != nullptr) {
        retPtr = reinterpret_cast<സ്ഥിരം uint8_t*>(fData->fHeader);
        length = fData->fHeader->fLength;
    }
    തിരികെ retPtr;
}


RuleBasedBreakIterator *RuleBasedBreakIterator::createBufferClone(
        ശൂന്യം * /*stackBuffer*/, int32_t &bufferSize, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)){
        തിരികെ nullptr;
    }

    എങ്കിൽ (bufferSize == 0) {
        bufferSize = 1;  // preflighting for deprecated functionality
        തിരികെ nullptr;
    }

    BreakIterator *clonedBI = clone();
    എങ്കിൽ (clonedBI == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } അല്ലെങ്കിൽ {
        status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    തിരികെ (RuleBasedBreakIterator *)clonedBI;
}

U_NAMESPACE_END


സ്ഥിതം icu::UStack *gLanguageBreakFactories = nullptr;
സ്ഥിതം സ്ഥിരം icu::UnicodeString *gEmptyString = nullptr;
സ്ഥിതം icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
സ്ഥിതം icu::UInitOnce gRBBIInitOnce {};
സ്ഥിതം icu::ICULanguageBreakFactory *gICULanguageBreakFactory = nullptr;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    delete gLanguageBreakFactories;
    gLanguageBreakFactories = nullptr;
    delete gEmptyString;
    gEmptyString = nullptr;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    തിരികെ സത്യം;
}
U_CDECL_END

U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം U_CALLCONV _deleteFactory(ശൂന്യം *obj) {
    delete (icu::LanguageBreakFactory *) obj;
}
U_CDECL_END
U_NAMESPACE_BEGIN

സ്ഥിതം ശൂന്യം U_CALLCONV rbbiInit() {
    gEmptyString = പുതിയ UnicodeString();
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

സ്ഥിതം ശൂന്യം U_CALLCONV initLanguageFactories(UErrorCode& status) {
    U_ASSERT(gLanguageBreakFactories == nullptr);
    gLanguageBreakFactories = പുതിയ UStack(_deleteFactory, nullptr, status);
    എങ്കിൽ (gLanguageBreakFactories != nullptr && U_SUCCESS(status)) {
        LocalPointer<ICULanguageBreakFactory> factory(പുതിയ ICULanguageBreakFactory(status), status);
        എങ്കിൽ (U_SUCCESS(status)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, status);
#ifdef U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &status);
            എങ്കിൽ (extra != nullptr) {
                gLanguageBreakFactories->push(extra, status);
            }
#endif
        }
    }
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ശൂന്യം ensureLanguageFactories(UErrorCode& status) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, status);
}

സ്ഥിതം സ്ഥിരം LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, സ്ഥിരം അക്ഷരം* locale)
{
    UErrorCode status = U_ZERO_ERROR;
    ensureLanguageFactories(status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ nullptr;

    int32_t i = gLanguageBreakFactories->size();
    സ്ഥിരം LanguageBreakEngine *lbe = nullptr;
    ആകുമ്പോൾ (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        എങ്കിൽ (lbe != nullptr) {
            നിർത്തുക;
        }
    }
    തിരികെ lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
സ്ഥിരം LanguageBreakEngine*
RuleBasedBreakIterator::getLanguageBreakEngine(UChar32 c, സ്ഥിരം അക്ഷരം* locale) U_LIFETIME_BOUND {
    സ്ഥിരം LanguageBreakEngine *lbe = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    എങ്കിൽ (fLanguageBreakEngines == nullptr) {
        fLanguageBreakEngines = പുതിയ UStack(status);
        എങ്കിൽ (fLanguageBreakEngines == nullptr || U_FAILURE(status)) {
            delete fLanguageBreakEngines;
            fLanguageBreakEngines = nullptr;
            തിരികെ nullptr;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    ആകുമ്പോൾ (--i >= 0) {
        lbe = static_cast<സ്ഥിരം LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        എങ്കിൽ (lbe->handles(c, locale)) {
            തിരികെ lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    എങ്കിൽ (lbe != nullptr) {
        fLanguageBreakEngines->push((ശൂന്യം *)lbe, status);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        തിരികെ lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    എങ്കിൽ (fUnhandledBreakEngine == nullptr) {
        fUnhandledBreakEngine = പുതിയ UnhandledEngine(status);
        എങ്കിൽ (U_SUCCESS(status) && fUnhandledBreakEngine == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->തിരുകുക_Element_At(fUnhandledBreakEngine, 0, status);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        എങ്കിൽ (U_FAILURE(status)) {
            delete fUnhandledBreakEngine;
            fUnhandledBreakEngine = nullptr;
            തിരികെ nullptr;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    തിരികെ fUnhandledBreakEngine;
}

#ifndef U_HIDE_DRAFT_API
ശൂന്യം U_EXPORT2 RuleBasedBreakIterator::registerExternalBreakEngine(
                  ExternalBreakEngine* toAdopt, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(toAdopt, status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    ensureLanguageFactories(status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    gICULanguageBreakFactory->addExternalEngine(engine.orphan(), status);
}
#endif  /* U_HIDE_DRAFT_API */


ശൂന്യം RuleBasedBreakIterator::dumpCache() {
    fBreakCache->dumpCache();
}

ശൂന്യം RuleBasedBreakIterator::dumpTables() {
    fData->printData();
}

/**
 * Returns the description used to create this iterator
 */

സ്ഥിരം UnicodeString&
RuleBasedBreakIterator::getRules() സ്ഥിരം U_LIFETIME_BOUND {
    എങ്കിൽ (fData != nullptr) {
        തിരികെ fData->getRuleSourceString();
    } അല്ലെങ്കിൽ {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        തിരികെ *gEmptyString;
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

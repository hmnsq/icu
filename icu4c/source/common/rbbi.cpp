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

#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव <cinttypes>

#अन्तर्भाव "unicode/rbbi.h"
#अन्तर्भाव "unicode/schriter.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/uclean.h"
#अन्तर्भाव "unicode/udata.h"

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "localsvc.h"
#अन्तर्भाव "rbbidata.h"
#अन्तर्भाव "rbbi_cache.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uvectr32.h"

#ifdef RBBI_DEBUG
स्थिर UBool gTrace = मिच्छ;
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
    fData = नूतन RBBIDataWrapper(data, status); // status checked in constructor
    जइ (U_FAILURE(status)) {प्रत्यागम;}
    जइ(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    जइ (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * परिमाण(int32_t)));
        जइ (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
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
RuleBasedBreakIterator::RuleBasedBreakIterator(ध्रुव uint8_t *compiledRules,
                       uint32_t       ruleLength,
                       UErrorCode     &status)
 : RuleBasedBreakIterator(&status)
{
    जइ (U_FAILURE(status)) {
        प्रत्यागम;
    }
    जइ (compiledRules == nullptr || ruleLength < परिमाण(RBBIDataHeader)) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    ध्रुव RBBIDataHeader* data = reinterpret_cast<ध्रुव RBBIDataHeader*>(compiledRules);
    जइ (data->fLength > ruleLength) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    fData = नूतन RBBIDataWrapper(data, RBBIDataWrapper::kDontAdopt, status);
    जइ (U_FAILURE(status)) {प्रत्यागम;}
    जइ(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    जइ (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * परिमाण(int32_t)));
        जइ (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
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
    fData = नूतन RBBIDataWrapper(udm, status); // status checked in constructor
    जइ (U_FAILURE(status)) {प्रत्यागम;}
    जइ(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    जइ (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * परिमाण(int32_t)));
        जइ (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator( ध्रुव UnicodeString  &rules,
                                                UParseError          &parseError,
                                                UErrorCode           &status)
 : RuleBasedBreakIterator(&status)
{
    जइ (U_FAILURE(status)) {प्रत्यागम;}
    RuleBasedBreakIterator *bi = (RuleBasedBreakIterator *)
        RBBIRuleBuilder::createRuleBasedBreakIterator(rules, &parseError, status);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    जइ (U_SUCCESS(status)) {
        *एतत् = *bi;
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
    जइ (status == nullptr) {
        status = &ec;
    }
    utext_openUChars(&fText, nullptr, 0, status);
    LocalPointer<DictionaryCache> lpDictionaryCache(नूतन DictionaryCache(एतत्, *status), *status);
    LocalPointer<BreakCache> lpBreakCache(नूतन BreakCache(एतत्, *status), *status);
    जइ (U_FAILURE(*status)) {
        fErrorCode = *status;
        प्रत्यागम;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#ifdef RBBI_DEBUG
    स्थिर UBool debugInitDone = मिच्छ;
    जइ (debugInitDone == मिच्छ) {
        वर्ण *debugEnv = getenv("U_RBBIDEBUG");
        जइ (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = सच्च;
        }
        debugInitDone = सच्च;
    }
#endif
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator(ध्रुव RuleBasedBreakIterator& other)
: RuleBasedBreakIterator()
{
    *एतत् = other;
}


/**
 * Destructor
 */
RuleBasedBreakIterator::~RuleBasedBreakIterator() {
    जइ (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        delete fCharIter;
    }
    fCharIter = nullptr;

    utext_close(&fText);

    जइ (fData != nullptr) {
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
RuleBasedBreakIterator::operator=(ध्रुव RuleBasedBreakIterator& that) U_LIFETIME_BOUND {
    जइ (एतत् == &that) {
        प्रत्यागम *एतत्;
    }
    BreakIterator::operator=(that);

    जइ (fLanguageBreakEngines != nullptr) {
        delete fLanguageBreakEngines;
        fLanguageBreakEngines = nullptr;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UErrorCode status = U_ZERO_ERROR;
    utext_clone(&fText, &that.fText, मिच्छ, सच्च, &status);

    जइ (fCharIter != &fSCharIter) {
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    जइ (that.fCharIter != nullptr && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    जइ (fCharIter == nullptr) {
        fCharIter = &fSCharIter;
    }

    जइ (fData != nullptr) {
        fData->removeReference();
        fData = nullptr;
    }
    जइ (that.fData != nullptr) {
        fData = that.fData->addReference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = nullptr;
    जइ (fData && fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * परिमाण(int32_t)));
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

    प्रत्यागम *एतत्;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
RuleBasedBreakIterator*
RuleBasedBreakIterator::clone() ध्रुव {
    प्रत्यागम नूतन RuleBasedBreakIterator(*एतत्);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
तार्किक
RuleBasedBreakIterator::operator==(ध्रुव BreakIterator& that) ध्रुव {
    जइ (typeid(*एतत्) != typeid(that)) {
        प्रत्यागम मिच्छ;
    }
    जइ (एतत् == &that) {
        प्रत्यागम सच्च;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ध्रुव RuleBasedBreakIterator& that2 = static_cast<ध्रुव RuleBasedBreakIterator&>(that);

    जइ (!utext_equals(&fText, &that2.fText)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        प्रत्यागम मिच्छ;
    }

    जइ (!(fPosition == that2.fPosition &&
            fRuleStatusIndex == that2.fRuleStatusIndex &&
            fDone == that2.fDone &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        प्रत्यागम मिच्छ;
    }

    जइ (that2.fData == fData ||
        (fData != nullptr && that2.fData != nullptr && *that2.fData == *fData)) {
            // The two break iterators are using the same rules.
            प्रत्यागम सच्च;
        }
    प्रत्यागम मिच्छ;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
RuleBasedBreakIterator::hashCode() ध्रुव {
    int32_t   hash = 0;
    जइ (fData != nullptr) {
        hash = fData->hashCode();
    }
    प्रत्यागम hash;
}


शून्य RuleBasedBreakIterator::setText(UText *ut, UErrorCode &status) {
    जइ (U_FAILURE(status)) {
        प्रत्यागम;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&fText, ut, मिच्छ, सच्च, &status);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.setText(u"", 0);

    जइ (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    एतत्->first();
}


UText* RuleBasedBreakIterator::getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ध्रुव {
    UText *result = utext_clone(fillIn, &fText, मिच्छ, सच्च, &status);
    प्रत्यागम result;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
RuleBasedBreakIterator::getText() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
शून्य
RuleBasedBreakIterator::adoptText(CharacterIterator* newText) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    जइ (fCharIter != &fSCharIter) {
        delete fCharIter;
    }

    fCharIter = newText;
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    जइ (newText==nullptr || newText->startIndex() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&fText, nullptr, 0, &status);
    } अण्णहा {
        utext_openCharacterIterator(&fText, newText, &status);
    }
    एतत्->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
शून्य
RuleBasedBreakIterator::setText(ध्रुव UnicodeString& newText) {
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&fText, &newText, &status);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.setText(newText.getBuffer(), newText.length());

    जइ (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    एतत्->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
RuleBasedBreakIterator&
RuleBasedBreakIterator::refresh_निवेशः_Text(UText* input, UErrorCode& status) U_LIFETIME_BOUND {
    जइ (U_FAILURE(status)) {
        प्रत्यागम *एतत्;
    }
    जइ (input == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    int64_t pos = utext_getNativeIndex(&fText);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&fText, input, मिच्छ, सच्च, &status);
    जइ (U_FAILURE(status)) {
        प्रत्यागम *एतत्;
    }
    utext_setNativeIndex(&fText, pos);
    जइ (utext_getNativeIndex(&fText) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    प्रत्यागम *एतत्;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t RuleBasedBreakIterator::first() {
    UErrorCode status = U_ZERO_ERROR;
    जइ (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, status);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    प्रत्यागम 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t RuleBasedBreakIterator::last() {
    int32_t endPos = static_cast<int32_t>(utext_nativeLength(&fText));
    UBool endShouldBeBoundary = isBoundary(endPos);      // Has side effect of setting iterator position.
    (शून्य)endShouldBeBoundary;
    U_ASSERT(endShouldBeBoundary);
    U_ASSERT(fPosition == endPos);
    प्रत्यागम endPos;
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
    जइ (n > 0) {
        प्रति (; n > 0 && result != UBRK_DONE; --n) {
            result = next();
        }
    } अण्णहा जइ (n < 0) {
        प्रति (; n < 0 && result != UBRK_DONE; ++n) {
            result = previous();
        }
    } अण्णहा {
        result = current();
    }
    प्रत्यागम result;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t RuleBasedBreakIterator::next() {
    fBreakCache->next();
    प्रत्यागम fDone ? UBRK_DONE : fPosition;
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
    प्रत्यागम fDone ? UBRK_DONE : fPosition;
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
    जइ (startPos < 0) {
        प्रत्यागम first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_setNativeIndex(&fText, startPos);
    startPos = static_cast<int32_t>(utext_getNativeIndex(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->following(startPos, status);
    प्रत्यागम fDone ? UBRK_DONE : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t RuleBasedBreakIterator::preceding(int32_t offset) {
    जइ (offset > utext_nativeLength(&fText)) {
        प्रत्यागम last();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_setNativeIndex(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_getNativeIndex(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->preceding(adjustedOffset, status);
    प्रत्यागम fDone ? UBRK_DONE : fPosition;
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
    जइ (offset < 0) {
        first();       // For side effects on current position, tag values.
        प्रत्यागम मिच्छ;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_setNativeIndex(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_getNativeIndex(&fText));

    तार्किक result = मिच्छ;
    UErrorCode status = U_ZERO_ERROR;
    जइ (fBreakCache->seek(adjustedOffset) || fBreakCache->populateNear(adjustedOffset, status)) {
        result = (fBreakCache->current() == offset);
    }

    जइ (result && adjustedOffset < offset && utext_char32At(&fText, offset) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        प्रत्यागम मिच्छ;
    }
    जइ (!result) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    प्रत्यागम result;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t RuleBasedBreakIterator::current() ध्रुव {
    प्रत्यागम fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
गणना RBBIRunMode {
    RBBI_START,     // state machine processing is before first char of input
    RBBI_RUN,       // state machine processing is in the user text
    RBBI_END        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
स्थिर अन्तःस्थ uint16_t TrieFunc8(ध्रुव UCPTrie *trie, UChar32 c) {
    प्रत्यागम UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

स्थिर अन्तःस्थ uint16_t TrieFunc16(ध्रुव UCPTrie *trie, UChar32 c) {
    प्रत्यागम UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t RuleBasedBreakIterator::handleNext() {
    ध्रुव RBBIStateTable *statetable = fData->fForwardTable;
    तार्किक use8BitsTrie = ucptrie_get_मानम्_Width(fData->fTrie) == UCPTRIE_मानम्_BITS_8;
    जइ (statetable->fFlags & RBBI_8BITS_ROWS) {
        जइ (use8BitsTrie) {
            प्रत्यागम handleNext<RBBIStateTableRow8, TrieFunc8>();
        } अण्णहा {
            प्रत्यागम handleNext<RBBIStateTableRow8, TrieFunc16>();
        }
    } अण्णहा {
        जइ (use8BitsTrie) {
            प्रत्यागम handleNext<RBBIStateTableRow16, TrieFunc8>();
        } अण्णहा {
            प्रत्यागम handleNext<RBBIStateTableRow16, TrieFunc16>();
        }
    }
}

int32_t RuleBasedBreakIterator::handleSafePrevious(int32_t fromPosition) {
    ध्रुव RBBIStateTable *statetable = fData->fReverseTable;
    तार्किक use8BitsTrie = ucptrie_get_मानम्_Width(fData->fTrie) == UCPTRIE_मानम्_BITS_8;
    जइ (statetable->fFlags & RBBI_8BITS_ROWS) {
        जइ (use8BitsTrie) {
            प्रत्यागम handleSafePrevious<RBBIStateTableRow8, TrieFunc8>(fromPosition);
        } अण्णहा {
            प्रत्यागम handleSafePrevious<RBBIStateTableRow8, TrieFunc16>(fromPosition);
        }
    } अण्णहा {
        जइ (use8BitsTrie) {
            प्रत्यागम handleSafePrevious<RBBIStateTableRow16, TrieFunc8>(fromPosition);
        } अण्णहा {
            प्रत्यागम handleSafePrevious<RBBIStateTableRow16, TrieFunc16>(fromPosition);
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
    ध्रुव RBBIStateTable *statetable       = fData->fForwardTable;
    ध्रुव वर्ण         *tableData          = statetable->fTableData;
    uint32_t            tableRowLen        = statetable->fRowLen;
    uint32_t            dictStart          = statetable->fDictCategoriesStart;
    #ifdef RBBI_DEBUG
        जइ (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #endif

    // handleNext always sets the break tag value.
    // Set the default for it.
    fRuleStatusIndex = 0;

    f_Dictionary_Char_गणनम् = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&fText, initialPosition);
    result          = initialPosition;
    c               = UTEXT_NEXT32(&fText);
    जइ (c==U_SENTINEL) {
        fDone = सच्च;
        प्रत्यागम UBRK_DONE;
    }

    //  Set the initial state for the state machine
    state = START_STATE;
    row = (RowType *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (tableData + tableRowLen * state);


    mode     = RBBI_RUN;


    // loop until we reach the end of the text or transition to state 0
    //
    प्रति (;;) {
        जइ (c == U_SENTINEL) {
            // Reached end of input string.
            जइ (mode == RBBI_END) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                भङ्ग;
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
        जइ (mode == RBBI_RUN) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(fData->fTrie, c);
            f_Dictionary_Char_गणनम् += (category >= dictStart);
        }

       #ifdef RBBI_DEBUG
            जइ (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_getNativeIndex(&fText));
                जइ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } अण्णहा {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<fData->fHeader->f_Cat_गणनम्);
        state = row->fNextState[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (tableData + tableRowLen * state);


        uint16_t accepting = row->fAccepting;
        जइ (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            जइ (mode != RBBI_START) {
                result = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            }
            fRuleStatusIndex = row->fTagsIdx;   // Remember the break status (tag) values.
        } अण्णहा जइ (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < fData->fForwardTable->fLookAheadResultsSize);
            int32_t lookaheadResult = fLookAheadMatches[accepting];
            जइ (lookaheadResult >= 0) {
                fRuleStatusIndex = row->fTagsIdx;
                fPosition = lookaheadResult;
                प्रत्यागम lookaheadResult;
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
        जइ (rule > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            fLookAheadMatches[rule] = pos;
        }

        जइ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            भङ्ग;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        जइ (mode == RBBI_RUN) {
            c = UTEXT_NEXT32(&fText);
        } अण्णहा {
            जइ (mode == RBBI_START) {
                mode = RBBI_RUN;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    जइ (result == initialPosition) {
        utext_setNativeIndex(&fText, initialPosition);
        utext_next32(&fText);
        result = static_cast<int32_t>(utext_getNativeIndex(&fText));
        fRuleStatusIndex = 0;
    }

    // Leave the iterator at our result position.
    fPosition = result;
    #ifdef RBBI_DEBUG
        जइ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", result);
        }
    #endif
    प्रत्यागम result;
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

    ध्रुव RBBIStateTable *stateTable = fData->fReverseTable;
    UTEXT_SETNATIVEINDEX(&fText, fromPosition);
    #ifdef RBBI_DEBUG
        जइ (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #endif

    // if we're already at the start of the text, return DONE.
    जइ (fData == nullptr || UTEXT_GETNATIVEINDEX(&fText)==0) {
        प्रत्यागम BreakIterator::DONE;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&fText);
    state = START_STATE;
    row = (RowType *)
            (stateTable->fTableData + (stateTable->fRowLen * state));

    // loop until we reach the start of the text or transition to state 0
    //
    प्रति (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&fText)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(fData->fTrie, c);

        #ifdef RBBI_DEBUG
            जइ (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_getNativeIndex(&fText));
                जइ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } अण्णहा {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<fData->fHeader->f_Cat_गणनम्);
        state = row->fNextState[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            (stateTable->fTableData + (stateTable->fRowLen * state));

        जइ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            भङ्ग;
        }
    }

    // The state machine is done.  Check whether it found a match...
    result = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
    #ifdef RBBI_DEBUG
        जइ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", result);
        }
    #endif
    प्रत्यागम result;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  RuleBasedBreakIterator::getRuleStatus() ध्रुव {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = fRuleStatusIndex + fData->fRuleStatusTable[fRuleStatusIndex];
    int32_t  tagVal = fData->fRuleStatusTable[idx];

    प्रत्यागम tagVal;
}


int32_t RuleBasedBreakIterator::getRuleStatusVec(
             int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    जइ (U_FAILURE(status)) {
        प्रत्यागम 0;
    }

    int32_t  अङ्कः_Vals = fData->fRuleStatusTable[fRuleStatusIndex];
    int32_t  अङ्कः_Vals_To_Copy = अङ्कः_Vals;
    जइ (अङ्कः_Vals > capacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        अङ्कः_Vals_To_Copy = capacity;
    }
    पूर्णाङ्क i;
    प्रति (i=0; i<अङ्कः_Vals_To_Copy; i++) {
        fillInVec[i] = fData->fRuleStatusTable[fRuleStatusIndex + i + 1];
    }
    प्रत्यागम अङ्कः_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ध्रुव uint8_t* RuleBasedBreakIterator::get_द्विमान_Rules(uint32_t& length) U_LIFETIME_BOUND {
    ध्रुव uint8_t  *retPtr = nullptr;
    length = 0;

    जइ (fData != nullptr) {
        retPtr = reinterpret_cast<ध्रुव uint8_t*>(fData->fHeader);
        length = fData->fHeader->fLength;
    }
    प्रत्यागम retPtr;
}


RuleBasedBreakIterator *RuleBasedBreakIterator::createBufferClone(
        शून्य * /*stackBuffer*/, int32_t &bufferSize, UErrorCode &status) {
    जइ (U_FAILURE(status)){
        प्रत्यागम nullptr;
    }

    जइ (bufferSize == 0) {
        bufferSize = 1;  // preflighting for deprecated functionality
        प्रत्यागम nullptr;
    }

    BreakIterator *clonedBI = clone();
    जइ (clonedBI == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अण्णहा {
        status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    प्रत्यागम (RuleBasedBreakIterator *)clonedBI;
}

U_NAMESPACE_END


स्थिर icu::UStack *gLanguageBreakFactories = nullptr;
स्थिर ध्रुव icu::UnicodeString *gEmptyString = nullptr;
स्थिर icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
स्थिर icu::UInitOnce gRBBIInitOnce {};
स्थिर icu::ICULanguageBreakFactory *gICULanguageBreakFactory = nullptr;

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
    प्रत्यागम सच्च;
}
U_CDECL_END

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV _deleteFactory(शून्य *obj) {
    delete (icu::LanguageBreakFactory *) obj;
}
U_CDECL_END
U_NAMESPACE_BEGIN

स्थिर शून्य U_CALLCONV rbbiInit() {
    gEmptyString = नूतन UnicodeString();
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

स्थिर शून्य U_CALLCONV initLanguageFactories(UErrorCode& status) {
    U_ASSERT(gLanguageBreakFactories == nullptr);
    gLanguageBreakFactories = नूतन UStack(_deleteFactory, nullptr, status);
    जइ (gLanguageBreakFactories != nullptr && U_SUCCESS(status)) {
        LocalPointer<ICULanguageBreakFactory> factory(नूतन ICULanguageBreakFactory(status), status);
        जइ (U_SUCCESS(status)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, status);
#ifdef U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &status);
            जइ (extra != nullptr) {
                gLanguageBreakFactories->push(extra, status);
            }
#endif
        }
    }
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

शून्य ensureLanguageFactories(UErrorCode& status) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, status);
}

स्थिर ध्रुव LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ध्रुव वर्ण* locale)
{
    UErrorCode status = U_ZERO_ERROR;
    ensureLanguageFactories(status);
    जइ (U_FAILURE(status)) प्रत्यागम nullptr;

    int32_t i = gLanguageBreakFactories->size();
    ध्रुव LanguageBreakEngine *lbe = nullptr;
    जाव (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        जइ (lbe != nullptr) {
            भङ्ग;
        }
    }
    प्रत्यागम lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
ध्रुव LanguageBreakEngine*
RuleBasedBreakIterator::getLanguageBreakEngine(UChar32 c, ध्रुव वर्ण* locale) U_LIFETIME_BOUND {
    ध्रुव LanguageBreakEngine *lbe = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    जइ (fLanguageBreakEngines == nullptr) {
        fLanguageBreakEngines = नूतन UStack(status);
        जइ (fLanguageBreakEngines == nullptr || U_FAILURE(status)) {
            delete fLanguageBreakEngines;
            fLanguageBreakEngines = nullptr;
            प्रत्यागम nullptr;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    जाव (--i >= 0) {
        lbe = static_cast<ध्रुव LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        जइ (lbe->handles(c, locale)) {
            प्रत्यागम lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    जइ (lbe != nullptr) {
        fLanguageBreakEngines->push((शून्य *)lbe, status);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        प्रत्यागम lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    जइ (fUnhandledBreakEngine == nullptr) {
        fUnhandledBreakEngine = नूतन UnhandledEngine(status);
        जइ (U_SUCCESS(status) && fUnhandledBreakEngine == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम nullptr;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->निवेशय_Element_At(fUnhandledBreakEngine, 0, status);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        जइ (U_FAILURE(status)) {
            delete fUnhandledBreakEngine;
            fUnhandledBreakEngine = nullptr;
            प्रत्यागम nullptr;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    प्रत्यागम fUnhandledBreakEngine;
}

#ifndef U_HIDE_DRAFT_API
शून्य U_EXPORT2 RuleBasedBreakIterator::registerExternalBreakEngine(
                  ExternalBreakEngine* toAdopt, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(toAdopt, status);
    जइ (U_FAILURE(status)) प्रत्यागम;
    ensureLanguageFactories(status);
    जइ (U_FAILURE(status)) प्रत्यागम;
    gICULanguageBreakFactory->addExternalEngine(engine.orphan(), status);
}
#endif  /* U_HIDE_DRAFT_API */


शून्य RuleBasedBreakIterator::dumpCache() {
    fBreakCache->dumpCache();
}

शून्य RuleBasedBreakIterator::dumpTables() {
    fData->printData();
}

/**
 * Returns the description used to create this iterator
 */

ध्रुव UnicodeString&
RuleBasedBreakIterator::getRules() ध्रुव U_LIFETIME_BOUND {
    जइ (fData != nullptr) {
        प्रत्यागम fData->getRuleSourceString();
    } अण्णहा {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        प्रत्यागम *gEmptyString;
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

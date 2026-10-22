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

#ضمن "utypeinfo.h"  // for 'typeid' to work

#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_BREAK_ITERATION

#ضمن <cinttypes>

#ضمن "unicode/rbbi.h"
#ضمن "unicode/schriter.h"
#ضمن "unicode/uchriter.h"
#ضمن "unicode/uclean.h"
#ضمن "unicode/udata.h"

#ضمن "brkeng.h"
#ضمن "ucln_cmn.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "localsvc.h"
#ضمن "rbbidata.h"
#ضمن "rbbi_cache.h"
#ضمن "rbbirb.h"
#ضمن "uassert.h"
#ضمن "umutex.h"
#ضمن "uvectr32.h"

#ifdef RBBI_DEBUG
ساكن UBool gTrace = خطأ;
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
    fData = جديد RBBIDataWrapper(data, status); // status checked in constructor
    𐤀𐤌 (U_FAILURE(status)) {أرجع;}
    𐤀𐤌(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    𐤀𐤌 (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * حجم(int32_t)));
        𐤀𐤌 (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
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
RuleBasedBreakIterator::RuleBasedBreakIterator(ثابت uint8_t *compiledRules,
                       uint32_t       ruleLength,
                       UErrorCode     &status)
 : RuleBasedBreakIterator(&status)
{
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع;
    }
    𐤀𐤌 (compiledRules == nullptr || ruleLength < حجم(RBBIDataHeader)) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    ثابت RBBIDataHeader* data = reinterpret_cast<ثابت RBBIDataHeader*>(compiledRules);
    𐤀𐤌 (data->fLength > ruleLength) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    fData = جديد RBBIDataWrapper(data, RBBIDataWrapper::kDontAdopt, status);
    𐤀𐤌 (U_FAILURE(status)) {أرجع;}
    𐤀𐤌(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    𐤀𐤌 (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * حجم(int32_t)));
        𐤀𐤌 (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
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
    fData = جديد RBBIDataWrapper(udm, status); // status checked in constructor
    𐤀𐤌 (U_FAILURE(status)) {أرجع;}
    𐤀𐤌(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    𐤀𐤌 (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * حجم(int32_t)));
        𐤀𐤌 (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator( ثابت UnicodeString  &rules,
                                                UParseError          &parseError,
                                                UErrorCode           &status)
 : RuleBasedBreakIterator(&status)
{
    𐤀𐤌 (U_FAILURE(status)) {أرجع;}
    RuleBasedBreakIterator *bi = (RuleBasedBreakIterator *)
        RBBIRuleBuilder::createRuleBasedBreakIterator(rules, &parseError, status);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    𐤀𐤌 (U_SUCCESS(status)) {
        *هذا = *bi;
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
    𐤀𐤌 (status == nullptr) {
        status = &ec;
    }
    utext_openUChars(&fText, nullptr, 0, status);
    LocalPointer<DictionaryCache> lpDictionaryCache(جديد DictionaryCache(هذا, *status), *status);
    LocalPointer<BreakCache> lpBreakCache(جديد BreakCache(هذا, *status), *status);
    𐤀𐤌 (U_FAILURE(*status)) {
        fErrorCode = *status;
        أرجع;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#ifdef RBBI_DEBUG
    ساكن UBool debugInitDone = خطأ;
    𐤀𐤌 (debugInitDone == خطأ) {
        حرف *debugEnv = getenv("U_RBBIDEBUG");
        𐤀𐤌 (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = true;
        }
        debugInitDone = true;
    }
#endif
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator(ثابت RuleBasedBreakIterator& other)
: RuleBasedBreakIterator()
{
    *هذا = other;
}


/**
 * Destructor
 */
RuleBasedBreakIterator::~RuleBasedBreakIterator() {
    𐤀𐤌 (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        delete fCharIter;
    }
    fCharIter = nullptr;

    utext_close(&fText);

    𐤀𐤌 (fData != nullptr) {
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
RuleBasedBreakIterator::operator=(ثابت RuleBasedBreakIterator& that) U_LIFETIME_BOUND {
    𐤀𐤌 (هذا == &that) {
        أرجع *هذا;
    }
    BreakIterator::operator=(that);

    𐤀𐤌 (fLanguageBreakEngines != nullptr) {
        delete fLanguageBreakEngines;
        fLanguageBreakEngines = nullptr;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UErrorCode status = U_ZERO_ERROR;
    utext_clone(&fText, &that.fText, خطأ, true, &status);

    𐤀𐤌 (fCharIter != &fSCharIter) {
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    𐤀𐤌 (that.fCharIter != nullptr && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    𐤀𐤌 (fCharIter == nullptr) {
        fCharIter = &fSCharIter;
    }

    𐤀𐤌 (fData != nullptr) {
        fData->removeReference();
        fData = nullptr;
    }
    𐤀𐤌 (that.fData != nullptr) {
        fData = that.fData->addReference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = nullptr;
    𐤀𐤌 (fData && fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * حجم(int32_t)));
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

    أرجع *هذا;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
RuleBasedBreakIterator*
RuleBasedBreakIterator::clone() ثابت {
    أرجع جديد RuleBasedBreakIterator(*هذا);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
منطقي
RuleBasedBreakIterator::operator==(ثابت BreakIterator& that) ثابت {
    𐤀𐤌 (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }
    𐤀𐤌 (هذا == &that) {
        أرجع true;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ثابت RuleBasedBreakIterator& that2 = static_cast<ثابت RuleBasedBreakIterator&>(that);

    𐤀𐤌 (!utext_equals(&fText, &that2.fText)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        أرجع خطأ;
    }

    𐤀𐤌 (!(fPosition == that2.fPosition &&
            fRuleStatusIndex == that2.fRuleStatusIndex &&
            fDone == that2.fDone &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        أرجع خطأ;
    }

    𐤀𐤌 (that2.fData == fData ||
        (fData != nullptr && that2.fData != nullptr && *that2.fData == *fData)) {
            // The two break iterators are using the same rules.
            أرجع true;
        }
    أرجع خطأ;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
RuleBasedBreakIterator::hashCode() ثابت {
    int32_t   hash = 0;
    𐤀𐤌 (fData != nullptr) {
        hash = fData->hashCode();
    }
    أرجع hash;
}


فراغ RuleBasedBreakIterator::setText(UText *ut, UErrorCode &status) {
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&fText, ut, خطأ, true, &status);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.setText(u"", 0);

    𐤀𐤌 (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    هذا->first();
}


UText* RuleBasedBreakIterator::getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ثابت {
    UText *result = utext_clone(fillIn, &fText, خطأ, true, &status);
    أرجع result;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
RuleBasedBreakIterator::getText() ثابت U_LIFETIME_BOUND {
    أرجع *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
فراغ
RuleBasedBreakIterator::adoptText(CharacterIterator* newText) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    𐤀𐤌 (fCharIter != &fSCharIter) {
        delete fCharIter;
    }

    fCharIter = newText;
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    𐤀𐤌 (newText==nullptr || newText->startIndex() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&fText, nullptr, 0, &status);
    } وإلا {
        utext_openCharacterIterator(&fText, newText, &status);
    }
    هذا->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
فراغ
RuleBasedBreakIterator::setText(ثابت UnicodeString& newText) {
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&fText, &newText, &status);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.setText(newText.getBuffer(), newText.length());

    𐤀𐤌 (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    هذا->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
RuleBasedBreakIterator&
RuleBasedBreakIterator::refreshInputText(UText* input, UErrorCode& status) U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع *هذا;
    }
    𐤀𐤌 (input == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    int64_t pos = utext_getNativeIndex(&fText);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&fText, input, خطأ, true, &status);
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع *هذا;
    }
    utext_setNativeIndex(&fText, pos);
    𐤀𐤌 (utext_getNativeIndex(&fText) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    أرجع *هذا;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t RuleBasedBreakIterator::first() {
    UErrorCode status = U_ZERO_ERROR;
    𐤀𐤌 (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, status);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    أرجع 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t RuleBasedBreakIterator::last() {
    int32_t endPos = static_cast<int32_t>(utext_nativeLength(&fText));
    UBool endShouldBeBoundary = isBoundary(endPos);      // Has side effect of setting iterator position.
    (فراغ)endShouldBeBoundary;
    U_ASSERT(endShouldBeBoundary);
    U_ASSERT(fPosition == endPos);
    أرجع endPos;
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
    𐤀𐤌 (n > 0) {
        لكل (; n > 0 && result != UBRK_DONE; --n) {
            result = next();
        }
    } وإلا 𐤀𐤌 (n < 0) {
        لكل (; n < 0 && result != UBRK_DONE; ++n) {
            result = previous();
        }
    } وإلا {
        result = current();
    }
    أرجع result;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t RuleBasedBreakIterator::next() {
    fBreakCache->next();
    أرجع fDone ? UBRK_DONE : fPosition;
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
    أرجع fDone ? UBRK_DONE : fPosition;
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
    𐤀𐤌 (startPos < 0) {
        أرجع first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_setNativeIndex(&fText, startPos);
    startPos = static_cast<int32_t>(utext_getNativeIndex(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->following(startPos, status);
    أرجع fDone ? UBRK_DONE : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t RuleBasedBreakIterator::preceding(int32_t offset) {
    𐤀𐤌 (offset > utext_nativeLength(&fText)) {
        أرجع last();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_setNativeIndex(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_getNativeIndex(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->preceding(adjustedOffset, status);
    أرجع fDone ? UBRK_DONE : fPosition;
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
    𐤀𐤌 (offset < 0) {
        first();       // For side effects on current position, tag values.
        أرجع خطأ;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_setNativeIndex(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_getNativeIndex(&fText));

    منطقي result = خطأ;
    UErrorCode status = U_ZERO_ERROR;
    𐤀𐤌 (fBreakCache->seek(adjustedOffset) || fBreakCache->populateNear(adjustedOffset, status)) {
        result = (fBreakCache->current() == offset);
    }

    𐤀𐤌 (result && adjustedOffset < offset && utext_char32At(&fText, offset) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        أرجع خطأ;
    }
    𐤀𐤌 (!result) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    أرجع result;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t RuleBasedBreakIterator::current() ثابت {
    أرجع fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
تعداد RBBIRunMode {
    RBBI_START,     // state machine processing is before first char of input
    RBBI_RUN,       // state machine processing is in the user text
    RBBI_END        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
ساكن مضمن uint16_t TrieFunc8(ثابت UCPTrie *trie, UChar32 c) {
    أرجع UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

ساكن مضمن uint16_t TrieFunc16(ثابت UCPTrie *trie, UChar32 c) {
    أرجع UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t RuleBasedBreakIterator::handleNext() {
    ثابت RBBIStateTable *statetable = fData->fForwardTable;
    منطقي use8BitsTrie = ucptrie_getValueWidth(fData->fTrie) == UCPTRIE_VALUE_BITS_8;
    𐤀𐤌 (statetable->fFlags & RBBI_8BITS_ROWS) {
        𐤀𐤌 (use8BitsTrie) {
            أرجع handleNext<RBBIStateTableRow8, TrieFunc8>();
        } وإلا {
            أرجع handleNext<RBBIStateTableRow8, TrieFunc16>();
        }
    } وإلا {
        𐤀𐤌 (use8BitsTrie) {
            أرجع handleNext<RBBIStateTableRow16, TrieFunc8>();
        } وإلا {
            أرجع handleNext<RBBIStateTableRow16, TrieFunc16>();
        }
    }
}

int32_t RuleBasedBreakIterator::handleSafePrevious(int32_t fromPosition) {
    ثابت RBBIStateTable *statetable = fData->fReverseTable;
    منطقي use8BitsTrie = ucptrie_getValueWidth(fData->fTrie) == UCPTRIE_VALUE_BITS_8;
    𐤀𐤌 (statetable->fFlags & RBBI_8BITS_ROWS) {
        𐤀𐤌 (use8BitsTrie) {
            أرجع handleSafePrevious<RBBIStateTableRow8, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handleSafePrevious<RBBIStateTableRow8, TrieFunc16>(fromPosition);
        }
    } وإلا {
        𐤀𐤌 (use8BitsTrie) {
            أرجع handleSafePrevious<RBBIStateTableRow16, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handleSafePrevious<RBBIStateTableRow16, TrieFunc16>(fromPosition);
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
    ثابت RBBIStateTable *statetable       = fData->fForwardTable;
    ثابت حرف         *tableData          = statetable->fTableData;
    uint32_t            tableRowLen        = statetable->fRowLen;
    uint32_t            dictStart          = statetable->fDictCategoriesStart;
    #ifdef RBBI_DEBUG
        𐤀𐤌 (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #endif

    // handleNext always sets the break tag value.
    // Set the default for it.
    fRuleStatusIndex = 0;

    fDictionaryCharCount = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&fText, initialPosition);
    result          = initialPosition;
    c               = UTEXT_NEXT32(&fText);
    𐤀𐤌 (c==U_SENTINEL) {
        fDone = true;
        أرجع UBRK_DONE;
    }

    //  Set the initial state for the state machine
    state = START_STATE;
    row = (RowType *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (tableData + tableRowLen * state);


    mode     = RBBI_RUN;


    // loop until we reach the end of the text or transition to state 0
    //
    لكل (;;) {
        𐤀𐤌 (c == U_SENTINEL) {
            // Reached end of input string.
            𐤀𐤌 (mode == RBBI_END) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                اكسر;
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
        𐤀𐤌 (mode == RBBI_RUN) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(fData->fTrie, c);
            fDictionaryCharCount += (category >= dictStart);
        }

       #ifdef RBBI_DEBUG
            𐤀𐤌 (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_getNativeIndex(&fText));
                𐤀𐤌 (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<fData->fHeader->fCatCount);
        state = row->fNextState[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (tableData + tableRowLen * state);


        uint16_t accepting = row->fAccepting;
        𐤀𐤌 (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            𐤀𐤌 (mode != RBBI_START) {
                result = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            }
            fRuleStatusIndex = row->fTagsIdx;   // Remember the break status (tag) values.
        } وإلا 𐤀𐤌 (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < fData->fForwardTable->fLookAheadResultsSize);
            int32_t lookaheadResult = fLookAheadMatches[accepting];
            𐤀𐤌 (lookaheadResult >= 0) {
                fRuleStatusIndex = row->fTagsIdx;
                fPosition = lookaheadResult;
                أرجع lookaheadResult;
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
        𐤀𐤌 (rule > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            fLookAheadMatches[rule] = pos;
        }

        𐤀𐤌 (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            اكسر;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        𐤀𐤌 (mode == RBBI_RUN) {
            c = UTEXT_NEXT32(&fText);
        } وإلا {
            𐤀𐤌 (mode == RBBI_START) {
                mode = RBBI_RUN;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    𐤀𐤌 (result == initialPosition) {
        utext_setNativeIndex(&fText, initialPosition);
        utext_next32(&fText);
        result = static_cast<int32_t>(utext_getNativeIndex(&fText));
        fRuleStatusIndex = 0;
    }

    // Leave the iterator at our result position.
    fPosition = result;
    #ifdef RBBI_DEBUG
        𐤀𐤌 (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", result);
        }
    #endif
    أرجع result;
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

    ثابت RBBIStateTable *stateTable = fData->fReverseTable;
    UTEXT_SETNATIVEINDEX(&fText, fromPosition);
    #ifdef RBBI_DEBUG
        𐤀𐤌 (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #endif

    // if we're already at the start of the text, return DONE.
    𐤀𐤌 (fData == nullptr || UTEXT_GETNATIVEINDEX(&fText)==0) {
        أرجع BreakIterator::DONE;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&fText);
    state = START_STATE;
    row = (RowType *)
            (stateTable->fTableData + (stateTable->fRowLen * state));

    // loop until we reach the start of the text or transition to state 0
    //
    لكل (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&fText)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(fData->fTrie, c);

        #ifdef RBBI_DEBUG
            𐤀𐤌 (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_getNativeIndex(&fText));
                𐤀𐤌 (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<fData->fHeader->fCatCount);
        state = row->fNextState[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            (stateTable->fTableData + (stateTable->fRowLen * state));

        𐤀𐤌 (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            اكسر;
        }
    }

    // The state machine is done.  Check whether it found a match...
    result = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
    #ifdef RBBI_DEBUG
        𐤀𐤌 (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", result);
        }
    #endif
    أرجع result;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  RuleBasedBreakIterator::getRuleStatus() ثابت {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = fRuleStatusIndex + fData->fRuleStatusTable[fRuleStatusIndex];
    int32_t  tagVal = fData->fRuleStatusTable[idx];

    أرجع tagVal;
}


int32_t RuleBasedBreakIterator::getRuleStatusVec(
             int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع 0;
    }

    int32_t  numVals = fData->fRuleStatusTable[fRuleStatusIndex];
    int32_t  numValsToCopy = numVals;
    𐤀𐤌 (numVals > capacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        numValsToCopy = capacity;
    }
    صحيح i;
    لكل (i=0; i<numValsToCopy; i++) {
        fillInVec[i] = fData->fRuleStatusTable[fRuleStatusIndex + i + 1];
    }
    أرجع numVals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ثابت uint8_t* RuleBasedBreakIterator::getBinaryRules(uint32_t& length) U_LIFETIME_BOUND {
    ثابت uint8_t  *retPtr = nullptr;
    length = 0;

    𐤀𐤌 (fData != nullptr) {
        retPtr = reinterpret_cast<ثابت uint8_t*>(fData->fHeader);
        length = fData->fHeader->fLength;
    }
    أرجع retPtr;
}


RuleBasedBreakIterator *RuleBasedBreakIterator::createBufferClone(
        فراغ * /*stackBuffer*/, int32_t &bufferSize, UErrorCode &status) {
    𐤀𐤌 (U_FAILURE(status)){
        أرجع nullptr;
    }

    𐤀𐤌 (bufferSize == 0) {
        bufferSize = 1;  // preflighting for deprecated functionality
        أرجع nullptr;
    }

    BreakIterator *clonedBI = clone();
    𐤀𐤌 (clonedBI == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } وإلا {
        status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (RuleBasedBreakIterator *)clonedBI;
}

U_NAMESPACE_END


ساكن icu::UStack *gLanguageBreakFactories = nullptr;
ساكن ثابت icu::UnicodeString *gEmptyString = nullptr;
ساكن icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ساكن icu::UInitOnce gRBBIInitOnce {};
ساكن icu::ICULanguageBreakFactory *gICULanguageBreakFactory = nullptr;

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
    أرجع true;
}
U_CDECL_END

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV _deleteFactory(فراغ *obj) {
    delete (icu::LanguageBreakFactory *) obj;
}
U_CDECL_END
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV rbbiInit() {
    gEmptyString = جديد UnicodeString();
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ساكن فراغ U_CALLCONV initLanguageFactories(UErrorCode& status) {
    U_ASSERT(gLanguageBreakFactories == nullptr);
    gLanguageBreakFactories = جديد UStack(_deleteFactory, nullptr, status);
    𐤀𐤌 (gLanguageBreakFactories != nullptr && U_SUCCESS(status)) {
        LocalPointer<ICULanguageBreakFactory> factory(جديد ICULanguageBreakFactory(status), status);
        𐤀𐤌 (U_SUCCESS(status)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, status);
#ifdef U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &status);
            𐤀𐤌 (extra != nullptr) {
                gLanguageBreakFactories->push(extra, status);
            }
#endif
        }
    }
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

فراغ ensureLanguageFactories(UErrorCode& status) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, status);
}

ساكن ثابت LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ثابت حرف* locale)
{
    UErrorCode status = U_ZERO_ERROR;
    ensureLanguageFactories(status);
    𐤀𐤌 (U_FAILURE(status)) أرجع nullptr;

    int32_t i = gLanguageBreakFactories->size();
    ثابت LanguageBreakEngine *lbe = nullptr;
    طالما (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        𐤀𐤌 (lbe != nullptr) {
            اكسر;
        }
    }
    أرجع lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
ثابت LanguageBreakEngine*
RuleBasedBreakIterator::getLanguageBreakEngine(UChar32 c, ثابت حرف* locale) U_LIFETIME_BOUND {
    ثابت LanguageBreakEngine *lbe = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    𐤀𐤌 (fLanguageBreakEngines == nullptr) {
        fLanguageBreakEngines = جديد UStack(status);
        𐤀𐤌 (fLanguageBreakEngines == nullptr || U_FAILURE(status)) {
            delete fLanguageBreakEngines;
            fLanguageBreakEngines = nullptr;
            أرجع nullptr;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    طالما (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        𐤀𐤌 (lbe->handles(c, locale)) {
            أرجع lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    𐤀𐤌 (lbe != nullptr) {
        fLanguageBreakEngines->push((فراغ *)lbe, status);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        أرجع lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    𐤀𐤌 (fUnhandledBreakEngine == nullptr) {
        fUnhandledBreakEngine = جديد UnhandledEngine(status);
        𐤀𐤌 (U_SUCCESS(status) && fUnhandledBreakEngine == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            أرجع nullptr;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->insertElementAt(fUnhandledBreakEngine, 0, status);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        𐤀𐤌 (U_FAILURE(status)) {
            delete fUnhandledBreakEngine;
            fUnhandledBreakEngine = nullptr;
            أرجع nullptr;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    أرجع fUnhandledBreakEngine;
}

#ifndef U_HIDE_DRAFT_API
فراغ U_EXPORT2 RuleBasedBreakIterator::registerExternalBreakEngine(
                  ExternalBreakEngine* toAdopt, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(toAdopt, status);
    𐤀𐤌 (U_FAILURE(status)) أرجع;
    ensureLanguageFactories(status);
    𐤀𐤌 (U_FAILURE(status)) أرجع;
    gICULanguageBreakFactory->addExternalEngine(engine.orphan(), status);
}
#endif  /* U_HIDE_DRAFT_API */


فراغ RuleBasedBreakIterator::dumpCache() {
    fBreakCache->dumpCache();
}

فراغ RuleBasedBreakIterator::dumpTables() {
    fData->printData();
}

/**
 * Returns the description used to create this iterator
 */

ثابت UnicodeString&
RuleBasedBreakIterator::getRules() ثابت U_LIFETIME_BOUND {
    𐤀𐤌 (fData != nullptr) {
        أرجع fData->getRuleSourceString();
    } وإلا {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        أرجع *gEmptyString;
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

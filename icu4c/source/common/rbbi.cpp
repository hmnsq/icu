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

#شامل "utypeinfo.h"  // for 'typeid' to work

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل <cinttypes>

#شامل "unicode/rbbi.h"
#شامل "unicode/schriter.h"
#شامل "unicode/uchriter.h"
#شامل "unicode/uclean.h"
#شامل "unicode/udata.h"

#شامل "brkeng.h"
#شامل "ucln_cmn.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "localsvc.h"
#شامل "rbbidata.h"
#شامل "rbbi_cache.h"
#شامل "rbbirb.h"
#شامل "uassert.h"
#شامل "umutex.h"
#شامل "uvectr32.h"

#ifdef RBBI_DEBUG
ایستا UBool gTrace = نادرست;
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
    fData = جدید RBBIDataWrapper(data, status); // status checked in constructor
    اگر (U_FAILURE(status)) {بازگشت;}
    اگر(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    اگر (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * اندازه(int32_t)));
        اگر (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
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
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    اگر (compiledRules == nullptr || ruleLength < اندازه(RBBIDataHeader)) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    ثابت RBBIDataHeader* data = reinterpret_cast<ثابت RBBIDataHeader*>(compiledRules);
    اگر (data->fLength > ruleLength) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    fData = جدید RBBIDataWrapper(data, RBBIDataWrapper::kDontAdopt, status);
    اگر (U_FAILURE(status)) {بازگشت;}
    اگر(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    اگر (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * اندازه(int32_t)));
        اگر (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
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
    fData = جدید RBBIDataWrapper(udm, status); // status checked in constructor
    اگر (U_FAILURE(status)) {بازگشت;}
    اگر(fData == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    اگر (fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * اندازه(int32_t)));
        اگر (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
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
    اگر (U_FAILURE(status)) {بازگشت;}
    RuleBasedBreakIterator *bi = (RuleBasedBreakIterator *)
        RBBIRuleBuilder::createRuleBasedBreakIterator(rules, &parseError, status);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    اگر (U_SUCCESS(status)) {
        *این = *bi;
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
    اگر (status == nullptr) {
        status = &ec;
    }
    utext_openUChars(&fText, nullptr, 0, status);
    LocalPointer<DictionaryCache> lpDictionaryCache(جدید DictionaryCache(این, *status), *status);
    LocalPointer<BreakCache> lpBreakCache(جدید BreakCache(این, *status), *status);
    اگر (U_FAILURE(*status)) {
        fErrorCode = *status;
        بازگشت;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#ifdef RBBI_DEBUG
    ایستا UBool debugInitDone = نادرست;
    اگر (debugInitDone == نادرست) {
        نویسه *debugEnv = getenv("U_RBBIDEBUG");
        اگر (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = درست;
        }
        debugInitDone = درست;
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
    *این = other;
}


/**
 * Destructor
 */
RuleBasedBreakIterator::~RuleBasedBreakIterator() {
    اگر (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        delete fCharIter;
    }
    fCharIter = nullptr;

    utext_close(&fText);

    اگر (fData != nullptr) {
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
    اگر (این == &that) {
        بازگشت *این;
    }
    BreakIterator::operator=(that);

    اگر (fLanguageBreakEngines != nullptr) {
        delete fLanguageBreakEngines;
        fLanguageBreakEngines = nullptr;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UErrorCode status = U_ZERO_ERROR;
    utext_clone(&fText, &that.fText, نادرست, درست, &status);

    اگر (fCharIter != &fSCharIter) {
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    اگر (that.fCharIter != nullptr && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    اگر (fCharIter == nullptr) {
        fCharIter = &fSCharIter;
    }

    اگر (fData != nullptr) {
        fData->removeReference();
        fData = nullptr;
    }
    اگر (that.fData != nullptr) {
        fData = that.fData->addReference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = nullptr;
    اگر (fData && fData->fForwardTable->fLookAheadResultsSize > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(fData->fForwardTable->fLookAheadResultsSize * اندازه(int32_t)));
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

    بازگشت *این;
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
    بازگشت جدید RuleBasedBreakIterator(*این);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
منطقی
RuleBasedBreakIterator::operator==(ثابت BreakIterator& that) ثابت {
    اگر (typeid(*این) != typeid(that)) {
        بازگشت نادرست;
    }
    اگر (این == &that) {
        بازگشت درست;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ثابت RuleBasedBreakIterator& that2 = static_cast<ثابت RuleBasedBreakIterator&>(that);

    اگر (!utext_equals(&fText, &that2.fText)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        بازگشت نادرست;
    }

    اگر (!(fPosition == that2.fPosition &&
            fRuleStatusIndex == that2.fRuleStatusIndex &&
            fDone == that2.fDone &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        بازگشت نادرست;
    }

    اگر (that2.fData == fData ||
        (fData != nullptr && that2.fData != nullptr && *that2.fData == *fData)) {
            // The two break iterators are using the same rules.
            بازگشت درست;
        }
    بازگشت نادرست;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
RuleBasedBreakIterator::hashCode() ثابت {
    int32_t   hash = 0;
    اگر (fData != nullptr) {
        hash = fData->hashCode();
    }
    بازگشت hash;
}


تهی RuleBasedBreakIterator::setText(UText *ut, UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&fText, ut, نادرست, درست, &status);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.setText(u"", 0);

    اگر (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    این->first();
}


UText* RuleBasedBreakIterator::getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ثابت {
    UText *result = utext_clone(fillIn, &fText, نادرست, درست, &status);
    بازگشت result;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
RuleBasedBreakIterator::getText() ثابت U_LIFETIME_BOUND {
    بازگشت *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
تهی
RuleBasedBreakIterator::adoptText(CharacterIterator* newText) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    اگر (fCharIter != &fSCharIter) {
        delete fCharIter;
    }

    fCharIter = newText;
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    اگر (newText==nullptr || newText->startIndex() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&fText, nullptr, 0, &status);
    } وگرنه {
        utext_openCharacterIterator(&fText, newText, &status);
    }
    این->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
تهی
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

    اگر (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    این->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
RuleBasedBreakIterator&
RuleBasedBreakIterator::refresh_درون_داد_Text(UText* input, UErrorCode& status) U_LIFETIME_BOUND {
    اگر (U_FAILURE(status)) {
        بازگشت *این;
    }
    اگر (input == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت *این;
    }
    int64_t pos = utext_getNativeIndex(&fText);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&fText, input, نادرست, درست, &status);
    اگر (U_FAILURE(status)) {
        بازگشت *این;
    }
    utext_setNativeIndex(&fText, pos);
    اگر (utext_getNativeIndex(&fText) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    بازگشت *این;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t RuleBasedBreakIterator::first() {
    UErrorCode status = U_ZERO_ERROR;
    اگر (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, status);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    بازگشت 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t RuleBasedBreakIterator::last() {
    int32_t endPos = static_cast<int32_t>(utext_nativeLength(&fText));
    UBool endShouldBeBoundary = isBoundary(endPos);      // Has side effect of setting iterator position.
    (تهی)endShouldBeBoundary;
    U_ASSERT(endShouldBeBoundary);
    U_ASSERT(fPosition == endPos);
    بازگشت endPos;
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
    اگر (n > 0) {
        برای (; n > 0 && result != UBRK_DONE; --n) {
            result = next();
        }
    } وگرنه اگر (n < 0) {
        برای (; n < 0 && result != UBRK_DONE; ++n) {
            result = previous();
        }
    } وگرنه {
        result = current();
    }
    بازگشت result;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t RuleBasedBreakIterator::next() {
    fBreakCache->next();
    بازگشت fDone ? UBRK_DONE : fPosition;
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
    بازگشت fDone ? UBRK_DONE : fPosition;
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
    اگر (startPos < 0) {
        بازگشت first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_setNativeIndex(&fText, startPos);
    startPos = static_cast<int32_t>(utext_getNativeIndex(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->following(startPos, status);
    بازگشت fDone ? UBRK_DONE : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t RuleBasedBreakIterator::preceding(int32_t offset) {
    اگر (offset > utext_nativeLength(&fText)) {
        بازگشت last();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_setNativeIndex(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_getNativeIndex(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->preceding(adjustedOffset, status);
    بازگشت fDone ? UBRK_DONE : fPosition;
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
    اگر (offset < 0) {
        first();       // For side effects on current position, tag values.
        بازگشت نادرست;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_setNativeIndex(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_getNativeIndex(&fText));

    منطقی result = نادرست;
    UErrorCode status = U_ZERO_ERROR;
    اگر (fBreakCache->seek(adjustedOffset) || fBreakCache->populateNear(adjustedOffset, status)) {
        result = (fBreakCache->current() == offset);
    }

    اگر (result && adjustedOffset < offset && utext_char32At(&fText, offset) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        بازگشت نادرست;
    }
    اگر (!result) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    بازگشت result;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t RuleBasedBreakIterator::current() ثابت {
    بازگشت fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
شمارش RBBIRunMode {
    RBBI_START,     // state machine processing is before first char of input
    RBBI_RUN,       // state machine processing is in the user text
    RBBI_END        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
ایستا درونخطی uint16_t TrieFunc8(ثابت UCPTrie *trie, UChar32 c) {
    بازگشت UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

ایستا درونخطی uint16_t TrieFunc16(ثابت UCPTrie *trie, UChar32 c) {
    بازگشت UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t RuleBasedBreakIterator::handleNext() {
    ثابت RBBIStateTable *statetable = fData->fForwardTable;
    منطقی use8BitsTrie = ucptrie_get_مقدار_Width(fData->fTrie) == UCPTRIE_مقدار_BITS_8;
    اگر (statetable->fFlags & RBBI_8BITS_ROWS) {
        اگر (use8BitsTrie) {
            بازگشت handleNext<RBBIStateTableRow8, TrieFunc8>();
        } وگرنه {
            بازگشت handleNext<RBBIStateTableRow8, TrieFunc16>();
        }
    } وگرنه {
        اگر (use8BitsTrie) {
            بازگشت handleNext<RBBIStateTableRow16, TrieFunc8>();
        } وگرنه {
            بازگشت handleNext<RBBIStateTableRow16, TrieFunc16>();
        }
    }
}

int32_t RuleBasedBreakIterator::handleSafePrevious(int32_t fromPosition) {
    ثابت RBBIStateTable *statetable = fData->fReverseTable;
    منطقی use8BitsTrie = ucptrie_get_مقدار_Width(fData->fTrie) == UCPTRIE_مقدار_BITS_8;
    اگر (statetable->fFlags & RBBI_8BITS_ROWS) {
        اگر (use8BitsTrie) {
            بازگشت handleSafePrevious<RBBIStateTableRow8, TrieFunc8>(fromPosition);
        } وگرنه {
            بازگشت handleSafePrevious<RBBIStateTableRow8, TrieFunc16>(fromPosition);
        }
    } وگرنه {
        اگر (use8BitsTrie) {
            بازگشت handleSafePrevious<RBBIStateTableRow16, TrieFunc8>(fromPosition);
        } وگرنه {
            بازگشت handleSafePrevious<RBBIStateTableRow16, TrieFunc16>(fromPosition);
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
    ثابت نویسه         *tableData          = statetable->fTableData;
    uint32_t            tableRowLen        = statetable->fRowLen;
    uint32_t            dictStart          = statetable->fDictCategoriesStart;
    #ifdef RBBI_DEBUG
        اگر (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #endif

    // handleNext always sets the break tag value.
    // Set the default for it.
    fRuleStatusIndex = 0;

    f_Dictionary_Char_شمار = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&fText, initialPosition);
    result          = initialPosition;
    c               = UTEXT_NEXT32(&fText);
    اگر (c==U_SENTINEL) {
        fDone = درست;
        بازگشت UBRK_DONE;
    }

    //  Set the initial state for the state machine
    state = START_STATE;
    row = (RowType *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (tableData + tableRowLen * state);


    mode     = RBBI_RUN;


    // loop until we reach the end of the text or transition to state 0
    //
    برای (;;) {
        اگر (c == U_SENTINEL) {
            // Reached end of input string.
            اگر (mode == RBBI_END) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                بشکن;
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
        اگر (mode == RBBI_RUN) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(fData->fTrie, c);
            f_Dictionary_Char_شمار += (category >= dictStart);
        }

       #ifdef RBBI_DEBUG
            اگر (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_getNativeIndex(&fText));
                اگر (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وگرنه {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<fData->fHeader->f_Cat_شمار);
        state = row->fNextState[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (tableData + tableRowLen * state);


        uint16_t accepting = row->fAccepting;
        اگر (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            اگر (mode != RBBI_START) {
                result = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            }
            fRuleStatusIndex = row->fTagsIdx;   // Remember the break status (tag) values.
        } وگرنه اگر (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < fData->fForwardTable->fLookAheadResultsSize);
            int32_t lookaheadResult = fLookAheadMatches[accepting];
            اگر (lookaheadResult >= 0) {
                fRuleStatusIndex = row->fTagsIdx;
                fPosition = lookaheadResult;
                بازگشت lookaheadResult;
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
        اگر (rule > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            fLookAheadMatches[rule] = pos;
        }

        اگر (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            بشکن;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        اگر (mode == RBBI_RUN) {
            c = UTEXT_NEXT32(&fText);
        } وگرنه {
            اگر (mode == RBBI_START) {
                mode = RBBI_RUN;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    اگر (result == initialPosition) {
        utext_setNativeIndex(&fText, initialPosition);
        utext_next32(&fText);
        result = static_cast<int32_t>(utext_getNativeIndex(&fText));
        fRuleStatusIndex = 0;
    }

    // Leave the iterator at our result position.
    fPosition = result;
    #ifdef RBBI_DEBUG
        اگر (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", result);
        }
    #endif
    بازگشت result;
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
        اگر (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #endif

    // if we're already at the start of the text, return DONE.
    اگر (fData == nullptr || UTEXT_GETNATIVEINDEX(&fText)==0) {
        بازگشت BreakIterator::DONE;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&fText);
    state = START_STATE;
    row = (RowType *)
            (stateTable->fTableData + (stateTable->fRowLen * state));

    // loop until we reach the start of the text or transition to state 0
    //
    برای (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&fText)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(fData->fTrie, c);

        #ifdef RBBI_DEBUG
            اگر (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_getNativeIndex(&fText));
                اگر (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وگرنه {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<fData->fHeader->f_Cat_شمار);
        state = row->fNextState[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            (stateTable->fTableData + (stateTable->fRowLen * state));

        اگر (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            بشکن;
        }
    }

    // The state machine is done.  Check whether it found a match...
    result = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
    #ifdef RBBI_DEBUG
        اگر (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", result);
        }
    #endif
    بازگشت result;
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

    بازگشت tagVal;
}


int32_t RuleBasedBreakIterator::getRuleStatusVec(
             int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت 0;
    }

    int32_t  رقم_Vals = fData->fRuleStatusTable[fRuleStatusIndex];
    int32_t  رقم_Vals_To_Copy = رقم_Vals;
    اگر (رقم_Vals > capacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        رقم_Vals_To_Copy = capacity;
    }
    صحیح i;
    برای (i=0; i<رقم_Vals_To_Copy; i++) {
        fillInVec[i] = fData->fRuleStatusTable[fRuleStatusIndex + i + 1];
    }
    بازگشت رقم_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ثابت uint8_t* RuleBasedBreakIterator::get_دودویی_Rules(uint32_t& length) U_LIFETIME_BOUND {
    ثابت uint8_t  *retPtr = nullptr;
    length = 0;

    اگر (fData != nullptr) {
        retPtr = reinterpret_cast<ثابت uint8_t*>(fData->fHeader);
        length = fData->fHeader->fLength;
    }
    بازگشت retPtr;
}


RuleBasedBreakIterator *RuleBasedBreakIterator::createBufferClone(
        تهی * /*stackBuffer*/, int32_t &bufferSize, UErrorCode &status) {
    اگر (U_FAILURE(status)){
        بازگشت nullptr;
    }

    اگر (bufferSize == 0) {
        bufferSize = 1;  // preflighting for deprecated functionality
        بازگشت nullptr;
    }

    BreakIterator *clonedBI = clone();
    اگر (clonedBI == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } وگرنه {
        status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    بازگشت (RuleBasedBreakIterator *)clonedBI;
}

U_NAMESPACE_END


ایستا icu::UStack *gLanguageBreakFactories = nullptr;
ایستا ثابت icu::UnicodeString *gEmptyString = nullptr;
ایستا icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ایستا icu::UInitOnce gRBBIInitOnce {};
ایستا icu::ICULanguageBreakFactory *gICULanguageBreakFactory = nullptr;

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
    بازگشت درست;
}
U_CDECL_END

U_CDECL_BEGIN
ایستا تهی U_CALLCONV _deleteFactory(تهی *obj) {
    delete (icu::LanguageBreakFactory *) obj;
}
U_CDECL_END
U_NAMESPACE_BEGIN

ایستا تهی U_CALLCONV rbbiInit() {
    gEmptyString = جدید UnicodeString();
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ایستا تهی U_CALLCONV initLanguageFactories(UErrorCode& status) {
    U_ASSERT(gLanguageBreakFactories == nullptr);
    gLanguageBreakFactories = جدید UStack(_deleteFactory, nullptr, status);
    اگر (gLanguageBreakFactories != nullptr && U_SUCCESS(status)) {
        LocalPointer<ICULanguageBreakFactory> factory(جدید ICULanguageBreakFactory(status), status);
        اگر (U_SUCCESS(status)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, status);
#ifdef U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &status);
            اگر (extra != nullptr) {
                gLanguageBreakFactories->push(extra, status);
            }
#endif
        }
    }
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

تهی ensureLanguageFactories(UErrorCode& status) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, status);
}

ایستا ثابت LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ثابت نویسه* locale)
{
    UErrorCode status = U_ZERO_ERROR;
    ensureLanguageFactories(status);
    اگر (U_FAILURE(status)) بازگشت nullptr;

    int32_t i = gLanguageBreakFactories->size();
    ثابت LanguageBreakEngine *lbe = nullptr;
    وقتی (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        اگر (lbe != nullptr) {
            بشکن;
        }
    }
    بازگشت lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
ثابت LanguageBreakEngine*
RuleBasedBreakIterator::getLanguageBreakEngine(UChar32 c, ثابت نویسه* locale) U_LIFETIME_BOUND {
    ثابت LanguageBreakEngine *lbe = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    اگر (fLanguageBreakEngines == nullptr) {
        fLanguageBreakEngines = جدید UStack(status);
        اگر (fLanguageBreakEngines == nullptr || U_FAILURE(status)) {
            delete fLanguageBreakEngines;
            fLanguageBreakEngines = nullptr;
            بازگشت nullptr;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    وقتی (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        اگر (lbe->handles(c, locale)) {
            بازگشت lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    اگر (lbe != nullptr) {
        fLanguageBreakEngines->push((تهی *)lbe, status);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        بازگشت lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    اگر (fUnhandledBreakEngine == nullptr) {
        fUnhandledBreakEngine = جدید UnhandledEngine(status);
        اگر (U_SUCCESS(status) && fUnhandledBreakEngine == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت nullptr;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->درج_Element_At(fUnhandledBreakEngine, 0, status);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        اگر (U_FAILURE(status)) {
            delete fUnhandledBreakEngine;
            fUnhandledBreakEngine = nullptr;
            بازگشت nullptr;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    بازگشت fUnhandledBreakEngine;
}

#ifndef U_HIDE_DRAFT_API
تهی U_EXPORT2 RuleBasedBreakIterator::registerExternalBreakEngine(
                  ExternalBreakEngine* toAdopt, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(toAdopt, status);
    اگر (U_FAILURE(status)) بازگشت;
    ensureLanguageFactories(status);
    اگر (U_FAILURE(status)) بازگشت;
    gICULanguageBreakFactory->addExternalEngine(engine.orphan(), status);
}
#endif  /* U_HIDE_DRAFT_API */


تهی RuleBasedBreakIterator::dumpCache() {
    fBreakCache->dumpCache();
}

تهی RuleBasedBreakIterator::dumpTables() {
    fData->printData();
}

/**
 * Returns the description used to create this iterator
 */

ثابت UnicodeString&
RuleBasedBreakIterator::getRules() ثابت U_LIFETIME_BOUND {
    اگر (fData != nullptr) {
        بازگشت fData->getRuleSourceString();
    } وگرنه {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        بازگشت *gEmptyString;
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

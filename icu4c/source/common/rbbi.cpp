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

#समावेश "utypeinfo.h"  // for 'typeid' to work

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश <cinttypes>

#समावेश "unicode/rbbi.h"
#समावेश "unicode/schriter.h"
#समावेश "unicode/uchriter.h"
#समावेश "unicode/uclean.h"
#समावेश "unicode/udata.h"

#समावेश "brkeng.h"
#समावेश "ucln_cmn.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "localsvc.h"
#समावेश "rbbidata.h"
#समावेश "rbbi_cache.h"
#समावेश "rbbirb.h"
#समावेश "uassert.h"
#समावेश "umutex.h"
#समावेश "uvectr32.h"

#अगर_घोषित RBBI_DEBUG
अटल UBool gTrace = false;
#पूर्ण_अगर

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
RuleBasedBreakIterator::RuleBasedBreakIterator(RBBIDataHeader* तथ्य, UErrorCode &status)
 : RuleBasedBreakIterator(&status)
{
    f_तथ्य = नया RBBIDataWrapper(तथ्य, status); // status checked in constructor
    अगर (U_FAILURE(status)) {वापस;}
    अगर(f_तथ्य == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    अगर (f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार * माप(int32_t)));
        अगर (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस;
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
RuleBasedBreakIterator::RuleBasedBreakIterator(स्थिर uint8_t *compiledRules,
                       uint32_t       rule_लंबाई,
                       UErrorCode     &status)
 : RuleBasedBreakIterator(&status)
{
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (compiledRules == nullptr || rule_लंबाई < माप(RBBIDataHeader)) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    स्थिर RBBIDataHeader* तथ्य = reinterpret_cast<स्थिर RBBIDataHeader*>(compiledRules);
    अगर (तथ्य->f_लंबाई > rule_लंबाई) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    f_तथ्य = नया RBBIDataWrapper(तथ्य, RBBIDataWrapper::kDontAdopt, status);
    अगर (U_FAILURE(status)) {वापस;}
    अगर(f_तथ्य == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    अगर (f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार * माप(int32_t)));
        अगर (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस;
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
    f_तथ्य = नया RBBIDataWrapper(udm, status); // status checked in constructor
    अगर (U_FAILURE(status)) {वापस;}
    अगर(f_तथ्य == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    अगर (f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार * माप(int32_t)));
        अगर (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator( स्थिर UnicodeString  &rules,
                                                UParseError          &parseError,
                                                UErrorCode           &status)
 : RuleBasedBreakIterator(&status)
{
    अगर (U_FAILURE(status)) {वापस;}
    RuleBasedBreakIterator *bi = (RuleBasedBreakIterator *)
        RBBIRuleBuilder::createRuleBasedBreakIterator(rules, &parseError, status);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    अगर (U_SUCCESS(status)) {
        *यह = *bi;
        मिटाओ bi;
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
    अगर (status == nullptr) {
        status = &ec;
    }
    utext_openUChars(&fText, nullptr, 0, status);
    LocalPointer<DictionaryCache> lpDictionaryCache(नया DictionaryCache(यह, *status), *status);
    LocalPointer<BreakCache> lpBreakCache(नया BreakCache(यह, *status), *status);
    अगर (U_FAILURE(*status)) {
        fErrorCode = *status;
        वापस;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#अगर_घोषित RBBI_DEBUG
    अटल UBool debugInitDone = false;
    अगर (debugInitDone == false) {
        अक्षर *debugEnv = दो_पर्या("U_RBBIDEBUG");
        अगर (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = true;
        }
        debugInitDone = true;
    }
#पूर्ण_अगर
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator(स्थिर RuleBasedBreakIterator& other)
: RuleBasedBreakIterator()
{
    *यह = other;
}


/**
 * Destructor
 */
RuleBasedBreakIterator::~RuleBasedBreakIterator() {
    अगर (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        मिटाओ fCharIter;
    }
    fCharIter = nullptr;

    utext_close(&fText);

    अगर (f_तथ्य != nullptr) {
        f_तथ्य->removeReference();
        f_तथ्य = nullptr;
    }
    मिटाओ fBreakCache;
    fBreakCache = nullptr;

    मिटाओ fDictionaryCache;
    fDictionaryCache = nullptr;

    मिटाओ fLanguageBreakEngines;
    fLanguageBreakEngines = nullptr;

    मिटाओ fUnhandledBreakEngine;
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
RuleBasedBreakIterator::चालक=(स्थिर RuleBasedBreakIterator& that) U_LIFETIME_BOUND {
    अगर (यह == &that) {
        वापस *यह;
    }
    BreakIterator::चालक=(that);

    अगर (fLanguageBreakEngines != nullptr) {
        मिटाओ fLanguageBreakEngines;
        fLanguageBreakEngines = nullptr;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UErrorCode status = U_ZERO_ERROR;
    utext_clone(&fText, &that.fText, false, true, &status);

    अगर (fCharIter != &fSCharIter) {
        मिटाओ fCharIter;
    }
    fCharIter = &fSCharIter;

    अगर (that.fCharIter != nullptr && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    अगर (fCharIter == nullptr) {
        fCharIter = &fSCharIter;
    }

    अगर (f_तथ्य != nullptr) {
        f_तथ्य->removeReference();
        f_तथ्य = nullptr;
    }
    अगर (that.f_तथ्य != nullptr) {
        f_तथ्य = that.f_तथ्य->addReference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = nullptr;
    अगर (f_तथ्य && f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार * माप(int32_t)));
    }


    fPosition = that.fPosition;
    f_Rule_Status_सूचकांक = that.f_Rule_Status_सूचकांक;
    fDone = that.fDone;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_Rule_Status_सूचकांक);
    fDictionaryCache->reset();

    वापस *यह;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
RuleBasedBreakIterator*
RuleBasedBreakIterator::clone() स्थिर {
    वापस नया RuleBasedBreakIterator(*यह);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
bool
RuleBasedBreakIterator::चालक==(स्थिर BreakIterator& that) स्थिर {
    अगर (typeid(*यह) != typeid(that)) {
        वापस false;
    }
    अगर (यह == &that) {
        वापस true;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    स्थिर RuleBasedBreakIterator& that2 = static_cast<स्थिर RuleBasedBreakIterator&>(that);

    अगर (!utext_equals(&fText, &that2.fText)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        वापस false;
    }

    अगर (!(fPosition == that2.fPosition &&
            f_Rule_Status_सूचकांक == that2.f_Rule_Status_सूचकांक &&
            fDone == that2.fDone &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        वापस false;
    }

    अगर (that2.f_तथ्य == f_तथ्य ||
        (f_तथ्य != nullptr && that2.f_तथ्य != nullptr && *that2.f_तथ्य == *f_तथ्य)) {
            // The two break iterators are using the same rules.
            वापस true;
        }
    वापस false;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
RuleBasedBreakIterator::hashCode() स्थिर {
    int32_t   hash = 0;
    अगर (f_तथ्य != nullptr) {
        hash = f_तथ्य->hashCode();
    }
    वापस hash;
}


व्योम RuleBasedBreakIterator::setText(UText *ut, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&fText, ut, false, true, &status);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.setText(u"", 0);

    अगर (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        मिटाओ fCharIter;
    }
    fCharIter = &fSCharIter;

    यह->first();
}


UText* RuleBasedBreakIterator::getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) स्थिर {
    UText *परिणाम = utext_clone(fillIn, &fText, false, true, &status);
    वापस परिणाम;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
RuleBasedBreakIterator::getText() स्थिर U_LIFETIME_BOUND {
    वापस *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
व्योम
RuleBasedBreakIterator::adoptText(CharacterIterator* newText) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    अगर (fCharIter != &fSCharIter) {
        मिटाओ fCharIter;
    }

    fCharIter = newText;
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    अगर (newText==nullptr || newText->start_सूचकांक() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&fText, nullptr, 0, &status);
    } अन्यथा {
        utext_openCharacterIterator(&fText, newText, &status);
    }
    यह->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
व्योम
RuleBasedBreakIterator::setText(स्थिर UnicodeString& newText) {
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&fText, &newText, &status);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.setText(newText.get_बफर(), newText.length());

    अगर (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        मिटाओ fCharIter;
    }
    fCharIter = &fSCharIter;

    यह->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
RuleBasedBreakIterator&
RuleBasedBreakIterator::refresh_निवेश_Text(UText* input, UErrorCode& status) U_LIFETIME_BOUND {
    अगर (U_FAILURE(status)) {
        वापस *यह;
    }
    अगर (input == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस *यह;
    }
    int64_t pos = utext_get_Native_सूचकांक(&fText);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&fText, input, false, true, &status);
    अगर (U_FAILURE(status)) {
        वापस *यह;
    }
    utext_set_Native_सूचकांक(&fText, pos);
    अगर (utext_get_Native_सूचकांक(&fText) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    वापस *यह;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t RuleBasedBreakIterator::first() {
    UErrorCode status = U_ZERO_ERROR;
    अगर (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, status);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    वापस 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t RuleBasedBreakIterator::last() {
    int32_t endPos = static_cast<int32_t>(utext_native_लंबाई(&fText));
    UBool endShouldBeBoundary = isBoundary(endPos);      // Has side effect of setting iterator position.
    (व्योम)endShouldBeBoundary;
    U_ASSERT(endShouldBeBoundary);
    U_ASSERT(fPosition == endPos);
    वापस endPos;
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
    int32_t परिणाम = 0;
    अगर (n > 0) {
        क्रम (; n > 0 && परिणाम != UBRK_DONE; --n) {
            परिणाम = next();
        }
    } अन्यथा अगर (n < 0) {
        क्रम (; n < 0 && परिणाम != UBRK_DONE; ++n) {
            परिणाम = previous();
        }
    } अन्यथा {
        परिणाम = current();
    }
    वापस परिणाम;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t RuleBasedBreakIterator::next() {
    fBreakCache->next();
    वापस fDone ? UBRK_DONE : fPosition;
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
    वापस fDone ? UBRK_DONE : fPosition;
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
    अगर (startPos < 0) {
        वापस first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_set_Native_सूचकांक(&fText, startPos);
    startPos = static_cast<int32_t>(utext_get_Native_सूचकांक(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->following(startPos, status);
    वापस fDone ? UBRK_DONE : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t RuleBasedBreakIterator::preceding(int32_t offset) {
    अगर (offset > utext_native_लंबाई(&fText)) {
        वापस last();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_set_Native_सूचकांक(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_get_Native_सूचकांक(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->preceding(adjustedOffset, status);
    वापस fDone ? UBRK_DONE : fPosition;
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
    अगर (offset < 0) {
        first();       // For side effects on current position, tag values.
        वापस false;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_set_Native_सूचकांक(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_get_Native_सूचकांक(&fText));

    bool परिणाम = false;
    UErrorCode status = U_ZERO_ERROR;
    अगर (fBreakCache->seek(adjustedOffset) || fBreakCache->populateNear(adjustedOffset, status)) {
        परिणाम = (fBreakCache->current() == offset);
    }

    अगर (परिणाम && adjustedOffset < offset && utext_char32At(&fText, offset) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        वापस false;
    }
    अगर (!परिणाम) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    वापस परिणाम;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t RuleBasedBreakIterator::current() स्थिर {
    वापस fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
क्रमागत RBBIRunMode {
    RBBI_START,     // state machine processing is before first char of input
    RBBI_RUN,       // state machine processing is in the user text
    RBBI_END        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
अटल अंतरभूत uint16_t TrieFunc8(स्थिर UCPTrie *trie, UChar32 c) {
    वापस UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

अटल अंतरभूत uint16_t TrieFunc16(स्थिर UCPTrie *trie, UChar32 c) {
    वापस UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t RuleBasedBreakIterator::handle_अग्र() {
    स्थिर RBBIStateTable *statetable = f_तथ्य->fForwardTable;
    bool use8BitsTrie = ucptrie_get_मूल्य_Width(f_तथ्य->fTrie) == UCPTRIE_मूल्य_BITS_8;
    अगर (statetable->fFlags & RBBI_8BITS_ROWS) {
        अगर (use8BitsTrie) {
            वापस handle_अग्र<RBBIStateTableRow8, TrieFunc8>();
        } अन्यथा {
            वापस handle_अग्र<RBBIStateTableRow8, TrieFunc16>();
        }
    } अन्यथा {
        अगर (use8BitsTrie) {
            वापस handle_अग्र<RBBIStateTableRow16, TrieFunc8>();
        } अन्यथा {
            वापस handle_अग्र<RBBIStateTableRow16, TrieFunc16>();
        }
    }
}

int32_t RuleBasedBreakIterator::handleSafePrevious(int32_t fromPosition) {
    स्थिर RBBIStateTable *statetable = f_तथ्य->fReverseTable;
    bool use8BitsTrie = ucptrie_get_मूल्य_Width(f_तथ्य->fTrie) == UCPTRIE_मूल्य_BITS_8;
    अगर (statetable->fFlags & RBBI_8BITS_ROWS) {
        अगर (use8BitsTrie) {
            वापस handleSafePrevious<RBBIStateTableRow8, TrieFunc8>(fromPosition);
        } अन्यथा {
            वापस handleSafePrevious<RBBIStateTableRow8, TrieFunc16>(fromPosition);
        }
    } अन्यथा {
        अगर (use8BitsTrie) {
            वापस handleSafePrevious<RBBIStateTableRow16, TrieFunc8>(fromPosition);
        } अन्यथा {
            वापस handleSafePrevious<RBBIStateTableRow16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
ढाँचा <typename RowType, RuleBasedBreakIterator::PTrieFunc trieFunc>
int32_t RuleBasedBreakIterator::handle_अग्र() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRunMode         mode;

    RowType             *row;
    UChar32             c;
    int32_t             परिणाम             = 0;
    int32_t             initialPosition    = 0;
    स्थिर RBBIStateTable *statetable       = f_तथ्य->fForwardTable;
    स्थिर अक्षर         *table_तथ्य          = statetable->f_Table_तथ्य;
    uint32_t            tableRowLen        = statetable->fRowLen;
    uint32_t            dictStart          = statetable->fDictCategoriesStart;
    #अगर_घोषित RBBI_DEBUG
        अगर (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #पूर्ण_अगर

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_Rule_Status_सूचकांक = 0;

    f_Dictionary_Char_गिनती = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&fText, initialPosition);
    परिणाम          = initialPosition;
    c               = UTEXT_NEXT32(&fText);
    अगर (c==U_SENTINEL) {
        fDone = true;
        वापस UBRK_DONE;
    }

    //  Set the initial state for the state machine
    state = START_STATE;
    row = (RowType *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (table_तथ्य + tableRowLen * state);


    mode     = RBBI_RUN;


    // loop until we reach the end of the text or transition to state 0
    //
    क्रम (;;) {
        अगर (c == U_SENTINEL) {
            // Reached end of input string.
            अगर (mode == RBBI_END) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                अवरोध;
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
        अगर (mode == RBBI_RUN) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_तथ्य->fTrie, c);
            f_Dictionary_Char_गिनती += (category >= dictStart);
        }

       #अगर_घोषित RBBI_DEBUG
            अगर (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_सूचकांक(&fText));
                अगर (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } अन्यथा {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #पूर्ण_अगर

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_तथ्य->fHeader->f_Cat_गिनती);
        state = row->f_अग्र_State[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (table_तथ्य + tableRowLen * state);


        uint16_t accepting = row->fAccepting;
        अगर (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            अगर (mode != RBBI_START) {
                परिणाम = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            }
            f_Rule_Status_सूचकांक = row->fTagsIdx;   // Remember the break status (tag) values.
        } अन्यथा अगर (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार);
            int32_t lookahead_परिणाम = fLookAheadMatches[accepting];
            अगर (lookahead_परिणाम >= 0) {
                f_Rule_Status_सूचकांक = row->fTagsIdx;
                fPosition = lookahead_परिणाम;
                वापस lookahead_परिणाम;
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
        U_ASSERT(rule == 0 || rule < f_तथ्य->fForwardTable->f_Look_Ahead_Results_आकार);
        अगर (rule > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            fLookAheadMatches[rule] = pos;
        }

        अगर (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            अवरोध;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        अगर (mode == RBBI_RUN) {
            c = UTEXT_NEXT32(&fText);
        } अन्यथा {
            अगर (mode == RBBI_START) {
                mode = RBBI_RUN;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    अगर (परिणाम == initialPosition) {
        utext_set_Native_सूचकांक(&fText, initialPosition);
        utext_next32(&fText);
        परिणाम = static_cast<int32_t>(utext_get_Native_सूचकांक(&fText));
        f_Rule_Status_सूचकांक = 0;
    }

    // Leave the iterator at our result position.
    fPosition = परिणाम;
    #अगर_घोषित RBBI_DEBUG
        अगर (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", परिणाम);
        }
    #पूर्ण_अगर
    वापस परिणाम;
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
ढाँचा <typename RowType, RuleBasedBreakIterator::PTrieFunc trieFunc>
int32_t RuleBasedBreakIterator::handleSafePrevious(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    RowType            *row;
    UChar32             c;
    int32_t             परिणाम          = 0;

    स्थिर RBBIStateTable *stateTable = f_तथ्य->fReverseTable;
    UTEXT_SETNATIVEINDEX(&fText, fromPosition);
    #अगर_घोषित RBBI_DEBUG
        अगर (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #पूर्ण_अगर

    // if we're already at the start of the text, return DONE.
    अगर (f_तथ्य == nullptr || UTEXT_GETNATIVEINDEX(&fText)==0) {
        वापस BreakIterator::DONE;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&fText);
    state = START_STATE;
    row = (RowType *)
            (stateTable->f_Table_तथ्य + (stateTable->fRowLen * state));

    // loop until we reach the start of the text or transition to state 0
    //
    क्रम (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&fText)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_तथ्य->fTrie, c);

        #अगर_घोषित RBBI_DEBUG
            अगर (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_सूचकांक(&fText));
                अगर (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } अन्यथा {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #पूर्ण_अगर

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_तथ्य->fHeader->f_Cat_गिनती);
        state = row->f_अग्र_State[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            (stateTable->f_Table_तथ्य + (stateTable->fRowLen * state));

        अगर (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            अवरोध;
        }
    }

    // The state machine is done.  Check whether it found a match...
    परिणाम = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
    #अगर_घोषित RBBI_DEBUG
        अगर (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", परिणाम);
        }
    #पूर्ण_अगर
    वापस परिणाम;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  RuleBasedBreakIterator::getRuleStatus() स्थिर {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_Rule_Status_सूचकांक + f_तथ्य->fRuleStatusTable[f_Rule_Status_सूचकांक];
    int32_t  tagVal = f_तथ्य->fRuleStatusTable[idx];

    वापस tagVal;
}


int32_t RuleBasedBreakIterator::getRuleStatusVec(
             int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस 0;
    }

    int32_t  अंक_Vals = f_तथ्य->fRuleStatusTable[f_Rule_Status_सूचकांक];
    int32_t  अंक_Vals_To_Copy = अंक_Vals;
    अगर (अंक_Vals > capacity) {
        status = U_बफर_OVERFLOW_ERROR;
        अंक_Vals_To_Copy = capacity;
    }
    पूर्णांक i;
    क्रम (i=0; i<अंक_Vals_To_Copy; i++) {
        fillInVec[i] = f_तथ्य->fRuleStatusTable[f_Rule_Status_सूचकांक + i + 1];
    }
    वापस अंक_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
स्थिर uint8_t* RuleBasedBreakIterator::get_द्विआधारी_Rules(uint32_t& length) U_LIFETIME_BOUND {
    स्थिर uint8_t  *retPtr = nullptr;
    length = 0;

    अगर (f_तथ्य != nullptr) {
        retPtr = reinterpret_cast<स्थिर uint8_t*>(f_तथ्य->fHeader);
        length = f_तथ्य->fHeader->f_लंबाई;
    }
    वापस retPtr;
}


RuleBasedBreakIterator *RuleBasedBreakIterator::create_बफर_Clone(
        व्योम * /*stackBuffer*/, int32_t &बफर_आकार, UErrorCode &status) {
    अगर (U_FAILURE(status)){
        वापस nullptr;
    }

    अगर (बफर_आकार == 0) {
        बफर_आकार = 1;  // preflighting for deprecated functionality
        वापस nullptr;
    }

    BreakIterator *clonedBI = clone();
    अगर (clonedBI == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    वापस (RuleBasedBreakIterator *)clonedBI;
}

U_NAMESPACE_END


अटल icu::UStack *gLanguageBreakFactories = nullptr;
अटल स्थिर icu::UnicodeString *gEmptyString = nullptr;
अटल icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
अटल icu::UInitOnce gRBBIInitOnce {};
अटल icu::ICULanguageBreakFactory *gICULanguageBreakFactory = nullptr;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    मिटाओ gLanguageBreakFactories;
    gLanguageBreakFactories = nullptr;
    मिटाओ gEmptyString;
    gEmptyString = nullptr;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    वापस true;
}
U_CDECL_END

U_CDECL_BEGIN
अटल व्योम U_CALLCONV _deleteFactory(व्योम *obj) {
    मिटाओ (icu::LanguageBreakFactory *) obj;
}
U_CDECL_END
U_NAMESPACE_BEGIN

अटल व्योम U_CALLCONV rbbiInit() {
    gEmptyString = नया UnicodeString();
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

अटल व्योम U_CALLCONV initLanguageFactories(UErrorCode& status) {
    U_ASSERT(gLanguageBreakFactories == nullptr);
    gLanguageBreakFactories = नया UStack(_deleteFactory, nullptr, status);
    अगर (gLanguageBreakFactories != nullptr && U_SUCCESS(status)) {
        LocalPointer<ICULanguageBreakFactory> factory(नया ICULanguageBreakFactory(status), status);
        अगर (U_SUCCESS(status)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, status);
#अगर_घोषित U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &status);
            अगर (extra != nullptr) {
                gLanguageBreakFactories->push(extra, status);
            }
#पूर्ण_अगर
        }
    }
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

व्योम ensureLanguageFactories(UErrorCode& status) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, status);
}

अटल स्थिर LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, स्थिर अक्षर* locale)
{
    UErrorCode status = U_ZERO_ERROR;
    ensureLanguageFactories(status);
    अगर (U_FAILURE(status)) वापस nullptr;

    int32_t i = gLanguageBreakFactories->size();
    स्थिर LanguageBreakEngine *lbe = nullptr;
    जबतक (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        अगर (lbe != nullptr) {
            अवरोध;
        }
    }
    वापस lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
स्थिर LanguageBreakEngine*
RuleBasedBreakIterator::getLanguageBreakEngine(UChar32 c, स्थिर अक्षर* locale) U_LIFETIME_BOUND {
    स्थिर LanguageBreakEngine *lbe = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    अगर (fLanguageBreakEngines == nullptr) {
        fLanguageBreakEngines = नया UStack(status);
        अगर (fLanguageBreakEngines == nullptr || U_FAILURE(status)) {
            मिटाओ fLanguageBreakEngines;
            fLanguageBreakEngines = nullptr;
            वापस nullptr;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    जबतक (--i >= 0) {
        lbe = static_cast<स्थिर LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        अगर (lbe->handles(c, locale)) {
            वापस lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    अगर (lbe != nullptr) {
        fLanguageBreakEngines->push((व्योम *)lbe, status);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        वापस lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    अगर (fUnhandledBreakEngine == nullptr) {
        fUnhandledBreakEngine = नया UnhandledEngine(status);
        अगर (U_SUCCESS(status) && fUnhandledBreakEngine == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->जोड़ो_Element_At(fUnhandledBreakEngine, 0, status);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        अगर (U_FAILURE(status)) {
            मिटाओ fUnhandledBreakEngine;
            fUnhandledBreakEngine = nullptr;
            वापस nullptr;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    वापस fUnhandledBreakEngine;
}

#अगर_अघोषित U_HIDE_DRAFT_API
व्योम U_EXPORT2 RuleBasedBreakIterator::registerExternalBreakEngine(
                  ExternalBreakEngine* toAdopt, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(toAdopt, status);
    अगर (U_FAILURE(status)) वापस;
    ensureLanguageFactories(status);
    अगर (U_FAILURE(status)) वापस;
    gICULanguageBreakFactory->addExternalEngine(engine.orphan(), status);
}
#पूर्ण_अगर  /* U_HIDE_DRAFT_API */


व्योम RuleBasedBreakIterator::dumpCache() {
    fBreakCache->dumpCache();
}

व्योम RuleBasedBreakIterator::dumpTables() {
    f_तथ्य->print_तथ्य();
}

/**
 * Returns the description used to create this iterator
 */

स्थिर UnicodeString&
RuleBasedBreakIterator::getRules() स्थिर U_LIFETIME_BOUND {
    अगर (f_तथ्य != nullptr) {
        वापस f_तथ्य->getRuleSourceString();
    } अन्यथा {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        वापस *gEmptyString;
    }
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */

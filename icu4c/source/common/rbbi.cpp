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

#অন্তর্ভাব "utypeinfo.h"  // for 'typeid' to work

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব <cinttypes>

#অন্তর্ভাব "unicode/rbbi.h"
#অন্তর্ভাব "unicode/schriter.h"
#অন্তর্ভাব "unicode/uchriter.h"
#অন্তর্ভাব "unicode/uclean.h"
#অন্তর্ভাব "unicode/udata.h"

#অন্তর্ভাব "brkeng.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "localsvc.h"
#অন্তর্ভাব "rbbidata.h"
#অন্তর্ভাব "rbbi_cache.h"
#অন্তর্ভাব "rbbirb.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uvectr32.h"

#ifdef RBBI_DEBUG
স্থির UBool gTrace = মিথ্যা;
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
RuleBasedBreakIterator::RuleBasedBreakIterator(RBBIDataHeader* তথ্য, UErrorCode &status)
 : RuleBasedBreakIterator(&status)
{
    f_তথ্য = নতুন RBBIDataWrapper(তথ্য, status); // status checked in constructor
    যদি (U_FAILURE(status)) {ফেরত;}
    যদি(f_তথ্য == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    যদি (f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার * আকার(int32_t)));
        যদি (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
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
RuleBasedBreakIterator::RuleBasedBreakIterator(ধ্রুবক uint8_t *compiledRules,
                       uint32_t       rule_দৈর্ঘ্য,
                       UErrorCode     &status)
 : RuleBasedBreakIterator(&status)
{
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (compiledRules == nullptr || rule_দৈর্ঘ্য < আকার(RBBIDataHeader)) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    ধ্রুবক RBBIDataHeader* তথ্য = reinterpret_cast<ধ্রুবক RBBIDataHeader*>(compiledRules);
    যদি (তথ্য->f_দৈর্ঘ্য > rule_দৈর্ঘ্য) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    f_তথ্য = নতুন RBBIDataWrapper(তথ্য, RBBIDataWrapper::kDontAdopt, status);
    যদি (U_FAILURE(status)) {ফেরত;}
    যদি(f_তথ্য == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    যদি (f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার * আকার(int32_t)));
        যদি (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
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
    f_তথ্য = নতুন RBBIDataWrapper(udm, status); // status checked in constructor
    যদি (U_FAILURE(status)) {ফেরত;}
    যদি(f_তথ্য == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    যদি (f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার * আকার(int32_t)));
        যদি (fLookAheadMatches == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator( ধ্রুবক UnicodeString  &rules,
                                                UParseError          &parseError,
                                                UErrorCode           &status)
 : RuleBasedBreakIterator(&status)
{
    যদি (U_FAILURE(status)) {ফেরত;}
    RuleBasedBreakIterator *bi = (RuleBasedBreakIterator *)
        RBBIRuleBuilder::createRuleBasedBreakIterator(rules, &parseError, status);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    যদি (U_SUCCESS(status)) {
        *এই = *bi;
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
    যদি (status == nullptr) {
        status = &ec;
    }
    utext_openUChars(&fText, nullptr, 0, status);
    LocalPointer<DictionaryCache> lpDictionaryCache(নতুন DictionaryCache(এই, *status), *status);
    LocalPointer<BreakCache> lpBreakCache(নতুন BreakCache(এই, *status), *status);
    যদি (U_FAILURE(*status)) {
        fErrorCode = *status;
        ফেরত;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#ifdef RBBI_DEBUG
    স্থির UBool debugInitDone = মিথ্যা;
    যদি (debugInitDone == মিথ্যা) {
        অক্ষর *debugEnv = getenv("U_RBBIDEBUG");
        যদি (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = সত্য;
        }
        debugInitDone = সত্য;
    }
#endif
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
RuleBasedBreakIterator::RuleBasedBreakIterator(ধ্রুবক RuleBasedBreakIterator& other)
: RuleBasedBreakIterator()
{
    *এই = other;
}


/**
 * Destructor
 */
RuleBasedBreakIterator::~RuleBasedBreakIterator() {
    যদি (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        delete fCharIter;
    }
    fCharIter = nullptr;

    utext_close(&fText);

    যদি (f_তথ্য != nullptr) {
        f_তথ্য->removeReference();
        f_তথ্য = nullptr;
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
RuleBasedBreakIterator::operator=(ধ্রুবক RuleBasedBreakIterator& that) U_LIFETIME_BOUND {
    যদি (এই == &that) {
        ফেরত *এই;
    }
    BreakIterator::operator=(that);

    যদি (fLanguageBreakEngines != nullptr) {
        delete fLanguageBreakEngines;
        fLanguageBreakEngines = nullptr;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UErrorCode status = U_ZERO_ERROR;
    utext_clone(&fText, &that.fText, মিথ্যা, সত্য, &status);

    যদি (fCharIter != &fSCharIter) {
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    যদি (that.fCharIter != nullptr && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    যদি (fCharIter == nullptr) {
        fCharIter = &fSCharIter;
    }

    যদি (f_তথ্য != nullptr) {
        f_তথ্য->removeReference();
        f_তথ্য = nullptr;
    }
    যদি (that.f_তথ্য != nullptr) {
        f_তথ্য = that.f_তথ্য->addReference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = nullptr;
    যদি (f_তথ্য && f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার * আকার(int32_t)));
    }


    fPosition = that.fPosition;
    f_Rule_Status_সূচক = that.f_Rule_Status_সূচক;
    fDone = that.fDone;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_Rule_Status_সূচক);
    fDictionaryCache->reset();

    ফেরত *এই;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
RuleBasedBreakIterator*
RuleBasedBreakIterator::clone() ধ্রুবক {
    ফেরত নতুন RuleBasedBreakIterator(*এই);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
যৌক্তিক
RuleBasedBreakIterator::operator==(ধ্রুবক BreakIterator& that) ধ্রুবক {
    যদি (typeid(*এই) != typeid(that)) {
        ফেরত মিথ্যা;
    }
    যদি (এই == &that) {
        ফেরত সত্য;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ধ্রুবক RuleBasedBreakIterator& that2 = static_cast<ধ্রুবক RuleBasedBreakIterator&>(that);

    যদি (!utext_equals(&fText, &that2.fText)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        ফেরত মিথ্যা;
    }

    যদি (!(fPosition == that2.fPosition &&
            f_Rule_Status_সূচক == that2.f_Rule_Status_সূচক &&
            fDone == that2.fDone &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        ফেরত মিথ্যা;
    }

    যদি (that2.f_তথ্য == f_তথ্য ||
        (f_তথ্য != nullptr && that2.f_তথ্য != nullptr && *that2.f_তথ্য == *f_তথ্য)) {
            // The two break iterators are using the same rules.
            ফেরত সত্য;
        }
    ফেরত মিথ্যা;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
RuleBasedBreakIterator::hashCode() ধ্রুবক {
    int32_t   hash = 0;
    যদি (f_তথ্য != nullptr) {
        hash = f_তথ্য->hashCode();
    }
    ফেরত hash;
}


শূন্য RuleBasedBreakIterator::setText(UText *ut, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&fText, ut, মিথ্যা, সত্য, &status);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.setText(u"", 0);

    যদি (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    এই->first();
}


UText* RuleBasedBreakIterator::getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ধ্রুবক {
    UText *ফলাফল = utext_clone(fillIn, &fText, মিথ্যা, সত্য, &status);
    ফেরত ফলাফল;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
RuleBasedBreakIterator::getText() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
শূন্য
RuleBasedBreakIterator::adoptText(CharacterIterator* newText) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    যদি (fCharIter != &fSCharIter) {
        delete fCharIter;
    }

    fCharIter = newText;
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    যদি (newText==nullptr || newText->start_সূচক() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&fText, nullptr, 0, &status);
    } অন্যথা {
        utext_openCharacterIterator(&fText, newText, &status);
    }
    এই->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
শূন্য
RuleBasedBreakIterator::setText(ধ্রুবক UnicodeString& newText) {
    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&fText, &newText, &status);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.setText(newText.get_বাফার(), newText.length());

    যদি (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        delete fCharIter;
    }
    fCharIter = &fSCharIter;

    এই->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
RuleBasedBreakIterator&
RuleBasedBreakIterator::refresh_প্রবেশ_Text(UText* input, UErrorCode& status) U_LIFETIME_BOUND {
    যদি (U_FAILURE(status)) {
        ফেরত *এই;
    }
    যদি (input == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত *এই;
    }
    int64_t pos = utext_get_Native_সূচক(&fText);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&fText, input, মিথ্যা, সত্য, &status);
    যদি (U_FAILURE(status)) {
        ফেরত *এই;
    }
    utext_set_Native_সূচক(&fText, pos);
    যদি (utext_get_Native_সূচক(&fText) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ফেরত *এই;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t RuleBasedBreakIterator::first() {
    UErrorCode status = U_ZERO_ERROR;
    যদি (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, status);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    ফেরত 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t RuleBasedBreakIterator::last() {
    int32_t endPos = static_cast<int32_t>(utext_native_দৈর্ঘ্য(&fText));
    UBool endShouldBeBoundary = isBoundary(endPos);      // Has side effect of setting iterator position.
    (শূন্য)endShouldBeBoundary;
    U_ASSERT(endShouldBeBoundary);
    U_ASSERT(fPosition == endPos);
    ফেরত endPos;
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
    int32_t ফলাফল = 0;
    যদি (n > 0) {
        জন্য (; n > 0 && ফলাফল != UBRK_DONE; --n) {
            ফলাফল = next();
        }
    } অন্যথা যদি (n < 0) {
        জন্য (; n < 0 && ফলাফল != UBRK_DONE; ++n) {
            ফলাফল = previous();
        }
    } অন্যথা {
        ফলাফল = current();
    }
    ফেরত ফলাফল;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t RuleBasedBreakIterator::next() {
    fBreakCache->next();
    ফেরত fDone ? UBRK_DONE : fPosition;
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
    ফেরত fDone ? UBRK_DONE : fPosition;
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
    যদি (startPos < 0) {
        ফেরত first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_set_Native_সূচক(&fText, startPos);
    startPos = static_cast<int32_t>(utext_get_Native_সূচক(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->following(startPos, status);
    ফেরত fDone ? UBRK_DONE : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t RuleBasedBreakIterator::preceding(int32_t offset) {
    যদি (offset > utext_native_দৈর্ঘ্য(&fText)) {
        ফেরত last();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_set_Native_সূচক(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_get_Native_সূচক(&fText));

    UErrorCode status = U_ZERO_ERROR;
    fBreakCache->preceding(adjustedOffset, status);
    ফেরত fDone ? UBRK_DONE : fPosition;
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
    যদি (offset < 0) {
        first();       // For side effects on current position, tag values.
        ফেরত মিথ্যা;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_set_Native_সূচক(&fText, offset);
    int32_t adjustedOffset = static_cast<int32_t>(utext_get_Native_সূচক(&fText));

    যৌক্তিক ফলাফল = মিথ্যা;
    UErrorCode status = U_ZERO_ERROR;
    যদি (fBreakCache->seek(adjustedOffset) || fBreakCache->populateNear(adjustedOffset, status)) {
        ফলাফল = (fBreakCache->current() == offset);
    }

    যদি (ফলাফল && adjustedOffset < offset && utext_char32At(&fText, offset) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        ফেরত মিথ্যা;
    }
    যদি (!ফলাফল) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    ফেরত ফলাফল;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t RuleBasedBreakIterator::current() ধ্রুবক {
    ফেরত fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
গণনা RBBIRunMode {
    RBBI_START,     // state machine processing is before first char of input
    RBBI_RUN,       // state machine processing is in the user text
    RBBI_END        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
স্থির অন্তঃরেখ uint16_t TrieFunc8(ধ্রুবক UCPTrie *trie, UChar32 c) {
    ফেরত UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

স্থির অন্তঃরেখ uint16_t TrieFunc16(ধ্রুবক UCPTrie *trie, UChar32 c) {
    ফেরত UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t RuleBasedBreakIterator::handle_পরবর্তী() {
    ধ্রুবক RBBIStateTable *statetable = f_তথ্য->fForwardTable;
    যৌক্তিক use8BitsTrie = ucptrie_get_মান_Width(f_তথ্য->fTrie) == UCPTRIE_মান_BITS_8;
    যদি (statetable->fFlags & RBBI_8BITS_ROWS) {
        যদি (use8BitsTrie) {
            ফেরত handle_পরবর্তী<RBBIStateTableRow8, TrieFunc8>();
        } অন্যথা {
            ফেরত handle_পরবর্তী<RBBIStateTableRow8, TrieFunc16>();
        }
    } অন্যথা {
        যদি (use8BitsTrie) {
            ফেরত handle_পরবর্তী<RBBIStateTableRow16, TrieFunc8>();
        } অন্যথা {
            ফেরত handle_পরবর্তী<RBBIStateTableRow16, TrieFunc16>();
        }
    }
}

int32_t RuleBasedBreakIterator::handleSafePrevious(int32_t fromPosition) {
    ধ্রুবক RBBIStateTable *statetable = f_তথ্য->fReverseTable;
    যৌক্তিক use8BitsTrie = ucptrie_get_মান_Width(f_তথ্য->fTrie) == UCPTRIE_মান_BITS_8;
    যদি (statetable->fFlags & RBBI_8BITS_ROWS) {
        যদি (use8BitsTrie) {
            ফেরত handleSafePrevious<RBBIStateTableRow8, TrieFunc8>(fromPosition);
        } অন্যথা {
            ফেরত handleSafePrevious<RBBIStateTableRow8, TrieFunc16>(fromPosition);
        }
    } অন্যথা {
        যদি (use8BitsTrie) {
            ফেরত handleSafePrevious<RBBIStateTableRow16, TrieFunc8>(fromPosition);
        } অন্যথা {
            ফেরত handleSafePrevious<RBBIStateTableRow16, TrieFunc16>(fromPosition);
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
int32_t RuleBasedBreakIterator::handle_পরবর্তী() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRunMode         mode;

    RowType             *row;
    UChar32             c;
    int32_t             ফলাফল             = 0;
    int32_t             initialPosition    = 0;
    ধ্রুবক RBBIStateTable *statetable       = f_তথ্য->fForwardTable;
    ধ্রুবক অক্ষর         *table_তথ্য          = statetable->f_Table_তথ্য;
    uint32_t            tableRowLen        = statetable->fRowLen;
    uint32_t            dictStart          = statetable->fDictCategoriesStart;
    #ifdef RBBI_DEBUG
        যদি (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #endif

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_Rule_Status_সূচক = 0;

    f_Dictionary_Char_গুনতি = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&fText, initialPosition);
    ফলাফল          = initialPosition;
    c               = UTEXT_NEXT32(&fText);
    যদি (c==U_SENTINEL) {
        fDone = সত্য;
        ফেরত UBRK_DONE;
    }

    //  Set the initial state for the state machine
    state = START_STATE;
    row = (RowType *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (table_তথ্য + tableRowLen * state);


    mode     = RBBI_RUN;


    // loop until we reach the end of the text or transition to state 0
    //
    জন্য (;;) {
        যদি (c == U_SENTINEL) {
            // Reached end of input string.
            যদি (mode == RBBI_END) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                বিরতি;
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
        যদি (mode == RBBI_RUN) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_তথ্য->fTrie, c);
            f_Dictionary_Char_গুনতি += (category >= dictStart);
        }

       #ifdef RBBI_DEBUG
            যদি (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_সূচক(&fText));
                যদি (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } অন্যথা {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_তথ্য->fHeader->f_Cat_গুনতি);
        state = row->f_পরবর্তী_State[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (table_তথ্য + tableRowLen * state);


        uint16_t accepting = row->fAccepting;
        যদি (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            যদি (mode != RBBI_START) {
                ফলাফল = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            }
            f_Rule_Status_সূচক = row->fTagsIdx;   // Remember the break status (tag) values.
        } অন্যথা যদি (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার);
            int32_t lookahead_ফলাফল = fLookAheadMatches[accepting];
            যদি (lookahead_ফলাফল >= 0) {
                f_Rule_Status_সূচক = row->fTagsIdx;
                fPosition = lookahead_ফলাফল;
                ফেরত lookahead_ফলাফল;
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
        U_ASSERT(rule == 0 || rule < f_তথ্য->fForwardTable->f_Look_Ahead_Results_আকার);
        যদি (rule > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            fLookAheadMatches[rule] = pos;
        }

        যদি (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            বিরতি;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        যদি (mode == RBBI_RUN) {
            c = UTEXT_NEXT32(&fText);
        } অন্যথা {
            যদি (mode == RBBI_START) {
                mode = RBBI_RUN;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    যদি (ফলাফল == initialPosition) {
        utext_set_Native_সূচক(&fText, initialPosition);
        utext_next32(&fText);
        ফলাফল = static_cast<int32_t>(utext_get_Native_সূচক(&fText));
        f_Rule_Status_সূচক = 0;
    }

    // Leave the iterator at our result position.
    fPosition = ফলাফল;
    #ifdef RBBI_DEBUG
        যদি (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", ফলাফল);
        }
    #endif
    ফেরত ফলাফল;
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
    int32_t             ফলাফল          = 0;

    ধ্রুবক RBBIStateTable *stateTable = f_তথ্য->fReverseTable;
    UTEXT_SETNATIVEINDEX(&fText, fromPosition);
    #ifdef RBBI_DEBUG
        যদি (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #endif

    // if we're already at the start of the text, return DONE.
    যদি (f_তথ্য == nullptr || UTEXT_GETNATIVEINDEX(&fText)==0) {
        ফেরত BreakIterator::DONE;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&fText);
    state = START_STATE;
    row = (RowType *)
            (stateTable->f_Table_তথ্য + (stateTable->fRowLen * state));

    // loop until we reach the start of the text or transition to state 0
    //
    জন্য (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&fText)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_তথ্য->fTrie, c);

        #ifdef RBBI_DEBUG
            যদি (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_সূচক(&fText));
                যদি (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } অন্যথা {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #endif

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_তথ্য->fHeader->f_Cat_গুনতি);
        state = row->f_পরবর্তী_State[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            (stateTable->f_Table_তথ্য + (stateTable->fRowLen * state));

        যদি (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            বিরতি;
        }
    }

    // The state machine is done.  Check whether it found a match...
    ফলাফল = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
    #ifdef RBBI_DEBUG
        যদি (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", ফলাফল);
        }
    #endif
    ফেরত ফলাফল;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  RuleBasedBreakIterator::getRuleStatus() ধ্রুবক {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_Rule_Status_সূচক + f_তথ্য->fRuleStatusTable[f_Rule_Status_সূচক];
    int32_t  tagVal = f_তথ্য->fRuleStatusTable[idx];

    ফেরত tagVal;
}


int32_t RuleBasedBreakIterator::getRuleStatusVec(
             int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত 0;
    }

    int32_t  অঙ্ক_Vals = f_তথ্য->fRuleStatusTable[f_Rule_Status_সূচক];
    int32_t  অঙ্ক_Vals_To_Copy = অঙ্ক_Vals;
    যদি (অঙ্ক_Vals > capacity) {
        status = U_বাফার_OVERFLOW_ERROR;
        অঙ্ক_Vals_To_Copy = capacity;
    }
    পূর্ণসংখ্যা i;
    জন্য (i=0; i<অঙ্ক_Vals_To_Copy; i++) {
        fillInVec[i] = f_তথ্য->fRuleStatusTable[f_Rule_Status_সূচক + i + 1];
    }
    ফেরত অঙ্ক_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ধ্রুবক uint8_t* RuleBasedBreakIterator::get_দ্বিমিক_Rules(uint32_t& length) U_LIFETIME_BOUND {
    ধ্রুবক uint8_t  *retPtr = nullptr;
    length = 0;

    যদি (f_তথ্য != nullptr) {
        retPtr = reinterpret_cast<ধ্রুবক uint8_t*>(f_তথ্য->fHeader);
        length = f_তথ্য->fHeader->f_দৈর্ঘ্য;
    }
    ফেরত retPtr;
}


RuleBasedBreakIterator *RuleBasedBreakIterator::create_বাফার_Clone(
        শূন্য * /*stackBuffer*/, int32_t &বাফার_আকার, UErrorCode &status) {
    যদি (U_FAILURE(status)){
        ফেরত nullptr;
    }

    যদি (বাফার_আকার == 0) {
        বাফার_আকার = 1;  // preflighting for deprecated functionality
        ফেরত nullptr;
    }

    BreakIterator *clonedBI = clone();
    যদি (clonedBI == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা {
        status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ফেরত (RuleBasedBreakIterator *)clonedBI;
}

U_NAMESPACE_END


স্থির icu::UStack *gLanguageBreakFactories = nullptr;
স্থির ধ্রুবক icu::UnicodeString *gEmptyString = nullptr;
স্থির icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
স্থির icu::UInitOnce gRBBIInitOnce {};
স্থির icu::ICULanguageBreakFactory *gICULanguageBreakFactory = nullptr;

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
    ফেরত সত্য;
}
U_CDECL_END

U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV _deleteFactory(শূন্য *obj) {
    delete (icu::LanguageBreakFactory *) obj;
}
U_CDECL_END
U_NAMESPACE_BEGIN

স্থির শূন্য U_CALLCONV rbbiInit() {
    gEmptyString = নতুন UnicodeString();
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

স্থির শূন্য U_CALLCONV initLanguageFactories(UErrorCode& status) {
    U_ASSERT(gLanguageBreakFactories == nullptr);
    gLanguageBreakFactories = নতুন UStack(_deleteFactory, nullptr, status);
    যদি (gLanguageBreakFactories != nullptr && U_SUCCESS(status)) {
        LocalPointer<ICULanguageBreakFactory> factory(নতুন ICULanguageBreakFactory(status), status);
        যদি (U_SUCCESS(status)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, status);
#ifdef U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &status);
            যদি (extra != nullptr) {
                gLanguageBreakFactories->push(extra, status);
            }
#endif
        }
    }
    ucln_common_registerCleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

শূন্য ensureLanguageFactories(UErrorCode& status) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, status);
}

স্থির ধ্রুবক LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ধ্রুবক অক্ষর* locale)
{
    UErrorCode status = U_ZERO_ERROR;
    ensureLanguageFactories(status);
    যদি (U_FAILURE(status)) ফেরত nullptr;

    int32_t i = gLanguageBreakFactories->size();
    ধ্রুবক LanguageBreakEngine *lbe = nullptr;
    যখন (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        যদি (lbe != nullptr) {
            বিরতি;
        }
    }
    ফেরত lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
ধ্রুবক LanguageBreakEngine*
RuleBasedBreakIterator::getLanguageBreakEngine(UChar32 c, ধ্রুবক অক্ষর* locale) U_LIFETIME_BOUND {
    ধ্রুবক LanguageBreakEngine *lbe = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    যদি (fLanguageBreakEngines == nullptr) {
        fLanguageBreakEngines = নতুন UStack(status);
        যদি (fLanguageBreakEngines == nullptr || U_FAILURE(status)) {
            delete fLanguageBreakEngines;
            fLanguageBreakEngines = nullptr;
            ফেরত nullptr;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    যখন (--i >= 0) {
        lbe = static_cast<ধ্রুবক LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        যদি (lbe->handles(c, locale)) {
            ফেরত lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    যদি (lbe != nullptr) {
        fLanguageBreakEngines->push((শূন্য *)lbe, status);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        ফেরত lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    যদি (fUnhandledBreakEngine == nullptr) {
        fUnhandledBreakEngine = নতুন UnhandledEngine(status);
        যদি (U_SUCCESS(status) && fUnhandledBreakEngine == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->ঢোকাও_Element_At(fUnhandledBreakEngine, 0, status);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        যদি (U_FAILURE(status)) {
            delete fUnhandledBreakEngine;
            fUnhandledBreakEngine = nullptr;
            ফেরত nullptr;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    ফেরত fUnhandledBreakEngine;
}

#ifndef U_HIDE_DRAFT_API
শূন্য U_EXPORT2 RuleBasedBreakIterator::registerExternalBreakEngine(
                  ExternalBreakEngine* toAdopt, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(toAdopt, status);
    যদি (U_FAILURE(status)) ফেরত;
    ensureLanguageFactories(status);
    যদি (U_FAILURE(status)) ফেরত;
    gICULanguageBreakFactory->addExternalEngine(engine.orphan(), status);
}
#endif  /* U_HIDE_DRAFT_API */


শূন্য RuleBasedBreakIterator::dumpCache() {
    fBreakCache->dumpCache();
}

শূন্য RuleBasedBreakIterator::dumpTables() {
    f_তথ্য->print_তথ্য();
}

/**
 * Returns the description used to create this iterator
 */

ধ্রুবক UnicodeString&
RuleBasedBreakIterator::getRules() ধ্রুবক U_LIFETIME_BOUND {
    যদি (f_তথ্য != nullptr) {
        ফেরত f_তথ্য->getRuleSourceString();
    } অন্যথা {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        ফেরত *gEmptyString;
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

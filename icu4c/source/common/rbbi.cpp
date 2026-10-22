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

#إذا !UCONFIG_NO_BREAK_ITERATION

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

#جے_تعریف_شدہ RBBI_DEBUG
ساكن UBool gTrace = خطأ;
#جے_انت

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t شروع_STATE = 1;

// The state-transition value indicating "stop"
constexpr int32_t STOP_STATE = 0;


UOBJECT_DEFINE_RTTI_IMPLEMENTATION(قاعدہ_Based_Break_Iterator)


//=======================================================================
// constructors
//=======================================================================

/**
 * Constructs a RuleBasedBreakIterator that uses the already-created
 * tables object that is passed in as a parameter.
 */
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(RBBIData_سرخی* انکڑے, UError_کوڈ &حالت)
 : قاعدہ_Based_Break_Iterator(&حالت)
{
    f_انکڑے = جديد RBBIDataWrapper(انکڑے, حالت); // status checked in constructor
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_انکڑے == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    إذا (f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ * حجم(int32_t)));
        إذا (fLookAheadMatches == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
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
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(UData_حافظہ* udm, UBool isPhraseBreaking,
        UError_کوڈ &حالت) : قاعدہ_Based_Break_Iterator(udm, حالت)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(ثابت uint8_t *compiled_قاعدے,
                       uint32_t       قاعدہ_لمائی,
                       UError_کوڈ     &حالت)
 : قاعدہ_Based_Break_Iterator(&حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (compiled_قاعدے == خالی_سوچک || قاعدہ_لمائی < حجم(RBBIData_سرخی)) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ثابت RBBIData_سرخی* انکڑے = reinterpret_cast<ثابت RBBIData_سرخی*>(compiled_قاعدے);
    إذا (انکڑے->f_لمائی > قاعدہ_لمائی) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    f_انکڑے = جديد RBBIDataWrapper(انکڑے, RBBIDataWrapper::kDontAdopt, حالت);
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_انکڑے == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    إذا (f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ * حجم(int32_t)));
        إذا (fLookAheadMatches == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
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
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(UData_حافظہ* udm, UError_کوڈ &حالت)
 : قاعدہ_Based_Break_Iterator(&حالت)
{
    f_انکڑے = جديد RBBIDataWrapper(udm, حالت); // status checked in constructor
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_انکڑے == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    إذا (f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ * حجم(int32_t)));
        إذا (fLookAheadMatches == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator( ثابت UnicodeString  &قاعدے,
                                                UParseError          &parseError,
                                                UError_کوڈ           &حالت)
 : قاعدہ_Based_Break_Iterator(&حالت)
{
    إذا (U_FAILURE(حالت)) {أرجع;}
    قاعدہ_Based_Break_Iterator *bi = (قاعدہ_Based_Break_Iterator *)
        RBBIRuleBuilder::create_قاعدہ_Based_Break_Iterator(قاعدے, &parseError, حالت);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    إذا (U_SUCCESS(حالت)) {
        *هذا = *bi;
        مٹاؤ bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator()
 : قاعدہ_Based_Break_Iterator(خالی_سوچک)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(UError_کوڈ *حالت) {
    UError_کوڈ ec = U_ZERO_ERROR;
    إذا (حالت == خالی_سوچک) {
        حالت = &ec;
    }
    utext_openUChars(&f_متن, خالی_سوچک, 0, حالت);
    Local_اشارہ_گر<DictionaryCache> lpDictionaryCache(جديد DictionaryCache(هذا, *حالت), *حالت);
    Local_اشارہ_گر<BreakCache> lpBreakCache(جديد BreakCache(هذا, *حالت), *حالت);
    إذا (U_FAILURE(*حالت)) {
        f_Error_کوڈ = *حالت;
        أرجع;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#جے_تعریف_شدہ RBBI_DEBUG
    ساكن UBool debug_Init_پورا = خطأ;
    إذا (debug_Init_پورا == خطأ) {
        حرف *debugEnv = getenv("U_RBBIDEBUG");
        إذا (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = سچ;
        }
        debug_Init_پورا = سچ;
    }
#جے_انت
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(ثابت قاعدہ_Based_Break_Iterator& other)
: قاعدہ_Based_Break_Iterator()
{
    *هذا = other;
}


/**
 * Destructor
 */
قاعدہ_Based_Break_Iterator::~قاعدہ_Based_Break_Iterator() {
    إذا (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        مٹاؤ fCharIter;
    }
    fCharIter = خالی_سوچک;

    utext_close(&f_متن);

    إذا (f_انکڑے != خالی_سوچک) {
        f_انکڑے->removeReference();
        f_انکڑے = خالی_سوچک;
    }
    مٹاؤ fBreakCache;
    fBreakCache = خالی_سوچک;

    مٹاؤ fDictionaryCache;
    fDictionaryCache = خالی_سوچک;

    مٹاؤ fLanguageBreakEngines;
    fLanguageBreakEngines = خالی_سوچک;

    مٹاؤ fUnhandledBreakEngine;
    fUnhandledBreakEngine = خالی_سوچک;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = خالی_سوچک;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
قاعدہ_Based_Break_Iterator&
قاعدہ_Based_Break_Iterator::عامل=(ثابت قاعدہ_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    إذا (هذا == &that) {
        أرجع *هذا;
    }
    BreakIterator::عامل=(that);

    إذا (fLanguageBreakEngines != خالی_سوچک) {
        مٹاؤ fLanguageBreakEngines;
        fLanguageBreakEngines = خالی_سوچک;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_کوڈ حالت = U_ZERO_ERROR;
    utext_clone(&f_متن, &that.f_متن, خطأ, سچ, &حالت);

    إذا (fCharIter != &fSCharIter) {
        مٹاؤ fCharIter;
    }
    fCharIter = &fSCharIter;

    إذا (that.fCharIter != خالی_سوچک && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    إذا (fCharIter == خالی_سوچک) {
        fCharIter = &fSCharIter;
    }

    إذا (f_انکڑے != خالی_سوچک) {
        f_انکڑے->removeReference();
        f_انکڑے = خالی_سوچک;
    }
    إذا (that.f_انکڑے != خالی_سوچک) {
        f_انکڑے = that.f_انکڑے->جوڑو_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = خالی_سوچک;
    إذا (f_انکڑے && f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ * حجم(int32_t)));
    }


    fPosition = that.fPosition;
    f_قاعدہ_حالت_اشاریہ = that.f_قاعدہ_حالت_اشاریہ;
    f_پورا = that.f_پورا;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_قاعدہ_حالت_اشاریہ);
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
قاعدہ_Based_Break_Iterator*
قاعدہ_Based_Break_Iterator::clone() ثابت {
    أرجع جديد قاعدہ_Based_Break_Iterator(*هذا);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
منطقي
قاعدہ_Based_Break_Iterator::عامل==(ثابت BreakIterator& that) ثابت {
    إذا (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }
    إذا (هذا == &that) {
        أرجع سچ;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ثابت قاعدہ_Based_Break_Iterator& that2 = static_cast<ثابت قاعدہ_Based_Break_Iterator&>(that);

    إذا (!utext_equals(&f_متن, &that2.f_متن)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        أرجع خطأ;
    }

    إذا (!(fPosition == that2.fPosition &&
            f_قاعدہ_حالت_اشاریہ == that2.f_قاعدہ_حالت_اشاریہ &&
            f_پورا == that2.f_پورا &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        أرجع خطأ;
    }

    إذا (that2.f_انکڑے == f_انکڑے ||
        (f_انکڑے != خالی_سوچک && that2.f_انکڑے != خالی_سوچک && *that2.f_انکڑے == *f_انکڑے)) {
            // The two break iterators are using the same rules.
            أرجع سچ;
        }
    أرجع خطأ;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
قاعدہ_Based_Break_Iterator::hash_کوڈ() ثابت {
    int32_t   hash = 0;
    إذا (f_انکڑے != خالی_سوچک) {
        hash = f_انکڑے->hash_کوڈ();
    }
    أرجع hash;
}


فراغ قاعدہ_Based_Break_Iterator::سیٹ_متن(UText *ut, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&f_متن, ut, خطأ, سچ, &حالت);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.سیٹ_متن(u"", 0);

    إذا (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        مٹاؤ fCharIter;
    }
    fCharIter = &fSCharIter;

    هذا->first();
}


UText* قاعدہ_Based_Break_Iterator::getUText(UText* fill_وچ U_LIFETIME_BOUND, UError_کوڈ& حالت) ثابت {
    UText *نتیجہ = utext_clone(fill_وچ, &f_متن, خطأ, سچ, &حالت);
    أرجع نتیجہ;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
قاعدہ_Based_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    أرجع *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
فراغ
قاعدہ_Based_Break_Iterator::adopt_متن(CharacterIterator* new_متن) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    إذا (fCharIter != &fSCharIter) {
        مٹاؤ fCharIter;
    }

    fCharIter = new_متن;
    UError_کوڈ حالت = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    إذا (new_متن==خالی_سوچک || new_متن->شروع_اشاریہ() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&f_متن, خالی_سوچک, 0, &حالت);
    } وإلا {
        utext_openCharacterIterator(&f_متن, new_متن, &حالت);
    }
    هذا->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
فراغ
قاعدہ_Based_Break_Iterator::سیٹ_متن(ثابت UnicodeString& new_متن) {
    UError_کوڈ حالت = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&f_متن, &new_متن, &حالت);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.سیٹ_متن(new_متن.get_بفر(), new_متن.length());

    إذا (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        مٹاؤ fCharIter;
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
قاعدہ_Based_Break_Iterator&
قاعدہ_Based_Break_Iterator::refresh_داخلہ_متن(UText* input, UError_کوڈ& حالت) U_LIFETIME_BOUND {
    إذا (U_FAILURE(حالت)) {
        أرجع *هذا;
    }
    إذا (input == خالی_سوچک) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    int64_t pos = utext_get_Native_اشاریہ(&f_متن);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&f_متن, input, خطأ, سچ, &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع *هذا;
    }
    utext_سیٹ_Native_اشاریہ(&f_متن, pos);
    إذا (utext_get_Native_اشاریہ(&f_متن) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع *هذا;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t قاعدہ_Based_Break_Iterator::first() {
    UError_کوڈ حالت = U_ZERO_ERROR;
    إذا (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, حالت);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    أرجع 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t قاعدہ_Based_Break_Iterator::آخری() {
    int32_t سرا_Pos = static_cast<int32_t>(utext_native_لمائی(&f_متن));
    UBool سرا_Should_Be_Boundary = isBoundary(سرا_Pos);      // Has side effect of setting iterator position.
    (فراغ)سرا_Should_Be_Boundary;
    U_ASSERT(سرا_Should_Be_Boundary);
    U_ASSERT(fPosition == سرا_Pos);
    أرجع سرا_Pos;
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
int32_t قاعدہ_Based_Break_Iterator::next(int32_t n) {
    int32_t نتیجہ = 0;
    إذا (n > 0) {
        لكل (; n > 0 && نتیجہ != UBRK_پورا; --n) {
            نتیجہ = next();
        }
    } وإلا إذا (n < 0) {
        لكل (; n < 0 && نتیجہ != UBRK_پورا; ++n) {
            نتیجہ = پچھلا();
        }
    } وإلا {
        نتیجہ = current();
    }
    أرجع نتیجہ;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t قاعدہ_Based_Break_Iterator::next() {
    fBreakCache->next();
    أرجع f_پورا ? UBRK_پورا : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t قاعدہ_Based_Break_Iterator::پچھلا() {
    UError_کوڈ حالت = U_ZERO_ERROR;
    fBreakCache->پچھلا(حالت);
    أرجع f_پورا ? UBRK_پورا : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t قاعدہ_Based_Break_Iterator::following(int32_t شروع_Pos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    إذا (شروع_Pos < 0) {
        أرجع first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_سیٹ_Native_اشاریہ(&f_متن, شروع_Pos);
    شروع_Pos = static_cast<int32_t>(utext_get_Native_اشاریہ(&f_متن));

    UError_کوڈ حالت = U_ZERO_ERROR;
    fBreakCache->following(شروع_Pos, حالت);
    أرجع f_پورا ? UBRK_پورا : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t قاعدہ_Based_Break_Iterator::preceding(int32_t وتھ) {
    إذا (وتھ > utext_native_لمائی(&f_متن)) {
        أرجع آخری();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_سیٹ_Native_اشاریہ(&f_متن, وتھ);
    int32_t adjusted_وتھ = static_cast<int32_t>(utext_get_Native_اشاریہ(&f_متن));

    UError_کوڈ حالت = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_وتھ, حالت);
    أرجع f_پورا ? UBRK_پورا : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool قاعدہ_Based_Break_Iterator::isBoundary(int32_t وتھ) {
    // out-of-range indexes are never boundary positions
    إذا (وتھ < 0) {
        first();       // For side effects on current position, tag values.
        أرجع خطأ;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_سیٹ_Native_اشاریہ(&f_متن, وتھ);
    int32_t adjusted_وتھ = static_cast<int32_t>(utext_get_Native_اشاریہ(&f_متن));

    منطقي نتیجہ = خطأ;
    UError_کوڈ حالت = U_ZERO_ERROR;
    إذا (fBreakCache->seek(adjusted_وتھ) || fBreakCache->populateNear(adjusted_وتھ, حالت)) {
        نتیجہ = (fBreakCache->current() == وتھ);
    }

    إذا (نتیجہ && adjusted_وتھ < وتھ && utext_char32At(&f_متن, وتھ) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        أرجع خطأ;
    }
    إذا (!نتیجہ) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    أرجع نتیجہ;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t قاعدہ_Based_Break_Iterator::current() ثابت {
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
تعداد RBBIRun_منوال {
    RBBI_شروع,     // state machine processing is before first char of input
    RBBI_لگاتار,       // state machine processing is in the user text
    RBBI_سرا        // state machine processing is after end of user text.
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

int32_t قاعدہ_Based_Break_Iterator::handle_اگلا() {
    ثابت RBBIState_جدول *statetable = f_انکڑے->f_Forward_جدول;
    منطقي use8_بٹ_Trie = ucptrie_get_قدر_چوڑائی(f_انکڑے->fTrie) == UCPTRIE_قدر_بٹ_8;
    إذا (statetable->fFlags & RBBI_8_بٹ_صفاں) {
        إذا (use8_بٹ_Trie) {
            أرجع handle_اگلا<RBBIState_جدول_Row8, TrieFunc8>();
        } وإلا {
            أرجع handle_اگلا<RBBIState_جدول_Row8, TrieFunc16>();
        }
    } وإلا {
        إذا (use8_بٹ_Trie) {
            أرجع handle_اگلا<RBBIState_جدول_Row16, TrieFunc8>();
        } وإلا {
            أرجع handle_اگلا<RBBIState_جدول_Row16, TrieFunc16>();
        }
    }
}

int32_t قاعدہ_Based_Break_Iterator::handle_محفوظ_پچھلا(int32_t fromPosition) {
    ثابت RBBIState_جدول *statetable = f_انکڑے->f_الٹ_جدول;
    منطقي use8_بٹ_Trie = ucptrie_get_قدر_چوڑائی(f_انکڑے->fTrie) == UCPTRIE_قدر_بٹ_8;
    إذا (statetable->fFlags & RBBI_8_بٹ_صفاں) {
        إذا (use8_بٹ_Trie) {
            أرجع handle_محفوظ_پچھلا<RBBIState_جدول_Row8, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handle_محفوظ_پچھلا<RBBIState_جدول_Row8, TrieFunc16>(fromPosition);
        }
    } وإلا {
        إذا (use8_بٹ_Trie) {
            أرجع handle_محفوظ_پچھلا<RBBIState_جدول_Row16, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handle_محفوظ_پچھلا<RBBIState_جدول_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
سانچا <قسم_ناں صف_Type, قاعدہ_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعدہ_Based_Break_Iterator::handle_اگلا() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_منوال         منوال;

    صف_Type             *صف;
    UChar32             c;
    int32_t             نتیجہ             = 0;
    int32_t             initialPosition    = 0;
    ثابت RBBIState_جدول *statetable       = f_انکڑے->f_Forward_جدول;
    ثابت حرف         *جدول_انکڑے          = statetable->f_جدول_انکڑے;
    uint32_t            جدول_صف_Len        = statetable->f_صف_Len;
    uint32_t            dict_شروع          = statetable->f_Dict_Categories_شروع;
    #جے_تعریف_شدہ RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #جے_انت

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_قاعدہ_حالت_اشاریہ = 0;

    f_Dictionary_Char_گنتی = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&f_متن, initialPosition);
    نتیجہ          = initialPosition;
    c               = UTEXT_NEXT32(&f_متن);
    إذا (c==U_SENTINEL) {
        f_پورا = سچ;
        أرجع UBRK_پورا;
    }

    //  Set the initial state for the state machine
    state = شروع_STATE;
    صف = (صف_Type *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (جدول_انکڑے + جدول_صف_Len * state);


    منوال     = RBBI_لگاتار;


    // loop until we reach the end of the text or transition to state 0
    //
    لكل (;;) {
        إذا (c == U_SENTINEL) {
            // Reached end of input string.
            إذا (منوال == RBBI_سرا) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                اكسر;
            }
            // Run the loop one last time with the fake end-of-input character category.
            منوال = RBBI_سرا;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        إذا (منوال == RBBI_لگاتار) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_انکڑے->fTrie, c);
            f_Dictionary_Char_گنتی += (category >= dict_شروع);
        }

       #جے_تعریف_شدہ RBBI_DEBUG
            إذا (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_اشاریہ(&f_متن));
                إذا (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #جے_انت

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_انکڑے->f_سرخی->f_Cat_گنتی);
        state = صف->f_اگلا_State[category];  /*Not accessing beyond memory*/
        صف = (صف_Type *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (جدول_انکڑے + جدول_صف_Len * state);


        uint16_t accepting = صف->fAccepting;
        إذا (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            إذا (منوال != RBBI_شروع) {
                نتیجہ = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            }
            f_قاعدہ_حالت_اشاریہ = صف->f_ٹیگ_سوچی_Idx;   // Remember the break status (tag) values.
        } وإلا إذا (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ);
            int32_t lookahead_نتیجہ = fLookAheadMatches[accepting];
            إذا (lookahead_نتیجہ >= 0) {
                f_قاعدہ_حالت_اشاریہ = صف->f_ٹیگ_سوچی_Idx;
                fPosition = lookahead_نتیجہ;
                أرجع lookahead_نتیجہ;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t قاعدہ = صف->fLookAhead;
        U_ASSERT(قاعدہ == 0 || قاعدہ > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(قاعدہ == 0 || قاعدہ < f_انکڑے->f_Forward_جدول->f_Look_Ahead_Results_ناپ);
        إذا (قاعدہ > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            fLookAheadMatches[قاعدہ] = pos;
        }

        إذا (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            اكسر;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        إذا (منوال == RBBI_لگاتار) {
            c = UTEXT_NEXT32(&f_متن);
        } وإلا {
            إذا (منوال == RBBI_شروع) {
                منوال = RBBI_لگاتار;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    إذا (نتیجہ == initialPosition) {
        utext_سیٹ_Native_اشاریہ(&f_متن, initialPosition);
        utext_next32(&f_متن);
        نتیجہ = static_cast<int32_t>(utext_get_Native_اشاریہ(&f_متن));
        f_قاعدہ_حالت_اشاریہ = 0;
    }

    // Leave the iterator at our result position.
    fPosition = نتیجہ;
    #جے_تعریف_شدہ RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", نتیجہ);
        }
    #جے_انت
    أرجع نتیجہ;
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
سانچا <قسم_ناں صف_Type, قاعدہ_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعدہ_Based_Break_Iterator::handle_محفوظ_پچھلا(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    صف_Type            *صف;
    UChar32             c;
    int32_t             نتیجہ          = 0;

    ثابت RBBIState_جدول *state_جدول = f_انکڑے->f_الٹ_جدول;
    UTEXT_SETNATIVEINDEX(&f_متن, fromPosition);
    #جے_تعریف_شدہ RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #جے_انت

    // if we're already at the start of the text, return DONE.
    إذا (f_انکڑے == خالی_سوچک || UTEXT_GETNATIVEINDEX(&f_متن)==0) {
        أرجع BreakIterator::پورا_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&f_متن);
    state = شروع_STATE;
    صف = (صف_Type *)
            (state_جدول->f_جدول_انکڑے + (state_جدول->f_صف_Len * state));

    // loop until we reach the start of the text or transition to state 0
    //
    لكل (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&f_متن)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_انکڑے->fTrie, c);

        #جے_تعریف_شدہ RBBI_DEBUG
            إذا (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_اشاریہ(&f_متن));
                إذا (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #جے_انت

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_انکڑے->f_سرخی->f_Cat_گنتی);
        state = صف->f_اگلا_State[category];  /*Not accessing beyond memory*/
        صف = (صف_Type *)
            (state_جدول->f_جدول_انکڑے + (state_جدول->f_صف_Len * state));

        إذا (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            اكسر;
        }
    }

    // The state machine is done.  Check whether it found a match...
    نتیجہ = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
    #جے_تعریف_شدہ RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", نتیجہ);
        }
    #جے_انت
    أرجع نتیجہ;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  قاعدہ_Based_Break_Iterator::get_قاعدہ_حالت() ثابت {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_قاعدہ_حالت_اشاریہ + f_انکڑے->f_قاعدہ_حالت_جدول[f_قاعدہ_حالت_اشاریہ];
    int32_t  ٹیگ_Val = f_انکڑے->f_قاعدہ_حالت_جدول[idx];

    أرجع ٹیگ_Val;
}


int32_t قاعدہ_Based_Break_Iterator::get_قاعدہ_حالت_Vec(
             int32_t *fill_وچ_Vec, int32_t capacity, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }

    int32_t  عدد_Vals = f_انکڑے->f_قاعدہ_حالت_جدول[f_قاعدہ_حالت_اشاریہ];
    int32_t  عدد_Vals_منزل_Copy = عدد_Vals;
    إذا (عدد_Vals > capacity) {
        حالت = U_بفر_حد_توں_ودھ_ERROR;
        عدد_Vals_منزل_Copy = capacity;
    }
    صحيح i;
    لكل (i=0; i<عدد_Vals_منزل_Copy; i++) {
        fill_وچ_Vec[i] = f_انکڑے->f_قاعدہ_حالت_جدول[f_قاعدہ_حالت_اشاریہ + i + 1];
    }
    أرجع عدد_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ثابت uint8_t* قاعدہ_Based_Break_Iterator::get_ثنائی_قاعدے(uint32_t& length) U_LIFETIME_BOUND {
    ثابت uint8_t  *retPtr = خالی_سوچک;
    length = 0;

    إذا (f_انکڑے != خالی_سوچک) {
        retPtr = reinterpret_cast<ثابت uint8_t*>(f_انکڑے->f_سرخی);
        length = f_انکڑے->f_سرخی->f_لمائی;
    }
    أرجع retPtr;
}


قاعدہ_Based_Break_Iterator *قاعدہ_Based_Break_Iterator::create_بفر_Clone(
        فراغ * /*stackBuffer*/, int32_t &بفر_ناپ, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)){
        أرجع خالی_سوچک;
    }

    إذا (بفر_ناپ == 0) {
        بفر_ناپ = 1;  // preflighting for deprecated functionality
        أرجع خالی_سوچک;
    }

    BreakIterator *clonedBI = clone();
    إذا (clonedBI == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
    } وإلا {
        حالت = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (قاعدہ_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_سرا


ساكن icu::UStack *gLanguageBreakFactories = خالی_سوچک;
ساكن ثابت icu::UnicodeString *g_خالی_تھاواں_String = خالی_سوچک;
ساكن icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ساكن icu::UInitOnce gRBBIInitOnce {};
ساكن icu::ICULanguageBreakFactory *gICULanguageBreakFactory = خالی_سوچک;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    مٹاؤ gLanguageBreakFactories;
    gLanguageBreakFactories = خالی_سوچک;
    مٹاؤ g_خالی_تھاواں_String;
    g_خالی_تھاواں_String = خالی_سوچک;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    أرجع سچ;
}
U_CDECL_سرا

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV _deleteFactory(فراغ *obj) {
    مٹاؤ (icu::LanguageBreakFactory *) obj;
}
U_CDECL_سرا
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV rbbiInit() {
    g_خالی_تھاواں_String = جديد UnicodeString();
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ساكن فراغ U_CALLCONV initLanguageFactories(UError_کوڈ& حالت) {
    U_ASSERT(gLanguageBreakFactories == خالی_سوچک);
    gLanguageBreakFactories = جديد UStack(_deleteFactory, خالی_سوچک, حالت);
    إذا (gLanguageBreakFactories != خالی_سوچک && U_SUCCESS(حالت)) {
        Local_اشارہ_گر<ICULanguageBreakFactory> factory(جديد ICULanguageBreakFactory(حالت), حالت);
        إذا (U_SUCCESS(حالت)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, حالت);
#جے_تعریف_شدہ U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &حالت);
            إذا (extra != خالی_سوچک) {
                gLanguageBreakFactories->push(extra, حالت);
            }
#جے_انت
        }
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

فراغ ensureLanguageFactories(UError_کوڈ& حالت) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, حالت);
}

ساكن ثابت LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ثابت حرف* locale)
{
    UError_کوڈ حالت = U_ZERO_ERROR;
    ensureLanguageFactories(حالت);
    إذا (U_FAILURE(حالت)) أرجع خالی_سوچک;

    int32_t i = gLanguageBreakFactories->size();
    ثابت LanguageBreakEngine *lbe = خالی_سوچک;
    طالما (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        إذا (lbe != خالی_سوچک) {
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
قاعدہ_Based_Break_Iterator::getLanguageBreakEngine(UChar32 c, ثابت حرف* locale) U_LIFETIME_BOUND {
    ثابت LanguageBreakEngine *lbe = خالی_سوچک;
    UError_کوڈ حالت = U_ZERO_ERROR;

    إذا (fLanguageBreakEngines == خالی_سوچک) {
        fLanguageBreakEngines = جديد UStack(حالت);
        إذا (fLanguageBreakEngines == خالی_سوچک || U_FAILURE(حالت)) {
            مٹاؤ fLanguageBreakEngines;
            fLanguageBreakEngines = خالی_سوچک;
            أرجع خالی_سوچک;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    طالما (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        إذا (lbe->handles(c, locale)) {
            أرجع lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    إذا (lbe != خالی_سوچک) {
        fLanguageBreakEngines->push((فراغ *)lbe, حالت);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        أرجع lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    إذا (fUnhandledBreakEngine == خالی_سوچک) {
        fUnhandledBreakEngine = جديد UnhandledEngine(حالت);
        إذا (U_SUCCESS(حالت) && fUnhandledBreakEngine == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->داخل_کرو_Element_At(fUnhandledBreakEngine, 0, حالت);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        إذا (U_FAILURE(حالت)) {
            مٹاؤ fUnhandledBreakEngine;
            fUnhandledBreakEngine = خالی_سوچک;
            أرجع خالی_سوچک;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    أرجع fUnhandledBreakEngine;
}

#جے_بنا_تعریف U_HIDE_DRAFT_API
فراغ U_EXPORT2 قاعدہ_Based_Break_Iterator::رجسٹر_External_Break_Engine(
                  ExternalBreakEngine* منزل_Adopt, UError_کوڈ& حالت) {
    Local_اشارہ_گر<ExternalBreakEngine> engine(منزل_Adopt, حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    ensureLanguageFactories(حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    gICULanguageBreakFactory->جوڑو_External_Engine(engine.orphan(), حالت);
}
#جے_انت  /* U_HIDE_DRAFT_API */


فراغ قاعدہ_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

فراغ قاعدہ_Based_Break_Iterator::dumpTables() {
    f_انکڑے->print_انکڑے();
}

/**
 * Returns the description used to create this iterator
 */

ثابت UnicodeString&
قاعدہ_Based_Break_Iterator::get_قاعدے() ثابت U_LIFETIME_BOUND {
    إذا (f_انکڑے != خالی_سوچک) {
        أرجع f_انکڑے->get_قاعدہ_سروت_String();
    } وإلا {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        أرجع *g_خالی_تھاواں_String;
    }
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */

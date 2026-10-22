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

#اگر_تعریف_شدہ RBBI_DEBUG
ساکن UBool gTrace = جھوٹ;
#اگر_ختم

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t آغاز_STATE = 1;

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
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(RBBIData_سرخی* ڈیٹا, UError_رمز &حالت_2)
 : قاعدہ_Based_Break_Iterator(&حالت_2)
{
    f_ڈیٹا = نیا RBBIDataWrapper(ڈیٹا, حالت_2); // status checked in constructor
    اگر (U_FAILURE(حالت_2)) {واپس;}
    اگر(f_ڈیٹا == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    اگر (f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز * حجم(int32_t)));
        اگر (fLookAheadMatches == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس;
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
        UError_رمز &حالت_2) : قاعدہ_Based_Break_Iterator(udm, حالت_2)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(ثابت uint8_t *compiled_قواعد,
                       uint32_t       قاعدہ_لمبائی,
                       UError_رمز     &حالت_2)
 : قاعدہ_Based_Break_Iterator(&حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (compiled_قواعد == خالی_اشاریہ || قاعدہ_لمبائی < حجم(RBBIData_سرخی)) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    ثابت RBBIData_سرخی* ڈیٹا = reinterpret_cast<ثابت RBBIData_سرخی*>(compiled_قواعد);
    اگر (ڈیٹا->f_لمبائی > قاعدہ_لمبائی) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    f_ڈیٹا = نیا RBBIDataWrapper(ڈیٹا, RBBIDataWrapper::kDontAdopt, حالت_2);
    اگر (U_FAILURE(حالت_2)) {واپس;}
    اگر(f_ڈیٹا == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    اگر (f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز * حجم(int32_t)));
        اگر (fLookAheadMatches == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
    }
}


//-------------------------------------------------------------------------------
//
//   Constructor   from a UDataMemory handle to precompiled break rules
//                 stored in an ICU data file.
//
//-------------------------------------------------------------------------------
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(UData_حافظہ* udm, UError_رمز &حالت_2)
 : قاعدہ_Based_Break_Iterator(&حالت_2)
{
    f_ڈیٹا = نیا RBBIDataWrapper(udm, حالت_2); // status checked in constructor
    اگر (U_FAILURE(حالت_2)) {واپس;}
    اگر(f_ڈیٹا == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    اگر (f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز * حجم(int32_t)));
        اگر (fLookAheadMatches == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator( ثابت UnicodeString  &قواعد,
                                                UParseError          &parseError,
                                                UError_رمز           &حالت_2)
 : قاعدہ_Based_Break_Iterator(&حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {واپس;}
    قاعدہ_Based_Break_Iterator *bi = (قاعدہ_Based_Break_Iterator *)
        RBBIRuleBuilder::create_قاعدہ_Based_Break_Iterator(قواعد, &parseError, حالت_2);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    اگر (U_SUCCESS(حالت_2)) {
        *یہ = *bi;
        حذف bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator()
 : قاعدہ_Based_Break_Iterator(خالی_اشاریہ)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
قاعدہ_Based_Break_Iterator::قاعدہ_Based_Break_Iterator(UError_رمز *حالت_2) {
    UError_رمز ec = U_ZERO_ERROR;
    اگر (حالت_2 == خالی_اشاریہ) {
        حالت_2 = &ec;
    }
    utext_openUChars(&f_متن, خالی_اشاریہ, 0, حالت_2);
    Local_اشارہ_گر<DictionaryCache> lpDictionaryCache(نیا DictionaryCache(یہ, *حالت_2), *حالت_2);
    Local_اشارہ_گر<BreakCache> lpBreakCache(نیا BreakCache(یہ, *حالت_2), *حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
        f_Error_رمز = *حالت_2;
        واپس;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#اگر_تعریف_شدہ RBBI_DEBUG
    ساکن UBool debug_Init_مکمل = جھوٹ;
    اگر (debug_Init_مکمل == جھوٹ) {
        حرف *debugEnv = getenv("U_RBBIDEBUG");
        اگر (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = سچ;
        }
        debug_Init_مکمل = سچ;
    }
#اگر_ختم
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
    *یہ = other;
}


/**
 * Destructor
 */
قاعدہ_Based_Break_Iterator::~قاعدہ_Based_Break_Iterator() {
    اگر (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        حذف fCharIter;
    }
    fCharIter = خالی_اشاریہ;

    utext_close(&f_متن);

    اگر (f_ڈیٹا != خالی_اشاریہ) {
        f_ڈیٹا->removeReference();
        f_ڈیٹا = خالی_اشاریہ;
    }
    حذف fBreakCache;
    fBreakCache = خالی_اشاریہ;

    حذف fDictionaryCache;
    fDictionaryCache = خالی_اشاریہ;

    حذف fLanguageBreakEngines;
    fLanguageBreakEngines = خالی_اشاریہ;

    حذف fUnhandledBreakEngine;
    fUnhandledBreakEngine = خالی_اشاریہ;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = خالی_اشاریہ;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
قاعدہ_Based_Break_Iterator&
قاعدہ_Based_Break_Iterator::عامل=(ثابت قاعدہ_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    اگر (یہ == &that) {
        واپس *یہ;
    }
    BreakIterator::عامل=(that);

    اگر (fLanguageBreakEngines != خالی_اشاریہ) {
        حذف fLanguageBreakEngines;
        fLanguageBreakEngines = خالی_اشاریہ;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_رمز حالت_2 = U_ZERO_ERROR;
    utext_clone(&f_متن, &that.f_متن, جھوٹ, سچ, &حالت_2);

    اگر (fCharIter != &fSCharIter) {
        حذف fCharIter;
    }
    fCharIter = &fSCharIter;

    اگر (that.fCharIter != خالی_اشاریہ && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    اگر (fCharIter == خالی_اشاریہ) {
        fCharIter = &fSCharIter;
    }

    اگر (f_ڈیٹا != خالی_اشاریہ) {
        f_ڈیٹا->removeReference();
        f_ڈیٹا = خالی_اشاریہ;
    }
    اگر (that.f_ڈیٹا != خالی_اشاریہ) {
        f_ڈیٹا = that.f_ڈیٹا->جوڑو_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = خالی_اشاریہ;
    اگر (f_ڈیٹا && f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز * حجم(int32_t)));
    }


    fPosition = that.fPosition;
    f_قاعدہ_حالت_اشاریہ = that.f_قاعدہ_حالت_اشاریہ;
    f_مکمل = that.f_مکمل;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_قاعدہ_حالت_اشاریہ);
    fDictionaryCache->reset();

    واپس *یہ;
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
    واپس نیا قاعدہ_Based_Break_Iterator(*یہ);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
منطقی
قاعدہ_Based_Break_Iterator::عامل==(ثابت BreakIterator& that) ثابت {
    اگر (typeid(*یہ) != typeid(that)) {
        واپس جھوٹ;
    }
    اگر (یہ == &that) {
        واپس سچ;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ثابت قاعدہ_Based_Break_Iterator& that2 = static_cast<ثابت قاعدہ_Based_Break_Iterator&>(that);

    اگر (!utext_equals(&f_متن, &that2.f_متن)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        واپس جھوٹ;
    }

    اگر (!(fPosition == that2.fPosition &&
            f_قاعدہ_حالت_اشاریہ == that2.f_قاعدہ_حالت_اشاریہ &&
            f_مکمل == that2.f_مکمل &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        واپس جھوٹ;
    }

    اگر (that2.f_ڈیٹا == f_ڈیٹا ||
        (f_ڈیٹا != خالی_اشاریہ && that2.f_ڈیٹا != خالی_اشاریہ && *that2.f_ڈیٹا == *f_ڈیٹا)) {
            // The two break iterators are using the same rules.
            واپس سچ;
        }
    واپس جھوٹ;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
قاعدہ_Based_Break_Iterator::hash_رمز() ثابت {
    int32_t   hash = 0;
    اگر (f_ڈیٹا != خالی_اشاریہ) {
        hash = f_ڈیٹا->hash_رمز();
    }
    واپس hash;
}


خالی قاعدہ_Based_Break_Iterator::سیٹ_متن(UText *ut, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&f_متن, ut, جھوٹ, سچ, &حالت_2);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.سیٹ_متن(u"", 0);

    اگر (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        حذف fCharIter;
    }
    fCharIter = &fSCharIter;

    یہ->first();
}


UText* قاعدہ_Based_Break_Iterator::getUText(UText* fill_اندر U_LIFETIME_BOUND, UError_رمز& حالت_2) ثابت {
    UText *نتیجہ = utext_clone(fill_اندر, &f_متن, جھوٹ, سچ, &حالت_2);
    واپس نتیجہ;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
قاعدہ_Based_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    واپس *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
خالی
قاعدہ_Based_Break_Iterator::adopt_متن(CharacterIterator* new_متن) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    اگر (fCharIter != &fSCharIter) {
        حذف fCharIter;
    }

    fCharIter = new_متن;
    UError_رمز حالت_2 = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    اگر (new_متن==خالی_اشاریہ || new_متن->آغاز_اشاریہ() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&f_متن, خالی_اشاریہ, 0, &حالت_2);
    } ورنہ {
        utext_openCharacterIterator(&f_متن, new_متن, &حالت_2);
    }
    یہ->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
خالی
قاعدہ_Based_Break_Iterator::سیٹ_متن(ثابت UnicodeString& new_متن) {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&f_متن, &new_متن, &حالت_2);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.سیٹ_متن(new_متن.get_بفر(), new_متن.length());

    اگر (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        حذف fCharIter;
    }
    fCharIter = &fSCharIter;

    یہ->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
قاعدہ_Based_Break_Iterator&
قاعدہ_Based_Break_Iterator::refresh_داخلہ_متن(UText* input, UError_رمز& حالت_2) U_LIFETIME_BOUND {
    اگر (U_FAILURE(حالت_2)) {
        واپس *یہ;
    }
    اگر (input == خالی_اشاریہ) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس *یہ;
    }
    int64_t pos = utext_get_Native_اشاریہ(&f_متن);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&f_متن, input, جھوٹ, سچ, &حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس *یہ;
    }
    utext_سیٹ_Native_اشاریہ(&f_متن, pos);
    اگر (utext_get_Native_اشاریہ(&f_متن) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }
    واپس *یہ;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t قاعدہ_Based_Break_Iterator::first() {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    اگر (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, حالت_2);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    واپس 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t قاعدہ_Based_Break_Iterator::آخری() {
    int32_t انتہا_Pos = static_cast<int32_t>(utext_native_لمبائی(&f_متن));
    UBool انتہا_Should_Be_Boundary = isBoundary(انتہا_Pos);      // Has side effect of setting iterator position.
    (خالی)انتہا_Should_Be_Boundary;
    U_ASSERT(انتہا_Should_Be_Boundary);
    U_ASSERT(fPosition == انتہا_Pos);
    واپس انتہا_Pos;
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
    اگر (n > 0) {
        برائے (; n > 0 && نتیجہ != UBRK_مکمل; --n) {
            نتیجہ = next();
        }
    } ورنہ اگر (n < 0) {
        برائے (; n < 0 && نتیجہ != UBRK_مکمل; ++n) {
            نتیجہ = پچھلا_2();
        }
    } ورنہ {
        نتیجہ = current();
    }
    واپس نتیجہ;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t قاعدہ_Based_Break_Iterator::next() {
    fBreakCache->next();
    واپس f_مکمل ? UBRK_مکمل : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t قاعدہ_Based_Break_Iterator::پچھلا_2() {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    fBreakCache->پچھلا_2(حالت_2);
    واپس f_مکمل ? UBRK_مکمل : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t قاعدہ_Based_Break_Iterator::following(int32_t آغاز_Pos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    اگر (آغاز_Pos < 0) {
        واپس first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_سیٹ_Native_اشاریہ(&f_متن, آغاز_Pos);
    آغاز_Pos = static_cast<int32_t>(utext_get_Native_اشاریہ(&f_متن));

    UError_رمز حالت_2 = U_ZERO_ERROR;
    fBreakCache->following(آغاز_Pos, حالت_2);
    واپس f_مکمل ? UBRK_مکمل : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t قاعدہ_Based_Break_Iterator::preceding(int32_t انحراف) {
    اگر (انحراف > utext_native_لمبائی(&f_متن)) {
        واپس آخری();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_سیٹ_Native_اشاریہ(&f_متن, انحراف);
    int32_t adjusted_انحراف = static_cast<int32_t>(utext_get_Native_اشاریہ(&f_متن));

    UError_رمز حالت_2 = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_انحراف, حالت_2);
    واپس f_مکمل ? UBRK_مکمل : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool قاعدہ_Based_Break_Iterator::isBoundary(int32_t انحراف) {
    // out-of-range indexes are never boundary positions
    اگر (انحراف < 0) {
        first();       // For side effects on current position, tag values.
        واپس جھوٹ;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_سیٹ_Native_اشاریہ(&f_متن, انحراف);
    int32_t adjusted_انحراف = static_cast<int32_t>(utext_get_Native_اشاریہ(&f_متن));

    منطقی نتیجہ = جھوٹ;
    UError_رمز حالت_2 = U_ZERO_ERROR;
    اگر (fBreakCache->seek(adjusted_انحراف) || fBreakCache->populateNear(adjusted_انحراف, حالت_2)) {
        نتیجہ = (fBreakCache->current() == انحراف);
    }

    اگر (نتیجہ && adjusted_انحراف < انحراف && utext_char32At(&f_متن, انحراف) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        واپس جھوٹ;
    }
    اگر (!نتیجہ) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    واپس نتیجہ;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t قاعدہ_Based_Break_Iterator::current() ثابت {
    واپس fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
شمار RBBIRun_منوال {
    RBBI_آغاز,     // state machine processing is before first char of input
    RBBI_تسلسل,       // state machine processing is in the user text
    RBBI_انتہا        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
ساکن اندرونی uint16_t TrieFunc8(ثابت UCPTrie *trie, UChar32 c) {
    واپس UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

ساکن اندرونی uint16_t TrieFunc16(ثابت UCPTrie *trie, UChar32 c) {
    واپس UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t قاعدہ_Based_Break_Iterator::handle_اگلا() {
    ثابت RBBIState_جدول *statetable = f_ڈیٹا->f_Forward_جدول;
    منطقی use8_بٹ_Trie = ucptrie_get_قدر_چوڑائی(f_ڈیٹا->fTrie) == UCPTRIE_قدر_بٹ_8;
    اگر (statetable->fFlags & RBBI_8_بٹ_صفیں) {
        اگر (use8_بٹ_Trie) {
            واپس handle_اگلا<RBBIState_جدول_Row8, TrieFunc8>();
        } ورنہ {
            واپس handle_اگلا<RBBIState_جدول_Row8, TrieFunc16>();
        }
    } ورنہ {
        اگر (use8_بٹ_Trie) {
            واپس handle_اگلا<RBBIState_جدول_Row16, TrieFunc8>();
        } ورنہ {
            واپس handle_اگلا<RBBIState_جدول_Row16, TrieFunc16>();
        }
    }
}

int32_t قاعدہ_Based_Break_Iterator::handle_محفوظ_پچھلا(int32_t fromPosition) {
    ثابت RBBIState_جدول *statetable = f_ڈیٹا->f_الٹ_جدول;
    منطقی use8_بٹ_Trie = ucptrie_get_قدر_چوڑائی(f_ڈیٹا->fTrie) == UCPTRIE_قدر_بٹ_8;
    اگر (statetable->fFlags & RBBI_8_بٹ_صفیں) {
        اگر (use8_بٹ_Trie) {
            واپس handle_محفوظ_پچھلا<RBBIState_جدول_Row8, TrieFunc8>(fromPosition);
        } ورنہ {
            واپس handle_محفوظ_پچھلا<RBBIState_جدول_Row8, TrieFunc16>(fromPosition);
        }
    } ورنہ {
        اگر (use8_بٹ_Trie) {
            واپس handle_محفوظ_پچھلا<RBBIState_جدول_Row16, TrieFunc8>(fromPosition);
        } ورنہ {
            واپس handle_محفوظ_پچھلا<RBBIState_جدول_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
سانچہ <قسم_نام صف_Type, قاعدہ_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعدہ_Based_Break_Iterator::handle_اگلا() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_منوال         منوال;

    صف_Type             *صف;
    UChar32             c;
    int32_t             نتیجہ             = 0;
    int32_t             initialPosition    = 0;
    ثابت RBBIState_جدول *statetable       = f_ڈیٹا->f_Forward_جدول;
    ثابت حرف         *جدول_ڈیٹا          = statetable->f_جدول_ڈیٹا;
    uint32_t            جدول_صف_Len        = statetable->f_صف_Len;
    uint32_t            dict_آغاز          = statetable->f_Dict_Categories_آغاز;
    #اگر_تعریف_شدہ RBBI_DEBUG
        اگر (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #اگر_ختم

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_قاعدہ_حالت_اشاریہ = 0;

    f_Dictionary_Char_گنتی = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&f_متن, initialPosition);
    نتیجہ          = initialPosition;
    c               = UTEXT_NEXT32(&f_متن);
    اگر (c==U_SENTINEL) {
        f_مکمل = سچ;
        واپس UBRK_مکمل;
    }

    //  Set the initial state for the state machine
    state = آغاز_STATE;
    صف = (صف_Type *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (جدول_ڈیٹا + جدول_صف_Len * state);


    منوال     = RBBI_تسلسل;


    // loop until we reach the end of the text or transition to state 0
    //
    برائے (;;) {
        اگر (c == U_SENTINEL) {
            // Reached end of input string.
            اگر (منوال == RBBI_انتہا) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                توڑو;
            }
            // Run the loop one last time with the fake end-of-input character category.
            منوال = RBBI_انتہا;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        اگر (منوال == RBBI_تسلسل) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_ڈیٹا->fTrie, c);
            f_Dictionary_Char_گنتی += (category >= dict_آغاز);
        }

       #اگر_تعریف_شدہ RBBI_DEBUG
            اگر (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_اشاریہ(&f_متن));
                اگر (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } ورنہ {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #اگر_ختم

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_ڈیٹا->f_سرخی->f_Cat_گنتی);
        state = صف->f_اگلا_State[category];  /*Not accessing beyond memory*/
        صف = (صف_Type *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (جدول_ڈیٹا + جدول_صف_Len * state);


        uint16_t accepting = صف->fAccepting;
        اگر (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            اگر (منوال != RBBI_آغاز) {
                نتیجہ = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            }
            f_قاعدہ_حالت_اشاریہ = صف->f_لیبل_فہرست_Idx;   // Remember the break status (tag) values.
        } ورنہ اگر (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز);
            int32_t lookahead_نتیجہ = fLookAheadMatches[accepting];
            اگر (lookahead_نتیجہ >= 0) {
                f_قاعدہ_حالت_اشاریہ = صف->f_لیبل_فہرست_Idx;
                fPosition = lookahead_نتیجہ;
                واپس lookahead_نتیجہ;
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
        U_ASSERT(قاعدہ == 0 || قاعدہ < f_ڈیٹا->f_Forward_جدول->f_Look_Ahead_Results_سائز);
        اگر (قاعدہ > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            fLookAheadMatches[قاعدہ] = pos;
        }

        اگر (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            توڑو;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        اگر (منوال == RBBI_تسلسل) {
            c = UTEXT_NEXT32(&f_متن);
        } ورنہ {
            اگر (منوال == RBBI_آغاز) {
                منوال = RBBI_تسلسل;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    اگر (نتیجہ == initialPosition) {
        utext_سیٹ_Native_اشاریہ(&f_متن, initialPosition);
        utext_next32(&f_متن);
        نتیجہ = static_cast<int32_t>(utext_get_Native_اشاریہ(&f_متن));
        f_قاعدہ_حالت_اشاریہ = 0;
    }

    // Leave the iterator at our result position.
    fPosition = نتیجہ;
    #اگر_تعریف_شدہ RBBI_DEBUG
        اگر (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", نتیجہ);
        }
    #اگر_ختم
    واپس نتیجہ;
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
سانچہ <قسم_نام صف_Type, قاعدہ_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعدہ_Based_Break_Iterator::handle_محفوظ_پچھلا(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    صف_Type            *صف;
    UChar32             c;
    int32_t             نتیجہ          = 0;

    ثابت RBBIState_جدول *state_جدول = f_ڈیٹا->f_الٹ_جدول;
    UTEXT_SETNATIVEINDEX(&f_متن, fromPosition);
    #اگر_تعریف_شدہ RBBI_DEBUG
        اگر (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #اگر_ختم

    // if we're already at the start of the text, return DONE.
    اگر (f_ڈیٹا == خالی_اشاریہ || UTEXT_GETNATIVEINDEX(&f_متن)==0) {
        واپس BreakIterator::مکمل_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&f_متن);
    state = آغاز_STATE;
    صف = (صف_Type *)
            (state_جدول->f_جدول_ڈیٹا + (state_جدول->f_صف_Len * state));

    // loop until we reach the start of the text or transition to state 0
    //
    برائے (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&f_متن)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_ڈیٹا->fTrie, c);

        #اگر_تعریف_شدہ RBBI_DEBUG
            اگر (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_اشاریہ(&f_متن));
                اگر (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } ورنہ {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #اگر_ختم

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_ڈیٹا->f_سرخی->f_Cat_گنتی);
        state = صف->f_اگلا_State[category];  /*Not accessing beyond memory*/
        صف = (صف_Type *)
            (state_جدول->f_جدول_ڈیٹا + (state_جدول->f_صف_Len * state));

        اگر (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            توڑو;
        }
    }

    // The state machine is done.  Check whether it found a match...
    نتیجہ = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
    #اگر_تعریف_شدہ RBBI_DEBUG
        اگر (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", نتیجہ);
        }
    #اگر_ختم
    واپس نتیجہ;
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
    int32_t  idx = f_قاعدہ_حالت_اشاریہ + f_ڈیٹا->f_قاعدہ_حالت_جدول[f_قاعدہ_حالت_اشاریہ];
    int32_t  لیبل_Val = f_ڈیٹا->f_قاعدہ_حالت_جدول[idx];

    واپس لیبل_Val;
}


int32_t قاعدہ_Based_Break_Iterator::get_قاعدہ_حالت_Vec(
             int32_t *fill_اندر_Vec, int32_t capacity, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }

    int32_t  عدد_Vals = f_ڈیٹا->f_قاعدہ_حالت_جدول[f_قاعدہ_حالت_اشاریہ];
    int32_t  عدد_Vals_مقصد_Copy = عدد_Vals;
    اگر (عدد_Vals > capacity) {
        حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
        عدد_Vals_مقصد_Copy = capacity;
    }
    صحیح i;
    برائے (i=0; i<عدد_Vals_مقصد_Copy; i++) {
        fill_اندر_Vec[i] = f_ڈیٹا->f_قاعدہ_حالت_جدول[f_قاعدہ_حالت_اشاریہ + i + 1];
    }
    واپس عدد_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ثابت uint8_t* قاعدہ_Based_Break_Iterator::get_ثنائی_قواعد(uint32_t& length) U_LIFETIME_BOUND {
    ثابت uint8_t  *retPtr = خالی_اشاریہ;
    length = 0;

    اگر (f_ڈیٹا != خالی_اشاریہ) {
        retPtr = reinterpret_cast<ثابت uint8_t*>(f_ڈیٹا->f_سرخی);
        length = f_ڈیٹا->f_سرخی->f_لمبائی;
    }
    واپس retPtr;
}


قاعدہ_Based_Break_Iterator *قاعدہ_Based_Break_Iterator::create_بفر_Clone(
        خالی * /*stackBuffer*/, int32_t &بفر_سائز, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)){
        واپس خالی_اشاریہ;
    }

    اگر (بفر_سائز == 0) {
        بفر_سائز = 1;  // preflighting for deprecated functionality
        واپس خالی_اشاریہ;
    }

    BreakIterator *clonedBI = clone();
    اگر (clonedBI == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    } ورنہ {
        حالت_2 = U_SAFECLONE_ALLOCATED_WARNING;
    }
    واپس (قاعدہ_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_انتہا


ساکن icu::UStack *gLanguageBreakFactories = خالی_اشاریہ;
ساکن ثابت icu::UnicodeString *g_خالی_خانے_String = خالی_اشاریہ;
ساکن icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ساکن icu::UInitOnce gRBBIInitOnce {};
ساکن icu::ICULanguageBreakFactory *gICULanguageBreakFactory = خالی_اشاریہ;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    حذف gLanguageBreakFactories;
    gLanguageBreakFactories = خالی_اشاریہ;
    حذف g_خالی_خانے_String;
    g_خالی_خانے_String = خالی_اشاریہ;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    واپس سچ;
}
U_CDECL_انتہا

U_CDECL_BEGIN
ساکن خالی U_CALLCONV _deleteFactory(خالی *obj) {
    حذف (icu::LanguageBreakFactory *) obj;
}
U_CDECL_انتہا
U_NAMESPACE_BEGIN

ساکن خالی U_CALLCONV rbbiInit() {
    g_خالی_خانے_String = نیا UnicodeString();
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ساکن خالی U_CALLCONV initLanguageFactories(UError_رمز& حالت_2) {
    U_ASSERT(gLanguageBreakFactories == خالی_اشاریہ);
    gLanguageBreakFactories = نیا UStack(_deleteFactory, خالی_اشاریہ, حالت_2);
    اگر (gLanguageBreakFactories != خالی_اشاریہ && U_SUCCESS(حالت_2)) {
        Local_اشارہ_گر<ICULanguageBreakFactory> factory(نیا ICULanguageBreakFactory(حالت_2), حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, حالت_2);
#اگر_تعریف_شدہ U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &حالت_2);
            اگر (extra != خالی_اشاریہ) {
                gLanguageBreakFactories->push(extra, حالت_2);
            }
#اگر_ختم
        }
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

خالی ensureLanguageFactories(UError_رمز& حالت_2) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, حالت_2);
}

ساکن ثابت LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ثابت حرف* locale)
{
    UError_رمز حالت_2 = U_ZERO_ERROR;
    ensureLanguageFactories(حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس خالی_اشاریہ;

    int32_t i = gLanguageBreakFactories->size();
    ثابت LanguageBreakEngine *lbe = خالی_اشاریہ;
    جب (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        اگر (lbe != خالی_اشاریہ) {
            توڑو;
        }
    }
    واپس lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
ثابت LanguageBreakEngine*
قاعدہ_Based_Break_Iterator::getLanguageBreakEngine(UChar32 c, ثابت حرف* locale) U_LIFETIME_BOUND {
    ثابت LanguageBreakEngine *lbe = خالی_اشاریہ;
    UError_رمز حالت_2 = U_ZERO_ERROR;

    اگر (fLanguageBreakEngines == خالی_اشاریہ) {
        fLanguageBreakEngines = نیا UStack(حالت_2);
        اگر (fLanguageBreakEngines == خالی_اشاریہ || U_FAILURE(حالت_2)) {
            حذف fLanguageBreakEngines;
            fLanguageBreakEngines = خالی_اشاریہ;
            واپس خالی_اشاریہ;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    جب (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        اگر (lbe->handles(c, locale)) {
            واپس lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    اگر (lbe != خالی_اشاریہ) {
        fLanguageBreakEngines->push((خالی *)lbe, حالت_2);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        واپس lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    اگر (fUnhandledBreakEngine == خالی_اشاریہ) {
        fUnhandledBreakEngine = نیا UnhandledEngine(حالت_2);
        اگر (U_SUCCESS(حالت_2) && fUnhandledBreakEngine == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->داخل_کرو_Element_At(fUnhandledBreakEngine, 0, حالت_2);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        اگر (U_FAILURE(حالت_2)) {
            حذف fUnhandledBreakEngine;
            fUnhandledBreakEngine = خالی_اشاریہ;
            واپس خالی_اشاریہ;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    واپس fUnhandledBreakEngine;
}

#اگر_غیر_تعریف_شدہ U_HIDE_DRAFT_API
خالی U_EXPORT2 قاعدہ_Based_Break_Iterator::رجسٹر_External_Break_Engine(
                  ExternalBreakEngine* مقصد_Adopt, UError_رمز& حالت_2) {
    Local_اشارہ_گر<ExternalBreakEngine> engine(مقصد_Adopt, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس;
    ensureLanguageFactories(حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس;
    gICULanguageBreakFactory->جوڑو_External_Engine(engine.orphan(), حالت_2);
}
#اگر_ختم  /* U_HIDE_DRAFT_API */


خالی قاعدہ_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

خالی قاعدہ_Based_Break_Iterator::dumpTables() {
    f_ڈیٹا->print_ڈیٹا();
}

/**
 * Returns the description used to create this iterator
 */

ثابت UnicodeString&
قاعدہ_Based_Break_Iterator::get_قواعد() ثابت U_LIFETIME_BOUND {
    اگر (f_ڈیٹا != خالی_اشاریہ) {
        واپس f_ڈیٹا->get_قاعدہ_ماخذ_String();
    } ورنہ {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        واپس *g_خالی_خانے_String;
    }
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */

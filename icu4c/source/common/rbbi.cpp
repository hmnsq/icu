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

#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
ساكن UBool gTrace = خطأ;
#جيڪڏهن_پورو

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t شروعات_STATE = 1;

// The state-transition value indicating "stop"
constexpr int32_t STOP_STATE = 0;


UOBJECT_DEFINE_RTTI_IMPLEMENTATION(قاعدو_Based_Break_Iterator)


//=======================================================================
// constructors
//=======================================================================

/**
 * Constructs a RuleBasedBreakIterator that uses the already-created
 * tables object that is passed in as a parameter.
 */
قاعدو_Based_Break_Iterator::قاعدو_Based_Break_Iterator(RBBIData_سرو* معلومات, UError_ڪوڊ &حالت)
 : قاعدو_Based_Break_Iterator(&حالت)
{
    f_معلومات = جديد RBBIDataWrapper(معلومات, حالت); // status checked in constructor
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_معلومات == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    إذا (f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز * حجم(int32_t)));
        إذا (fLookAheadMatches == خالي_اشارو) {
            حالت = U_يادگيري_ورهاست_ERROR;
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
قاعدو_Based_Break_Iterator::قاعدو_Based_Break_Iterator(UData_يادگيري* udm, UBool isPhraseBreaking,
        UError_ڪوڊ &حالت) : قاعدو_Based_Break_Iterator(udm, حالت)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
قاعدو_Based_Break_Iterator::قاعدو_Based_Break_Iterator(ثابت uint8_t *compiled_قاعدا,
                       uint32_t       قاعدو_ڊيگهائي,
                       UError_ڪوڊ     &حالت)
 : قاعدو_Based_Break_Iterator(&حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (compiled_قاعدا == خالي_اشارو || قاعدو_ڊيگهائي < حجم(RBBIData_سرو)) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    ثابت RBBIData_سرو* معلومات = reinterpret_cast<ثابت RBBIData_سرو*>(compiled_قاعدا);
    إذا (معلومات->f_ڊيگهائي > قاعدو_ڊيگهائي) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    f_معلومات = جديد RBBIDataWrapper(معلومات, RBBIDataWrapper::kDontAdopt, حالت);
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_معلومات == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    إذا (f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز * حجم(int32_t)));
        إذا (fLookAheadMatches == خالي_اشارو) {
            حالت = U_يادگيري_ورهاست_ERROR;
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
قاعدو_Based_Break_Iterator::قاعدو_Based_Break_Iterator(UData_يادگيري* udm, UError_ڪوڊ &حالت)
 : قاعدو_Based_Break_Iterator(&حالت)
{
    f_معلومات = جديد RBBIDataWrapper(udm, حالت); // status checked in constructor
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_معلومات == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    إذا (f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز * حجم(int32_t)));
        إذا (fLookAheadMatches == خالي_اشارو) {
            حالت = U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
قاعدو_Based_Break_Iterator::قاعدو_Based_Break_Iterator( ثابت UnicodeString  &قاعدا,
                                                UParseError          &parseError,
                                                UError_ڪوڊ           &حالت)
 : قاعدو_Based_Break_Iterator(&حالت)
{
    إذا (U_FAILURE(حالت)) {أرجع;}
    قاعدو_Based_Break_Iterator *bi = (قاعدو_Based_Break_Iterator *)
        RBBIRuleBuilder::create_قاعدو_Based_Break_Iterator(قاعدا, &parseError, حالت);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    إذا (U_SUCCESS(حالت)) {
        *هذا = *bi;
        ميٽيو bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
قاعدو_Based_Break_Iterator::قاعدو_Based_Break_Iterator()
 : قاعدو_Based_Break_Iterator(خالي_اشارو)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
قاعدو_Based_Break_Iterator::قاعدو_Based_Break_Iterator(UError_ڪوڊ *حالت) {
    UError_ڪوڊ ec = U_ZERO_ERROR;
    إذا (حالت == خالي_اشارو) {
        حالت = &ec;
    }
    utext_openUChars(&f_متن, خالي_اشارو, 0, حالت);
    Local_اشارو<DictionaryCache> lpDictionaryCache(جديد DictionaryCache(هذا, *حالت), *حالت);
    Local_اشارو<BreakCache> lpBreakCache(جديد BreakCache(هذا, *حالت), *حالت);
    إذا (U_FAILURE(*حالت)) {
        f_Error_ڪوڊ = *حالت;
        أرجع;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    ساكن UBool debug_Init_پورو = خطأ;
    إذا (debug_Init_پورو == خطأ) {
        حرف *debugEnv = getenv("U_RBBIDEBUG");
        إذا (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = سچ;
        }
        debug_Init_پورو = سچ;
    }
#جيڪڏهن_پورو
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
قاعدو_Based_Break_Iterator::قاعدو_Based_Break_Iterator(ثابت قاعدو_Based_Break_Iterator& other)
: قاعدو_Based_Break_Iterator()
{
    *هذا = other;
}


/**
 * Destructor
 */
قاعدو_Based_Break_Iterator::~قاعدو_Based_Break_Iterator() {
    إذا (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        ميٽيو fCharIter;
    }
    fCharIter = خالي_اشارو;

    utext_close(&f_متن);

    إذا (f_معلومات != خالي_اشارو) {
        f_معلومات->removeReference();
        f_معلومات = خالي_اشارو;
    }
    ميٽيو fBreakCache;
    fBreakCache = خالي_اشارو;

    ميٽيو fDictionaryCache;
    fDictionaryCache = خالي_اشارو;

    ميٽيو fLanguageBreakEngines;
    fLanguageBreakEngines = خالي_اشارو;

    ميٽيو fUnhandledBreakEngine;
    fUnhandledBreakEngine = خالي_اشارو;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = خالي_اشارو;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
قاعدو_Based_Break_Iterator&
قاعدو_Based_Break_Iterator::عامل=(ثابت قاعدو_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    إذا (هذا == &that) {
        أرجع *هذا;
    }
    BreakIterator::عامل=(that);

    إذا (fLanguageBreakEngines != خالي_اشارو) {
        ميٽيو fLanguageBreakEngines;
        fLanguageBreakEngines = خالي_اشارو;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    utext_clone(&f_متن, &that.f_متن, خطأ, سچ, &حالت);

    إذا (fCharIter != &fSCharIter) {
        ميٽيو fCharIter;
    }
    fCharIter = &fSCharIter;

    إذا (that.fCharIter != خالي_اشارو && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    إذا (fCharIter == خالي_اشارو) {
        fCharIter = &fSCharIter;
    }

    إذا (f_معلومات != خالي_اشارو) {
        f_معلومات->removeReference();
        f_معلومات = خالي_اشارو;
    }
    إذا (that.f_معلومات != خالي_اشارو) {
        f_معلومات = that.f_معلومات->جوڙيو_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = خالي_اشارو;
    إذا (f_معلومات && f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز * حجم(int32_t)));
    }


    fPosition = that.fPosition;
    f_قاعدو_حالت_اشاريو = that.f_قاعدو_حالت_اشاريو;
    f_پورو = that.f_پورو;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_قاعدو_حالت_اشاريو);
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
قاعدو_Based_Break_Iterator*
قاعدو_Based_Break_Iterator::clone() ثابت {
    أرجع جديد قاعدو_Based_Break_Iterator(*هذا);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
منطقي
قاعدو_Based_Break_Iterator::عامل==(ثابت BreakIterator& that) ثابت {
    إذا (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }
    إذا (هذا == &that) {
        أرجع سچ;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ثابت قاعدو_Based_Break_Iterator& that2 = static_cast<ثابت قاعدو_Based_Break_Iterator&>(that);

    إذا (!utext_equals(&f_متن, &that2.f_متن)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        أرجع خطأ;
    }

    إذا (!(fPosition == that2.fPosition &&
            f_قاعدو_حالت_اشاريو == that2.f_قاعدو_حالت_اشاريو &&
            f_پورو == that2.f_پورو &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        أرجع خطأ;
    }

    إذا (that2.f_معلومات == f_معلومات ||
        (f_معلومات != خالي_اشارو && that2.f_معلومات != خالي_اشارو && *that2.f_معلومات == *f_معلومات)) {
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
قاعدو_Based_Break_Iterator::hash_ڪوڊ() ثابت {
    int32_t   hash = 0;
    إذا (f_معلومات != خالي_اشارو) {
        hash = f_معلومات->hash_ڪوڊ();
    }
    أرجع hash;
}


فراغ قاعدو_Based_Break_Iterator::سيٽ_متن(UText *ut, UError_ڪوڊ &حالت) {
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
    fSCharIter.سيٽ_متن(u"", 0);

    إذا (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        ميٽيو fCharIter;
    }
    fCharIter = &fSCharIter;

    هذا->first();
}


UText* قاعدو_Based_Break_Iterator::getUText(UText* fill_اندريون U_LIFETIME_BOUND, UError_ڪوڊ& حالت) ثابت {
    UText *نتيجو = utext_clone(fill_اندريون, &f_متن, خطأ, سچ, &حالت);
    أرجع نتيجو;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
قاعدو_Based_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    أرجع *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
فراغ
قاعدو_Based_Break_Iterator::adopt_متن(CharacterIterator* new_متن) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    إذا (fCharIter != &fSCharIter) {
        ميٽيو fCharIter;
    }

    fCharIter = new_متن;
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    إذا (new_متن==خالي_اشارو || new_متن->شروعات_اشاريو() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&f_متن, خالي_اشارو, 0, &حالت);
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
قاعدو_Based_Break_Iterator::سيٽ_متن(ثابت UnicodeString& new_متن) {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&f_متن, &new_متن, &حالت);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.سيٽ_متن(new_متن.get_بفر(), new_متن.length());

    إذا (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        ميٽيو fCharIter;
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
قاعدو_Based_Break_Iterator&
قاعدو_Based_Break_Iterator::refresh_داخلا_متن(UText* input, UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    إذا (U_FAILURE(حالت)) {
        أرجع *هذا;
    }
    إذا (input == خالي_اشارو) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع *هذا;
    }
    int64_t pos = utext_get_Native_اشاريو(&f_متن);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&f_متن, input, خطأ, سچ, &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع *هذا;
    }
    utext_سيٽ_Native_اشاريو(&f_متن, pos);
    إذا (utext_get_Native_اشاريو(&f_متن) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        حالت = U_ILLEGAL_دليل_ERROR;
    }
    أرجع *هذا;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t قاعدو_Based_Break_Iterator::first() {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
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
int32_t قاعدو_Based_Break_Iterator::آخري() {
    int32_t پڇاڙي_Pos = static_cast<int32_t>(utext_native_ڊيگهائي(&f_متن));
    UBool پڇاڙي_Should_Be_Boundary = isBoundary(پڇاڙي_Pos);      // Has side effect of setting iterator position.
    (فراغ)پڇاڙي_Should_Be_Boundary;
    U_ASSERT(پڇاڙي_Should_Be_Boundary);
    U_ASSERT(fPosition == پڇاڙي_Pos);
    أرجع پڇاڙي_Pos;
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
int32_t قاعدو_Based_Break_Iterator::next(int32_t n) {
    int32_t نتيجو = 0;
    إذا (n > 0) {
        لكل (; n > 0 && نتيجو != UBRK_پورو; --n) {
            نتيجو = next();
        }
    } وإلا إذا (n < 0) {
        لكل (; n < 0 && نتيجو != UBRK_پورو; ++n) {
            نتيجو = اڳيون();
        }
    } وإلا {
        نتيجو = current();
    }
    أرجع نتيجو;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t قاعدو_Based_Break_Iterator::next() {
    fBreakCache->next();
    أرجع f_پورو ? UBRK_پورو : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t قاعدو_Based_Break_Iterator::اڳيون() {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    fBreakCache->اڳيون(حالت);
    أرجع f_پورو ? UBRK_پورو : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t قاعدو_Based_Break_Iterator::following(int32_t شروعات_Pos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    إذا (شروعات_Pos < 0) {
        أرجع first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_سيٽ_Native_اشاريو(&f_متن, شروعات_Pos);
    شروعات_Pos = static_cast<int32_t>(utext_get_Native_اشاريو(&f_متن));

    UError_ڪوڊ حالت = U_ZERO_ERROR;
    fBreakCache->following(شروعات_Pos, حالت);
    أرجع f_پورو ? UBRK_پورو : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t قاعدو_Based_Break_Iterator::preceding(int32_t وٿي) {
    إذا (وٿي > utext_native_ڊيگهائي(&f_متن)) {
        أرجع آخري();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_سيٽ_Native_اشاريو(&f_متن, وٿي);
    int32_t adjusted_وٿي = static_cast<int32_t>(utext_get_Native_اشاريو(&f_متن));

    UError_ڪوڊ حالت = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_وٿي, حالت);
    أرجع f_پورو ? UBRK_پورو : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool قاعدو_Based_Break_Iterator::isBoundary(int32_t وٿي) {
    // out-of-range indexes are never boundary positions
    إذا (وٿي < 0) {
        first();       // For side effects on current position, tag values.
        أرجع خطأ;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_سيٽ_Native_اشاريو(&f_متن, وٿي);
    int32_t adjusted_وٿي = static_cast<int32_t>(utext_get_Native_اشاريو(&f_متن));

    منطقي نتيجو = خطأ;
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    إذا (fBreakCache->seek(adjusted_وٿي) || fBreakCache->populateNear(adjusted_وٿي, حالت)) {
        نتيجو = (fBreakCache->current() == وٿي);
    }

    إذا (نتيجو && adjusted_وٿي < وٿي && utext_char32At(&f_متن, وٿي) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        أرجع خطأ;
    }
    إذا (!نتيجو) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    أرجع نتيجو;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t قاعدو_Based_Break_Iterator::current() ثابت {
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
تعداد RBBIRun_گهڻائي {
    RBBI_شروعات,     // state machine processing is before first char of input
    RBBI_لاڳيتو,       // state machine processing is in the user text
    RBBI_پڇاڙي        // state machine processing is after end of user text.
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

int32_t قاعدو_Based_Break_Iterator::handle_ايندڙ() {
    ثابت RBBIState_جدول *statetable = f_معلومات->f_Forward_جدول;
    منطقي use8_بٽ_Trie = ucptrie_get_قدر_ويڪر(f_معلومات->fTrie) == UCPTRIE_قدر_بٽ_8;
    إذا (statetable->fFlags & RBBI_8_بٽ_صفون) {
        إذا (use8_بٽ_Trie) {
            أرجع handle_ايندڙ<RBBIState_جدول_Row8, TrieFunc8>();
        } وإلا {
            أرجع handle_ايندڙ<RBBIState_جدول_Row8, TrieFunc16>();
        }
    } وإلا {
        إذا (use8_بٽ_Trie) {
            أرجع handle_ايندڙ<RBBIState_جدول_Row16, TrieFunc8>();
        } وإلا {
            أرجع handle_ايندڙ<RBBIState_جدول_Row16, TrieFunc16>();
        }
    }
}

int32_t قاعدو_Based_Break_Iterator::handle_محفوظ_اڳيون(int32_t fromPosition) {
    ثابت RBBIState_جدول *statetable = f_معلومات->f_ابتو_جدول;
    منطقي use8_بٽ_Trie = ucptrie_get_قدر_ويڪر(f_معلومات->fTrie) == UCPTRIE_قدر_بٽ_8;
    إذا (statetable->fFlags & RBBI_8_بٽ_صفون) {
        إذا (use8_بٽ_Trie) {
            أرجع handle_محفوظ_اڳيون<RBBIState_جدول_Row8, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handle_محفوظ_اڳيون<RBBIState_جدول_Row8, TrieFunc16>(fromPosition);
        }
    } وإلا {
        إذا (use8_بٽ_Trie) {
            أرجع handle_محفوظ_اڳيون<RBBIState_جدول_Row16, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handle_محفوظ_اڳيون<RBBIState_جدول_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
سانچو <قسم_نالو صف_Type, قاعدو_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعدو_Based_Break_Iterator::handle_ايندڙ() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_گهڻائي         گهڻائي;

    صف_Type             *صف;
    UChar32             c;
    int32_t             نتيجو             = 0;
    int32_t             initialPosition    = 0;
    ثابت RBBIState_جدول *statetable       = f_معلومات->f_Forward_جدول;
    ثابت حرف         *جدول_معلومات          = statetable->f_جدول_معلومات;
    uint32_t            جدول_صف_Len        = statetable->f_صف_Len;
    uint32_t            dict_شروعات          = statetable->f_Dict_Categories_شروعات;
    #جيڪڏهن_وصف_ٿيل RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #جيڪڏهن_پورو

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_قاعدو_حالت_اشاريو = 0;

    f_Dictionary_Char_ڳڻپ = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&f_متن, initialPosition);
    نتيجو          = initialPosition;
    c               = UTEXT_NEXT32(&f_متن);
    إذا (c==U_SENTINEL) {
        f_پورو = سچ;
        أرجع UBRK_پورو;
    }

    //  Set the initial state for the state machine
    state = شروعات_STATE;
    صف = (صف_Type *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (جدول_معلومات + جدول_صف_Len * state);


    گهڻائي     = RBBI_لاڳيتو;


    // loop until we reach the end of the text or transition to state 0
    //
    لكل (;;) {
        إذا (c == U_SENTINEL) {
            // Reached end of input string.
            إذا (گهڻائي == RBBI_پڇاڙي) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                اكسر;
            }
            // Run the loop one last time with the fake end-of-input character category.
            گهڻائي = RBBI_پڇاڙي;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        إذا (گهڻائي == RBBI_لاڳيتو) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_معلومات->fTrie, c);
            f_Dictionary_Char_ڳڻپ += (category >= dict_شروعات);
        }

       #جيڪڏهن_وصف_ٿيل RBBI_DEBUG
            إذا (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_اشاريو(&f_متن));
                إذا (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #جيڪڏهن_پورو

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_معلومات->f_سرو->f_Cat_ڳڻپ);
        state = صف->f_ايندڙ_State[category];  /*Not accessing beyond memory*/
        صف = (صف_Type *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (جدول_معلومات + جدول_صف_Len * state);


        uint16_t accepting = صف->fAccepting;
        إذا (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            إذا (گهڻائي != RBBI_شروعات) {
                نتيجو = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            }
            f_قاعدو_حالت_اشاريو = صف->f_ٽيگ_فهرست_Idx;   // Remember the break status (tag) values.
        } وإلا إذا (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز);
            int32_t lookahead_نتيجو = fLookAheadMatches[accepting];
            إذا (lookahead_نتيجو >= 0) {
                f_قاعدو_حالت_اشاريو = صف->f_ٽيگ_فهرست_Idx;
                fPosition = lookahead_نتيجو;
                أرجع lookahead_نتيجو;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t قاعدو = صف->fLookAhead;
        U_ASSERT(قاعدو == 0 || قاعدو > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(قاعدو == 0 || قاعدو < f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_سائيز);
        إذا (قاعدو > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            fLookAheadMatches[قاعدو] = pos;
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
        إذا (گهڻائي == RBBI_لاڳيتو) {
            c = UTEXT_NEXT32(&f_متن);
        } وإلا {
            إذا (گهڻائي == RBBI_شروعات) {
                گهڻائي = RBBI_لاڳيتو;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    إذا (نتيجو == initialPosition) {
        utext_سيٽ_Native_اشاريو(&f_متن, initialPosition);
        utext_next32(&f_متن);
        نتيجو = static_cast<int32_t>(utext_get_Native_اشاريو(&f_متن));
        f_قاعدو_حالت_اشاريو = 0;
    }

    // Leave the iterator at our result position.
    fPosition = نتيجو;
    #جيڪڏهن_وصف_ٿيل RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", نتيجو);
        }
    #جيڪڏهن_پورو
    أرجع نتيجو;
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
سانچو <قسم_نالو صف_Type, قاعدو_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعدو_Based_Break_Iterator::handle_محفوظ_اڳيون(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    صف_Type            *صف;
    UChar32             c;
    int32_t             نتيجو          = 0;

    ثابت RBBIState_جدول *state_جدول = f_معلومات->f_ابتو_جدول;
    UTEXT_SETNATIVEINDEX(&f_متن, fromPosition);
    #جيڪڏهن_وصف_ٿيل RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #جيڪڏهن_پورو

    // if we're already at the start of the text, return DONE.
    إذا (f_معلومات == خالي_اشارو || UTEXT_GETNATIVEINDEX(&f_متن)==0) {
        أرجع BreakIterator::پورو_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&f_متن);
    state = شروعات_STATE;
    صف = (صف_Type *)
            (state_جدول->f_جدول_معلومات + (state_جدول->f_صف_Len * state));

    // loop until we reach the start of the text or transition to state 0
    //
    لكل (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&f_متن)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_معلومات->fTrie, c);

        #جيڪڏهن_وصف_ٿيل RBBI_DEBUG
            إذا (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_اشاريو(&f_متن));
                إذا (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #جيڪڏهن_پورو

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_معلومات->f_سرو->f_Cat_ڳڻپ);
        state = صف->f_ايندڙ_State[category];  /*Not accessing beyond memory*/
        صف = (صف_Type *)
            (state_جدول->f_جدول_معلومات + (state_جدول->f_صف_Len * state));

        إذا (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            اكسر;
        }
    }

    // The state machine is done.  Check whether it found a match...
    نتيجو = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
    #جيڪڏهن_وصف_ٿيل RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", نتيجو);
        }
    #جيڪڏهن_پورو
    أرجع نتيجو;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  قاعدو_Based_Break_Iterator::get_قاعدو_حالت() ثابت {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_قاعدو_حالت_اشاريو + f_معلومات->f_قاعدو_حالت_جدول[f_قاعدو_حالت_اشاريو];
    int32_t  ٽيگ_Val = f_معلومات->f_قاعدو_حالت_جدول[idx];

    أرجع ٽيگ_Val;
}


int32_t قاعدو_Based_Break_Iterator::get_قاعدو_حالت_Vec(
             int32_t *fill_اندريون_Vec, int32_t capacity, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }

    int32_t  انگ_Vals = f_معلومات->f_قاعدو_حالت_جدول[f_قاعدو_حالت_اشاريو];
    int32_t  انگ_Vals_منزل_Copy = انگ_Vals;
    إذا (انگ_Vals > capacity) {
        حالت = U_بفر_حد_کان_مٿي_ERROR;
        انگ_Vals_منزل_Copy = capacity;
    }
    صحيح i;
    لكل (i=0; i<انگ_Vals_منزل_Copy; i++) {
        fill_اندريون_Vec[i] = f_معلومات->f_قاعدو_حالت_جدول[f_قاعدو_حالت_اشاريو + i + 1];
    }
    أرجع انگ_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ثابت uint8_t* قاعدو_Based_Break_Iterator::get_ٻٽو_قاعدا(uint32_t& length) U_LIFETIME_BOUND {
    ثابت uint8_t  *retPtr = خالي_اشارو;
    length = 0;

    إذا (f_معلومات != خالي_اشارو) {
        retPtr = reinterpret_cast<ثابت uint8_t*>(f_معلومات->f_سرو);
        length = f_معلومات->f_سرو->f_ڊيگهائي;
    }
    أرجع retPtr;
}


قاعدو_Based_Break_Iterator *قاعدو_Based_Break_Iterator::create_بفر_Clone(
        فراغ * /*stackBuffer*/, int32_t &بفر_سائيز, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)){
        أرجع خالي_اشارو;
    }

    إذا (بفر_سائيز == 0) {
        بفر_سائيز = 1;  // preflighting for deprecated functionality
        أرجع خالي_اشارو;
    }

    BreakIterator *clonedBI = clone();
    إذا (clonedBI == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
    } وإلا {
        حالت = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (قاعدو_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_پڇاڙي


ساكن icu::UStack *gLanguageBreakFactories = خالي_اشارو;
ساكن ثابت icu::UnicodeString *g_خالي_جايون_String = خالي_اشارو;
ساكن icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ساكن icu::UInitOnce gRBBIInitOnce {};
ساكن icu::ICULanguageBreakFactory *gICULanguageBreakFactory = خالي_اشارو;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    ميٽيو gLanguageBreakFactories;
    gLanguageBreakFactories = خالي_اشارو;
    ميٽيو g_خالي_جايون_String;
    g_خالي_جايون_String = خالي_اشارو;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    أرجع سچ;
}
U_CDECL_پڇاڙي

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV _deleteFactory(فراغ *obj) {
    ميٽيو (icu::LanguageBreakFactory *) obj;
}
U_CDECL_پڇاڙي
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV rbbiInit() {
    g_خالي_جايون_String = جديد UnicodeString();
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ساكن فراغ U_CALLCONV initLanguageFactories(UError_ڪوڊ& حالت) {
    U_ASSERT(gLanguageBreakFactories == خالي_اشارو);
    gLanguageBreakFactories = جديد UStack(_deleteFactory, خالي_اشارو, حالت);
    إذا (gLanguageBreakFactories != خالي_اشارو && U_SUCCESS(حالت)) {
        Local_اشارو<ICULanguageBreakFactory> factory(جديد ICULanguageBreakFactory(حالت), حالت);
        إذا (U_SUCCESS(حالت)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, حالت);
#جيڪڏهن_وصف_ٿيل U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &حالت);
            إذا (extra != خالي_اشارو) {
                gLanguageBreakFactories->push(extra, حالت);
            }
#جيڪڏهن_پورو
        }
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

فراغ ensureLanguageFactories(UError_ڪوڊ& حالت) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, حالت);
}

ساكن ثابت LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ثابت حرف* locale)
{
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    ensureLanguageFactories(حالت);
    إذا (U_FAILURE(حالت)) أرجع خالي_اشارو;

    int32_t i = gLanguageBreakFactories->size();
    ثابت LanguageBreakEngine *lbe = خالي_اشارو;
    طالما (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        إذا (lbe != خالي_اشارو) {
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
قاعدو_Based_Break_Iterator::getLanguageBreakEngine(UChar32 c, ثابت حرف* locale) U_LIFETIME_BOUND {
    ثابت LanguageBreakEngine *lbe = خالي_اشارو;
    UError_ڪوڊ حالت = U_ZERO_ERROR;

    إذا (fLanguageBreakEngines == خالي_اشارو) {
        fLanguageBreakEngines = جديد UStack(حالت);
        إذا (fLanguageBreakEngines == خالي_اشارو || U_FAILURE(حالت)) {
            ميٽيو fLanguageBreakEngines;
            fLanguageBreakEngines = خالي_اشارو;
            أرجع خالي_اشارو;
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
    إذا (lbe != خالي_اشارو) {
        fLanguageBreakEngines->push((فراغ *)lbe, حالت);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        أرجع lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    إذا (fUnhandledBreakEngine == خالي_اشارو) {
        fUnhandledBreakEngine = جديد UnhandledEngine(حالت);
        إذا (U_SUCCESS(حالت) && fUnhandledBreakEngine == خالي_اشارو) {
            حالت = U_يادگيري_ورهاست_ERROR;
            أرجع خالي_اشارو;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->وجهو_Element_At(fUnhandledBreakEngine, 0, حالت);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        إذا (U_FAILURE(حالت)) {
            ميٽيو fUnhandledBreakEngine;
            fUnhandledBreakEngine = خالي_اشارو;
            أرجع خالي_اشارو;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    أرجع fUnhandledBreakEngine;
}

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_DRAFT_API
فراغ U_EXPORT2 قاعدو_Based_Break_Iterator::رجسٽر_External_Break_Engine(
                  ExternalBreakEngine* منزل_Adopt, UError_ڪوڊ& حالت) {
    Local_اشارو<ExternalBreakEngine> engine(منزل_Adopt, حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    ensureLanguageFactories(حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    gICULanguageBreakFactory->جوڙيو_External_Engine(engine.orphan(), حالت);
}
#جيڪڏهن_پورو  /* U_HIDE_DRAFT_API */


فراغ قاعدو_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

فراغ قاعدو_Based_Break_Iterator::dumpTables() {
    f_معلومات->print_معلومات();
}

/**
 * Returns the description used to create this iterator
 */

ثابت UnicodeString&
قاعدو_Based_Break_Iterator::get_قاعدا() ثابت U_LIFETIME_BOUND {
    إذا (f_معلومات != خالي_اشارو) {
        أرجع f_معلومات->get_قاعدو_ذريعو_String();
    } وإلا {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        أرجع *g_خالي_جايون_String;
    }
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */

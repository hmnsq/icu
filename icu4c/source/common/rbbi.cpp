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

#اگر_تعریف_شده RBBI_DEBUG
ساكن UBool gTrace = خطأ;
#پایان_اگر

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t شروع_STATE = 1;

// The state-transition value indicating "stop"
constexpr int32_t STOP_STATE = 0;


UOBJECT_DEFINE_RTTI_IMPLEMENTATION(قاعده_Based_Break_Iterator)


//=======================================================================
// constructors
//=======================================================================

/**
 * Constructs a RuleBasedBreakIterator that uses the already-created
 * tables object that is passed in as a parameter.
 */
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(RBBIData_سرآیند* داده, UError_کد &وضعیت)
 : قاعده_Based_Break_Iterator(&وضعیت)
{
    f_داده = جديد RBBIDataWrapper(داده, وضعیت); // status checked in constructor
    إذا (U_FAILURE(وضعیت)) {أرجع;}
    إذا(f_داده == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    إذا (f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه * حجم(int32_t)));
        إذا (fLookAheadMatches == اشاره_تهی) {
            وضعیت = U_حافظه_تخصیص_ERROR;
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
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(UData_حافظه* udm, UBool isPhraseBreaking,
        UError_کد &وضعیت) : قاعده_Based_Break_Iterator(udm, وضعیت)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(ثابت uint8_t *compiled_قواعد,
                       uint32_t       قاعده_درازی,
                       UError_کد     &وضعیت)
 : قاعده_Based_Break_Iterator(&وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    إذا (compiled_قواعد == اشاره_تهی || قاعده_درازی < حجم(RBBIData_سرآیند)) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    ثابت RBBIData_سرآیند* داده = reinterpret_cast<ثابت RBBIData_سرآیند*>(compiled_قواعد);
    إذا (داده->f_درازی > قاعده_درازی) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    f_داده = جديد RBBIDataWrapper(داده, RBBIDataWrapper::kDontAdopt, وضعیت);
    إذا (U_FAILURE(وضعیت)) {أرجع;}
    إذا(f_داده == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    إذا (f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه * حجم(int32_t)));
        إذا (fLookAheadMatches == اشاره_تهی) {
            وضعیت = U_حافظه_تخصیص_ERROR;
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
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(UData_حافظه* udm, UError_کد &وضعیت)
 : قاعده_Based_Break_Iterator(&وضعیت)
{
    f_داده = جديد RBBIDataWrapper(udm, وضعیت); // status checked in constructor
    إذا (U_FAILURE(وضعیت)) {أرجع;}
    إذا(f_داده == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    إذا (f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه * حجم(int32_t)));
        إذا (fLookAheadMatches == اشاره_تهی) {
            وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator( ثابت UnicodeString  &قواعد,
                                                UParseError          &parseError,
                                                UError_کد           &وضعیت)
 : قاعده_Based_Break_Iterator(&وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {أرجع;}
    قاعده_Based_Break_Iterator *bi = (قاعده_Based_Break_Iterator *)
        RBBIRuleBuilder::create_قاعده_Based_Break_Iterator(قواعد, &parseError, وضعیت);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    إذا (U_SUCCESS(وضعیت)) {
        *هذا = *bi;
        حذف bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator()
 : قاعده_Based_Break_Iterator(اشاره_تهی)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(UError_کد *وضعیت) {
    UError_کد ec = U_ZERO_ERROR;
    إذا (وضعیت == اشاره_تهی) {
        وضعیت = &ec;
    }
    utext_openUChars(&f_متن, اشاره_تهی, 0, وضعیت);
    Local_اشاره_گر<DictionaryCache> lpDictionaryCache(جديد DictionaryCache(هذا, *وضعیت), *وضعیت);
    Local_اشاره_گر<BreakCache> lpBreakCache(جديد BreakCache(هذا, *وضعیت), *وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
        f_Error_کد = *وضعیت;
        أرجع;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#اگر_تعریف_شده RBBI_DEBUG
    ساكن UBool debug_Init_انجامشده = خطأ;
    إذا (debug_Init_انجامشده == خطأ) {
        حرف *debugEnv = getenv("U_RBBIDEBUG");
        إذا (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = درست;
        }
        debug_Init_انجامشده = درست;
    }
#پایان_اگر
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(ثابت قاعده_Based_Break_Iterator& other)
: قاعده_Based_Break_Iterator()
{
    *هذا = other;
}


/**
 * Destructor
 */
قاعده_Based_Break_Iterator::~قاعده_Based_Break_Iterator() {
    إذا (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        حذف fCharIter;
    }
    fCharIter = اشاره_تهی;

    utext_close(&f_متن);

    إذا (f_داده != اشاره_تهی) {
        f_داده->removeReference();
        f_داده = اشاره_تهی;
    }
    حذف fBreakCache;
    fBreakCache = اشاره_تهی;

    حذف fDictionaryCache;
    fDictionaryCache = اشاره_تهی;

    حذف fLanguageBreakEngines;
    fLanguageBreakEngines = اشاره_تهی;

    حذف fUnhandledBreakEngine;
    fUnhandledBreakEngine = اشاره_تهی;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = اشاره_تهی;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
قاعده_Based_Break_Iterator&
قاعده_Based_Break_Iterator::عملگر=(ثابت قاعده_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    إذا (هذا == &that) {
        أرجع *هذا;
    }
    BreakIterator::عملگر=(that);

    إذا (fLanguageBreakEngines != اشاره_تهی) {
        حذف fLanguageBreakEngines;
        fLanguageBreakEngines = اشاره_تهی;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_کد وضعیت = U_ZERO_ERROR;
    utext_clone(&f_متن, &that.f_متن, خطأ, درست, &وضعیت);

    إذا (fCharIter != &fSCharIter) {
        حذف fCharIter;
    }
    fCharIter = &fSCharIter;

    إذا (that.fCharIter != اشاره_تهی && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    إذا (fCharIter == اشاره_تهی) {
        fCharIter = &fSCharIter;
    }

    إذا (f_داده != اشاره_تهی) {
        f_داده->removeReference();
        f_داده = اشاره_تهی;
    }
    إذا (that.f_داده != اشاره_تهی) {
        f_داده = that.f_داده->جمع_کن_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = اشاره_تهی;
    إذا (f_داده && f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه * حجم(int32_t)));
    }


    fPosition = that.fPosition;
    f_قاعده_وضعیت_شاخص = that.f_قاعده_وضعیت_شاخص;
    f_انجامشده = that.f_انجامشده;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_قاعده_وضعیت_شاخص);
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
قاعده_Based_Break_Iterator*
قاعده_Based_Break_Iterator::clone() ثابت {
    أرجع جديد قاعده_Based_Break_Iterator(*هذا);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
منطقي
قاعده_Based_Break_Iterator::عملگر==(ثابت BreakIterator& that) ثابت {
    إذا (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }
    إذا (هذا == &that) {
        أرجع درست;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ثابت قاعده_Based_Break_Iterator& that2 = static_cast<ثابت قاعده_Based_Break_Iterator&>(that);

    إذا (!utext_equals(&f_متن, &that2.f_متن)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        أرجع خطأ;
    }

    إذا (!(fPosition == that2.fPosition &&
            f_قاعده_وضعیت_شاخص == that2.f_قاعده_وضعیت_شاخص &&
            f_انجامشده == that2.f_انجامشده &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        أرجع خطأ;
    }

    إذا (that2.f_داده == f_داده ||
        (f_داده != اشاره_تهی && that2.f_داده != اشاره_تهی && *that2.f_داده == *f_داده)) {
            // The two break iterators are using the same rules.
            أرجع درست;
        }
    أرجع خطأ;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
قاعده_Based_Break_Iterator::hash_کد() ثابت {
    int32_t   hash = 0;
    إذا (f_داده != اشاره_تهی) {
        hash = f_داده->hash_کد();
    }
    أرجع hash;
}


فراغ قاعده_Based_Break_Iterator::مجموعه_متن(UText *ut, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&f_متن, ut, خطأ, درست, &وضعیت);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.مجموعه_متن(u"", 0);

    إذا (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        حذف fCharIter;
    }
    fCharIter = &fSCharIter;

    هذا->first();
}


UText* قاعده_Based_Break_Iterator::getUText(UText* fill_درون U_LIFETIME_BOUND, UError_کد& وضعیت) ثابت {
    UText *نتیجه = utext_clone(fill_درون, &f_متن, خطأ, درست, &وضعیت);
    أرجع نتیجه;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
قاعده_Based_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    أرجع *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
فراغ
قاعده_Based_Break_Iterator::adopt_متن(CharacterIterator* new_متن) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    إذا (fCharIter != &fSCharIter) {
        حذف fCharIter;
    }

    fCharIter = new_متن;
    UError_کد وضعیت = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    إذا (new_متن==اشاره_تهی || new_متن->شروع_شاخص() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&f_متن, اشاره_تهی, 0, &وضعیت);
    } وإلا {
        utext_openCharacterIterator(&f_متن, new_متن, &وضعیت);
    }
    هذا->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
فراغ
قاعده_Based_Break_Iterator::مجموعه_متن(ثابت UnicodeString& new_متن) {
    UError_کد وضعیت = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&f_متن, &new_متن, &وضعیت);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.مجموعه_متن(new_متن.get_میانگیر(), new_متن.length());

    إذا (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        حذف fCharIter;
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
قاعده_Based_Break_Iterator&
قاعده_Based_Break_Iterator::refresh_درون_داد_متن(UText* input, UError_کد& وضعیت) U_LIFETIME_BOUND {
    إذا (U_FAILURE(وضعیت)) {
        أرجع *هذا;
    }
    إذا (input == اشاره_تهی) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع *هذا;
    }
    int64_t pos = utext_get_Native_شاخص(&f_متن);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&f_متن, input, خطأ, درست, &وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع *هذا;
    }
    utext_مجموعه_Native_شاخص(&f_متن, pos);
    إذا (utext_get_Native_شاخص(&f_متن) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }
    أرجع *هذا;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t قاعده_Based_Break_Iterator::first() {
    UError_کد وضعیت = U_ZERO_ERROR;
    إذا (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, وضعیت);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    أرجع 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t قاعده_Based_Break_Iterator::آخرین() {
    int32_t انتها_Pos = static_cast<int32_t>(utext_native_درازی(&f_متن));
    UBool انتها_Should_Be_Boundary = isBoundary(انتها_Pos);      // Has side effect of setting iterator position.
    (فراغ)انتها_Should_Be_Boundary;
    U_ASSERT(انتها_Should_Be_Boundary);
    U_ASSERT(fPosition == انتها_Pos);
    أرجع انتها_Pos;
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
int32_t قاعده_Based_Break_Iterator::next(int32_t n) {
    int32_t نتیجه = 0;
    إذا (n > 0) {
        لكل (; n > 0 && نتیجه != UBRK_انجامشده; --n) {
            نتیجه = next();
        }
    } وإلا إذا (n < 0) {
        لكل (; n < 0 && نتیجه != UBRK_انجامشده; ++n) {
            نتیجه = قبلی();
        }
    } وإلا {
        نتیجه = current();
    }
    أرجع نتیجه;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t قاعده_Based_Break_Iterator::next() {
    fBreakCache->next();
    أرجع f_انجامشده ? UBRK_انجامشده : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t قاعده_Based_Break_Iterator::قبلی() {
    UError_کد وضعیت = U_ZERO_ERROR;
    fBreakCache->قبلی(وضعیت);
    أرجع f_انجامشده ? UBRK_انجامشده : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t قاعده_Based_Break_Iterator::following(int32_t شروع_Pos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    إذا (شروع_Pos < 0) {
        أرجع first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_مجموعه_Native_شاخص(&f_متن, شروع_Pos);
    شروع_Pos = static_cast<int32_t>(utext_get_Native_شاخص(&f_متن));

    UError_کد وضعیت = U_ZERO_ERROR;
    fBreakCache->following(شروع_Pos, وضعیت);
    أرجع f_انجامشده ? UBRK_انجامشده : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t قاعده_Based_Break_Iterator::preceding(int32_t جابه_جایی) {
    إذا (جابه_جایی > utext_native_درازی(&f_متن)) {
        أرجع آخرین();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_مجموعه_Native_شاخص(&f_متن, جابه_جایی);
    int32_t adjusted_جابه_جایی = static_cast<int32_t>(utext_get_Native_شاخص(&f_متن));

    UError_کد وضعیت = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_جابه_جایی, وضعیت);
    أرجع f_انجامشده ? UBRK_انجامشده : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool قاعده_Based_Break_Iterator::isBoundary(int32_t جابه_جایی) {
    // out-of-range indexes are never boundary positions
    إذا (جابه_جایی < 0) {
        first();       // For side effects on current position, tag values.
        أرجع خطأ;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_مجموعه_Native_شاخص(&f_متن, جابه_جایی);
    int32_t adjusted_جابه_جایی = static_cast<int32_t>(utext_get_Native_شاخص(&f_متن));

    منطقي نتیجه = خطأ;
    UError_کد وضعیت = U_ZERO_ERROR;
    إذا (fBreakCache->seek(adjusted_جابه_جایی) || fBreakCache->populateNear(adjusted_جابه_جایی, وضعیت)) {
        نتیجه = (fBreakCache->current() == جابه_جایی);
    }

    إذا (نتیجه && adjusted_جابه_جایی < جابه_جایی && utext_char32At(&f_متن, جابه_جایی) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        أرجع خطأ;
    }
    إذا (!نتیجه) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    أرجع نتیجه;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t قاعده_Based_Break_Iterator::current() ثابت {
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
تعداد RBBIRun_نما {
    RBBI_شروع,     // state machine processing is before first char of input
    RBBI_پیاپی,       // state machine processing is in the user text
    RBBI_انتها        // state machine processing is after end of user text.
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

int32_t قاعده_Based_Break_Iterator::handle_بعدی() {
    ثابت RBBIState_جدول *statetable = f_داده->f_Forward_جدول;
    منطقي use8_بیتها_Trie = ucptrie_get_مقدار_عرض(f_داده->fTrie) == UCPTRIE_مقدار_بیتها_8;
    إذا (statetable->fFlags & RBBI_8_بیتها_ردیفها) {
        إذا (use8_بیتها_Trie) {
            أرجع handle_بعدی<RBBIState_جدول_Row8, TrieFunc8>();
        } وإلا {
            أرجع handle_بعدی<RBBIState_جدول_Row8, TrieFunc16>();
        }
    } وإلا {
        إذا (use8_بیتها_Trie) {
            أرجع handle_بعدی<RBBIState_جدول_Row16, TrieFunc8>();
        } وإلا {
            أرجع handle_بعدی<RBBIState_جدول_Row16, TrieFunc16>();
        }
    }
}

int32_t قاعده_Based_Break_Iterator::handle_امن_قبلی(int32_t fromPosition) {
    ثابت RBBIState_جدول *statetable = f_داده->f_معکوس_جدول;
    منطقي use8_بیتها_Trie = ucptrie_get_مقدار_عرض(f_داده->fTrie) == UCPTRIE_مقدار_بیتها_8;
    إذا (statetable->fFlags & RBBI_8_بیتها_ردیفها) {
        إذا (use8_بیتها_Trie) {
            أرجع handle_امن_قبلی<RBBIState_جدول_Row8, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handle_امن_قبلی<RBBIState_جدول_Row8, TrieFunc16>(fromPosition);
        }
    } وإلا {
        إذا (use8_بیتها_Trie) {
            أرجع handle_امن_قبلی<RBBIState_جدول_Row16, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handle_امن_قبلی<RBBIState_جدول_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
قالب <نام_نوع ردیف_Type, قاعده_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعده_Based_Break_Iterator::handle_بعدی() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_نما         نما;

    ردیف_Type             *ردیف;
    UChar32             c;
    int32_t             نتیجه             = 0;
    int32_t             initialPosition    = 0;
    ثابت RBBIState_جدول *statetable       = f_داده->f_Forward_جدول;
    ثابت حرف         *جدول_داده          = statetable->f_جدول_داده;
    uint32_t            جدول_ردیف_Len        = statetable->f_ردیف_Len;
    uint32_t            dict_شروع          = statetable->f_Dict_Categories_شروع;
    #اگر_تعریف_شده RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #پایان_اگر

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_قاعده_وضعیت_شاخص = 0;

    f_Dictionary_Char_شمار = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&f_متن, initialPosition);
    نتیجه          = initialPosition;
    c               = UTEXT_NEXT32(&f_متن);
    إذا (c==U_SENTINEL) {
        f_انجامشده = درست;
        أرجع UBRK_انجامشده;
    }

    //  Set the initial state for the state machine
    state = شروع_STATE;
    ردیف = (ردیف_Type *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (جدول_داده + جدول_ردیف_Len * state);


    نما     = RBBI_پیاپی;


    // loop until we reach the end of the text or transition to state 0
    //
    لكل (;;) {
        إذا (c == U_SENTINEL) {
            // Reached end of input string.
            إذا (نما == RBBI_انتها) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                اكسر;
            }
            // Run the loop one last time with the fake end-of-input character category.
            نما = RBBI_انتها;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        إذا (نما == RBBI_پیاپی) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_داده->fTrie, c);
            f_Dictionary_Char_شمار += (category >= dict_شروع);
        }

       #اگر_تعریف_شده RBBI_DEBUG
            إذا (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_شاخص(&f_متن));
                إذا (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #پایان_اگر

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_داده->f_سرآیند->f_Cat_شمار);
        state = ردیف->f_بعدی_State[category];  /*Not accessing beyond memory*/
        ردیف = (ردیف_Type *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (جدول_داده + جدول_ردیف_Len * state);


        uint16_t accepting = ردیف->fAccepting;
        إذا (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            إذا (نما != RBBI_شروع) {
                نتیجه = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            }
            f_قاعده_وضعیت_شاخص = ردیف->f_برچسبها_Idx;   // Remember the break status (tag) values.
        } وإلا إذا (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه);
            int32_t lookahead_نتیجه = fLookAheadMatches[accepting];
            إذا (lookahead_نتیجه >= 0) {
                f_قاعده_وضعیت_شاخص = ردیف->f_برچسبها_Idx;
                fPosition = lookahead_نتیجه;
                أرجع lookahead_نتیجه;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t قاعده = ردیف->fLookAhead;
        U_ASSERT(قاعده == 0 || قاعده > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(قاعده == 0 || قاعده < f_داده->f_Forward_جدول->f_Look_Ahead_Results_اندازه);
        إذا (قاعده > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            fLookAheadMatches[قاعده] = pos;
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
        إذا (نما == RBBI_پیاپی) {
            c = UTEXT_NEXT32(&f_متن);
        } وإلا {
            إذا (نما == RBBI_شروع) {
                نما = RBBI_پیاپی;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    إذا (نتیجه == initialPosition) {
        utext_مجموعه_Native_شاخص(&f_متن, initialPosition);
        utext_next32(&f_متن);
        نتیجه = static_cast<int32_t>(utext_get_Native_شاخص(&f_متن));
        f_قاعده_وضعیت_شاخص = 0;
    }

    // Leave the iterator at our result position.
    fPosition = نتیجه;
    #اگر_تعریف_شده RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", نتیجه);
        }
    #پایان_اگر
    أرجع نتیجه;
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
قالب <نام_نوع ردیف_Type, قاعده_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعده_Based_Break_Iterator::handle_امن_قبلی(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    ردیف_Type            *ردیف;
    UChar32             c;
    int32_t             نتیجه          = 0;

    ثابت RBBIState_جدول *state_جدول = f_داده->f_معکوس_جدول;
    UTEXT_SETNATIVEINDEX(&f_متن, fromPosition);
    #اگر_تعریف_شده RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #پایان_اگر

    // if we're already at the start of the text, return DONE.
    إذا (f_داده == اشاره_تهی || UTEXT_GETNATIVEINDEX(&f_متن)==0) {
        أرجع BreakIterator::انجامشده_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&f_متن);
    state = شروع_STATE;
    ردیف = (ردیف_Type *)
            (state_جدول->f_جدول_داده + (state_جدول->f_ردیف_Len * state));

    // loop until we reach the start of the text or transition to state 0
    //
    لكل (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&f_متن)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_داده->fTrie, c);

        #اگر_تعریف_شده RBBI_DEBUG
            إذا (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_شاخص(&f_متن));
                إذا (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #پایان_اگر

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_داده->f_سرآیند->f_Cat_شمار);
        state = ردیف->f_بعدی_State[category];  /*Not accessing beyond memory*/
        ردیف = (ردیف_Type *)
            (state_جدول->f_جدول_داده + (state_جدول->f_ردیف_Len * state));

        إذا (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            اكسر;
        }
    }

    // The state machine is done.  Check whether it found a match...
    نتیجه = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
    #اگر_تعریف_شده RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", نتیجه);
        }
    #پایان_اگر
    أرجع نتیجه;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  قاعده_Based_Break_Iterator::get_قاعده_وضعیت() ثابت {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_قاعده_وضعیت_شاخص + f_داده->f_قاعده_وضعیت_جدول[f_قاعده_وضعیت_شاخص];
    int32_t  برچسب_Val = f_داده->f_قاعده_وضعیت_جدول[idx];

    أرجع برچسب_Val;
}


int32_t قاعده_Based_Break_Iterator::get_قاعده_وضعیت_Vec(
             int32_t *fill_درون_Vec, int32_t capacity, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع 0;
    }

    int32_t  رقم_Vals = f_داده->f_قاعده_وضعیت_جدول[f_قاعده_وضعیت_شاخص];
    int32_t  رقم_Vals_مقصد_Copy = رقم_Vals;
    إذا (رقم_Vals > capacity) {
        وضعیت = U_میانگیر_سرریز_ERROR;
        رقم_Vals_مقصد_Copy = capacity;
    }
    صحيح i;
    لكل (i=0; i<رقم_Vals_مقصد_Copy; i++) {
        fill_درون_Vec[i] = f_داده->f_قاعده_وضعیت_جدول[f_قاعده_وضعیت_شاخص + i + 1];
    }
    أرجع رقم_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ثابت uint8_t* قاعده_Based_Break_Iterator::get_دودویی_قواعد(uint32_t& length) U_LIFETIME_BOUND {
    ثابت uint8_t  *retPtr = اشاره_تهی;
    length = 0;

    إذا (f_داده != اشاره_تهی) {
        retPtr = reinterpret_cast<ثابت uint8_t*>(f_داده->f_سرآیند);
        length = f_داده->f_سرآیند->f_درازی;
    }
    أرجع retPtr;
}


قاعده_Based_Break_Iterator *قاعده_Based_Break_Iterator::create_میانگیر_Clone(
        فراغ * /*stackBuffer*/, int32_t &میانگیر_اندازه, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)){
        أرجع اشاره_تهی;
    }

    إذا (میانگیر_اندازه == 0) {
        میانگیر_اندازه = 1;  // preflighting for deprecated functionality
        أرجع اشاره_تهی;
    }

    BreakIterator *clonedBI = clone();
    إذا (clonedBI == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
    } وإلا {
        وضعیت = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (قاعده_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_انتها


ساكن icu::UStack *gLanguageBreakFactories = اشاره_تهی;
ساكن ثابت icu::UnicodeString *g_خالی_String = اشاره_تهی;
ساكن icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ساكن icu::UInitOnce gRBBIInitOnce {};
ساكن icu::ICULanguageBreakFactory *gICULanguageBreakFactory = اشاره_تهی;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    حذف gLanguageBreakFactories;
    gLanguageBreakFactories = اشاره_تهی;
    حذف g_خالی_String;
    g_خالی_String = اشاره_تهی;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    أرجع درست;
}
U_CDECL_انتها

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV _deleteFactory(فراغ *obj) {
    حذف (icu::LanguageBreakFactory *) obj;
}
U_CDECL_انتها
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV rbbiInit() {
    g_خالی_String = جديد UnicodeString();
    ucln_common_ثبات_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ساكن فراغ U_CALLCONV initLanguageFactories(UError_کد& وضعیت) {
    U_ASSERT(gLanguageBreakFactories == اشاره_تهی);
    gLanguageBreakFactories = جديد UStack(_deleteFactory, اشاره_تهی, وضعیت);
    إذا (gLanguageBreakFactories != اشاره_تهی && U_SUCCESS(وضعیت)) {
        Local_اشاره_گر<ICULanguageBreakFactory> factory(جديد ICULanguageBreakFactory(وضعیت), وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, وضعیت);
#اگر_تعریف_شده U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &وضعیت);
            إذا (extra != اشاره_تهی) {
                gLanguageBreakFactories->push(extra, وضعیت);
            }
#پایان_اگر
        }
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

فراغ ensureLanguageFactories(UError_کد& وضعیت) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, وضعیت);
}

ساكن ثابت LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ثابت حرف* locale)
{
    UError_کد وضعیت = U_ZERO_ERROR;
    ensureLanguageFactories(وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع اشاره_تهی;

    int32_t i = gLanguageBreakFactories->size();
    ثابت LanguageBreakEngine *lbe = اشاره_تهی;
    طالما (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        إذا (lbe != اشاره_تهی) {
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
قاعده_Based_Break_Iterator::getLanguageBreakEngine(UChar32 c, ثابت حرف* locale) U_LIFETIME_BOUND {
    ثابت LanguageBreakEngine *lbe = اشاره_تهی;
    UError_کد وضعیت = U_ZERO_ERROR;

    إذا (fLanguageBreakEngines == اشاره_تهی) {
        fLanguageBreakEngines = جديد UStack(وضعیت);
        إذا (fLanguageBreakEngines == اشاره_تهی || U_FAILURE(وضعیت)) {
            حذف fLanguageBreakEngines;
            fLanguageBreakEngines = اشاره_تهی;
            أرجع اشاره_تهی;
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
    إذا (lbe != اشاره_تهی) {
        fLanguageBreakEngines->push((فراغ *)lbe, وضعیت);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        أرجع lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    إذا (fUnhandledBreakEngine == اشاره_تهی) {
        fUnhandledBreakEngine = جديد UnhandledEngine(وضعیت);
        إذا (U_SUCCESS(وضعیت) && fUnhandledBreakEngine == اشاره_تهی) {
            وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع اشاره_تهی;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->درج_Element_At(fUnhandledBreakEngine, 0, وضعیت);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        إذا (U_FAILURE(وضعیت)) {
            حذف fUnhandledBreakEngine;
            fUnhandledBreakEngine = اشاره_تهی;
            أرجع اشاره_تهی;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    أرجع fUnhandledBreakEngine;
}

#اگر_تعریف_نشده U_HIDE_DRAFT_API
فراغ U_EXPORT2 قاعده_Based_Break_Iterator::ثبات_External_Break_Engine(
                  ExternalBreakEngine* مقصد_Adopt, UError_کد& وضعیت) {
    Local_اشاره_گر<ExternalBreakEngine> engine(مقصد_Adopt, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع;
    ensureLanguageFactories(وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع;
    gICULanguageBreakFactory->جمع_کن_External_Engine(engine.orphan(), وضعیت);
}
#پایان_اگر  /* U_HIDE_DRAFT_API */


فراغ قاعده_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

فراغ قاعده_Based_Break_Iterator::dumpTables() {
    f_داده->print_داده();
}

/**
 * Returns the description used to create this iterator
 */

ثابت UnicodeString&
قاعده_Based_Break_Iterator::get_قواعد() ثابت U_LIFETIME_BOUND {
    إذا (f_داده != اشاره_تهی) {
        أرجع f_داده->get_قاعده_منبع_String();
    } وإلا {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        أرجع *g_خالی_String;
    }
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */

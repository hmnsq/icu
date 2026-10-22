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

#که_تعریف_شوی RBBI_DEBUG
ساكن UBool gTrace = خطأ;
#د_که_پای

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t پیل_STATE = 1;

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
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(RBBIData_سرلیک* معلومات, UError_کوډ &حالت)
 : قاعده_Based_Break_Iterator(&حالت)
{
    f_معلومات = جديد RBBIDataWrapper(معلومات, حالت); // status checked in constructor
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_معلومات == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    إذا (f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه * حجم(int32_t)));
        إذا (fLookAheadMatches == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
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
        UError_کوډ &حالت) : قاعده_Based_Break_Iterator(udm, حالت)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(ثابت uint8_t *compiled_قواعد,
                       uint32_t       قاعده_اوږدتیا,
                       UError_کوډ     &حالت)
 : قاعده_Based_Break_Iterator(&حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (compiled_قواعد == تش_اشاره || قاعده_اوږدتیا < حجم(RBBIData_سرلیک)) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    ثابت RBBIData_سرلیک* معلومات = reinterpret_cast<ثابت RBBIData_سرلیک*>(compiled_قواعد);
    إذا (معلومات->f_اوږدتیا > قاعده_اوږدتیا) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    f_معلومات = جديد RBBIDataWrapper(معلومات, RBBIDataWrapper::kDontAdopt, حالت);
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_معلومات == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    إذا (f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه * حجم(int32_t)));
        إذا (fLookAheadMatches == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
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
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(UData_حافظه* udm, UError_کوډ &حالت)
 : قاعده_Based_Break_Iterator(&حالت)
{
    f_معلومات = جديد RBBIDataWrapper(udm, حالت); // status checked in constructor
    إذا (U_FAILURE(حالت)) {أرجع;}
    إذا(f_معلومات == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    إذا (f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه * حجم(int32_t)));
        إذا (fLookAheadMatches == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
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
                                                UError_کوډ           &حالت)
 : قاعده_Based_Break_Iterator(&حالت)
{
    إذا (U_FAILURE(حالت)) {أرجع;}
    قاعده_Based_Break_Iterator *bi = (قاعده_Based_Break_Iterator *)
        RBBIRuleBuilder::create_قاعده_Based_Break_Iterator(قواعد, &parseError, حالت);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    إذا (U_SUCCESS(حالت)) {
        *هذا = *bi;
        ړنګول bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator()
 : قاعده_Based_Break_Iterator(تش_اشاره)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
قاعده_Based_Break_Iterator::قاعده_Based_Break_Iterator(UError_کوډ *حالت) {
    UError_کوډ ec = U_ZERO_ERROR;
    إذا (حالت == تش_اشاره) {
        حالت = &ec;
    }
    utext_openUChars(&f_متن, تش_اشاره, 0, حالت);
    Local_اشاره_ګر<DictionaryCache> lpDictionaryCache(جديد DictionaryCache(هذا, *حالت), *حالت);
    Local_اشاره_ګر<BreakCache> lpBreakCache(جديد BreakCache(هذا, *حالت), *حالت);
    إذا (U_FAILURE(*حالت)) {
        f_Error_کوډ = *حالت;
        أرجع;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#که_تعریف_شوی RBBI_DEBUG
    ساكن UBool debug_Init_بشپړ = خطأ;
    إذا (debug_Init_بشپړ == خطأ) {
        حرف *debugEnv = getenv("U_RBBIDEBUG");
        إذا (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = ریښتیا;
        }
        debug_Init_بشپړ = ریښتیا;
    }
#د_که_پای
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
        ړنګول fCharIter;
    }
    fCharIter = تش_اشاره;

    utext_close(&f_متن);

    إذا (f_معلومات != تش_اشاره) {
        f_معلومات->removeReference();
        f_معلومات = تش_اشاره;
    }
    ړنګول fBreakCache;
    fBreakCache = تش_اشاره;

    ړنګول fDictionaryCache;
    fDictionaryCache = تش_اشاره;

    ړنګول fLanguageBreakEngines;
    fLanguageBreakEngines = تش_اشاره;

    ړنګول fUnhandledBreakEngine;
    fUnhandledBreakEngine = تش_اشاره;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = تش_اشاره;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
قاعده_Based_Break_Iterator&
قاعده_Based_Break_Iterator::چلوونکی=(ثابت قاعده_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    إذا (هذا == &that) {
        أرجع *هذا;
    }
    BreakIterator::چلوونکی=(that);

    إذا (fLanguageBreakEngines != تش_اشاره) {
        ړنګول fLanguageBreakEngines;
        fLanguageBreakEngines = تش_اشاره;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_کوډ حالت = U_ZERO_ERROR;
    utext_clone(&f_متن, &that.f_متن, خطأ, ریښتیا, &حالت);

    إذا (fCharIter != &fSCharIter) {
        ړنګول fCharIter;
    }
    fCharIter = &fSCharIter;

    إذا (that.fCharIter != تش_اشاره && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    إذا (fCharIter == تش_اشاره) {
        fCharIter = &fSCharIter;
    }

    إذا (f_معلومات != تش_اشاره) {
        f_معلومات->removeReference();
        f_معلومات = تش_اشاره;
    }
    إذا (that.f_معلومات != تش_اشاره) {
        f_معلومات = that.f_معلومات->جمع_کړه_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = تش_اشاره;
    إذا (f_معلومات && f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه * حجم(int32_t)));
    }


    fPosition = that.fPosition;
    f_قاعده_حالت_شاخص = that.f_قاعده_حالت_شاخص;
    f_بشپړ = that.f_بشپړ;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_قاعده_حالت_شاخص);
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
قاعده_Based_Break_Iterator::چلوونکی==(ثابت BreakIterator& that) ثابت {
    إذا (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }
    إذا (هذا == &that) {
        أرجع ریښتیا;
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
            f_قاعده_حالت_شاخص == that2.f_قاعده_حالت_شاخص &&
            f_بشپړ == that2.f_بشپړ &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        أرجع خطأ;
    }

    إذا (that2.f_معلومات == f_معلومات ||
        (f_معلومات != تش_اشاره && that2.f_معلومات != تش_اشاره && *that2.f_معلومات == *f_معلومات)) {
            // The two break iterators are using the same rules.
            أرجع ریښتیا;
        }
    أرجع خطأ;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
قاعده_Based_Break_Iterator::hash_کوډ() ثابت {
    int32_t   hash = 0;
    إذا (f_معلومات != تش_اشاره) {
        hash = f_معلومات->hash_کوډ();
    }
    أرجع hash;
}


فراغ قاعده_Based_Break_Iterator::سېټ_متن(UText *ut, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&f_متن, ut, خطأ, ریښتیا, &حالت);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.سېټ_متن(u"", 0);

    إذا (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        ړنګول fCharIter;
    }
    fCharIter = &fSCharIter;

    هذا->first();
}


UText* قاعده_Based_Break_Iterator::getUText(UText* fill_دننه U_LIFETIME_BOUND, UError_کوډ& حالت) ثابت {
    UText *پایله = utext_clone(fill_دننه, &f_متن, خطأ, ریښتیا, &حالت);
    أرجع پایله;
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
        ړنګول fCharIter;
    }

    fCharIter = new_متن;
    UError_کوډ حالت = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    إذا (new_متن==تش_اشاره || new_متن->پیل_شاخص() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&f_متن, تش_اشاره, 0, &حالت);
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
قاعده_Based_Break_Iterator::سېټ_متن(ثابت UnicodeString& new_متن) {
    UError_کوډ حالت = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&f_متن, &new_متن, &حالت);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.سېټ_متن(new_متن.get_بفر(), new_متن.length());

    إذا (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        ړنګول fCharIter;
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
قاعده_Based_Break_Iterator::refresh_ننوت_متن(UText* input, UError_کوډ& حالت) U_LIFETIME_BOUND {
    إذا (U_FAILURE(حالت)) {
        أرجع *هذا;
    }
    إذا (input == تش_اشاره) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    int64_t pos = utext_get_Native_شاخص(&f_متن);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&f_متن, input, خطأ, ریښتیا, &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع *هذا;
    }
    utext_سېټ_Native_شاخص(&f_متن, pos);
    إذا (utext_get_Native_شاخص(&f_متن) != pos) {
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
int32_t قاعده_Based_Break_Iterator::first() {
    UError_کوډ حالت = U_ZERO_ERROR;
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
int32_t قاعده_Based_Break_Iterator::وروستی() {
    int32_t پای_ټکی_Pos = static_cast<int32_t>(utext_native_اوږدتیا(&f_متن));
    UBool پای_ټکی_Should_Be_Boundary = isBoundary(پای_ټکی_Pos);      // Has side effect of setting iterator position.
    (فراغ)پای_ټکی_Should_Be_Boundary;
    U_ASSERT(پای_ټکی_Should_Be_Boundary);
    U_ASSERT(fPosition == پای_ټکی_Pos);
    أرجع پای_ټکی_Pos;
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
    int32_t پایله = 0;
    إذا (n > 0) {
        لكل (; n > 0 && پایله != UBRK_بشپړ; --n) {
            پایله = next();
        }
    } وإلا إذا (n < 0) {
        لكل (; n < 0 && پایله != UBRK_بشپړ; ++n) {
            پایله = پخوانی();
        }
    } وإلا {
        پایله = current();
    }
    أرجع پایله;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t قاعده_Based_Break_Iterator::next() {
    fBreakCache->next();
    أرجع f_بشپړ ? UBRK_بشپړ : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t قاعده_Based_Break_Iterator::پخوانی() {
    UError_کوډ حالت = U_ZERO_ERROR;
    fBreakCache->پخوانی(حالت);
    أرجع f_بشپړ ? UBRK_بشپړ : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t قاعده_Based_Break_Iterator::following(int32_t پیل_Pos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    إذا (پیل_Pos < 0) {
        أرجع first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_سېټ_Native_شاخص(&f_متن, پیل_Pos);
    پیل_Pos = static_cast<int32_t>(utext_get_Native_شاخص(&f_متن));

    UError_کوډ حالت = U_ZERO_ERROR;
    fBreakCache->following(پیل_Pos, حالت);
    أرجع f_بشپړ ? UBRK_بشپړ : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t قاعده_Based_Break_Iterator::preceding(int32_t دننه_واټن) {
    إذا (دننه_واټن > utext_native_اوږدتیا(&f_متن)) {
        أرجع وروستی();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_سېټ_Native_شاخص(&f_متن, دننه_واټن);
    int32_t adjusted_دننه_واټن = static_cast<int32_t>(utext_get_Native_شاخص(&f_متن));

    UError_کوډ حالت = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_دننه_واټن, حالت);
    أرجع f_بشپړ ? UBRK_بشپړ : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool قاعده_Based_Break_Iterator::isBoundary(int32_t دننه_واټن) {
    // out-of-range indexes are never boundary positions
    إذا (دننه_واټن < 0) {
        first();       // For side effects on current position, tag values.
        أرجع خطأ;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_سېټ_Native_شاخص(&f_متن, دننه_واټن);
    int32_t adjusted_دننه_واټن = static_cast<int32_t>(utext_get_Native_شاخص(&f_متن));

    منطقي پایله = خطأ;
    UError_کوډ حالت = U_ZERO_ERROR;
    إذا (fBreakCache->seek(adjusted_دننه_واټن) || fBreakCache->populateNear(adjusted_دننه_واټن, حالت)) {
        پایله = (fBreakCache->current() == دننه_واټن);
    }

    إذا (پایله && adjusted_دننه_واټن < دننه_واټن && utext_char32At(&f_متن, دننه_واټن) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        أرجع خطأ;
    }
    إذا (!پایله) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    أرجع پایله;
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
تعداد RBBIRun_ډېرپېښ {
    RBBI_پیل,     // state machine processing is before first char of input
    RBBI_پرلپسې,       // state machine processing is in the user text
    RBBI_پای_ټکی        // state machine processing is after end of user text.
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

int32_t قاعده_Based_Break_Iterator::handle_بل() {
    ثابت RBBIState_جدول *statetable = f_معلومات->f_Forward_جدول;
    منطقي use8_بټونه_Trie = ucptrie_get_ارزښت_پلنوالی(f_معلومات->fTrie) == UCPTRIE_ارزښت_بټونه_8;
    إذا (statetable->fFlags & RBBI_8_بټونه_لیکې) {
        إذا (use8_بټونه_Trie) {
            أرجع handle_بل<RBBIState_جدول_Row8, TrieFunc8>();
        } وإلا {
            أرجع handle_بل<RBBIState_جدول_Row8, TrieFunc16>();
        }
    } وإلا {
        إذا (use8_بټونه_Trie) {
            أرجع handle_بل<RBBIState_جدول_Row16, TrieFunc8>();
        } وإلا {
            أرجع handle_بل<RBBIState_جدول_Row16, TrieFunc16>();
        }
    }
}

int32_t قاعده_Based_Break_Iterator::handle_خوندي_پخوانی(int32_t fromPosition) {
    ثابت RBBIState_جدول *statetable = f_معلومات->f_سرچپه_جدول;
    منطقي use8_بټونه_Trie = ucptrie_get_ارزښت_پلنوالی(f_معلومات->fTrie) == UCPTRIE_ارزښت_بټونه_8;
    إذا (statetable->fFlags & RBBI_8_بټونه_لیکې) {
        إذا (use8_بټونه_Trie) {
            أرجع handle_خوندي_پخوانی<RBBIState_جدول_Row8, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handle_خوندي_پخوانی<RBBIState_جدول_Row8, TrieFunc16>(fromPosition);
        }
    } وإلا {
        إذا (use8_بټونه_Trie) {
            أرجع handle_خوندي_پخوانی<RBBIState_جدول_Row16, TrieFunc8>(fromPosition);
        } وإلا {
            أرجع handle_خوندي_پخوانی<RBBIState_جدول_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
کينډۍ <ډول_نوم لیکه_Type, قاعده_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعده_Based_Break_Iterator::handle_بل() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_ډېرپېښ         ډېرپېښ;

    لیکه_Type             *لیکه;
    UChar32             c;
    int32_t             پایله             = 0;
    int32_t             initialPosition    = 0;
    ثابت RBBIState_جدول *statetable       = f_معلومات->f_Forward_جدول;
    ثابت حرف         *جدول_معلومات          = statetable->f_جدول_معلومات;
    uint32_t            جدول_لیکه_Len        = statetable->f_لیکه_Len;
    uint32_t            dict_پیل          = statetable->f_Dict_Categories_پیل;
    #که_تعریف_شوی RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #د_که_پای

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_قاعده_حالت_شاخص = 0;

    f_Dictionary_Char_شمېر = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&f_متن, initialPosition);
    پایله          = initialPosition;
    c               = UTEXT_NEXT32(&f_متن);
    إذا (c==U_SENTINEL) {
        f_بشپړ = ریښتیا;
        أرجع UBRK_بشپړ;
    }

    //  Set the initial state for the state machine
    state = پیل_STATE;
    لیکه = (لیکه_Type *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (جدول_معلومات + جدول_لیکه_Len * state);


    ډېرپېښ     = RBBI_پرلپسې;


    // loop until we reach the end of the text or transition to state 0
    //
    لكل (;;) {
        إذا (c == U_SENTINEL) {
            // Reached end of input string.
            إذا (ډېرپېښ == RBBI_پای_ټکی) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                اكسر;
            }
            // Run the loop one last time with the fake end-of-input character category.
            ډېرپېښ = RBBI_پای_ټکی;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        إذا (ډېرپېښ == RBBI_پرلپسې) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_معلومات->fTrie, c);
            f_Dictionary_Char_شمېر += (category >= dict_پیل);
        }

       #که_تعریف_شوی RBBI_DEBUG
            إذا (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_شاخص(&f_متن));
                إذا (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #د_که_پای

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_معلومات->f_سرلیک->f_Cat_شمېر);
        state = لیکه->f_بل_State[category];  /*Not accessing beyond memory*/
        لیکه = (لیکه_Type *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (جدول_معلومات + جدول_لیکه_Len * state);


        uint16_t accepting = لیکه->fAccepting;
        إذا (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            إذا (ډېرپېښ != RBBI_پیل) {
                پایله = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
            }
            f_قاعده_حالت_شاخص = لیکه->f_نښانونه_Idx;   // Remember the break status (tag) values.
        } وإلا إذا (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه);
            int32_t lookahead_پایله = fLookAheadMatches[accepting];
            إذا (lookahead_پایله >= 0) {
                f_قاعده_حالت_شاخص = لیکه->f_نښانونه_Idx;
                fPosition = lookahead_پایله;
                أرجع lookahead_پایله;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t قاعده = لیکه->fLookAhead;
        U_ASSERT(قاعده == 0 || قاعده > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(قاعده == 0 || قاعده < f_معلومات->f_Forward_جدول->f_Look_Ahead_Results_اندازه);
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
        إذا (ډېرپېښ == RBBI_پرلپسې) {
            c = UTEXT_NEXT32(&f_متن);
        } وإلا {
            إذا (ډېرپېښ == RBBI_پیل) {
                ډېرپېښ = RBBI_پرلپسې;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    إذا (پایله == initialPosition) {
        utext_سېټ_Native_شاخص(&f_متن, initialPosition);
        utext_next32(&f_متن);
        پایله = static_cast<int32_t>(utext_get_Native_شاخص(&f_متن));
        f_قاعده_حالت_شاخص = 0;
    }

    // Leave the iterator at our result position.
    fPosition = پایله;
    #که_تعریف_شوی RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", پایله);
        }
    #د_که_پای
    أرجع پایله;
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
کينډۍ <ډول_نوم لیکه_Type, قاعده_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t قاعده_Based_Break_Iterator::handle_خوندي_پخوانی(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    لیکه_Type            *لیکه;
    UChar32             c;
    int32_t             پایله          = 0;

    ثابت RBBIState_جدول *state_جدول = f_معلومات->f_سرچپه_جدول;
    UTEXT_SETNATIVEINDEX(&f_متن, fromPosition);
    #که_تعریف_شوی RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #د_که_پای

    // if we're already at the start of the text, return DONE.
    إذا (f_معلومات == تش_اشاره || UTEXT_GETNATIVEINDEX(&f_متن)==0) {
        أرجع BreakIterator::بشپړ_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&f_متن);
    state = پیل_STATE;
    لیکه = (لیکه_Type *)
            (state_جدول->f_جدول_معلومات + (state_جدول->f_لیکه_Len * state));

    // loop until we reach the start of the text or transition to state 0
    //
    لكل (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&f_متن)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_معلومات->fTrie, c);

        #که_تعریف_شوی RBBI_DEBUG
            إذا (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_شاخص(&f_متن));
                إذا (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } وإلا {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #د_که_پای

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_معلومات->f_سرلیک->f_Cat_شمېر);
        state = لیکه->f_بل_State[category];  /*Not accessing beyond memory*/
        لیکه = (لیکه_Type *)
            (state_جدول->f_جدول_معلومات + (state_جدول->f_لیکه_Len * state));

        إذا (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            اكسر;
        }
    }

    // The state machine is done.  Check whether it found a match...
    پایله = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_متن));
    #که_تعریف_شوی RBBI_DEBUG
        إذا (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", پایله);
        }
    #د_که_پای
    أرجع پایله;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  قاعده_Based_Break_Iterator::get_قاعده_حالت() ثابت {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_قاعده_حالت_شاخص + f_معلومات->f_قاعده_حالت_جدول[f_قاعده_حالت_شاخص];
    int32_t  نښان_Val = f_معلومات->f_قاعده_حالت_جدول[idx];

    أرجع نښان_Val;
}


int32_t قاعده_Based_Break_Iterator::get_قاعده_حالت_Vec(
             int32_t *fill_دننه_Vec, int32_t capacity, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }

    int32_t  عدد_Vals = f_معلومات->f_قاعده_حالت_جدول[f_قاعده_حالت_شاخص];
    int32_t  عدد_Vals_منزل_Copy = عدد_Vals;
    إذا (عدد_Vals > capacity) {
        حالت = U_بفر_ډکېدنه_ERROR;
        عدد_Vals_منزل_Copy = capacity;
    }
    صحيح i;
    لكل (i=0; i<عدد_Vals_منزل_Copy; i++) {
        fill_دننه_Vec[i] = f_معلومات->f_قاعده_حالت_جدول[f_قاعده_حالت_شاخص + i + 1];
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
ثابت uint8_t* قاعده_Based_Break_Iterator::get_دوګونی_قواعد(uint32_t& length) U_LIFETIME_BOUND {
    ثابت uint8_t  *retPtr = تش_اشاره;
    length = 0;

    إذا (f_معلومات != تش_اشاره) {
        retPtr = reinterpret_cast<ثابت uint8_t*>(f_معلومات->f_سرلیک);
        length = f_معلومات->f_سرلیک->f_اوږدتیا;
    }
    أرجع retPtr;
}


قاعده_Based_Break_Iterator *قاعده_Based_Break_Iterator::create_بفر_Clone(
        فراغ * /*stackBuffer*/, int32_t &بفر_اندازه, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)){
        أرجع تش_اشاره;
    }

    إذا (بفر_اندازه == 0) {
        بفر_اندازه = 1;  // preflighting for deprecated functionality
        أرجع تش_اشاره;
    }

    BreakIterator *clonedBI = clone();
    إذا (clonedBI == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
    } وإلا {
        حالت = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (قاعده_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_پای_ټکی


ساكن icu::UStack *gLanguageBreakFactories = تش_اشاره;
ساكن ثابت icu::UnicodeString *g_تشه_String = تش_اشاره;
ساكن icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ساكن icu::UInitOnce gRBBIInitOnce {};
ساكن icu::ICULanguageBreakFactory *gICULanguageBreakFactory = تش_اشاره;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    ړنګول gLanguageBreakFactories;
    gLanguageBreakFactories = تش_اشاره;
    ړنګول g_تشه_String;
    g_تشه_String = تش_اشاره;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    أرجع ریښتیا;
}
U_CDECL_پای_ټکی

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV _deleteFactory(فراغ *obj) {
    ړنګول (icu::LanguageBreakFactory *) obj;
}
U_CDECL_پای_ټکی
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV rbbiInit() {
    g_تشه_String = جديد UnicodeString();
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ساكن فراغ U_CALLCONV initLanguageFactories(UError_کوډ& حالت) {
    U_ASSERT(gLanguageBreakFactories == تش_اشاره);
    gLanguageBreakFactories = جديد UStack(_deleteFactory, تش_اشاره, حالت);
    إذا (gLanguageBreakFactories != تش_اشاره && U_SUCCESS(حالت)) {
        Local_اشاره_ګر<ICULanguageBreakFactory> factory(جديد ICULanguageBreakFactory(حالت), حالت);
        إذا (U_SUCCESS(حالت)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, حالت);
#که_تعریف_شوی U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &حالت);
            إذا (extra != تش_اشاره) {
                gLanguageBreakFactories->push(extra, حالت);
            }
#د_که_پای
        }
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

فراغ ensureLanguageFactories(UError_کوډ& حالت) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, حالت);
}

ساكن ثابت LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ثابت حرف* locale)
{
    UError_کوډ حالت = U_ZERO_ERROR;
    ensureLanguageFactories(حالت);
    إذا (U_FAILURE(حالت)) أرجع تش_اشاره;

    int32_t i = gLanguageBreakFactories->size();
    ثابت LanguageBreakEngine *lbe = تش_اشاره;
    طالما (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        إذا (lbe != تش_اشاره) {
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
    ثابت LanguageBreakEngine *lbe = تش_اشاره;
    UError_کوډ حالت = U_ZERO_ERROR;

    إذا (fLanguageBreakEngines == تش_اشاره) {
        fLanguageBreakEngines = جديد UStack(حالت);
        إذا (fLanguageBreakEngines == تش_اشاره || U_FAILURE(حالت)) {
            ړنګول fLanguageBreakEngines;
            fLanguageBreakEngines = تش_اشاره;
            أرجع تش_اشاره;
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
    إذا (lbe != تش_اشاره) {
        fLanguageBreakEngines->push((فراغ *)lbe, حالت);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        أرجع lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    إذا (fUnhandledBreakEngine == تش_اشاره) {
        fUnhandledBreakEngine = جديد UnhandledEngine(حالت);
        إذا (U_SUCCESS(حالت) && fUnhandledBreakEngine == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->ورننباسه_Element_At(fUnhandledBreakEngine, 0, حالت);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        إذا (U_FAILURE(حالت)) {
            ړنګول fUnhandledBreakEngine;
            fUnhandledBreakEngine = تش_اشاره;
            أرجع تش_اشاره;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    أرجع fUnhandledBreakEngine;
}

#که_نه_تعریف_شوی U_HIDE_DRAFT_API
فراغ U_EXPORT2 قاعده_Based_Break_Iterator::ثبتوونکی_External_Break_Engine(
                  ExternalBreakEngine* منزل_Adopt, UError_کوډ& حالت) {
    Local_اشاره_ګر<ExternalBreakEngine> engine(منزل_Adopt, حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    ensureLanguageFactories(حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    gICULanguageBreakFactory->جمع_کړه_External_Engine(engine.orphan(), حالت);
}
#د_که_پای  /* U_HIDE_DRAFT_API */


فراغ قاعده_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

فراغ قاعده_Based_Break_Iterator::dumpTables() {
    f_معلومات->print_معلومات();
}

/**
 * Returns the description used to create this iterator
 */

ثابت UnicodeString&
قاعده_Based_Break_Iterator::get_قواعد() ثابت U_LIFETIME_BOUND {
    إذا (f_معلومات != تش_اشاره) {
        أرجع f_معلومات->get_قاعده_سرچینه_String();
    } وإلا {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        أرجع *g_تشه_String;
    }
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */

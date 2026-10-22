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

#ਅਨ੍ਤਰ੍ਭਾਵ "utypeinfo.h"  // for 'typeid' to work

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ <cinttypes>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rbbi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/schriter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchriter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uclean.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "brkeng.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "localsvc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbidata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbi_cache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਸਥਿਰ UBool gTrace = ਝੂਠ;
#ਜੇ_ਅੰਤ

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t ਸ਼ੁਰੂ_STATE = 1;

// The state-transition value indicating "stop"
constexpr int32_t STOP_STATE = 0;


UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ਨਿਯਮ_Based_Break_Iterator)


//=======================================================================
// constructors
//=======================================================================

/**
 * Constructs a RuleBasedBreakIterator that uses the already-created
 * tables object that is passed in as a parameter.
 */
ਨਿਯਮ_Based_Break_Iterator::ਨਿਯਮ_Based_Break_Iterator(RBBIData_ਸਿਰਲੇਖ* ਅੰਕੜੇ, UError_ਕੋਡ &ਸਥਿਤੀ)
 : ਨਿਯਮ_Based_Break_Iterator(&ਸਥਿਤੀ)
{
    f_ਅੰਕੜੇ = ਨਵਾਂ RBBIDataWrapper(ਅੰਕੜੇ, ਸਥਿਤੀ); // status checked in constructor
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {ਵਾਪਸ;}
    ਜੇ(f_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ * ਆਕਾਰ(int32_t)));
        ਜੇ (fLookAheadMatches == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
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
ਨਿਯਮ_Based_Break_Iterator::ਨਿਯਮ_Based_Break_Iterator(UData_ਸਮ੍ਰਿਤੀ* udm, UBool isPhraseBreaking,
        UError_ਕੋਡ &ਸਥਿਤੀ) : ਨਿਯਮ_Based_Break_Iterator(udm, ਸਥਿਤੀ)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
ਨਿਯਮ_Based_Break_Iterator::ਨਿਯਮ_Based_Break_Iterator(ਅਚਲ uint8_t *compiled_ਨਿਯਮਾਂ,
                       uint32_t       ਨਿਯਮ_ਲੰਮਾਈ,
                       UError_ਕੋਡ     &ਸਥਿਤੀ)
 : ਨਿਯਮ_Based_Break_Iterator(&ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (compiled_ਨਿਯਮਾਂ == ਖਾਲੀ_ਸੂਚਕ || ਨਿਯਮ_ਲੰਮਾਈ < ਆਕਾਰ(RBBIData_ਸਿਰਲੇਖ)) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਅਚਲ RBBIData_ਸਿਰਲੇਖ* ਅੰਕੜੇ = reinterpret_cast<ਅਚਲ RBBIData_ਸਿਰਲੇਖ*>(compiled_ਨਿਯਮਾਂ);
    ਜੇ (ਅੰਕੜੇ->f_ਲੰਮਾਈ > ਨਿਯਮ_ਲੰਮਾਈ) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    f_ਅੰਕੜੇ = ਨਵਾਂ RBBIDataWrapper(ਅੰਕੜੇ, RBBIDataWrapper::kDontAdopt, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {ਵਾਪਸ;}
    ਜੇ(f_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ * ਆਕਾਰ(int32_t)));
        ਜੇ (fLookAheadMatches == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
    }
}


//-------------------------------------------------------------------------------
//
//   Constructor   from a UDataMemory handle to precompiled break rules
//                 stored in an ICU data file.
//
//-------------------------------------------------------------------------------
ਨਿਯਮ_Based_Break_Iterator::ਨਿਯਮ_Based_Break_Iterator(UData_ਸਮ੍ਰਿਤੀ* udm, UError_ਕੋਡ &ਸਥਿਤੀ)
 : ਨਿਯਮ_Based_Break_Iterator(&ਸਥਿਤੀ)
{
    f_ਅੰਕੜੇ = ਨਵਾਂ RBBIDataWrapper(udm, ਸਥਿਤੀ); // status checked in constructor
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {ਵਾਪਸ;}
    ਜੇ(f_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ * ਆਕਾਰ(int32_t)));
        ਜੇ (fLookAheadMatches == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
ਨਿਯਮ_Based_Break_Iterator::ਨਿਯਮ_Based_Break_Iterator( ਅਚਲ UnicodeString  &ਨਿਯਮਾਂ,
                                                UParseError          &parseError,
                                                UError_ਕੋਡ           &ਸਥਿਤੀ)
 : ਨਿਯਮ_Based_Break_Iterator(&ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {ਵਾਪਸ;}
    ਨਿਯਮ_Based_Break_Iterator *bi = (ਨਿਯਮ_Based_Break_Iterator *)
        RBBIRuleBuilder::create_ਨਿਯਮ_Based_Break_Iterator(ਨਿਯਮਾਂ, &parseError, ਸਥਿਤੀ);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        *ਇਹ = *bi;
        ਮਿਟਾਓ bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
ਨਿਯਮ_Based_Break_Iterator::ਨਿਯਮ_Based_Break_Iterator()
 : ਨਿਯਮ_Based_Break_Iterator(ਖਾਲੀ_ਸੂਚਕ)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
ਨਿਯਮ_Based_Break_Iterator::ਨਿਯਮ_Based_Break_Iterator(UError_ਕੋਡ *ਸਥਿਤੀ) {
    UError_ਕੋਡ ec = U_ZERO_ERROR;
    ਜੇ (ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = &ec;
    }
    utext_openUChars(&f_ਪਾਠ, ਖਾਲੀ_ਸੂਚਕ, 0, ਸਥਿਤੀ);
    Local_ਸੂਚਕ<DictionaryCache> lpDictionaryCache(ਨਵਾਂ DictionaryCache(ਇਹ, *ਸਥਿਤੀ), *ਸਥਿਤੀ);
    Local_ਸੂਚਕ<BreakCache> lpBreakCache(ਨਵਾਂ BreakCache(ਇਹ, *ਸਥਿਤੀ), *ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        f_Error_ਕੋਡ = *ਸਥਿਤੀ;
        ਵਾਪਸ;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਸਥਿਰ UBool debug_Init_ਪੂਰਾ = ਝੂਠ;
    ਜੇ (debug_Init_ਪੂਰਾ == ਝੂਠ) {
        ਅੱਖਰ *debugEnv = getenv("U_RBBIDEBUG");
        ਜੇ (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = ਸੱਚ;
        }
        debug_Init_ਪੂਰਾ = ਸੱਚ;
    }
#ਜੇ_ਅੰਤ
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
ਨਿਯਮ_Based_Break_Iterator::ਨਿਯਮ_Based_Break_Iterator(ਅਚਲ ਨਿਯਮ_Based_Break_Iterator& other)
: ਨਿਯਮ_Based_Break_Iterator()
{
    *ਇਹ = other;
}


/**
 * Destructor
 */
ਨਿਯਮ_Based_Break_Iterator::~ਨਿਯਮ_Based_Break_Iterator() {
    ਜੇ (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        ਮਿਟਾਓ fCharIter;
    }
    fCharIter = ਖਾਲੀ_ਸੂਚਕ;

    utext_close(&f_ਪਾਠ);

    ਜੇ (f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        f_ਅੰਕੜੇ->removeReference();
        f_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਮਿਟਾਓ fBreakCache;
    fBreakCache = ਖਾਲੀ_ਸੂਚਕ;

    ਮਿਟਾਓ fDictionaryCache;
    fDictionaryCache = ਖਾਲੀ_ਸੂਚਕ;

    ਮਿਟਾਓ fLanguageBreakEngines;
    fLanguageBreakEngines = ਖਾਲੀ_ਸੂਚਕ;

    ਮਿਟਾਓ fUnhandledBreakEngine;
    fUnhandledBreakEngine = ਖਾਲੀ_ਸੂਚਕ;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = ਖਾਲੀ_ਸੂਚਕ;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
ਨਿਯਮ_Based_Break_Iterator&
ਨਿਯਮ_Based_Break_Iterator::ਸੰਚਾਲਕ=(ਅਚਲ ਨਿਯਮ_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    ਜੇ (ਇਹ == &that) {
        ਵਾਪਸ *ਇਹ;
    }
    BreakIterator::ਸੰਚਾਲਕ=(that);

    ਜੇ (fLanguageBreakEngines != ਖਾਲੀ_ਸੂਚਕ) {
        ਮਿਟਾਓ fLanguageBreakEngines;
        fLanguageBreakEngines = ਖਾਲੀ_ਸੂਚਕ;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    utext_clone(&f_ਪਾਠ, &that.f_ਪਾਠ, ਝੂਠ, ਸੱਚ, &ਸਥਿਤੀ);

    ਜੇ (fCharIter != &fSCharIter) {
        ਮਿਟਾਓ fCharIter;
    }
    fCharIter = &fSCharIter;

    ਜੇ (that.fCharIter != ਖਾਲੀ_ਸੂਚਕ && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    ਜੇ (fCharIter == ਖਾਲੀ_ਸੂਚਕ) {
        fCharIter = &fSCharIter;
    }

    ਜੇ (f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        f_ਅੰਕੜੇ->removeReference();
        f_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (that.f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        f_ਅੰਕੜੇ = that.f_ਅੰਕੜੇ->ਜੋੜੋ_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (f_ਅੰਕੜੇ && f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ * ਆਕਾਰ(int32_t)));
    }


    fPosition = that.fPosition;
    f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = that.f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ;
    f_ਪੂਰਾ = that.f_ਪੂਰਾ;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ);
    fDictionaryCache->reset();

    ਵਾਪਸ *ਇਹ;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
ਨਿਯਮ_Based_Break_Iterator*
ਨਿਯਮ_Based_Break_Iterator::clone() ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ ਨਿਯਮ_Based_Break_Iterator(*ਇਹ);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
ਤਾਰਕਿਕ
ਨਿਯਮ_Based_Break_Iterator::ਸੰਚਾਲਕ==(ਅਚਲ BreakIterator& that) ਅਚਲ {
    ਜੇ (typeid(*ਇਹ) != typeid(that)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (ਇਹ == &that) {
        ਵਾਪਸ ਸੱਚ;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ਅਚਲ ਨਿਯਮ_Based_Break_Iterator& that2 = static_cast<ਅਚਲ ਨਿਯਮ_Based_Break_Iterator&>(that);

    ਜੇ (!utext_equals(&f_ਪਾਠ, &that2.f_ਪਾਠ)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ (!(fPosition == that2.fPosition &&
            f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ == that2.f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ &&
            f_ਪੂਰਾ == that2.f_ਪੂਰਾ &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ (that2.f_ਅੰਕੜੇ == f_ਅੰਕੜੇ ||
        (f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ && that2.f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ && *that2.f_ਅੰਕੜੇ == *f_ਅੰਕੜੇ)) {
            // The two break iterators are using the same rules.
            ਵਾਪਸ ਸੱਚ;
        }
    ਵਾਪਸ ਝੂਠ;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
ਨਿਯਮ_Based_Break_Iterator::hash_ਕੋਡ() ਅਚਲ {
    int32_t   hash = 0;
    ਜੇ (f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        hash = f_ਅੰਕੜੇ->hash_ਕੋਡ();
    }
    ਵਾਪਸ hash;
}


ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::ਸੈੱਟ_ਪਾਠ(UText *ut, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&f_ਪਾਠ, ut, ਝੂਠ, ਸੱਚ, &ਸਥਿਤੀ);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.ਸੈੱਟ_ਪਾਠ(u"", 0);

    ਜੇ (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        ਮਿਟਾਓ fCharIter;
    }
    fCharIter = &fSCharIter;

    ਇਹ->first();
}


UText* ਨਿਯਮ_Based_Break_Iterator::getUText(UText* fill_ਵਿੱਚ U_LIFETIME_BOUND, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    UText *ਨਤੀਜਾ = utext_clone(fill_ਵਿੱਚ, &f_ਪਾਠ, ਝੂਠ, ਸੱਚ, &ਸਥਿਤੀ);
    ਵਾਪਸ ਨਤੀਜਾ;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
ਨਿਯਮ_Based_Break_Iterator::get_ਪਾਠ() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
ਖਾਲੀ
ਨਿਯਮ_Based_Break_Iterator::adopt_ਪਾਠ(CharacterIterator* new_ਪਾਠ) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    ਜੇ (fCharIter != &fSCharIter) {
        ਮਿਟਾਓ fCharIter;
    }

    fCharIter = new_ਪਾਠ;
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    ਜੇ (new_ਪਾਠ==ਖਾਲੀ_ਸੂਚਕ || new_ਪਾਠ->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&f_ਪਾਠ, ਖਾਲੀ_ਸੂਚਕ, 0, &ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        utext_openCharacterIterator(&f_ਪਾਠ, new_ਪਾਠ, &ਸਥਿਤੀ);
    }
    ਇਹ->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
ਖਾਲੀ
ਨਿਯਮ_Based_Break_Iterator::ਸੈੱਟ_ਪਾਠ(ਅਚਲ UnicodeString& new_ਪਾਠ) {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&f_ਪਾਠ, &new_ਪਾਠ, &ਸਥਿਤੀ);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.ਸੈੱਟ_ਪਾਠ(new_ਪਾਠ.get_ਬਫਰ(), new_ਪਾਠ.length());

    ਜੇ (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        ਮਿਟਾਓ fCharIter;
    }
    fCharIter = &fSCharIter;

    ਇਹ->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
ਨਿਯਮ_Based_Break_Iterator&
ਨਿਯਮ_Based_Break_Iterator::refresh_ਨਿਵੇਸ਼_ਪਾਠ(UText* input, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (input == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    int64_t pos = utext_get_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&f_ਪਾਠ, input, ਝੂਠ, ਸੱਚ, &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ *ਇਹ;
    }
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ, pos);
    ਜੇ (utext_get_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਵਾਪਸ *ਇਹ;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t ਨਿਯਮ_Based_Break_Iterator::first() {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ਜੇ (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, ਸਥਿਤੀ);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    ਵਾਪਸ 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t ਨਿਯਮ_Based_Break_Iterator::ਆਖਰੀ() {
    int32_t ਸਿਰਾ_Pos = static_cast<int32_t>(utext_native_ਲੰਮਾਈ(&f_ਪਾਠ));
    UBool ਸਿਰਾ_Should_Be_Boundary = isBoundary(ਸਿਰਾ_Pos);      // Has side effect of setting iterator position.
    (ਖਾਲੀ)ਸਿਰਾ_Should_Be_Boundary;
    U_ASSERT(ਸਿਰਾ_Should_Be_Boundary);
    U_ASSERT(fPosition == ਸਿਰਾ_Pos);
    ਵਾਪਸ ਸਿਰਾ_Pos;
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
int32_t ਨਿਯਮ_Based_Break_Iterator::next(int32_t n) {
    int32_t ਨਤੀਜਾ = 0;
    ਜੇ (n > 0) {
        ਲਈ (; n > 0 && ਨਤੀਜਾ != UBRK_ਪੂਰਾ; --n) {
            ਨਤੀਜਾ = next();
        }
    } ਨਹੀਂਤਾਂ ਜੇ (n < 0) {
        ਲਈ (; n < 0 && ਨਤੀਜਾ != UBRK_ਪੂਰਾ; ++n) {
            ਨਤੀਜਾ = ਪਿਛਲਾ();
        }
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = current();
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t ਨਿਯਮ_Based_Break_Iterator::next() {
    fBreakCache->next();
    ਵਾਪਸ f_ਪੂਰਾ ? UBRK_ਪੂਰਾ : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t ਨਿਯਮ_Based_Break_Iterator::ਪਿਛਲਾ() {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    fBreakCache->ਪਿਛਲਾ(ਸਥਿਤੀ);
    ਵਾਪਸ f_ਪੂਰਾ ? UBRK_ਪੂਰਾ : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t ਨਿਯਮ_Based_Break_Iterator::following(int32_t ਸ਼ੁਰੂ_Pos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    ਜੇ (ਸ਼ੁਰੂ_Pos < 0) {
        ਵਾਪਸ first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ, ਸ਼ੁਰੂ_Pos);
    ਸ਼ੁਰੂ_Pos = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ));

    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    fBreakCache->following(ਸ਼ੁਰੂ_Pos, ਸਥਿਤੀ);
    ਵਾਪਸ f_ਪੂਰਾ ? UBRK_ਪੂਰਾ : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t ਨਿਯਮ_Based_Break_Iterator::preceding(int32_t ਵਿੱਥ) {
    ਜੇ (ਵਿੱਥ > utext_native_ਲੰਮਾਈ(&f_ਪਾਠ)) {
        ਵਾਪਸ ਆਖਰੀ();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ, ਵਿੱਥ);
    int32_t adjusted_ਵਿੱਥ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ));

    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_ਵਿੱਥ, ਸਥਿਤੀ);
    ਵਾਪਸ f_ਪੂਰਾ ? UBRK_ਪੂਰਾ : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool ਨਿਯਮ_Based_Break_Iterator::isBoundary(int32_t ਵਿੱਥ) {
    // out-of-range indexes are never boundary positions
    ਜੇ (ਵਿੱਥ < 0) {
        first();       // For side effects on current position, tag values.
        ਵਾਪਸ ਝੂਠ;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ, ਵਿੱਥ);
    int32_t adjusted_ਵਿੱਥ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ));

    ਤਾਰਕਿਕ ਨਤੀਜਾ = ਝੂਠ;
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ਜੇ (fBreakCache->seek(adjusted_ਵਿੱਥ) || fBreakCache->populateNear(adjusted_ਵਿੱਥ, ਸਥਿਤੀ)) {
        ਨਤੀਜਾ = (fBreakCache->current() == ਵਿੱਥ);
    }

    ਜੇ (ਨਤੀਜਾ && adjusted_ਵਿੱਥ < ਵਿੱਥ && utext_char32At(&f_ਪਾਠ, ਵਿੱਥ) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (!ਨਤੀਜਾ) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t ਨਿਯਮ_Based_Break_Iterator::current() ਅਚਲ {
    ਵਾਪਸ fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
ਗਿਣਤੀ RBBIRun_ਬਹੁਲਕ {
    RBBI_ਸ਼ੁਰੂ,     // state machine processing is before first char of input
    RBBI_ਲੜੀਵਾਰ,       // state machine processing is in the user text
    RBBI_ਸਿਰਾ        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
ਸਥਿਰ ਸਤਰਅੰਦਰ uint16_t TrieFunc8(ਅਚਲ UCPTrie *trie, UChar32 c) {
    ਵਾਪਸ UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint16_t TrieFunc16(ਅਚਲ UCPTrie *trie, UChar32 c) {
    ਵਾਪਸ UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t ਨਿਯਮ_Based_Break_Iterator::handle_ਅਗਲਾ() {
    ਅਚਲ RBBIState_ਸਾਰਣੀ *statetable = f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ;
    ਤਾਰਕਿਕ use8_ਬਿੱਟ_Trie = ucptrie_get_ਮੁੱਲ_ਚੌੜਾਈ(f_ਅੰਕੜੇ->fTrie) == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8;
    ਜੇ (statetable->fFlags & RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ) {
        ਜੇ (use8_ਬਿੱਟ_Trie) {
            ਵਾਪਸ handle_ਅਗਲਾ<RBBIState_ਸਾਰਣੀ_Row8, TrieFunc8>();
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ handle_ਅਗਲਾ<RBBIState_ਸਾਰਣੀ_Row8, TrieFunc16>();
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (use8_ਬਿੱਟ_Trie) {
            ਵਾਪਸ handle_ਅਗਲਾ<RBBIState_ਸਾਰਣੀ_Row16, TrieFunc8>();
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ handle_ਅਗਲਾ<RBBIState_ਸਾਰਣੀ_Row16, TrieFunc16>();
        }
    }
}

int32_t ਨਿਯਮ_Based_Break_Iterator::handle_ਸੁਰੱਖਿਅਤ_ਪਿਛਲਾ(int32_t fromPosition) {
    ਅਚਲ RBBIState_ਸਾਰਣੀ *statetable = f_ਅੰਕੜੇ->f_ਉਲਟ_ਸਾਰਣੀ;
    ਤਾਰਕਿਕ use8_ਬਿੱਟ_Trie = ucptrie_get_ਮੁੱਲ_ਚੌੜਾਈ(f_ਅੰਕੜੇ->fTrie) == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8;
    ਜੇ (statetable->fFlags & RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ) {
        ਜੇ (use8_ਬਿੱਟ_Trie) {
            ਵਾਪਸ handle_ਸੁਰੱਖਿਅਤ_ਪਿਛਲਾ<RBBIState_ਸਾਰਣੀ_Row8, TrieFunc8>(fromPosition);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ handle_ਸੁਰੱਖਿਅਤ_ਪਿਛਲਾ<RBBIState_ਸਾਰਣੀ_Row8, TrieFunc16>(fromPosition);
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (use8_ਬਿੱਟ_Trie) {
            ਵਾਪਸ handle_ਸੁਰੱਖਿਅਤ_ਪਿਛਲਾ<RBBIState_ਸਾਰਣੀ_Row16, TrieFunc8>(fromPosition);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ handle_ਸੁਰੱਖਿਅਤ_ਪਿਛਲਾ<RBBIState_ਸਾਰਣੀ_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ਪੰਕਤੀ_Type, ਨਿਯਮ_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t ਨਿਯਮ_Based_Break_Iterator::handle_ਅਗਲਾ() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_ਬਹੁਲਕ         ਬਹੁਲਕ;

    ਪੰਕਤੀ_Type             *ਪੰਕਤੀ;
    UChar32             c;
    int32_t             ਨਤੀਜਾ             = 0;
    int32_t             initialPosition    = 0;
    ਅਚਲ RBBIState_ਸਾਰਣੀ *statetable       = f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ;
    ਅਚਲ ਅੱਖਰ         *ਸਾਰਣੀ_ਅੰਕੜੇ          = statetable->f_ਸਾਰਣੀ_ਅੰਕੜੇ;
    uint32_t            ਸਾਰਣੀ_ਪੰਕਤੀ_Len        = statetable->f_ਪੰਕਤੀ_Len;
    uint32_t            dict_ਸ਼ੁਰੂ          = statetable->f_Dict_Categories_ਸ਼ੁਰੂ;
    #ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
        ਜੇ (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #ਜੇ_ਅੰਤ

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = 0;

    f_Dictionary_Char_ਗਿਣਤੀ = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&f_ਪਾਠ, initialPosition);
    ਨਤੀਜਾ          = initialPosition;
    c               = UTEXT_NEXT32(&f_ਪਾਠ);
    ਜੇ (c==U_SENTINEL) {
        f_ਪੂਰਾ = ਸੱਚ;
        ਵਾਪਸ UBRK_ਪੂਰਾ;
    }

    //  Set the initial state for the state machine
    state = ਸ਼ੁਰੂ_STATE;
    ਪੰਕਤੀ = (ਪੰਕਤੀ_Type *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (ਸਾਰਣੀ_ਅੰਕੜੇ + ਸਾਰਣੀ_ਪੰਕਤੀ_Len * state);


    ਬਹੁਲਕ     = RBBI_ਲੜੀਵਾਰ;


    // loop until we reach the end of the text or transition to state 0
    //
    ਲਈ (;;) {
        ਜੇ (c == U_SENTINEL) {
            // Reached end of input string.
            ਜੇ (ਬਹੁਲਕ == RBBI_ਸਿਰਾ) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                ਰੋਕੋ;
            }
            // Run the loop one last time with the fake end-of-input character category.
            ਬਹੁਲਕ = RBBI_ਸਿਰਾ;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        ਜੇ (ਬਹੁਲਕ == RBBI_ਲੜੀਵਾਰ) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_ਅੰਕੜੇ->fTrie, c);
            f_Dictionary_Char_ਗਿਣਤੀ += (category >= dict_ਸ਼ੁਰੂ);
        }

       #ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
            ਜੇ (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ));
                ਜੇ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } ਨਹੀਂਤਾਂ {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #ਜੇ_ਅੰਤ

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_ਅੰਕੜੇ->f_ਸਿਰਲੇਖ->f_Cat_ਗਿਣਤੀ);
        state = ਪੰਕਤੀ->f_ਅਗਲਾ_State[category];  /*Not accessing beyond memory*/
        ਪੰਕਤੀ = (ਪੰਕਤੀ_Type *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (ਸਾਰਣੀ_ਅੰਕੜੇ + ਸਾਰਣੀ_ਪੰਕਤੀ_Len * state);


        uint16_t accepting = ਪੰਕਤੀ->fAccepting;
        ਜੇ (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            ਜੇ (ਬਹੁਲਕ != RBBI_ਸ਼ੁਰੂ) {
                ਨਤੀਜਾ = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_ਪਾਠ));
            }
            f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = ਪੰਕਤੀ->f_ਟੈਗ_ਸੂਚੀ_Idx;   // Remember the break status (tag) values.
        } ਨਹੀਂਤਾਂ ਜੇ (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ);
            int32_t lookahead_ਨਤੀਜਾ = fLookAheadMatches[accepting];
            ਜੇ (lookahead_ਨਤੀਜਾ >= 0) {
                f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = ਪੰਕਤੀ->f_ਟੈਗ_ਸੂਚੀ_Idx;
                fPosition = lookahead_ਨਤੀਜਾ;
                ਵਾਪਸ lookahead_ਨਤੀਜਾ;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t ਨਿਯਮ = ਪੰਕਤੀ->fLookAhead;
        U_ASSERT(ਨਿਯਮ == 0 || ਨਿਯਮ > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(ਨਿਯਮ == 0 || ਨਿਯਮ < f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ);
        ਜੇ (ਨਿਯਮ > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_ਪਾਠ));
            fLookAheadMatches[ਨਿਯਮ] = pos;
        }

        ਜੇ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            ਰੋਕੋ;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        ਜੇ (ਬਹੁਲਕ == RBBI_ਲੜੀਵਾਰ) {
            c = UTEXT_NEXT32(&f_ਪਾਠ);
        } ਨਹੀਂਤਾਂ {
            ਜੇ (ਬਹੁਲਕ == RBBI_ਸ਼ੁਰੂ) {
                ਬਹੁਲਕ = RBBI_ਲੜੀਵਾਰ;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    ਜੇ (ਨਤੀਜਾ == initialPosition) {
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ, initialPosition);
        utext_next32(&f_ਪਾਠ);
        ਨਤੀਜਾ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ));
        f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = 0;
    }

    // Leave the iterator at our result position.
    fPosition = ਨਤੀਜਾ;
    #ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
        ਜੇ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", ਨਤੀਜਾ);
        }
    #ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ਪੰਕਤੀ_Type, ਨਿਯਮ_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t ਨਿਯਮ_Based_Break_Iterator::handle_ਸੁਰੱਖਿਅਤ_ਪਿਛਲਾ(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    ਪੰਕਤੀ_Type            *ਪੰਕਤੀ;
    UChar32             c;
    int32_t             ਨਤੀਜਾ          = 0;

    ਅਚਲ RBBIState_ਸਾਰਣੀ *state_ਸਾਰਣੀ = f_ਅੰਕੜੇ->f_ਉਲਟ_ਸਾਰਣੀ;
    UTEXT_SETNATIVEINDEX(&f_ਪਾਠ, fromPosition);
    #ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
        ਜੇ (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #ਜੇ_ਅੰਤ

    // if we're already at the start of the text, return DONE.
    ਜੇ (f_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ || UTEXT_GETNATIVEINDEX(&f_ਪਾਠ)==0) {
        ਵਾਪਸ BreakIterator::ਪੂਰਾ_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&f_ਪਾਠ);
    state = ਸ਼ੁਰੂ_STATE;
    ਪੰਕਤੀ = (ਪੰਕਤੀ_Type *)
            (state_ਸਾਰਣੀ->f_ਸਾਰਣੀ_ਅੰਕੜੇ + (state_ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len * state));

    // loop until we reach the start of the text or transition to state 0
    //
    ਲਈ (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&f_ਪਾਠ)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_ਅੰਕੜੇ->fTrie, c);

        #ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
            ਜੇ (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_ਸੂਚਕਾਂਕ(&f_ਪਾਠ));
                ਜੇ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } ਨਹੀਂਤਾਂ {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #ਜੇ_ਅੰਤ

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_ਅੰਕੜੇ->f_ਸਿਰਲੇਖ->f_Cat_ਗਿਣਤੀ);
        state = ਪੰਕਤੀ->f_ਅਗਲਾ_State[category];  /*Not accessing beyond memory*/
        ਪੰਕਤੀ = (ਪੰਕਤੀ_Type *)
            (state_ਸਾਰਣੀ->f_ਸਾਰਣੀ_ਅੰਕੜੇ + (state_ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len * state));

        ਜੇ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            ਰੋਕੋ;
        }
    }

    // The state machine is done.  Check whether it found a match...
    ਨਤੀਜਾ = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_ਪਾਠ));
    #ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
        ਜੇ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", ਨਤੀਜਾ);
        }
    #ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਤੀਜਾ;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  ਨਿਯਮ_Based_Break_Iterator::get_ਨਿਯਮ_ਸਥਿਤੀ() ਅਚਲ {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ + f_ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ[f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ];
    int32_t  ਟੈਗ_Val = f_ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ[idx];

    ਵਾਪਸ ਟੈਗ_Val;
}


int32_t ਨਿਯਮ_Based_Break_Iterator::get_ਨਿਯਮ_ਸਥਿਤੀ_Vec(
             int32_t *fill_ਵਿੱਚ_Vec, int32_t capacity, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }

    int32_t  ਅੰਕ_Vals = f_ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ[f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ];
    int32_t  ਅੰਕ_Vals_ਮੰਜ਼ਿਲ_Copy = ਅੰਕ_Vals;
    ਜੇ (ਅੰਕ_Vals > capacity) {
        ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਅੰਕ_Vals_ਮੰਜ਼ਿਲ_Copy = capacity;
    }
    ਪੂਰਨ i;
    ਲਈ (i=0; i<ਅੰਕ_Vals_ਮੰਜ਼ਿਲ_Copy; i++) {
        fill_ਵਿੱਚ_Vec[i] = f_ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ[f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ + i + 1];
    }
    ਵਾਪਸ ਅੰਕ_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ਅਚਲ uint8_t* ਨਿਯਮ_Based_Break_Iterator::get_ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ(uint32_t& length) U_LIFETIME_BOUND {
    ਅਚਲ uint8_t  *retPtr = ਖਾਲੀ_ਸੂਚਕ;
    length = 0;

    ਜੇ (f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        retPtr = reinterpret_cast<ਅਚਲ uint8_t*>(f_ਅੰਕੜੇ->f_ਸਿਰਲੇਖ);
        length = f_ਅੰਕੜੇ->f_ਸਿਰਲੇਖ->f_ਲੰਮਾਈ;
    }
    ਵਾਪਸ retPtr;
}


ਨਿਯਮ_Based_Break_Iterator *ਨਿਯਮ_Based_Break_Iterator::create_ਬਫਰ_Clone(
        ਖਾਲੀ * /*stackBuffer*/, int32_t &ਬਫਰ_ਮਾਪ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (ਬਫਰ_ਮਾਪ == 0) {
        ਬਫਰ_ਮਾਪ = 1;  // preflighting for deprecated functionality
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    BreakIterator *clonedBI = clone();
    ਜੇ (clonedBI == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ {
        ਸਥਿਤੀ = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ਵਾਪਸ (ਨਿਯਮ_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_ਸਿਰਾ


ਸਥਿਰ icu::UStack *gLanguageBreakFactories = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ ਅਚਲ icu::UnicodeString *g_ਖਾਲੀ_ਥਾਵਾਂ_String = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ਸਥਿਰ icu::UInitOnce gRBBIInitOnce {};
ਸਥਿਰ icu::ICULanguageBreakFactory *gICULanguageBreakFactory = ਖਾਲੀ_ਸੂਚਕ;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    ਮਿਟਾਓ gLanguageBreakFactories;
    gLanguageBreakFactories = ਖਾਲੀ_ਸੂਚਕ;
    ਮਿਟਾਓ g_ਖਾਲੀ_ਥਾਵਾਂ_String;
    g_ਖਾਲੀ_ਥਾਵਾਂ_String = ਖਾਲੀ_ਸੂਚਕ;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}
U_CDECL_ਸਿਰਾ

U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV _deleteFactory(ਖਾਲੀ *obj) {
    ਮਿਟਾਓ (icu::LanguageBreakFactory *) obj;
}
U_CDECL_ਸਿਰਾ
U_NAMESPACE_BEGIN

ਸਥਿਰ ਖਾਲੀ U_CALLCONV rbbiInit() {
    g_ਖਾਲੀ_ਥਾਵਾਂ_String = ਨਵਾਂ UnicodeString();
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV initLanguageFactories(UError_ਕੋਡ& ਸਥਿਤੀ) {
    U_ASSERT(gLanguageBreakFactories == ਖਾਲੀ_ਸੂਚਕ);
    gLanguageBreakFactories = ਨਵਾਂ UStack(_deleteFactory, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
    ਜੇ (gLanguageBreakFactories != ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(ਸਥਿਤੀ)) {
        Local_ਸੂਚਕ<ICULanguageBreakFactory> factory(ਨਵਾਂ ICULanguageBreakFactory(ਸਥਿਤੀ), ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, ਸਥਿਤੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &ਸਥਿਤੀ);
            ਜੇ (extra != ਖਾਲੀ_ਸੂਚਕ) {
                gLanguageBreakFactories->push(extra, ਸਥਿਤੀ);
            }
#ਜੇ_ਅੰਤ
        }
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ਖਾਲੀ ensureLanguageFactories(UError_ਕੋਡ& ਸਥਿਤੀ) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, ਸਥਿਤੀ);
}

ਸਥਿਰ ਅਚਲ LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ਅਚਲ ਅੱਖਰ* locale)
{
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ensureLanguageFactories(ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

    int32_t i = gLanguageBreakFactories->size();
    ਅਚਲ LanguageBreakEngine *lbe = ਖਾਲੀ_ਸੂਚਕ;
    ਜਦੋਂ (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        ਜੇ (lbe != ਖਾਲੀ_ਸੂਚਕ) {
            ਰੋਕੋ;
        }
    }
    ਵਾਪਸ lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
ਅਚਲ LanguageBreakEngine*
ਨਿਯਮ_Based_Break_Iterator::getLanguageBreakEngine(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) U_LIFETIME_BOUND {
    ਅਚਲ LanguageBreakEngine *lbe = ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

    ਜੇ (fLanguageBreakEngines == ਖਾਲੀ_ਸੂਚਕ) {
        fLanguageBreakEngines = ਨਵਾਂ UStack(ਸਥਿਤੀ);
        ਜੇ (fLanguageBreakEngines == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(ਸਥਿਤੀ)) {
            ਮਿਟਾਓ fLanguageBreakEngines;
            fLanguageBreakEngines = ਖਾਲੀ_ਸੂਚਕ;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    ਜਦੋਂ (--i >= 0) {
        lbe = static_cast<ਅਚਲ LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        ਜੇ (lbe->handles(c, locale)) {
            ਵਾਪਸ lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    ਜੇ (lbe != ਖਾਲੀ_ਸੂਚਕ) {
        fLanguageBreakEngines->push((ਖਾਲੀ *)lbe, ਸਥਿਤੀ);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        ਵਾਪਸ lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    ਜੇ (fUnhandledBreakEngine == ਖਾਲੀ_ਸੂਚਕ) {
        fUnhandledBreakEngine = ਨਵਾਂ UnhandledEngine(ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && fUnhandledBreakEngine == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->ਪਾਓ_Element_At(fUnhandledBreakEngine, 0, ਸਥਿਤੀ);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਮਿਟਾਓ fUnhandledBreakEngine;
            fUnhandledBreakEngine = ਖਾਲੀ_ਸੂਚਕ;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    ਵਾਪਸ fUnhandledBreakEngine;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_DRAFT_API
ਖਾਲੀ U_EXPORT2 ਨਿਯਮ_Based_Break_Iterator::ਰਜਿਸਟਰ_External_Break_Engine(
                  ExternalBreakEngine* ਮੰਜ਼ਿਲ_Adopt, UError_ਕੋਡ& ਸਥਿਤੀ) {
    Local_ਸੂਚਕ<ExternalBreakEngine> engine(ਮੰਜ਼ਿਲ_Adopt, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    ensureLanguageFactories(ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    gICULanguageBreakFactory->ਜੋੜੋ_External_Engine(engine.orphan(), ਸਥਿਤੀ);
}
#ਜੇ_ਅੰਤ  /* U_HIDE_DRAFT_API */


ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::dumpTables() {
    f_ਅੰਕੜੇ->print_ਅੰਕੜੇ();
}

/**
 * Returns the description used to create this iterator
 */

ਅਚਲ UnicodeString&
ਨਿਯਮ_Based_Break_Iterator::get_ਨਿਯਮਾਂ() ਅਚਲ U_LIFETIME_BOUND {
    ਜੇ (f_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ f_ਅੰਕੜੇ->get_ਨਿਯਮ_ਸਰੋਤ_String();
    } ਨਹੀਂਤਾਂ {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        ਵਾਪਸ *g_ਖਾਲੀ_ਥਾਵਾਂ_String;
    }
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */

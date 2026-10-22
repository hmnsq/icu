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

#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ <cinttypes>

#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"
#ಅನ್ತರ್ಭಾವ "unicode/schriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/uclean.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "localsvc.h"
#ಅನ್ತರ್ಭಾವ "rbbidata.h"
#ಅನ್ತರ್ಭಾವ "rbbi_cache.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಸ್ಥಿರ UBool gTrace = ಸುಳ್ಳು;
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t ಆರಂಭ_STATE = 1;

// The state-transition value indicating "stop"
constexpr int32_t STOP_STATE = 0;


UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ನಿಯಮ_Based_Break_Iterator)


//=======================================================================
// constructors
//=======================================================================

/**
 * Constructs a RuleBasedBreakIterator that uses the already-created
 * tables object that is passed in as a parameter.
 */
ನಿಯಮ_Based_Break_Iterator::ನಿಯಮ_Based_Break_Iterator(RBBIData_ಶೀರ್ಷಿಕೆ* ದತ್ತಾಂಶ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
 : ನಿಯಮ_Based_Break_Iterator(&ಸ್ಥಿತಿ_2)
{
    f_ದತ್ತಾಂಶ = ಹೊಸ RBBIDataWrapper(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2); // status checked in constructor
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {ಹಿಂತಿರುಗಿ;}
    ಆದರೆ(f_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ * ಗಾತ್ರ(int32_t)));
        ಆದರೆ (fLookAheadMatches == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
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
ನಿಯಮ_Based_Break_Iterator::ನಿಯಮ_Based_Break_Iterator(UData_ಸ್ಮರಣೆ* udm, UBool isPhraseBreaking,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) : ನಿಯಮ_Based_Break_Iterator(udm, ಸ್ಥಿತಿ_2)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
ನಿಯಮ_Based_Break_Iterator::ನಿಯಮ_Based_Break_Iterator(ಸ್ಥಿರಾಂಕ uint8_t *compiled_ನಿಯಮಗಳು,
                       uint32_t       ನಿಯಮ_ಉದ್ದಳತೆ,
                       UError_ಸಂಕೇತ     &ಸ್ಥಿತಿ_2)
 : ನಿಯಮ_Based_Break_Iterator(&ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (compiled_ನಿಯಮಗಳು == ಶೂನ್ಯಸೂಚಕ || ನಿಯಮ_ಉದ್ದಳತೆ < ಗಾತ್ರ(RBBIData_ಶೀರ್ಷಿಕೆ)) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ* ದತ್ತಾಂಶ = reinterpret_cast<ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ*>(compiled_ನಿಯಮಗಳು);
    ಆದರೆ (ದತ್ತಾಂಶ->f_ಉದ್ದಳತೆ > ನಿಯಮ_ಉದ್ದಳತೆ) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    f_ದತ್ತಾಂಶ = ಹೊಸ RBBIDataWrapper(ದತ್ತಾಂಶ, RBBIDataWrapper::kDontAdopt, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {ಹಿಂತಿರುಗಿ;}
    ಆದರೆ(f_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ * ಗಾತ್ರ(int32_t)));
        ಆದರೆ (fLookAheadMatches == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
    }
}


//-------------------------------------------------------------------------------
//
//   Constructor   from a UDataMemory handle to precompiled break rules
//                 stored in an ICU data file.
//
//-------------------------------------------------------------------------------
ನಿಯಮ_Based_Break_Iterator::ನಿಯಮ_Based_Break_Iterator(UData_ಸ್ಮರಣೆ* udm, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
 : ನಿಯಮ_Based_Break_Iterator(&ಸ್ಥಿತಿ_2)
{
    f_ದತ್ತಾಂಶ = ಹೊಸ RBBIDataWrapper(udm, ಸ್ಥಿತಿ_2); // status checked in constructor
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {ಹಿಂತಿರುಗಿ;}
    ಆದರೆ(f_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ * ಗಾತ್ರ(int32_t)));
        ಆದರೆ (fLookAheadMatches == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
ನಿಯಮ_Based_Break_Iterator::ನಿಯಮ_Based_Break_Iterator( ಸ್ಥಿರಾಂಕ UnicodeString  &ನಿಯಮಗಳು,
                                                UParseError          &parseError,
                                                UError_ಸಂಕೇತ           &ಸ್ಥಿತಿ_2)
 : ನಿಯಮ_Based_Break_Iterator(&ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {ಹಿಂತಿರುಗಿ;}
    ನಿಯಮ_Based_Break_Iterator *bi = (ನಿಯಮ_Based_Break_Iterator *)
        RBBIRuleBuilder::create_ನಿಯಮ_Based_Break_Iterator(ನಿಯಮಗಳು, &parseError, ಸ್ಥಿತಿ_2);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        *ಇದು = *bi;
        ಅಳಿಸು bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
ನಿಯಮ_Based_Break_Iterator::ನಿಯಮ_Based_Break_Iterator()
 : ನಿಯಮ_Based_Break_Iterator(ಶೂನ್ಯಸೂಚಕ)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
ನಿಯಮ_Based_Break_Iterator::ನಿಯಮ_Based_Break_Iterator(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UError_ಸಂಕೇತ ec = U_ZERO_ERROR;
    ಆದರೆ (ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = &ec;
    }
    utext_openUChars(&f_ಪಠ್ಯ, ಶೂನ್ಯಸೂಚಕ, 0, ಸ್ಥಿತಿ_2);
    Local_ಸೂಚಕ<DictionaryCache> lpDictionaryCache(ಹೊಸ DictionaryCache(ಇದು, *ಸ್ಥಿತಿ_2), *ಸ್ಥಿತಿ_2);
    Local_ಸೂಚಕ<BreakCache> lpBreakCache(ಹೊಸ BreakCache(ಇದು, *ಸ್ಥಿತಿ_2), *ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        f_Error_ಸಂಕೇತ = *ಸ್ಥಿತಿ_2;
        ಹಿಂತಿರುಗಿ;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಸ್ಥಿರ UBool debug_Init_ಮುಗಿದ = ಸುಳ್ಳು;
    ಆದರೆ (debug_Init_ಮುಗಿದ == ಸುಳ್ಳು) {
        ಅಕ್ಷರ *debugEnv = getenv("U_RBBIDEBUG");
        ಆದರೆ (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = ನಿಜ;
        }
        debug_Init_ಮುಗಿದ = ನಿಜ;
    }
#ಆದರೆ_ಅಂತ್ಯ
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
ನಿಯಮ_Based_Break_Iterator::ನಿಯಮ_Based_Break_Iterator(ಸ್ಥಿರಾಂಕ ನಿಯಮ_Based_Break_Iterator& other)
: ನಿಯಮ_Based_Break_Iterator()
{
    *ಇದು = other;
}


/**
 * Destructor
 */
ನಿಯಮ_Based_Break_Iterator::~ನಿಯಮ_Based_Break_Iterator() {
    ಆದರೆ (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        ಅಳಿಸು fCharIter;
    }
    fCharIter = ಶೂನ್ಯಸೂಚಕ;

    utext_close(&f_ಪಠ್ಯ);

    ಆದರೆ (f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        f_ದತ್ತಾಂಶ->removeReference();
        f_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    }
    ಅಳಿಸು fBreakCache;
    fBreakCache = ಶೂನ್ಯಸೂಚಕ;

    ಅಳಿಸು fDictionaryCache;
    fDictionaryCache = ಶೂನ್ಯಸೂಚಕ;

    ಅಳಿಸು fLanguageBreakEngines;
    fLanguageBreakEngines = ಶೂನ್ಯಸೂಚಕ;

    ಅಳಿಸು fUnhandledBreakEngine;
    fUnhandledBreakEngine = ಶೂನ್ಯಸೂಚಕ;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = ಶೂನ್ಯಸೂಚಕ;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
ನಿಯಮ_Based_Break_Iterator&
ನಿಯಮ_Based_Break_Iterator::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ನಿಯಮ_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    ಆದರೆ (ಇದು == &that) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    BreakIterator::ಕ್ರಿಯಾಕಾರಕ=(that);

    ಆದರೆ (fLanguageBreakEngines != ಶೂನ್ಯಸೂಚಕ) {
        ಅಳಿಸು fLanguageBreakEngines;
        fLanguageBreakEngines = ಶೂನ್ಯಸೂಚಕ;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    utext_clone(&f_ಪಠ್ಯ, &that.f_ಪಠ್ಯ, ಸುಳ್ಳು, ನಿಜ, &ಸ್ಥಿತಿ_2);

    ಆದರೆ (fCharIter != &fSCharIter) {
        ಅಳಿಸು fCharIter;
    }
    fCharIter = &fSCharIter;

    ಆದರೆ (that.fCharIter != ಶೂನ್ಯಸೂಚಕ && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    ಆದರೆ (fCharIter == ಶೂನ್ಯಸೂಚಕ) {
        fCharIter = &fSCharIter;
    }

    ಆದರೆ (f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        f_ದತ್ತಾಂಶ->removeReference();
        f_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (that.f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        f_ದತ್ತಾಂಶ = that.f_ದತ್ತಾಂಶ->ಕೂಡಿಸು_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (f_ದತ್ತಾಂಶ && f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ * ಗಾತ್ರ(int32_t)));
    }


    fPosition = that.fPosition;
    f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = that.f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ;
    f_ಮುಗಿದ = that.f_ಮುಗಿದ;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ);
    fDictionaryCache->reset();

    ಹಿಂತಿರುಗಿ *ಇದು;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
ನಿಯಮ_Based_Break_Iterator*
ನಿಯಮ_Based_Break_Iterator::clone() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ ನಿಯಮ_Based_Break_Iterator(*ಇದು);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
ತಾರ್ಕಿಕ
ನಿಯಮ_Based_Break_Iterator::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ BreakIterator& that) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (typeid(*ಇದು) != typeid(that)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (ಇದು == &that) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ಸ್ಥಿರಾಂಕ ನಿಯಮ_Based_Break_Iterator& that2 = static_cast<ಸ್ಥಿರಾಂಕ ನಿಯಮ_Based_Break_Iterator&>(that);

    ಆದರೆ (!utext_equals(&f_ಪಠ್ಯ, &that2.f_ಪಠ್ಯ)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ (!(fPosition == that2.fPosition &&
            f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ == that2.f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ &&
            f_ಮುಗಿದ == that2.f_ಮುಗಿದ &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ (that2.f_ದತ್ತಾಂಶ == f_ದತ್ತಾಂಶ ||
        (f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ && that2.f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ && *that2.f_ದತ್ತಾಂಶ == *f_ದತ್ತಾಂಶ)) {
            // The two break iterators are using the same rules.
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
ನಿಯಮ_Based_Break_Iterator::hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ {
    int32_t   hash = 0;
    ಆದರೆ (f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        hash = f_ದತ್ತಾಂಶ->hash_ಸಂಕೇತ();
    }
    ಹಿಂತಿರುಗಿ hash;
}


ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::ಗಣ_ಪಠ್ಯ(UText *ut, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&f_ಪಠ್ಯ, ut, ಸುಳ್ಳು, ನಿಜ, &ಸ್ಥಿತಿ_2);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.ಗಣ_ಪಠ್ಯ(u"", 0);

    ಆದರೆ (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        ಅಳಿಸು fCharIter;
    }
    fCharIter = &fSCharIter;

    ಇದು->first();
}


UText* ನಿಯಮ_Based_Break_Iterator::getUText(UText* fill_ಒಳ U_LIFETIME_BOUND, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    UText *ಫಲಿತಾಂಶ = utext_clone(fill_ಒಳ, &f_ಪಠ್ಯ, ಸುಳ್ಳು, ನಿಜ, &ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
ನಿಯಮ_Based_Break_Iterator::get_ಪಠ್ಯ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
ಶೂನ್ಯ
ನಿಯಮ_Based_Break_Iterator::adopt_ಪಠ್ಯ(CharacterIterator* new_ಪಠ್ಯ) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    ಆದರೆ (fCharIter != &fSCharIter) {
        ಅಳಿಸು fCharIter;
    }

    fCharIter = new_ಪಠ್ಯ;
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    ಆದರೆ (new_ಪಠ್ಯ==ಶೂನ್ಯಸೂಚಕ || new_ಪಠ್ಯ->ಆರಂಭ_ಸೂಚ್ಯಂಕ() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&f_ಪಠ್ಯ, ಶೂನ್ಯಸೂಚಕ, 0, &ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        utext_openCharacterIterator(&f_ಪಠ್ಯ, new_ಪಠ್ಯ, &ಸ್ಥಿತಿ_2);
    }
    ಇದು->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
ಶೂನ್ಯ
ನಿಯಮ_Based_Break_Iterator::ಗಣ_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString& new_ಪಠ್ಯ) {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&f_ಪಠ್ಯ, &new_ಪಠ್ಯ, &ಸ್ಥಿತಿ_2);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.ಗಣ_ಪಠ್ಯ(new_ಪಠ್ಯ.get_ಬಫರ್(), new_ಪಠ್ಯ.length());

    ಆದರೆ (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        ಅಳಿಸು fCharIter;
    }
    fCharIter = &fSCharIter;

    ಇದು->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
ನಿಯಮ_Based_Break_Iterator&
ನಿಯಮ_Based_Break_Iterator::refresh_ಒಳಹರಿವು_ಪಠ್ಯ(UText* input, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (input == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    int64_t pos = utext_get_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&f_ಪಠ್ಯ, input, ಸುಳ್ಳು, ನಿಜ, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ, pos);
    ಆದರೆ (utext_get_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t ನಿಯಮ_Based_Break_Iterator::first() {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ಆದರೆ (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, ಸ್ಥಿತಿ_2);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    ಹಿಂತಿರುಗಿ 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t ನಿಯಮ_Based_Break_Iterator::ಕೊನೆಯ() {
    int32_t ಕೊನೆ_Pos = static_cast<int32_t>(utext_native_ಉದ್ದಳತೆ(&f_ಪಠ್ಯ));
    UBool ಕೊನೆ_Should_Be_Boundary = isBoundary(ಕೊನೆ_Pos);      // Has side effect of setting iterator position.
    (ಶೂನ್ಯ)ಕೊನೆ_Should_Be_Boundary;
    U_ASSERT(ಕೊನೆ_Should_Be_Boundary);
    U_ASSERT(fPosition == ಕೊನೆ_Pos);
    ಹಿಂತಿರುಗಿ ಕೊನೆ_Pos;
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
int32_t ನಿಯಮ_Based_Break_Iterator::next(int32_t n) {
    int32_t ಫಲಿತಾಂಶ = 0;
    ಆದರೆ (n > 0) {
        ಗಾಗಿ (; n > 0 && ಫಲಿತಾಂಶ != UBRK_ಮುಗಿದ; --n) {
            ಫಲಿತಾಂಶ = next();
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n < 0) {
        ಗಾಗಿ (; n < 0 && ಫಲಿತಾಂಶ != UBRK_ಮುಗಿದ; ++n) {
            ಫಲಿತಾಂಶ = ಹಿಂದಿನ();
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = current();
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t ನಿಯಮ_Based_Break_Iterator::next() {
    fBreakCache->next();
    ಹಿಂತಿರುಗಿ f_ಮುಗಿದ ? UBRK_ಮುಗಿದ : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t ನಿಯಮ_Based_Break_Iterator::ಹಿಂದಿನ() {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    fBreakCache->ಹಿಂದಿನ(ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ f_ಮುಗಿದ ? UBRK_ಮುಗಿದ : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t ನಿಯಮ_Based_Break_Iterator::following(int32_t ಆರಂಭ_Pos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    ಆದರೆ (ಆರಂಭ_Pos < 0) {
        ಹಿಂತಿರುಗಿ first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ, ಆರಂಭ_Pos);
    ಆರಂಭ_Pos = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ));

    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    fBreakCache->following(ಆರಂಭ_Pos, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ f_ಮುಗಿದ ? UBRK_ಮುಗಿದ : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t ನಿಯಮ_Based_Break_Iterator::preceding(int32_t ಸ್ಥಾನಾಂತರ) {
    ಆದರೆ (ಸ್ಥಾನಾಂತರ > utext_native_ಉದ್ದಳತೆ(&f_ಪಠ್ಯ)) {
        ಹಿಂತಿರುಗಿ ಕೊನೆಯ();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ, ಸ್ಥಾನಾಂತರ);
    int32_t adjusted_ಸ್ಥಾನಾಂತರ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ));

    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ f_ಮುಗಿದ ? UBRK_ಮುಗಿದ : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool ನಿಯಮ_Based_Break_Iterator::isBoundary(int32_t ಸ್ಥಾನಾಂತರ) {
    // out-of-range indexes are never boundary positions
    ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0) {
        first();       // For side effects on current position, tag values.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ, ಸ್ಥಾನಾಂತರ);
    int32_t adjusted_ಸ್ಥಾನಾಂತರ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ));

    ತಾರ್ಕಿಕ ಫಲಿತಾಂಶ = ಸುಳ್ಳು;
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ಆದರೆ (fBreakCache->seek(adjusted_ಸ್ಥಾನಾಂತರ) || fBreakCache->populateNear(adjusted_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿತಿ_2)) {
        ಫಲಿತಾಂಶ = (fBreakCache->current() == ಸ್ಥಾನಾಂತರ);
    }

    ಆದರೆ (ಫಲಿತಾಂಶ && adjusted_ಸ್ಥಾನಾಂತರ < ಸ್ಥಾನಾಂತರ && utext_char32At(&f_ಪಠ್ಯ, ಸ್ಥಾನಾಂತರ) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (!ಫಲಿತಾಂಶ) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t ನಿಯಮ_Based_Break_Iterator::current() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
ಎಣಿಕೆ RBBIRun_ಬಹುಲಕ {
    RBBI_ಆರಂಭ,     // state machine processing is before first char of input
    RBBI_ಸತತ,       // state machine processing is in the user text
    RBBI_ಕೊನೆ        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint16_t TrieFunc8(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 c) {
    ಹಿಂತಿರುಗಿ UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint16_t TrieFunc16(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 c) {
    ಹಿಂತಿರುಗಿ UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t ನಿಯಮ_Based_Break_Iterator::handle_ಮುಂದಿನ() {
    ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ *statetable = f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ;
    ತಾರ್ಕಿಕ use8_ಬಿಟ್ಟುಗಳು_Trie = ucptrie_get_ಮೌಲ್ಯ_ಅಗಲ(f_ದತ್ತಾಂಶ->fTrie) == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8;
    ಆದರೆ (statetable->fFlags & RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು) {
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_Trie) {
            ಹಿಂತಿರುಗಿ handle_ಮುಂದಿನ<RBBIState_ಕೋಷ್ಟಕ_Row8, TrieFunc8>();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ handle_ಮುಂದಿನ<RBBIState_ಕೋಷ್ಟಕ_Row8, TrieFunc16>();
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_Trie) {
            ಹಿಂತಿರುಗಿ handle_ಮುಂದಿನ<RBBIState_ಕೋಷ್ಟಕ_Row16, TrieFunc8>();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ handle_ಮುಂದಿನ<RBBIState_ಕೋಷ್ಟಕ_Row16, TrieFunc16>();
        }
    }
}

int32_t ನಿಯಮ_Based_Break_Iterator::handle_ಸುರಕ್ಷಿತ_ಹಿಂದಿನ(int32_t fromPosition) {
    ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ *statetable = f_ದತ್ತಾಂಶ->f_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ;
    ತಾರ್ಕಿಕ use8_ಬಿಟ್ಟುಗಳು_Trie = ucptrie_get_ಮೌಲ್ಯ_ಅಗಲ(f_ದತ್ತಾಂಶ->fTrie) == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8;
    ಆದರೆ (statetable->fFlags & RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು) {
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_Trie) {
            ಹಿಂತಿರುಗಿ handle_ಸುರಕ್ಷಿತ_ಹಿಂದಿನ<RBBIState_ಕೋಷ್ಟಕ_Row8, TrieFunc8>(fromPosition);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ handle_ಸುರಕ್ಷಿತ_ಹಿಂದಿನ<RBBIState_ಕೋಷ್ಟಕ_Row8, TrieFunc16>(fromPosition);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_Trie) {
            ಹಿಂತಿರುಗಿ handle_ಸುರಕ್ಷಿತ_ಹಿಂದಿನ<RBBIState_ಕೋಷ್ಟಕ_Row16, TrieFunc8>(fromPosition);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ handle_ಸುರಕ್ಷಿತ_ಹಿಂದಿನ<RBBIState_ಕೋಷ್ಟಕ_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ಅಡ್ಡಸಾಲು_Type, ನಿಯಮ_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t ನಿಯಮ_Based_Break_Iterator::handle_ಮುಂದಿನ() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_ಬಹುಲಕ         ಬಹುಲಕ;

    ಅಡ್ಡಸಾಲು_Type             *ಅಡ್ಡಸಾಲು;
    UChar32             c;
    int32_t             ಫಲಿತಾಂಶ             = 0;
    int32_t             initialPosition    = 0;
    ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ *statetable       = f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ         *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ          = statetable->f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ;
    uint32_t            ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು_Len        = statetable->f_ಅಡ್ಡಸಾಲು_Len;
    uint32_t            dict_ಆರಂಭ          = statetable->f_Dict_Categories_ಆರಂಭ;
    #ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
        ಆದರೆ (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #ಆದರೆ_ಅಂತ್ಯ

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = 0;

    f_Dictionary_Char_ಎಣಿಕೆ = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&f_ಪಠ್ಯ, initialPosition);
    ಫಲಿತಾಂಶ          = initialPosition;
    c               = UTEXT_NEXT32(&f_ಪಠ್ಯ);
    ಆದರೆ (c==U_SENTINEL) {
        f_ಮುಗಿದ = ನಿಜ;
        ಹಿಂತಿರುಗಿ UBRK_ಮುಗಿದ;
    }

    //  Set the initial state for the state machine
    state = ಆರಂಭ_STATE;
    ಅಡ್ಡಸಾಲು = (ಅಡ್ಡಸಾಲು_Type *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ + ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು_Len * state);


    ಬಹುಲಕ     = RBBI_ಸತತ;


    // loop until we reach the end of the text or transition to state 0
    //
    ಗಾಗಿ (;;) {
        ಆದರೆ (c == U_SENTINEL) {
            // Reached end of input string.
            ಆದರೆ (ಬಹುಲಕ == RBBI_ಕೊನೆ) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                ನಿಲ್ಲಿಸು;
            }
            // Run the loop one last time with the fake end-of-input character category.
            ಬಹುಲಕ = RBBI_ಕೊನೆ;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        ಆದರೆ (ಬಹುಲಕ == RBBI_ಸತತ) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_ದತ್ತಾಂಶ->fTrie, c);
            f_Dictionary_Char_ಎಣಿಕೆ += (category >= dict_ಆರಂಭ);
        }

       #ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
            ಆದರೆ (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ));
                ಆದರೆ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #ಆದರೆ_ಅಂತ್ಯ

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_ದತ್ತಾಂಶ->f_ಶೀರ್ಷಿಕೆ->f_Cat_ಎಣಿಕೆ);
        state = ಅಡ್ಡಸಾಲು->f_ಮುಂದಿನ_State[category];  /*Not accessing beyond memory*/
        ಅಡ್ಡಸಾಲು = (ಅಡ್ಡಸಾಲು_Type *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ + ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು_Len * state);


        uint16_t accepting = ಅಡ್ಡಸಾಲು->fAccepting;
        ಆದರೆ (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            ಆದರೆ (ಬಹುಲಕ != RBBI_ಆರಂಭ) {
                ಫಲಿತಾಂಶ = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_ಪಠ್ಯ));
            }
            f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = ಅಡ್ಡಸಾಲು->f_ಟ್ಯಾಗುಗಳು_Idx;   // Remember the break status (tag) values.
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ);
            int32_t lookahead_ಫಲಿತಾಂಶ = fLookAheadMatches[accepting];
            ಆದರೆ (lookahead_ಫಲಿತಾಂಶ >= 0) {
                f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = ಅಡ್ಡಸಾಲು->f_ಟ್ಯಾಗುಗಳು_Idx;
                fPosition = lookahead_ಫಲಿತಾಂಶ;
                ಹಿಂತಿರುಗಿ lookahead_ಫಲಿತಾಂಶ;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t ನಿಯಮ = ಅಡ್ಡಸಾಲು->fLookAhead;
        U_ASSERT(ನಿಯಮ == 0 || ನಿಯಮ > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(ನಿಯಮ == 0 || ನಿಯಮ < f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ);
        ಆದರೆ (ನಿಯಮ > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_ಪಠ್ಯ));
            fLookAheadMatches[ನಿಯಮ] = pos;
        }

        ಆದರೆ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            ನಿಲ್ಲಿಸು;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        ಆದರೆ (ಬಹುಲಕ == RBBI_ಸತತ) {
            c = UTEXT_NEXT32(&f_ಪಠ್ಯ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (ಬಹುಲಕ == RBBI_ಆರಂಭ) {
                ಬಹುಲಕ = RBBI_ಸತತ;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    ಆದರೆ (ಫಲಿತಾಂಶ == initialPosition) {
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ, initialPosition);
        utext_next32(&f_ಪಠ್ಯ);
        ಫಲಿತಾಂಶ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ));
        f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = 0;
    }

    // Leave the iterator at our result position.
    fPosition = ಫಲಿತಾಂಶ;
    #ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
        ಆದರೆ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", ಫಲಿತಾಂಶ);
        }
    #ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ಅಡ್ಡಸಾಲು_Type, ನಿಯಮ_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t ನಿಯಮ_Based_Break_Iterator::handle_ಸುರಕ್ಷಿತ_ಹಿಂದಿನ(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    ಅಡ್ಡಸಾಲು_Type            *ಅಡ್ಡಸಾಲು;
    UChar32             c;
    int32_t             ಫಲಿತಾಂಶ          = 0;

    ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ *state_ಕೋಷ್ಟಕ = f_ದತ್ತಾಂಶ->f_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ;
    UTEXT_SETNATIVEINDEX(&f_ಪಠ್ಯ, fromPosition);
    #ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
        ಆದರೆ (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #ಆದರೆ_ಅಂತ್ಯ

    // if we're already at the start of the text, return DONE.
    ಆದರೆ (f_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ || UTEXT_GETNATIVEINDEX(&f_ಪಠ್ಯ)==0) {
        ಹಿಂತಿರುಗಿ BreakIterator::ಮುಗಿದ_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&f_ಪಠ್ಯ);
    state = ಆರಂಭ_STATE;
    ಅಡ್ಡಸಾಲು = (ಅಡ್ಡಸಾಲು_Type *)
            (state_ಕೋಷ್ಟಕ->f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ + (state_ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len * state));

    // loop until we reach the start of the text or transition to state 0
    //
    ಗಾಗಿ (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&f_ಪಠ್ಯ)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_ದತ್ತಾಂಶ->fTrie, c);

        #ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
            ಆದರೆ (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_ಸೂಚ್ಯಂಕ(&f_ಪಠ್ಯ));
                ಆದರೆ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #ಆದರೆ_ಅಂತ್ಯ

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_ದತ್ತಾಂಶ->f_ಶೀರ್ಷಿಕೆ->f_Cat_ಎಣಿಕೆ);
        state = ಅಡ್ಡಸಾಲು->f_ಮುಂದಿನ_State[category];  /*Not accessing beyond memory*/
        ಅಡ್ಡಸಾಲು = (ಅಡ್ಡಸಾಲು_Type *)
            (state_ಕೋಷ್ಟಕ->f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ + (state_ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len * state));

        ಆದರೆ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            ನಿಲ್ಲಿಸು;
        }
    }

    // The state machine is done.  Check whether it found a match...
    ಫಲಿತಾಂಶ = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_ಪಠ್ಯ));
    #ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
        ಆದರೆ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", ಫಲಿತಾಂಶ);
        }
    #ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  ನಿಯಮ_Based_Break_Iterator::get_ನಿಯಮ_ಸ್ಥಿತಿ() ಸ್ಥಿರಾಂಕ {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ + f_ದತ್ತಾಂಶ->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ[f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ];
    int32_t  ಟ್ಯಾಗ್_Val = f_ದತ್ತಾಂಶ->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ[idx];

    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_Val;
}


int32_t ನಿಯಮ_Based_Break_Iterator::get_ನಿಯಮ_ಸ್ಥಿತಿ_Vec(
             int32_t *fill_ಒಳ_Vec, int32_t capacity, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t  ಅಂಕೆ_Vals = f_ದತ್ತಾಂಶ->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ[f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ];
    int32_t  ಅಂಕೆ_Vals_ಗಮ್ಯ_Copy = ಅಂಕೆ_Vals;
    ಆದರೆ (ಅಂಕೆ_Vals > capacity) {
        ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಅಂಕೆ_Vals_ಗಮ್ಯ_Copy = capacity;
    }
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i=0; i<ಅಂಕೆ_Vals_ಗಮ್ಯ_Copy; i++) {
        fill_ಒಳ_Vec[i] = f_ದತ್ತಾಂಶ->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ[f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ + i + 1];
    }
    ಹಿಂತಿರುಗಿ ಅಂಕೆ_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ಸ್ಥಿರಾಂಕ uint8_t* ನಿಯಮ_Based_Break_Iterator::get_ದ್ವಿಮಾನ_ನಿಯಮಗಳು(uint32_t& length) U_LIFETIME_BOUND {
    ಸ್ಥಿರಾಂಕ uint8_t  *retPtr = ಶೂನ್ಯಸೂಚಕ;
    length = 0;

    ಆದರೆ (f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        retPtr = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(f_ದತ್ತಾಂಶ->f_ಶೀರ್ಷಿಕೆ);
        length = f_ದತ್ತಾಂಶ->f_ಶೀರ್ಷಿಕೆ->f_ಉದ್ದಳತೆ;
    }
    ಹಿಂತಿರುಗಿ retPtr;
}


ನಿಯಮ_Based_Break_Iterator *ನಿಯಮ_Based_Break_Iterator::create_ಬಫರ್_Clone(
        ಶೂನ್ಯ * /*stackBuffer*/, int32_t &ಬಫರ್_ಗಾತ್ರ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (ಬಫರ್_ಗಾತ್ರ == 0) {
        ಬಫರ್_ಗಾತ್ರ = 1;  // preflighting for deprecated functionality
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    BreakIterator *clonedBI = clone();
    ಆದರೆ (clonedBI == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿತಿ_2 = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ಹಿಂತಿರುಗಿ (ನಿಯಮ_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_ಕೊನೆ


ಸ್ಥಿರ icu::UStack *gLanguageBreakFactories = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ icu::UnicodeString *g_ಖಾಲಿಗಳು_String = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
ಸ್ಥಿರ icu::UInitOnce gRBBIInitOnce {};
ಸ್ಥಿರ icu::ICULanguageBreakFactory *gICULanguageBreakFactory = ಶೂನ್ಯಸೂಚಕ;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    ಅಳಿಸು gLanguageBreakFactories;
    gLanguageBreakFactories = ಶೂನ್ಯಸೂಚಕ;
    ಅಳಿಸು g_ಖಾಲಿಗಳು_String;
    g_ಖಾಲಿಗಳು_String = ಶೂನ್ಯಸೂಚಕ;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}
U_CDECL_ಕೊನೆ

U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV _deleteFactory(ಶೂನ್ಯ *obj) {
    ಅಳಿಸು (icu::LanguageBreakFactory *) obj;
}
U_CDECL_ಕೊನೆ
U_NAMESPACE_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV rbbiInit() {
    g_ಖಾಲಿಗಳು_String = ಹೊಸ UnicodeString();
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initLanguageFactories(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    U_ASSERT(gLanguageBreakFactories == ಶೂನ್ಯಸೂಚಕ);
    gLanguageBreakFactories = ಹೊಸ UStack(_deleteFactory, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (gLanguageBreakFactories != ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        Local_ಸೂಚಕ<ICULanguageBreakFactory> factory(ಹೊಸ ICULanguageBreakFactory(ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, ಸ್ಥಿತಿ_2);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &ಸ್ಥಿತಿ_2);
            ಆದರೆ (extra != ಶೂನ್ಯಸೂಚಕ) {
                gLanguageBreakFactories->push(extra, ಸ್ಥಿತಿ_2);
            }
#ಆದರೆ_ಅಂತ್ಯ
        }
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

ಶೂನ್ಯ ensureLanguageFactories(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale)
{
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ensureLanguageFactories(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

    int32_t i = gLanguageBreakFactories->size();
    ಸ್ಥಿರಾಂಕ LanguageBreakEngine *lbe = ಶೂನ್ಯಸೂಚಕ;
    ಆಗ (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        ಆದರೆ (lbe != ಶೂನ್ಯಸೂಚಕ) {
            ನಿಲ್ಲಿಸು;
        }
    }
    ಹಿಂತಿರುಗಿ lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
ಸ್ಥಿರಾಂಕ LanguageBreakEngine*
ನಿಯಮ_Based_Break_Iterator::getLanguageBreakEngine(UChar32 c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale) U_LIFETIME_BOUND {
    ಸ್ಥಿರಾಂಕ LanguageBreakEngine *lbe = ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

    ಆದರೆ (fLanguageBreakEngines == ಶೂನ್ಯಸೂಚಕ) {
        fLanguageBreakEngines = ಹೊಸ UStack(ಸ್ಥಿತಿ_2);
        ಆದರೆ (fLanguageBreakEngines == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಅಳಿಸು fLanguageBreakEngines;
            fLanguageBreakEngines = ಶೂನ್ಯಸೂಚಕ;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    ಆಗ (--i >= 0) {
        lbe = static_cast<ಸ್ಥಿರಾಂಕ LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        ಆದರೆ (lbe->handles(c, locale)) {
            ಹಿಂತಿರುಗಿ lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    ಆದರೆ (lbe != ಶೂನ್ಯಸೂಚಕ) {
        fLanguageBreakEngines->push((ಶೂನ್ಯ *)lbe, ಸ್ಥಿತಿ_2);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        ಹಿಂತಿರುಗಿ lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    ಆದರೆ (fUnhandledBreakEngine == ಶೂನ್ಯಸೂಚಕ) {
        fUnhandledBreakEngine = ಹೊಸ UnhandledEngine(ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && fUnhandledBreakEngine == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->ಸೇರಿಸು_Element_At(fUnhandledBreakEngine, 0, ಸ್ಥಿತಿ_2);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಅಳಿಸು fUnhandledBreakEngine;
            fUnhandledBreakEngine = ಶೂನ್ಯಸೂಚಕ;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    ಹಿಂತಿರುಗಿ fUnhandledBreakEngine;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_DRAFT_API
ಶೂನ್ಯ U_EXPORT2 ನಿಯಮ_Based_Break_Iterator::ರಿಜಿಸ್ಟರ್_External_Break_Engine(
                  ExternalBreakEngine* ಗಮ್ಯ_Adopt, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    Local_ಸೂಚಕ<ExternalBreakEngine> engine(ಗಮ್ಯ_Adopt, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    ensureLanguageFactories(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    gICULanguageBreakFactory->ಕೂಡಿಸು_External_Engine(engine.orphan(), ಸ್ಥಿತಿ_2);
}
#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_DRAFT_API */


ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::dumpTables() {
    f_ದತ್ತಾಂಶ->print_ದತ್ತಾಂಶ();
}

/**
 * Returns the description used to create this iterator
 */

ಸ್ಥಿರಾಂಕ UnicodeString&
ನಿಯಮ_Based_Break_Iterator::get_ನಿಯಮಗಳು() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಆದರೆ (f_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ f_ದತ್ತಾಂಶ->get_ನಿಯಮ_ಮೂಲ_String();
    } ಇಲ್ಲದಿದ್ದರೆ {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        ಹಿಂತಿರುಗಿ *g_ಖಾಲಿಗಳು_String;
    }
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */

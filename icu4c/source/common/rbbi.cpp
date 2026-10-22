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

#అన్తర్భావ "utypeinfo.h"  // for 'typeid' to work

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ <cinttypes>

#అన్తర్భావ "unicode/rbbi.h"
#అన్తర్భావ "unicode/schriter.h"
#అన్తర్భావ "unicode/uchriter.h"
#అన్తర్భావ "unicode/uclean.h"
#అన్తర్భావ "unicode/udata.h"

#అన్తర్భావ "brkeng.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "localsvc.h"
#అన్తర్భావ "rbbidata.h"
#అన్తర్భావ "rbbi_cache.h"
#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uvectr32.h"

#నిర్వచితమైతే RBBI_DEBUG
స్థిర UBool gTrace = అబద్ధం;
#అయితే_ముగింపు

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t ప్రారంభం_STATE = 1;

// The state-transition value indicating "stop"
constexpr int32_t STOP_STATE = 0;


UOBJECT_DEFINE_RTTI_IMPLEMENTATION(నియమం_Based_Break_Iterator)


//=======================================================================
// constructors
//=======================================================================

/**
 * Constructs a RuleBasedBreakIterator that uses the already-created
 * tables object that is passed in as a parameter.
 */
నియమం_Based_Break_Iterator::నియమం_Based_Break_Iterator(RBBIData_శీర్షిక* దత్తాంశం, UError_సంకేతం &స్థితి_2)
 : నియమం_Based_Break_Iterator(&స్థితి_2)
{
    f_దత్తాంశం = కొత్త RBBIDataWrapper(దత్తాంశం, స్థితి_2); // status checked in constructor
    ఒకవేళ (U_FAILURE(స్థితి_2)) {తిరిగి;}
    ఒకవేళ(f_దత్తాంశం == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    ఒకవేళ (f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత * పరిమాణం(int32_t)));
        ఒకవేళ (fLookAheadMatches == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
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
నియమం_Based_Break_Iterator::నియమం_Based_Break_Iterator(UData_జ్ఞాపకం* udm, UBool isPhraseBreaking,
        UError_సంకేతం &స్థితి_2) : నియమం_Based_Break_Iterator(udm, స్థితి_2)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
నియమం_Based_Break_Iterator::నియమం_Based_Break_Iterator(స్థిరరాశి uint8_t *compiled_నియమాలు,
                       uint32_t       నియమం_నిడివి,
                       UError_సంకేతం     &స్థితి_2)
 : నియమం_Based_Break_Iterator(&స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (compiled_నియమాలు == శూన్యసూచిక || నియమం_నిడివి < పరిమాణం(RBBIData_శీర్షిక)) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    స్థిరరాశి RBBIData_శీర్షిక* దత్తాంశం = reinterpret_cast<స్థిరరాశి RBBIData_శీర్షిక*>(compiled_నియమాలు);
    ఒకవేళ (దత్తాంశం->f_నిడివి > నియమం_నిడివి) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    f_దత్తాంశం = కొత్త RBBIDataWrapper(దత్తాంశం, RBBIDataWrapper::kDontAdopt, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {తిరిగి;}
    ఒకవేళ(f_దత్తాంశం == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    ఒకవేళ (f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత * పరిమాణం(int32_t)));
        ఒకవేళ (fLookAheadMatches == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
    }
}


//-------------------------------------------------------------------------------
//
//   Constructor   from a UDataMemory handle to precompiled break rules
//                 stored in an ICU data file.
//
//-------------------------------------------------------------------------------
నియమం_Based_Break_Iterator::నియమం_Based_Break_Iterator(UData_జ్ఞాపకం* udm, UError_సంకేతం &స్థితి_2)
 : నియమం_Based_Break_Iterator(&స్థితి_2)
{
    f_దత్తాంశం = కొత్త RBBIDataWrapper(udm, స్థితి_2); // status checked in constructor
    ఒకవేళ (U_FAILURE(స్థితి_2)) {తిరిగి;}
    ఒకవేళ(f_దత్తాంశం == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    ఒకవేళ (f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత * పరిమాణం(int32_t)));
        ఒకవేళ (fLookAheadMatches == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
నియమం_Based_Break_Iterator::నియమం_Based_Break_Iterator( స్థిరరాశి UnicodeString  &నియమాలు,
                                                UParseError          &parseError,
                                                UError_సంకేతం           &స్థితి_2)
 : నియమం_Based_Break_Iterator(&స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {తిరిగి;}
    నియమం_Based_Break_Iterator *bi = (నియమం_Based_Break_Iterator *)
        RBBIRuleBuilder::create_నియమం_Based_Break_Iterator(నియమాలు, &parseError, స్థితి_2);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        *ఇది = *bi;
        తొలగించు bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
నియమం_Based_Break_Iterator::నియమం_Based_Break_Iterator()
 : నియమం_Based_Break_Iterator(శూన్యసూచిక)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
నియమం_Based_Break_Iterator::నియమం_Based_Break_Iterator(UError_సంకేతం *స్థితి_2) {
    UError_సంకేతం ec = U_ZERO_ERROR;
    ఒకవేళ (స్థితి_2 == శూన్యసూచిక) {
        స్థితి_2 = &ec;
    }
    utext_openUChars(&f_పాఠ్యం, శూన్యసూచిక, 0, స్థితి_2);
    Local_సూచిక<DictionaryCache> lpDictionaryCache(కొత్త DictionaryCache(ఇది, *స్థితి_2), *స్థితి_2);
    Local_సూచిక<BreakCache> lpBreakCache(కొత్త BreakCache(ఇది, *స్థితి_2), *స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        f_Error_సంకేతం = *స్థితి_2;
        తిరిగి;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#నిర్వచితమైతే RBBI_DEBUG
    స్థిర UBool debug_Init_పూర్తి = అబద్ధం;
    ఒకవేళ (debug_Init_పూర్తి == అబద్ధం) {
        అక్షరం *debugEnv = getenv("U_RBBIDEBUG");
        ఒకవేళ (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = నిజం;
        }
        debug_Init_పూర్తి = నిజం;
    }
#అయితే_ముగింపు
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
నియమం_Based_Break_Iterator::నియమం_Based_Break_Iterator(స్థిరరాశి నియమం_Based_Break_Iterator& other)
: నియమం_Based_Break_Iterator()
{
    *ఇది = other;
}


/**
 * Destructor
 */
నియమం_Based_Break_Iterator::~నియమం_Based_Break_Iterator() {
    ఒకవేళ (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        తొలగించు fCharIter;
    }
    fCharIter = శూన్యసూచిక;

    utext_close(&f_పాఠ్యం);

    ఒకవేళ (f_దత్తాంశం != శూన్యసూచిక) {
        f_దత్తాంశం->removeReference();
        f_దత్తాంశం = శూన్యసూచిక;
    }
    తొలగించు fBreakCache;
    fBreakCache = శూన్యసూచిక;

    తొలగించు fDictionaryCache;
    fDictionaryCache = శూన్యసూచిక;

    తొలగించు fLanguageBreakEngines;
    fLanguageBreakEngines = శూన్యసూచిక;

    తొలగించు fUnhandledBreakEngine;
    fUnhandledBreakEngine = శూన్యసూచిక;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = శూన్యసూచిక;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
నియమం_Based_Break_Iterator&
నియమం_Based_Break_Iterator::క్రియాకారకం=(స్థిరరాశి నియమం_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    ఒకవేళ (ఇది == &that) {
        తిరిగి *ఇది;
    }
    BreakIterator::క్రియాకారకం=(that);

    ఒకవేళ (fLanguageBreakEngines != శూన్యసూచిక) {
        తొలగించు fLanguageBreakEngines;
        fLanguageBreakEngines = శూన్యసూచిక;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    utext_clone(&f_పాఠ్యం, &that.f_పాఠ్యం, అబద్ధం, నిజం, &స్థితి_2);

    ఒకవేళ (fCharIter != &fSCharIter) {
        తొలగించు fCharIter;
    }
    fCharIter = &fSCharIter;

    ఒకవేళ (that.fCharIter != శూన్యసూచిక && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    ఒకవేళ (fCharIter == శూన్యసూచిక) {
        fCharIter = &fSCharIter;
    }

    ఒకవేళ (f_దత్తాంశం != శూన్యసూచిక) {
        f_దత్తాంశం->removeReference();
        f_దత్తాంశం = శూన్యసూచిక;
    }
    ఒకవేళ (that.f_దత్తాంశం != శూన్యసూచిక) {
        f_దత్తాంశం = that.f_దత్తాంశం->కూడు_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = శూన్యసూచిక;
    ఒకవేళ (f_దత్తాంశం && f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత * పరిమాణం(int32_t)));
    }


    fPosition = that.fPosition;
    f_నియమం_స్థితి_సూచీ = that.f_నియమం_స్థితి_సూచీ;
    f_పూర్తి = that.f_పూర్తి;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_నియమం_స్థితి_సూచీ);
    fDictionaryCache->reset();

    తిరిగి *ఇది;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
నియమం_Based_Break_Iterator*
నియమం_Based_Break_Iterator::clone() స్థిరరాశి {
    తిరిగి కొత్త నియమం_Based_Break_Iterator(*ఇది);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
తార్కిక
నియమం_Based_Break_Iterator::క్రియాకారకం==(స్థిరరాశి BreakIterator& that) స్థిరరాశి {
    ఒకవేళ (typeid(*ఇది) != typeid(that)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (ఇది == &that) {
        తిరిగి నిజం;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    స్థిరరాశి నియమం_Based_Break_Iterator& that2 = static_cast<స్థిరరాశి నియమం_Based_Break_Iterator&>(that);

    ఒకవేళ (!utext_equals(&f_పాఠ్యం, &that2.f_పాఠ్యం)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        తిరిగి అబద్ధం;
    }

    ఒకవేళ (!(fPosition == that2.fPosition &&
            f_నియమం_స్థితి_సూచీ == that2.f_నియమం_స్థితి_సూచీ &&
            f_పూర్తి == that2.f_పూర్తి &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        తిరిగి అబద్ధం;
    }

    ఒకవేళ (that2.f_దత్తాంశం == f_దత్తాంశం ||
        (f_దత్తాంశం != శూన్యసూచిక && that2.f_దత్తాంశం != శూన్యసూచిక && *that2.f_దత్తాంశం == *f_దత్తాంశం)) {
            // The two break iterators are using the same rules.
            తిరిగి నిజం;
        }
    తిరిగి అబద్ధం;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
నియమం_Based_Break_Iterator::hash_సంకేతం() స్థిరరాశి {
    int32_t   hash = 0;
    ఒకవేళ (f_దత్తాంశం != శూన్యసూచిక) {
        hash = f_దత్తాంశం->hash_సంకేతం();
    }
    తిరిగి hash;
}


శూన్య నియమం_Based_Break_Iterator::సమితి_పాఠ్యం(UText *ut, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&f_పాఠ్యం, ut, అబద్ధం, నిజం, &స్థితి_2);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.సమితి_పాఠ్యం(u"", 0);

    ఒకవేళ (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        తొలగించు fCharIter;
    }
    fCharIter = &fSCharIter;

    ఇది->first();
}


UText* నియమం_Based_Break_Iterator::getUText(UText* fill_లోపల U_LIFETIME_BOUND, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    UText *ఫలితం = utext_clone(fill_లోపల, &f_పాఠ్యం, అబద్ధం, నిజం, &స్థితి_2);
    తిరిగి ఫలితం;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
నియమం_Based_Break_Iterator::get_పాఠ్యం() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
శూన్య
నియమం_Based_Break_Iterator::adopt_పాఠ్యం(CharacterIterator* new_పాఠ్యం) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    ఒకవేళ (fCharIter != &fSCharIter) {
        తొలగించు fCharIter;
    }

    fCharIter = new_పాఠ్యం;
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    ఒకవేళ (new_పాఠ్యం==శూన్యసూచిక || new_పాఠ్యం->ప్రారంభం_సూచీ() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&f_పాఠ్యం, శూన్యసూచిక, 0, &స్థితి_2);
    } లేకపోతే {
        utext_openCharacterIterator(&f_పాఠ్యం, new_పాఠ్యం, &స్థితి_2);
    }
    ఇది->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
శూన్య
నియమం_Based_Break_Iterator::సమితి_పాఠ్యం(స్థిరరాశి UnicodeString& new_పాఠ్యం) {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&f_పాఠ్యం, &new_పాఠ్యం, &స్థితి_2);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.సమితి_పాఠ్యం(new_పాఠ్యం.get_బఫరు(), new_పాఠ్యం.length());

    ఒకవేళ (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        తొలగించు fCharIter;
    }
    fCharIter = &fSCharIter;

    ఇది->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
నియమం_Based_Break_Iterator&
నియమం_Based_Break_Iterator::refresh_ఆదానం_పాఠ్యం(UText* input, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి *ఇది;
    }
    ఒకవేళ (input == శూన్యసూచిక) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి *ఇది;
    }
    int64_t pos = utext_get_Native_సూచీ(&f_పాఠ్యం);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&f_పాఠ్యం, input, అబద్ధం, నిజం, &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి *ఇది;
    }
    utext_సమితి_Native_సూచీ(&f_పాఠ్యం, pos);
    ఒకవేళ (utext_get_Native_సూచీ(&f_పాఠ్యం) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }
    తిరిగి *ఇది;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t నియమం_Based_Break_Iterator::first() {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    ఒకవేళ (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, స్థితి_2);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    తిరిగి 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t నియమం_Based_Break_Iterator::చివరి() {
    int32_t చివర_Pos = static_cast<int32_t>(utext_native_నిడివి(&f_పాఠ్యం));
    UBool చివర_Should_Be_Boundary = isBoundary(చివర_Pos);      // Has side effect of setting iterator position.
    (శూన్య)చివర_Should_Be_Boundary;
    U_ASSERT(చివర_Should_Be_Boundary);
    U_ASSERT(fPosition == చివర_Pos);
    తిరిగి చివర_Pos;
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
int32_t నియమం_Based_Break_Iterator::next(int32_t n) {
    int32_t ఫలితం = 0;
    ఒకవేళ (n > 0) {
        కొరకు (; n > 0 && ఫలితం != UBRK_పూర్తి; --n) {
            ఫలితం = next();
        }
    } లేకపోతే ఒకవేళ (n < 0) {
        కొరకు (; n < 0 && ఫలితం != UBRK_పూర్తి; ++n) {
            ఫలితం = మునుపటి();
        }
    } లేకపోతే {
        ఫలితం = current();
    }
    తిరిగి ఫలితం;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t నియమం_Based_Break_Iterator::next() {
    fBreakCache->next();
    తిరిగి f_పూర్తి ? UBRK_పూర్తి : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t నియమం_Based_Break_Iterator::మునుపటి() {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    fBreakCache->మునుపటి(స్థితి_2);
    తిరిగి f_పూర్తి ? UBRK_పూర్తి : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t నియమం_Based_Break_Iterator::following(int32_t ప్రారంభం_Pos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    ఒకవేళ (ప్రారంభం_Pos < 0) {
        తిరిగి first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_సమితి_Native_సూచీ(&f_పాఠ్యం, ప్రారంభం_Pos);
    ప్రారంభం_Pos = static_cast<int32_t>(utext_get_Native_సూచీ(&f_పాఠ్యం));

    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    fBreakCache->following(ప్రారంభం_Pos, స్థితి_2);
    తిరిగి f_పూర్తి ? UBRK_పూర్తి : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t నియమం_Based_Break_Iterator::preceding(int32_t స్థానభ్రంశం) {
    ఒకవేళ (స్థానభ్రంశం > utext_native_నిడివి(&f_పాఠ్యం)) {
        తిరిగి చివరి();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_సమితి_Native_సూచీ(&f_పాఠ్యం, స్థానభ్రంశం);
    int32_t adjusted_స్థానభ్రంశం = static_cast<int32_t>(utext_get_Native_సూచీ(&f_పాఠ్యం));

    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_స్థానభ్రంశం, స్థితి_2);
    తిరిగి f_పూర్తి ? UBRK_పూర్తి : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool నియమం_Based_Break_Iterator::isBoundary(int32_t స్థానభ్రంశం) {
    // out-of-range indexes are never boundary positions
    ఒకవేళ (స్థానభ్రంశం < 0) {
        first();       // For side effects on current position, tag values.
        తిరిగి అబద్ధం;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_సమితి_Native_సూచీ(&f_పాఠ్యం, స్థానభ్రంశం);
    int32_t adjusted_స్థానభ్రంశం = static_cast<int32_t>(utext_get_Native_సూచీ(&f_పాఠ్యం));

    తార్కిక ఫలితం = అబద్ధం;
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    ఒకవేళ (fBreakCache->seek(adjusted_స్థానభ్రంశం) || fBreakCache->populateNear(adjusted_స్థానభ్రంశం, స్థితి_2)) {
        ఫలితం = (fBreakCache->current() == స్థానభ్రంశం);
    }

    ఒకవేళ (ఫలితం && adjusted_స్థానభ్రంశం < స్థానభ్రంశం && utext_char32At(&f_పాఠ్యం, స్థానభ్రంశం) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (!ఫలితం) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    తిరిగి ఫలితం;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t నియమం_Based_Break_Iterator::current() స్థిరరాశి {
    తిరిగి fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
గణన RBBIRun_బహుళకం {
    RBBI_ప్రారంభం,     // state machine processing is before first char of input
    RBBI_నిరంతరం,       // state machine processing is in the user text
    RBBI_చివర        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
స్థిర అంతఃవరుస uint16_t TrieFunc8(స్థిరరాశి UCPTrie *trie, UChar32 c) {
    తిరిగి UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

స్థిర అంతఃవరుస uint16_t TrieFunc16(స్థిరరాశి UCPTrie *trie, UChar32 c) {
    తిరిగి UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t నియమం_Based_Break_Iterator::handle_తదుపరి() {
    స్థిరరాశి RBBIState_పట్టిక *statetable = f_దత్తాంశం->f_Forward_పట్టిక;
    తార్కిక use8_బిట్లు_Trie = ucptrie_get_విలువ_వెడల్పు(f_దత్తాంశం->fTrie) == UCPTRIE_విలువ_బిట్లు_8;
    ఒకవేళ (statetable->fFlags & RBBI_8_బిట్లు_అడ్డువరుసలు) {
        ఒకవేళ (use8_బిట్లు_Trie) {
            తిరిగి handle_తదుపరి<RBBIState_పట్టిక_Row8, TrieFunc8>();
        } లేకపోతే {
            తిరిగి handle_తదుపరి<RBBIState_పట్టిక_Row8, TrieFunc16>();
        }
    } లేకపోతే {
        ఒకవేళ (use8_బిట్లు_Trie) {
            తిరిగి handle_తదుపరి<RBBIState_పట్టిక_Row16, TrieFunc8>();
        } లేకపోతే {
            తిరిగి handle_తదుపరి<RBBIState_పట్టిక_Row16, TrieFunc16>();
        }
    }
}

int32_t నియమం_Based_Break_Iterator::handle_సురక్షిత_మునుపటి(int32_t fromPosition) {
    స్థిరరాశి RBBIState_పట్టిక *statetable = f_దత్తాంశం->f_తిరగేయి_పట్టిక;
    తార్కిక use8_బిట్లు_Trie = ucptrie_get_విలువ_వెడల్పు(f_దత్తాంశం->fTrie) == UCPTRIE_విలువ_బిట్లు_8;
    ఒకవేళ (statetable->fFlags & RBBI_8_బిట్లు_అడ్డువరుసలు) {
        ఒకవేళ (use8_బిట్లు_Trie) {
            తిరిగి handle_సురక్షిత_మునుపటి<RBBIState_పట్టిక_Row8, TrieFunc8>(fromPosition);
        } లేకపోతే {
            తిరిగి handle_సురక్షిత_మునుపటి<RBBIState_పట్టిక_Row8, TrieFunc16>(fromPosition);
        }
    } లేకపోతే {
        ఒకవేళ (use8_బిట్లు_Trie) {
            తిరిగి handle_సురక్షిత_మునుపటి<RBBIState_పట్టిక_Row16, TrieFunc8>(fromPosition);
        } లేకపోతే {
            తిరిగి handle_సురక్షిత_మునుపటి<RBBIState_పట్టిక_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
మూస <రకం_పేరు అడ్డువరుస_Type, నియమం_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t నియమం_Based_Break_Iterator::handle_తదుపరి() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_బహుళకం         బహుళకం;

    అడ్డువరుస_Type             *అడ్డువరుస;
    UChar32             c;
    int32_t             ఫలితం             = 0;
    int32_t             initialPosition    = 0;
    స్థిరరాశి RBBIState_పట్టిక *statetable       = f_దత్తాంశం->f_Forward_పట్టిక;
    స్థిరరాశి అక్షరం         *పట్టిక_దత్తాంశం          = statetable->f_పట్టిక_దత్తాంశం;
    uint32_t            పట్టిక_అడ్డువరుస_Len        = statetable->f_అడ్డువరుస_Len;
    uint32_t            dict_ప్రారంభం          = statetable->f_Dict_Categories_ప్రారంభం;
    #నిర్వచితమైతే RBBI_DEBUG
        ఒకవేళ (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #అయితే_ముగింపు

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_నియమం_స్థితి_సూచీ = 0;

    f_Dictionary_Char_లెక్క = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&f_పాఠ్యం, initialPosition);
    ఫలితం          = initialPosition;
    c               = UTEXT_NEXT32(&f_పాఠ్యం);
    ఒకవేళ (c==U_SENTINEL) {
        f_పూర్తి = నిజం;
        తిరిగి UBRK_పూర్తి;
    }

    //  Set the initial state for the state machine
    state = ప్రారంభం_STATE;
    అడ్డువరుస = (అడ్డువరుస_Type *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (పట్టిక_దత్తాంశం + పట్టిక_అడ్డువరుస_Len * state);


    బహుళకం     = RBBI_నిరంతరం;


    // loop until we reach the end of the text or transition to state 0
    //
    కొరకు (;;) {
        ఒకవేళ (c == U_SENTINEL) {
            // Reached end of input string.
            ఒకవేళ (బహుళకం == RBBI_చివర) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                ఆపు;
            }
            // Run the loop one last time with the fake end-of-input character category.
            బహుళకం = RBBI_చివర;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        ఒకవేళ (బహుళకం == RBBI_నిరంతరం) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_దత్తాంశం->fTrie, c);
            f_Dictionary_Char_లెక్క += (category >= dict_ప్రారంభం);
        }

       #నిర్వచితమైతే RBBI_DEBUG
            ఒకవేళ (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_సూచీ(&f_పాఠ్యం));
                ఒకవేళ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } లేకపోతే {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #అయితే_ముగింపు

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_దత్తాంశం->f_శీర్షిక->f_Cat_లెక్క);
        state = అడ్డువరుస->f_తదుపరి_State[category];  /*Not accessing beyond memory*/
        అడ్డువరుస = (అడ్డువరుస_Type *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (పట్టిక_దత్తాంశం + పట్టిక_అడ్డువరుస_Len * state);


        uint16_t accepting = అడ్డువరుస->fAccepting;
        ఒకవేళ (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            ఒకవేళ (బహుళకం != RBBI_ప్రారంభం) {
                ఫలితం = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_పాఠ్యం));
            }
            f_నియమం_స్థితి_సూచీ = అడ్డువరుస->f_ట్యాగులు_Idx;   // Remember the break status (tag) values.
        } లేకపోతే ఒకవేళ (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత);
            int32_t lookahead_ఫలితం = fLookAheadMatches[accepting];
            ఒకవేళ (lookahead_ఫలితం >= 0) {
                f_నియమం_స్థితి_సూచీ = అడ్డువరుస->f_ట్యాగులు_Idx;
                fPosition = lookahead_ఫలితం;
                తిరిగి lookahead_ఫలితం;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t నియమం = అడ్డువరుస->fLookAhead;
        U_ASSERT(నియమం == 0 || నియమం > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(నియమం == 0 || నియమం < f_దత్తాంశం->f_Forward_పట్టిక->f_Look_Ahead_Results_కొలత);
        ఒకవేళ (నియమం > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_పాఠ్యం));
            fLookAheadMatches[నియమం] = pos;
        }

        ఒకవేళ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            ఆపు;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        ఒకవేళ (బహుళకం == RBBI_నిరంతరం) {
            c = UTEXT_NEXT32(&f_పాఠ్యం);
        } లేకపోతే {
            ఒకవేళ (బహుళకం == RBBI_ప్రారంభం) {
                బహుళకం = RBBI_నిరంతరం;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    ఒకవేళ (ఫలితం == initialPosition) {
        utext_సమితి_Native_సూచీ(&f_పాఠ్యం, initialPosition);
        utext_next32(&f_పాఠ్యం);
        ఫలితం = static_cast<int32_t>(utext_get_Native_సూచీ(&f_పాఠ్యం));
        f_నియమం_స్థితి_సూచీ = 0;
    }

    // Leave the iterator at our result position.
    fPosition = ఫలితం;
    #నిర్వచితమైతే RBBI_DEBUG
        ఒకవేళ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", ఫలితం);
        }
    #అయితే_ముగింపు
    తిరిగి ఫలితం;
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
మూస <రకం_పేరు అడ్డువరుస_Type, నియమం_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t నియమం_Based_Break_Iterator::handle_సురక్షిత_మునుపటి(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    అడ్డువరుస_Type            *అడ్డువరుస;
    UChar32             c;
    int32_t             ఫలితం          = 0;

    స్థిరరాశి RBBIState_పట్టిక *state_పట్టిక = f_దత్తాంశం->f_తిరగేయి_పట్టిక;
    UTEXT_SETNATIVEINDEX(&f_పాఠ్యం, fromPosition);
    #నిర్వచితమైతే RBBI_DEBUG
        ఒకవేళ (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #అయితే_ముగింపు

    // if we're already at the start of the text, return DONE.
    ఒకవేళ (f_దత్తాంశం == శూన్యసూచిక || UTEXT_GETNATIVEINDEX(&f_పాఠ్యం)==0) {
        తిరిగి BreakIterator::పూర్తి_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&f_పాఠ్యం);
    state = ప్రారంభం_STATE;
    అడ్డువరుస = (అడ్డువరుస_Type *)
            (state_పట్టిక->f_పట్టిక_దత్తాంశం + (state_పట్టిక->f_అడ్డువరుస_Len * state));

    // loop until we reach the start of the text or transition to state 0
    //
    కొరకు (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&f_పాఠ్యం)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_దత్తాంశం->fTrie, c);

        #నిర్వచితమైతే RBBI_DEBUG
            ఒకవేళ (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_సూచీ(&f_పాఠ్యం));
                ఒకవేళ (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } లేకపోతే {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #అయితే_ముగింపు

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_దత్తాంశం->f_శీర్షిక->f_Cat_లెక్క);
        state = అడ్డువరుస->f_తదుపరి_State[category];  /*Not accessing beyond memory*/
        అడ్డువరుస = (అడ్డువరుస_Type *)
            (state_పట్టిక->f_పట్టిక_దత్తాంశం + (state_పట్టిక->f_అడ్డువరుస_Len * state));

        ఒకవేళ (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            ఆపు;
        }
    }

    // The state machine is done.  Check whether it found a match...
    ఫలితం = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&f_పాఠ్యం));
    #నిర్వచితమైతే RBBI_DEBUG
        ఒకవేళ (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", ఫలితం);
        }
    #అయితే_ముగింపు
    తిరిగి ఫలితం;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  నియమం_Based_Break_Iterator::get_నియమం_స్థితి() స్థిరరాశి {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_నియమం_స్థితి_సూచీ + f_దత్తాంశం->f_నియమం_స్థితి_పట్టిక[f_నియమం_స్థితి_సూచీ];
    int32_t  ట్యాగ్_Val = f_దత్తాంశం->f_నియమం_స్థితి_పట్టిక[idx];

    తిరిగి ట్యాగ్_Val;
}


int32_t నియమం_Based_Break_Iterator::get_నియమం_స్థితి_Vec(
             int32_t *fill_లోపల_Vec, int32_t capacity, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }

    int32_t  అంకె_Vals = f_దత్తాంశం->f_నియమం_స్థితి_పట్టిక[f_నియమం_స్థితి_సూచీ];
    int32_t  అంకె_Vals_గమ్యం_Copy = అంకె_Vals;
    ఒకవేళ (అంకె_Vals > capacity) {
        స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
        అంకె_Vals_గమ్యం_Copy = capacity;
    }
    పూర్ణాంకం i;
    కొరకు (i=0; i<అంకె_Vals_గమ్యం_Copy; i++) {
        fill_లోపల_Vec[i] = f_దత్తాంశం->f_నియమం_స్థితి_పట్టిక[f_నియమం_స్థితి_సూచీ + i + 1];
    }
    తిరిగి అంకె_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
స్థిరరాశి uint8_t* నియమం_Based_Break_Iterator::get_ద్విమాన_నియమాలు(uint32_t& length) U_LIFETIME_BOUND {
    స్థిరరాశి uint8_t  *retPtr = శూన్యసూచిక;
    length = 0;

    ఒకవేళ (f_దత్తాంశం != శూన్యసూచిక) {
        retPtr = reinterpret_cast<స్థిరరాశి uint8_t*>(f_దత్తాంశం->f_శీర్షిక);
        length = f_దత్తాంశం->f_శీర్షిక->f_నిడివి;
    }
    తిరిగి retPtr;
}


నియమం_Based_Break_Iterator *నియమం_Based_Break_Iterator::create_బఫరు_Clone(
        శూన్య * /*stackBuffer*/, int32_t &బఫరు_కొలత, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (బఫరు_కొలత == 0) {
        బఫరు_కొలత = 1;  // preflighting for deprecated functionality
        తిరిగి శూన్యసూచిక;
    }

    BreakIterator *clonedBI = clone();
    ఒకవేళ (clonedBI == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే {
        స్థితి_2 = U_SAFECLONE_ALLOCATED_WARNING;
    }
    తిరిగి (నియమం_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_చివర


స్థిర icu::UStack *gLanguageBreakFactories = శూన్యసూచిక;
స్థిర స్థిరరాశి icu::UnicodeString *g_ఖాళీలు_String = శూన్యసూచిక;
స్థిర icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
స్థిర icu::UInitOnce gRBBIInitOnce {};
స్థిర icu::ICULanguageBreakFactory *gICULanguageBreakFactory = శూన్యసూచిక;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    తొలగించు gLanguageBreakFactories;
    gLanguageBreakFactories = శూన్యసూచిక;
    తొలగించు g_ఖాళీలు_String;
    g_ఖాళీలు_String = శూన్యసూచిక;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    తిరిగి నిజం;
}
U_CDECL_చివర

U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV _deleteFactory(శూన్య *obj) {
    తొలగించు (icu::LanguageBreakFactory *) obj;
}
U_CDECL_చివర
U_NAMESPACE_BEGIN

స్థిర శూన్య U_CALLCONV rbbiInit() {
    g_ఖాళీలు_String = కొత్త UnicodeString();
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

స్థిర శూన్య U_CALLCONV initLanguageFactories(UError_సంకేతం& స్థితి_2) {
    U_ASSERT(gLanguageBreakFactories == శూన్యసూచిక);
    gLanguageBreakFactories = కొత్త UStack(_deleteFactory, శూన్యసూచిక, స్థితి_2);
    ఒకవేళ (gLanguageBreakFactories != శూన్యసూచిక && U_SUCCESS(స్థితి_2)) {
        Local_సూచిక<ICULanguageBreakFactory> factory(కొత్త ICULanguageBreakFactory(స్థితి_2), స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, స్థితి_2);
#నిర్వచితమైతే U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &స్థితి_2);
            ఒకవేళ (extra != శూన్యసూచిక) {
                gLanguageBreakFactories->push(extra, స్థితి_2);
            }
#అయితే_ముగింపు
        }
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

శూన్య ensureLanguageFactories(UError_సంకేతం& స్థితి_2) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, స్థితి_2);
}

స్థిర స్థిరరాశి LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, స్థిరరాశి అక్షరం* locale)
{
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    ensureLanguageFactories(స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి శూన్యసూచిక;

    int32_t i = gLanguageBreakFactories->size();
    స్థిరరాశి LanguageBreakEngine *lbe = శూన్యసూచిక;
    అయితే (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        ఒకవేళ (lbe != శూన్యసూచిక) {
            ఆపు;
        }
    }
    తిరిగి lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
స్థిరరాశి LanguageBreakEngine*
నియమం_Based_Break_Iterator::getLanguageBreakEngine(UChar32 c, స్థిరరాశి అక్షరం* locale) U_LIFETIME_BOUND {
    స్థిరరాశి LanguageBreakEngine *lbe = శూన్యసూచిక;
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

    ఒకవేళ (fLanguageBreakEngines == శూన్యసూచిక) {
        fLanguageBreakEngines = కొత్త UStack(స్థితి_2);
        ఒకవేళ (fLanguageBreakEngines == శూన్యసూచిక || U_FAILURE(స్థితి_2)) {
            తొలగించు fLanguageBreakEngines;
            fLanguageBreakEngines = శూన్యసూచిక;
            తిరిగి శూన్యసూచిక;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    అయితే (--i >= 0) {
        lbe = static_cast<స్థిరరాశి LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        ఒకవేళ (lbe->handles(c, locale)) {
            తిరిగి lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    ఒకవేళ (lbe != శూన్యసూచిక) {
        fLanguageBreakEngines->push((శూన్య *)lbe, స్థితి_2);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        తిరిగి lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    ఒకవేళ (fUnhandledBreakEngine == శూన్యసూచిక) {
        fUnhandledBreakEngine = కొత్త UnhandledEngine(స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2) && fUnhandledBreakEngine == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->చొప్పించు_Element_At(fUnhandledBreakEngine, 0, స్థితి_2);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తొలగించు fUnhandledBreakEngine;
            fUnhandledBreakEngine = శూన్యసూచిక;
            తిరిగి శూన్యసూచిక;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    తిరిగి fUnhandledBreakEngine;
}

#నిర్వచితం_కాకపోతే U_HIDE_DRAFT_API
శూన్య U_EXPORT2 నియమం_Based_Break_Iterator::రిజిస్టరు_External_Break_Engine(
                  ExternalBreakEngine* గమ్యం_Adopt, UError_సంకేతం& స్థితి_2) {
    Local_సూచిక<ExternalBreakEngine> engine(గమ్యం_Adopt, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    ensureLanguageFactories(స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    gICULanguageBreakFactory->కూడు_External_Engine(engine.orphan(), స్థితి_2);
}
#అయితే_ముగింపు  /* U_HIDE_DRAFT_API */


శూన్య నియమం_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

శూన్య నియమం_Based_Break_Iterator::dumpTables() {
    f_దత్తాంశం->print_దత్తాంశం();
}

/**
 * Returns the description used to create this iterator
 */

స్థిరరాశి UnicodeString&
నియమం_Based_Break_Iterator::get_నియమాలు() స్థిరరాశి U_LIFETIME_BOUND {
    ఒకవేళ (f_దత్తాంశం != శూన్యసూచిక) {
        తిరిగి f_దత్తాంశం->get_నియమం_ఆధారం_String();
    } లేకపోతే {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        తిరిగి *g_ఖాళీలు_String;
    }
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */

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

#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव <cinttypes>

#अन्तर्भाव "unicode/rbbi.h"
#अन्तर्भाव "unicode/schriter.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/uclean.h"
#अन्तर्भाव "unicode/udata.h"

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "localsvc.h"
#अन्तर्भाव "rbbidata.h"
#अन्तर्भाव "rbbi_cache.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uvectr32.h"

#यदि_परिभाषितम् RBBI_DEBUG
स्थिर UBool gTrace = असत्य;
#यदि_अन्तः

U_NAMESPACE_BEGIN

// The state number of the starting state
constexpr int32_t START_STATE = 1;

// The state-transition value indicating "stop"
constexpr int32_t STOP_STATE = 0;


UOBJECT_DEFINE_RTTI_IMPLEMENTATION(नियमः_Based_Break_Iterator)


//=======================================================================
// constructors
//=======================================================================

/**
 * Constructs a RuleBasedBreakIterator that uses the already-created
 * tables object that is passed in as a parameter.
 */
नियमः_Based_Break_Iterator::नियमः_Based_Break_Iterator(RBBIData_शीर्षकम्* दत्त, UError_संहिता &स्थितिः)
 : नियमः_Based_Break_Iterator(&स्थितिः)
{
    f_दत्त = नूतन RBBIDataWrapper(दत्त, स्थितिः); // status checked in constructor
    यदि (U_FAILURE(स्थितिः)) {प्रत्यागम;}
    यदि(f_दत्त == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    यदि (f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार * परिमाण(int32_t)));
        यदि (fLookAheadMatches == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
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
नियमः_Based_Break_Iterator::नियमः_Based_Break_Iterator(UData_स्मृतिः* udm, UBool isPhraseBreaking,
        UError_संहिता &स्थितिः) : नियमः_Based_Break_Iterator(udm, स्थितिः)
{
    fIsPhraseBreaking = isPhraseBreaking;
}

//
//  Construct from precompiled binary rules (tables).  This constructor is public API,
//  taking the rules as a (const uint8_t *) to match the type produced by getBinaryRules().
//
नियमः_Based_Break_Iterator::नियमः_Based_Break_Iterator(ध्रुव uint8_t *compiled_नियमाः,
                       uint32_t       नियमः_दीर्घता,
                       UError_संहिता     &स्थितिः)
 : नियमः_Based_Break_Iterator(&स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (compiled_नियमाः == शून्यसूचकः || नियमः_दीर्घता < परिमाण(RBBIData_शीर्षकम्)) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    ध्रुव RBBIData_शीर्षकम्* दत्त = reinterpret_cast<ध्रुव RBBIData_शीर्षकम्*>(compiled_नियमाः);
    यदि (दत्त->f_दीर्घता > नियमः_दीर्घता) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    f_दत्त = नूतन RBBIDataWrapper(दत्त, RBBIDataWrapper::kDontAdopt, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {प्रत्यागम;}
    यदि(f_दत्त == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    यदि (f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार * परिमाण(int32_t)));
        यदि (fLookAheadMatches == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
    }
}


//-------------------------------------------------------------------------------
//
//   Constructor   from a UDataMemory handle to precompiled break rules
//                 stored in an ICU data file.
//
//-------------------------------------------------------------------------------
नियमः_Based_Break_Iterator::नियमः_Based_Break_Iterator(UData_स्मृतिः* udm, UError_संहिता &स्थितिः)
 : नियमः_Based_Break_Iterator(&स्थितिः)
{
    f_दत्त = नूतन RBBIDataWrapper(udm, स्थितिः); // status checked in constructor
    यदि (U_FAILURE(स्थितिः)) {प्रत्यागम;}
    यदि(f_दत्त == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    यदि (f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार * परिमाण(int32_t)));
        यदि (fLookAheadMatches == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
    }
}



//-------------------------------------------------------------------------------
//
//   Constructor       from a set of rules supplied as a string.
//
//-------------------------------------------------------------------------------
नियमः_Based_Break_Iterator::नियमः_Based_Break_Iterator( ध्रुव UnicodeString  &नियमाः,
                                                UParseError          &parseError,
                                                UError_संहिता           &स्थितिः)
 : नियमः_Based_Break_Iterator(&स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {प्रत्यागम;}
    नियमः_Based_Break_Iterator *bi = (नियमः_Based_Break_Iterator *)
        RBBIRuleBuilder::create_नियमः_Based_Break_Iterator(नियमाः, &parseError, स्थितिः);
    // Note:  This is a bit awkward.  The RBBI ruleBuilder has a factory method that
    //        creates and returns a complete RBBI.  From here, in a constructor, we
    //        can't just return the object created by the builder factory, hence
    //        the assignment of the factory created object to "this".
    यदि (U_SUCCESS(स्थितिः)) {
        *एतत् = *bi;
        विलोपय bi;
    }
}


//-------------------------------------------------------------------------------
//
// Default Constructor.      Create an empty shell that can be set up later.
//                           Used when creating a RuleBasedBreakIterator from a set
//                           of rules.
//-------------------------------------------------------------------------------
नियमः_Based_Break_Iterator::नियमः_Based_Break_Iterator()
 : नियमः_Based_Break_Iterator(शून्यसूचकः)
{
}

/**
 * Simple Constructor with an error code.
 * Handles common initialization for all other constructors.
 */
नियमः_Based_Break_Iterator::नियमः_Based_Break_Iterator(UError_संहिता *स्थितिः) {
    UError_संहिता ec = U_ZERO_ERROR;
    यदि (स्थितिः == शून्यसूचकः) {
        स्थितिः = &ec;
    }
    utext_openUChars(&fText, शून्यसूचकः, 0, स्थितिः);
    LocalPointer<DictionaryCache> lpDictionaryCache(नूतन DictionaryCache(एतत्, *स्थितिः), *स्थितिः);
    LocalPointer<BreakCache> lpBreakCache(नूतन BreakCache(एतत्, *स्थितिः), *स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        f_Error_संहिता = *स्थितिः;
        प्रत्यागम;
    }
    fDictionaryCache = lpDictionaryCache.orphan();
    fBreakCache = lpBreakCache.orphan();

#यदि_परिभाषितम् RBBI_DEBUG
    स्थिर UBool debug_Init_समाप्तम् = असत्य;
    यदि (debug_Init_समाप्तम् == असत्य) {
        वर्ण *debugEnv = getenv("U_RBBIDEBUG");
        यदि (debugEnv && uprv_strstr(debugEnv, "trace")) {
            gTrace = सत्य;
        }
        debug_Init_समाप्तम् = सत्य;
    }
#यदि_अन्तः
}


//-------------------------------------------------------------------------------
//
//   Copy constructor.  Will produce a break iterator with the same behavior,
//                      and which iterates over the same text, as the one passed in.
//
//-------------------------------------------------------------------------------
नियमः_Based_Break_Iterator::नियमः_Based_Break_Iterator(ध्रुव नियमः_Based_Break_Iterator& other)
: नियमः_Based_Break_Iterator()
{
    *एतत् = other;
}


/**
 * Destructor
 */
नियमः_Based_Break_Iterator::~नियमः_Based_Break_Iterator() {
    यदि (fCharIter != &fSCharIter) {
        // fCharIter was adopted from the outside.
        विलोपय fCharIter;
    }
    fCharIter = शून्यसूचकः;

    utext_close(&fText);

    यदि (f_दत्त != शून्यसूचकः) {
        f_दत्त->removeReference();
        f_दत्त = शून्यसूचकः;
    }
    विलोपय fBreakCache;
    fBreakCache = शून्यसूचकः;

    विलोपय fDictionaryCache;
    fDictionaryCache = शून्यसूचकः;

    विलोपय fLanguageBreakEngines;
    fLanguageBreakEngines = शून्यसूचकः;

    विलोपय fUnhandledBreakEngine;
    fUnhandledBreakEngine = शून्यसूचकः;

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = शून्यसूचकः;
}

/**
 * Assignment operator.  Sets this iterator to have the same behavior,
 * and iterate over the same text, as the one passed in.
 * TODO: needs better handling of memory allocation errors.
 */
नियमः_Based_Break_Iterator&
नियमः_Based_Break_Iterator::संक्रियकः=(ध्रुव नियमः_Based_Break_Iterator& that) U_LIFETIME_BOUND {
    यदि (एतत् == &that) {
        प्रत्यागम *एतत्;
    }
    BreakIterator::संक्रियकः=(that);

    यदि (fLanguageBreakEngines != शून्यसूचकः) {
        विलोपय fLanguageBreakEngines;
        fLanguageBreakEngines = शून्यसूचकः;   // Just rebuild for now
    }
    // TODO: clone fLanguageBreakEngines from "that"
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    utext_clone(&fText, &that.fText, असत्य, सत्य, &स्थितिः);

    यदि (fCharIter != &fSCharIter) {
        विलोपय fCharIter;
    }
    fCharIter = &fSCharIter;

    यदि (that.fCharIter != शून्यसूचकः && that.fCharIter != &that.fSCharIter) {
        // This is a little bit tricky - it will initially appear that
        //  this->fCharIter is adopted, even if that->fCharIter was
        //  not adopted.  That's ok.
        fCharIter = that.fCharIter->clone();
    }
    fSCharIter = that.fSCharIter;
    यदि (fCharIter == शून्यसूचकः) {
        fCharIter = &fSCharIter;
    }

    यदि (f_दत्त != शून्यसूचकः) {
        f_दत्त->removeReference();
        f_दत्त = शून्यसूचकः;
    }
    यदि (that.f_दत्त != शून्यसूचकः) {
        f_दत्त = that.f_दत्त->सङ्कलय_Reference();
    }

    uprv_free(fLookAheadMatches);
    fLookAheadMatches = शून्यसूचकः;
    यदि (f_दत्त && f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार > 0) {
        fLookAheadMatches = static_cast<int32_t *>(
            uprv_malloc(f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार * परिमाण(int32_t)));
    }


    fPosition = that.fPosition;
    f_नियमः_स्थितिः_सूचकांक = that.f_नियमः_स्थितिः_सूचकांक;
    f_समाप्तम् = that.f_समाप्तम्;
    fIsPhraseBreaking = that.fIsPhraseBreaking;

    // TODO: both the dictionary and the main cache need to be copied.
    //       Current position could be within a dictionary range. Trying to continue
    //       the iteration without the caches present would go to the rules, with
    //       the assumption that the current position is on a rule boundary.
    fBreakCache->reset(fPosition, f_नियमः_स्थितिः_सूचकांक);
    fDictionaryCache->reset();

    प्रत्यागम *एतत्;
}

//-----------------------------------------------------------------------------
//
//    clone - Returns a newly-constructed RuleBasedBreakIterator with the same
//            behavior, and iterating over the same text, as this one.
//            Virtual function: does the right thing with subclasses.
//
//-----------------------------------------------------------------------------
नियमः_Based_Break_Iterator*
नियमः_Based_Break_Iterator::clone() ध्रुव {
    प्रत्यागम नूतन नियमः_Based_Break_Iterator(*एतत्);
}

/**
 * Equality operator.  Returns true if both BreakIterators are of the
 * same class, have the same behavior, and iterate over the same text.
 */
तार्किक
नियमः_Based_Break_Iterator::संक्रियकः==(ध्रुव BreakIterator& that) ध्रुव {
    यदि (typeid(*एतत्) != typeid(that)) {
        प्रत्यागम असत्य;
    }
    यदि (एतत् == &that) {
        प्रत्यागम सत्य;
    }

    // The base class BreakIterator carries no state that participates in equality,
    // and does not implement an equality function that would otherwise be
    // checked at this point.

    ध्रुव नियमः_Based_Break_Iterator& that2 = static_cast<ध्रुव नियमः_Based_Break_Iterator&>(that);

    यदि (!utext_equals(&fText, &that2.fText)) {
        // The two break iterators are operating on different text,
        //   or have a different iteration position.
        //   Note that fText's position is always the same as the break iterator's position.
        प्रत्यागम असत्य;
    }

    यदि (!(fPosition == that2.fPosition &&
            f_नियमः_स्थितिः_सूचकांक == that2.f_नियमः_स्थितिः_सूचकांक &&
            f_समाप्तम् == that2.f_समाप्तम् &&
            fIsPhraseBreaking == that2.fIsPhraseBreaking)) {
        प्रत्यागम असत्य;
    }

    यदि (that2.f_दत्त == f_दत्त ||
        (f_दत्त != शून्यसूचकः && that2.f_दत्त != शून्यसूचकः && *that2.f_दत्त == *f_दत्त)) {
            // The two break iterators are using the same rules.
            प्रत्यागम सत्य;
        }
    प्रत्यागम असत्य;
}

/**
 * Compute a hash code for this BreakIterator
 * @return A hash code
 */
int32_t
नियमः_Based_Break_Iterator::hash_संहिता() ध्रुव {
    int32_t   hash = 0;
    यदि (f_दत्त != शून्यसूचकः) {
        hash = f_दत्त->hash_संहिता();
    }
    प्रत्यागम hash;
}


शून्य नियमः_Based_Break_Iterator::समुच्चयः_Text(UText *ut, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_clone(&fText, ut, असत्य, सत्य, &स्थितिः);

    // Set up a dummy CharacterIterator to be returned if anyone
    //   calls getText().  With input from UText, there is no reasonable
    //   way to return a characterIterator over the actual input text.
    //   Return one over an empty string instead - this is the closest
    //   we can come to signaling a failure.
    //   (GetText() is obsolete, this failure is sort of OK)
    fSCharIter.समुच्चयः_Text(u"", 0);

    यदि (fCharIter != &fSCharIter) {
        // existing fCharIter was adopted from the outside.  Delete it now.
        विलोपय fCharIter;
    }
    fCharIter = &fSCharIter;

    एतत्->first();
}


UText* नियमः_Based_Break_Iterator::getUText(UText* fill_अन्तर् U_LIFETIME_BOUND, UError_संहिता& स्थितिः) ध्रुव {
    UText *परिणाम = utext_clone(fill_अन्तर्, &fText, असत्य, सत्य, &स्थितिः);
    प्रत्यागम परिणाम;
}


//=======================================================================
// BreakIterator overrides
//=======================================================================

/**
 * Return a CharacterIterator over the text being analyzed.
 */
CharacterIterator&
नियमः_Based_Break_Iterator::getText() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम *fCharIter;
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
शून्य
नियमः_Based_Break_Iterator::adoptText(CharacterIterator* newText) {
    // If we are holding a CharacterIterator adopted from a
    //   previous call to this function, delete it now.
    यदि (fCharIter != &fSCharIter) {
        विलोपय fCharIter;
    }

    fCharIter = newText;
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    यदि (newText==शून्यसूचकः || newText->start_सूचकांक() != 0) {
        // startIndex !=0 wants to be an error, but there's no way to report it.
        // Make the iterator text be an empty string.
        utext_openUChars(&fText, शून्यसूचकः, 0, &स्थितिः);
    } अन्यथा {
        utext_openCharacterIterator(&fText, newText, &स्थितिः);
    }
    एतत्->first();
}

/**
 * Set the iterator to analyze a new piece of text.  This function resets
 * the current iteration position to the beginning of the text.
 * @param newText An iterator over the text to analyze.
 */
शून्य
नियमः_Based_Break_Iterator::समुच्चयः_Text(ध्रुव UnicodeString& newText) {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    fBreakCache->reset();
    fDictionaryCache->reset();
    utext_openConstUnicodeString(&fText, &newText, &स्थितिः);

    // Set up a character iterator on the string.
    //   Needed in case someone calls getText().
    //  Can not, unfortunately, do this lazily on the (probably never)
    //  call to getText(), because getText is const.
    fSCharIter.समुच्चयः_Text(newText.get_बफर(), newText.length());

    यदि (fCharIter != &fSCharIter) {
        // old fCharIter was adopted from the outside.  Delete it.
        विलोपय fCharIter;
    }
    fCharIter = &fSCharIter;

    एतत्->first();
}


/**
 *  Provide a new UText for the input text.  Must reference text with contents identical
 *  to the original.
 *  Intended for use with text data originating in Java (garbage collected) environments
 *  where the data may be moved in memory at arbitrary times.
 */
नियमः_Based_Break_Iterator&
नियमः_Based_Break_Iterator::refresh_निवेशः_Text(UText* input, UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम *एतत्;
    }
    यदि (input == शून्यसूचकः) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम *एतत्;
    }
    int64_t pos = utext_get_Native_सूचकांक(&fText);
    //  Shallow read-only clone of the new UText into the existing input UText
    utext_clone(&fText, input, असत्य, सत्य, &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम *एतत्;
    }
    utext_समुच्चयः_Native_सूचकांक(&fText, pos);
    यदि (utext_get_Native_सूचकांक(&fText) != pos) {
        // Sanity check.  The new input utext is supposed to have the exact same
        // contents as the old.  If we can't set to the same position, it doesn't.
        // The contents underlying the old utext might be invalid at this point,
        // so it's not safe to check directly.
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }
    प्रत्यागम *एतत्;
}


/**
 * Sets the current iteration position to the beginning of the text, position zero.
 * @return The new iterator position, which is zero.
 */
int32_t नियमः_Based_Break_Iterator::first() {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    यदि (!fBreakCache->seek(0)) {
        fBreakCache->populateNear(0, स्थितिः);
    }
    fBreakCache->current();
    U_ASSERT(fPosition == 0);
    प्रत्यागम 0;
}

/**
 * Sets the current iteration position to the end of the text.
 * @return The text's past-the-end offset.
 */
int32_t नियमः_Based_Break_Iterator::चरमः() {
    int32_t अन्तः_Pos = static_cast<int32_t>(utext_native_दीर्घता(&fText));
    UBool अन्तः_Should_Be_Boundary = isBoundary(अन्तः_Pos);      // Has side effect of setting iterator position.
    (शून्य)अन्तः_Should_Be_Boundary;
    U_ASSERT(अन्तः_Should_Be_Boundary);
    U_ASSERT(fPosition == अन्तः_Pos);
    प्रत्यागम अन्तः_Pos;
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
int32_t नियमः_Based_Break_Iterator::next(int32_t n) {
    int32_t परिणाम = 0;
    यदि (n > 0) {
        प्रति (; n > 0 && परिणाम != UBRK_समाप्तम्; --n) {
            परिणाम = next();
        }
    } अन्यथा यदि (n < 0) {
        प्रति (; n < 0 && परिणाम != UBRK_समाप्तम्; ++n) {
            परिणाम = previous();
        }
    } अन्यथा {
        परिणाम = current();
    }
    प्रत्यागम परिणाम;
}

/**
 * Advances the iterator to the next boundary position.
 * @return The position of the first boundary after this one.
 */
int32_t नियमः_Based_Break_Iterator::next() {
    fBreakCache->next();
    प्रत्यागम f_समाप्तम् ? UBRK_समाप्तम् : fPosition;
}

/**
 * Move the iterator backwards, to the boundary preceding the current one.
 *
 *         Starts from the current position within fText.
 *         Starting position need not be on a boundary.
 *
 * @return The position of the boundary position immediately preceding the starting position.
 */
int32_t नियमः_Based_Break_Iterator::previous() {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    fBreakCache->previous(स्थितिः);
    प्रत्यागम f_समाप्तम् ? UBRK_समाप्तम् : fPosition;
}

/**
 * Sets the iterator to refer to the first boundary position following
 * the specified position.
 * @param startPos The position from which to begin searching for a break position.
 * @return The position of the first break after the current position.
 */
int32_t नियमः_Based_Break_Iterator::following(int32_t startPos) {
    // if the supplied position is before the beginning, return the
    // text's starting offset
    यदि (startPos < 0) {
        प्रत्यागम first();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8. Or it may be beyond the end of the text.
    utext_समुच्चयः_Native_सूचकांक(&fText, startPos);
    startPos = static_cast<int32_t>(utext_get_Native_सूचकांक(&fText));

    UError_संहिता स्थितिः = U_ZERO_ERROR;
    fBreakCache->following(startPos, स्थितिः);
    प्रत्यागम f_समाप्तम् ? UBRK_समाप्तम् : fPosition;
}

/**
 * Sets the iterator to refer to the last boundary position before the
 * specified position.
 * @param offset The position to begin searching for a break from.
 * @return The position of the last boundary before the starting position.
 */
int32_t नियमः_Based_Break_Iterator::preceding(int32_t विस्थापनम्) {
    यदि (विस्थापनम् > utext_native_दीर्घता(&fText)) {
        प्रत्यागम चरमः();
    }

    // Move requested offset to a code point start. It might be on a trail surrogate,
    // or on a trail byte if the input is UTF-8.

    utext_समुच्चयः_Native_सूचकांक(&fText, विस्थापनम्);
    int32_t adjusted_विस्थापनम् = static_cast<int32_t>(utext_get_Native_सूचकांक(&fText));

    UError_संहिता स्थितिः = U_ZERO_ERROR;
    fBreakCache->preceding(adjusted_विस्थापनम्, स्थितिः);
    प्रत्यागम f_समाप्तम् ? UBRK_समाप्तम् : fPosition;
}

/**
 * Returns true if the specified position is a boundary position.  As a side
 * effect, leaves the iterator pointing to the first boundary position at
 * or after "offset".
 *
 * @param offset the offset to check.
 * @return True if "offset" is a boundary position.
 */
UBool नियमः_Based_Break_Iterator::isBoundary(int32_t विस्थापनम्) {
    // out-of-range indexes are never boundary positions
    यदि (विस्थापनम् < 0) {
        first();       // For side effects on current position, tag values.
        प्रत्यागम असत्य;
    }

    // Adjust offset to be on a code point boundary and not beyond the end of the text.
    // Note that isBoundary() is always false for offsets that are not on code point boundaries.
    // But we still need the side effect of leaving iteration at the following boundary.

    utext_समुच्चयः_Native_सूचकांक(&fText, विस्थापनम्);
    int32_t adjusted_विस्थापनम् = static_cast<int32_t>(utext_get_Native_सूचकांक(&fText));

    तार्किक परिणाम = असत्य;
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    यदि (fBreakCache->seek(adjusted_विस्थापनम्) || fBreakCache->populateNear(adjusted_विस्थापनम्, स्थितिः)) {
        परिणाम = (fBreakCache->current() == विस्थापनम्);
    }

    यदि (परिणाम && adjusted_विस्थापनम् < विस्थापनम् && utext_char32At(&fText, विस्थापनम्) == U_SENTINEL) {
        // Original offset is beyond the end of the text. Return false, it's not a boundary,
        // but the iteration position remains set to the end of the text, which is a boundary.
        प्रत्यागम असत्य;
    }
    यदि (!परिणाम) {
        // Not on a boundary. isBoundary() must leave iterator on the following boundary.
        // Cache->seek(), above, left us on the preceding boundary, so advance one.
        next();
    }
    प्रत्यागम परिणाम;
}


/**
 * Returns the current iteration position.
 * @return The current iteration position.
 */
int32_t नियमः_Based_Break_Iterator::current() ध्रुव {
    प्रत्यागम fPosition;
}


//=======================================================================
// implementation
//=======================================================================

//
// RBBIRunMode  -  the state machine runs an extra iteration at the beginning and end
//                 of user text.  A variable with this enum type keeps track of where we
//                 are.  The state machine only fetches user input while in the RUN mode.
//
गणना RBBIRun_बहुलकम् {
    RBBI_START,     // state machine processing is before first char of input
    RBBI_सततम्,       // state machine processing is in the user text
    RBBI_अन्तः        // state machine processing is after end of user text.
};


// Wrapper functions to select the appropriate handleNext() or handleSafePrevious()
// instantiation, based on whether an 8 or 16 bit table is required.
//
// These Trie access functions will be inlined within the handleNext()/Previous() instantions.
स्थिर अन्तःस्थ uint16_t TrieFunc8(ध्रुव UCPTrie *trie, UChar32 c) {
    प्रत्यागम UCPTRIE_FAST_GET(trie, UCPTRIE_8, c);
}

स्थिर अन्तःस्थ uint16_t TrieFunc16(ध्रुव UCPTrie *trie, UChar32 c) {
    प्रत्यागम UCPTRIE_FAST_GET(trie, UCPTRIE_16, c);
}

int32_t नियमः_Based_Break_Iterator::handle_अग्र() {
    ध्रुव RBBIState_सारणी *statetable = f_दत्त->f_Forward_सारणी;
    तार्किक use8_द्वयङ्काः_Trie = ucptrie_get_मानम्_विस्तारः(f_दत्त->fTrie) == UCPTRIE_मानम्_द्वयङ्काः_8;
    यदि (statetable->fFlags & RBBI_8_द्वयङ्काः_पङ्क्तिकाः) {
        यदि (use8_द्वयङ्काः_Trie) {
            प्रत्यागम handle_अग्र<RBBIState_सारणी_Row8, TrieFunc8>();
        } अन्यथा {
            प्रत्यागम handle_अग्र<RBBIState_सारणी_Row8, TrieFunc16>();
        }
    } अन्यथा {
        यदि (use8_द्वयङ्काः_Trie) {
            प्रत्यागम handle_अग्र<RBBIState_सारणी_Row16, TrieFunc8>();
        } अन्यथा {
            प्रत्यागम handle_अग्र<RBBIState_सारणी_Row16, TrieFunc16>();
        }
    }
}

int32_t नियमः_Based_Break_Iterator::handleSafePrevious(int32_t fromPosition) {
    ध्रुव RBBIState_सारणी *statetable = f_दत्त->f_Reverse_सारणी;
    तार्किक use8_द्वयङ्काः_Trie = ucptrie_get_मानम्_विस्तारः(f_दत्त->fTrie) == UCPTRIE_मानम्_द्वयङ्काः_8;
    यदि (statetable->fFlags & RBBI_8_द्वयङ्काः_पङ्क्तिकाः) {
        यदि (use8_द्वयङ्काः_Trie) {
            प्रत्यागम handleSafePrevious<RBBIState_सारणी_Row8, TrieFunc8>(fromPosition);
        } अन्यथा {
            प्रत्यागम handleSafePrevious<RBBIState_सारणी_Row8, TrieFunc16>(fromPosition);
        }
    } अन्यथा {
        यदि (use8_द्वयङ्काः_Trie) {
            प्रत्यागम handleSafePrevious<RBBIState_सारणी_Row16, TrieFunc8>(fromPosition);
        } अन्यथा {
            प्रत्यागम handleSafePrevious<RBBIState_सारणी_Row16, TrieFunc16>(fromPosition);
        }
    }
}


//-----------------------------------------------------------------------------------
//
//  handleNext()
//     Run the state machine to find a boundary
//
//-----------------------------------------------------------------------------------
प्रतिरूपम् <प्रकारनाम RowType, नियमः_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t नियमः_Based_Break_Iterator::handle_अग्र() {
    int32_t             state;
    uint16_t            category        = 0;
    RBBIRun_बहुलकम्         बहुलकम्;

    RowType             *row;
    UChar32             c;
    int32_t             परिणाम             = 0;
    int32_t             initialPosition    = 0;
    ध्रुव RBBIState_सारणी *statetable       = f_दत्त->f_Forward_सारणी;
    ध्रुव वर्ण         *सारणी_दत्त          = statetable->f_सारणी_दत्त;
    uint32_t            सारणी_Row_Len        = statetable->fRowLen;
    uint32_t            dictStart          = statetable->fDictCategoriesStart;
    #यदि_परिभाषितम् RBBI_DEBUG
        यदि (gTrace) {
            RBBIDebugPuts("Handle Next   pos   char  state category");
        }
    #यदि_अन्तः

    // handleNext always sets the break tag value.
    // Set the default for it.
    f_नियमः_स्थितिः_सूचकांक = 0;

    f_Dictionary_Char_गणनम् = 0;

    // if we're already at the end of the text, return DONE.
    initialPosition = fPosition;
    UTEXT_SETNATIVEINDEX(&fText, initialPosition);
    परिणाम          = initialPosition;
    c               = UTEXT_NEXT32(&fText);
    यदि (c==U_SENTINEL) {
        f_समाप्तम् = सत्य;
        प्रत्यागम UBRK_समाप्तम्;
    }

    //  Set the initial state for the state machine
    state = START_STATE;
    row = (RowType *)
            //(statetable->fTableData + (statetable->fRowLen * state));
            (सारणी_दत्त + सारणी_Row_Len * state);


    बहुलकम्     = RBBI_सततम्;


    // loop until we reach the end of the text or transition to state 0
    //
    प्रति (;;) {
        यदि (c == U_SENTINEL) {
            // Reached end of input string.
            यदि (बहुलकम् == RBBI_अन्तः) {
                // We have already run the loop one last time with the
                //   character set to the psueudo {eof} value.  Now it is time
                //   to unconditionally bail out.
                भङ्ग;
            }
            // Run the loop one last time with the fake end-of-input character category.
            बहुलकम् = RBBI_अन्तः;
            category = 1;
        }

        //
        // Get the char category.  An incoming category of 1 or 2 means that
        //      we are preset for doing the beginning or end of input, and
        //      that we shouldn't get a category from an actual text input character.
        //
        यदि (बहुलकम् == RBBI_सततम्) {
            // look up the current character's character category, which tells us
            // which column in the state table to look at.
            category = trieFunc(f_दत्त->fTrie, c);
            f_Dictionary_Char_गणनम् += (category >= dictStart);
        }

       #यदि_परिभाषितम् RBBI_DEBUG
            यदि (gTrace) {
                RBBIDebugPrintf("             %4" PRId64 "   ", utext_get_Native_सूचकांक(&fText));
                यदि (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } अन्यथा {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #यदि_अन्तः

        // State Transition - move machine to its next state
        //

        // fNextState is a variable-length array.
        U_ASSERT(category<f_दत्त->f_शीर्षकम्->f_Cat_गणनम्);
        state = row->f_अग्र_State[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            // (statetable->fTableData + (statetable->fRowLen * state));
            (सारणी_दत्त + सारणी_Row_Len * state);


        uint16_t accepting = row->fAccepting;
        यदि (accepting == ACCEPTING_UNCONDITIONAL) {
            // Match found, common case.
            यदि (बहुलकम् != RBBI_START) {
                परिणाम = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            }
            f_नियमः_स्थितिः_सूचकांक = row->f_चिह्नकानि_Idx;   // Remember the break status (tag) values.
        } अन्यथा यदि (accepting > ACCEPTING_UNCONDITIONAL) {
            // Lookahead match is completed.
            U_ASSERT(accepting < f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार);
            int32_t lookahead_परिणाम = fLookAheadMatches[accepting];
            यदि (lookahead_परिणाम >= 0) {
                f_नियमः_स्थितिः_सूचकांक = row->f_चिह्नकानि_Idx;
                fPosition = lookahead_परिणाम;
                प्रत्यागम lookahead_परिणाम;
            }
        }

        // If we are at the position of the '/' in a look-ahead (hard break) rule;
        // record the current position, to be returned later, if the full rule matches.
        // TODO: Move this check before the previous check of fAccepting.
        //       This would enable hard-break rules with no following context.
        //       But there are line break test failures when trying this. Investigate.
        //       Issue ICU-20837
        uint16_t नियमः = row->fLookAhead;
        U_ASSERT(नियमः == 0 || नियमः > ACCEPTING_UNCONDITIONAL);
        U_ASSERT(नियमः == 0 || नियमः < f_दत्त->f_Forward_सारणी->f_Look_Ahead_Results_आकार);
        यदि (नियमः > ACCEPTING_UNCONDITIONAL) {
            int32_t pos = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
            fLookAheadMatches[नियमः] = pos;
        }

        यदि (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // We have advanced through the string until it is certain that no
            //   longer match is possible, no matter what characters follow.
            भङ्ग;
        }

        // Advance to the next character.
        // If this is a beginning-of-input loop iteration, don't advance
        //    the input position.  The next iteration will be processing the
        //    first real input character.
        यदि (बहुलकम् == RBBI_सततम्) {
            c = UTEXT_NEXT32(&fText);
        } अन्यथा {
            यदि (बहुलकम् == RBBI_START) {
                बहुलकम् = RBBI_सततम्;
            }
        }
    }

    // The state machine is done.  Check whether it found a match...

    // If the iterator failed to advance in the match engine, force it ahead by one.
    //   (This really indicates a defect in the break rules.  They should always match
    //    at least one character.)
    यदि (परिणाम == initialPosition) {
        utext_समुच्चयः_Native_सूचकांक(&fText, initialPosition);
        utext_next32(&fText);
        परिणाम = static_cast<int32_t>(utext_get_Native_सूचकांक(&fText));
        f_नियमः_स्थितिः_सूचकांक = 0;
    }

    // Leave the iterator at our result position.
    fPosition = परिणाम;
    #यदि_परिभाषितम् RBBI_DEBUG
        यदि (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", परिणाम);
        }
    #यदि_अन्तः
    प्रत्यागम परिणाम;
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
प्रतिरूपम् <प्रकारनाम RowType, नियमः_Based_Break_Iterator::PTrieFunc trieFunc>
int32_t नियमः_Based_Break_Iterator::handleSafePrevious(int32_t fromPosition) {

    int32_t             state;
    uint16_t            category        = 0;
    RowType            *row;
    UChar32             c;
    int32_t             परिणाम          = 0;

    ध्रुव RBBIState_सारणी *state_सारणी = f_दत्त->f_Reverse_सारणी;
    UTEXT_SETNATIVEINDEX(&fText, fromPosition);
    #यदि_परिभाषितम् RBBI_DEBUG
        यदि (gTrace) {
            RBBIDebugPuts("Handle Previous   pos   char  state category");
        }
    #यदि_अन्तः

    // if we're already at the start of the text, return DONE.
    यदि (f_दत्त == शून्यसूचकः || UTEXT_GETNATIVEINDEX(&fText)==0) {
        प्रत्यागम BreakIterator::समाप्तम्_2;
    }

    //  Set the initial state for the state machine
    c = UTEXT_PREVIOUS32(&fText);
    state = START_STATE;
    row = (RowType *)
            (state_सारणी->f_सारणी_दत्त + (state_सारणी->fRowLen * state));

    // loop until we reach the start of the text or transition to state 0
    //
    प्रति (; c != U_SENTINEL; c = UTEXT_PREVIOUS32(&fText)) {

        // look up the current character's character category, which tells us
        // which column in the state table to look at.
        //
        //  Off the dictionary flag bit. For reverse iteration it is not used.
        category = trieFunc(f_दत्त->fTrie, c);

        #यदि_परिभाषितम् RBBI_DEBUG
            यदि (gTrace) {
                RBBIDebugPrintf("             %4d   ", (int32_t)utext_get_Native_सूचकांक(&fText));
                यदि (0x20<=c && c<0x7f) {
                    RBBIDebugPrintf("\"%c\"  ", c);
                } अन्यथा {
                    RBBIDebugPrintf("%5x  ", c);
                }
                RBBIDebugPrintf("%3d  %3d\n", state, category);
            }
        #यदि_अन्तः

        // State Transition - move machine to its next state
        //
        // fNextState is a variable-length array.
        U_ASSERT(category<f_दत्त->f_शीर्षकम्->f_Cat_गणनम्);
        state = row->f_अग्र_State[category];  /*Not accessing beyond memory*/
        row = (RowType *)
            (state_सारणी->f_सारणी_दत्त + (state_सारणी->fRowLen * state));

        यदि (state == STOP_STATE) {
            // This is the normal exit from the lookup state machine.
            // Transition to state zero means we have found a safe point.
            भङ्ग;
        }
    }

    // The state machine is done.  Check whether it found a match...
    परिणाम = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(&fText));
    #यदि_परिभाषितम् RBBI_DEBUG
        यदि (gTrace) {
            RBBIDebugPrintf("result = %d\n\n", परिणाम);
        }
    #यदि_अन्तः
    प्रत्यागम परिणाम;
}


//-------------------------------------------------------------------------------
//
//   getRuleStatus()   Return the break rule tag associated with the current
//                     iterator position.  If the iterator arrived at its current
//                     position by iterating forwards, the value will have been
//                     cached by the handleNext() function.
//
//-------------------------------------------------------------------------------

int32_t  नियमः_Based_Break_Iterator::get_नियमः_स्थितिः() ध्रुव {

    // fLastRuleStatusIndex indexes to the start of the appropriate status record
    //                                                 (the number of status values.)
    //   This function returns the last (largest) of the array of status values.
    int32_t  idx = f_नियमः_स्थितिः_सूचकांक + f_दत्त->f_नियमः_स्थितिः_सारणी[f_नियमः_स्थितिः_सूचकांक];
    int32_t  चिह्नकम्_Val = f_दत्त->f_नियमः_स्थितिः_सारणी[idx];

    प्रत्यागम चिह्नकम्_Val;
}


int32_t नियमः_Based_Break_Iterator::get_नियमः_स्थितिः_Vec(
             int32_t *fill_अन्तर्_Vec, int32_t capacity, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }

    int32_t  अङ्कः_Vals = f_दत्त->f_नियमः_स्थितिः_सारणी[f_नियमः_स्थितिः_सूचकांक];
    int32_t  अङ्कः_Vals_गन्तव्यम्_Copy = अङ्कः_Vals;
    यदि (अङ्कः_Vals > capacity) {
        स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
        अङ्कः_Vals_गन्तव्यम्_Copy = capacity;
    }
    पूर्णाङ्क i;
    प्रति (i=0; i<अङ्कः_Vals_गन्तव्यम्_Copy; i++) {
        fill_अन्तर्_Vec[i] = f_दत्त->f_नियमः_स्थितिः_सारणी[f_नियमः_स्थितिः_सूचकांक + i + 1];
    }
    प्रत्यागम अङ्कः_Vals;
}



//-------------------------------------------------------------------------------
//
//   getBinaryRules        Access to the compiled form of the rules,
//                         for use by build system tools that save the data
//                         for standard iterator types.
//
//-------------------------------------------------------------------------------
ध्रुव uint8_t* नियमः_Based_Break_Iterator::get_द्विमान_नियमाः(uint32_t& length) U_LIFETIME_BOUND {
    ध्रुव uint8_t  *retPtr = शून्यसूचकः;
    length = 0;

    यदि (f_दत्त != शून्यसूचकः) {
        retPtr = reinterpret_cast<ध्रुव uint8_t*>(f_दत्त->f_शीर्षकम्);
        length = f_दत्त->f_शीर्षकम्->f_दीर्घता;
    }
    प्रत्यागम retPtr;
}


नियमः_Based_Break_Iterator *नियमः_Based_Break_Iterator::create_बफर_Clone(
        शून्य * /*stackBuffer*/, int32_t &बफर_आकार, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }

    यदि (बफर_आकार == 0) {
        बफर_आकार = 1;  // preflighting for deprecated functionality
        प्रत्यागम शून्यसूचकः;
    }

    BreakIterator *clonedBI = clone();
    यदि (clonedBI == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा {
        स्थितिः = U_SAFECLONE_ALLOCATED_WARNING;
    }
    प्रत्यागम (नियमः_Based_Break_Iterator *)clonedBI;
}

U_NAMESPACE_अन्तः


स्थिर icu::UStack *gLanguageBreakFactories = शून्यसूचकः;
स्थिर ध्रुव icu::UnicodeString *g_रिक्तानि_String = शून्यसूचकः;
स्थिर icu::UInitOnce gLanguageBreakFactoriesInitOnce {};
स्थिर icu::UInitOnce gRBBIInitOnce {};
स्थिर icu::ICULanguageBreakFactory *gICULanguageBreakFactory = शून्यसूचकः;

/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
UBool U_CALLCONV rbbi_cleanup() {
    विलोपय gLanguageBreakFactories;
    gLanguageBreakFactories = शून्यसूचकः;
    विलोपय g_रिक्तानि_String;
    g_रिक्तानि_String = शून्यसूचकः;
    gLanguageBreakFactoriesInitOnce.reset();
    gRBBIInitOnce.reset();
    प्रत्यागम सत्य;
}
U_CDECL_अन्तः

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV _deleteFactory(शून्य *obj) {
    विलोपय (icu::LanguageBreakFactory *) obj;
}
U_CDECL_अन्तः
U_NAMESPACE_BEGIN

स्थिर शून्य U_CALLCONV rbbiInit() {
    g_रिक्तानि_String = नूतन UnicodeString();
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

स्थिर शून्य U_CALLCONV initLanguageFactories(UError_संहिता& स्थितिः) {
    U_ASSERT(gLanguageBreakFactories == शून्यसूचकः);
    gLanguageBreakFactories = नूतन UStack(_deleteFactory, शून्यसूचकः, स्थितिः);
    यदि (gLanguageBreakFactories != शून्यसूचकः && U_SUCCESS(स्थितिः)) {
        LocalPointer<ICULanguageBreakFactory> factory(नूतन ICULanguageBreakFactory(स्थितिः), स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
            gICULanguageBreakFactory = factory.orphan();
            gLanguageBreakFactories->push(gICULanguageBreakFactory, स्थितिः);
#यदि_परिभाषितम् U_LOCAL_SERVICE_HOOK
            LanguageBreakFactory *extra = (LanguageBreakFactory *)uprv_svc_hook("languageBreakFactory", &स्थितिः);
            यदि (extra != शून्यसूचकः) {
                gLanguageBreakFactories->push(extra, स्थितिः);
            }
#यदि_अन्तः
        }
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_RBBI, rbbi_cleanup);
}

शून्य ensureLanguageFactories(UError_संहिता& स्थितिः) {
    umtx_initOnce(gLanguageBreakFactoriesInitOnce, &initLanguageFactories, स्थितिः);
}

स्थिर ध्रुव LanguageBreakEngine*
getLanguageBreakEngineFromFactory(UChar32 c, ध्रुव वर्ण* locale)
{
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    ensureLanguageFactories(स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम शून्यसूचकः;

    int32_t i = gLanguageBreakFactories->size();
    ध्रुव LanguageBreakEngine *lbe = शून्यसूचकः;
    यावत् (--i >= 0) {
        LanguageBreakFactory* factory = static_cast<LanguageBreakFactory*>(gLanguageBreakFactories->elementAt(i));
        lbe = factory->getEngineFor(c, locale);
        यदि (lbe != शून्यसूचकः) {
            भङ्ग;
        }
    }
    प्रत्यागम lbe;
}


//-------------------------------------------------------------------------------
//
//  getLanguageBreakEngine  Find an appropriate LanguageBreakEngine for the
//                          the character c.
//
//-------------------------------------------------------------------------------
ध्रुव LanguageBreakEngine*
नियमः_Based_Break_Iterator::getLanguageBreakEngine(UChar32 c, ध्रुव वर्ण* locale) U_LIFETIME_BOUND {
    ध्रुव LanguageBreakEngine *lbe = शून्यसूचकः;
    UError_संहिता स्थितिः = U_ZERO_ERROR;

    यदि (fLanguageBreakEngines == शून्यसूचकः) {
        fLanguageBreakEngines = नूतन UStack(स्थितिः);
        यदि (fLanguageBreakEngines == शून्यसूचकः || U_FAILURE(स्थितिः)) {
            विलोपय fLanguageBreakEngines;
            fLanguageBreakEngines = शून्यसूचकः;
            प्रत्यागम शून्यसूचकः;
        }
    }

    int32_t i = fLanguageBreakEngines->size();
    यावत् (--i >= 0) {
        lbe = static_cast<ध्रुव LanguageBreakEngine*>(fLanguageBreakEngines->elementAt(i));
        यदि (lbe->handles(c, locale)) {
            प्रत्यागम lbe;
        }
    }

    // No existing dictionary took the character. See if a factory wants to
    // give us a new LanguageBreakEngine for this character.
    lbe = getLanguageBreakEngineFromFactory(c, locale);

    // If we got one, use it and push it on our stack.
    यदि (lbe != शून्यसूचकः) {
        fLanguageBreakEngines->push((शून्य *)lbe, स्थितिः);
        // Even if we can't remember it, we can keep looking it up, so
        // return it even if the push fails.
        प्रत्यागम lbe;
    }

    // No engine is forthcoming for this character. Add it to the
    // reject set. Create the reject break engine if needed.
    यदि (fUnhandledBreakEngine == शून्यसूचकः) {
        fUnhandledBreakEngine = नूतन UnhandledEngine(स्थितिः);
        यदि (U_SUCCESS(स्थितिः) && fUnhandledBreakEngine == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        // Put it last so that scripts for which we have an engine get tried
        // first.
        fLanguageBreakEngines->निवेशय_Element_At(fUnhandledBreakEngine, 0, स्थितिः);
        // If we can't insert it, or creation failed, get rid of it
        U_ASSERT(!fLanguageBreakEngines->hasDeleter());
        यदि (U_FAILURE(स्थितिः)) {
            विलोपय fUnhandledBreakEngine;
            fUnhandledBreakEngine = शून्यसूचकः;
            प्रत्यागम शून्यसूचकः;
        }
    }

    // Tell the reject engine about the character; at its discretion, it may
    // add more than just the one character.
    fUnhandledBreakEngine->handleCharacter(c);

    प्रत्यागम fUnhandledBreakEngine;
}

#यदि_अपरिभाषितम् U_HIDE_DRAFT_API
शून्य U_EXPORT2 नियमः_Based_Break_Iterator::पञ्जिकाकोशः_External_Break_Engine(
                  ExternalBreakEngine* गन्तव्यम्_Adopt, UError_संहिता& स्थितिः) {
    LocalPointer<ExternalBreakEngine> engine(गन्तव्यम्_Adopt, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    ensureLanguageFactories(स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    gICULanguageBreakFactory->सङ्कलय_External_Engine(engine.orphan(), स्थितिः);
}
#यदि_अन्तः  /* U_HIDE_DRAFT_API */


शून्य नियमः_Based_Break_Iterator::dumpCache() {
    fBreakCache->dumpCache();
}

शून्य नियमः_Based_Break_Iterator::dumpTables() {
    f_दत्त->print_दत्त();
}

/**
 * Returns the description used to create this iterator
 */

ध्रुव UnicodeString&
नियमः_Based_Break_Iterator::get_नियमाः() ध्रुव U_LIFETIME_BOUND {
    यदि (f_दत्त != शून्यसूचकः) {
        प्रत्यागम f_दत्त->get_नियमः_Source_String();
    } अन्यथा {
        umtx_initOnce(gRBBIInitOnce, &rbbiInit);
        प्रत्यागम *g_रिक्तानि_String;
    }
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */

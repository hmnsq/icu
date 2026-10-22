// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "cmemory.h"

#ಅನ್ತರ್ಭಾವ "unicode/filteredbrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstriebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"

#ಅನ್ತರ್ಭಾವ "uresimp.h" // ures_getByKeyWithFallback
#ಅನ್ತರ್ಭಾವ "ubrkimpl.h" // U_ICUDATA_BRKITR
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"

U_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FB_DEBUG
#ವ್ಯಾಖ್ಯಾನಿಸು FB_DEBUG 0
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ FB_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಸ್ಥಿರ ಶೂನ್ಯ _fb_trace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *m, ಸ್ಥಿರಾಂಕ UnicodeString *s, UBool b, int32_t d, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಪೂರ್ಣಾಂಕ l) {
  ಅಕ್ಷರ buf[2048];
  ಆದರೆ(s) {
    s->extract(0,s->length(),buf,2048);
  } ಇಲ್ಲದಿದ್ದರೆ {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*)s, b?'T':'F',(ಪೂರ್ಣಾಂಕ)d);
}

#ವ್ಯಾಖ್ಯಾನಿಸು FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು FB_TRACE(m,s,b,d)
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Used with sortedInsert()
 */
ಸ್ಥಿರ int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ಸ್ಥಿರಾಂಕ UnicodeString& a = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(t1.ಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ UnicodeString& b = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(t2.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
ವರ್ಗ UString_ಗಣ : ಸಾರ್ವಜನಿಕ UVector {
 ಸಾರ್ವಜನಿಕ:
  UString_ಗಣ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           ಸ್ಥಿತಿ_2) {}
  ವಾಸ್ತವಿಕ ~UString_ಗಣ();
  /**
   * Is this UnicodeSet contained?
   */
  ಸಾಲೊಳಗೆ UBool ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ UnicodeString& s) {
    ಹಿಂತಿರುಗಿ ಒಳಗೊಂಡಿದೆ((ಶೂನ್ಯ*) &s);
  }
  ಬಳಸಿ UVector::ಒಳಗೊಂಡಿದೆ;
  /**
   * Return the ith UnicodeString alias
   */
  ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ UnicodeString* getStringAt(int32_t i) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  ಸಾಲೊಳಗೆ UBool adopt(UnicodeString *str, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2) || ಒಳಗೊಂಡಿದೆ(*str)) {
      ಅಳಿಸು str;
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
      sorted_ಸೇರಿಸು(str, compareUnicodeString, ಸ್ಥಿತಿ_2);
      ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }
      ಹಿಂತಿರುಗಿ ನಿಜ;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  ಸಾಲೊಳಗೆ UBool ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ UnicodeString& str, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    UnicodeString *t = ಹೊಸ UnicodeString(str);
    ಆದರೆ(t==ಶೂನ್ಯಸೂಚಕ) {
      ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR; ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ adopt(t, ಸ್ಥಿತಿ_2);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  ಸಾಲೊಳಗೆ UBool remove(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    ಹಿಂತಿರುಗಿ removeElement((ಶೂನ್ಯ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_ಗಣ::~UString_ಗಣ() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_ಹೊಂದಿಕೆ   = (1<<1); //< exact match - skip this one.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Suppress_ಒಳ_ಹಿಮ್ಮುಖ = (1<<0);
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_ಕೂಡಿಸು_ಗಮ್ಯ_Forward = (1<<1);
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
ವರ್ಗ Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
  Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        ಹಿಂತಿರುಗಿ ಇದು;
    }
    Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ *decr() {
        ಆದರೆ(umtx_atomic_dec(&refcount) <= 0) {
            ಅಳಿಸು ಇದು;
        }
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ವಾಸ್ತವಿಕ ~Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ();

    ತಾರ್ಕಿಕ hasForwardsPartialTrie() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ fForwardsPartialTrie.isValid(); }
    ತಾರ್ಕಿಕ hasBackwardsTrie() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ fBackwardsTrie.isValid(); }

    ಸ್ಥಿರಾಂಕ UCharsTrie& getForwardsPartialTrie() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
        ಹಿಂತಿರುಗಿ *fForwardsPartialTrie;
    }
    ಸ್ಥಿರಾಂಕ UCharsTrie& getBackwardsTrie() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ *fBackwardsTrie; }

ಖಾಸಗಿ:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    Local_ಸೂಚಕ<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    Local_ಸೂಚಕ<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ::~Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ() {}

/**
 * Concrete implementation
 */
ವರ್ಗ SimpleFilteredSentenceBreakIterator : ಸಾರ್ವಜನಿಕ BreakIterator {
ಸಾರ್ವಜನಿಕ:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
  SimpleFilteredSentenceBreakIterator(ಸ್ಥಿರಾಂಕ SimpleFilteredSentenceBreakIterator& other);
  ವಾಸ್ತವಿಕ ~SimpleFilteredSentenceBreakIterator();
ಖಾಸಗಿ:
  Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ *f_ದತ್ತಾಂಶ;
  Local_ಸೂಚಕ<BreakIterator> fDelegate;
  Local_UText_ಸೂಚಕ           f_ಪಠ್ಯ;

  /* -- subclass interface -- */
ಸಾರ್ವಜನಿಕ:
  /* -- cloning and other subclass stuff -- */
  ವಾಸ್ತವಿಕ BreakIterator *  create_ಬಫರ್_Clone(ಶೂನ್ಯ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) override {
    // for now - always deep clone
    ಸ್ಥಿತಿ_2 = U_SAFECLONE_ALLOCATED_WARNING;
    ಹಿಂತಿರುಗಿ clone();
  }
  ವಾಸ್ತವಿಕ SimpleFilteredSentenceBreakIterator* clone() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಹೊಸ SimpleFilteredSentenceBreakIterator(*ಇದು); }
  ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
  ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ BreakIterator& o) ಸ್ಥಿರಾಂಕ override { ಆದರೆ(ಇದು==&o) ಹಿಂತಿರುಗಿ ನಿಜ; ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

  /* -- text modifying -- */
  ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಪಠ್ಯ(UText *ಪಠ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) override { fDelegate->ಗಣ_ಪಠ್ಯ(ಪಠ್ಯ,ಸ್ಥಿತಿ_2); }
  ವಾಸ್ತವಿಕ BreakIterator& refresh_ಒಳಹರಿವು_ಪಠ್ಯ(UText* input,
                                          UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND override {
      fDelegate->refresh_ಒಳಹರಿವು_ಪಠ್ಯ(input, ಸ್ಥಿತಿ_2);
      ಹಿಂತಿರುಗಿ *ಇದು;
  }
  ವಾಸ್ತವಿಕ ಶೂನ್ಯ adopt_ಪಠ್ಯ(CharacterIterator* it) override { fDelegate->adopt_ಪಠ್ಯ(it); }
  ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString &ಪಠ್ಯ) override { fDelegate->ಗಣ_ಪಠ್ಯ(ಪಠ್ಯ); }

  /* -- other functions that are just delegated -- */
  ವಾಸ್ತವಿಕ UText* getUText(UText* fill_ಒಳ U_LIFETIME_BOUND, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override {
      ಹಿಂತಿರುಗಿ fDelegate->getUText(fill_ಒಳ, ಸ್ಥಿತಿ_2);
  }
  ವಾಸ್ತವಿಕ CharacterIterator& get_ಪಠ್ಯ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND override {
      ಹಿಂತಿರುಗಿ fDelegate->get_ಪಠ್ಯ();
  }

  /* -- ITERATION -- */
  ವಾಸ್ತವಿಕ int32_t first() override;
  ವಾಸ್ತವಿಕ int32_t preceding(int32_t ಸ್ಥಾನಾಂತರ) override;
  ವಾಸ್ತವಿಕ int32_t ಹಿಂದಿನ() override;
  ವಾಸ್ತವಿಕ UBool isBoundary(int32_t ಸ್ಥಾನಾಂತರ) override;
  ವಾಸ್ತವಿಕ int32_t current() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ fDelegate->current(); } // we keep the delegate current, so this should be correct.

  ವಾಸ್ತವಿಕ int32_t next() override;

  ವಾಸ್ತವಿಕ int32_t next(int32_t n) override;
  ವಾಸ್ತವಿಕ int32_t following(int32_t ಸ್ಥಾನಾಂತರ) override;
  ವಾಸ್ತವಿಕ int32_t ಕೊನೆಯ() override;

ಖಾಸಗಿ:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t ಆಂತರಿಕ_ಮುಂದಿನ(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t ಆಂತರಿಕ_Prev(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    ಶೂನ್ಯ resetState(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    /**
     * Is there a match  (exception) at this spot?
     */
    ಎಣಿಕೆ EFBMatch_ಫಲಿತಾಂಶ { k_No_Exception_ಇಲ್ಲಿ, k_Exception_ಇಲ್ಲಿ };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    ಎಣಿಕೆ EFBMatch_ಫಲಿತಾಂಶ breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ಸ್ಥಿರಾಂಕ SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_ದತ್ತಾಂಶ(other.f_ದತ್ತಾಂಶ->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,ಸ್ಥಿತಿ_2),adopt->getLocale(ULOC_ACTUAL_LOCALE,ಸ್ಥಿತಿ_2)),
  f_ದತ್ತಾಂಶ(ಹೊಸ Simple_Filtered_Sentence_Break_ದತ್ತಾಂಶ(forwards, backwards)),
  fDelegate(adopt)
{
    ಆದರೆ (f_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಅಳಿಸು forwards;
        ಅಳಿಸು backwards;
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_ದತ್ತಾಂಶ = f_ದತ್ತಾಂಶ->decr();
}

ಶೂನ್ಯ SimpleFilteredSentenceBreakIterator::resetState(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
  f_ಪಠ್ಯ.adoptInstead(fDelegate->getUText(f_ಪಠ್ಯ.orphan(), ಸ್ಥಿತಿ_2));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_ಫಲಿತಾಂಶ
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t ಉತ್ತಮ_Posn = -1;
    int32_t ಉತ್ತಮ_ಮೌಲ್ಯ = -1;
    // loops while 'n' points to an exception.
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(f_ಪಠ್ಯ.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    ಆದರೆ(utext_previous32(f_ಪಠ್ಯ.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } ಇಲ್ಲದಿದ್ದರೆ {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(f_ಪಠ್ಯ.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(f_ದತ್ತಾಂಶ->getBackwardsTrie());
        UChar32 uch;
        ಆಗ((uch=utext_previous32(f_ಪಠ್ಯ.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_ಫಲಿತಾಂಶ r = iter.ಮುಂದಿನ_For_ಸಂಕೇತ_ಬಿಂದು(uch);
            ಆದರೆ(USTRINGTRIE_HAS_ಮೌಲ್ಯ(r)) { // remember the best match so far
                ಉತ್ತಮ_Posn = utext_get_Native_ಸೂಚ್ಯಂಕ(f_ಪಠ್ಯ.getAlias());
                ಉತ್ತಮ_ಮೌಲ್ಯ = iter.get_ಮೌಲ್ಯ();
            }
            ಆದರೆ(!USTRINGTRIE_HAS_ಮುಂದಿನ(r)) {
                ನಿಲ್ಲಿಸು;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    ಆದರೆ(ಉತ್ತಮ_Posn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      ಆದರೆ(ಉತ್ತಮ_ಮೌಲ್ಯ == k_ಹೊಂದಿಕೆ) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        ಹಿಂತಿರುಗಿ k_Exception_ಇಲ್ಲಿ; // See if the next is another exception.
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಉತ್ತಮ_ಮೌಲ್ಯ == kPARTIAL
                && f_ದತ್ತಾಂಶ->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_ಫಲಿತಾಂಶ rfwd = USTRINGTRIE_INTERMEDIATE_ಮೌಲ್ಯ;
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(f_ಪಠ್ಯ.getAlias(), ಉತ್ತಮ_Posn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_ದತ್ತಾಂಶ->getForwardsPartialTrie());
        UChar32 uch;
        ಆಗ((uch=utext_next32(f_ಪಠ್ಯ.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_ಮುಂದಿನ(rfwd=iter.ಮುಂದಿನ_For_ಸಂಕೇತ_ಬಿಂದು(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        ಆದರೆ(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            ಹಿಂತಿರುಗಿ k_Exception_ಇಲ್ಲಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          ಹಿಂತಿರುಗಿ k_No_Exception_ಇಲ್ಲಿ;
        }
      } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ k_No_Exception_ಇಲ್ಲಿ; // internal error and/or no forwards trie
      }
    } ಇಲ್ಲದಿದ್ದರೆ {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      ಹಿಂತಿರುಗಿ k_No_Exception_ಇಲ್ಲಿ; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::ಆಂತರಿಕ_ಮುಂದಿನ(int32_t n) {
  ಆದರೆ(n == UBRK_ಮುಗಿದ || // at end  or
    !f_ದತ್ತಾಂಶ->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ಹಿಂತಿರುಗಿ n;
  }
  // OK, do we need to break here?
  UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
  // refresh text
  resetState(ಸ್ಥಿತಿ_2);
  ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ UBRK_ಮುಗಿದ; // bail out
  int64_t utextLen = utext_native_ಉದ್ದಳತೆ(f_ಪಠ್ಯ.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ಆಗ (n != UBRK_ಮುಗಿದ && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_ಫಲಿತಾಂಶ m = breakExceptionAt(n);

    ಆಯ್ಕೆ(m) {
    ಸ್ಥಿತಿ k_Exception_ಇಲ್ಲಿ:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      ಮುಂದುವರಿ;

    ಪೂರ್ವನಿಯೋಜಿತ:
    ಸ್ಥಿತಿ k_No_Exception_ಇಲ್ಲಿ:
      ಹಿಂತಿರುಗಿ n;
    }    
  }
  ಹಿಂತಿರುಗಿ n;
}

int32_t
SimpleFilteredSentenceBreakIterator::ಆಂತರಿಕ_Prev(int32_t n) {
  ಆದರೆ(n == 0 || n == UBRK_ಮುಗಿದ || // at end  or
    !f_ದತ್ತಾಂಶ->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ಹಿಂತಿರುಗಿ n;
  }
  // OK, do we need to break here?
  UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
  // refresh text
  resetState(ಸ್ಥಿತಿ_2);
  ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ UBRK_ಮುಗಿದ; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ಆಗ (n != UBRK_ಮುಗಿದ && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_ಫಲಿತಾಂಶ m = breakExceptionAt(n);

    ಆಯ್ಕೆ(m) {
    ಸ್ಥಿತಿ k_Exception_ಇಲ್ಲಿ:
      n = fDelegate->ಹಿಂದಿನ(); // skip this one. Find the next lowerlevel break.
      ಮುಂದುವರಿ;

    ಪೂರ್ವನಿಯೋಜಿತ:
    ಸ್ಥಿತಿ k_No_Exception_ಇಲ್ಲಿ:
      ಹಿಂತಿರುಗಿ n;
    }    
  }
  ಹಿಂತಿರುಗಿ n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  ಹಿಂತಿರುಗಿ ಆಂತರಿಕ_ಮುಂದಿನ(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  ಹಿಂತಿರುಗಿ fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t ಸ್ಥಾನಾಂತರ) {
  ಹಿಂತಿರುಗಿ ಆಂತರಿಕ_Prev(fDelegate->preceding(ಸ್ಥಾನಾಂತರ));
}

int32_t
SimpleFilteredSentenceBreakIterator::ಹಿಂದಿನ() {
  ಹಿಂತಿರುಗಿ ಆಂತರಿಕ_Prev(fDelegate->ಹಿಂದಿನ());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t ಸ್ಥಾನಾಂತರ) {
  ಆದರೆ (!fDelegate->isBoundary(ಸ್ಥಾನಾಂತರ)) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; // no break to suppress

  ಆದರೆ (!f_ದತ್ತಾಂಶ->hasBackwardsTrie()) ಹಿಂತಿರುಗಿ ನಿಜ; // no data = no suppressions

  UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
  resetState(ಸ್ಥಿತಿ_2);

  SimpleFilteredSentenceBreakIterator::EFBMatch_ಫಲಿತಾಂಶ m = breakExceptionAt(ಸ್ಥಾನಾಂತರ);

  ಆಯ್ಕೆ(m) {
  ಸ್ಥಿತಿ k_Exception_ಇಲ್ಲಿ:
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  ಪೂರ್ವನಿಯೋಜಿತ:
  ಸ್ಥಿತಿ k_No_Exception_ಇಲ್ಲಿ:
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t ಸ್ಥಾನಾಂತರ) {
  ಹಿಂತಿರುಗಿ ಆಂತರಿಕ_ಮುಂದಿನ(fDelegate->next(ಸ್ಥಾನಾಂತರ));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t ಸ್ಥಾನಾಂತರ) {
  ಹಿಂತಿರುಗಿ ಆಂತರಿಕ_ಮುಂದಿನ(fDelegate->following(ಸ್ಥಾನಾಂತರ));
}

int32_t
SimpleFilteredSentenceBreakIterator::ಕೊನೆಯ() {
  // Don't suppress a break opportunity at the end of text.
  ಹಿಂತಿರುಗಿ fDelegate->ಕೊನೆಯ();
}


/**
 * Concrete implementation of builder class.
 */
ವರ್ಗ SimpleFilteredBreakIteratorBuilder : ಸಾರ್ವಜನಿಕ FilteredBreakIteratorBuilder {
ಸಾರ್ವಜನಿಕ:
  ವಾಸ್ತವಿಕ ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ಸ್ಥಿರಾಂಕ Locale &fromLocale, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
  SimpleFilteredBreakIteratorBuilder(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
  ವಾಸ್ತವಿಕ UBool suppressBreakAfter(ಸ್ಥಿರಾಂಕ UnicodeString& exception, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) override;
  ವಾಸ್ತವಿಕ UBool unsuppressBreakAfter(ಸ್ಥಿರಾಂಕ UnicodeString& exception, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) override;
  ವಾಸ್ತವಿಕ BreakIterator *build(BreakIterator* adoptBreakIterator, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) override;
ಖಾಸಗಿ:
  UString_ಗಣ f_ಗಣ;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) 
  : f_ಗಣ(ಸ್ಥಿತಿ_2)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ಸ್ಥಿರಾಂಕ Locale &fromLocale, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
  : f_ಗಣ(ಸ್ಥಿತಿ_2)
{
  ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
    UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    Local_UResource_Bundle_ಸೂಚಕ b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_ತಳ_Name(), &sub_ಸ್ಥಿತಿ));
    ಆದರೆ (U_FAILURE(sub_ಸ್ಥಿತಿ) || (sub_ಸ್ಥಿತಿ == U_USING_DEFAULT_WARNING) ) {    
      ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ; // copy the failing status 
#ಆದರೆ FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_ತಳ_Name(), "[exit]", u_errorName(ಸ್ಥಿತಿ_2));
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_ಸೂಚಕ exceptions(ures_get_By_ಕೀಲಿ_With_Fallback(b.getAlias(), "exceptions", ಶೂನ್ಯಸೂಚಕ, &sub_ಸ್ಥಿತಿ));
    ಆದರೆ (U_FAILURE(sub_ಸ್ಥಿತಿ) || (sub_ಸ್ಥಿತಿ == U_USING_DEFAULT_WARNING) ) {    
      ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ; // copy the failing status 
#ಆದರೆ FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_ತಳ_Name(), "[exit]", u_errorName(ಸ್ಥಿತಿ_2));
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_ಸೂಚಕ breaks(ures_get_By_ಕೀಲಿ_With_Fallback(exceptions.getAlias(), "SentenceBreak", ಶೂನ್ಯಸೂಚಕ, &sub_ಸ್ಥಿತಿ));

#ಆದರೆ FB_DEBUG
    {
      UError_ಸಂಕೇತ subsub = sub_ಸ್ಥಿತಿ;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_ತಳ_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_ಸ್ಥಿತಿ));
    }
#ಆದರೆ_ಅಂತ್ಯ
    
    ಆದರೆ (U_FAILURE(sub_ಸ್ಥಿತಿ) || (sub_ಸ್ಥಿತಿ == U_USING_DEFAULT_WARNING) ) {    
      ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ; // copy the failing status 
#ಆದರೆ FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_ತಳ_Name(), "[exit]", u_errorName(ಸ್ಥಿತಿ_2));
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ;  // leaves the builder empty, if you try to use it.
    }

    Local_UResource_Bundle_ಸೂಚಕ strs;
    sub_ಸ್ಥಿತಿ = ಸ್ಥಿತಿ_2; // Pick up inherited warning status now 
    ಮಾಡು {
      strs.adoptInstead(ures_get_ಮುಂದಿನ_Resource(breaks.getAlias(), strs.orphan(), &sub_ಸ್ಥಿತಿ));
      ಆದರೆ(strs.isValid() && U_SUCCESS(sub_ಸ್ಥಿತಿ)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &ಸ್ಥಿತಿ_2));
        suppressBreakAfter(str, ಸ್ಥಿತಿ_2); // load the string
      }
    } ಆಗ (strs.isValid() && U_SUCCESS(sub_ಸ್ಥಿತಿ));
    ಆದರೆ(U_FAILURE(sub_ಸ್ಥಿತಿ)&&sub_ಸ್ಥಿತಿ!=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR&&U_SUCCESS(ಸ್ಥಿತಿ_2)) {
      ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ಸ್ಥಿರಾಂಕ UnicodeString& exception, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
  UBool r = f_ಗಣ.ಕೂಡಿಸು(exception, ಸ್ಥಿತಿ_2);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  ಹಿಂತಿರುಗಿ r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ಸ್ಥಿರಾಂಕ UnicodeString& exception, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
  UBool r = f_ಗಣ.remove(exception, ಸ್ಥಿತಿ_2);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  ಹಿಂತಿರುಗಿ r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UnicodeString* newUnicodeStringArray(size_t count) {
    ಹಿಂತಿರುಗಿ ಹೊಸ UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
  Local_ಸೂಚಕ<BreakIterator> adopt(adoptBreakIterator);

  Local_ಸೂಚಕ<UCharsTrieBuilder> builder(ಹೊಸ UCharsTrieBuilder(ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
  Local_ಸೂಚಕ<UCharsTrieBuilder> builder2(ಹೊಸ UCharsTrieBuilder(ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
  ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  int32_t rev_ಎಣಿಕೆ = 0;
  int32_t fwd_ಎಣಿಕೆ = 0;

  int32_t sub_ಎಣಿಕೆ = f_ಗಣ.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_ಎಣಿಕೆ);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_ಸ್ಮರಣೆ<ಪೂರ್ಣಾಂಕ> partials;
  partials.allocateInsteadAndReset(sub_ಎಣಿಕೆ);

  Local_ಸೂಚಕ<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  Local_ಸೂಚಕ<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  ಪೂರ್ಣಾಂಕ n=0;
  ಗಾಗಿ ( int32_t i = 0;
        i<f_ಗಣ.size();
        i++) {
    ಸ್ಥಿರಾಂಕ UnicodeString *abbr = f_ಗಣ.getStringAt(i);
    ಆದರೆ(abbr) {
      FB_TRACE("build",abbr,ನಿಜ,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],ನಿಜ,i);
    } ಇಲ್ಲದಿದ್ದರೆ {
      FB_TRACE("build",abbr,ಸುಳ್ಳು,i);
      ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0;i<sub_ಎಣಿಕೆ;i++) {
    ಪೂರ್ಣಾಂಕ nn = ustrs[i].ಸೂಚ್ಯಂಕ_ದ(kFULLSTOP); // TODO: non-'.' abbreviations
    ಆದರೆ(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],ಸುಳ್ಳು,i);
      // is partial.
      // is it unique?
      ಪೂರ್ಣಾಂಕ sameAs = -1;
      ಗಾಗಿ(ಪೂರ್ಣಾಂಕ j=0;j<sub_ಎಣಿಕೆ;j++) {
        ಆದರೆ(j==i) ಮುಂದುವರಿ;
        ಆದರೆ(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],ಸುಳ್ಳು,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          ಆದರೆ(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_ಒಳ_ಹಿಮ್ಮುಖ | k_ಕೂಡಿಸು_ಗಮ್ಯ_Forward;
            FB_TRACE("suppressing",&ustrs[j],ಸುಳ್ಳು,j);
          } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(partials[j] & k_Suppress_ಒಳ_ಹಿಮ್ಮುಖ) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],ಸುಳ್ಳು,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],ಸುಳ್ಳು,partials[i]);
      UnicodeString ಪೂರ್ವಪ್ರತ್ಯಯ(ustrs[i], 0, nn+1);
      ಆದರೆ(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        ಪೂರ್ವಪ್ರತ್ಯಯ.reverse();
        builder->ಕೂಡಿಸು(ಪೂರ್ವಪ್ರತ್ಯಯ, kPARTIAL, ಸ್ಥಿತಿ_2);
        rev_ಎಣಿಕೆ++;
        FB_TRACE("Added partial",&ಪೂರ್ವಪ್ರತ್ಯಯ,ಸುಳ್ಳು, i);
        FB_TRACE(u_errorName(ಸ್ಥಿತಿ_2),&ustrs[i],ಸುಳ್ಳು,i);
        partials[i] = k_Suppress_ಒಳ_ಹಿಮ್ಮುಖ | k_ಕೂಡಿಸು_ಗಮ್ಯ_Forward;
      } ಇಲ್ಲದಿದ್ದರೆ {
        FB_TRACE("NOT adding partial",&ಪೂರ್ವಪ್ರತ್ಯಯ,ಸುಳ್ಳು, i);
        FB_TRACE(u_errorName(ಸ್ಥಿತಿ_2),&ustrs[i],ಸುಳ್ಳು,i);
      }
    }
  }
  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0;i<sub_ಎಣಿಕೆ;i++) {
    ಆದರೆ(partials[i]==0) {
      ustrs[i].reverse();
      builder->ಕೂಡಿಸು(ustrs[i], k_ಹೊಂದಿಕೆ, ಸ್ಥಿತಿ_2);
      rev_ಎಣಿಕೆ++;
      FB_TRACE(u_errorName(ಸ್ಥಿತಿ_2), &ustrs[i], ಸುಳ್ಳು, i);
    } ಇಲ್ಲದಿದ್ದರೆ {
      FB_TRACE("Adding fwd",&ustrs[i], ಸುಳ್ಳು, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->ಕೂಡಿಸು(ustrs[i], k_ಹೊಂದಿಕೆ, ಸ್ಥಿತಿ_2); // forward
      fwd_ಎಣಿಕೆ++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",ಶೂನ್ಯಸೂಚಕ,ಸುಳ್ಳು, sub_ಎಣಿಕೆ);

  ಆದರೆ(rev_ಎಣಿಕೆ>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, ಸ್ಥಿತಿ_2));
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
      FB_TRACE(u_errorName(ಸ್ಥಿತಿ_2),ಶೂನ್ಯಸೂಚಕ,ಸುಳ್ಳು, -1);
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
  }

  ಆದರೆ(fwd_ಎಣಿಕೆ>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, ಸ್ಥಿತಿ_2));
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
      FB_TRACE(u_errorName(ಸ್ಥಿತಿ_2),ಶೂನ್ಯಸೂಚಕ,ಸುಳ್ಳು, -1);
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
  }

  ಹಿಂತಿರುಗಿ ಹೊಸ SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), ಸ್ಥಿತಿ_2);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ಸ್ಥಿರಾಂಕ Locale& where, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
  ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  Local_ಸೂಚಕ<FilteredBreakIteratorBuilder> ret(ಹೊಸ SimpleFilteredBreakIteratorBuilder(where, ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
  ಹಿಂತಿರುಗಿ (U_SUCCESS(ಸ್ಥಿತಿ_2))? ret.orphan(): ಶೂನ್ಯಸೂಚಕ;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
  ಹಿಂತಿರುಗಿ create_ಖಾಲಿಗಳು_Instance(ಸ್ಥಿತಿ_2);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_ಖಾಲಿಗಳು_Instance(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
  ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  Local_ಸೂಚಕ<FilteredBreakIteratorBuilder> ret(ಹೊಸ SimpleFilteredBreakIteratorBuilder(ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
  ಹಿಂತಿರುಗಿ (U_SUCCESS(ಸ್ಥಿತಿ_2))? ret.orphan(): ಶೂನ್ಯಸೂಚಕ;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

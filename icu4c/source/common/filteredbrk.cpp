// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

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

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#ಯದಿ FB_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಸ್ಥಿರ ಶೂನ್ಯ _fb_trace(ಧ್ರುವ ವರ್ಣ *m, ಧ್ರುವ UnicodeString *s, UBool b, int32_t d, ಧ್ರುವ ವರ್ಣ *f, ಪೂರ್ಣಾಙ್ಕ l) {
  ವರ್ಣ buf[2048];
  ಯದಿ(s) {
    s->extract(0,s->length(),buf,2048);
  } ಅನ್ಯಥಾ {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ಧ್ರುವ ಶೂನ್ಯ*)s, b?'T':'F',(ಪೂರ್ಣಾಙ್ಕ)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#ಅನ್ಯಥಾ
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
ಸ್ಥಿರ int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ಧ್ರುವ UnicodeString& a = *static_cast<ಧ್ರುವ UnicodeString*>(t1.pointer);
    ಧ್ರುವ UnicodeString& b = *static_cast<ಧ್ರುವ UnicodeString*>(t2.pointer);
    ಪ್ರತ್ಯಾಗಮ a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
ವರ್ಗ UStringSet : ಸಾರ್ವಜನಿಕ UVector {
 ಸಾರ್ವಜನಿಕ:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  ಆಭಾಸ ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  ಅನ್ತಃಸ್ಥ UBool contains(ಧ್ರುವ UnicodeString& s) {
    ಪ್ರತ್ಯಾಗಮ contains((ಶೂನ್ಯ*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  ಅನ್ತಃಸ್ಥ ಧ್ರುವ UnicodeString* getStringAt(int32_t i) ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ static_cast<ಧ್ರುವ UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  ಅನ್ತಃಸ್ಥ UBool adopt(UnicodeString *str, UErrorCode &status) {
    ಯದಿ(U_FAILURE(status) || contains(*str)) {
      delete str;
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ {
      sorted_ಸೇರಿಸು(str, compareUnicodeString, status);
      ಯದಿ(U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  ಅನ್ತಃಸ್ಥ UBool add(ಧ್ರುವ UnicodeString& str, UErrorCode &status) {
    ಯದಿ(U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    UnicodeString *t = ನೂತನ UnicodeString(str);
    ಯದಿ(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  ಅನ್ತಃಸ್ಥ UBool remove(ಧ್ರುವ UnicodeString &s, UErrorCode &status) {
    ಯದಿ(U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    ಪ್ರತ್ಯಾಗಮ removeElement((ಶೂನ್ಯ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ಸ್ಥಿರ ಧ್ರುವ int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ಸ್ಥಿರ ಧ್ರುವ int32_t kMATCH   = (1<<1); //< exact match - skip this one.
ಸ್ಥಿರ ಧ್ರುವ int32_t kSuppressInReverse = (1<<0);
ಸ್ಥಿರ ಧ್ರುವ int32_t kAddToForward = (1<<1);
ಸ್ಥಿರ ಧ್ರುವ char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
ವರ್ಗ SimpleFilteredSentenceBreakData : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        ಪ್ರತ್ಯಾಗಮ ಏತತ್;
    }
    SimpleFilteredSentenceBreakData *decr() {
        ಯದಿ(umtx_atomic_dec(&refcount) <= 0) {
            delete ಏತತ್;
        }
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಆಭಾಸ ~SimpleFilteredSentenceBreakData();

    ತಾರ್ಕಿಕ hasForwardsPartialTrie() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ fForwardsPartialTrie.isValid(); }
    ತಾರ್ಕಿಕ hasBackwardsTrie() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ fBackwardsTrie.isValid(); }

    ಧ್ರುವ UCharsTrie& getForwardsPartialTrie() ಧ್ರುವ U_LIFETIME_BOUND {
        ಪ್ರತ್ಯಾಗಮ *fForwardsPartialTrie;
    }
    ಧ್ರುವ UCharsTrie& getBackwardsTrie() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ *fBackwardsTrie; }

ಗುಪ್ತ:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    LocalPointer<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    LocalPointer<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

SimpleFilteredSentenceBreakData::~SimpleFilteredSentenceBreakData() {}

/**
 * Concrete implementation
 */
ವರ್ಗ SimpleFilteredSentenceBreakIterator : ಸಾರ್ವಜನಿಕ BreakIterator {
ಸಾರ್ವಜನಿಕ:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(ಧ್ರುವ SimpleFilteredSentenceBreakIterator& other);
  ಆಭಾಸ ~SimpleFilteredSentenceBreakIterator();
ಗುಪ್ತ:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
ಸಾರ್ವಜನಿಕ:
  /* -- cloning and other subclass stuff -- */
  ಆಭಾಸ BreakIterator *  createBufferClone(ಶೂನ್ಯ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    ಪ್ರತ್ಯಾಗಮ clone();
  }
  ಆಭಾಸ SimpleFilteredSentenceBreakIterator* clone() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ ನೂತನ SimpleFilteredSentenceBreakIterator(*ಏತತ್); }
  ಆಭಾಸ UClassID getDynamicClassID() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ nullptr; }
  ಆಭಾಸ ತಾರ್ಕಿಕ operator==(ಧ್ರುವ BreakIterator& o) ಧ್ರುವ override { ಯದಿ(ಏತತ್==&o) ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }

  /* -- text modifying -- */
  ಆಭಾಸ ಶೂನ್ಯ setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  ಆಭಾಸ BreakIterator& refresh_ಒಳಹರಿವು_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_ಒಳಹರಿವು_Text(input, status);
      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
  }
  ಆಭಾಸ ಶೂನ್ಯ adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  ಆಭಾಸ ಶೂನ್ಯ setText(ಧ್ರುವ UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  ಆಭಾಸ UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ಧ್ರುವ override {
      ಪ್ರತ್ಯಾಗಮ fDelegate->getUText(fillIn, status);
  }
  ಆಭಾಸ CharacterIterator& getText() ಧ್ರುವ U_LIFETIME_BOUND override {
      ಪ್ರತ್ಯಾಗಮ fDelegate->getText();
  }

  /* -- ITERATION -- */
  ಆಭಾಸ int32_t first() override;
  ಆಭಾಸ int32_t preceding(int32_t offset) override;
  ಆಭಾಸ int32_t previous() override;
  ಆಭಾಸ UBool isBoundary(int32_t offset) override;
  ಆಭಾಸ int32_t current() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ fDelegate->current(); } // we keep the delegate current, so this should be correct.

  ಆಭಾಸ int32_t next() override;

  ಆಭಾಸ int32_t next(int32_t n) override;
  ಆಭಾಸ int32_t following(int32_t offset) override;
  ಆಭಾಸ int32_t last() override;

ಗುಪ್ತ:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t internalNext(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t internalPrev(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    ಶೂನ್ಯ resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    ಗಣನಾ EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    ಗಣನಾ EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ಧ್ರುವ SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(ನೂತನ SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    ಯದಿ (fData == nullptr) {
        delete forwards;
        delete backwards;
        ಯದಿ (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

ಶೂನ್ಯ SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_ಮೌಲ್ಯ = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    ಯದಿ(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } ಅನ್ಯಥಾ {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        ಯಾವತ್((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            ಯದಿ(USTRINGTRIE_HAS_ಮೌಲ್ಯ(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_ಮೌಲ್ಯ = iter.get_ಮೌಲ್ಯ();
            }
            ಯದಿ(!USTRINGTRIE_HAS_NEXT(r)) {
                ಭಙ್ಗ;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    ಯದಿ(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      ಯದಿ(best_ಮೌಲ್ಯ == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        ಪ್ರತ್ಯಾಗಮ kExceptionHere; // See if the next is another exception.
      } ಅನ್ಯಥಾ ಯದಿ(best_ಮೌಲ್ಯ == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_ಮೌಲ್ಯ;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        ಯಾವತ್((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        ಯದಿ(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            ಪ್ರತ್ಯಾಗಮ kExceptionHere;
        } ಅನ್ಯಥಾ {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          ಪ್ರತ್ಯಾಗಮ kNoExceptionHere;
        }
      } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ kNoExceptionHere; // internal error and/or no forwards trie
      }
    } ಅನ್ಯಥಾ {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      ಪ್ರತ್ಯಾಗಮ kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  ಯದಿ(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ಪ್ರತ್ಯಾಗಮ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  ಯದಿ(U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ಯಾವತ್ (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    ವಿಕಲ್ಪ(m) {
    ಪಕ್ಷ kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      ಅನುವರ್ತ;

    ಸ್ವಭಾವ:
    ಪಕ್ಷ kNoExceptionHere:
      ಪ್ರತ್ಯಾಗಮ n;
    }    
  }
  ಪ್ರತ್ಯಾಗಮ n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  ಯದಿ(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ಪ್ರತ್ಯಾಗಮ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  ಯದಿ(U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ಯಾವತ್ (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    ವಿಕಲ್ಪ(m) {
    ಪಕ್ಷ kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      ಅನುವರ್ತ;

    ಸ್ವಭಾವ:
    ಪಕ್ಷ kNoExceptionHere:
      ಪ್ರತ್ಯಾಗಮ n;
    }    
  }
  ಪ್ರತ್ಯಾಗಮ n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  ಪ್ರತ್ಯಾಗಮ internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  ಪ್ರತ್ಯಾಗಮ fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  ಪ್ರತ್ಯಾಗಮ internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  ಪ್ರತ್ಯಾಗಮ internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  ಯದಿ (!fDelegate->isBoundary(offset)) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; // no break to suppress

  ಯದಿ (!fData->hasBackwardsTrie()) ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  ವಿಕಲ್ಪ(m) {
  ಪಕ್ಷ kExceptionHere:
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
  ಸ್ವಭಾವ:
  ಪಕ್ಷ kNoExceptionHere:
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  ಪ್ರತ್ಯಾಗಮ internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  ಪ್ರತ್ಯಾಗಮ internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  ಪ್ರತ್ಯಾಗಮ fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
ವರ್ಗ SimpleFilteredBreakIteratorBuilder : ಸಾರ್ವಜನಿಕ FilteredBreakIteratorBuilder {
ಸಾರ್ವಜನಿಕ:
  ಆಭಾಸ ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ಧ್ರುವ Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  ಆಭಾಸ UBool suppressBreakAfter(ಧ್ರುವ UnicodeString& exception, UErrorCode& status) override;
  ಆಭಾಸ UBool unsuppressBreakAfter(ಧ್ರುವ UnicodeString& exception, UErrorCode& status) override;
  ಆಭಾಸ BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
ಗುಪ್ತ:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ಧ್ರುವ Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  ಯದಿ(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    ಯದಿ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ಯದಿ FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ಪ್ರತ್ಯಾಗಮ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_ಕೀಲಿ_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    ಯದಿ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ಯದಿ FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ಪ್ರತ್ಯಾಗಮ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_ಕೀಲಿ_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#ಯದಿ FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    ಯದಿ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ಯದಿ FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ಪ್ರತ್ಯಾಗಮ;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    ಕುರು {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      ಯದಿ(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } ಯಾವತ್ (strs.isValid() && U_SUCCESS(subStatus));
    ಯದಿ(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ಧ್ರುವ UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  ಪ್ರತ್ಯಾಗಮ r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ಧ್ರುವ UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  ಪ್ರತ್ಯಾಗಮ r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UnicodeString* newUnicodeStringArray(size_t count) {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(ನೂತನ UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(ನೂತನ UCharsTrieBuilder(status), status);
  ಯದಿ(U_FAILURE(status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  int32_t rev_ಎಣಿಕೆ = 0;
  int32_t fwd_ಎಣಿಕೆ = 0;

  int32_t sub_ಎಣಿಕೆ = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_ಎಣಿಕೆ);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<ಪೂರ್ಣಾಙ್ಕ> partials;
  partials.allocateInsteadAndReset(sub_ಎಣಿಕೆ);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  ಪೂರ್ಣಾಙ್ಕ n=0;
  ಪ್ರತಿ ( int32_t i = 0;
        i<fSet.size();
        i++) {
    ಧ್ರುವ UnicodeString *abbr = fSet.getStringAt(i);
    ಯದಿ(abbr) {
      FB_TRACE("build",abbr,ಸತ್ಯ,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],ಸತ್ಯ,i);
    } ಅನ್ಯಥಾ {
      FB_TRACE("build",abbr,ಅಸತ್ಯ,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0;i<sub_ಎಣಿಕೆ;i++) {
    ಪೂರ್ಣಾಙ್ಕ nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    ಯದಿ(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],ಅಸತ್ಯ,i);
      // is partial.
      // is it unique?
      ಪೂರ್ಣಾಙ್ಕ sameAs = -1;
      ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ j=0;j<sub_ಎಣಿಕೆ;j++) {
        ಯದಿ(j==i) ಅನುವರ್ತ;
        ಯದಿ(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],ಅಸತ್ಯ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          ಯದಿ(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],ಅಸತ್ಯ,j);
          } ಅನ್ಯಥಾ ಯದಿ(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],ಅಸತ್ಯ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],ಅಸತ್ಯ,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      ಯದಿ(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_ಎಣಿಕೆ++;
        FB_TRACE("Added partial",&prefix,ಅಸತ್ಯ, i);
        FB_TRACE(u_errorName(status),&ustrs[i],ಅಸತ್ಯ,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } ಅನ್ಯಥಾ {
        FB_TRACE("NOT adding partial",&prefix,ಅಸತ್ಯ, i);
        FB_TRACE(u_errorName(status),&ustrs[i],ಅಸತ್ಯ,i);
      }
    }
  }
  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0;i<sub_ಎಣಿಕೆ;i++) {
    ಯದಿ(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_ಎಣಿಕೆ++;
      FB_TRACE(u_errorName(status), &ustrs[i], ಅಸತ್ಯ, i);
    } ಅನ್ಯಥಾ {
      FB_TRACE("Adding fwd",&ustrs[i], ಅಸತ್ಯ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_ಎಣಿಕೆ++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,ಅಸತ್ಯ, sub_ಎಣಿಕೆ);

  ಯದಿ(rev_ಎಣಿಕೆ>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    ಯದಿ(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,ಅಸತ್ಯ, -1);
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
  }

  ಯದಿ(fwd_ಎಣಿಕೆ>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    ಯದಿ(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,ಅಸತ್ಯ, -1);
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
  }

  ಪ್ರತ್ಯಾಗಮ ನೂತನ SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ಧ್ರುವ Locale& where, UErrorCode& status) {
  ಯದಿ(U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(ನೂತನ SimpleFilteredBreakIteratorBuilder(where, status), status);
  ಪ್ರತ್ಯಾಗಮ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  ಪ್ರತ್ಯಾಗಮ createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  ಯದಿ(U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(ನೂತನ SimpleFilteredBreakIteratorBuilder(status), status);
  ಪ್ರತ್ಯಾಗಮ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

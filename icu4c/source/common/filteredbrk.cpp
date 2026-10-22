// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଯଦି !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "cmemory.h"

#ଅନ୍ତର୍ଭାଵ "unicode/filteredbrk.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucharstriebuilder.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"

#ଅନ୍ତର୍ଭାଵ "uresimp.h" // ures_getByKeyWithFallback
#ଅନ୍ତର୍ଭାଵ "ubrkimpl.h" // U_ICUDATA_BRKITR
#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#ଯଦି FB_DEBUG
#ଅନ୍ତର୍ଭାଵ <stdio.h>
ସ୍ଥିର ଶୂନ୍ୟ _fb_trace(ଅଚଳ ଅକ୍ଷର *m, ଅଚଳ UnicodeString *s, UBool b, int32_t d, ଅଚଳ ଅକ୍ଷର *f, ପୂର୍ଣ୍ଣସଂଖ୍ୟା l) {
  ଅକ୍ଷର buf[2048];
  ଯଦି(s) {
    s->extract(0,s->length(),buf,2048);
  } ଅନ୍ୟଥା {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ଅଚଳ ଶୂନ୍ୟ*)s, b?'T':'F',(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#ଅନ୍ୟଥା
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
ସ୍ଥିର int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ଅଚଳ UnicodeString& a = *static_cast<ଅଚଳ UnicodeString*>(t1.pointer);
    ଅଚଳ UnicodeString& b = *static_cast<ଅଚଳ UnicodeString*>(t2.pointer);
    ଫେରନ୍ତୁ a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
ଶ୍ରେଣୀ UStringSet : ସର୍ବସାଧାରଣ UVector {
 ସର୍ବସାଧାରଣ:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  ଆଭାସୀ ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  ଧାଡ଼ିଭିତରେ UBool contains(ଅଚଳ UnicodeString& s) {
    ଫେରନ୍ତୁ contains((ଶୂନ୍ୟ*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  ଧାଡ଼ିଭିତରେ ଅଚଳ UnicodeString* getStringAt(int32_t i) ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ static_cast<ଅଚଳ UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  ଧାଡ଼ିଭିତରେ UBool adopt(UnicodeString *str, UErrorCode &status) {
    ଯଦି(U_FAILURE(status) || contains(*str)) {
      delete str;
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
    } ଅନ୍ୟଥା {
      sorted_ଭର୍ତ୍ତି(str, compareUnicodeString, status);
      ଯଦି(U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }
      ଫେରନ୍ତୁ ସତ୍ୟ;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  ଧାଡ଼ିଭିତରେ UBool add(ଅଚଳ UnicodeString& str, UErrorCode &status) {
    ଯଦି(U_FAILURE(status)) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    UnicodeString *t = ନୂତନ UnicodeString(str);
    ଯଦି(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  ଧାଡ଼ିଭିତରେ UBool remove(ଅଚଳ UnicodeString &s, UErrorCode &status) {
    ଯଦି(U_FAILURE(status)) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    ଫେରନ୍ତୁ removeElement((ଶୂନ୍ୟ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ସ୍ଥିର ଅଚଳ int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ସ୍ଥିର ଅଚଳ int32_t kMATCH   = (1<<1); //< exact match - skip this one.
ସ୍ଥିର ଅଚଳ int32_t kSuppressInReverse = (1<<0);
ସ୍ଥିର ଅଚଳ int32_t kAddToForward = (1<<1);
ସ୍ଥିର ଅଚଳ char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
ଶ୍ରେଣୀ SimpleFilteredSentenceBreakData : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        ଫେରନ୍ତୁ ଏହି;
    }
    SimpleFilteredSentenceBreakData *decr() {
        ଯଦି(umtx_atomic_dec(&refcount) <= 0) {
            delete ଏହି;
        }
        ଫେରନ୍ତୁ nullptr;
    }
    ଆଭାସୀ ~SimpleFilteredSentenceBreakData();

    ତାର୍କିକ hasForwardsPartialTrie() ଅଚଳ { ଫେରନ୍ତୁ fForwardsPartialTrie.isValid(); }
    ତାର୍କିକ hasBackwardsTrie() ଅଚଳ { ଫେରନ୍ତୁ fBackwardsTrie.isValid(); }

    ଅଚଳ UCharsTrie& getForwardsPartialTrie() ଅଚଳ U_LIFETIME_BOUND {
        ଫେରନ୍ତୁ *fForwardsPartialTrie;
    }
    ଅଚଳ UCharsTrie& getBackwardsTrie() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ *fBackwardsTrie; }

ବ୍ୟକ୍ତିଗତ:
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
ଶ୍ରେଣୀ SimpleFilteredSentenceBreakIterator : ସର୍ବସାଧାରଣ BreakIterator {
ସର୍ବସାଧାରଣ:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(ଅଚଳ SimpleFilteredSentenceBreakIterator& other);
  ଆଭାସୀ ~SimpleFilteredSentenceBreakIterator();
ବ୍ୟକ୍ତିଗତ:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
ସର୍ବସାଧାରଣ:
  /* -- cloning and other subclass stuff -- */
  ଆଭାସୀ BreakIterator *  createBufferClone(ଶୂନ୍ୟ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    ଫେରନ୍ତୁ clone();
  }
  ଆଭାସୀ SimpleFilteredSentenceBreakIterator* clone() ଅଚଳ override { ଫେରନ୍ତୁ ନୂତନ SimpleFilteredSentenceBreakIterator(*ଏହି); }
  ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override { ଫେରନ୍ତୁ nullptr; }
  ଆଭାସୀ ତାର୍କିକ operator==(ଅଚଳ BreakIterator& o) ଅଚଳ override { ଯଦି(ଏହି==&o) ଫେରନ୍ତୁ ସତ୍ୟ; ଫେରନ୍ତୁ ମିଥ୍ୟା; }

  /* -- text modifying -- */
  ଆଭାସୀ ଶୂନ୍ୟ setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  ଆଭାସୀ BreakIterator& refresh_ନିବେଶ_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_ନିବେଶ_Text(input, status);
      ଫେରନ୍ତୁ *ଏହି;
  }
  ଆଭାସୀ ଶୂନ୍ୟ adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  ଆଭାସୀ ଶୂନ୍ୟ setText(ଅଚଳ UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  ଆଭାସୀ UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ଅଚଳ override {
      ଫେରନ୍ତୁ fDelegate->getUText(fillIn, status);
  }
  ଆଭାସୀ CharacterIterator& getText() ଅଚଳ U_LIFETIME_BOUND override {
      ଫେରନ୍ତୁ fDelegate->getText();
  }

  /* -- ITERATION -- */
  ଆଭାସୀ int32_t first() override;
  ଆଭାସୀ int32_t preceding(int32_t offset) override;
  ଆଭାସୀ int32_t previous() override;
  ଆଭାସୀ UBool isBoundary(int32_t offset) override;
  ଆଭାସୀ int32_t current() ଅଚଳ override { ଫେରନ୍ତୁ fDelegate->current(); } // we keep the delegate current, so this should be correct.

  ଆଭାସୀ int32_t next() override;

  ଆଭାସୀ int32_t next(int32_t n) override;
  ଆଭାସୀ int32_t following(int32_t offset) override;
  ଆଭାସୀ int32_t last() override;

ବ୍ୟକ୍ତିଗତ:
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
    ଶୂନ୍ୟ resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    ଗଣନା EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    ଗଣନା EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ଅଚଳ SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(ନୂତନ SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    ଯଦି (fData == nullptr) {
        delete forwards;
        delete backwards;
        ଯଦି (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

ଶୂନ୍ୟ SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_ମୂଲ୍ୟ = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    ଯଦି(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } ଅନ୍ୟଥା {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        ଯେତେବେଳେ((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            ଯଦି(USTRINGTRIE_HAS_ମୂଲ୍ୟ(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_ମୂଲ୍ୟ = iter.get_ମୂଲ୍ୟ();
            }
            ଯଦି(!USTRINGTRIE_HAS_NEXT(r)) {
                ଭାଙ୍ଗ;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    ଯଦି(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      ଯଦି(best_ମୂଲ୍ୟ == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        ଫେରନ୍ତୁ kExceptionHere; // See if the next is another exception.
      } ଅନ୍ୟଥା ଯଦି(best_ମୂଲ୍ୟ == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_ମୂଲ୍ୟ;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        ଯେତେବେଳେ((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        ଯଦି(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            ଫେରନ୍ତୁ kExceptionHere;
        } ଅନ୍ୟଥା {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          ଫେରନ୍ତୁ kNoExceptionHere;
        }
      } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ kNoExceptionHere; // internal error and/or no forwards trie
      }
    } ଅନ୍ୟଥା {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      ଫେରନ୍ତୁ kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  ଯଦି(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ଫେରନ୍ତୁ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  ଯଦି(U_FAILURE(status)) ଫେରନ୍ତୁ UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ଯେତେବେଳେ (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    ବାଛ(m) {
    ସ୍ଥିତି kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      ଜାରି;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ସ୍ଥିତି kNoExceptionHere:
      ଫେରନ୍ତୁ n;
    }    
  }
  ଫେରନ୍ତୁ n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  ଯଦି(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ଫେରନ୍ତୁ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  ଯଦି(U_FAILURE(status)) ଫେରନ୍ତୁ UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ଯେତେବେଳେ (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    ବାଛ(m) {
    ସ୍ଥିତି kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      ଜାରି;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ସ୍ଥିତି kNoExceptionHere:
      ଫେରନ୍ତୁ n;
    }    
  }
  ଫେରନ୍ତୁ n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  ଫେରନ୍ତୁ internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  ଫେରନ୍ତୁ fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  ଫେରନ୍ତୁ internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  ଫେରନ୍ତୁ internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  ଯଦି (!fDelegate->isBoundary(offset)) ଫେରନ୍ତୁ ମିଥ୍ୟା; // no break to suppress

  ଯଦି (!fData->hasBackwardsTrie()) ଫେରନ୍ତୁ ସତ୍ୟ; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  ବାଛ(m) {
  ସ୍ଥିତି kExceptionHere:
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
  ସ୍ଥିତି kNoExceptionHere:
    ଫେରନ୍ତୁ ସତ୍ୟ;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  ଫେରନ୍ତୁ internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  ଫେରନ୍ତୁ internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  ଫେରନ୍ତୁ fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
ଶ୍ରେଣୀ SimpleFilteredBreakIteratorBuilder : ସର୍ବସାଧାରଣ FilteredBreakIteratorBuilder {
ସର୍ବସାଧାରଣ:
  ଆଭାସୀ ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ଅଚଳ Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  ଆଭାସୀ UBool suppressBreakAfter(ଅଚଳ UnicodeString& exception, UErrorCode& status) override;
  ଆଭାସୀ UBool unsuppressBreakAfter(ଅଚଳ UnicodeString& exception, UErrorCode& status) override;
  ଆଭାସୀ BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
ବ୍ୟକ୍ତିଗତ:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ଅଚଳ Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  ଯଦି(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    ଯଦି (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ଯଦି FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ଫେରନ୍ତୁ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_ଚାବି_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    ଯଦି (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ଯଦି FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ଫେରନ୍ତୁ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_ଚାବି_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#ଯଦି FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    ଯଦି (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ଯଦି FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ଫେରନ୍ତୁ;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    କର {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      ଯଦି(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } ଯେତେବେଳେ (strs.isValid() && U_SUCCESS(subStatus));
    ଯଦି(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ଅଚଳ UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  ଫେରନ୍ତୁ r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ଅଚଳ UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  ଫେରନ୍ତୁ r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UnicodeString* newUnicodeStringArray(size_t count) {
    ଫେରନ୍ତୁ ନୂତନ UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(ନୂତନ UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(ନୂତନ UCharsTrieBuilder(status), status);
  ଯଦି(U_FAILURE(status)) {
    ଫେରନ୍ତୁ nullptr;
  }

  int32_t rev_ଗଣନା = 0;
  int32_t fwd_ଗଣନା = 0;

  int32_t sub_ଗଣନା = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_ଗଣନା);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> partials;
  partials.allocateInsteadAndReset(sub_ଗଣନା);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n=0;
  ପାଇଁ ( int32_t i = 0;
        i<fSet.size();
        i++) {
    ଅଚଳ UnicodeString *abbr = fSet.getStringAt(i);
    ଯଦି(abbr) {
      FB_TRACE("build",abbr,ସତ୍ୟ,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],ସତ୍ୟ,i);
    } ଅନ୍ୟଥା {
      FB_TRACE("build",abbr,ମିଥ୍ୟା,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      ଫେରନ୍ତୁ nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0;i<sub_ଗଣନା;i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    ଯଦି(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],ମିଥ୍ୟା,i);
      // is partial.
      // is it unique?
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sameAs = -1;
      ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା j=0;j<sub_ଗଣନା;j++) {
        ଯଦି(j==i) ଜାରି;
        ଯଦି(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],ମିଥ୍ୟା,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          ଯଦି(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],ମିଥ୍ୟା,j);
          } ଅନ୍ୟଥା ଯଦି(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],ମିଥ୍ୟା,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],ମିଥ୍ୟା,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      ଯଦି(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_ଗଣନା++;
        FB_TRACE("Added partial",&prefix,ମିଥ୍ୟା, i);
        FB_TRACE(u_errorName(status),&ustrs[i],ମିଥ୍ୟା,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } ଅନ୍ୟଥା {
        FB_TRACE("NOT adding partial",&prefix,ମିଥ୍ୟା, i);
        FB_TRACE(u_errorName(status),&ustrs[i],ମିଥ୍ୟା,i);
      }
    }
  }
  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0;i<sub_ଗଣନା;i++) {
    ଯଦି(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_ଗଣନା++;
      FB_TRACE(u_errorName(status), &ustrs[i], ମିଥ୍ୟା, i);
    } ଅନ୍ୟଥା {
      FB_TRACE("Adding fwd",&ustrs[i], ମିଥ୍ୟା, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_ଗଣନା++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,ମିଥ୍ୟା, sub_ଗଣନା);

  ଯଦି(rev_ଗଣନା>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    ଯଦି(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,ମିଥ୍ୟା, -1);
      ଫେରନ୍ତୁ nullptr;
    }
  }

  ଯଦି(fwd_ଗଣନା>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    ଯଦି(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,ମିଥ୍ୟା, -1);
      ଫେରନ୍ତୁ nullptr;
    }
  }

  ଫେରନ୍ତୁ ନୂତନ SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ଅଚଳ Locale& where, UErrorCode& status) {
  ଯଦି(U_FAILURE(status)) ଫେରନ୍ତୁ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(ନୂତନ SimpleFilteredBreakIteratorBuilder(where, status), status);
  ଫେରନ୍ତୁ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  ଫେରନ୍ତୁ createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  ଯଦି(U_FAILURE(status)) ଫେରନ୍ତୁ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(ନୂତନ SimpleFilteredBreakIteratorBuilder(status), status);
  ଫେରନ୍ତୁ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

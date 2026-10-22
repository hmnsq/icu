// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#അന്തര്ഭാവ "unicode/utypes.h"
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#അന്തര്ഭാവ "cmemory.h"

#അന്തര്ഭാവ "unicode/filteredbrk.h"
#അന്തര്ഭാവ "unicode/ucharstriebuilder.h"
#അന്തര്ഭാവ "unicode/ures.h"

#അന്തര്ഭാവ "uresimp.h" // ures_getByKeyWithFallback
#അന്തര്ഭാവ "ubrkimpl.h" // U_ICUDATA_BRKITR
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#എങ്കിൽ FB_DEBUG
#അന്തര്ഭാവ <stdio.h>
സ്ഥിതം ശൂന്യം _fb_trace(സ്ഥിരം അക്ഷരം *m, സ്ഥിരം UnicodeString *s, UBool b, int32_t d, സ്ഥിരം അക്ഷരം *f, പൂർണ്ണസംഖ്യ l) {
  അക്ഷരം buf[2048];
  എങ്കിൽ(s) {
    s->extract(0,s->length(),buf,2048);
  } അല്ലെങ്കിൽ {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (സ്ഥിരം ശൂന്യം*)s, b?'T':'F',(പൂർണ്ണസംഖ്യ)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#അല്ലെങ്കിൽ
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
സ്ഥിതം int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    സ്ഥിരം UnicodeString& a = *static_cast<സ്ഥിരം UnicodeString*>(t1.pointer);
    സ്ഥിരം UnicodeString& b = *static_cast<സ്ഥിരം UnicodeString*>(t2.pointer);
    തിരികെ a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
വർഗ്ഗം UStringSet : പൊതു UVector {
 പൊതു:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  മായാ ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  വരിയിൽ UBool contains(സ്ഥിരം UnicodeString& s) {
    തിരികെ contains((ശൂന്യം*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  വരിയിൽ സ്ഥിരം UnicodeString* getStringAt(int32_t i) സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ static_cast<സ്ഥിരം UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  വരിയിൽ UBool adopt(UnicodeString *str, UErrorCode &status) {
    എങ്കിൽ(U_FAILURE(status) || contains(*str)) {
      delete str;
      തിരികെ അസത്യം;
    } അല്ലെങ്കിൽ {
      sorted_തിരുകുക(str, compareUnicodeString, status);
      എങ്കിൽ(U_FAILURE(status)) {
        തിരികെ അസത്യം;
      }
      തിരികെ സത്യം;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  വരിയിൽ UBool add(സ്ഥിരം UnicodeString& str, UErrorCode &status) {
    എങ്കിൽ(U_FAILURE(status)) തിരികെ അസത്യം;
    UnicodeString *t = പുതിയ UnicodeString(str);
    എങ്കിൽ(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; തിരികെ അസത്യം;
    }
    തിരികെ adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  വരിയിൽ UBool remove(സ്ഥിരം UnicodeString &s, UErrorCode &status) {
    എങ്കിൽ(U_FAILURE(status)) തിരികെ അസത്യം;
    തിരികെ removeElement((ശൂന്യം*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
സ്ഥിതം സ്ഥിരം int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
സ്ഥിതം സ്ഥിരം int32_t kMATCH   = (1<<1); //< exact match - skip this one.
സ്ഥിതം സ്ഥിരം int32_t kSuppressInReverse = (1<<0);
സ്ഥിതം സ്ഥിരം int32_t kAddToForward = (1<<1);
സ്ഥിതം സ്ഥിരം char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
വർഗ്ഗം SimpleFilteredSentenceBreakData : പൊതു UMemory {
പൊതു:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        തിരികെ ഇത്;
    }
    SimpleFilteredSentenceBreakData *decr() {
        എങ്കിൽ(umtx_atomic_dec(&refcount) <= 0) {
            delete ഇത്;
        }
        തിരികെ nullptr;
    }
    മായാ ~SimpleFilteredSentenceBreakData();

    തർക്കം hasForwardsPartialTrie() സ്ഥിരം { തിരികെ fForwardsPartialTrie.isValid(); }
    തർക്കം hasBackwardsTrie() സ്ഥിരം { തിരികെ fBackwardsTrie.isValid(); }

    സ്ഥിരം UCharsTrie& getForwardsPartialTrie() സ്ഥിരം U_LIFETIME_BOUND {
        തിരികെ *fForwardsPartialTrie;
    }
    സ്ഥിരം UCharsTrie& getBackwardsTrie() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ *fBackwardsTrie; }

സ്വകാര്യ:
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
വർഗ്ഗം SimpleFilteredSentenceBreakIterator : പൊതു BreakIterator {
പൊതു:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(സ്ഥിരം SimpleFilteredSentenceBreakIterator& other);
  മായാ ~SimpleFilteredSentenceBreakIterator();
സ്വകാര്യ:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
പൊതു:
  /* -- cloning and other subclass stuff -- */
  മായാ BreakIterator *  createBufferClone(ശൂന്യം * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    തിരികെ clone();
  }
  മായാ SimpleFilteredSentenceBreakIterator* clone() സ്ഥിരം override { തിരികെ പുതിയ SimpleFilteredSentenceBreakIterator(*ഇത്); }
  മായാ UClassID getDynamicClassID() സ്ഥിരം override { തിരികെ nullptr; }
  മായാ തർക്കം operator==(സ്ഥിരം BreakIterator& o) സ്ഥിരം override { എങ്കിൽ(ഇത്==&o) തിരികെ സത്യം; തിരികെ അസത്യം; }

  /* -- text modifying -- */
  മായാ ശൂന്യം setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  മായാ BreakIterator& refresh_നിവേശം_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_നിവേശം_Text(input, status);
      തിരികെ *ഇത്;
  }
  മായാ ശൂന്യം adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  മായാ ശൂന്യം setText(സ്ഥിരം UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  മായാ UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) സ്ഥിരം override {
      തിരികെ fDelegate->getUText(fillIn, status);
  }
  മായാ CharacterIterator& getText() സ്ഥിരം U_LIFETIME_BOUND override {
      തിരികെ fDelegate->getText();
  }

  /* -- ITERATION -- */
  മായാ int32_t first() override;
  മായാ int32_t preceding(int32_t offset) override;
  മായാ int32_t previous() override;
  മായാ UBool isBoundary(int32_t offset) override;
  മായാ int32_t current() സ്ഥിരം override { തിരികെ fDelegate->current(); } // we keep the delegate current, so this should be correct.

  മായാ int32_t next() override;

  മായാ int32_t next(int32_t n) override;
  മായാ int32_t following(int32_t offset) override;
  മായാ int32_t last() override;

സ്വകാര്യ:
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
    ശൂന്യം resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    എണ്ണൽ EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    എണ്ണൽ EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(സ്ഥിരം SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(പുതിയ SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    എങ്കിൽ (fData == nullptr) {
        delete forwards;
        delete backwards;
        എങ്കിൽ (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

ശൂന്യം SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_മൂല്യം = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    എങ്കിൽ(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } അല്ലെങ്കിൽ {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        ആകുമ്പോൾ((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            എങ്കിൽ(USTRINGTRIE_HAS_മൂല്യം(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_മൂല്യം = iter.get_മൂല്യം();
            }
            എങ്കിൽ(!USTRINGTRIE_HAS_NEXT(r)) {
                നിർത്തുക;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    എങ്കിൽ(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      എങ്കിൽ(best_മൂല്യം == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        തിരികെ kExceptionHere; // See if the next is another exception.
      } അല്ലെങ്കിൽ എങ്കിൽ(best_മൂല്യം == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_മൂല്യം;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        ആകുമ്പോൾ((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        എങ്കിൽ(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            തിരികെ kExceptionHere;
        } അല്ലെങ്കിൽ {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          തിരികെ kNoExceptionHere;
        }
      } അല്ലെങ്കിൽ {
        തിരികെ kNoExceptionHere; // internal error and/or no forwards trie
      }
    } അല്ലെങ്കിൽ {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      തിരികെ kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  എങ്കിൽ(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      തിരികെ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  എങ്കിൽ(U_FAILURE(status)) തിരികെ UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ആകുമ്പോൾ (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    തിരഞ്ഞെടുപ്പ്(m) {
    അവസ്ഥ kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      തുടരുക;

    സ്ഥിരസ്ഥിതി:
    അവസ്ഥ kNoExceptionHere:
      തിരികെ n;
    }    
  }
  തിരികെ n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  എങ്കിൽ(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      തിരികെ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  എങ്കിൽ(U_FAILURE(status)) തിരികെ UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ആകുമ്പോൾ (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    തിരഞ്ഞെടുപ്പ്(m) {
    അവസ്ഥ kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      തുടരുക;

    സ്ഥിരസ്ഥിതി:
    അവസ്ഥ kNoExceptionHere:
      തിരികെ n;
    }    
  }
  തിരികെ n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  തിരികെ internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  തിരികെ fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  തിരികെ internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  തിരികെ internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  എങ്കിൽ (!fDelegate->isBoundary(offset)) തിരികെ അസത്യം; // no break to suppress

  എങ്കിൽ (!fData->hasBackwardsTrie()) തിരികെ സത്യം; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  തിരഞ്ഞെടുപ്പ്(m) {
  അവസ്ഥ kExceptionHere:
    തിരികെ അസത്യം;
  സ്ഥിരസ്ഥിതി:
  അവസ്ഥ kNoExceptionHere:
    തിരികെ സത്യം;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  തിരികെ internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  തിരികെ internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  തിരികെ fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
വർഗ്ഗം SimpleFilteredBreakIteratorBuilder : പൊതു FilteredBreakIteratorBuilder {
പൊതു:
  മായാ ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(സ്ഥിരം Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  മായാ UBool suppressBreakAfter(സ്ഥിരം UnicodeString& exception, UErrorCode& status) override;
  മായാ UBool unsuppressBreakAfter(സ്ഥിരം UnicodeString& exception, UErrorCode& status) override;
  മായാ BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
സ്വകാര്യ:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(സ്ഥിരം Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  എങ്കിൽ(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    എങ്കിൽ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#എങ്കിൽ FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      തിരികെ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_താക്കോൽ_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    എങ്കിൽ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#എങ്കിൽ FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      തിരികെ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_താക്കോൽ_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#എങ്കിൽ FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    എങ്കിൽ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#എങ്കിൽ FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      തിരികെ;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    ചെയ്യുക {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      എങ്കിൽ(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } ആകുമ്പോൾ (strs.isValid() && U_SUCCESS(subStatus));
    എങ്കിൽ(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(സ്ഥിരം UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  തിരികെ r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(സ്ഥിരം UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  തിരികെ r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
സ്ഥിതം വരിയിൽ UnicodeString* newUnicodeStringArray(size_t count) {
    തിരികെ പുതിയ UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(പുതിയ UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(പുതിയ UCharsTrieBuilder(status), status);
  എങ്കിൽ(U_FAILURE(status)) {
    തിരികെ nullptr;
  }

  int32_t rev_എണ്ണം = 0;
  int32_t fwd_എണ്ണം = 0;

  int32_t sub_എണ്ണം = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_എണ്ണം);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<പൂർണ്ണസംഖ്യ> partials;
  partials.allocateInsteadAndReset(sub_എണ്ണം);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  പൂർണ്ണസംഖ്യ n=0;
  വേണ്ടി ( int32_t i = 0;
        i<fSet.size();
        i++) {
    സ്ഥിരം UnicodeString *abbr = fSet.getStringAt(i);
    എങ്കിൽ(abbr) {
      FB_TRACE("build",abbr,സത്യം,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],സത്യം,i);
    } അല്ലെങ്കിൽ {
      FB_TRACE("build",abbr,അസത്യം,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      തിരികെ nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  വേണ്ടി(പൂർണ്ണസംഖ്യ i=0;i<sub_എണ്ണം;i++) {
    പൂർണ്ണസംഖ്യ nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    എങ്കിൽ(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],അസത്യം,i);
      // is partial.
      // is it unique?
      പൂർണ്ണസംഖ്യ sameAs = -1;
      വേണ്ടി(പൂർണ്ണസംഖ്യ j=0;j<sub_എണ്ണം;j++) {
        എങ്കിൽ(j==i) തുടരുക;
        എങ്കിൽ(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],അസത്യം,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          എങ്കിൽ(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],അസത്യം,j);
          } അല്ലെങ്കിൽ എങ്കിൽ(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],അസത്യം,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],അസത്യം,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      എങ്കിൽ(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_എണ്ണം++;
        FB_TRACE("Added partial",&prefix,അസത്യം, i);
        FB_TRACE(u_errorName(status),&ustrs[i],അസത്യം,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } അല്ലെങ്കിൽ {
        FB_TRACE("NOT adding partial",&prefix,അസത്യം, i);
        FB_TRACE(u_errorName(status),&ustrs[i],അസത്യം,i);
      }
    }
  }
  വേണ്ടി(പൂർണ്ണസംഖ്യ i=0;i<sub_എണ്ണം;i++) {
    എങ്കിൽ(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_എണ്ണം++;
      FB_TRACE(u_errorName(status), &ustrs[i], അസത്യം, i);
    } അല്ലെങ്കിൽ {
      FB_TRACE("Adding fwd",&ustrs[i], അസത്യം, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_എണ്ണം++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,അസത്യം, sub_എണ്ണം);

  എങ്കിൽ(rev_എണ്ണം>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    എങ്കിൽ(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,അസത്യം, -1);
      തിരികെ nullptr;
    }
  }

  എങ്കിൽ(fwd_എണ്ണം>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    എങ്കിൽ(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,അസത്യം, -1);
      തിരികെ nullptr;
    }
  }

  തിരികെ പുതിയ SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(സ്ഥിരം Locale& where, UErrorCode& status) {
  എങ്കിൽ(U_FAILURE(status)) തിരികെ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(പുതിയ SimpleFilteredBreakIteratorBuilder(where, status), status);
  തിരികെ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  തിരികെ createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  എങ്കിൽ(U_FAILURE(status)) തിരികെ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(പുതിയ SimpleFilteredBreakIteratorBuilder(status), status);
  തിരികെ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

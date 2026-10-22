// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"
#जइ !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#अन्तर्भाव "cmemory.h"

#अन्तर्भाव "unicode/filteredbrk.h"
#अन्तर्भाव "unicode/ucharstriebuilder.h"
#अन्तर्भाव "unicode/ures.h"

#अन्तर्भाव "uresimp.h" // ures_getByKeyWithFallback
#अन्तर्भाव "ubrkimpl.h" // U_ICUDATA_BRKITR
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#जइ FB_DEBUG
#अन्तर्भाव <stdio.h>
स्थिर शून्य _fb_trace(ध्रुव वर्ण *m, ध्रुव UnicodeString *s, UBool b, int32_t d, ध्रुव वर्ण *f, पूर्णाङ्क l) {
  वर्ण buf[2048];
  जइ(s) {
    s->extract(0,s->length(),buf,2048);
  } अण्णहा {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ध्रुव शून्य*)s, b?'T':'F',(पूर्णाङ्क)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#अण्णहा
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
स्थिर int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ध्रुव UnicodeString& a = *static_cast<ध्रुव UnicodeString*>(t1.pointer);
    ध्रुव UnicodeString& b = *static_cast<ध्रुव UnicodeString*>(t2.pointer);
    प्रत्यागम a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
वर्ग UStringSet : सार्वजनिक UVector {
 सार्वजनिक:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  आभास ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  अन्तःस्थ UBool contains(ध्रुव UnicodeString& s) {
    प्रत्यागम contains((शून्य*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  अन्तःस्थ ध्रुव UnicodeString* getStringAt(int32_t i) ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम static_cast<ध्रुव UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  अन्तःस्थ UBool adopt(UnicodeString *str, UErrorCode &status) {
    जइ(U_FAILURE(status) || contains(*str)) {
      delete str;
      प्रत्यागम मिच्छ;
    } अण्णहा {
      sorted_निवेशय(str, compareUnicodeString, status);
      जइ(U_FAILURE(status)) {
        प्रत्यागम मिच्छ;
      }
      प्रत्यागम सच्च;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  अन्तःस्थ UBool add(ध्रुव UnicodeString& str, UErrorCode &status) {
    जइ(U_FAILURE(status)) प्रत्यागम मिच्छ;
    UnicodeString *t = नूतन UnicodeString(str);
    जइ(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; प्रत्यागम मिच्छ;
    }
    प्रत्यागम adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  अन्तःस्थ UBool remove(ध्रुव UnicodeString &s, UErrorCode &status) {
    जइ(U_FAILURE(status)) प्रत्यागम मिच्छ;
    प्रत्यागम removeElement((शून्य*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
स्थिर ध्रुव int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
स्थिर ध्रुव int32_t kMATCH   = (1<<1); //< exact match - skip this one.
स्थिर ध्रुव int32_t kSuppressInReverse = (1<<0);
स्थिर ध्रुव int32_t kAddToForward = (1<<1);
स्थिर ध्रुव char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
वर्ग SimpleFilteredSentenceBreakData : सार्वजनिक UMemory {
सार्वजनिक:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        प्रत्यागम एतत्;
    }
    SimpleFilteredSentenceBreakData *decr() {
        जइ(umtx_atomic_dec(&refcount) <= 0) {
            delete एतत्;
        }
        प्रत्यागम nullptr;
    }
    आभास ~SimpleFilteredSentenceBreakData();

    तार्किक hasForwardsPartialTrie() ध्रुव { प्रत्यागम fForwardsPartialTrie.isValid(); }
    तार्किक hasBackwardsTrie() ध्रुव { प्रत्यागम fBackwardsTrie.isValid(); }

    ध्रुव UCharsTrie& getForwardsPartialTrie() ध्रुव U_LIFETIME_BOUND {
        प्रत्यागम *fForwardsPartialTrie;
    }
    ध्रुव UCharsTrie& getBackwardsTrie() ध्रुव U_LIFETIME_BOUND { प्रत्यागम *fBackwardsTrie; }

गुप्त:
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
वर्ग SimpleFilteredSentenceBreakIterator : सार्वजनिक BreakIterator {
सार्वजनिक:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(ध्रुव SimpleFilteredSentenceBreakIterator& other);
  आभास ~SimpleFilteredSentenceBreakIterator();
गुप्त:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
सार्वजनिक:
  /* -- cloning and other subclass stuff -- */
  आभास BreakIterator *  createBufferClone(शून्य * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    प्रत्यागम clone();
  }
  आभास SimpleFilteredSentenceBreakIterator* clone() ध्रुव override { प्रत्यागम नूतन SimpleFilteredSentenceBreakIterator(*एतत्); }
  आभास UClassID getDynamicClassID() ध्रुव override { प्रत्यागम nullptr; }
  आभास तार्किक operator==(ध्रुव BreakIterator& o) ध्रुव override { जइ(एतत्==&o) प्रत्यागम सच्च; प्रत्यागम मिच्छ; }

  /* -- text modifying -- */
  आभास शून्य setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  आभास BreakIterator& refresh_निवेशः_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_निवेशः_Text(input, status);
      प्रत्यागम *एतत्;
  }
  आभास शून्य adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  आभास शून्य setText(ध्रुव UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  आभास UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ध्रुव override {
      प्रत्यागम fDelegate->getUText(fillIn, status);
  }
  आभास CharacterIterator& getText() ध्रुव U_LIFETIME_BOUND override {
      प्रत्यागम fDelegate->getText();
  }

  /* -- ITERATION -- */
  आभास int32_t first() override;
  आभास int32_t preceding(int32_t offset) override;
  आभास int32_t previous() override;
  आभास UBool isBoundary(int32_t offset) override;
  आभास int32_t current() ध्रुव override { प्रत्यागम fDelegate->current(); } // we keep the delegate current, so this should be correct.

  आभास int32_t next() override;

  आभास int32_t next(int32_t n) override;
  आभास int32_t following(int32_t offset) override;
  आभास int32_t last() override;

गुप्त:
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
    शून्य resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    गणना EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    गणना EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ध्रुव SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(नूतन SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    जइ (fData == nullptr) {
        delete forwards;
        delete backwards;
        जइ (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

शून्य SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_मानम् = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    जइ(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } अण्णहा {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        जाव((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            जइ(USTRINGTRIE_HAS_मानम्(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_मानम् = iter.get_मानम्();
            }
            जइ(!USTRINGTRIE_HAS_NEXT(r)) {
                भङ्ग;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    जइ(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      जइ(best_मानम् == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        प्रत्यागम kExceptionHere; // See if the next is another exception.
      } अण्णहा जइ(best_मानम् == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_मानम्;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        जाव((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        जइ(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            प्रत्यागम kExceptionHere;
        } अण्णहा {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          प्रत्यागम kNoExceptionHere;
        }
      } अण्णहा {
        प्रत्यागम kNoExceptionHere; // internal error and/or no forwards trie
      }
    } अण्णहा {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      प्रत्यागम kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  जइ(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      प्रत्यागम n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  जइ(U_FAILURE(status)) प्रत्यागम UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  जाव (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    विकल्प(m) {
    पक्ष kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      अनुवर्त;

    स्वभाव:
    पक्ष kNoExceptionHere:
      प्रत्यागम n;
    }    
  }
  प्रत्यागम n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  जइ(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      प्रत्यागम n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  जइ(U_FAILURE(status)) प्रत्यागम UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  जाव (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    विकल्प(m) {
    पक्ष kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      अनुवर्त;

    स्वभाव:
    पक्ष kNoExceptionHere:
      प्रत्यागम n;
    }    
  }
  प्रत्यागम n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  प्रत्यागम internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  प्रत्यागम fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  प्रत्यागम internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  प्रत्यागम internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  जइ (!fDelegate->isBoundary(offset)) प्रत्यागम मिच्छ; // no break to suppress

  जइ (!fData->hasBackwardsTrie()) प्रत्यागम सच्च; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  विकल्प(m) {
  पक्ष kExceptionHere:
    प्रत्यागम मिच्छ;
  स्वभाव:
  पक्ष kNoExceptionHere:
    प्रत्यागम सच्च;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  प्रत्यागम internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  प्रत्यागम internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  प्रत्यागम fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
वर्ग SimpleFilteredBreakIteratorBuilder : सार्वजनिक FilteredBreakIteratorBuilder {
सार्वजनिक:
  आभास ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ध्रुव Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  आभास UBool suppressBreakAfter(ध्रुव UnicodeString& exception, UErrorCode& status) override;
  आभास UBool unsuppressBreakAfter(ध्रुव UnicodeString& exception, UErrorCode& status) override;
  आभास BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
गुप्त:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ध्रुव Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  जइ(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    जइ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#जइ FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      प्रत्यागम;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_कुञ्चिका_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    जइ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#जइ FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      प्रत्यागम;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_कुञ्चिका_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#जइ FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    जइ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#जइ FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      प्रत्यागम;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    करेहि {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      जइ(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } जाव (strs.isValid() && U_SUCCESS(subStatus));
    जइ(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ध्रुव UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  प्रत्यागम r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ध्रुव UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  प्रत्यागम r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
स्थिर अन्तःस्थ UnicodeString* newUnicodeStringArray(size_t count) {
    प्रत्यागम नूतन UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(नूतन UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(नूतन UCharsTrieBuilder(status), status);
  जइ(U_FAILURE(status)) {
    प्रत्यागम nullptr;
  }

  int32_t rev_गणनम् = 0;
  int32_t fwd_गणनम् = 0;

  int32_t sub_गणनम् = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_गणनम्);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<पूर्णाङ्क> partials;
  partials.allocateInsteadAndReset(sub_गणनम्);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  पूर्णाङ्क n=0;
  प्रति ( int32_t i = 0;
        i<fSet.size();
        i++) {
    ध्रुव UnicodeString *abbr = fSet.getStringAt(i);
    जइ(abbr) {
      FB_TRACE("build",abbr,सच्च,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],सच्च,i);
    } अण्णहा {
      FB_TRACE("build",abbr,मिच्छ,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      प्रत्यागम nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  प्रति(पूर्णाङ्क i=0;i<sub_गणनम्;i++) {
    पूर्णाङ्क nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    जइ(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],मिच्छ,i);
      // is partial.
      // is it unique?
      पूर्णाङ्क sameAs = -1;
      प्रति(पूर्णाङ्क j=0;j<sub_गणनम्;j++) {
        जइ(j==i) अनुवर्त;
        जइ(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],मिच्छ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          जइ(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],मिच्छ,j);
          } अण्णहा जइ(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],मिच्छ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],मिच्छ,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      जइ(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_गणनम्++;
        FB_TRACE("Added partial",&prefix,मिच्छ, i);
        FB_TRACE(u_errorName(status),&ustrs[i],मिच्छ,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } अण्णहा {
        FB_TRACE("NOT adding partial",&prefix,मिच्छ, i);
        FB_TRACE(u_errorName(status),&ustrs[i],मिच्छ,i);
      }
    }
  }
  प्रति(पूर्णाङ्क i=0;i<sub_गणनम्;i++) {
    जइ(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_गणनम्++;
      FB_TRACE(u_errorName(status), &ustrs[i], मिच्छ, i);
    } अण्णहा {
      FB_TRACE("Adding fwd",&ustrs[i], मिच्छ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_गणनम्++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,मिच्छ, sub_गणनम्);

  जइ(rev_गणनम्>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    जइ(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,मिच्छ, -1);
      प्रत्यागम nullptr;
    }
  }

  जइ(fwd_गणनम्>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    जइ(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,मिच्छ, -1);
      प्रत्यागम nullptr;
    }
  }

  प्रत्यागम नूतन SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ध्रुव Locale& where, UErrorCode& status) {
  जइ(U_FAILURE(status)) प्रत्यागम nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नूतन SimpleFilteredBreakIteratorBuilder(where, status), status);
  प्रत्यागम (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  प्रत्यागम createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  जइ(U_FAILURE(status)) प्रत्यागम nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नूतन SimpleFilteredBreakIteratorBuilder(status), status);
  प्रत्यागम (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

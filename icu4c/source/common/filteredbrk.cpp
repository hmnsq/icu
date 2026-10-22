// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"
#जर !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

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

#जर FB_DEBUG
#अन्तर्भाव <stdio.h>
स्थिर शून्य _fb_trace(स्थिरांक अक्षर *m, स्थिरांक UnicodeString *s, UBool b, int32_t d, स्थिरांक अक्षर *f, पूर्णांक l) {
  अक्षर buf[2048];
  जर(s) {
    s->extract(0,s->length(),buf,2048);
  } अन्यथा {
    म_नकल(buf,"nullptr");
  }
  ख_लिखो(मानक_त्रुटि,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (स्थिरांक शून्य*)s, b?'T':'F',(पूर्णांक)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#अन्यथा
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
स्थिर int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    स्थिरांक UnicodeString& a = *static_cast<स्थिरांक UnicodeString*>(t1.pointer);
    स्थिरांक UnicodeString& b = *static_cast<स्थिरांक UnicodeString*>(t2.pointer);
    परत a.compare(b);
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
  आभासी ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  ओळीत UBool contains(स्थिरांक UnicodeString& s) {
    परत contains((शून्य*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  ओळीत स्थिरांक UnicodeString* getStringAt(int32_t i) स्थिरांक U_LIFETIME_BOUND {
    परत static_cast<स्थिरांक UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  ओळीत UBool adopt(UnicodeString *str, UErrorCode &status) {
    जर(U_FAILURE(status) || contains(*str)) {
      delete str;
      परत असत्य;
    } अन्यथा {
      sorted_घाला(str, compareUnicodeString, status);
      जर(U_FAILURE(status)) {
        परत असत्य;
      }
      परत सत्य;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  ओळीत UBool add(स्थिरांक UnicodeString& str, UErrorCode &status) {
    जर(U_FAILURE(status)) परत असत्य;
    UnicodeString *t = नवीन UnicodeString(str);
    जर(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; परत असत्य;
    }
    परत adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  ओळीत UBool हटाओ(स्थिरांक UnicodeString &s, UErrorCode &status) {
    जर(U_FAILURE(status)) परत असत्य;
    परत removeElement((शून्य*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
स्थिर स्थिरांक int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
स्थिर स्थिरांक int32_t kMATCH   = (1<<1); //< exact match - skip this one.
स्थिर स्थिरांक int32_t kSuppressInReverse = (1<<0);
स्थिर स्थिरांक int32_t kAddToForward = (1<<1);
स्थिर स्थिरांक char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
वर्ग SimpleFilteredSentenceBreakData : सार्वजनिक UMemory {
सार्वजनिक:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        परत हे;
    }
    SimpleFilteredSentenceBreakData *decr() {
        जर(umtx_atomic_dec(&refcount) <= 0) {
            delete हे;
        }
        परत nullptr;
    }
    आभासी ~SimpleFilteredSentenceBreakData();

    तार्किक hasForwardsPartialTrie() स्थिरांक { परत fForwardsPartialTrie.isValid(); }
    तार्किक hasBackwardsTrie() स्थिरांक { परत fBackwardsTrie.isValid(); }

    स्थिरांक UCharsTrie& getForwardsPartialTrie() स्थिरांक U_LIFETIME_BOUND {
        परत *fForwardsPartialTrie;
    }
    स्थिरांक UCharsTrie& getBackwardsTrie() स्थिरांक U_LIFETIME_BOUND { परत *fBackwardsTrie; }

खाजगी:
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
  SimpleFilteredSentenceBreakIterator(स्थिरांक SimpleFilteredSentenceBreakIterator& other);
  आभासी ~SimpleFilteredSentenceBreakIterator();
खाजगी:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
सार्वजनिक:
  /* -- cloning and other subclass stuff -- */
  आभासी BreakIterator *  createBufferClone(शून्य * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    परत clone();
  }
  आभासी SimpleFilteredSentenceBreakIterator* clone() स्थिरांक override { परत नवीन SimpleFilteredSentenceBreakIterator(*हे); }
  आभासी UClassID getDynamicClassID() स्थिरांक override { परत nullptr; }
  आभासी तार्किक चालक==(स्थिरांक BreakIterator& o) स्थिरांक override { जर(हे==&o) परत सत्य; परत असत्य; }

  /* -- text modifying -- */
  आभासी शून्य setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  आभासी BreakIterator& refresh_आदान_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_आदान_Text(input, status);
      परत *हे;
  }
  आभासी शून्य adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  आभासी शून्य setText(स्थिरांक UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  आभासी UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) स्थिरांक override {
      परत fDelegate->getUText(fillIn, status);
  }
  आभासी CharacterIterator& getText() स्थिरांक U_LIFETIME_BOUND override {
      परत fDelegate->getText();
  }

  /* -- ITERATION -- */
  आभासी int32_t first() override;
  आभासी int32_t preceding(int32_t offset) override;
  आभासी int32_t previous() override;
  आभासी UBool isBoundary(int32_t offset) override;
  आभासी int32_t current() स्थिरांक override { परत fDelegate->current(); } // we keep the delegate current, so this should be correct.

  आभासी int32_t next() override;

  आभासी int32_t next(int32_t n) override;
  आभासी int32_t following(int32_t offset) override;
  आभासी int32_t last() override;

खाजगी:
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

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(स्थिरांक SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(नवीन SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    जर (fData == nullptr) {
        delete forwards;
        delete backwards;
        जर (U_SUCCESS(status)) {
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
    int32_t best_मूल्य = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    जर(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } अन्यथा {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        पर्यंत((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            जर(USTRINGTRIE_HAS_मूल्य(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_मूल्य = iter.get_मूल्य();
            }
            जर(!USTRINGTRIE_HAS_NEXT(r)) {
                थांबा;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    जर(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      जर(best_मूल्य == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        परत kExceptionHere; // See if the next is another exception.
      } अन्यथा जर(best_मूल्य == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_मूल्य;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        पर्यंत((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        जर(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            परत kExceptionHere;
        } अन्यथा {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          परत kNoExceptionHere;
        }
      } अन्यथा {
        परत kNoExceptionHere; // internal error and/or no forwards trie
      }
    } अन्यथा {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      परत kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  जर(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      परत n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  जर(U_FAILURE(status)) परत UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  पर्यंत (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    निवड(m) {
    स्थिती kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      सुरूठेवा;

    पूर्वनिर्धारित:
    स्थिती kNoExceptionHere:
      परत n;
    }    
  }
  परत n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  जर(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      परत n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  जर(U_FAILURE(status)) परत UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  पर्यंत (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    निवड(m) {
    स्थिती kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      सुरूठेवा;

    पूर्वनिर्धारित:
    स्थिती kNoExceptionHere:
      परत n;
    }    
  }
  परत n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  परत internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  परत fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  परत internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  परत internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  जर (!fDelegate->isBoundary(offset)) परत असत्य; // no break to suppress

  जर (!fData->hasBackwardsTrie()) परत सत्य; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  निवड(m) {
  स्थिती kExceptionHere:
    परत असत्य;
  पूर्वनिर्धारित:
  स्थिती kNoExceptionHere:
    परत सत्य;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  परत internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  परत internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  परत fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
वर्ग SimpleFilteredBreakIteratorBuilder : सार्वजनिक FilteredBreakIteratorBuilder {
सार्वजनिक:
  आभासी ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(स्थिरांक Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  आभासी UBool suppressBreakAfter(स्थिरांक UnicodeString& exception, UErrorCode& status) override;
  आभासी UBool unsuppressBreakAfter(स्थिरांक UnicodeString& exception, UErrorCode& status) override;
  आभासी BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
खाजगी:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(स्थिरांक Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  जर(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    जर (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#जर FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      परत;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_किल्ली_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    जर (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#जर FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      परत;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_किल्ली_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#जर FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      ख_लिखो(मानक_त्रुटि, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    जर (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#जर FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      परत;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    करा {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      जर(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } पर्यंत (strs.isValid() && U_SUCCESS(subStatus));
    जर(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(स्थिरांक UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  परत r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(स्थिरांक UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.हटाओ(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  परत r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
स्थिर ओळीत UnicodeString* newUnicodeStringArray(माप_प्रकार count) {
    परत नवीन UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(नवीन UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(नवीन UCharsTrieBuilder(status), status);
  जर(U_FAILURE(status)) {
    परत nullptr;
  }

  int32_t rev_मोजणी = 0;
  int32_t fwd_मोजणी = 0;

  int32_t sub_मोजणी = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_मोजणी);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<पूर्णांक> partials;
  partials.allocateInsteadAndReset(sub_मोजणी);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  पूर्णांक n=0;
  साठी ( int32_t i = 0;
        i<fSet.size();
        i++) {
    स्थिरांक UnicodeString *abbr = fSet.getStringAt(i);
    जर(abbr) {
      FB_TRACE("build",abbr,सत्य,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],सत्य,i);
    } अन्यथा {
      FB_TRACE("build",abbr,असत्य,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      परत nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  साठी(पूर्णांक i=0;i<sub_मोजणी;i++) {
    पूर्णांक nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    जर(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],असत्य,i);
      // is partial.
      // is it unique?
      पूर्णांक sameAs = -1;
      साठी(पूर्णांक j=0;j<sub_मोजणी;j++) {
        जर(j==i) सुरूठेवा;
        जर(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],असत्य,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          जर(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],असत्य,j);
          } अन्यथा जर(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],असत्य,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],असत्य,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      जर(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_मोजणी++;
        FB_TRACE("Added partial",&prefix,असत्य, i);
        FB_TRACE(u_errorName(status),&ustrs[i],असत्य,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } अन्यथा {
        FB_TRACE("NOT adding partial",&prefix,असत्य, i);
        FB_TRACE(u_errorName(status),&ustrs[i],असत्य,i);
      }
    }
  }
  साठी(पूर्णांक i=0;i<sub_मोजणी;i++) {
    जर(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_मोजणी++;
      FB_TRACE(u_errorName(status), &ustrs[i], असत्य, i);
    } अन्यथा {
      FB_TRACE("Adding fwd",&ustrs[i], असत्य, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_मोजणी++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,असत्य, sub_मोजणी);

  जर(rev_मोजणी>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    जर(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,असत्य, -1);
      परत nullptr;
    }
  }

  जर(fwd_मोजणी>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    जर(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,असत्य, -1);
      परत nullptr;
    }
  }

  परत नवीन SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(स्थिरांक Locale& where, UErrorCode& status) {
  जर(U_FAILURE(status)) परत nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नवीन SimpleFilteredBreakIteratorBuilder(where, status), status);
  परत (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  परत createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  जर(U_FAILURE(status)) परत nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नवीन SimpleFilteredBreakIteratorBuilder(status), status);
  परत (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

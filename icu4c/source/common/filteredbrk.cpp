// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#অন্তর্ভাব "unicode/utypes.h"
#যদি !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#অন্তর্ভাব "cmemory.h"

#অন্তর্ভাব "unicode/filteredbrk.h"
#অন্তর্ভাব "unicode/ucharstriebuilder.h"
#অন্তর্ভাব "unicode/ures.h"

#অন্তর্ভাব "uresimp.h" // ures_getByKeyWithFallback
#অন্তর্ভাব "ubrkimpl.h" // U_ICUDATA_BRKITR
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#যদি FB_DEBUG
#অন্তর্ভাব <stdio.h>
স্থির শূন্য _fb_trace(ধ্রুব বর্ণ *m, ধ্রুব UnicodeString *s, UBool b, int32_t d, ধ্রুব বর্ণ *f, পূর্ণাঙ্ক l) {
  বর্ণ buf[2048];
  যদি(s) {
    s->extract(0,s->length(),buf,2048);
  } অন্যথা {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ধ্রুব শূন্য*)s, b?'T':'F',(পূর্ণাঙ্ক)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#অন্যথা
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
স্থির int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ধ্রুব UnicodeString& a = *static_cast<ধ্রুব UnicodeString*>(t1.pointer);
    ধ্রুব UnicodeString& b = *static_cast<ধ্রুব UnicodeString*>(t2.pointer);
    প্রত্যাগম a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
বর্গ UStringSet : সার্বজনিক UVector {
 সার্বজনিক:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  আভাস ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  অন্তঃস্থ UBool contains(ধ্রুব UnicodeString& s) {
    প্রত্যাগম contains((শূন্য*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  অন্তঃস্থ ধ্রুব UnicodeString* getStringAt(int32_t i) ধ্রুব U_LIFETIME_BOUND {
    প্রত্যাগম static_cast<ধ্রুব UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  অন্তঃস্থ UBool adopt(UnicodeString *str, UErrorCode &status) {
    যদি(U_FAILURE(status) || contains(*str)) {
      delete str;
      প্রত্যাগম অসত্য;
    } অন্যথা {
      sorted_ঢোকাও(str, compareUnicodeString, status);
      যদি(U_FAILURE(status)) {
        প্রত্যাগম অসত্য;
      }
      প্রত্যাগম সত্য;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  অন্তঃস্থ UBool add(ধ্রুব UnicodeString& str, UErrorCode &status) {
    যদি(U_FAILURE(status)) প্রত্যাগম অসত্য;
    UnicodeString *t = নূতন UnicodeString(str);
    যদি(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; প্রত্যাগম অসত্য;
    }
    প্রত্যাগম adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  অন্তঃস্থ UBool remove(ধ্রুব UnicodeString &s, UErrorCode &status) {
    যদি(U_FAILURE(status)) প্রত্যাগম অসত্য;
    প্রত্যাগম removeElement((শূন্য*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
স্থির ধ্রুব int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
স্থির ধ্রুব int32_t kMATCH   = (1<<1); //< exact match - skip this one.
স্থির ধ্রুব int32_t kSuppressInReverse = (1<<0);
স্থির ধ্রুব int32_t kAddToForward = (1<<1);
স্থির ধ্রুব char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
বর্গ SimpleFilteredSentenceBreakData : সার্বজনিক UMemory {
সার্বজনিক:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        প্রত্যাগম এতত্;
    }
    SimpleFilteredSentenceBreakData *decr() {
        যদি(umtx_atomic_dec(&refcount) <= 0) {
            delete এতত্;
        }
        প্রত্যাগম nullptr;
    }
    আভাস ~SimpleFilteredSentenceBreakData();

    তার্কিক hasForwardsPartialTrie() ধ্রুব { প্রত্যাগম fForwardsPartialTrie.isValid(); }
    তার্কিক hasBackwardsTrie() ধ্রুব { প্রত্যাগম fBackwardsTrie.isValid(); }

    ধ্রুব UCharsTrie& getForwardsPartialTrie() ধ্রুব U_LIFETIME_BOUND {
        প্রত্যাগম *fForwardsPartialTrie;
    }
    ধ্রুব UCharsTrie& getBackwardsTrie() ধ্রুব U_LIFETIME_BOUND { প্রত্যাগম *fBackwardsTrie; }

গুপ্ত:
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
বর্গ SimpleFilteredSentenceBreakIterator : সার্বজনিক BreakIterator {
সার্বজনিক:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(ধ্রুব SimpleFilteredSentenceBreakIterator& other);
  আভাস ~SimpleFilteredSentenceBreakIterator();
গুপ্ত:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
সার্বজনিক:
  /* -- cloning and other subclass stuff -- */
  আভাস BreakIterator *  createBufferClone(শূন্য * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    প্রত্যাগম clone();
  }
  আভাস SimpleFilteredSentenceBreakIterator* clone() ধ্রুব override { প্রত্যাগম নূতন SimpleFilteredSentenceBreakIterator(*এতত্); }
  আভাস UClassID getDynamicClassID() ধ্রুব override { প্রত্যাগম nullptr; }
  আভাস তার্কিক operator==(ধ্রুব BreakIterator& o) ধ্রুব override { যদি(এতত্==&o) প্রত্যাগম সত্য; প্রত্যাগম অসত্য; }

  /* -- text modifying -- */
  আভাস শূন্য setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  আভাস BreakIterator& refresh_প্রবেশ_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_প্রবেশ_Text(input, status);
      প্রত্যাগম *এতত্;
  }
  আভাস শূন্য adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  আভাস শূন্য setText(ধ্রুব UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  আভাস UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ধ্রুব override {
      প্রত্যাগম fDelegate->getUText(fillIn, status);
  }
  আভাস CharacterIterator& getText() ধ্রুব U_LIFETIME_BOUND override {
      প্রত্যাগম fDelegate->getText();
  }

  /* -- ITERATION -- */
  আভাস int32_t first() override;
  আভাস int32_t preceding(int32_t offset) override;
  আভাস int32_t previous() override;
  আভাস UBool isBoundary(int32_t offset) override;
  আভাস int32_t current() ধ্রুব override { প্রত্যাগম fDelegate->current(); } // we keep the delegate current, so this should be correct.

  আভাস int32_t next() override;

  আভাস int32_t next(int32_t n) override;
  আভাস int32_t following(int32_t offset) override;
  আভাস int32_t last() override;

গুপ্ত:
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
    শূন্য resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    গণনা EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    গণনা EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ধ্রুব SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(নূতন SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    যদি (fData == nullptr) {
        delete forwards;
        delete backwards;
        যদি (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

শূন্য SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_মান = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    যদি(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } অন্যথা {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        যাবত্((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            যদি(USTRINGTRIE_HAS_মান(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_মান = iter.get_মান();
            }
            যদি(!USTRINGTRIE_HAS_NEXT(r)) {
                ভঙ্গ;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    যদি(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      যদি(best_মান == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        প্রত্যাগম kExceptionHere; // See if the next is another exception.
      } অন্যথা যদি(best_মান == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_মান;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        যাবত্((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        যদি(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            প্রত্যাগম kExceptionHere;
        } অন্যথা {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          প্রত্যাগম kNoExceptionHere;
        }
      } অন্যথা {
        প্রত্যাগম kNoExceptionHere; // internal error and/or no forwards trie
      }
    } অন্যথা {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      প্রত্যাগম kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  যদি(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      প্রত্যাগম n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  যদি(U_FAILURE(status)) প্রত্যাগম UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  যাবত্ (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    বিকল্প(m) {
    পক্ষ kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      অনুবর্ত;

    স্বভাব:
    পক্ষ kNoExceptionHere:
      প্রত্যাগম n;
    }    
  }
  প্রত্যাগম n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  যদি(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      প্রত্যাগম n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  যদি(U_FAILURE(status)) প্রত্যাগম UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  যাবত্ (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    বিকল্প(m) {
    পক্ষ kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      অনুবর্ত;

    স্বভাব:
    পক্ষ kNoExceptionHere:
      প্রত্যাগম n;
    }    
  }
  প্রত্যাগম n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  প্রত্যাগম internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  প্রত্যাগম fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  প্রত্যাগম internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  প্রত্যাগম internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  যদি (!fDelegate->isBoundary(offset)) প্রত্যাগম অসত্য; // no break to suppress

  যদি (!fData->hasBackwardsTrie()) প্রত্যাগম সত্য; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  বিকল্প(m) {
  পক্ষ kExceptionHere:
    প্রত্যাগম অসত্য;
  স্বভাব:
  পক্ষ kNoExceptionHere:
    প্রত্যাগম সত্য;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  প্রত্যাগম internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  প্রত্যাগম internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  প্রত্যাগম fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
বর্গ SimpleFilteredBreakIteratorBuilder : সার্বজনিক FilteredBreakIteratorBuilder {
সার্বজনিক:
  আভাস ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ধ্রুব Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  আভাস UBool suppressBreakAfter(ধ্রুব UnicodeString& exception, UErrorCode& status) override;
  আভাস UBool unsuppressBreakAfter(ধ্রুব UnicodeString& exception, UErrorCode& status) override;
  আভাস BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
গুপ্ত:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ধ্রুব Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  যদি(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    যদি (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#যদি FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      প্রত্যাগম;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_চাবি_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    যদি (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#যদি FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      প্রত্যাগম;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_চাবি_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#যদি FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    যদি (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#যদি FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      প্রত্যাগম;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    কুরু {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      যদি(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } যাবত্ (strs.isValid() && U_SUCCESS(subStatus));
    যদি(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ধ্রুব UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  প্রত্যাগম r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ধ্রুব UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  প্রত্যাগম r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
স্থির অন্তঃস্থ UnicodeString* newUnicodeStringArray(size_t count) {
    প্রত্যাগম নূতন UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(নূতন UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(নূতন UCharsTrieBuilder(status), status);
  যদি(U_FAILURE(status)) {
    প্রত্যাগম nullptr;
  }

  int32_t rev_গুনতি = 0;
  int32_t fwd_গুনতি = 0;

  int32_t sub_গুনতি = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_গুনতি);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<পূর্ণাঙ্ক> partials;
  partials.allocateInsteadAndReset(sub_গুনতি);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  পূর্ণাঙ্ক n=0;
  প্রতি ( int32_t i = 0;
        i<fSet.size();
        i++) {
    ধ্রুব UnicodeString *abbr = fSet.getStringAt(i);
    যদি(abbr) {
      FB_TRACE("build",abbr,সত্য,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],সত্য,i);
    } অন্যথা {
      FB_TRACE("build",abbr,অসত্য,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      প্রত্যাগম nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  প্রতি(পূর্ণাঙ্ক i=0;i<sub_গুনতি;i++) {
    পূর্ণাঙ্ক nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    যদি(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],অসত্য,i);
      // is partial.
      // is it unique?
      পূর্ণাঙ্ক sameAs = -1;
      প্রতি(পূর্ণাঙ্ক j=0;j<sub_গুনতি;j++) {
        যদি(j==i) অনুবর্ত;
        যদি(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],অসত্য,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          যদি(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],অসত্য,j);
          } অন্যথা যদি(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],অসত্য,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],অসত্য,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      যদি(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_গুনতি++;
        FB_TRACE("Added partial",&prefix,অসত্য, i);
        FB_TRACE(u_errorName(status),&ustrs[i],অসত্য,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } অন্যথা {
        FB_TRACE("NOT adding partial",&prefix,অসত্য, i);
        FB_TRACE(u_errorName(status),&ustrs[i],অসত্য,i);
      }
    }
  }
  প্রতি(পূর্ণাঙ্ক i=0;i<sub_গুনতি;i++) {
    যদি(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_গুনতি++;
      FB_TRACE(u_errorName(status), &ustrs[i], অসত্য, i);
    } অন্যথা {
      FB_TRACE("Adding fwd",&ustrs[i], অসত্য, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_গুনতি++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,অসত্য, sub_গুনতি);

  যদি(rev_গুনতি>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    যদি(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,অসত্য, -1);
      প্রত্যাগম nullptr;
    }
  }

  যদি(fwd_গুনতি>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    যদি(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,অসত্য, -1);
      প্রত্যাগম nullptr;
    }
  }

  প্রত্যাগম নূতন SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ধ্রুব Locale& where, UErrorCode& status) {
  যদি(U_FAILURE(status)) প্রত্যাগম nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(নূতন SimpleFilteredBreakIteratorBuilder(where, status), status);
  প্রত্যাগম (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  প্রত্যাগম createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  যদি(U_FAILURE(status)) প্রত্যাগম nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(নূতন SimpleFilteredBreakIteratorBuilder(status), status);
  প্রত্যাগম (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

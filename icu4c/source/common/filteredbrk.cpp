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
স্থির শূন্য _fb_trace(ধ্রুবক অক্ষর *m, ধ্রুবক UnicodeString *s, UBool b, int32_t d, ধ্রুবক অক্ষর *f, পূর্ণসংখ্যা l) {
  অক্ষর buf[2048];
  যদি(s) {
    s->extract(0,s->length(),buf,2048);
  } অন্যথা {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ধ্রুবক শূন্য*)s, b?'T':'F',(পূর্ণসংখ্যা)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#অন্যথা
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
স্থির int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ধ্রুবক UnicodeString& a = *static_cast<ধ্রুবক UnicodeString*>(t1.pointer);
    ধ্রুবক UnicodeString& b = *static_cast<ধ্রুবক UnicodeString*>(t2.pointer);
    ফেরত a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
শ্রেণী UStringSet : সর্বজনীন UVector {
 সর্বজনীন:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  আভাসী ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  অন্তঃরেখ UBool contains(ধ্রুবক UnicodeString& s) {
    ফেরত contains((শূন্য*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  অন্তঃরেখ ধ্রুবক UnicodeString* getStringAt(int32_t i) ধ্রুবক U_LIFETIME_BOUND {
    ফেরত static_cast<ধ্রুবক UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  অন্তঃরেখ UBool adopt(UnicodeString *str, UErrorCode &status) {
    যদি(U_FAILURE(status) || contains(*str)) {
      delete str;
      ফেরত মিথ্যা;
    } অন্যথা {
      sorted_ঢোকাও(str, compareUnicodeString, status);
      যদি(U_FAILURE(status)) {
        ফেরত মিথ্যা;
      }
      ফেরত সত্য;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  অন্তঃরেখ UBool add(ধ্রুবক UnicodeString& str, UErrorCode &status) {
    যদি(U_FAILURE(status)) ফেরত মিথ্যা;
    UnicodeString *t = নতুন UnicodeString(str);
    যদি(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; ফেরত মিথ্যা;
    }
    ফেরত adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  অন্তঃরেখ UBool remove(ধ্রুবক UnicodeString &s, UErrorCode &status) {
    যদি(U_FAILURE(status)) ফেরত মিথ্যা;
    ফেরত removeElement((শূন্য*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
স্থির ধ্রুবক int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
স্থির ধ্রুবক int32_t kMATCH   = (1<<1); //< exact match - skip this one.
স্থির ধ্রুবক int32_t kSuppressInReverse = (1<<0);
স্থির ধ্রুবক int32_t kAddToForward = (1<<1);
স্থির ধ্রুবক char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
শ্রেণী Simple_Filtered_Sentence_Break_তথ্য : সর্বজনীন UMemory {
সর্বজনীন:
  Simple_Filtered_Sentence_Break_তথ্য(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_তথ্য* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        ফেরত এই;
    }
    Simple_Filtered_Sentence_Break_তথ্য *decr() {
        যদি(umtx_atomic_dec(&refcount) <= 0) {
            delete এই;
        }
        ফেরত nullptr;
    }
    আভাসী ~Simple_Filtered_Sentence_Break_তথ্য();

    যৌক্তিক hasForwardsPartialTrie() ধ্রুবক { ফেরত fForwardsPartialTrie.isValid(); }
    যৌক্তিক hasBackwardsTrie() ধ্রুবক { ফেরত fBackwardsTrie.isValid(); }

    ধ্রুবক UCharsTrie& getForwardsPartialTrie() ধ্রুবক U_LIFETIME_BOUND {
        ফেরত *fForwardsPartialTrie;
    }
    ধ্রুবক UCharsTrie& getBackwardsTrie() ধ্রুবক U_LIFETIME_BOUND { ফেরত *fBackwardsTrie; }

ব্যক্তিগত:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    LocalPointer<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    LocalPointer<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_তথ্য::~Simple_Filtered_Sentence_Break_তথ্য() {}

/**
 * Concrete implementation
 */
শ্রেণী SimpleFilteredSentenceBreakIterator : সর্বজনীন BreakIterator {
সর্বজনীন:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(ধ্রুবক SimpleFilteredSentenceBreakIterator& other);
  আভাসী ~SimpleFilteredSentenceBreakIterator();
ব্যক্তিগত:
  Simple_Filtered_Sentence_Break_তথ্য *f_তথ্য;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
সর্বজনীন:
  /* -- cloning and other subclass stuff -- */
  আভাসী BreakIterator *  create_বাফার_Clone(শূন্য * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    ফেরত clone();
  }
  আভাসী SimpleFilteredSentenceBreakIterator* clone() ধ্রুবক override { ফেরত নতুন SimpleFilteredSentenceBreakIterator(*এই); }
  আভাসী UClassID getDynamicClassID() ধ্রুবক override { ফেরত nullptr; }
  আভাসী যৌক্তিক operator==(ধ্রুবক BreakIterator& o) ধ্রুবক override { যদি(এই==&o) ফেরত সত্য; ফেরত মিথ্যা; }

  /* -- text modifying -- */
  আভাসী শূন্য setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  আভাসী BreakIterator& refresh_প্রবেশ_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_প্রবেশ_Text(input, status);
      ফেরত *এই;
  }
  আভাসী শূন্য adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  আভাসী শূন্য setText(ধ্রুবক UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  আভাসী UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ধ্রুবক override {
      ফেরত fDelegate->getUText(fillIn, status);
  }
  আভাসী CharacterIterator& getText() ধ্রুবক U_LIFETIME_BOUND override {
      ফেরত fDelegate->getText();
  }

  /* -- ITERATION -- */
  আভাসী int32_t first() override;
  আভাসী int32_t preceding(int32_t offset) override;
  আভাসী int32_t previous() override;
  আভাসী UBool isBoundary(int32_t offset) override;
  আভাসী int32_t current() ধ্রুবক override { ফেরত fDelegate->current(); } // we keep the delegate current, so this should be correct.

  আভাসী int32_t next() override;

  আভাসী int32_t next(int32_t n) override;
  আভাসী int32_t following(int32_t offset) override;
  আভাসী int32_t last() override;

ব্যক্তিগত:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t internal_পরবর্তী(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t internal_পূর্ব(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    শূন্য resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    গণনা EFBMatch_ফলাফল { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    গণনা EFBMatch_ফলাফল breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ধ্রুবক SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_তথ্য(other.f_তথ্য->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  f_তথ্য(নতুন Simple_Filtered_Sentence_Break_তথ্য(forwards, backwards)),
  fDelegate(adopt)
{
    যদি (f_তথ্য == nullptr) {
        delete forwards;
        delete backwards;
        যদি (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_তথ্য = f_তথ্য->decr();
}

শূন্য SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_ফলাফল
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_মান = -1;
    // loops while 'n' points to an exception.
    utext_set_Native_সূচক(fText.getAlias(), n); // from n..

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
        UCharsTrie iter(f_তথ্য->getBackwardsTrie());
        UChar32 uch;
        যখন((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_ফলাফল r = iter.পরবর্তী_For_Code_Point(uch);
            যদি(USTRINGTRIE_HAS_মান(r)) { // remember the best match so far
                bestPosn = utext_get_Native_সূচক(fText.getAlias());
                best_মান = iter.get_মান();
            }
            যদি(!USTRINGTRIE_HAS_পরবর্তী(r)) {
                বিরতি;
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
        ফেরত kExceptionHere; // See if the next is another exception.
      } অন্যথা যদি(best_মান == kPARTIAL
                && f_তথ্য->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_ফলাফল rfwd = USTRINGTRIE_INTERMEDIATE_মান;
        utext_set_Native_সূচক(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_তথ্য->getForwardsPartialTrie());
        UChar32 uch;
        যখন((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_পরবর্তী(rfwd=iter.পরবর্তী_For_Code_Point(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        যদি(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            ফেরত kExceptionHere;
        } অন্যথা {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          ফেরত kNoExceptionHere;
        }
      } অন্যথা {
        ফেরত kNoExceptionHere; // internal error and/or no forwards trie
      }
    } অন্যথা {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      ফেরত kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internal_পরবর্তী(int32_t n) {
  যদি(n == UBRK_DONE || // at end  or
    !f_তথ্য->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ফেরত n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  যদি(U_FAILURE(status)) ফেরত UBRK_DONE; // bail out
  int64_t utextLen = utext_native_দৈর্ঘ্য(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  যখন (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_ফলাফল m = breakExceptionAt(n);

    নির্বাচন(m) {
    ক্ষেত্র kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      চালু;

    পূর্বনির্ধারিত:
    ক্ষেত্র kNoExceptionHere:
      ফেরত n;
    }    
  }
  ফেরত n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internal_পূর্ব(int32_t n) {
  যদি(n == 0 || n == UBRK_DONE || // at end  or
    !f_তথ্য->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ফেরত n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  যদি(U_FAILURE(status)) ফেরত UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  যখন (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_ফলাফল m = breakExceptionAt(n);

    নির্বাচন(m) {
    ক্ষেত্র kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      চালু;

    পূর্বনির্ধারিত:
    ক্ষেত্র kNoExceptionHere:
      ফেরত n;
    }    
  }
  ফেরত n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  ফেরত internal_পরবর্তী(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  ফেরত fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  ফেরত internal_পূর্ব(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  ফেরত internal_পূর্ব(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  যদি (!fDelegate->isBoundary(offset)) ফেরত মিথ্যা; // no break to suppress

  যদি (!f_তথ্য->hasBackwardsTrie()) ফেরত সত্য; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatch_ফলাফল m = breakExceptionAt(offset);

  নির্বাচন(m) {
  ক্ষেত্র kExceptionHere:
    ফেরত মিথ্যা;
  পূর্বনির্ধারিত:
  ক্ষেত্র kNoExceptionHere:
    ফেরত সত্য;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  ফেরত internal_পরবর্তী(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  ফেরত internal_পরবর্তী(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  ফেরত fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
শ্রেণী SimpleFilteredBreakIteratorBuilder : সর্বজনীন FilteredBreakIteratorBuilder {
সর্বজনীন:
  আভাসী ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ধ্রুবক Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  আভাসী UBool suppressBreakAfter(ধ্রুবক UnicodeString& exception, UErrorCode& status) override;
  আভাসী UBool unsuppressBreakAfter(ধ্রুবক UnicodeString& exception, UErrorCode& status) override;
  আভাসী BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
ব্যক্তিগত:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ধ্রুবক Locale &fromLocale, UErrorCode &status)
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
      ফেরত;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_চাবি_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    যদি (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#যদি FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ফেরত;  // leaves the builder empty, if you try to use it.
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
      ফেরত;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    করো {
      strs.adoptInstead(ures_get_পরবর্তী_Resource(breaks.getAlias(), strs.orphan(), &subStatus));
      যদি(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } যখন (strs.isValid() && U_SUCCESS(subStatus));
    যদি(U_FAILURE(subStatus)&&subStatus!=U_সূচক_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ধ্রুবক UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  ফেরত r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ধ্রুবক UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  ফেরত r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
স্থির অন্তঃরেখ UnicodeString* newUnicodeStringArray(size_t count) {
    ফেরত নতুন UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(নতুন UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(নতুন UCharsTrieBuilder(status), status);
  যদি(U_FAILURE(status)) {
    ফেরত nullptr;
  }

  int32_t rev_গুনতি = 0;
  int32_t fwd_গুনতি = 0;

  int32_t sub_গুনতি = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_গুনতি);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<পূর্ণসংখ্যা> partials;
  partials.allocateInsteadAndReset(sub_গুনতি);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  পূর্ণসংখ্যা n=0;
  জন্য ( int32_t i = 0;
        i<fSet.size();
        i++) {
    ধ্রুবক UnicodeString *abbr = fSet.getStringAt(i);
    যদি(abbr) {
      FB_TRACE("build",abbr,সত্য,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],সত্য,i);
    } অন্যথা {
      FB_TRACE("build",abbr,মিথ্যা,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      ফেরত nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  জন্য(পূর্ণসংখ্যা i=0;i<sub_গুনতি;i++) {
    পূর্ণসংখ্যা nn = ustrs[i].সূচক_Of(kFULLSTOP); // TODO: non-'.' abbreviations
    যদি(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],মিথ্যা,i);
      // is partial.
      // is it unique?
      পূর্ণসংখ্যা sameAs = -1;
      জন্য(পূর্ণসংখ্যা j=0;j<sub_গুনতি;j++) {
        যদি(j==i) চালু;
        যদি(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],মিথ্যা,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          যদি(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],মিথ্যা,j);
          } অন্যথা যদি(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],মিথ্যা,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],মিথ্যা,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      যদি(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_গুনতি++;
        FB_TRACE("Added partial",&prefix,মিথ্যা, i);
        FB_TRACE(u_errorName(status),&ustrs[i],মিথ্যা,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } অন্যথা {
        FB_TRACE("NOT adding partial",&prefix,মিথ্যা, i);
        FB_TRACE(u_errorName(status),&ustrs[i],মিথ্যা,i);
      }
    }
  }
  জন্য(পূর্ণসংখ্যা i=0;i<sub_গুনতি;i++) {
    যদি(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_গুনতি++;
      FB_TRACE(u_errorName(status), &ustrs[i], মিথ্যা, i);
    } অন্যথা {
      FB_TRACE("Adding fwd",&ustrs[i], মিথ্যা, i);

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
  FB_TRACE("AbbrCount",nullptr,মিথ্যা, sub_গুনতি);

  যদি(rev_গুনতি>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    যদি(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,মিথ্যা, -1);
      ফেরত nullptr;
    }
  }

  যদি(fwd_গুনতি>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    যদি(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,মিথ্যা, -1);
      ফেরত nullptr;
    }
  }

  ফেরত নতুন SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ধ্রুবক Locale& where, UErrorCode& status) {
  যদি(U_FAILURE(status)) ফেরত nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(নতুন SimpleFilteredBreakIteratorBuilder(where, status), status);
  ফেরত (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  ফেরত createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  যদি(U_FAILURE(status)) ফেরত nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(নতুন SimpleFilteredBreakIteratorBuilder(status), status);
  ফেরত (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

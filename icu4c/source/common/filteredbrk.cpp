// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#شامل "unicode/utypes.h"
#اگر !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#شامل "cmemory.h"

#شامل "unicode/filteredbrk.h"
#شامل "unicode/ucharstriebuilder.h"
#شامل "unicode/ures.h"

#شامل "uresimp.h" // ures_getByKeyWithFallback
#شامل "ubrkimpl.h" // U_ICUDATA_BRKITR
#شامل "uvector.h"
#شامل "cmemory.h"
#شامل "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#اگر FB_DEBUG
#شامل <stdio.h>
ایستا تهی _fb_trace(ثابت نویسه *m, ثابت UnicodeString *s, UBool b, int32_t d, ثابت نویسه *f, صحیح l) {
  نویسه buf[2048];
  اگر(s) {
    s->extract(0,s->length(),buf,2048);
  } وگرنه {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ثابت تهی*)s, b?'T':'F',(صحیح)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#وگرنه
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
ایستا int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.pointer);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.pointer);
    بازگشت a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
رده UStringSet : عمومی UVector {
 عمومی:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  مجازی ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  درونخطی UBool contains(ثابت UnicodeString& s) {
    بازگشت contains((تهی*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  درونخطی ثابت UnicodeString* getStringAt(int32_t i) ثابت U_LIFETIME_BOUND {
    بازگشت static_cast<ثابت UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  درونخطی UBool adopt(UnicodeString *str, UErrorCode &status) {
    اگر(U_FAILURE(status) || contains(*str)) {
      delete str;
      بازگشت نادرست;
    } وگرنه {
      sorted_درج(str, compareUnicodeString, status);
      اگر(U_FAILURE(status)) {
        بازگشت نادرست;
      }
      بازگشت درست;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  درونخطی UBool add(ثابت UnicodeString& str, UErrorCode &status) {
    اگر(U_FAILURE(status)) بازگشت نادرست;
    UnicodeString *t = جدید UnicodeString(str);
    اگر(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; بازگشت نادرست;
    }
    بازگشت adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  درونخطی UBool remove(ثابت UnicodeString &s, UErrorCode &status) {
    اگر(U_FAILURE(status)) بازگشت نادرست;
    بازگشت removeElement((تهی*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ایستا ثابت int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ایستا ثابت int32_t kMATCH   = (1<<1); //< exact match - skip this one.
ایستا ثابت int32_t kSuppressInReverse = (1<<0);
ایستا ثابت int32_t kAddToForward = (1<<1);
ایستا ثابت char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
رده SimpleFilteredSentenceBreakData : عمومی UMemory {
عمومی:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        بازگشت این;
    }
    SimpleFilteredSentenceBreakData *decr() {
        اگر(umtx_atomic_dec(&refcount) <= 0) {
            delete این;
        }
        بازگشت nullptr;
    }
    مجازی ~SimpleFilteredSentenceBreakData();

    منطقی hasForwardsPartialTrie() ثابت { بازگشت fForwardsPartialTrie.isValid(); }
    منطقی hasBackwardsTrie() ثابت { بازگشت fBackwardsTrie.isValid(); }

    ثابت UCharsTrie& getForwardsPartialTrie() ثابت U_LIFETIME_BOUND {
        بازگشت *fForwardsPartialTrie;
    }
    ثابت UCharsTrie& getBackwardsTrie() ثابت U_LIFETIME_BOUND { بازگشت *fBackwardsTrie; }

خصوصی:
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
رده SimpleFilteredSentenceBreakIterator : عمومی BreakIterator {
عمومی:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other);
  مجازی ~SimpleFilteredSentenceBreakIterator();
خصوصی:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
عمومی:
  /* -- cloning and other subclass stuff -- */
  مجازی BreakIterator *  createBufferClone(تهی * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    بازگشت clone();
  }
  مجازی SimpleFilteredSentenceBreakIterator* clone() ثابت override { بازگشت جدید SimpleFilteredSentenceBreakIterator(*این); }
  مجازی UClassID getDynamicClassID() ثابت override { بازگشت nullptr; }
  مجازی منطقی operator==(ثابت BreakIterator& o) ثابت override { اگر(این==&o) بازگشت درست; بازگشت نادرست; }

  /* -- text modifying -- */
  مجازی تهی setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  مجازی BreakIterator& refresh_درون_داد_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_درون_داد_Text(input, status);
      بازگشت *این;
  }
  مجازی تهی adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  مجازی تهی setText(ثابت UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  مجازی UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ثابت override {
      بازگشت fDelegate->getUText(fillIn, status);
  }
  مجازی CharacterIterator& getText() ثابت U_LIFETIME_BOUND override {
      بازگشت fDelegate->getText();
  }

  /* -- ITERATION -- */
  مجازی int32_t first() override;
  مجازی int32_t preceding(int32_t offset) override;
  مجازی int32_t previous() override;
  مجازی UBool isBoundary(int32_t offset) override;
  مجازی int32_t current() ثابت override { بازگشت fDelegate->current(); } // we keep the delegate current, so this should be correct.

  مجازی int32_t next() override;

  مجازی int32_t next(int32_t n) override;
  مجازی int32_t following(int32_t offset) override;
  مجازی int32_t last() override;

خصوصی:
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
    تهی resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    شمارش EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    شمارش EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(جدید SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    اگر (fData == nullptr) {
        delete forwards;
        delete backwards;
        اگر (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

تهی SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_مقدار = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    اگر(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } وگرنه {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        وقتی((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            اگر(USTRINGTRIE_HAS_مقدار(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_مقدار = iter.get_مقدار();
            }
            اگر(!USTRINGTRIE_HAS_NEXT(r)) {
                بشکن;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    اگر(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      اگر(best_مقدار == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        بازگشت kExceptionHere; // See if the next is another exception.
      } وگرنه اگر(best_مقدار == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_مقدار;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        وقتی((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        اگر(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            بازگشت kExceptionHere;
        } وگرنه {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          بازگشت kNoExceptionHere;
        }
      } وگرنه {
        بازگشت kNoExceptionHere; // internal error and/or no forwards trie
      }
    } وگرنه {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      بازگشت kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  اگر(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      بازگشت n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  اگر(U_FAILURE(status)) بازگشت UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  وقتی (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    انتخاب(m) {
    حالت kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      ادامه;

    پیشفرض:
    حالت kNoExceptionHere:
      بازگشت n;
    }    
  }
  بازگشت n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  اگر(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      بازگشت n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  اگر(U_FAILURE(status)) بازگشت UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  وقتی (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    انتخاب(m) {
    حالت kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      ادامه;

    پیشفرض:
    حالت kNoExceptionHere:
      بازگشت n;
    }    
  }
  بازگشت n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  بازگشت internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  بازگشت fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  بازگشت internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  بازگشت internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  اگر (!fDelegate->isBoundary(offset)) بازگشت نادرست; // no break to suppress

  اگر (!fData->hasBackwardsTrie()) بازگشت درست; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  انتخاب(m) {
  حالت kExceptionHere:
    بازگشت نادرست;
  پیشفرض:
  حالت kNoExceptionHere:
    بازگشت درست;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  بازگشت internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  بازگشت internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  بازگشت fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
رده SimpleFilteredBreakIteratorBuilder : عمومی FilteredBreakIteratorBuilder {
عمومی:
  مجازی ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  مجازی UBool suppressBreakAfter(ثابت UnicodeString& exception, UErrorCode& status) override;
  مجازی UBool unsuppressBreakAfter(ثابت UnicodeString& exception, UErrorCode& status) override;
  مجازی BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
خصوصی:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  اگر(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    اگر (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#اگر FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      بازگشت;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_کلید_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    اگر (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#اگر FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      بازگشت;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_کلید_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#اگر FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    اگر (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#اگر FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      بازگشت;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    انجامبده {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      اگر(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } وقتی (strs.isValid() && U_SUCCESS(subStatus));
    اگر(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ثابت UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  بازگشت r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ثابت UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  بازگشت r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
ایستا درونخطی UnicodeString* newUnicodeStringArray(size_t count) {
    بازگشت جدید UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(جدید UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(جدید UCharsTrieBuilder(status), status);
  اگر(U_FAILURE(status)) {
    بازگشت nullptr;
  }

  int32_t rev_شمار = 0;
  int32_t fwd_شمار = 0;

  int32_t sub_شمار = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_شمار);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<صحیح> partials;
  partials.allocateInsteadAndReset(sub_شمار);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  صحیح n=0;
  برای ( int32_t i = 0;
        i<fSet.size();
        i++) {
    ثابت UnicodeString *abbr = fSet.getStringAt(i);
    اگر(abbr) {
      FB_TRACE("build",abbr,درست,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],درست,i);
    } وگرنه {
      FB_TRACE("build",abbr,نادرست,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      بازگشت nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  برای(صحیح i=0;i<sub_شمار;i++) {
    صحیح nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    اگر(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],نادرست,i);
      // is partial.
      // is it unique?
      صحیح sameAs = -1;
      برای(صحیح j=0;j<sub_شمار;j++) {
        اگر(j==i) ادامه;
        اگر(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],نادرست,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          اگر(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],نادرست,j);
          } وگرنه اگر(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],نادرست,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],نادرست,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      اگر(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_شمار++;
        FB_TRACE("Added partial",&prefix,نادرست, i);
        FB_TRACE(u_errorName(status),&ustrs[i],نادرست,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } وگرنه {
        FB_TRACE("NOT adding partial",&prefix,نادرست, i);
        FB_TRACE(u_errorName(status),&ustrs[i],نادرست,i);
      }
    }
  }
  برای(صحیح i=0;i<sub_شمار;i++) {
    اگر(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_شمار++;
      FB_TRACE(u_errorName(status), &ustrs[i], نادرست, i);
    } وگرنه {
      FB_TRACE("Adding fwd",&ustrs[i], نادرست, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_شمار++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,نادرست, sub_شمار);

  اگر(rev_شمار>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    اگر(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,نادرست, -1);
      بازگشت nullptr;
    }
  }

  اگر(fwd_شمار>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    اگر(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,نادرست, -1);
      بازگشت nullptr;
    }
  }

  بازگشت جدید SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ثابت Locale& where, UErrorCode& status) {
  اگر(U_FAILURE(status)) بازگشت nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(جدید SimpleFilteredBreakIteratorBuilder(where, status), status);
  بازگشت (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  بازگشت createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  اگر(U_FAILURE(status)) بازگشت nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(جدید SimpleFilteredBreakIteratorBuilder(status), status);
  بازگشت (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"
#ܐܢ !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#ضمن "cmemory.h"

#ضمن "unicode/filteredbrk.h"
#ضمن "unicode/ucharstriebuilder.h"
#ضمن "unicode/ures.h"

#ضمن "uresimp.h" // ures_getByKeyWithFallback
#ضمن "ubrkimpl.h" // U_ICUDATA_BRKITR
#ضمن "uvector.h"
#ضمن "cmemory.h"
#ضمن "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#ܐܢ FB_DEBUG
#ضمن <stdio.h>
ساكن فراغ _fb_trace(ثابت حرف *m, ثابت UnicodeString *s, UBool b, int32_t d, ثابت حرف *f, صحيح l) {
  حرف buf[2048];
  ܐܢ(s) {
    s->extract(0,s->length(),buf,2048);
  } ܐܚܪܢܐ {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ثابت فراغ*)s, b?'T':'F',(صحيح)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#ܐܚܪܢܐ
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.pointer);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.pointer);
    ܦܢܐ a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
صنف UStringSet : عام UVector {
 عام:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  virtual ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  مضمن UBool contains(ثابت UnicodeString& s) {
    ܦܢܐ contains((فراغ*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  مضمن ثابت UnicodeString* getStringAt(int32_t i) ثابت U_LIFETIME_BOUND {
    ܦܢܐ static_cast<ثابت UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  مضمن UBool adopt(UnicodeString *str, UErrorCode &status) {
    ܐܢ(U_FAILURE(status) || contains(*str)) {
      delete str;
      ܦܢܐ ܕܓܠܐ;
    } ܐܚܪܢܐ {
      sortedInsert(str, compareUnicodeString, status);
      ܐܢ(U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
      }
      ܦܢܐ ܫܪܪܐ;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  مضمن UBool add(ثابت UnicodeString& str, UErrorCode &status) {
    ܐܢ(U_FAILURE(status)) ܦܢܐ ܕܓܠܐ;
    UnicodeString *t = جديد UnicodeString(str);
    ܐܢ(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; ܦܢܐ ܕܓܠܐ;
    }
    ܦܢܐ adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  مضمن UBool remove(ثابت UnicodeString &s, UErrorCode &status) {
    ܐܢ(U_FAILURE(status)) ܦܢܐ ܕܓܠܐ;
    ܦܢܐ removeElement((فراغ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ساكن ثابت int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ساكن ثابت int32_t kMATCH   = (1<<1); //< exact match - skip this one.
ساكن ثابت int32_t kSuppressInReverse = (1<<0);
ساكن ثابت int32_t kAddToForward = (1<<1);
ساكن ثابت char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
صنف SimpleFilteredSentenceBreakData : عام UMemory {
عام:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        ܦܢܐ هذا;
    }
    SimpleFilteredSentenceBreakData *decr() {
        ܐܢ(umtx_atomic_dec(&refcount) <= 0) {
            delete هذا;
        }
        ܦܢܐ nullptr;
    }
    virtual ~SimpleFilteredSentenceBreakData();

    منطقي hasForwardsPartialTrie() ثابت { ܦܢܐ fForwardsPartialTrie.isValid(); }
    منطقي hasBackwardsTrie() ثابت { ܦܢܐ fBackwardsTrie.isValid(); }

    ثابت UCharsTrie& getForwardsPartialTrie() ثابت U_LIFETIME_BOUND {
        ܦܢܐ *fForwardsPartialTrie;
    }
    ثابت UCharsTrie& getBackwardsTrie() ثابت U_LIFETIME_BOUND { ܦܢܐ *fBackwardsTrie; }

خاص:
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
صنف SimpleFilteredSentenceBreakIterator : عام BreakIterator {
عام:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other);
  virtual ~SimpleFilteredSentenceBreakIterator();
خاص:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
عام:
  /* -- cloning and other subclass stuff -- */
  virtual BreakIterator *  createBufferClone(فراغ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    ܦܢܐ clone();
  }
  virtual SimpleFilteredSentenceBreakIterator* clone() ثابت override { ܦܢܐ جديد SimpleFilteredSentenceBreakIterator(*هذا); }
  virtual UClassID getDynamicClassID() ثابت override { ܦܢܐ nullptr; }
  virtual منطقي operator==(ثابت BreakIterator& o) ثابت override { ܐܢ(هذا==&o) ܦܢܐ ܫܪܪܐ; ܦܢܐ ܕܓܠܐ; }

  /* -- text modifying -- */
  virtual فراغ setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  virtual BreakIterator& refreshInputText(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refreshInputText(input, status);
      ܦܢܐ *هذا;
  }
  virtual فراغ adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  virtual فراغ setText(ثابت UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  virtual UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) ثابت override {
      ܦܢܐ fDelegate->getUText(fillIn, status);
  }
  virtual CharacterIterator& getText() ثابت U_LIFETIME_BOUND override {
      ܦܢܐ fDelegate->getText();
  }

  /* -- ITERATION -- */
  virtual int32_t first() override;
  virtual int32_t preceding(int32_t offset) override;
  virtual int32_t previous() override;
  virtual UBool isBoundary(int32_t offset) override;
  virtual int32_t current() ثابت override { ܦܢܐ fDelegate->current(); } // we keep the delegate current, so this should be correct.

  virtual int32_t next() override;

  virtual int32_t next(int32_t n) override;
  virtual int32_t following(int32_t offset) override;
  virtual int32_t last() override;

خاص:
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
    فراغ resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    تعداد EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    تعداد EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(جديد SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    ܐܢ (fData == nullptr) {
        delete forwards;
        delete backwards;
        ܐܢ (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

فراغ SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t bestValue = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    ܐܢ(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } ܐܚܪܢܐ {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        ܟܡܐ((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            ܐܢ(USTRINGTRIE_HAS_VALUE(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                bestValue = iter.getValue();
            }
            ܐܢ(!USTRINGTRIE_HAS_NEXT(r)) {
                اكسر;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    ܐܢ(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      ܐܢ(bestValue == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        ܦܢܐ kExceptionHere; // See if the next is another exception.
      } ܐܚܪܢܐ ܐܢ(bestValue == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_VALUE;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        ܟܡܐ((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        ܐܢ(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            ܦܢܐ kExceptionHere;
        } ܐܚܪܢܐ {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          ܦܢܐ kNoExceptionHere;
        }
      } ܐܚܪܢܐ {
        ܦܢܐ kNoExceptionHere; // internal error and/or no forwards trie
      }
    } ܐܚܪܢܐ {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      ܦܢܐ kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  ܐܢ(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ܦܢܐ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  ܐܢ(U_FAILURE(status)) ܦܢܐ UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ܟܡܐ (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    بدل(m) {
    حالة kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة kNoExceptionHere:
      ܦܢܐ n;
    }    
  }
  ܦܢܐ n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  ܐܢ(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ܦܢܐ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  ܐܢ(U_FAILURE(status)) ܦܢܐ UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ܟܡܐ (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    بدل(m) {
    حالة kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة kNoExceptionHere:
      ܦܢܐ n;
    }    
  }
  ܦܢܐ n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  ܦܢܐ internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  ܦܢܐ fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  ܦܢܐ internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  ܦܢܐ internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  ܐܢ (!fDelegate->isBoundary(offset)) ܦܢܐ ܕܓܠܐ; // no break to suppress

  ܐܢ (!fData->hasBackwardsTrie()) ܦܢܐ ܫܪܪܐ; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  بدل(m) {
  حالة kExceptionHere:
    ܦܢܐ ܕܓܠܐ;
  افتراضي:
  حالة kNoExceptionHere:
    ܦܢܐ ܫܪܪܐ;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  ܦܢܐ internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  ܦܢܐ internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  ܦܢܐ fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
صنف SimpleFilteredBreakIteratorBuilder : عام FilteredBreakIteratorBuilder {
عام:
  virtual ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  virtual UBool suppressBreakAfter(ثابت UnicodeString& exception, UErrorCode& status) override;
  virtual UBool unsuppressBreakAfter(ثابت UnicodeString& exception, UErrorCode& status) override;
  virtual BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
خاص:
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
  ܐܢ(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    ܐܢ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ܐܢ FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ܦܢܐ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_getByKeyWithFallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    ܐܢ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ܐܢ FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ܦܢܐ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_getByKeyWithFallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#ܐܢ FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    ܐܢ (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ܐܢ FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ܦܢܐ;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    ܥܒܕ {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      ܐܢ(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } ܟܡܐ (strs.isValid() && U_SUCCESS(subStatus));
    ܐܢ(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ثابت UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  ܦܢܐ r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ثابت UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  ܦܢܐ r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
ساكن مضمن UnicodeString* newUnicodeStringArray(size_t count) {
    ܦܢܐ جديد UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(جديد UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(جديد UCharsTrieBuilder(status), status);
  ܐܢ(U_FAILURE(status)) {
    ܦܢܐ nullptr;
  }

  int32_t revCount = 0;
  int32_t fwdCount = 0;

  int32_t subCount = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(subCount);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<صحيح> partials;
  partials.allocateInsteadAndReset(subCount);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  صحيح n=0;
  لكل ( int32_t i = 0;
        i<fSet.size();
        i++) {
    ثابت UnicodeString *abbr = fSet.getStringAt(i);
    ܐܢ(abbr) {
      FB_TRACE("build",abbr,ܫܪܪܐ,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],ܫܪܪܐ,i);
    } ܐܚܪܢܐ {
      FB_TRACE("build",abbr,ܕܓܠܐ,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      ܦܢܐ nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  لكل(صحيح i=0;i<subCount;i++) {
    صحيح nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    ܐܢ(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],ܕܓܠܐ,i);
      // is partial.
      // is it unique?
      صحيح sameAs = -1;
      لكل(صحيح j=0;j<subCount;j++) {
        ܐܢ(j==i) استمر;
        ܐܢ(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],ܕܓܠܐ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          ܐܢ(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],ܕܓܠܐ,j);
          } ܐܚܪܢܐ ܐܢ(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],ܕܓܠܐ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],ܕܓܠܐ,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      ܐܢ(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        revCount++;
        FB_TRACE("Added partial",&prefix,ܕܓܠܐ, i);
        FB_TRACE(u_errorName(status),&ustrs[i],ܕܓܠܐ,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } ܐܚܪܢܐ {
        FB_TRACE("NOT adding partial",&prefix,ܕܓܠܐ, i);
        FB_TRACE(u_errorName(status),&ustrs[i],ܕܓܠܐ,i);
      }
    }
  }
  لكل(صحيح i=0;i<subCount;i++) {
    ܐܢ(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      revCount++;
      FB_TRACE(u_errorName(status), &ustrs[i], ܕܓܠܐ, i);
    } ܐܚܪܢܐ {
      FB_TRACE("Adding fwd",&ustrs[i], ܕܓܠܐ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwdCount++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,ܕܓܠܐ, subCount);

  ܐܢ(revCount>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    ܐܢ(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,ܕܓܠܐ, -1);
      ܦܢܐ nullptr;
    }
  }

  ܐܢ(fwdCount>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    ܐܢ(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,ܕܓܠܐ, -1);
      ܦܢܐ nullptr;
    }
  }

  ܦܢܐ جديد SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ثابت Locale& where, UErrorCode& status) {
  ܐܢ(U_FAILURE(status)) ܦܢܐ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(where, status), status);
  ܦܢܐ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  ܦܢܐ createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  ܐܢ(U_FAILURE(status)) ܦܢܐ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(status), status);
  ܦܢܐ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

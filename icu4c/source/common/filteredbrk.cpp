// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#அந்தர்பாவ "unicode/utypes.h"
#எனில் !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#அந்தர்பாவ "cmemory.h"

#அந்தர்பாவ "unicode/filteredbrk.h"
#அந்தர்பாவ "unicode/ucharstriebuilder.h"
#அந்தர்பாவ "unicode/ures.h"

#அந்தர்பாவ "uresimp.h" // ures_getByKeyWithFallback
#அந்தர்பாவ "ubrkimpl.h" // U_ICUDATA_BRKITR
#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#எனில் FB_DEBUG
#அந்தர்பாவ <stdio.h>
நிலையான வெறுமை _fb_trace(மாறிலி எழுத்து *m, மாறிலி UnicodeString *s, UBool b, int32_t d, மாறிலி எழுத்து *f, முழு l) {
  எழுத்து buf[2048];
  எனில்(s) {
    s->extract(0,s->length(),buf,2048);
  } இல்லையெனில் {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (மாறிலி வெறுமை*)s, b?'T':'F',(முழு)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#இல்லையெனில்
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
நிலையான int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    மாறிலி UnicodeString& a = *static_cast<மாறிலி UnicodeString*>(t1.pointer);
    மாறிலி UnicodeString& b = *static_cast<மாறிலி UnicodeString*>(t2.pointer);
    திரும்பு a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
வகுப்பு UStringSet : பொது UVector {
 பொது:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  மெய்நிகர் ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  உள்வரி UBool contains(மாறிலி UnicodeString& s) {
    திரும்பு contains((வெறுமை*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  உள்வரி மாறிலி UnicodeString* getStringAt(int32_t i) மாறிலி U_LIFETIME_BOUND {
    திரும்பு static_cast<மாறிலி UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  உள்வரி UBool adopt(UnicodeString *str, UErrorCode &status) {
    எனில்(U_FAILURE(status) || contains(*str)) {
      delete str;
      திரும்பு பொய்;
    } இல்லையெனில் {
      sorted_செருகு(str, compareUnicodeString, status);
      எனில்(U_FAILURE(status)) {
        திரும்பு பொய்;
      }
      திரும்பு உண்மை;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  உள்வரி UBool add(மாறிலி UnicodeString& str, UErrorCode &status) {
    எனில்(U_FAILURE(status)) திரும்பு பொய்;
    UnicodeString *t = புதிய UnicodeString(str);
    எனில்(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; திரும்பு பொய்;
    }
    திரும்பு adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  உள்வரி UBool remove(மாறிலி UnicodeString &s, UErrorCode &status) {
    எனில்(U_FAILURE(status)) திரும்பு பொய்;
    திரும்பு removeElement((வெறுமை*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
நிலையான மாறிலி int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
நிலையான மாறிலி int32_t kMATCH   = (1<<1); //< exact match - skip this one.
நிலையான மாறிலி int32_t kSuppressInReverse = (1<<0);
நிலையான மாறிலி int32_t kAddToForward = (1<<1);
நிலையான மாறிலி char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
வகுப்பு SimpleFilteredSentenceBreakData : பொது UMemory {
பொது:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        திரும்பு இது;
    }
    SimpleFilteredSentenceBreakData *decr() {
        எனில்(umtx_atomic_dec(&refcount) <= 0) {
            delete இது;
        }
        திரும்பு nullptr;
    }
    மெய்நிகர் ~SimpleFilteredSentenceBreakData();

    தருக்க hasForwardsPartialTrie() மாறிலி { திரும்பு fForwardsPartialTrie.isValid(); }
    தருக்க hasBackwardsTrie() மாறிலி { திரும்பு fBackwardsTrie.isValid(); }

    மாறிலி UCharsTrie& getForwardsPartialTrie() மாறிலி U_LIFETIME_BOUND {
        திரும்பு *fForwardsPartialTrie;
    }
    மாறிலி UCharsTrie& getBackwardsTrie() மாறிலி U_LIFETIME_BOUND { திரும்பு *fBackwardsTrie; }

தனிப்பட்ட:
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
வகுப்பு SimpleFilteredSentenceBreakIterator : பொது BreakIterator {
பொது:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(மாறிலி SimpleFilteredSentenceBreakIterator& other);
  மெய்நிகர் ~SimpleFilteredSentenceBreakIterator();
தனிப்பட்ட:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
பொது:
  /* -- cloning and other subclass stuff -- */
  மெய்நிகர் BreakIterator *  createBufferClone(வெறுமை * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    திரும்பு clone();
  }
  மெய்நிகர் SimpleFilteredSentenceBreakIterator* clone() மாறிலி override { திரும்பு புதிய SimpleFilteredSentenceBreakIterator(*இது); }
  மெய்நிகர் UClassID getDynamicClassID() மாறிலி override { திரும்பு nullptr; }
  மெய்நிகர் தருக்க operator==(மாறிலி BreakIterator& o) மாறிலி override { எனில்(இது==&o) திரும்பு உண்மை; திரும்பு பொய்; }

  /* -- text modifying -- */
  மெய்நிகர் வெறுமை setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  மெய்நிகர் BreakIterator& refresh_உள்ளீடு_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_உள்ளீடு_Text(input, status);
      திரும்பு *இது;
  }
  மெய்நிகர் வெறுமை adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  மெய்நிகர் வெறுமை setText(மாறிலி UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  மெய்நிகர் UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) மாறிலி override {
      திரும்பு fDelegate->getUText(fillIn, status);
  }
  மெய்நிகர் CharacterIterator& getText() மாறிலி U_LIFETIME_BOUND override {
      திரும்பு fDelegate->getText();
  }

  /* -- ITERATION -- */
  மெய்நிகர் int32_t first() override;
  மெய்நிகர் int32_t preceding(int32_t offset) override;
  மெய்நிகர் int32_t previous() override;
  மெய்நிகர் UBool isBoundary(int32_t offset) override;
  மெய்நிகர் int32_t current() மாறிலி override { திரும்பு fDelegate->current(); } // we keep the delegate current, so this should be correct.

  மெய்நிகர் int32_t next() override;

  மெய்நிகர் int32_t next(int32_t n) override;
  மெய்நிகர் int32_t following(int32_t offset) override;
  மெய்நிகர் int32_t last() override;

தனிப்பட்ட:
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
    வெறுமை resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    எண்ணிக்கை EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    எண்ணிக்கை EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(மாறிலி SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(புதிய SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    எனில் (fData == nullptr) {
        delete forwards;
        delete backwards;
        எனில் (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

வெறுமை SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_மதிப்பு = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    எனில்(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } இல்லையெனில் {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        போது((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            எனில்(USTRINGTRIE_HAS_மதிப்பு(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_மதிப்பு = iter.get_மதிப்பு();
            }
            எனில்(!USTRINGTRIE_HAS_NEXT(r)) {
                நிறுத்து;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    எனில்(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      எனில்(best_மதிப்பு == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        திரும்பு kExceptionHere; // See if the next is another exception.
      } இல்லையெனில் எனில்(best_மதிப்பு == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_மதிப்பு;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        போது((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        எனில்(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            திரும்பு kExceptionHere;
        } இல்லையெனில் {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          திரும்பு kNoExceptionHere;
        }
      } இல்லையெனில் {
        திரும்பு kNoExceptionHere; // internal error and/or no forwards trie
      }
    } இல்லையெனில் {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      திரும்பு kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  எனில்(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      திரும்பு n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  எனில்(U_FAILURE(status)) திரும்பு UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  போது (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    தேர்வு(m) {
    நிலை kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      தொடர்;

    இயல்பு:
    நிலை kNoExceptionHere:
      திரும்பு n;
    }    
  }
  திரும்பு n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  எனில்(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      திரும்பு n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  எனில்(U_FAILURE(status)) திரும்பு UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  போது (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    தேர்வு(m) {
    நிலை kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      தொடர்;

    இயல்பு:
    நிலை kNoExceptionHere:
      திரும்பு n;
    }    
  }
  திரும்பு n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  திரும்பு internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  திரும்பு fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  திரும்பு internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  திரும்பு internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  எனில் (!fDelegate->isBoundary(offset)) திரும்பு பொய்; // no break to suppress

  எனில் (!fData->hasBackwardsTrie()) திரும்பு உண்மை; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  தேர்வு(m) {
  நிலை kExceptionHere:
    திரும்பு பொய்;
  இயல்பு:
  நிலை kNoExceptionHere:
    திரும்பு உண்மை;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  திரும்பு internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  திரும்பு internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  திரும்பு fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
வகுப்பு SimpleFilteredBreakIteratorBuilder : பொது FilteredBreakIteratorBuilder {
பொது:
  மெய்நிகர் ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(மாறிலி Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  மெய்நிகர் UBool suppressBreakAfter(மாறிலி UnicodeString& exception, UErrorCode& status) override;
  மெய்நிகர் UBool unsuppressBreakAfter(மாறிலி UnicodeString& exception, UErrorCode& status) override;
  மெய்நிகர் BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
தனிப்பட்ட:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(மாறிலி Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  எனில்(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    எனில் (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#எனில் FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      திரும்பு;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_சாவி_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    எனில் (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#எனில் FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      திரும்பு;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_சாவி_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#எனில் FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    எனில் (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#எனில் FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      திரும்பு;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    செய் {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      எனில்(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } போது (strs.isValid() && U_SUCCESS(subStatus));
    எனில்(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(மாறிலி UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  திரும்பு r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(மாறிலி UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  திரும்பு r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
நிலையான உள்வரி UnicodeString* newUnicodeStringArray(size_t count) {
    திரும்பு புதிய UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(புதிய UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(புதிய UCharsTrieBuilder(status), status);
  எனில்(U_FAILURE(status)) {
    திரும்பு nullptr;
  }

  int32_t rev_மொத்தம் = 0;
  int32_t fwd_மொத்தம் = 0;

  int32_t sub_மொத்தம் = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_மொத்தம்);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<முழு> partials;
  partials.allocateInsteadAndReset(sub_மொத்தம்);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  முழு n=0;
  க்கு ( int32_t i = 0;
        i<fSet.size();
        i++) {
    மாறிலி UnicodeString *abbr = fSet.getStringAt(i);
    எனில்(abbr) {
      FB_TRACE("build",abbr,உண்மை,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],உண்மை,i);
    } இல்லையெனில் {
      FB_TRACE("build",abbr,பொய்,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      திரும்பு nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  க்கு(முழு i=0;i<sub_மொத்தம்;i++) {
    முழு nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    எனில்(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],பொய்,i);
      // is partial.
      // is it unique?
      முழு sameAs = -1;
      க்கு(முழு j=0;j<sub_மொத்தம்;j++) {
        எனில்(j==i) தொடர்;
        எனில்(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],பொய்,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          எனில்(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],பொய்,j);
          } இல்லையெனில் எனில்(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],பொய்,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],பொய்,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      எனில்(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_மொத்தம்++;
        FB_TRACE("Added partial",&prefix,பொய், i);
        FB_TRACE(u_errorName(status),&ustrs[i],பொய்,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } இல்லையெனில் {
        FB_TRACE("NOT adding partial",&prefix,பொய், i);
        FB_TRACE(u_errorName(status),&ustrs[i],பொய்,i);
      }
    }
  }
  க்கு(முழு i=0;i<sub_மொத்தம்;i++) {
    எனில்(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_மொத்தம்++;
      FB_TRACE(u_errorName(status), &ustrs[i], பொய், i);
    } இல்லையெனில் {
      FB_TRACE("Adding fwd",&ustrs[i], பொய், i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_மொத்தம்++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,பொய், sub_மொத்தம்);

  எனில்(rev_மொத்தம்>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    எனில்(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,பொய், -1);
      திரும்பு nullptr;
    }
  }

  எனில்(fwd_மொத்தம்>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    எனில்(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,பொய், -1);
      திரும்பு nullptr;
    }
  }

  திரும்பு புதிய SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(மாறிலி Locale& where, UErrorCode& status) {
  எனில்(U_FAILURE(status)) திரும்பு nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(புதிய SimpleFilteredBreakIteratorBuilder(where, status), status);
  திரும்பு (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  திரும்பு createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  எனில்(U_FAILURE(status)) திரும்பு nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(புதிய SimpleFilteredBreakIteratorBuilder(status), status);
  திரும்பு (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

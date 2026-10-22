// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#અન્તર્ભાવ "unicode/utypes.h"
#જો !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#અન્તર્ભાવ "cmemory.h"

#અન્તર્ભાવ "unicode/filteredbrk.h"
#અન્તર્ભાવ "unicode/ucharstriebuilder.h"
#અન્તર્ભાવ "unicode/ures.h"

#અન્તર્ભાવ "uresimp.h" // ures_getByKeyWithFallback
#અન્તર્ભાવ "ubrkimpl.h" // U_ICUDATA_BRKITR
#અન્તર્ભાવ "uvector.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#જો FB_DEBUG
#અન્તર્ભાવ <stdio.h>
સ્થિર શૂન્ય _fb_trace(અચલ અક્ષર *m, અચલ UnicodeString *s, UBool b, int32_t d, અચલ અક્ષર *f, પૂર્ણાંક l) {
  અક્ષર buf[2048];
  જો(s) {
    s->extract(0,s->length(),buf,2048);
  } નહીંતર {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (અચલ શૂન્ય*)s, b?'T':'F',(પૂર્ણાંક)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#નહીંતર
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
સ્થિર int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    અચલ UnicodeString& a = *static_cast<અચલ UnicodeString*>(t1.pointer);
    અચલ UnicodeString& b = *static_cast<અચલ UnicodeString*>(t2.pointer);
    પાછા a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
વર્ગ UStringSet : જાહેર UVector {
 જાહેર:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  આભાસી ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  અંતઃરેખ UBool contains(અચલ UnicodeString& s) {
    પાછા contains((શૂન્ય*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  અંતઃરેખ અચલ UnicodeString* getStringAt(int32_t i) અચલ U_LIFETIME_BOUND {
    પાછા static_cast<અચલ UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  અંતઃરેખ UBool adopt(UnicodeString *str, UErrorCode &status) {
    જો(U_FAILURE(status) || contains(*str)) {
      delete str;
      પાછા ખોટું;
    } નહીંતર {
      sorted_ઉમેરો(str, compareUnicodeString, status);
      જો(U_FAILURE(status)) {
        પાછા ખોટું;
      }
      પાછા સાચું;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  અંતઃરેખ UBool add(અચલ UnicodeString& str, UErrorCode &status) {
    જો(U_FAILURE(status)) પાછા ખોટું;
    UnicodeString *t = નવું UnicodeString(str);
    જો(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; પાછા ખોટું;
    }
    પાછા adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  અંતઃરેખ UBool remove(અચલ UnicodeString &s, UErrorCode &status) {
    જો(U_FAILURE(status)) પાછા ખોટું;
    પાછા removeElement((શૂન્ય*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
સ્થિર અચલ int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
સ્થિર અચલ int32_t kMATCH   = (1<<1); //< exact match - skip this one.
સ્થિર અચલ int32_t kSuppressInReverse = (1<<0);
સ્થિર અચલ int32_t kAddToForward = (1<<1);
સ્થિર અચલ char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
વર્ગ SimpleFilteredSentenceBreakData : જાહેર UMemory {
જાહેર:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        પાછા આ;
    }
    SimpleFilteredSentenceBreakData *decr() {
        જો(umtx_atomic_dec(&refcount) <= 0) {
            delete આ;
        }
        પાછા nullptr;
    }
    આભાસી ~SimpleFilteredSentenceBreakData();

    તાર્કિક hasForwardsPartialTrie() અચલ { પાછા fForwardsPartialTrie.isValid(); }
    તાર્કિક hasBackwardsTrie() અચલ { પાછા fBackwardsTrie.isValid(); }

    અચલ UCharsTrie& getForwardsPartialTrie() અચલ U_LIFETIME_BOUND {
        પાછા *fForwardsPartialTrie;
    }
    અચલ UCharsTrie& getBackwardsTrie() અચલ U_LIFETIME_BOUND { પાછા *fBackwardsTrie; }

ખાનગી:
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
વર્ગ SimpleFilteredSentenceBreakIterator : જાહેર BreakIterator {
જાહેર:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(અચલ SimpleFilteredSentenceBreakIterator& other);
  આભાસી ~SimpleFilteredSentenceBreakIterator();
ખાનગી:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
જાહેર:
  /* -- cloning and other subclass stuff -- */
  આભાસી BreakIterator *  createBufferClone(શૂન્ય * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    પાછા clone();
  }
  આભાસી SimpleFilteredSentenceBreakIterator* clone() અચલ override { પાછા નવું SimpleFilteredSentenceBreakIterator(*આ); }
  આભાસી UClassID getDynamicClassID() અચલ override { પાછા nullptr; }
  આભાસી તાર્કિક operator==(અચલ BreakIterator& o) અચલ override { જો(આ==&o) પાછા સાચું; પાછા ખોટું; }

  /* -- text modifying -- */
  આભાસી શૂન્ય setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  આભાસી BreakIterator& refresh_આગત_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_આગત_Text(input, status);
      પાછા *આ;
  }
  આભાસી શૂન્ય adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  આભાસી શૂન્ય setText(અચલ UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  આભાસી UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) અચલ override {
      પાછા fDelegate->getUText(fillIn, status);
  }
  આભાસી CharacterIterator& getText() અચલ U_LIFETIME_BOUND override {
      પાછા fDelegate->getText();
  }

  /* -- ITERATION -- */
  આભાસી int32_t first() override;
  આભાસી int32_t preceding(int32_t offset) override;
  આભાસી int32_t previous() override;
  આભાસી UBool isBoundary(int32_t offset) override;
  આભાસી int32_t current() અચલ override { પાછા fDelegate->current(); } // we keep the delegate current, so this should be correct.

  આભાસી int32_t next() override;

  આભાસી int32_t next(int32_t n) override;
  આભાસી int32_t following(int32_t offset) override;
  આભાસી int32_t last() override;

ખાનગી:
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
    શૂન્ય resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    ગણના EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    ગણના EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(અચલ SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(નવું SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    જો (fData == nullptr) {
        delete forwards;
        delete backwards;
        જો (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

શૂન્ય SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_કિંમત = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    જો(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } નહીંતર {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        જ્યારે((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            જો(USTRINGTRIE_HAS_કિંમત(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_કિંમત = iter.get_કિંમત();
            }
            જો(!USTRINGTRIE_HAS_NEXT(r)) {
                તોડો;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    જો(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      જો(best_કિંમત == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        પાછા kExceptionHere; // See if the next is another exception.
      } નહીંતર જો(best_કિંમત == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_કિંમત;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        જ્યારે((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        જો(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            પાછા kExceptionHere;
        } નહીંતર {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          પાછા kNoExceptionHere;
        }
      } નહીંતર {
        પાછા kNoExceptionHere; // internal error and/or no forwards trie
      }
    } નહીંતર {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      પાછા kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  જો(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      પાછા n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  જો(U_FAILURE(status)) પાછા UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  જ્યારે (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    પસંદગી(m) {
    સ્થિતિ kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      ચાલુ;

    મૂળભૂત:
    સ્થિતિ kNoExceptionHere:
      પાછા n;
    }    
  }
  પાછા n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  જો(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      પાછા n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  જો(U_FAILURE(status)) પાછા UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  જ્યારે (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    પસંદગી(m) {
    સ્થિતિ kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      ચાલુ;

    મૂળભૂત:
    સ્થિતિ kNoExceptionHere:
      પાછા n;
    }    
  }
  પાછા n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  પાછા internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  પાછા fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  પાછા internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  પાછા internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  જો (!fDelegate->isBoundary(offset)) પાછા ખોટું; // no break to suppress

  જો (!fData->hasBackwardsTrie()) પાછા સાચું; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  પસંદગી(m) {
  સ્થિતિ kExceptionHere:
    પાછા ખોટું;
  મૂળભૂત:
  સ્થિતિ kNoExceptionHere:
    પાછા સાચું;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  પાછા internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  પાછા internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  પાછા fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
વર્ગ SimpleFilteredBreakIteratorBuilder : જાહેર FilteredBreakIteratorBuilder {
જાહેર:
  આભાસી ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(અચલ Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  આભાસી UBool suppressBreakAfter(અચલ UnicodeString& exception, UErrorCode& status) override;
  આભાસી UBool unsuppressBreakAfter(અચલ UnicodeString& exception, UErrorCode& status) override;
  આભાસી BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
ખાનગી:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(અચલ Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  જો(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    જો (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#જો FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      પાછા;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_ચાવી_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    જો (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#જો FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      પાછા;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_ચાવી_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#જો FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    જો (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#જો FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      પાછા;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    કરો {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      જો(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } જ્યારે (strs.isValid() && U_SUCCESS(subStatus));
    જો(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(અચલ UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  પાછા r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(અચલ UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  પાછા r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
સ્થિર અંતઃરેખ UnicodeString* newUnicodeStringArray(size_t count) {
    પાછા નવું UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(નવું UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(નવું UCharsTrieBuilder(status), status);
  જો(U_FAILURE(status)) {
    પાછા nullptr;
  }

  int32_t rev_ગણતરી = 0;
  int32_t fwd_ગણતરી = 0;

  int32_t sub_ગણતરી = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_ગણતરી);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<પૂર્ણાંક> partials;
  partials.allocateInsteadAndReset(sub_ગણતરી);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  પૂર્ણાંક n=0;
  માટે ( int32_t i = 0;
        i<fSet.size();
        i++) {
    અચલ UnicodeString *abbr = fSet.getStringAt(i);
    જો(abbr) {
      FB_TRACE("build",abbr,સાચું,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],સાચું,i);
    } નહીંતર {
      FB_TRACE("build",abbr,ખોટું,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      પાછા nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  માટે(પૂર્ણાંક i=0;i<sub_ગણતરી;i++) {
    પૂર્ણાંક nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    જો(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],ખોટું,i);
      // is partial.
      // is it unique?
      પૂર્ણાંક sameAs = -1;
      માટે(પૂર્ણાંક j=0;j<sub_ગણતરી;j++) {
        જો(j==i) ચાલુ;
        જો(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],ખોટું,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          જો(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],ખોટું,j);
          } નહીંતર જો(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],ખોટું,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],ખોટું,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      જો(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_ગણતરી++;
        FB_TRACE("Added partial",&prefix,ખોટું, i);
        FB_TRACE(u_errorName(status),&ustrs[i],ખોટું,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } નહીંતર {
        FB_TRACE("NOT adding partial",&prefix,ખોટું, i);
        FB_TRACE(u_errorName(status),&ustrs[i],ખોટું,i);
      }
    }
  }
  માટે(પૂર્ણાંક i=0;i<sub_ગણતરી;i++) {
    જો(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_ગણતરી++;
      FB_TRACE(u_errorName(status), &ustrs[i], ખોટું, i);
    } નહીંતર {
      FB_TRACE("Adding fwd",&ustrs[i], ખોટું, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_ગણતરી++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,ખોટું, sub_ગણતરી);

  જો(rev_ગણતરી>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    જો(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,ખોટું, -1);
      પાછા nullptr;
    }
  }

  જો(fwd_ગણતરી>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    જો(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,ખોટું, -1);
      પાછા nullptr;
    }
  }

  પાછા નવું SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(અચલ Locale& where, UErrorCode& status) {
  જો(U_FAILURE(status)) પાછા nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(નવું SimpleFilteredBreakIteratorBuilder(where, status), status);
  પાછા (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  પાછા createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  જો(U_FAILURE(status)) પાછા nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(નવું SimpleFilteredBreakIteratorBuilder(status), status);
  પાછા (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

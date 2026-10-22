// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"
#यदि !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

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

#यदि FB_DEBUG
#अन्तर्भाव <stdio.h>
स्थिर शून्य _fb_trace(अचर वर्ण *m, अचर UnicodeString *s, UBool b, int32_t d, अचर वर्ण *f, पूर्णाङ्क l) {
  वर्ण buf[2048];
  यदि(s) {
    s->extract(0,s->length(),buf,2048);
  } अन्यथा {
    म_नकल(buf,"nullptr");
  }
  ख_लिखो(मानक_त्रुटि,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (अचर शून्य*)s, b?'T':'F',(पूर्णाङ्क)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#अन्यथा
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
स्थिर int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    अचर UnicodeString& a = *static_cast<अचर UnicodeString*>(t1.pointer);
    अचर UnicodeString& b = *static_cast<अचर UnicodeString*>(t2.pointer);
    फर्काऊ a.compare(b);
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
  अन्तःस्थ UBool contains(अचर UnicodeString& s) {
    फर्काऊ contains((शून्य*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  अन्तःस्थ अचर UnicodeString* getStringAt(int32_t i) अचर U_LIFETIME_BOUND {
    फर्काऊ static_cast<अचर UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  अन्तःस्थ UBool adopt(UnicodeString *str, UErrorCode &status) {
    यदि(U_FAILURE(status) || contains(*str)) {
      delete str;
      फर्काऊ असत्य;
    } अन्यथा {
      sorted_घुसाउ(str, compareUnicodeString, status);
      यदि(U_FAILURE(status)) {
        फर्काऊ असत्य;
      }
      फर्काऊ सत्य;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  अन्तःस्थ UBool add(अचर UnicodeString& str, UErrorCode &status) {
    यदि(U_FAILURE(status)) फर्काऊ असत्य;
    UnicodeString *t = नूतन UnicodeString(str);
    यदि(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; फर्काऊ असत्य;
    }
    फर्काऊ adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  अन्तःस्थ UBool हटाओ(अचर UnicodeString &s, UErrorCode &status) {
    यदि(U_FAILURE(status)) फर्काऊ असत्य;
    फर्काऊ removeElement((शून्य*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
स्थिर अचर int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
स्थिर अचर int32_t kMATCH   = (1<<1); //< exact match - skip this one.
स्थिर अचर int32_t kSuppressInReverse = (1<<0);
स्थिर अचर int32_t kAddToForward = (1<<1);
स्थिर अचर char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
वर्ग SimpleFilteredSentenceBreakData : सार्वजनिक UMemory {
सार्वजनिक:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        फर्काऊ एतत्;
    }
    SimpleFilteredSentenceBreakData *decr() {
        यदि(umtx_atomic_dec(&refcount) <= 0) {
            delete एतत्;
        }
        फर्काऊ nullptr;
    }
    आभास ~SimpleFilteredSentenceBreakData();

    तार्किक hasForwardsPartialTrie() अचर { फर्काऊ fForwardsPartialTrie.isValid(); }
    तार्किक hasBackwardsTrie() अचर { फर्काऊ fBackwardsTrie.isValid(); }

    अचर UCharsTrie& getForwardsPartialTrie() अचर U_LIFETIME_BOUND {
        फर्काऊ *fForwardsPartialTrie;
    }
    अचर UCharsTrie& getBackwardsTrie() अचर U_LIFETIME_BOUND { फर्काऊ *fBackwardsTrie; }

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
  SimpleFilteredSentenceBreakIterator(अचर SimpleFilteredSentenceBreakIterator& other);
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
    फर्काऊ clone();
  }
  आभास SimpleFilteredSentenceBreakIterator* clone() अचर override { फर्काऊ नूतन SimpleFilteredSentenceBreakIterator(*एतत्); }
  आभास UClassID getDynamicClassID() अचर override { फर्काऊ nullptr; }
  आभास तार्किक चालक==(अचर BreakIterator& o) अचर override { यदि(एतत्==&o) फर्काऊ सत्य; फर्काऊ असत्य; }

  /* -- text modifying -- */
  आभास शून्य setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  आभास BreakIterator& refresh_आगत_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_आगत_Text(input, status);
      फर्काऊ *एतत्;
  }
  आभास शून्य adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  आभास शून्य setText(अचर UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  आभास UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) अचर override {
      फर्काऊ fDelegate->getUText(fillIn, status);
  }
  आभास CharacterIterator& getText() अचर U_LIFETIME_BOUND override {
      फर्काऊ fDelegate->getText();
  }

  /* -- ITERATION -- */
  आभास int32_t first() override;
  आभास int32_t preceding(int32_t offset) override;
  आभास int32_t previous() override;
  आभास UBool isBoundary(int32_t offset) override;
  आभास int32_t current() अचर override { फर्काऊ fDelegate->current(); } // we keep the delegate current, so this should be correct.

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

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(अचर SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(नूतन SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    यदि (fData == nullptr) {
        delete forwards;
        delete backwards;
        यदि (U_SUCCESS(status)) {
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
    int32_t best_मान = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    यदि(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
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
        जबसम्म((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            यदि(USTRINGTRIE_HAS_मान(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                best_मान = iter.get_मान();
            }
            यदि(!USTRINGTRIE_HAS_NEXT(r)) {
                रोक;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    यदि(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      यदि(best_मान == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        फर्काऊ kExceptionHere; // See if the next is another exception.
      } अन्यथा यदि(best_मान == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_मान;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        जबसम्म((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        यदि(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            फर्काऊ kExceptionHere;
        } अन्यथा {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          फर्काऊ kNoExceptionHere;
        }
      } अन्यथा {
        फर्काऊ kNoExceptionHere; // internal error and/or no forwards trie
      }
    } अन्यथा {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      फर्काऊ kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  यदि(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      फर्काऊ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  यदि(U_FAILURE(status)) फर्काऊ UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  जबसम्म (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    विकल्प(m) {
    पक्ष kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      जारी;

    स्वभाव:
    पक्ष kNoExceptionHere:
      फर्काऊ n;
    }    
  }
  फर्काऊ n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  यदि(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      फर्काऊ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  यदि(U_FAILURE(status)) फर्काऊ UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  जबसम्म (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    विकल्प(m) {
    पक्ष kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      जारी;

    स्वभाव:
    पक्ष kNoExceptionHere:
      फर्काऊ n;
    }    
  }
  फर्काऊ n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  फर्काऊ internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  फर्काऊ fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  फर्काऊ internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  फर्काऊ internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  यदि (!fDelegate->isBoundary(offset)) फर्काऊ असत्य; // no break to suppress

  यदि (!fData->hasBackwardsTrie()) फर्काऊ सत्य; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  विकल्प(m) {
  पक्ष kExceptionHere:
    फर्काऊ असत्य;
  स्वभाव:
  पक्ष kNoExceptionHere:
    फर्काऊ सत्य;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  फर्काऊ internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  फर्काऊ internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  फर्काऊ fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
वर्ग SimpleFilteredBreakIteratorBuilder : सार्वजनिक FilteredBreakIteratorBuilder {
सार्वजनिक:
  आभास ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(अचर Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  आभास UBool suppressBreakAfter(अचर UnicodeString& exception, UErrorCode& status) override;
  आभास UBool unsuppressBreakAfter(अचर UnicodeString& exception, UErrorCode& status) override;
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

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(अचर Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  यदि(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    यदि (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#यदि FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      फर्काऊ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_साँचो_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    यदि (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#यदि FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      फर्काऊ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_साँचो_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#यदि FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      ख_लिखो(मानक_त्रुटि, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    यदि (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#यदि FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      फर्काऊ;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    गर {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      यदि(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } जबसम्म (strs.isValid() && U_SUCCESS(subStatus));
    यदि(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(अचर UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  फर्काऊ r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(अचर UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.हटाओ(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  फर्काऊ r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
स्थिर अन्तःस्थ UnicodeString* newUnicodeStringArray(माप_प्रकार count) {
    फर्काऊ नूतन UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(नूतन UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(नूतन UCharsTrieBuilder(status), status);
  यदि(U_FAILURE(status)) {
    फर्काऊ nullptr;
  }

  int32_t rev_गन्ती = 0;
  int32_t fwd_गन्ती = 0;

  int32_t sub_गन्ती = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_गन्ती);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<पूर्णाङ्क> partials;
  partials.allocateInsteadAndReset(sub_गन्ती);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  पूर्णाङ्क n=0;
  लागि ( int32_t i = 0;
        i<fSet.size();
        i++) {
    अचर UnicodeString *abbr = fSet.getStringAt(i);
    यदि(abbr) {
      FB_TRACE("build",abbr,सत्य,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],सत्य,i);
    } अन्यथा {
      FB_TRACE("build",abbr,असत्य,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      फर्काऊ nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  लागि(पूर्णाङ्क i=0;i<sub_गन्ती;i++) {
    पूर्णाङ्क nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    यदि(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],असत्य,i);
      // is partial.
      // is it unique?
      पूर्णाङ्क sameAs = -1;
      लागि(पूर्णाङ्क j=0;j<sub_गन्ती;j++) {
        यदि(j==i) जारी;
        यदि(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],असत्य,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          यदि(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],असत्य,j);
          } अन्यथा यदि(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],असत्य,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],असत्य,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      यदि(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_गन्ती++;
        FB_TRACE("Added partial",&prefix,असत्य, i);
        FB_TRACE(u_errorName(status),&ustrs[i],असत्य,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } अन्यथा {
        FB_TRACE("NOT adding partial",&prefix,असत्य, i);
        FB_TRACE(u_errorName(status),&ustrs[i],असत्य,i);
      }
    }
  }
  लागि(पूर्णाङ्क i=0;i<sub_गन्ती;i++) {
    यदि(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_गन्ती++;
      FB_TRACE(u_errorName(status), &ustrs[i], असत्य, i);
    } अन्यथा {
      FB_TRACE("Adding fwd",&ustrs[i], असत्य, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_गन्ती++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,असत्य, sub_गन्ती);

  यदि(rev_गन्ती>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    यदि(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,असत्य, -1);
      फर्काऊ nullptr;
    }
  }

  यदि(fwd_गन्ती>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    यदि(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,असत्य, -1);
      फर्काऊ nullptr;
    }
  }

  फर्काऊ नूतन SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(अचर Locale& where, UErrorCode& status) {
  यदि(U_FAILURE(status)) फर्काऊ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नूतन SimpleFilteredBreakIteratorBuilder(where, status), status);
  फर्काऊ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  फर्काऊ createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  यदि(U_FAILURE(status)) फर्काऊ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नूतन SimpleFilteredBreakIteratorBuilder(status), status);
  फर्काऊ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

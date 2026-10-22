// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#समावेश "unicode/utypes.h"
#अगर !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#समावेश "cmemory.h"

#समावेश "unicode/filteredbrk.h"
#समावेश "unicode/ucharstriebuilder.h"
#समावेश "unicode/ures.h"

#समावेश "uresimp.h" // ures_getByKeyWithFallback
#समावेश "ubrkimpl.h" // U_ICUDATA_BRKITR
#समावेश "uvector.h"
#समावेश "cmemory.h"
#समावेश "umutex.h"

U_NAMESPACE_BEGIN

#अगर_अघोषित FB_DEBUG
#घोषणा FB_DEBUG 0
#पूर्ण_अगर

#अगर FB_DEBUG
#समावेश <stdio.h>
अटल व्योम _fb_trace(स्थिर अक्षर *m, स्थिर UnicodeString *s, UBool b, int32_t d, स्थिर अक्षर *f, पूर्णांक l) {
  अक्षर buf[2048];
  अगर(s) {
    s->extract(0,s->length(),buf,2048);
  } अन्यथा {
    म_नकल(buf,"nullptr");
  }
  ख_लिखो(मानक_त्रुटि,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (स्थिर व्योम*)s, b?'T':'F',(पूर्णांक)d);
}

#घोषणा FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__खाता__,__LINE__)
#अन्यथा
#घोषणा FB_TRACE(m,s,b,d)
#पूर्ण_अगर

/**
 * Used with sortedInsert()
 */
अटल int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    स्थिर UnicodeString& a = *static_cast<स्थिर UnicodeString*>(t1.pointer);
    स्थिर UnicodeString& b = *static_cast<स्थिर UnicodeString*>(t2.pointer);
    वापस a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
श्रेणी UStringSet : खुला UVector {
 खुला:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  भव ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  अंतरभूत UBool contains(स्थिर UnicodeString& s) {
    वापस contains((व्योम*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  अंतरभूत स्थिर UnicodeString* getStringAt(int32_t i) स्थिर U_LIFETIME_BOUND {
    वापस static_cast<स्थिर UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  अंतरभूत UBool adopt(UnicodeString *str, UErrorCode &status) {
    अगर(U_FAILURE(status) || contains(*str)) {
      मिटाओ str;
      वापस false;
    } अन्यथा {
      sorted_जोड़ो(str, compareUnicodeString, status);
      अगर(U_FAILURE(status)) {
        वापस false;
      }
      वापस true;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  अंतरभूत UBool add(स्थिर UnicodeString& str, UErrorCode &status) {
    अगर(U_FAILURE(status)) वापस false;
    UnicodeString *t = नया UnicodeString(str);
    अगर(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; वापस false;
    }
    वापस adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  अंतरभूत UBool हटाओ(स्थिर UnicodeString &s, UErrorCode &status) {
    अगर(U_FAILURE(status)) वापस false;
    वापस removeElement((व्योम*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
अटल स्थिर int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
अटल स्थिर int32_t kMATCH   = (1<<1); //< exact match - skip this one.
अटल स्थिर int32_t kSuppressInReverse = (1<<0);
अटल स्थिर int32_t kAddToForward = (1<<1);
अटल स्थिर char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
श्रेणी Simple_Filtered_Sentence_Break_तथ्य : खुला UMemory {
खुला:
  Simple_Filtered_Sentence_Break_तथ्य(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_तथ्य* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        वापस यह;
    }
    Simple_Filtered_Sentence_Break_तथ्य *decr() {
        अगर(umtx_atomic_dec(&refcount) <= 0) {
            मिटाओ यह;
        }
        वापस nullptr;
    }
    भव ~Simple_Filtered_Sentence_Break_तथ्य();

    bool hasForwardsPartialTrie() स्थिर { वापस fForwardsPartialTrie.isValid(); }
    bool hasBackwardsTrie() स्थिर { वापस fBackwardsTrie.isValid(); }

    स्थिर UCharsTrie& getForwardsPartialTrie() स्थिर U_LIFETIME_BOUND {
        वापस *fForwardsPartialTrie;
    }
    स्थिर UCharsTrie& getBackwardsTrie() स्थिर U_LIFETIME_BOUND { वापस *fBackwardsTrie; }

निजी:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    LocalPointer<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    LocalPointer<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_तथ्य::~Simple_Filtered_Sentence_Break_तथ्य() {}

/**
 * Concrete implementation
 */
श्रेणी SimpleFilteredSentenceBreakIterator : खुला BreakIterator {
खुला:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(स्थिर SimpleFilteredSentenceBreakIterator& other);
  भव ~SimpleFilteredSentenceBreakIterator();
निजी:
  Simple_Filtered_Sentence_Break_तथ्य *f_तथ्य;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
खुला:
  /* -- cloning and other subclass stuff -- */
  भव BreakIterator *  create_बफर_Clone(व्योम * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    वापस clone();
  }
  भव SimpleFilteredSentenceBreakIterator* clone() स्थिर override { वापस नया SimpleFilteredSentenceBreakIterator(*यह); }
  भव UClassID getDynamicClassID() स्थिर override { वापस nullptr; }
  भव bool चालक==(स्थिर BreakIterator& o) स्थिर override { अगर(यह==&o) वापस true; वापस false; }

  /* -- text modifying -- */
  भव व्योम setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  भव BreakIterator& refresh_निवेश_Text(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refresh_निवेश_Text(input, status);
      वापस *यह;
  }
  भव व्योम adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  भव व्योम setText(स्थिर UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  भव UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) स्थिर override {
      वापस fDelegate->getUText(fillIn, status);
  }
  भव CharacterIterator& getText() स्थिर U_LIFETIME_BOUND override {
      वापस fDelegate->getText();
  }

  /* -- ITERATION -- */
  भव int32_t first() override;
  भव int32_t preceding(int32_t offset) override;
  भव int32_t previous() override;
  भव UBool isBoundary(int32_t offset) override;
  भव int32_t current() स्थिर override { वापस fDelegate->current(); } // we keep the delegate current, so this should be correct.

  भव int32_t next() override;

  भव int32_t next(int32_t n) override;
  भव int32_t following(int32_t offset) override;
  भव int32_t last() override;

निजी:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t internal_अग्र(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t internal_पूर्व(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    व्योम resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    क्रमागत EFBMatch_परिणाम { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    क्रमागत EFBMatch_परिणाम breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(स्थिर SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_तथ्य(other.f_तथ्य->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  f_तथ्य(नया Simple_Filtered_Sentence_Break_तथ्य(forwards, backwards)),
  fDelegate(adopt)
{
    अगर (f_तथ्य == nullptr) {
        मिटाओ forwards;
        मिटाओ backwards;
        अगर (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_तथ्य = f_तथ्य->decr();
}

व्योम SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_परिणाम
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_मूल्य = -1;
    // loops while 'n' points to an exception.
    utext_set_Native_सूचकांक(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    अगर(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } अन्यथा {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(f_तथ्य->getBackwardsTrie());
        UChar32 uch;
        जबतक((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_परिणाम r = iter.अग्र_For_Code_Point(uch);
            अगर(USTRINGTRIE_HAS_मूल्य(r)) { // remember the best match so far
                bestPosn = utext_get_Native_सूचकांक(fText.getAlias());
                best_मूल्य = iter.get_मूल्य();
            }
            अगर(!USTRINGTRIE_HAS_अग्र(r)) {
                अवरोध;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    अगर(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      अगर(best_मूल्य == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        वापस kExceptionHere; // See if the next is another exception.
      } अन्यथा अगर(best_मूल्य == kPARTIAL
                && f_तथ्य->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_परिणाम rfwd = USTRINGTRIE_INTERMEDIATE_मूल्य;
        utext_set_Native_सूचकांक(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_तथ्य->getForwardsPartialTrie());
        UChar32 uch;
        जबतक((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_अग्र(rfwd=iter.अग्र_For_Code_Point(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        अगर(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            वापस kExceptionHere;
        } अन्यथा {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          वापस kNoExceptionHere;
        }
      } अन्यथा {
        वापस kNoExceptionHere; // internal error and/or no forwards trie
      }
    } अन्यथा {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      वापस kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internal_अग्र(int32_t n) {
  अगर(n == UBRK_DONE || // at end  or
    !f_तथ्य->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      वापस n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  अगर(U_FAILURE(status)) वापस UBRK_DONE; // bail out
  int64_t utextLen = utext_native_लंबाई(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  जबतक (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_परिणाम m = breakExceptionAt(n);

    चयन(m) {
    हाल kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      जारी;

    शेष:
    हाल kNoExceptionHere:
      वापस n;
    }    
  }
  वापस n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internal_पूर्व(int32_t n) {
  अगर(n == 0 || n == UBRK_DONE || // at end  or
    !f_तथ्य->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      वापस n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  अगर(U_FAILURE(status)) वापस UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  जबतक (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_परिणाम m = breakExceptionAt(n);

    चयन(m) {
    हाल kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      जारी;

    शेष:
    हाल kNoExceptionHere:
      वापस n;
    }    
  }
  वापस n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  वापस internal_अग्र(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  वापस fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  वापस internal_पूर्व(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  वापस internal_पूर्व(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  अगर (!fDelegate->isBoundary(offset)) वापस false; // no break to suppress

  अगर (!f_तथ्य->hasBackwardsTrie()) वापस true; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatch_परिणाम m = breakExceptionAt(offset);

  चयन(m) {
  हाल kExceptionHere:
    वापस false;
  शेष:
  हाल kNoExceptionHere:
    वापस true;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  वापस internal_अग्र(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  वापस internal_अग्र(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  वापस fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
श्रेणी SimpleFilteredBreakIteratorBuilder : खुला FilteredBreakIteratorBuilder {
खुला:
  भव ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(स्थिर Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  भव UBool suppressBreakAfter(स्थिर UnicodeString& exception, UErrorCode& status) override;
  भव UBool unsuppressBreakAfter(स्थिर UnicodeString& exception, UErrorCode& status) override;
  भव BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
निजी:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(स्थिर Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  अगर(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    अगर (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#अगर FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#पूर्ण_अगर
      वापस;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_कुंजी_With_Fallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    अगर (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#अगर FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#पूर्ण_अगर
      वापस;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_कुंजी_With_Fallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#अगर FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      ख_लिखो(मानक_त्रुटि, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#पूर्ण_अगर
    
    अगर (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#अगर FB_DEBUG
      ख_लिखो(मानक_त्रुटि, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#पूर्ण_अगर
      वापस;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    करो {
      strs.adoptInstead(ures_get_अग्र_Resource(breaks.getAlias(), strs.orphan(), &subStatus));
      अगर(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } जबतक (strs.isValid() && U_SUCCESS(subStatus));
    अगर(U_FAILURE(subStatus)&&subStatus!=U_सूचकांक_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(स्थिर UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  वापस r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(स्थिर UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.हटाओ(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  वापस r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
अटल अंतरभूत UnicodeString* newUnicodeStringArray(माप_प्रकार count) {
    वापस नया UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(नया UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(नया UCharsTrieBuilder(status), status);
  अगर(U_FAILURE(status)) {
    वापस nullptr;
  }

  int32_t rev_गिनती = 0;
  int32_t fwd_गिनती = 0;

  int32_t sub_गिनती = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_गिनती);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<पूर्णांक> partials;
  partials.allocateInsteadAndReset(sub_गिनती);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  पूर्णांक n=0;
  क्रम ( int32_t i = 0;
        i<fSet.size();
        i++) {
    स्थिर UnicodeString *abbr = fSet.getStringAt(i);
    अगर(abbr) {
      FB_TRACE("build",abbr,true,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],true,i);
    } अन्यथा {
      FB_TRACE("build",abbr,false,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      वापस nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  क्रम(पूर्णांक i=0;i<sub_गिनती;i++) {
    पूर्णांक nn = ustrs[i].सूचकांक_Of(kFULLSTOP); // TODO: non-'.' abbreviations
    अगर(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],false,i);
      // is partial.
      // is it unique?
      पूर्णांक sameAs = -1;
      क्रम(पूर्णांक j=0;j<sub_गिनती;j++) {
        अगर(j==i) जारी;
        अगर(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],false,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          अगर(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],false,j);
          } अन्यथा अगर(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],false,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],false,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      अगर(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        rev_गिनती++;
        FB_TRACE("Added partial",&prefix,false, i);
        FB_TRACE(u_errorName(status),&ustrs[i],false,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } अन्यथा {
        FB_TRACE("NOT adding partial",&prefix,false, i);
        FB_TRACE(u_errorName(status),&ustrs[i],false,i);
      }
    }
  }
  क्रम(पूर्णांक i=0;i<sub_गिनती;i++) {
    अगर(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      rev_गिनती++;
      FB_TRACE(u_errorName(status), &ustrs[i], false, i);
    } अन्यथा {
      FB_TRACE("Adding fwd",&ustrs[i], false, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwd_गिनती++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,false, sub_गिनती);

  अगर(rev_गिनती>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    अगर(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,false, -1);
      वापस nullptr;
    }
  }

  अगर(fwd_गिनती>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    अगर(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,false, -1);
      वापस nullptr;
    }
  }

  वापस नया SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(स्थिर Locale& where, UErrorCode& status) {
  अगर(U_FAILURE(status)) वापस nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नया SimpleFilteredBreakIteratorBuilder(where, status), status);
  वापस (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  वापस createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  अगर(U_FAILURE(status)) वापस nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नया SimpleFilteredBreakIteratorBuilder(status), status);
  वापस (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#पूर्ण_अगर //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

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

#यदि_अपरिभाषितम् FB_DEBUG
#परिभाषय FB_DEBUG 0
#यदि_अन्तः

#यदि FB_DEBUG
#अन्तर्भाव <stdio.h>
स्थिर शून्य _fb_trace(ध्रुव वर्ण *m, ध्रुव UnicodeString *s, UBool b, int32_t d, ध्रुव वर्ण *f, पूर्णाङ्क l) {
  वर्ण buf[2048];
  यदि(s) {
    s->extract(0,s->length(),buf,2048);
  } अन्यथा {
    वाक्यं_प्रतिलिख(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ध्रुव शून्य*)s, b?'T':'F',(पूर्णाङ्क)d);
}

#परिभाषय FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#अन्यथा
#परिभाषय FB_TRACE(m,s,b,d)
#यदि_अन्तः

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
वर्ग UString_समुच्चयः : सार्वजनिक UVector {
 सार्वजनिक:
  UString_समुच्चयः(UError_संहिता &स्थितिः) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           स्थितिः) {}
  आभास ~UString_समुच्चयः();
  /**
   * Is this UnicodeSet contained?
   */
  अन्तःस्थ UBool अन्तर्भवति(ध्रुव UnicodeString& s) {
    प्रत्यागम अन्तर्भवति((शून्य*) &s);
  }
  उपयुज्य UVector::अन्तर्भवति;
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
  अन्तःस्थ UBool adopt(UnicodeString *str, UError_संहिता &स्थितिः) {
    यदि(U_FAILURE(स्थितिः) || अन्तर्भवति(*str)) {
      विलोपय str;
      प्रत्यागम असत्य;
    } अन्यथा {
      sorted_निवेशय(str, compareUnicodeString, स्थितिः);
      यदि(U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
      }
      प्रत्यागम सत्य;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  अन्तःस्थ UBool सङ्कलय(ध्रुव UnicodeString& str, UError_संहिता &स्थितिः) {
    यदि(U_FAILURE(स्थितिः)) प्रत्यागम असत्य;
    UnicodeString *t = नूतन UnicodeString(str);
    यदि(t==शून्यसूचकः) {
      स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR; प्रत्यागम असत्य;
    }
    प्रत्यागम adopt(t, स्थितिः);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  अन्तःस्थ UBool remove(ध्रुव UnicodeString &s, UError_संहिता &स्थितिः) {
    यदि(U_FAILURE(स्थितिः)) प्रत्यागम असत्य;
    प्रत्यागम removeElement((शून्य*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_समुच्चयः::~UString_समुच्चयः() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
स्थिर ध्रुव int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
स्थिर ध्रुव int32_t k_मेलनम्   = (1<<1); //< exact match - skip this one.
स्थिर ध्रुव int32_t k_Suppress_अन्तर्_Reverse = (1<<0);
स्थिर ध्रुव int32_t k_सङ्कलय_गन्तव्यम्_Forward = (1<<1);
स्थिर ध्रुव char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
वर्ग Simple_Filtered_Sentence_Break_दत्त : सार्वजनिक UMemory {
सार्वजनिक:
  Simple_Filtered_Sentence_Break_दत्त(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_दत्त* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        प्रत्यागम एतत्;
    }
    Simple_Filtered_Sentence_Break_दत्त *decr() {
        यदि(umtx_atomic_dec(&refcount) <= 0) {
            विलोपय एतत्;
        }
        प्रत्यागम शून्यसूचकः;
    }
    आभास ~Simple_Filtered_Sentence_Break_दत्त();

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

Simple_Filtered_Sentence_Break_दत्त::~Simple_Filtered_Sentence_Break_दत्त() {}

/**
 * Concrete implementation
 */
वर्ग SimpleFilteredSentenceBreakIterator : सार्वजनिक BreakIterator {
सार्वजनिक:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_संहिता &स्थितिः);
  SimpleFilteredSentenceBreakIterator(ध्रुव SimpleFilteredSentenceBreakIterator& other);
  आभास ~SimpleFilteredSentenceBreakIterator();
गुप्त:
  Simple_Filtered_Sentence_Break_दत्त *f_दत्त;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
सार्वजनिक:
  /* -- cloning and other subclass stuff -- */
  आभास BreakIterator *  create_बफर_Clone(शून्य * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_संहिता &स्थितिः) override {
    // for now - always deep clone
    स्थितिः = U_SAFECLONE_ALLOCATED_WARNING;
    प्रत्यागम clone();
  }
  आभास SimpleFilteredSentenceBreakIterator* clone() ध्रुव override { प्रत्यागम नूतन SimpleFilteredSentenceBreakIterator(*एतत्); }
  आभास UClassID getDynamicClassID() ध्रुव override { प्रत्यागम शून्यसूचकः; }
  आभास तार्किक संक्रियकः==(ध्रुव BreakIterator& o) ध्रुव override { यदि(एतत्==&o) प्रत्यागम सत्य; प्रत्यागम असत्य; }

  /* -- text modifying -- */
  आभास शून्य समुच्चयः_Text(UText *text, UError_संहिता &स्थितिः) override { fDelegate->समुच्चयः_Text(text,स्थितिः); }
  आभास BreakIterator& refresh_निवेशः_Text(UText* input,
                                          UError_संहिता& स्थितिः) U_LIFETIME_BOUND override {
      fDelegate->refresh_निवेशः_Text(input, स्थितिः);
      प्रत्यागम *एतत्;
  }
  आभास शून्य adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  आभास शून्य समुच्चयः_Text(ध्रुव UnicodeString &text) override { fDelegate->समुच्चयः_Text(text); }

  /* -- other functions that are just delegated -- */
  आभास UText* getUText(UText* fill_अन्तर् U_LIFETIME_BOUND, UError_संहिता& स्थितिः) ध्रुव override {
      प्रत्यागम fDelegate->getUText(fill_अन्तर्, स्थितिः);
  }
  आभास CharacterIterator& getText() ध्रुव U_LIFETIME_BOUND override {
      प्रत्यागम fDelegate->getText();
  }

  /* -- ITERATION -- */
  आभास int32_t first() override;
  आभास int32_t preceding(int32_t विस्थापनम्) override;
  आभास int32_t previous() override;
  आभास UBool isBoundary(int32_t विस्थापनम्) override;
  आभास int32_t current() ध्रुव override { प्रत्यागम fDelegate->current(); } // we keep the delegate current, so this should be correct.

  आभास int32_t next() override;

  आभास int32_t next(int32_t n) override;
  आभास int32_t following(int32_t विस्थापनम्) override;
  आभास int32_t चरमः() override;

गुप्त:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t आन्तरिकम्_अग्र(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t आन्तरिकम्_पूर्व(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    शून्य resetState(UError_संहिता &स्थितिः);
    /**
     * Is there a match  (exception) at this spot?
     */
    गणना EFBMatch_परिणाम { k_No_Exception_अत्र, k_Exception_अत्र };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    गणना EFBMatch_परिणाम breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ध्रुव SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_दत्त(other.f_दत्त->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_संहिता &स्थितिः) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,स्थितिः),adopt->getLocale(ULOC_ACTUAL_LOCALE,स्थितिः)),
  f_दत्त(नूतन Simple_Filtered_Sentence_Break_दत्त(forwards, backwards)),
  fDelegate(adopt)
{
    यदि (f_दत्त == शून्यसूचकः) {
        विलोपय forwards;
        विलोपय backwards;
        यदि (U_SUCCESS(स्थितिः)) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_दत्त = f_दत्त->decr();
}

शून्य SimpleFilteredSentenceBreakIterator::resetState(UError_संहिता &स्थितिः) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), स्थितिः));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_परिणाम
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t best_मानम् = -1;
    // loops while 'n' points to an exception.
    utext_समुच्चयः_Native_सूचकांक(fText.getAlias(), n); // from n..

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
        UCharsTrie iter(f_दत्त->getBackwardsTrie());
        UChar32 uch;
        यावत्((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_परिणाम r = iter.अग्र_For_संहिता_Point(uch);
            यदि(USTRINGTRIE_HAS_मानम्(r)) { // remember the best match so far
                bestPosn = utext_get_Native_सूचकांक(fText.getAlias());
                best_मानम् = iter.get_मानम्();
            }
            यदि(!USTRINGTRIE_HAS_अग्र(r)) {
                भङ्ग;
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

      यदि(best_मानम् == k_मेलनम्) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        प्रत्यागम k_Exception_अत्र; // See if the next is another exception.
      } अन्यथा यदि(best_मानम् == kPARTIAL
                && f_दत्त->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_परिणाम rfwd = USTRINGTRIE_INTERMEDIATE_मानम्;
        utext_समुच्चयः_Native_सूचकांक(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_दत्त->getForwardsPartialTrie());
        UChar32 uch;
        यावत्((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_अग्र(rfwd=iter.अग्र_For_संहिता_Point(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        यदि(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            प्रत्यागम k_Exception_अत्र;
        } अन्यथा {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          प्रत्यागम k_No_Exception_अत्र;
        }
      } अन्यथा {
        प्रत्यागम k_No_Exception_अत्र; // internal error and/or no forwards trie
      }
    } अन्यथा {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      प्रत्यागम k_No_Exception_अत्र; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::आन्तरिकम्_अग्र(int32_t n) {
  यदि(n == UBRK_समाप्तम् || // at end  or
    !f_दत्त->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      प्रत्यागम n;
  }
  // OK, do we need to break here?
  UError_संहिता स्थितिः = U_ZERO_ERROR;
  // refresh text
  resetState(स्थितिः);
  यदि(U_FAILURE(स्थितिः)) प्रत्यागम UBRK_समाप्तम्; // bail out
  int64_t utextLen = utext_native_दीर्घता(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  यावत् (n != UBRK_समाप्तम् && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_परिणाम m = breakExceptionAt(n);

    विकल्प(m) {
    पक्ष k_Exception_अत्र:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      अनुवर्त;

    स्वभाव:
    पक्ष k_No_Exception_अत्र:
      प्रत्यागम n;
    }    
  }
  प्रत्यागम n;
}

int32_t
SimpleFilteredSentenceBreakIterator::आन्तरिकम्_पूर्व(int32_t n) {
  यदि(n == 0 || n == UBRK_समाप्तम् || // at end  or
    !f_दत्त->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      प्रत्यागम n;
  }
  // OK, do we need to break here?
  UError_संहिता स्थितिः = U_ZERO_ERROR;
  // refresh text
  resetState(स्थितिः);
  यदि(U_FAILURE(स्थितिः)) प्रत्यागम UBRK_समाप्तम्; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  यावत् (n != UBRK_समाप्तम् && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_परिणाम m = breakExceptionAt(n);

    विकल्प(m) {
    पक्ष k_Exception_अत्र:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      अनुवर्त;

    स्वभाव:
    पक्ष k_No_Exception_अत्र:
      प्रत्यागम n;
    }    
  }
  प्रत्यागम n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  प्रत्यागम आन्तरिकम्_अग्र(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  प्रत्यागम fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t विस्थापनम्) {
  प्रत्यागम आन्तरिकम्_पूर्व(fDelegate->preceding(विस्थापनम्));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  प्रत्यागम आन्तरिकम्_पूर्व(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t विस्थापनम्) {
  यदि (!fDelegate->isBoundary(विस्थापनम्)) प्रत्यागम असत्य; // no break to suppress

  यदि (!f_दत्त->hasBackwardsTrie()) प्रत्यागम सत्य; // no data = no suppressions

  UError_संहिता स्थितिः = U_ZERO_ERROR;
  resetState(स्थितिः);

  SimpleFilteredSentenceBreakIterator::EFBMatch_परिणाम m = breakExceptionAt(विस्थापनम्);

  विकल्प(m) {
  पक्ष k_Exception_अत्र:
    प्रत्यागम असत्य;
  स्वभाव:
  पक्ष k_No_Exception_अत्र:
    प्रत्यागम सत्य;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t विस्थापनम्) {
  प्रत्यागम आन्तरिकम्_अग्र(fDelegate->next(विस्थापनम्));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t विस्थापनम्) {
  प्रत्यागम आन्तरिकम्_अग्र(fDelegate->following(विस्थापनम्));
}

int32_t
SimpleFilteredSentenceBreakIterator::चरमः() {
  // Don't suppress a break opportunity at the end of text.
  प्रत्यागम fDelegate->चरमः();
}


/**
 * Concrete implementation of builder class.
 */
वर्ग SimpleFilteredBreakIteratorBuilder : सार्वजनिक FilteredBreakIteratorBuilder {
सार्वजनिक:
  आभास ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ध्रुव Locale &fromLocale, UError_संहिता &स्थितिः);
  SimpleFilteredBreakIteratorBuilder(UError_संहिता &स्थितिः);
  आभास UBool suppressBreakAfter(ध्रुव UnicodeString& exception, UError_संहिता& स्थितिः) override;
  आभास UBool unsuppressBreakAfter(ध्रुव UnicodeString& exception, UError_संहिता& स्थितिः) override;
  आभास BreakIterator *build(BreakIterator* adoptBreakIterator, UError_संहिता& स्थितिः) override;
गुप्त:
  UString_समुच्चयः f_समुच्चयः;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_संहिता &स्थितिः) 
  : f_समुच्चयः(स्थितिः)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ध्रुव Locale &fromLocale, UError_संहिता &स्थितिः)
  : f_समुच्चयः(स्थितिः)
{
  यदि(U_SUCCESS(स्थितिः)) {
    UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_आधारः_Name(), &sub_स्थितिः));
    यदि (U_FAILURE(sub_स्थितिः) || (sub_स्थितिः == U_USING_DEFAULT_WARNING) ) {    
      स्थितिः = sub_स्थितिः; // copy the failing status 
#यदि FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_आधारः_Name(), "[exit]", u_errorName(स्थितिः));
#यदि_अन्तः
      प्रत्यागम;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_get_By_कुञ्चिका_With_Fallback(b.getAlias(), "exceptions", शून्यसूचकः, &sub_स्थितिः));
    यदि (U_FAILURE(sub_स्थितिः) || (sub_स्थितिः == U_USING_DEFAULT_WARNING) ) {    
      स्थितिः = sub_स्थितिः; // copy the failing status 
#यदि FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_आधारः_Name(), "[exit]", u_errorName(स्थितिः));
#यदि_अन्तः
      प्रत्यागम;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_get_By_कुञ्चिका_With_Fallback(exceptions.getAlias(), "SentenceBreak", शून्यसूचकः, &sub_स्थितिः));

#यदि FB_DEBUG
    {
      UError_संहिता subsub = sub_स्थितिः;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_आधारः_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_स्थितिः));
    }
#यदि_अन्तः
    
    यदि (U_FAILURE(sub_स्थितिः) || (sub_स्थितिः == U_USING_DEFAULT_WARNING) ) {    
      स्थितिः = sub_स्थितिः; // copy the failing status 
#यदि FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_आधारः_Name(), "[exit]", u_errorName(स्थितिः));
#यदि_अन्तः
      प्रत्यागम;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    sub_स्थितिः = स्थितिः; // Pick up inherited warning status now 
    कुरु {
      strs.adoptInstead(ures_get_अग्र_Resource(breaks.getAlias(), strs.orphan(), &sub_स्थितिः));
      यदि(strs.isValid() && U_SUCCESS(sub_स्थितिः)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &स्थितिः));
        suppressBreakAfter(str, स्थितिः); // load the string
      }
    } यावत् (strs.isValid() && U_SUCCESS(sub_स्थितिः));
    यदि(U_FAILURE(sub_स्थितिः)&&sub_स्थितिः!=U_सूचकांक_OUTOFBOUNDS_ERROR&&U_SUCCESS(स्थितिः)) {
      स्थितिः = sub_स्थितिः;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ध्रुव UnicodeString& exception, UError_संहिता& स्थितिः)
{
  UBool r = f_समुच्चयः.सङ्कलय(exception, स्थितिः);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  प्रत्यागम r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ध्रुव UnicodeString& exception, UError_संहिता& स्थितिः)
{
  UBool r = f_समुच्चयः.remove(exception, स्थितिः);
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
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_संहिता& स्थितिः) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(नूतन UCharsTrieBuilder(स्थितिः), स्थितिः);
  LocalPointer<UCharsTrieBuilder> builder2(नूतन UCharsTrieBuilder(स्थितिः), स्थितिः);
  यदि(U_FAILURE(स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }

  int32_t rev_गणनम् = 0;
  int32_t fwd_गणनम् = 0;

  int32_t sub_गणनम् = f_समुच्चयः.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_गणनम्);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_स्मृतिः<पूर्णाङ्क> partials;
  partials.allocateInsteadAndReset(sub_गणनम्);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  पूर्णाङ्क n=0;
  प्रति ( int32_t i = 0;
        i<f_समुच्चयः.size();
        i++) {
    ध्रुव UnicodeString *abbr = f_समुच्चयः.getStringAt(i);
    यदि(abbr) {
      FB_TRACE("build",abbr,सत्य,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],सत्य,i);
    } अन्यथा {
      FB_TRACE("build",abbr,असत्य,i);
      स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
      प्रत्यागम शून्यसूचकः;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  प्रति(पूर्णाङ्क i=0;i<sub_गणनम्;i++) {
    पूर्णाङ्क nn = ustrs[i].सूचकांक_सम्बन्धि(kFULLSTOP); // TODO: non-'.' abbreviations
    यदि(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],असत्य,i);
      // is partial.
      // is it unique?
      पूर्णाङ्क sameAs = -1;
      प्रति(पूर्णाङ्क j=0;j<sub_गणनम्;j++) {
        यदि(j==i) अनुवर्त;
        यदि(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],असत्य,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          यदि(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_अन्तर्_Reverse | k_सङ्कलय_गन्तव्यम्_Forward;
            FB_TRACE("suppressing",&ustrs[j],असत्य,j);
          } अन्यथा यदि(partials[j] & k_Suppress_अन्तर्_Reverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],असत्य,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],असत्य,partials[i]);
      UnicodeString उपसर्गः(ustrs[i], 0, nn+1);
      यदि(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        उपसर्गः.reverse();
        builder->सङ्कलय(उपसर्गः, kPARTIAL, स्थितिः);
        rev_गणनम्++;
        FB_TRACE("Added partial",&उपसर्गः,असत्य, i);
        FB_TRACE(u_errorName(स्थितिः),&ustrs[i],असत्य,i);
        partials[i] = k_Suppress_अन्तर्_Reverse | k_सङ्कलय_गन्तव्यम्_Forward;
      } अन्यथा {
        FB_TRACE("NOT adding partial",&उपसर्गः,असत्य, i);
        FB_TRACE(u_errorName(स्थितिः),&ustrs[i],असत्य,i);
      }
    }
  }
  प्रति(पूर्णाङ्क i=0;i<sub_गणनम्;i++) {
    यदि(partials[i]==0) {
      ustrs[i].reverse();
      builder->सङ्कलय(ustrs[i], k_मेलनम्, स्थितिः);
      rev_गणनम्++;
      FB_TRACE(u_errorName(स्थितिः), &ustrs[i], असत्य, i);
    } अन्यथा {
      FB_TRACE("Adding fwd",&ustrs[i], असत्य, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->सङ्कलय(ustrs[i], k_मेलनम्, स्थितिः); // forward
      fwd_गणनम्++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",शून्यसूचकः,असत्य, sub_गणनम्);

  यदि(rev_गणनम्>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, स्थितिः));
    यदि(U_FAILURE(स्थितिः)) {
      FB_TRACE(u_errorName(स्थितिः),शून्यसूचकः,असत्य, -1);
      प्रत्यागम शून्यसूचकः;
    }
  }

  यदि(fwd_गणनम्>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, स्थितिः));
    यदि(U_FAILURE(स्थितिः)) {
      FB_TRACE(u_errorName(स्थितिः),शून्यसूचकः,असत्य, -1);
      प्रत्यागम शून्यसूचकः;
    }
  }

  प्रत्यागम नूतन SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), स्थितिः);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ध्रुव Locale& where, UError_संहिता& स्थितिः) {
  यदि(U_FAILURE(स्थितिः)) प्रत्यागम शून्यसूचकः;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नूतन SimpleFilteredBreakIteratorBuilder(where, स्थितिः), स्थितिः);
  प्रत्यागम (U_SUCCESS(स्थितिः))? ret.orphan(): शून्यसूचकः;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_संहिता &स्थितिः) {
  प्रत्यागम create_रिक्तानि_Instance(स्थितिः);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_रिक्तानि_Instance(UError_संहिता& स्थितिः) {
  यदि(U_FAILURE(स्थितिः)) प्रत्यागम शून्यसूचकः;
  LocalPointer<FilteredBreakIteratorBuilder> ret(नूतन SimpleFilteredBreakIteratorBuilder(स्थितिः), स्थितिः);
  प्रत्यागम (U_SUCCESS(स्थितिः))? ret.orphan(): शून्यसूचकः;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

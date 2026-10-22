// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#అన్తర్భావ "unicode/utypes.h"
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#అన్తర్భావ "cmemory.h"

#అన్తర్భావ "unicode/filteredbrk.h"
#అన్తర్భావ "unicode/ucharstriebuilder.h"
#అన్తర్భావ "unicode/ures.h"

#అన్తర్భావ "uresimp.h" // ures_getByKeyWithFallback
#అన్తర్భావ "ubrkimpl.h" // U_ICUDATA_BRKITR
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "umutex.h"

U_NAMESPACE_BEGIN

#నిర్వచితం_కాకపోతే FB_DEBUG
#నిర్వచించు FB_DEBUG 0
#అయితే_ముగింపు

#ఒకవేళ FB_DEBUG
#అన్తర్భావ <stdio.h>
స్థిర శూన్య _fb_trace(స్థిరరాశి అక్షరం *m, స్థిరరాశి UnicodeString *s, UBool b, int32_t d, స్థిరరాశి అక్షరం *f, పూర్ణాంకం l) {
  అక్షరం buf[2048];
  ఒకవేళ(s) {
    s->extract(0,s->length(),buf,2048);
  } లేకపోతే {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (స్థిరరాశి శూన్య*)s, b?'T':'F',(పూర్ణాంకం)d);
}

#నిర్వచించు FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#లేకపోతే
#నిర్వచించు FB_TRACE(m,s,b,d)
#అయితే_ముగింపు

/**
 * Used with sortedInsert()
 */
స్థిర int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    స్థిరరాశి UnicodeString& a = *static_cast<స్థిరరాశి UnicodeString*>(t1.సూచిక);
    స్థిరరాశి UnicodeString& b = *static_cast<స్థిరరాశి UnicodeString*>(t2.సూచిక);
    తిరిగి a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
తరగతి UString_సమితి : బహిరంగ UVector {
 బహిరంగ:
  UString_సమితి(UError_సంకేతం &స్థితి_2) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           స్థితి_2) {}
  మాయా ~UString_సమితి();
  /**
   * Is this UnicodeSet contained?
   */
  అంతఃవరుస UBool కలిగియుంది(స్థిరరాశి UnicodeString& s) {
    తిరిగి కలిగియుంది((శూన్య*) &s);
  }
  ఉపయోగించి UVector::కలిగియుంది;
  /**
   * Return the ith UnicodeString alias
   */
  అంతఃవరుస స్థిరరాశి UnicodeString* getStringAt(int32_t i) స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి static_cast<స్థిరరాశి UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  అంతఃవరుస UBool adopt(UnicodeString *str, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ(U_FAILURE(స్థితి_2) || కలిగియుంది(*str)) {
      తొలగించు str;
      తిరిగి అబద్ధం;
    } లేకపోతే {
      sorted_చొప్పించు(str, compareUnicodeString, స్థితి_2);
      ఒకవేళ(U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
      }
      తిరిగి నిజం;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  అంతఃవరుస UBool కూడు(స్థిరరాశి UnicodeString& str, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ(U_FAILURE(స్థితి_2)) తిరిగి అబద్ధం;
    UnicodeString *t = కొత్త UnicodeString(str);
    ఒకవేళ(t==శూన్యసూచిక) {
      స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR; తిరిగి అబద్ధం;
    }
    తిరిగి adopt(t, స్థితి_2);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  అంతఃవరుస UBool remove(స్థిరరాశి UnicodeString &s, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ(U_FAILURE(స్థితి_2)) తిరిగి అబద్ధం;
    తిరిగి removeElement((శూన్య*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_సమితి::~UString_సమితి() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
స్థిర స్థిరరాశి int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
స్థిర స్థిరరాశి int32_t k_సరిపోలిక   = (1<<1); //< exact match - skip this one.
స్థిర స్థిరరాశి int32_t k_Suppress_లోపల_తిరగేయి = (1<<0);
స్థిర స్థిరరాశి int32_t k_కూడు_గమ్యం_Forward = (1<<1);
స్థిర స్థిరరాశి char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
తరగతి Simple_Filtered_Sentence_Break_దత్తాంశం : బహిరంగ UMemory {
బహిరంగ:
  Simple_Filtered_Sentence_Break_దత్తాంశం(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_దత్తాంశం* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        తిరిగి ఇది;
    }
    Simple_Filtered_Sentence_Break_దత్తాంశం *decr() {
        ఒకవేళ(umtx_atomic_dec(&refcount) <= 0) {
            తొలగించు ఇది;
        }
        తిరిగి శూన్యసూచిక;
    }
    మాయా ~Simple_Filtered_Sentence_Break_దత్తాంశం();

    తార్కిక hasForwardsPartialTrie() స్థిరరాశి { తిరిగి fForwardsPartialTrie.isValid(); }
    తార్కిక hasBackwardsTrie() స్థిరరాశి { తిరిగి fBackwardsTrie.isValid(); }

    స్థిరరాశి UCharsTrie& getForwardsPartialTrie() స్థిరరాశి U_LIFETIME_BOUND {
        తిరిగి *fForwardsPartialTrie;
    }
    స్థిరరాశి UCharsTrie& getBackwardsTrie() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి *fBackwardsTrie; }

అంతరంగ:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    Local_సూచిక<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    Local_సూచిక<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_దత్తాంశం::~Simple_Filtered_Sentence_Break_దత్తాంశం() {}

/**
 * Concrete implementation
 */
తరగతి SimpleFilteredSentenceBreakIterator : బహిరంగ BreakIterator {
బహిరంగ:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_సంకేతం &స్థితి_2);
  SimpleFilteredSentenceBreakIterator(స్థిరరాశి SimpleFilteredSentenceBreakIterator& other);
  మాయా ~SimpleFilteredSentenceBreakIterator();
అంతరంగ:
  Simple_Filtered_Sentence_Break_దత్తాంశం *f_దత్తాంశం;
  Local_సూచిక<BreakIterator> fDelegate;
  Local_UText_సూచిక           f_పాఠ్యం;

  /* -- subclass interface -- */
బహిరంగ:
  /* -- cloning and other subclass stuff -- */
  మాయా BreakIterator *  create_బఫరు_Clone(శూన్య * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_సంకేతం &స్థితి_2) override {
    // for now - always deep clone
    స్థితి_2 = U_SAFECLONE_ALLOCATED_WARNING;
    తిరిగి clone();
  }
  మాయా SimpleFilteredSentenceBreakIterator* clone() స్థిరరాశి override { తిరిగి కొత్త SimpleFilteredSentenceBreakIterator(*ఇది); }
  మాయా UClassID getDynamicClassID() స్థిరరాశి override { తిరిగి శూన్యసూచిక; }
  మాయా తార్కిక క్రియాకారకం==(స్థిరరాశి BreakIterator& o) స్థిరరాశి override { ఒకవేళ(ఇది==&o) తిరిగి నిజం; తిరిగి అబద్ధం; }

  /* -- text modifying -- */
  మాయా శూన్య సమితి_పాఠ్యం(UText *పాఠ్యం, UError_సంకేతం &స్థితి_2) override { fDelegate->సమితి_పాఠ్యం(పాఠ్యం,స్థితి_2); }
  మాయా BreakIterator& refresh_ఆదానం_పాఠ్యం(UText* input,
                                          UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND override {
      fDelegate->refresh_ఆదానం_పాఠ్యం(input, స్థితి_2);
      తిరిగి *ఇది;
  }
  మాయా శూన్య adopt_పాఠ్యం(CharacterIterator* it) override { fDelegate->adopt_పాఠ్యం(it); }
  మాయా శూన్య సమితి_పాఠ్యం(స్థిరరాశి UnicodeString &పాఠ్యం) override { fDelegate->సమితి_పాఠ్యం(పాఠ్యం); }

  /* -- other functions that are just delegated -- */
  మాయా UText* getUText(UText* fill_లోపల U_LIFETIME_BOUND, UError_సంకేతం& స్థితి_2) స్థిరరాశి override {
      తిరిగి fDelegate->getUText(fill_లోపల, స్థితి_2);
  }
  మాయా CharacterIterator& get_పాఠ్యం() స్థిరరాశి U_LIFETIME_BOUND override {
      తిరిగి fDelegate->get_పాఠ్యం();
  }

  /* -- ITERATION -- */
  మాయా int32_t first() override;
  మాయా int32_t preceding(int32_t స్థానభ్రంశం) override;
  మాయా int32_t మునుపటి() override;
  మాయా UBool isBoundary(int32_t స్థానభ్రంశం) override;
  మాయా int32_t current() స్థిరరాశి override { తిరిగి fDelegate->current(); } // we keep the delegate current, so this should be correct.

  మాయా int32_t next() override;

  మాయా int32_t next(int32_t n) override;
  మాయా int32_t following(int32_t స్థానభ్రంశం) override;
  మాయా int32_t చివరి() override;

అంతరంగ:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t అంతర్గత_తదుపరి(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t అంతర్గత_Prev(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    శూన్య resetState(UError_సంకేతం &స్థితి_2);
    /**
     * Is there a match  (exception) at this spot?
     */
    గణన EFBMatch_ఫలితం { k_No_Exception_ఇక్కడ, k_Exception_ఇక్కడ };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    గణన EFBMatch_ఫలితం breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(స్థిరరాశి SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_దత్తాంశం(other.f_దత్తాంశం->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_సంకేతం &స్థితి_2) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,స్థితి_2),adopt->getLocale(ULOC_ACTUAL_LOCALE,స్థితి_2)),
  f_దత్తాంశం(కొత్త Simple_Filtered_Sentence_Break_దత్తాంశం(forwards, backwards)),
  fDelegate(adopt)
{
    ఒకవేళ (f_దత్తాంశం == శూన్యసూచిక) {
        తొలగించు forwards;
        తొలగించు backwards;
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_దత్తాంశం = f_దత్తాంశం->decr();
}

శూన్య SimpleFilteredSentenceBreakIterator::resetState(UError_సంకేతం &స్థితి_2) {
  f_పాఠ్యం.adoptInstead(fDelegate->getUText(f_పాఠ్యం.orphan(), స్థితి_2));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_ఫలితం
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t ఉత్తమ_Posn = -1;
    int32_t ఉత్తమ_విలువ = -1;
    // loops while 'n' points to an exception.
    utext_సమితి_Native_సూచీ(f_పాఠ్యం.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    ఒకవేళ(utext_previous32(f_పాఠ్యం.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } లేకపోతే {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(f_పాఠ్యం.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(f_దత్తాంశం->getBackwardsTrie());
        UChar32 uch;
        అయితే((uch=utext_previous32(f_పాఠ్యం.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_ఫలితం r = iter.తదుపరి_For_సంకేతం_బిందువు(uch);
            ఒకవేళ(USTRINGTRIE_HAS_విలువ(r)) { // remember the best match so far
                ఉత్తమ_Posn = utext_get_Native_సూచీ(f_పాఠ్యం.getAlias());
                ఉత్తమ_విలువ = iter.get_విలువ();
            }
            ఒకవేళ(!USTRINGTRIE_HAS_తదుపరి(r)) {
                ఆపు;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    ఒకవేళ(ఉత్తమ_Posn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      ఒకవేళ(ఉత్తమ_విలువ == k_సరిపోలిక) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        తిరిగి k_Exception_ఇక్కడ; // See if the next is another exception.
      } లేకపోతే ఒకవేళ(ఉత్తమ_విలువ == kPARTIAL
                && f_దత్తాంశం->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_ఫలితం rfwd = USTRINGTRIE_INTERMEDIATE_విలువ;
        utext_సమితి_Native_సూచీ(f_పాఠ్యం.getAlias(), ఉత్తమ_Posn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_దత్తాంశం->getForwardsPartialTrie());
        UChar32 uch;
        అయితే((uch=utext_next32(f_పాఠ్యం.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_తదుపరి(rfwd=iter.తదుపరి_For_సంకేతం_బిందువు(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        ఒకవేళ(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            తిరిగి k_Exception_ఇక్కడ;
        } లేకపోతే {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          తిరిగి k_No_Exception_ఇక్కడ;
        }
      } లేకపోతే {
        తిరిగి k_No_Exception_ఇక్కడ; // internal error and/or no forwards trie
      }
    } లేకపోతే {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      తిరిగి k_No_Exception_ఇక్కడ; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::అంతర్గత_తదుపరి(int32_t n) {
  ఒకవేళ(n == UBRK_పూర్తి || // at end  or
    !f_దత్తాంశం->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      తిరిగి n;
  }
  // OK, do we need to break here?
  UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
  // refresh text
  resetState(స్థితి_2);
  ఒకవేళ(U_FAILURE(స్థితి_2)) తిరిగి UBRK_పూర్తి; // bail out
  int64_t utextLen = utext_native_నిడివి(f_పాఠ్యం.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  అయితే (n != UBRK_పూర్తి && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_ఫలితం m = breakExceptionAt(n);

    ఎంపిక(m) {
    స్థితి k_Exception_ఇక్కడ:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      కొనసాగు;

    అప్రమేయ:
    స్థితి k_No_Exception_ఇక్కడ:
      తిరిగి n;
    }    
  }
  తిరిగి n;
}

int32_t
SimpleFilteredSentenceBreakIterator::అంతర్గత_Prev(int32_t n) {
  ఒకవేళ(n == 0 || n == UBRK_పూర్తి || // at end  or
    !f_దత్తాంశం->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      తిరిగి n;
  }
  // OK, do we need to break here?
  UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
  // refresh text
  resetState(స్థితి_2);
  ఒకవేళ(U_FAILURE(స్థితి_2)) తిరిగి UBRK_పూర్తి; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  అయితే (n != UBRK_పూర్తి && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_ఫలితం m = breakExceptionAt(n);

    ఎంపిక(m) {
    స్థితి k_Exception_ఇక్కడ:
      n = fDelegate->మునుపటి(); // skip this one. Find the next lowerlevel break.
      కొనసాగు;

    అప్రమేయ:
    స్థితి k_No_Exception_ఇక్కడ:
      తిరిగి n;
    }    
  }
  తిరిగి n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  తిరిగి అంతర్గత_తదుపరి(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  తిరిగి fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t స్థానభ్రంశం) {
  తిరిగి అంతర్గత_Prev(fDelegate->preceding(స్థానభ్రంశం));
}

int32_t
SimpleFilteredSentenceBreakIterator::మునుపటి() {
  తిరిగి అంతర్గత_Prev(fDelegate->మునుపటి());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t స్థానభ్రంశం) {
  ఒకవేళ (!fDelegate->isBoundary(స్థానభ్రంశం)) తిరిగి అబద్ధం; // no break to suppress

  ఒకవేళ (!f_దత్తాంశం->hasBackwardsTrie()) తిరిగి నిజం; // no data = no suppressions

  UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
  resetState(స్థితి_2);

  SimpleFilteredSentenceBreakIterator::EFBMatch_ఫలితం m = breakExceptionAt(స్థానభ్రంశం);

  ఎంపిక(m) {
  స్థితి k_Exception_ఇక్కడ:
    తిరిగి అబద్ధం;
  అప్రమేయ:
  స్థితి k_No_Exception_ఇక్కడ:
    తిరిగి నిజం;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t స్థానభ్రంశం) {
  తిరిగి అంతర్గత_తదుపరి(fDelegate->next(స్థానభ్రంశం));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t స్థానభ్రంశం) {
  తిరిగి అంతర్గత_తదుపరి(fDelegate->following(స్థానభ్రంశం));
}

int32_t
SimpleFilteredSentenceBreakIterator::చివరి() {
  // Don't suppress a break opportunity at the end of text.
  తిరిగి fDelegate->చివరి();
}


/**
 * Concrete implementation of builder class.
 */
తరగతి SimpleFilteredBreakIteratorBuilder : బహిరంగ FilteredBreakIteratorBuilder {
బహిరంగ:
  మాయా ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(స్థిరరాశి Locale &fromLocale, UError_సంకేతం &స్థితి_2);
  SimpleFilteredBreakIteratorBuilder(UError_సంకేతం &స్థితి_2);
  మాయా UBool suppressBreakAfter(స్థిరరాశి UnicodeString& exception, UError_సంకేతం& స్థితి_2) override;
  మాయా UBool unsuppressBreakAfter(స్థిరరాశి UnicodeString& exception, UError_సంకేతం& స్థితి_2) override;
  మాయా BreakIterator *build(BreakIterator* adoptBreakIterator, UError_సంకేతం& స్థితి_2) override;
అంతరంగ:
  UString_సమితి f_సమితి;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_సంకేతం &స్థితి_2) 
  : f_సమితి(స్థితి_2)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(స్థిరరాశి Locale &fromLocale, UError_సంకేతం &స్థితి_2)
  : f_సమితి(స్థితి_2)
{
  ఒకవేళ(U_SUCCESS(స్థితి_2)) {
    UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
    Local_UResource_Bundle_సూచిక b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_పునాది_Name(), &sub_స్థితి));
    ఒకవేళ (U_FAILURE(sub_స్థితి) || (sub_స్థితి == U_USING_DEFAULT_WARNING) ) {    
      స్థితి_2 = sub_స్థితి; // copy the failing status 
#ఒకవేళ FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_పునాది_Name(), "[exit]", u_errorName(స్థితి_2));
#అయితే_ముగింపు
      తిరిగి;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_సూచిక exceptions(ures_get_By_కీలకం_With_Fallback(b.getAlias(), "exceptions", శూన్యసూచిక, &sub_స్థితి));
    ఒకవేళ (U_FAILURE(sub_స్థితి) || (sub_స్థితి == U_USING_DEFAULT_WARNING) ) {    
      స్థితి_2 = sub_స్థితి; // copy the failing status 
#ఒకవేళ FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_పునాది_Name(), "[exit]", u_errorName(స్థితి_2));
#అయితే_ముగింపు
      తిరిగి;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_సూచిక breaks(ures_get_By_కీలకం_With_Fallback(exceptions.getAlias(), "SentenceBreak", శూన్యసూచిక, &sub_స్థితి));

#ఒకవేళ FB_DEBUG
    {
      UError_సంకేతం subsub = sub_స్థితి;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_పునాది_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_స్థితి));
    }
#అయితే_ముగింపు
    
    ఒకవేళ (U_FAILURE(sub_స్థితి) || (sub_స్థితి == U_USING_DEFAULT_WARNING) ) {    
      స్థితి_2 = sub_స్థితి; // copy the failing status 
#ఒకవేళ FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_పునాది_Name(), "[exit]", u_errorName(స్థితి_2));
#అయితే_ముగింపు
      తిరిగి;  // leaves the builder empty, if you try to use it.
    }

    Local_UResource_Bundle_సూచిక strs;
    sub_స్థితి = స్థితి_2; // Pick up inherited warning status now 
    చేయి {
      strs.adoptInstead(ures_get_తదుపరి_Resource(breaks.getAlias(), strs.orphan(), &sub_స్థితి));
      ఒకవేళ(strs.isValid() && U_SUCCESS(sub_స్థితి)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &స్థితి_2));
        suppressBreakAfter(str, స్థితి_2); // load the string
      }
    } అయితే (strs.isValid() && U_SUCCESS(sub_స్థితి));
    ఒకవేళ(U_FAILURE(sub_స్థితి)&&sub_స్థితి!=U_సూచీ_OUTOFBOUNDS_ERROR&&U_SUCCESS(స్థితి_2)) {
      స్థితి_2 = sub_స్థితి;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(స్థిరరాశి UnicodeString& exception, UError_సంకేతం& స్థితి_2)
{
  UBool r = f_సమితి.కూడు(exception, స్థితి_2);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  తిరిగి r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(స్థిరరాశి UnicodeString& exception, UError_సంకేతం& స్థితి_2)
{
  UBool r = f_సమితి.remove(exception, స్థితి_2);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  తిరిగి r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
స్థిర అంతఃవరుస UnicodeString* newUnicodeStringArray(size_t count) {
    తిరిగి కొత్త UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_సంకేతం& స్థితి_2) {
  Local_సూచిక<BreakIterator> adopt(adoptBreakIterator);

  Local_సూచిక<UCharsTrieBuilder> builder(కొత్త UCharsTrieBuilder(స్థితి_2), స్థితి_2);
  Local_సూచిక<UCharsTrieBuilder> builder2(కొత్త UCharsTrieBuilder(స్థితి_2), స్థితి_2);
  ఒకవేళ(U_FAILURE(స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }

  int32_t rev_లెక్క = 0;
  int32_t fwd_లెక్క = 0;

  int32_t sub_లెక్క = f_సమితి.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_లెక్క);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_జ్ఞాపకం<పూర్ణాంకం> partials;
  partials.allocateInsteadAndReset(sub_లెక్క);

  Local_సూచిక<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  Local_సూచిక<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  పూర్ణాంకం n=0;
  కొరకు ( int32_t i = 0;
        i<f_సమితి.size();
        i++) {
    స్థిరరాశి UnicodeString *abbr = f_సమితి.getStringAt(i);
    ఒకవేళ(abbr) {
      FB_TRACE("build",abbr,నిజం,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],నిజం,i);
    } లేకపోతే {
      FB_TRACE("build",abbr,అబద్ధం,i);
      స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
      తిరిగి శూన్యసూచిక;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  కొరకు(పూర్ణాంకం i=0;i<sub_లెక్క;i++) {
    పూర్ణాంకం nn = ustrs[i].సూచీ_యొక్క(kFULLSTOP); // TODO: non-'.' abbreviations
    ఒకవేళ(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],అబద్ధం,i);
      // is partial.
      // is it unique?
      పూర్ణాంకం sameAs = -1;
      కొరకు(పూర్ణాంకం j=0;j<sub_లెక్క;j++) {
        ఒకవేళ(j==i) కొనసాగు;
        ఒకవేళ(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],అబద్ధం,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          ఒకవేళ(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_లోపల_తిరగేయి | k_కూడు_గమ్యం_Forward;
            FB_TRACE("suppressing",&ustrs[j],అబద్ధం,j);
          } లేకపోతే ఒకవేళ(partials[j] & k_Suppress_లోపల_తిరగేయి) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],అబద్ధం,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],అబద్ధం,partials[i]);
      UnicodeString ఉపసర్గ(ustrs[i], 0, nn+1);
      ఒకవేళ(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        ఉపసర్గ.reverse();
        builder->కూడు(ఉపసర్గ, kPARTIAL, స్థితి_2);
        rev_లెక్క++;
        FB_TRACE("Added partial",&ఉపసర్గ,అబద్ధం, i);
        FB_TRACE(u_errorName(స్థితి_2),&ustrs[i],అబద్ధం,i);
        partials[i] = k_Suppress_లోపల_తిరగేయి | k_కూడు_గమ్యం_Forward;
      } లేకపోతే {
        FB_TRACE("NOT adding partial",&ఉపసర్గ,అబద్ధం, i);
        FB_TRACE(u_errorName(స్థితి_2),&ustrs[i],అబద్ధం,i);
      }
    }
  }
  కొరకు(పూర్ణాంకం i=0;i<sub_లెక్క;i++) {
    ఒకవేళ(partials[i]==0) {
      ustrs[i].reverse();
      builder->కూడు(ustrs[i], k_సరిపోలిక, స్థితి_2);
      rev_లెక్క++;
      FB_TRACE(u_errorName(స్థితి_2), &ustrs[i], అబద్ధం, i);
    } లేకపోతే {
      FB_TRACE("Adding fwd",&ustrs[i], అబద్ధం, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->కూడు(ustrs[i], k_సరిపోలిక, స్థితి_2); // forward
      fwd_లెక్క++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",శూన్యసూచిక,అబద్ధం, sub_లెక్క);

  ఒకవేళ(rev_లెక్క>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, స్థితి_2));
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
      FB_TRACE(u_errorName(స్థితి_2),శూన్యసూచిక,అబద్ధం, -1);
      తిరిగి శూన్యసూచిక;
    }
  }

  ఒకవేళ(fwd_లెక్క>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, స్థితి_2));
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
      FB_TRACE(u_errorName(స్థితి_2),శూన్యసూచిక,అబద్ధం, -1);
      తిరిగి శూన్యసూచిక;
    }
  }

  తిరిగి కొత్త SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), స్థితి_2);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(స్థిరరాశి Locale& where, UError_సంకేతం& స్థితి_2) {
  ఒకవేళ(U_FAILURE(స్థితి_2)) తిరిగి శూన్యసూచిక;
  Local_సూచిక<FilteredBreakIteratorBuilder> ret(కొత్త SimpleFilteredBreakIteratorBuilder(where, స్థితి_2), స్థితి_2);
  తిరిగి (U_SUCCESS(స్థితి_2))? ret.orphan(): శూన్యసూచిక;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_సంకేతం &స్థితి_2) {
  తిరిగి create_ఖాళీలు_Instance(స్థితి_2);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_ఖాళీలు_Instance(UError_సంకేతం& స్థితి_2) {
  ఒకవేళ(U_FAILURE(స్థితి_2)) తిరిగి శూన్యసూచిక;
  Local_సూచిక<FilteredBreakIteratorBuilder> ret(కొత్త SimpleFilteredBreakIteratorBuilder(స్థితి_2), స్థితి_2);
  తిరిగి (U_SUCCESS(స్థితి_2))? ret.orphan(): శూన్యసూచిక;
}

U_NAMESPACE_చివర

#అయితే_ముగింపు //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

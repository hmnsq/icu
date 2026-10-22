// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਜੇ !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/filteredbrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstriebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h" // ures_getByKeyWithFallback
#ਅਨ੍ਤਰ੍ਭਾਵ "ubrkimpl.h" // U_ICUDATA_BRKITR
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

U_NAMESPACE_BEGIN

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FB_DEBUG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FB_DEBUG 0
#ਜੇ_ਅੰਤ

#ਜੇ FB_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
ਸਥਿਰ ਖਾਲੀ _fb_trace(ਅਚਲ ਅੱਖਰ *m, ਅਚਲ UnicodeString *s, UBool b, int32_t d, ਅਚਲ ਅੱਖਰ *f, ਪੂਰਨ l) {
  ਅੱਖਰ buf[2048];
  ਜੇ(s) {
    s->extract(0,s->length(),buf,2048);
  } ਨਹੀਂਤਾਂ {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ਅਚਲ ਖਾਲੀ*)s, b?'T':'F',(ਪੂਰਨ)d);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FB_TRACE(m,s,b,d)
#ਜੇ_ਅੰਤ

/**
 * Used with sortedInsert()
 */
ਸਥਿਰ int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ਅਚਲ UnicodeString& a = *static_cast<ਅਚਲ UnicodeString*>(t1.ਸੂਚਕ);
    ਅਚਲ UnicodeString& b = *static_cast<ਅਚਲ UnicodeString*>(t2.ਸੂਚਕ);
    ਵਾਪਸ a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
ਵਰਗ UString_ਸੈੱਟ : ਜਨਤਕ UVector {
 ਜਨਤਕ:
  UString_ਸੈੱਟ(UError_ਕੋਡ &ਸਥਿਤੀ) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           ਸਥਿਤੀ) {}
  ਆਭਾਸੀ ~UString_ਸੈੱਟ();
  /**
   * Is this UnicodeSet contained?
   */
  ਸਤਰਅੰਦਰ UBool ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ UnicodeString& s) {
    ਵਾਪਸ ਸ਼ਾਮਲ_ਹੈ((ਖਾਲੀ*) &s);
  }
  ਵਰਤੋਂ UVector::ਸ਼ਾਮਲ_ਹੈ;
  /**
   * Return the ith UnicodeString alias
   */
  ਸਤਰਅੰਦਰ ਅਚਲ UnicodeString* getStringAt(int32_t i) ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ static_cast<ਅਚਲ UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  ਸਤਰਅੰਦਰ UBool adopt(UnicodeString *str, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ(U_FAILURE(ਸਥਿਤੀ) || ਸ਼ਾਮਲ_ਹੈ(*str)) {
      ਮਿਟਾਓ str;
      ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
      sorted_ਪਾਓ(str, compareUnicodeString, ਸਥਿਤੀ);
      ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
      }
      ਵਾਪਸ ਸੱਚ;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  ਸਤਰਅੰਦਰ UBool ਜੋੜੋ(ਅਚਲ UnicodeString& str, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ ਝੂਠ;
    UnicodeString *t = ਨਵਾਂ UnicodeString(str);
    ਜੇ(t==ਖਾਲੀ_ਸੂਚਕ) {
      ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR; ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ adopt(t, ਸਥਿਤੀ);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  ਸਤਰਅੰਦਰ UBool remove(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ ਝੂਠ;
    ਵਾਪਸ removeElement((ਖਾਲੀ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_ਸੈੱਟ::~UString_ਸੈੱਟ() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ਸਥਿਰ ਅਚਲ int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ਸਥਿਰ ਅਚਲ int32_t k_ਮੇਲ   = (1<<1); //< exact match - skip this one.
ਸਥਿਰ ਅਚਲ int32_t k_Suppress_ਵਿੱਚ_ਉਲਟ = (1<<0);
ਸਥਿਰ ਅਚਲ int32_t k_ਜੋੜੋ_ਮੰਜ਼ਿਲ_Forward = (1<<1);
ਸਥਿਰ ਅਚਲ char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
ਵਰਗ Simple_Filtered_Sentence_Break_ਅੰਕੜੇ : ਜਨਤਕ UMemory {
ਜਨਤਕ:
  Simple_Filtered_Sentence_Break_ਅੰਕੜੇ(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_ਅੰਕੜੇ* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        ਵਾਪਸ ਇਹ;
    }
    Simple_Filtered_Sentence_Break_ਅੰਕੜੇ *decr() {
        ਜੇ(umtx_atomic_dec(&refcount) <= 0) {
            ਮਿਟਾਓ ਇਹ;
        }
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਆਭਾਸੀ ~Simple_Filtered_Sentence_Break_ਅੰਕੜੇ();

    ਤਾਰਕਿਕ hasForwardsPartialTrie() ਅਚਲ { ਵਾਪਸ fForwardsPartialTrie.isValid(); }
    ਤਾਰਕਿਕ hasBackwardsTrie() ਅਚਲ { ਵਾਪਸ fBackwardsTrie.isValid(); }

    ਅਚਲ UCharsTrie& getForwardsPartialTrie() ਅਚਲ U_LIFETIME_BOUND {
        ਵਾਪਸ *fForwardsPartialTrie;
    }
    ਅਚਲ UCharsTrie& getBackwardsTrie() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ *fBackwardsTrie; }

ਨਿਜੀ:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    Local_ਸੂਚਕ<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    Local_ਸੂਚਕ<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_ਅੰਕੜੇ::~Simple_Filtered_Sentence_Break_ਅੰਕੜੇ() {}

/**
 * Concrete implementation
 */
ਵਰਗ SimpleFilteredSentenceBreakIterator : ਜਨਤਕ BreakIterator {
ਜਨਤਕ:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_ਕੋਡ &ਸਥਿਤੀ);
  SimpleFilteredSentenceBreakIterator(ਅਚਲ SimpleFilteredSentenceBreakIterator& other);
  ਆਭਾਸੀ ~SimpleFilteredSentenceBreakIterator();
ਨਿਜੀ:
  Simple_Filtered_Sentence_Break_ਅੰਕੜੇ *f_ਅੰਕੜੇ;
  Local_ਸੂਚਕ<BreakIterator> fDelegate;
  Local_UText_ਸੂਚਕ           f_ਪਾਠ;

  /* -- subclass interface -- */
ਜਨਤਕ:
  /* -- cloning and other subclass stuff -- */
  ਆਭਾਸੀ BreakIterator *  create_ਬਫਰ_Clone(ਖਾਲੀ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_ਕੋਡ &ਸਥਿਤੀ) override {
    // for now - always deep clone
    ਸਥਿਤੀ = U_SAFECLONE_ALLOCATED_WARNING;
    ਵਾਪਸ clone();
  }
  ਆਭਾਸੀ SimpleFilteredSentenceBreakIterator* clone() ਅਚਲ override { ਵਾਪਸ ਨਵਾਂ SimpleFilteredSentenceBreakIterator(*ਇਹ); }
  ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
  ਆਭਾਸੀ ਤਾਰਕਿਕ ਸੰਚਾਲਕ==(ਅਚਲ BreakIterator& o) ਅਚਲ override { ਜੇ(ਇਹ==&o) ਵਾਪਸ ਸੱਚ; ਵਾਪਸ ਝੂਠ; }

  /* -- text modifying -- */
  ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਪਾਠ(UText *ਪਾਠ, UError_ਕੋਡ &ਸਥਿਤੀ) override { fDelegate->ਸੈੱਟ_ਪਾਠ(ਪਾਠ,ਸਥਿਤੀ); }
  ਆਭਾਸੀ BreakIterator& refresh_ਨਿਵੇਸ਼_ਪਾਠ(UText* input,
                                          UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND override {
      fDelegate->refresh_ਨਿਵੇਸ਼_ਪਾਠ(input, ਸਥਿਤੀ);
      ਵਾਪਸ *ਇਹ;
  }
  ਆਭਾਸੀ ਖਾਲੀ adopt_ਪਾਠ(CharacterIterator* it) override { fDelegate->adopt_ਪਾਠ(it); }
  ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਪਾਠ(ਅਚਲ UnicodeString &ਪਾਠ) override { fDelegate->ਸੈੱਟ_ਪਾਠ(ਪਾਠ); }

  /* -- other functions that are just delegated -- */
  ਆਭਾਸੀ UText* getUText(UText* fill_ਵਿੱਚ U_LIFETIME_BOUND, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override {
      ਵਾਪਸ fDelegate->getUText(fill_ਵਿੱਚ, ਸਥਿਤੀ);
  }
  ਆਭਾਸੀ CharacterIterator& get_ਪਾਠ() ਅਚਲ U_LIFETIME_BOUND override {
      ਵਾਪਸ fDelegate->get_ਪਾਠ();
  }

  /* -- ITERATION -- */
  ਆਭਾਸੀ int32_t first() override;
  ਆਭਾਸੀ int32_t preceding(int32_t ਵਿੱਥ) override;
  ਆਭਾਸੀ int32_t ਪਿਛਲਾ() override;
  ਆਭਾਸੀ UBool isBoundary(int32_t ਵਿੱਥ) override;
  ਆਭਾਸੀ int32_t current() ਅਚਲ override { ਵਾਪਸ fDelegate->current(); } // we keep the delegate current, so this should be correct.

  ਆਭਾਸੀ int32_t next() override;

  ਆਭਾਸੀ int32_t next(int32_t n) override;
  ਆਭਾਸੀ int32_t following(int32_t ਵਿੱਥ) override;
  ਆਭਾਸੀ int32_t ਆਖਰੀ() override;

ਨਿਜੀ:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t ਅੰਦਰੂਨੀ_ਅਗਲਾ(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t ਅੰਦਰੂਨੀ_Prev(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    ਖਾਲੀ resetState(UError_ਕੋਡ &ਸਥਿਤੀ);
    /**
     * Is there a match  (exception) at this spot?
     */
    ਗਿਣਤੀ EFBMatch_ਨਤੀਜਾ { k_No_Exception_ਇੱਥੇ, k_Exception_ਇੱਥੇ };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    ਗਿਣਤੀ EFBMatch_ਨਤੀਜਾ breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ਅਚਲ SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_ਅੰਕੜੇ(other.f_ਅੰਕੜੇ->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_ਕੋਡ &ਸਥਿਤੀ) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,ਸਥਿਤੀ),adopt->getLocale(ULOC_ACTUAL_LOCALE,ਸਥਿਤੀ)),
  f_ਅੰਕੜੇ(ਨਵਾਂ Simple_Filtered_Sentence_Break_ਅੰਕੜੇ(forwards, backwards)),
  fDelegate(adopt)
{
    ਜੇ (f_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        ਮਿਟਾਓ forwards;
        ਮਿਟਾਓ backwards;
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_ਅੰਕੜੇ = f_ਅੰਕੜੇ->decr();
}

ਖਾਲੀ SimpleFilteredSentenceBreakIterator::resetState(UError_ਕੋਡ &ਸਥਿਤੀ) {
  f_ਪਾਠ.adoptInstead(fDelegate->getUText(f_ਪਾਠ.orphan(), ਸਥਿਤੀ));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_ਨਤੀਜਾ
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t ਉੱਤਮ_Posn = -1;
    int32_t ਉੱਤਮ_ਮੁੱਲ = -1;
    // loops while 'n' points to an exception.
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(f_ਪਾਠ.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    ਜੇ(utext_previous32(f_ਪਾਠ.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } ਨਹੀਂਤਾਂ {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(f_ਪਾਠ.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(f_ਅੰਕੜੇ->getBackwardsTrie());
        UChar32 uch;
        ਜਦੋਂ((uch=utext_previous32(f_ਪਾਠ.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_ਨਤੀਜਾ r = iter.ਅਗਲਾ_For_ਕੋਡ_ਬਿੰਦੂ(uch);
            ਜੇ(USTRINGTRIE_HAS_ਮੁੱਲ(r)) { // remember the best match so far
                ਉੱਤਮ_Posn = utext_get_Native_ਸੂਚਕਾਂਕ(f_ਪਾਠ.getAlias());
                ਉੱਤਮ_ਮੁੱਲ = iter.get_ਮੁੱਲ();
            }
            ਜੇ(!USTRINGTRIE_HAS_ਅਗਲਾ(r)) {
                ਰੋਕੋ;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    ਜੇ(ਉੱਤਮ_Posn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      ਜੇ(ਉੱਤਮ_ਮੁੱਲ == k_ਮੇਲ) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        ਵਾਪਸ k_Exception_ਇੱਥੇ; // See if the next is another exception.
      } ਨਹੀਂਤਾਂ ਜੇ(ਉੱਤਮ_ਮੁੱਲ == kPARTIAL
                && f_ਅੰਕੜੇ->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_ਨਤੀਜਾ rfwd = USTRINGTRIE_INTERMEDIATE_ਮੁੱਲ;
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(f_ਪਾਠ.getAlias(), ਉੱਤਮ_Posn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_ਅੰਕੜੇ->getForwardsPartialTrie());
        UChar32 uch;
        ਜਦੋਂ((uch=utext_next32(f_ਪਾਠ.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_ਅਗਲਾ(rfwd=iter.ਅਗਲਾ_For_ਕੋਡ_ਬਿੰਦੂ(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        ਜੇ(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            ਵਾਪਸ k_Exception_ਇੱਥੇ;
        } ਨਹੀਂਤਾਂ {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          ਵਾਪਸ k_No_Exception_ਇੱਥੇ;
        }
      } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ k_No_Exception_ਇੱਥੇ; // internal error and/or no forwards trie
      }
    } ਨਹੀਂਤਾਂ {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      ਵਾਪਸ k_No_Exception_ਇੱਥੇ; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::ਅੰਦਰੂਨੀ_ਅਗਲਾ(int32_t n) {
  ਜੇ(n == UBRK_ਪੂਰਾ || // at end  or
    !f_ਅੰਕੜੇ->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ਵਾਪਸ n;
  }
  // OK, do we need to break here?
  UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
  // refresh text
  resetState(ਸਥਿਤੀ);
  ਜੇ(U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ UBRK_ਪੂਰਾ; // bail out
  int64_t utextLen = utext_native_ਲੰਮਾਈ(f_ਪਾਠ.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ਜਦੋਂ (n != UBRK_ਪੂਰਾ && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_ਨਤੀਜਾ m = breakExceptionAt(n);

    ਚੋਣ(m) {
    ਹਾਲਤ k_Exception_ਇੱਥੇ:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      ਜਾਰੀ;

    ਮੂਲ:
    ਹਾਲਤ k_No_Exception_ਇੱਥੇ:
      ਵਾਪਸ n;
    }    
  }
  ਵਾਪਸ n;
}

int32_t
SimpleFilteredSentenceBreakIterator::ਅੰਦਰੂਨੀ_Prev(int32_t n) {
  ਜੇ(n == 0 || n == UBRK_ਪੂਰਾ || // at end  or
    !f_ਅੰਕੜੇ->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ਵਾਪਸ n;
  }
  // OK, do we need to break here?
  UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
  // refresh text
  resetState(ਸਥਿਤੀ);
  ਜੇ(U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ UBRK_ਪੂਰਾ; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ਜਦੋਂ (n != UBRK_ਪੂਰਾ && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_ਨਤੀਜਾ m = breakExceptionAt(n);

    ਚੋਣ(m) {
    ਹਾਲਤ k_Exception_ਇੱਥੇ:
      n = fDelegate->ਪਿਛਲਾ(); // skip this one. Find the next lowerlevel break.
      ਜਾਰੀ;

    ਮੂਲ:
    ਹਾਲਤ k_No_Exception_ਇੱਥੇ:
      ਵਾਪਸ n;
    }    
  }
  ਵਾਪਸ n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  ਵਾਪਸ ਅੰਦਰੂਨੀ_ਅਗਲਾ(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  ਵਾਪਸ fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t ਵਿੱਥ) {
  ਵਾਪਸ ਅੰਦਰੂਨੀ_Prev(fDelegate->preceding(ਵਿੱਥ));
}

int32_t
SimpleFilteredSentenceBreakIterator::ਪਿਛਲਾ() {
  ਵਾਪਸ ਅੰਦਰੂਨੀ_Prev(fDelegate->ਪਿਛਲਾ());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t ਵਿੱਥ) {
  ਜੇ (!fDelegate->isBoundary(ਵਿੱਥ)) ਵਾਪਸ ਝੂਠ; // no break to suppress

  ਜੇ (!f_ਅੰਕੜੇ->hasBackwardsTrie()) ਵਾਪਸ ਸੱਚ; // no data = no suppressions

  UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
  resetState(ਸਥਿਤੀ);

  SimpleFilteredSentenceBreakIterator::EFBMatch_ਨਤੀਜਾ m = breakExceptionAt(ਵਿੱਥ);

  ਚੋਣ(m) {
  ਹਾਲਤ k_Exception_ਇੱਥੇ:
    ਵਾਪਸ ਝੂਠ;
  ਮੂਲ:
  ਹਾਲਤ k_No_Exception_ਇੱਥੇ:
    ਵਾਪਸ ਸੱਚ;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t ਵਿੱਥ) {
  ਵਾਪਸ ਅੰਦਰੂਨੀ_ਅਗਲਾ(fDelegate->next(ਵਿੱਥ));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t ਵਿੱਥ) {
  ਵਾਪਸ ਅੰਦਰੂਨੀ_ਅਗਲਾ(fDelegate->following(ਵਿੱਥ));
}

int32_t
SimpleFilteredSentenceBreakIterator::ਆਖਰੀ() {
  // Don't suppress a break opportunity at the end of text.
  ਵਾਪਸ fDelegate->ਆਖਰੀ();
}


/**
 * Concrete implementation of builder class.
 */
ਵਰਗ SimpleFilteredBreakIteratorBuilder : ਜਨਤਕ FilteredBreakIteratorBuilder {
ਜਨਤਕ:
  ਆਭਾਸੀ ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ਅਚਲ Locale &fromLocale, UError_ਕੋਡ &ਸਥਿਤੀ);
  SimpleFilteredBreakIteratorBuilder(UError_ਕੋਡ &ਸਥਿਤੀ);
  ਆਭਾਸੀ UBool suppressBreakAfter(ਅਚਲ UnicodeString& exception, UError_ਕੋਡ& ਸਥਿਤੀ) override;
  ਆਭਾਸੀ UBool unsuppressBreakAfter(ਅਚਲ UnicodeString& exception, UError_ਕੋਡ& ਸਥਿਤੀ) override;
  ਆਭਾਸੀ BreakIterator *build(BreakIterator* adoptBreakIterator, UError_ਕੋਡ& ਸਥਿਤੀ) override;
ਨਿਜੀ:
  UString_ਸੈੱਟ f_ਸੈੱਟ;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_ਕੋਡ &ਸਥਿਤੀ) 
  : f_ਸੈੱਟ(ਸਥਿਤੀ)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ਅਚਲ Locale &fromLocale, UError_ਕੋਡ &ਸਥਿਤੀ)
  : f_ਸੈੱਟ(ਸਥਿਤੀ)
{
  ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
    UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
    Local_UResource_Bundle_ਸੂਚਕ b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_ਅਧਾਰ_Name(), &sub_ਸਥਿਤੀ));
    ਜੇ (U_FAILURE(sub_ਸਥਿਤੀ) || (sub_ਸਥਿਤੀ == U_USING_DEFAULT_WARNING) ) {    
      ਸਥਿਤੀ = sub_ਸਥਿਤੀ; // copy the failing status 
#ਜੇ FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_ਅਧਾਰ_Name(), "[exit]", u_errorName(ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
      ਵਾਪਸ;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_ਸੂਚਕ exceptions(ures_get_By_ਕੁੰਜੀ_With_Fallback(b.getAlias(), "exceptions", ਖਾਲੀ_ਸੂਚਕ, &sub_ਸਥਿਤੀ));
    ਜੇ (U_FAILURE(sub_ਸਥਿਤੀ) || (sub_ਸਥਿਤੀ == U_USING_DEFAULT_WARNING) ) {    
      ਸਥਿਤੀ = sub_ਸਥਿਤੀ; // copy the failing status 
#ਜੇ FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_ਅਧਾਰ_Name(), "[exit]", u_errorName(ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
      ਵਾਪਸ;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_ਸੂਚਕ breaks(ures_get_By_ਕੁੰਜੀ_With_Fallback(exceptions.getAlias(), "SentenceBreak", ਖਾਲੀ_ਸੂਚਕ, &sub_ਸਥਿਤੀ));

#ਜੇ FB_DEBUG
    {
      UError_ਕੋਡ subsub = sub_ਸਥਿਤੀ;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_ਅਧਾਰ_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_ਸਥਿਤੀ));
    }
#ਜੇ_ਅੰਤ
    
    ਜੇ (U_FAILURE(sub_ਸਥਿਤੀ) || (sub_ਸਥਿਤੀ == U_USING_DEFAULT_WARNING) ) {    
      ਸਥਿਤੀ = sub_ਸਥਿਤੀ; // copy the failing status 
#ਜੇ FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_ਅਧਾਰ_Name(), "[exit]", u_errorName(ਸਥਿਤੀ));
#ਜੇ_ਅੰਤ
      ਵਾਪਸ;  // leaves the builder empty, if you try to use it.
    }

    Local_UResource_Bundle_ਸੂਚਕ strs;
    sub_ਸਥਿਤੀ = ਸਥਿਤੀ; // Pick up inherited warning status now 
    ਕਰੋ {
      strs.adoptInstead(ures_get_ਅਗਲਾ_Resource(breaks.getAlias(), strs.orphan(), &sub_ਸਥਿਤੀ));
      ਜੇ(strs.isValid() && U_SUCCESS(sub_ਸਥਿਤੀ)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &ਸਥਿਤੀ));
        suppressBreakAfter(str, ਸਥਿਤੀ); // load the string
      }
    } ਜਦੋਂ (strs.isValid() && U_SUCCESS(sub_ਸਥਿਤੀ));
    ਜੇ(U_FAILURE(sub_ਸਥਿਤੀ)&&sub_ਸਥਿਤੀ!=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR&&U_SUCCESS(ਸਥਿਤੀ)) {
      ਸਥਿਤੀ = sub_ਸਥਿਤੀ;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ਅਚਲ UnicodeString& exception, UError_ਕੋਡ& ਸਥਿਤੀ)
{
  UBool r = f_ਸੈੱਟ.ਜੋੜੋ(exception, ਸਥਿਤੀ);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  ਵਾਪਸ r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ਅਚਲ UnicodeString& exception, UError_ਕੋਡ& ਸਥਿਤੀ)
{
  UBool r = f_ਸੈੱਟ.remove(exception, ਸਥਿਤੀ);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  ਵਾਪਸ r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ UnicodeString* newUnicodeStringArray(size_t count) {
    ਵਾਪਸ ਨਵਾਂ UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_ਕੋਡ& ਸਥਿਤੀ) {
  Local_ਸੂਚਕ<BreakIterator> adopt(adoptBreakIterator);

  Local_ਸੂਚਕ<UCharsTrieBuilder> builder(ਨਵਾਂ UCharsTrieBuilder(ਸਥਿਤੀ), ਸਥਿਤੀ);
  Local_ਸੂਚਕ<UCharsTrieBuilder> builder2(ਨਵਾਂ UCharsTrieBuilder(ਸਥਿਤੀ), ਸਥਿਤੀ);
  ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  int32_t rev_ਗਿਣਤੀ = 0;
  int32_t fwd_ਗਿਣਤੀ = 0;

  int32_t sub_ਗਿਣਤੀ = f_ਸੈੱਟ.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_ਗਿਣਤੀ);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_ਸਮ੍ਰਿਤੀ<ਪੂਰਨ> partials;
  partials.allocateInsteadAndReset(sub_ਗਿਣਤੀ);

  Local_ਸੂਚਕ<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  Local_ਸੂਚਕ<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  ਪੂਰਨ n=0;
  ਲਈ ( int32_t i = 0;
        i<f_ਸੈੱਟ.size();
        i++) {
    ਅਚਲ UnicodeString *abbr = f_ਸੈੱਟ.getStringAt(i);
    ਜੇ(abbr) {
      FB_TRACE("build",abbr,ਸੱਚ,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],ਸੱਚ,i);
    } ਨਹੀਂਤਾਂ {
      FB_TRACE("build",abbr,ਝੂਠ,i);
      ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  ਲਈ(ਪੂਰਨ i=0;i<sub_ਗਿਣਤੀ;i++) {
    ਪੂਰਨ nn = ustrs[i].ਸੂਚਕਾਂਕ_ਦਾ(kFULLSTOP); // TODO: non-'.' abbreviations
    ਜੇ(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],ਝੂਠ,i);
      // is partial.
      // is it unique?
      ਪੂਰਨ sameAs = -1;
      ਲਈ(ਪੂਰਨ j=0;j<sub_ਗਿਣਤੀ;j++) {
        ਜੇ(j==i) ਜਾਰੀ;
        ਜੇ(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],ਝੂਠ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          ਜੇ(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_ਵਿੱਚ_ਉਲਟ | k_ਜੋੜੋ_ਮੰਜ਼ਿਲ_Forward;
            FB_TRACE("suppressing",&ustrs[j],ਝੂਠ,j);
          } ਨਹੀਂਤਾਂ ਜੇ(partials[j] & k_Suppress_ਵਿੱਚ_ਉਲਟ) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],ਝੂਠ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],ਝੂਠ,partials[i]);
      UnicodeString ਅਗੇਤਰ(ustrs[i], 0, nn+1);
      ਜੇ(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        ਅਗੇਤਰ.reverse();
        builder->ਜੋੜੋ(ਅਗੇਤਰ, kPARTIAL, ਸਥਿਤੀ);
        rev_ਗਿਣਤੀ++;
        FB_TRACE("Added partial",&ਅਗੇਤਰ,ਝੂਠ, i);
        FB_TRACE(u_errorName(ਸਥਿਤੀ),&ustrs[i],ਝੂਠ,i);
        partials[i] = k_Suppress_ਵਿੱਚ_ਉਲਟ | k_ਜੋੜੋ_ਮੰਜ਼ਿਲ_Forward;
      } ਨਹੀਂਤਾਂ {
        FB_TRACE("NOT adding partial",&ਅਗੇਤਰ,ਝੂਠ, i);
        FB_TRACE(u_errorName(ਸਥਿਤੀ),&ustrs[i],ਝੂਠ,i);
      }
    }
  }
  ਲਈ(ਪੂਰਨ i=0;i<sub_ਗਿਣਤੀ;i++) {
    ਜੇ(partials[i]==0) {
      ustrs[i].reverse();
      builder->ਜੋੜੋ(ustrs[i], k_ਮੇਲ, ਸਥਿਤੀ);
      rev_ਗਿਣਤੀ++;
      FB_TRACE(u_errorName(ਸਥਿਤੀ), &ustrs[i], ਝੂਠ, i);
    } ਨਹੀਂਤਾਂ {
      FB_TRACE("Adding fwd",&ustrs[i], ਝੂਠ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->ਜੋੜੋ(ustrs[i], k_ਮੇਲ, ਸਥਿਤੀ); // forward
      fwd_ਗਿਣਤੀ++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",ਖਾਲੀ_ਸੂਚਕ,ਝੂਠ, sub_ਗਿਣਤੀ);

  ਜੇ(rev_ਗਿਣਤੀ>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, ਸਥਿਤੀ));
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
      FB_TRACE(u_errorName(ਸਥਿਤੀ),ਖਾਲੀ_ਸੂਚਕ,ਝੂਠ, -1);
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
  }

  ਜੇ(fwd_ਗਿਣਤੀ>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, ਸਥਿਤੀ));
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
      FB_TRACE(u_errorName(ਸਥਿਤੀ),ਖਾਲੀ_ਸੂਚਕ,ਝੂਠ, -1);
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
  }

  ਵਾਪਸ ਨਵਾਂ SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), ਸਥਿਤੀ);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ਅਚਲ Locale& where, UError_ਕੋਡ& ਸਥਿਤੀ) {
  ਜੇ(U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  Local_ਸੂਚਕ<FilteredBreakIteratorBuilder> ret(ਨਵਾਂ SimpleFilteredBreakIteratorBuilder(where, ਸਥਿਤੀ), ਸਥਿਤੀ);
  ਵਾਪਸ (U_SUCCESS(ਸਥਿਤੀ))? ret.orphan(): ਖਾਲੀ_ਸੂਚਕ;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_ਕੋਡ &ਸਥਿਤੀ) {
  ਵਾਪਸ create_ਖਾਲੀ_ਥਾਵਾਂ_Instance(ਸਥਿਤੀ);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_ਖਾਲੀ_ਥਾਵਾਂ_Instance(UError_ਕੋਡ& ਸਥਿਤੀ) {
  ਜੇ(U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  Local_ਸੂਚਕ<FilteredBreakIteratorBuilder> ret(ਨਵਾਂ SimpleFilteredBreakIteratorBuilder(ਸਥਿਤੀ), ਸਥਿਤੀ);
  ਵਾਪਸ (U_SUCCESS(ਸਥਿਤੀ))? ret.orphan(): ਖਾਲੀ_ਸੂਚਕ;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

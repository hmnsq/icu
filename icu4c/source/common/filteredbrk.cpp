// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"
#إذا !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

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

#جيڪڏهن_وصف_نه_ٿيل FB_DEBUG
#وصف_ڪريو FB_DEBUG 0
#جيڪڏهن_پورو

#إذا FB_DEBUG
#ضمن <stdio.h>
ساكن فراغ _fb_trace(ثابت حرف *m, ثابت UnicodeString *s, UBool b, int32_t d, ثابت حرف *f, صحيح l) {
  حرف buf[2048];
  إذا(s) {
    s->extract(0,s->length(),buf,2048);
  } وإلا {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ثابت فراغ*)s, b?'T':'F',(صحيح)d);
}

#وصف_ڪريو FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#وإلا
#وصف_ڪريو FB_TRACE(m,s,b,d)
#جيڪڏهن_پورو

/**
 * Used with sortedInsert()
 */
ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشارو);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشارو);
    أرجع a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
صنف UString_سيٽ : عام UVector {
 عام:
  UString_سيٽ(UError_ڪوڊ &حالت) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           حالت) {}
  مجازي ~UString_سيٽ();
  /**
   * Is this UnicodeSet contained?
   */
  مضمن UBool شامل_آهي(ثابت UnicodeString& s) {
    أرجع شامل_آهي((فراغ*) &s);
  }
  استعمال UVector::شامل_آهي;
  /**
   * Return the ith UnicodeString alias
   */
  مضمن ثابت UnicodeString* getStringAt(int32_t i) ثابت U_LIFETIME_BOUND {
    أرجع static_cast<ثابت UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  مضمن UBool adopt(UnicodeString *str, UError_ڪوڊ &حالت) {
    إذا(U_FAILURE(حالت) || شامل_آهي(*str)) {
      ميٽيو str;
      أرجع خطأ;
    } وإلا {
      sorted_وجهو(str, compareUnicodeString, حالت);
      إذا(U_FAILURE(حالت)) {
        أرجع خطأ;
      }
      أرجع سچ;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  مضمن UBool جوڙيو(ثابت UnicodeString& str, UError_ڪوڊ &حالت) {
    إذا(U_FAILURE(حالت)) أرجع خطأ;
    UnicodeString *t = جديد UnicodeString(str);
    إذا(t==خالي_اشارو) {
      حالت = U_يادگيري_ورهاست_ERROR; أرجع خطأ;
    }
    أرجع adopt(t, حالت);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  مضمن UBool remove(ثابت UnicodeString &s, UError_ڪوڊ &حالت) {
    إذا(U_FAILURE(حالت)) أرجع خطأ;
    أرجع removeElement((فراغ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_سيٽ::~UString_سيٽ() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ساكن ثابت int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ساكن ثابت int32_t k_ميلاپ   = (1<<1); //< exact match - skip this one.
ساكن ثابت int32_t k_Suppress_اندريون_ابتو = (1<<0);
ساكن ثابت int32_t k_جوڙيو_منزل_Forward = (1<<1);
ساكن ثابت char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
صنف Simple_Filtered_Sentence_Break_معلومات : عام UMemory {
عام:
  Simple_Filtered_Sentence_Break_معلومات(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_معلومات* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        أرجع هذا;
    }
    Simple_Filtered_Sentence_Break_معلومات *decr() {
        إذا(umtx_atomic_dec(&refcount) <= 0) {
            ميٽيو هذا;
        }
        أرجع خالي_اشارو;
    }
    مجازي ~Simple_Filtered_Sentence_Break_معلومات();

    منطقي hasForwardsPartialTrie() ثابت { أرجع fForwardsPartialTrie.isValid(); }
    منطقي hasBackwardsTrie() ثابت { أرجع fBackwardsTrie.isValid(); }

    ثابت UCharsTrie& getForwardsPartialTrie() ثابت U_LIFETIME_BOUND {
        أرجع *fForwardsPartialTrie;
    }
    ثابت UCharsTrie& getBackwardsTrie() ثابت U_LIFETIME_BOUND { أرجع *fBackwardsTrie; }

خاص:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    Local_اشارو<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    Local_اشارو<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_معلومات::~Simple_Filtered_Sentence_Break_معلومات() {}

/**
 * Concrete implementation
 */
صنف SimpleFilteredSentenceBreakIterator : عام BreakIterator {
عام:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_ڪوڊ &حالت);
  SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other);
  مجازي ~SimpleFilteredSentenceBreakIterator();
خاص:
  Simple_Filtered_Sentence_Break_معلومات *f_معلومات;
  Local_اشارو<BreakIterator> fDelegate;
  Local_UText_اشارو           f_متن;

  /* -- subclass interface -- */
عام:
  /* -- cloning and other subclass stuff -- */
  مجازي BreakIterator *  create_بفر_Clone(فراغ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_ڪوڊ &حالت) override {
    // for now - always deep clone
    حالت = U_SAFECLONE_ALLOCATED_WARNING;
    أرجع clone();
  }
  مجازي SimpleFilteredSentenceBreakIterator* clone() ثابت override { أرجع جديد SimpleFilteredSentenceBreakIterator(*هذا); }
  مجازي UClassID getDynamicClassID() ثابت override { أرجع خالي_اشارو; }
  مجازي منطقي عامل==(ثابت BreakIterator& o) ثابت override { إذا(هذا==&o) أرجع سچ; أرجع خطأ; }

  /* -- text modifying -- */
  مجازي فراغ سيٽ_متن(UText *متن, UError_ڪوڊ &حالت) override { fDelegate->سيٽ_متن(متن,حالت); }
  مجازي BreakIterator& refresh_داخلا_متن(UText* input,
                                          UError_ڪوڊ& حالت) U_LIFETIME_BOUND override {
      fDelegate->refresh_داخلا_متن(input, حالت);
      أرجع *هذا;
  }
  مجازي فراغ adopt_متن(CharacterIterator* it) override { fDelegate->adopt_متن(it); }
  مجازي فراغ سيٽ_متن(ثابت UnicodeString &متن) override { fDelegate->سيٽ_متن(متن); }

  /* -- other functions that are just delegated -- */
  مجازي UText* getUText(UText* fill_اندريون U_LIFETIME_BOUND, UError_ڪوڊ& حالت) ثابت override {
      أرجع fDelegate->getUText(fill_اندريون, حالت);
  }
  مجازي CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override {
      أرجع fDelegate->get_متن();
  }

  /* -- ITERATION -- */
  مجازي int32_t first() override;
  مجازي int32_t preceding(int32_t وٿي) override;
  مجازي int32_t اڳيون() override;
  مجازي UBool isBoundary(int32_t وٿي) override;
  مجازي int32_t current() ثابت override { أرجع fDelegate->current(); } // we keep the delegate current, so this should be correct.

  مجازي int32_t next() override;

  مجازي int32_t next(int32_t n) override;
  مجازي int32_t following(int32_t وٿي) override;
  مجازي int32_t آخري() override;

خاص:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t اندروني_ايندڙ(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t اندروني_Prev(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    فراغ resetState(UError_ڪوڊ &حالت);
    /**
     * Is there a match  (exception) at this spot?
     */
    تعداد EFBMatch_نتيجو { k_No_Exception_هتي, k_Exception_هتي };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    تعداد EFBMatch_نتيجو breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_معلومات(other.f_معلومات->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_ڪوڊ &حالت) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,حالت),adopt->getLocale(ULOC_ACTUAL_LOCALE,حالت)),
  f_معلومات(جديد Simple_Filtered_Sentence_Break_معلومات(forwards, backwards)),
  fDelegate(adopt)
{
    إذا (f_معلومات == خالي_اشارو) {
        ميٽيو forwards;
        ميٽيو backwards;
        إذا (U_SUCCESS(حالت)) {
            حالت = U_يادگيري_ورهاست_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_معلومات = f_معلومات->decr();
}

فراغ SimpleFilteredSentenceBreakIterator::resetState(UError_ڪوڊ &حالت) {
  f_متن.adoptInstead(fDelegate->getUText(f_متن.orphan(), حالت));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_نتيجو
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t بهترين_Posn = -1;
    int32_t بهترين_قدر = -1;
    // loops while 'n' points to an exception.
    utext_سيٽ_Native_اشاريو(f_متن.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    إذا(utext_previous32(f_متن.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } وإلا {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(f_متن.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(f_معلومات->getBackwardsTrie());
        UChar32 uch;
        طالما((uch=utext_previous32(f_متن.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_نتيجو r = iter.ايندڙ_For_ڪوڊ_نقطو(uch);
            إذا(USTRINGTRIE_HAS_قدر(r)) { // remember the best match so far
                بهترين_Posn = utext_get_Native_اشاريو(f_متن.getAlias());
                بهترين_قدر = iter.get_قدر();
            }
            إذا(!USTRINGTRIE_HAS_ايندڙ(r)) {
                اكسر;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    إذا(بهترين_Posn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      إذا(بهترين_قدر == k_ميلاپ) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        أرجع k_Exception_هتي; // See if the next is another exception.
      } وإلا إذا(بهترين_قدر == kPARTIAL
                && f_معلومات->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_نتيجو rfwd = USTRINGTRIE_INTERMEDIATE_قدر;
        utext_سيٽ_Native_اشاريو(f_متن.getAlias(), بهترين_Posn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_معلومات->getForwardsPartialTrie());
        UChar32 uch;
        طالما((uch=utext_next32(f_متن.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_ايندڙ(rfwd=iter.ايندڙ_For_ڪوڊ_نقطو(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        إذا(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            أرجع k_Exception_هتي;
        } وإلا {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          أرجع k_No_Exception_هتي;
        }
      } وإلا {
        أرجع k_No_Exception_هتي; // internal error and/or no forwards trie
      }
    } وإلا {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      أرجع k_No_Exception_هتي; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::اندروني_ايندڙ(int32_t n) {
  إذا(n == UBRK_پورو || // at end  or
    !f_معلومات->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      أرجع n;
  }
  // OK, do we need to break here?
  UError_ڪوڊ حالت = U_ZERO_ERROR;
  // refresh text
  resetState(حالت);
  إذا(U_FAILURE(حالت)) أرجع UBRK_پورو; // bail out
  int64_t utextLen = utext_native_ڊيگهائي(f_متن.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  طالما (n != UBRK_پورو && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_نتيجو m = breakExceptionAt(n);

    بدل(m) {
    حالة k_Exception_هتي:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة k_No_Exception_هتي:
      أرجع n;
    }    
  }
  أرجع n;
}

int32_t
SimpleFilteredSentenceBreakIterator::اندروني_Prev(int32_t n) {
  إذا(n == 0 || n == UBRK_پورو || // at end  or
    !f_معلومات->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      أرجع n;
  }
  // OK, do we need to break here?
  UError_ڪوڊ حالت = U_ZERO_ERROR;
  // refresh text
  resetState(حالت);
  إذا(U_FAILURE(حالت)) أرجع UBRK_پورو; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  طالما (n != UBRK_پورو && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_نتيجو m = breakExceptionAt(n);

    بدل(m) {
    حالة k_Exception_هتي:
      n = fDelegate->اڳيون(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة k_No_Exception_هتي:
      أرجع n;
    }    
  }
  أرجع n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  أرجع اندروني_ايندڙ(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  أرجع fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t وٿي) {
  أرجع اندروني_Prev(fDelegate->preceding(وٿي));
}

int32_t
SimpleFilteredSentenceBreakIterator::اڳيون() {
  أرجع اندروني_Prev(fDelegate->اڳيون());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t وٿي) {
  إذا (!fDelegate->isBoundary(وٿي)) أرجع خطأ; // no break to suppress

  إذا (!f_معلومات->hasBackwardsTrie()) أرجع سچ; // no data = no suppressions

  UError_ڪوڊ حالت = U_ZERO_ERROR;
  resetState(حالت);

  SimpleFilteredSentenceBreakIterator::EFBMatch_نتيجو m = breakExceptionAt(وٿي);

  بدل(m) {
  حالة k_Exception_هتي:
    أرجع خطأ;
  افتراضي:
  حالة k_No_Exception_هتي:
    أرجع سچ;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t وٿي) {
  أرجع اندروني_ايندڙ(fDelegate->next(وٿي));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t وٿي) {
  أرجع اندروني_ايندڙ(fDelegate->following(وٿي));
}

int32_t
SimpleFilteredSentenceBreakIterator::آخري() {
  // Don't suppress a break opportunity at the end of text.
  أرجع fDelegate->آخري();
}


/**
 * Concrete implementation of builder class.
 */
صنف SimpleFilteredBreakIteratorBuilder : عام FilteredBreakIteratorBuilder {
عام:
  مجازي ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_ڪوڊ &حالت);
  SimpleFilteredBreakIteratorBuilder(UError_ڪوڊ &حالت);
  مجازي UBool suppressBreakAfter(ثابت UnicodeString& exception, UError_ڪوڊ& حالت) override;
  مجازي UBool unsuppressBreakAfter(ثابت UnicodeString& exception, UError_ڪوڊ& حالت) override;
  مجازي BreakIterator *build(BreakIterator* adoptBreakIterator, UError_ڪوڊ& حالت) override;
خاص:
  UString_سيٽ f_سيٽ;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_ڪوڊ &حالت) 
  : f_سيٽ(حالت)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_ڪوڊ &حالت)
  : f_سيٽ(حالت)
{
  إذا(U_SUCCESS(حالت)) {
    UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارو b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_بنياد_Name(), &sub_حالت));
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_بنياد_Name(), "[exit]", u_errorName(حالت));
#جيڪڏهن_پورو
      أرجع;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشارو exceptions(ures_get_By_ڪنجي_With_Fallback(b.getAlias(), "exceptions", خالي_اشارو, &sub_حالت));
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_بنياد_Name(), "[exit]", u_errorName(حالت));
#جيڪڏهن_پورو
      أرجع;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشارو breaks(ures_get_By_ڪنجي_With_Fallback(exceptions.getAlias(), "SentenceBreak", خالي_اشارو, &sub_حالت));

#إذا FB_DEBUG
    {
      UError_ڪوڊ subsub = sub_حالت;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_بنياد_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_حالت));
    }
#جيڪڏهن_پورو
    
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_بنياد_Name(), "[exit]", u_errorName(حالت));
#جيڪڏهن_پورو
      أرجع;  // leaves the builder empty, if you try to use it.
    }

    Local_UResource_Bundle_اشارو strs;
    sub_حالت = حالت; // Pick up inherited warning status now 
    افعل {
      strs.adoptInstead(ures_get_ايندڙ_Resource(breaks.getAlias(), strs.orphan(), &sub_حالت));
      إذا(strs.isValid() && U_SUCCESS(sub_حالت)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &حالت));
        suppressBreakAfter(str, حالت); // load the string
      }
    } طالما (strs.isValid() && U_SUCCESS(sub_حالت));
    إذا(U_FAILURE(sub_حالت)&&sub_حالت!=U_اشاريو_OUTOFBOUNDS_ERROR&&U_SUCCESS(حالت)) {
      حالت = sub_حالت;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ثابت UnicodeString& exception, UError_ڪوڊ& حالت)
{
  UBool r = f_سيٽ.جوڙيو(exception, حالت);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  أرجع r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ثابت UnicodeString& exception, UError_ڪوڊ& حالت)
{
  UBool r = f_سيٽ.remove(exception, حالت);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  أرجع r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
ساكن مضمن UnicodeString* newUnicodeStringArray(size_t count) {
    أرجع جديد UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_ڪوڊ& حالت) {
  Local_اشارو<BreakIterator> adopt(adoptBreakIterator);

  Local_اشارو<UCharsTrieBuilder> builder(جديد UCharsTrieBuilder(حالت), حالت);
  Local_اشارو<UCharsTrieBuilder> builder2(جديد UCharsTrieBuilder(حالت), حالت);
  إذا(U_FAILURE(حالت)) {
    أرجع خالي_اشارو;
  }

  int32_t rev_ڳڻپ = 0;
  int32_t fwd_ڳڻپ = 0;

  int32_t sub_ڳڻپ = f_سيٽ.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_ڳڻپ);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_يادگيري<صحيح> partials;
  partials.allocateInsteadAndReset(sub_ڳڻپ);

  Local_اشارو<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  Local_اشارو<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  صحيح n=0;
  لكل ( int32_t i = 0;
        i<f_سيٽ.size();
        i++) {
    ثابت UnicodeString *abbr = f_سيٽ.getStringAt(i);
    إذا(abbr) {
      FB_TRACE("build",abbr,سچ,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],سچ,i);
    } وإلا {
      FB_TRACE("build",abbr,خطأ,i);
      حالت = U_يادگيري_ورهاست_ERROR;
      أرجع خالي_اشارو;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  لكل(صحيح i=0;i<sub_ڳڻپ;i++) {
    صحيح nn = ustrs[i].اشاريو_جو(kFULLSTOP); // TODO: non-'.' abbreviations
    إذا(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],خطأ,i);
      // is partial.
      // is it unique?
      صحيح sameAs = -1;
      لكل(صحيح j=0;j<sub_ڳڻپ;j++) {
        إذا(j==i) استمر;
        إذا(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],خطأ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          إذا(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_اندريون_ابتو | k_جوڙيو_منزل_Forward;
            FB_TRACE("suppressing",&ustrs[j],خطأ,j);
          } وإلا إذا(partials[j] & k_Suppress_اندريون_ابتو) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],خطأ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],خطأ,partials[i]);
      UnicodeString اڳياڙي(ustrs[i], 0, nn+1);
      إذا(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        اڳياڙي.reverse();
        builder->جوڙيو(اڳياڙي, kPARTIAL, حالت);
        rev_ڳڻپ++;
        FB_TRACE("Added partial",&اڳياڙي,خطأ, i);
        FB_TRACE(u_errorName(حالت),&ustrs[i],خطأ,i);
        partials[i] = k_Suppress_اندريون_ابتو | k_جوڙيو_منزل_Forward;
      } وإلا {
        FB_TRACE("NOT adding partial",&اڳياڙي,خطأ, i);
        FB_TRACE(u_errorName(حالت),&ustrs[i],خطأ,i);
      }
    }
  }
  لكل(صحيح i=0;i<sub_ڳڻپ;i++) {
    إذا(partials[i]==0) {
      ustrs[i].reverse();
      builder->جوڙيو(ustrs[i], k_ميلاپ, حالت);
      rev_ڳڻپ++;
      FB_TRACE(u_errorName(حالت), &ustrs[i], خطأ, i);
    } وإلا {
      FB_TRACE("Adding fwd",&ustrs[i], خطأ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->جوڙيو(ustrs[i], k_ميلاپ, حالت); // forward
      fwd_ڳڻپ++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",خالي_اشارو,خطأ, sub_ڳڻپ);

  إذا(rev_ڳڻپ>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, حالت));
    إذا(U_FAILURE(حالت)) {
      FB_TRACE(u_errorName(حالت),خالي_اشارو,خطأ, -1);
      أرجع خالي_اشارو;
    }
  }

  إذا(fwd_ڳڻپ>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, حالت));
    إذا(U_FAILURE(حالت)) {
      FB_TRACE(u_errorName(حالت),خالي_اشارو,خطأ, -1);
      أرجع خالي_اشارو;
    }
  }

  أرجع جديد SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), حالت);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ثابت Locale& where, UError_ڪوڊ& حالت) {
  إذا(U_FAILURE(حالت)) أرجع خالي_اشارو;
  Local_اشارو<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(where, حالت), حالت);
  أرجع (U_SUCCESS(حالت))? ret.orphan(): خالي_اشارو;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_ڪوڊ &حالت) {
  أرجع create_خالي_جايون_Instance(حالت);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_خالي_جايون_Instance(UError_ڪوڊ& حالت) {
  إذا(U_FAILURE(حالت)) أرجع خالي_اشارو;
  Local_اشارو<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(حالت), حالت);
  أرجع (U_SUCCESS(حالت))? ret.orphan(): خالي_اشارو;
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

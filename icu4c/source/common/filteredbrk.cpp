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

#که_نه_تعریف_شوی FB_DEBUG
#تعریف_کړه FB_DEBUG 0
#د_که_پای

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

#تعریف_کړه FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#وإلا
#تعریف_کړه FB_TRACE(m,s,b,d)
#د_که_پای

/**
 * Used with sortedInsert()
 */
ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشاره_ګر);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشاره_ګر);
    أرجع a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
صنف UString_سېټ : عام UVector {
 عام:
  UString_سېټ(UError_کوډ &حالت) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           حالت) {}
  مجازي ~UString_سېټ();
  /**
   * Is this UnicodeSet contained?
   */
  مضمن UBool لري(ثابت UnicodeString& s) {
    أرجع لري((فراغ*) &s);
  }
  کارول UVector::لري;
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
  مضمن UBool adopt(UnicodeString *str, UError_کوډ &حالت) {
    إذا(U_FAILURE(حالت) || لري(*str)) {
      ړنګول str;
      أرجع خطأ;
    } وإلا {
      sorted_ورننباسه(str, compareUnicodeString, حالت);
      إذا(U_FAILURE(حالت)) {
        أرجع خطأ;
      }
      أرجع ریښتیا;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  مضمن UBool جمع_کړه(ثابت UnicodeString& str, UError_کوډ &حالت) {
    إذا(U_FAILURE(حالت)) أرجع خطأ;
    UnicodeString *t = جديد UnicodeString(str);
    إذا(t==تش_اشاره) {
      حالت = U_حافظه_ویش_ERROR; أرجع خطأ;
    }
    أرجع adopt(t, حالت);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  مضمن UBool remove(ثابت UnicodeString &s, UError_کوډ &حالت) {
    إذا(U_FAILURE(حالت)) أرجع خطأ;
    أرجع removeElement((فراغ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_سېټ::~UString_سېټ() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ساكن ثابت int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ساكن ثابت int32_t k_سمون   = (1<<1); //< exact match - skip this one.
ساكن ثابت int32_t k_Suppress_دننه_سرچپه = (1<<0);
ساكن ثابت int32_t k_جمع_کړه_منزل_Forward = (1<<1);
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
            ړنګول هذا;
        }
        أرجع تش_اشاره;
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
    Local_اشاره_ګر<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    Local_اشاره_ګر<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_معلومات::~Simple_Filtered_Sentence_Break_معلومات() {}

/**
 * Concrete implementation
 */
صنف SimpleFilteredSentenceBreakIterator : عام BreakIterator {
عام:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_کوډ &حالت);
  SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other);
  مجازي ~SimpleFilteredSentenceBreakIterator();
خاص:
  Simple_Filtered_Sentence_Break_معلومات *f_معلومات;
  Local_اشاره_ګر<BreakIterator> fDelegate;
  Local_UText_اشاره_ګر           f_متن;

  /* -- subclass interface -- */
عام:
  /* -- cloning and other subclass stuff -- */
  مجازي BreakIterator *  create_بفر_Clone(فراغ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_کوډ &حالت) override {
    // for now - always deep clone
    حالت = U_SAFECLONE_ALLOCATED_WARNING;
    أرجع clone();
  }
  مجازي SimpleFilteredSentenceBreakIterator* clone() ثابت override { أرجع جديد SimpleFilteredSentenceBreakIterator(*هذا); }
  مجازي UClassID getDynamicClassID() ثابت override { أرجع تش_اشاره; }
  مجازي منطقي چلوونکی==(ثابت BreakIterator& o) ثابت override { إذا(هذا==&o) أرجع ریښتیا; أرجع خطأ; }

  /* -- text modifying -- */
  مجازي فراغ سېټ_متن(UText *متن, UError_کوډ &حالت) override { fDelegate->سېټ_متن(متن,حالت); }
  مجازي BreakIterator& refresh_ننوت_متن(UText* input,
                                          UError_کوډ& حالت) U_LIFETIME_BOUND override {
      fDelegate->refresh_ننوت_متن(input, حالت);
      أرجع *هذا;
  }
  مجازي فراغ adopt_متن(CharacterIterator* it) override { fDelegate->adopt_متن(it); }
  مجازي فراغ سېټ_متن(ثابت UnicodeString &متن) override { fDelegate->سېټ_متن(متن); }

  /* -- other functions that are just delegated -- */
  مجازي UText* getUText(UText* fill_دننه U_LIFETIME_BOUND, UError_کوډ& حالت) ثابت override {
      أرجع fDelegate->getUText(fill_دننه, حالت);
  }
  مجازي CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override {
      أرجع fDelegate->get_متن();
  }

  /* -- ITERATION -- */
  مجازي int32_t first() override;
  مجازي int32_t preceding(int32_t دننه_واټن) override;
  مجازي int32_t پخوانی() override;
  مجازي UBool isBoundary(int32_t دننه_واټن) override;
  مجازي int32_t current() ثابت override { أرجع fDelegate->current(); } // we keep the delegate current, so this should be correct.

  مجازي int32_t next() override;

  مجازي int32_t next(int32_t n) override;
  مجازي int32_t following(int32_t دننه_واټن) override;
  مجازي int32_t وروستی() override;

خاص:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t دننی_بل(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t دننی_Prev(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    فراغ resetState(UError_کوډ &حالت);
    /**
     * Is there a match  (exception) at this spot?
     */
    تعداد EFBMatch_پایله { k_No_Exception_دلته, k_Exception_دلته };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    تعداد EFBMatch_پایله breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_معلومات(other.f_معلومات->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_کوډ &حالت) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,حالت),adopt->getLocale(ULOC_ACTUAL_LOCALE,حالت)),
  f_معلومات(جديد Simple_Filtered_Sentence_Break_معلومات(forwards, backwards)),
  fDelegate(adopt)
{
    إذا (f_معلومات == تش_اشاره) {
        ړنګول forwards;
        ړنګول backwards;
        إذا (U_SUCCESS(حالت)) {
            حالت = U_حافظه_ویش_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_معلومات = f_معلومات->decr();
}

فراغ SimpleFilteredSentenceBreakIterator::resetState(UError_کوډ &حالت) {
  f_متن.adoptInstead(fDelegate->getUText(f_متن.orphan(), حالت));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_پایله
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t غوره_Posn = -1;
    int32_t غوره_ارزښت = -1;
    // loops while 'n' points to an exception.
    utext_سېټ_Native_شاخص(f_متن.getAlias(), n); // from n..

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
            UString_Trie_پایله r = iter.بل_For_کوډ_ټکی(uch);
            إذا(USTRINGTRIE_HAS_ارزښت(r)) { // remember the best match so far
                غوره_Posn = utext_get_Native_شاخص(f_متن.getAlias());
                غوره_ارزښت = iter.get_ارزښت();
            }
            إذا(!USTRINGTRIE_HAS_بل(r)) {
                اكسر;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    إذا(غوره_Posn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      إذا(غوره_ارزښت == k_سمون) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        أرجع k_Exception_دلته; // See if the next is another exception.
      } وإلا إذا(غوره_ارزښت == kPARTIAL
                && f_معلومات->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_پایله rfwd = USTRINGTRIE_INTERMEDIATE_ارزښت;
        utext_سېټ_Native_شاخص(f_متن.getAlias(), غوره_Posn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_معلومات->getForwardsPartialTrie());
        UChar32 uch;
        طالما((uch=utext_next32(f_متن.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_بل(rfwd=iter.بل_For_کوډ_ټکی(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        إذا(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            أرجع k_Exception_دلته;
        } وإلا {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          أرجع k_No_Exception_دلته;
        }
      } وإلا {
        أرجع k_No_Exception_دلته; // internal error and/or no forwards trie
      }
    } وإلا {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      أرجع k_No_Exception_دلته; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::دننی_بل(int32_t n) {
  إذا(n == UBRK_بشپړ || // at end  or
    !f_معلومات->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      أرجع n;
  }
  // OK, do we need to break here?
  UError_کوډ حالت = U_ZERO_ERROR;
  // refresh text
  resetState(حالت);
  إذا(U_FAILURE(حالت)) أرجع UBRK_بشپړ; // bail out
  int64_t utextLen = utext_native_اوږدتیا(f_متن.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  طالما (n != UBRK_بشپړ && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_پایله m = breakExceptionAt(n);

    بدل(m) {
    حالة k_Exception_دلته:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة k_No_Exception_دلته:
      أرجع n;
    }    
  }
  أرجع n;
}

int32_t
SimpleFilteredSentenceBreakIterator::دننی_Prev(int32_t n) {
  إذا(n == 0 || n == UBRK_بشپړ || // at end  or
    !f_معلومات->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      أرجع n;
  }
  // OK, do we need to break here?
  UError_کوډ حالت = U_ZERO_ERROR;
  // refresh text
  resetState(حالت);
  إذا(U_FAILURE(حالت)) أرجع UBRK_بشپړ; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  طالما (n != UBRK_بشپړ && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_پایله m = breakExceptionAt(n);

    بدل(m) {
    حالة k_Exception_دلته:
      n = fDelegate->پخوانی(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة k_No_Exception_دلته:
      أرجع n;
    }    
  }
  أرجع n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  أرجع دننی_بل(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  أرجع fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t دننه_واټن) {
  أرجع دننی_Prev(fDelegate->preceding(دننه_واټن));
}

int32_t
SimpleFilteredSentenceBreakIterator::پخوانی() {
  أرجع دننی_Prev(fDelegate->پخوانی());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t دننه_واټن) {
  إذا (!fDelegate->isBoundary(دننه_واټن)) أرجع خطأ; // no break to suppress

  إذا (!f_معلومات->hasBackwardsTrie()) أرجع ریښتیا; // no data = no suppressions

  UError_کوډ حالت = U_ZERO_ERROR;
  resetState(حالت);

  SimpleFilteredSentenceBreakIterator::EFBMatch_پایله m = breakExceptionAt(دننه_واټن);

  بدل(m) {
  حالة k_Exception_دلته:
    أرجع خطأ;
  افتراضي:
  حالة k_No_Exception_دلته:
    أرجع ریښتیا;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t دننه_واټن) {
  أرجع دننی_بل(fDelegate->next(دننه_واټن));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t دننه_واټن) {
  أرجع دننی_بل(fDelegate->following(دننه_واټن));
}

int32_t
SimpleFilteredSentenceBreakIterator::وروستی() {
  // Don't suppress a break opportunity at the end of text.
  أرجع fDelegate->وروستی();
}


/**
 * Concrete implementation of builder class.
 */
صنف SimpleFilteredBreakIteratorBuilder : عام FilteredBreakIteratorBuilder {
عام:
  مجازي ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_کوډ &حالت);
  SimpleFilteredBreakIteratorBuilder(UError_کوډ &حالت);
  مجازي UBool suppressBreakAfter(ثابت UnicodeString& exception, UError_کوډ& حالت) override;
  مجازي UBool unsuppressBreakAfter(ثابت UnicodeString& exception, UError_کوډ& حالت) override;
  مجازي BreakIterator *build(BreakIterator* adoptBreakIterator, UError_کوډ& حالت) override;
خاص:
  UString_سېټ f_سېټ;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_کوډ &حالت) 
  : f_سېټ(حالت)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_کوډ &حالت)
  : f_سېټ(حالت)
{
  إذا(U_SUCCESS(حالت)) {
    UError_کوډ sub_حالت = U_ZERO_ERROR;
    Local_UResource_Bundle_اشاره_ګر b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_بنسټ_Name(), &sub_حالت));
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_بنسټ_Name(), "[exit]", u_errorName(حالت));
#د_که_پای
      أرجع;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشاره_ګر exceptions(ures_get_By_کلی_With_Fallback(b.getAlias(), "exceptions", تش_اشاره, &sub_حالت));
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_بنسټ_Name(), "[exit]", u_errorName(حالت));
#د_که_پای
      أرجع;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشاره_ګر breaks(ures_get_By_کلی_With_Fallback(exceptions.getAlias(), "SentenceBreak", تش_اشاره, &sub_حالت));

#إذا FB_DEBUG
    {
      UError_کوډ subsub = sub_حالت;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_بنسټ_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_حالت));
    }
#د_که_پای
    
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_بنسټ_Name(), "[exit]", u_errorName(حالت));
#د_که_پای
      أرجع;  // leaves the builder empty, if you try to use it.
    }

    Local_UResource_Bundle_اشاره_ګر strs;
    sub_حالت = حالت; // Pick up inherited warning status now 
    افعل {
      strs.adoptInstead(ures_get_بل_Resource(breaks.getAlias(), strs.orphan(), &sub_حالت));
      إذا(strs.isValid() && U_SUCCESS(sub_حالت)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &حالت));
        suppressBreakAfter(str, حالت); // load the string
      }
    } طالما (strs.isValid() && U_SUCCESS(sub_حالت));
    إذا(U_FAILURE(sub_حالت)&&sub_حالت!=U_شاخص_OUTOFBOUNDS_ERROR&&U_SUCCESS(حالت)) {
      حالت = sub_حالت;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ثابت UnicodeString& exception, UError_کوډ& حالت)
{
  UBool r = f_سېټ.جمع_کړه(exception, حالت);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  أرجع r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ثابت UnicodeString& exception, UError_کوډ& حالت)
{
  UBool r = f_سېټ.remove(exception, حالت);
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
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_کوډ& حالت) {
  Local_اشاره_ګر<BreakIterator> adopt(adoptBreakIterator);

  Local_اشاره_ګر<UCharsTrieBuilder> builder(جديد UCharsTrieBuilder(حالت), حالت);
  Local_اشاره_ګر<UCharsTrieBuilder> builder2(جديد UCharsTrieBuilder(حالت), حالت);
  إذا(U_FAILURE(حالت)) {
    أرجع تش_اشاره;
  }

  int32_t rev_شمېر = 0;
  int32_t fwd_شمېر = 0;

  int32_t sub_شمېر = f_سېټ.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_شمېر);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_حافظه<صحيح> partials;
  partials.allocateInsteadAndReset(sub_شمېر);

  Local_اشاره_ګر<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  Local_اشاره_ګر<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  صحيح n=0;
  لكل ( int32_t i = 0;
        i<f_سېټ.size();
        i++) {
    ثابت UnicodeString *abbr = f_سېټ.getStringAt(i);
    إذا(abbr) {
      FB_TRACE("build",abbr,ریښتیا,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],ریښتیا,i);
    } وإلا {
      FB_TRACE("build",abbr,خطأ,i);
      حالت = U_حافظه_ویش_ERROR;
      أرجع تش_اشاره;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  لكل(صحيح i=0;i<sub_شمېر;i++) {
    صحيح nn = ustrs[i].شاخص_د(kFULLSTOP); // TODO: non-'.' abbreviations
    إذا(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],خطأ,i);
      // is partial.
      // is it unique?
      صحيح sameAs = -1;
      لكل(صحيح j=0;j<sub_شمېر;j++) {
        إذا(j==i) استمر;
        إذا(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],خطأ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          إذا(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_دننه_سرچپه | k_جمع_کړه_منزل_Forward;
            FB_TRACE("suppressing",&ustrs[j],خطأ,j);
          } وإلا إذا(partials[j] & k_Suppress_دننه_سرچپه) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],خطأ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],خطأ,partials[i]);
      UnicodeString مختاړی(ustrs[i], 0, nn+1);
      إذا(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        مختاړی.reverse();
        builder->جمع_کړه(مختاړی, kPARTIAL, حالت);
        rev_شمېر++;
        FB_TRACE("Added partial",&مختاړی,خطأ, i);
        FB_TRACE(u_errorName(حالت),&ustrs[i],خطأ,i);
        partials[i] = k_Suppress_دننه_سرچپه | k_جمع_کړه_منزل_Forward;
      } وإلا {
        FB_TRACE("NOT adding partial",&مختاړی,خطأ, i);
        FB_TRACE(u_errorName(حالت),&ustrs[i],خطأ,i);
      }
    }
  }
  لكل(صحيح i=0;i<sub_شمېر;i++) {
    إذا(partials[i]==0) {
      ustrs[i].reverse();
      builder->جمع_کړه(ustrs[i], k_سمون, حالت);
      rev_شمېر++;
      FB_TRACE(u_errorName(حالت), &ustrs[i], خطأ, i);
    } وإلا {
      FB_TRACE("Adding fwd",&ustrs[i], خطأ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->جمع_کړه(ustrs[i], k_سمون, حالت); // forward
      fwd_شمېر++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",تش_اشاره,خطأ, sub_شمېر);

  إذا(rev_شمېر>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, حالت));
    إذا(U_FAILURE(حالت)) {
      FB_TRACE(u_errorName(حالت),تش_اشاره,خطأ, -1);
      أرجع تش_اشاره;
    }
  }

  إذا(fwd_شمېر>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, حالت));
    إذا(U_FAILURE(حالت)) {
      FB_TRACE(u_errorName(حالت),تش_اشاره,خطأ, -1);
      أرجع تش_اشاره;
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
FilteredBreakIteratorBuilder::createInstance(ثابت Locale& where, UError_کوډ& حالت) {
  إذا(U_FAILURE(حالت)) أرجع تش_اشاره;
  Local_اشاره_ګر<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(where, حالت), حالت);
  أرجع (U_SUCCESS(حالت))? ret.orphan(): تش_اشاره;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_کوډ &حالت) {
  أرجع create_تشه_Instance(حالت);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_تشه_Instance(UError_کوډ& حالت) {
  إذا(U_FAILURE(حالت)) أرجع تش_اشاره;
  Local_اشاره_ګر<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(حالت), حالت);
  أرجع (U_SUCCESS(حالت))? ret.orphan(): تش_اشاره;
}

U_NAMESPACE_پای_ټکی

#د_که_پای //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

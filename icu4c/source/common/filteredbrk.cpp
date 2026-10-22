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

#جے_بنا_تعریف FB_DEBUG
#تعریف_کرو FB_DEBUG 0
#جے_انت

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

#تعریف_کرو FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#وإلا
#تعریف_کرو FB_TRACE(m,s,b,d)
#جے_انت

/**
 * Used with sortedInsert()
 */
ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشارہ_گر);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشارہ_گر);
    أرجع a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
صنف UString_سیٹ : عام UVector {
 عام:
  UString_سیٹ(UError_کوڈ &حالت) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           حالت) {}
  مجازی ~UString_سیٹ();
  /**
   * Is this UnicodeSet contained?
   */
  مضمن UBool شامل_اے(ثابت UnicodeString& s) {
    أرجع شامل_اے((فراغ*) &s);
  }
  ورتوں UVector::شامل_اے;
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
  مضمن UBool adopt(UnicodeString *str, UError_کوڈ &حالت) {
    إذا(U_FAILURE(حالت) || شامل_اے(*str)) {
      مٹاؤ str;
      أرجع خطأ;
    } وإلا {
      sorted_داخل_کرو(str, compareUnicodeString, حالت);
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
  مضمن UBool جوڑو(ثابت UnicodeString& str, UError_کوڈ &حالت) {
    إذا(U_FAILURE(حالت)) أرجع خطأ;
    UnicodeString *t = جديد UnicodeString(str);
    إذا(t==خالی_سوچک) {
      حالت = U_حافظہ_ونڈ_ERROR; أرجع خطأ;
    }
    أرجع adopt(t, حالت);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  مضمن UBool remove(ثابت UnicodeString &s, UError_کوڈ &حالت) {
    إذا(U_FAILURE(حالت)) أرجع خطأ;
    أرجع removeElement((فراغ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_سیٹ::~UString_سیٹ() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ساكن ثابت int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ساكن ثابت int32_t k_میل   = (1<<1); //< exact match - skip this one.
ساكن ثابت int32_t k_Suppress_وچ_الٹ = (1<<0);
ساكن ثابت int32_t k_جوڑو_منزل_Forward = (1<<1);
ساكن ثابت char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
صنف Simple_Filtered_Sentence_Break_انکڑے : عام UMemory {
عام:
  Simple_Filtered_Sentence_Break_انکڑے(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_انکڑے* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        أرجع هذا;
    }
    Simple_Filtered_Sentence_Break_انکڑے *decr() {
        إذا(umtx_atomic_dec(&refcount) <= 0) {
            مٹاؤ هذا;
        }
        أرجع خالی_سوچک;
    }
    مجازی ~Simple_Filtered_Sentence_Break_انکڑے();

    منطقي hasForwardsPartialTrie() ثابت { أرجع fForwardsPartialTrie.isValid(); }
    منطقي hasBackwardsTrie() ثابت { أرجع fBackwardsTrie.isValid(); }

    ثابت UCharsTrie& getForwardsPartialTrie() ثابت U_LIFETIME_BOUND {
        أرجع *fForwardsPartialTrie;
    }
    ثابت UCharsTrie& getBackwardsTrie() ثابت U_LIFETIME_BOUND { أرجع *fBackwardsTrie; }

خاص:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    Local_اشارہ_گر<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    Local_اشارہ_گر<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_انکڑے::~Simple_Filtered_Sentence_Break_انکڑے() {}

/**
 * Concrete implementation
 */
صنف SimpleFilteredSentenceBreakIterator : عام BreakIterator {
عام:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_کوڈ &حالت);
  SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other);
  مجازی ~SimpleFilteredSentenceBreakIterator();
خاص:
  Simple_Filtered_Sentence_Break_انکڑے *f_انکڑے;
  Local_اشارہ_گر<BreakIterator> fDelegate;
  Local_UText_اشارہ_گر           f_متن;

  /* -- subclass interface -- */
عام:
  /* -- cloning and other subclass stuff -- */
  مجازی BreakIterator *  create_بفر_Clone(فراغ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_کوڈ &حالت) override {
    // for now - always deep clone
    حالت = U_SAFECLONE_ALLOCATED_WARNING;
    أرجع clone();
  }
  مجازی SimpleFilteredSentenceBreakIterator* clone() ثابت override { أرجع جديد SimpleFilteredSentenceBreakIterator(*هذا); }
  مجازی UClassID getDynamicClassID() ثابت override { أرجع خالی_سوچک; }
  مجازی منطقي عامل==(ثابت BreakIterator& o) ثابت override { إذا(هذا==&o) أرجع سچ; أرجع خطأ; }

  /* -- text modifying -- */
  مجازی فراغ سیٹ_متن(UText *متن, UError_کوڈ &حالت) override { fDelegate->سیٹ_متن(متن,حالت); }
  مجازی BreakIterator& refresh_داخلہ_متن(UText* input,
                                          UError_کوڈ& حالت) U_LIFETIME_BOUND override {
      fDelegate->refresh_داخلہ_متن(input, حالت);
      أرجع *هذا;
  }
  مجازی فراغ adopt_متن(CharacterIterator* it) override { fDelegate->adopt_متن(it); }
  مجازی فراغ سیٹ_متن(ثابت UnicodeString &متن) override { fDelegate->سیٹ_متن(متن); }

  /* -- other functions that are just delegated -- */
  مجازی UText* getUText(UText* fill_وچ U_LIFETIME_BOUND, UError_کوڈ& حالت) ثابت override {
      أرجع fDelegate->getUText(fill_وچ, حالت);
  }
  مجازی CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override {
      أرجع fDelegate->get_متن();
  }

  /* -- ITERATION -- */
  مجازی int32_t first() override;
  مجازی int32_t preceding(int32_t وتھ) override;
  مجازی int32_t پچھلا() override;
  مجازی UBool isBoundary(int32_t وتھ) override;
  مجازی int32_t current() ثابت override { أرجع fDelegate->current(); } // we keep the delegate current, so this should be correct.

  مجازی int32_t next() override;

  مجازی int32_t next(int32_t n) override;
  مجازی int32_t following(int32_t وتھ) override;
  مجازی int32_t آخری() override;

خاص:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t اندرونی_اگلا(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t اندرونی_Prev(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    فراغ resetState(UError_کوڈ &حالت);
    /**
     * Is there a match  (exception) at this spot?
     */
    تعداد EFBMatch_نتیجہ { k_No_Exception_ایتھے, k_Exception_ایتھے };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    تعداد EFBMatch_نتیجہ breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_انکڑے(other.f_انکڑے->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_کوڈ &حالت) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,حالت),adopt->getLocale(ULOC_ACTUAL_LOCALE,حالت)),
  f_انکڑے(جديد Simple_Filtered_Sentence_Break_انکڑے(forwards, backwards)),
  fDelegate(adopt)
{
    إذا (f_انکڑے == خالی_سوچک) {
        مٹاؤ forwards;
        مٹاؤ backwards;
        إذا (U_SUCCESS(حالت)) {
            حالت = U_حافظہ_ونڈ_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_انکڑے = f_انکڑے->decr();
}

فراغ SimpleFilteredSentenceBreakIterator::resetState(UError_کوڈ &حالت) {
  f_متن.adoptInstead(fDelegate->getUText(f_متن.orphan(), حالت));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجہ
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t بہترین_Posn = -1;
    int32_t بہترین_قدر = -1;
    // loops while 'n' points to an exception.
    utext_سیٹ_Native_اشاریہ(f_متن.getAlias(), n); // from n..

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
        UCharsTrie iter(f_انکڑے->getBackwardsTrie());
        UChar32 uch;
        طالما((uch=utext_previous32(f_متن.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_نتیجہ r = iter.اگلا_For_کوڈ_نقطہ(uch);
            إذا(USTRINGTRIE_HAS_قدر(r)) { // remember the best match so far
                بہترین_Posn = utext_get_Native_اشاریہ(f_متن.getAlias());
                بہترین_قدر = iter.get_قدر();
            }
            إذا(!USTRINGTRIE_HAS_اگلا(r)) {
                اكسر;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    إذا(بہترین_Posn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      إذا(بہترین_قدر == k_میل) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        أرجع k_Exception_ایتھے; // See if the next is another exception.
      } وإلا إذا(بہترین_قدر == kPARTIAL
                && f_انکڑے->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_نتیجہ rfwd = USTRINGTRIE_INTERMEDIATE_قدر;
        utext_سیٹ_Native_اشاریہ(f_متن.getAlias(), بہترین_Posn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_انکڑے->getForwardsPartialTrie());
        UChar32 uch;
        طالما((uch=utext_next32(f_متن.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_اگلا(rfwd=iter.اگلا_For_کوڈ_نقطہ(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        إذا(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            أرجع k_Exception_ایتھے;
        } وإلا {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          أرجع k_No_Exception_ایتھے;
        }
      } وإلا {
        أرجع k_No_Exception_ایتھے; // internal error and/or no forwards trie
      }
    } وإلا {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      أرجع k_No_Exception_ایتھے; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::اندرونی_اگلا(int32_t n) {
  إذا(n == UBRK_پورا || // at end  or
    !f_انکڑے->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      أرجع n;
  }
  // OK, do we need to break here?
  UError_کوڈ حالت = U_ZERO_ERROR;
  // refresh text
  resetState(حالت);
  إذا(U_FAILURE(حالت)) أرجع UBRK_پورا; // bail out
  int64_t utextLen = utext_native_لمائی(f_متن.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  طالما (n != UBRK_پورا && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجہ m = breakExceptionAt(n);

    بدل(m) {
    حالة k_Exception_ایتھے:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة k_No_Exception_ایتھے:
      أرجع n;
    }    
  }
  أرجع n;
}

int32_t
SimpleFilteredSentenceBreakIterator::اندرونی_Prev(int32_t n) {
  إذا(n == 0 || n == UBRK_پورا || // at end  or
    !f_انکڑے->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      أرجع n;
  }
  // OK, do we need to break here?
  UError_کوڈ حالت = U_ZERO_ERROR;
  // refresh text
  resetState(حالت);
  إذا(U_FAILURE(حالت)) أرجع UBRK_پورا; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  طالما (n != UBRK_پورا && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجہ m = breakExceptionAt(n);

    بدل(m) {
    حالة k_Exception_ایتھے:
      n = fDelegate->پچھلا(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة k_No_Exception_ایتھے:
      أرجع n;
    }    
  }
  أرجع n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  أرجع اندرونی_اگلا(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  أرجع fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t وتھ) {
  أرجع اندرونی_Prev(fDelegate->preceding(وتھ));
}

int32_t
SimpleFilteredSentenceBreakIterator::پچھلا() {
  أرجع اندرونی_Prev(fDelegate->پچھلا());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t وتھ) {
  إذا (!fDelegate->isBoundary(وتھ)) أرجع خطأ; // no break to suppress

  إذا (!f_انکڑے->hasBackwardsTrie()) أرجع سچ; // no data = no suppressions

  UError_کوڈ حالت = U_ZERO_ERROR;
  resetState(حالت);

  SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجہ m = breakExceptionAt(وتھ);

  بدل(m) {
  حالة k_Exception_ایتھے:
    أرجع خطأ;
  افتراضي:
  حالة k_No_Exception_ایتھے:
    أرجع سچ;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t وتھ) {
  أرجع اندرونی_اگلا(fDelegate->next(وتھ));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t وتھ) {
  أرجع اندرونی_اگلا(fDelegate->following(وتھ));
}

int32_t
SimpleFilteredSentenceBreakIterator::آخری() {
  // Don't suppress a break opportunity at the end of text.
  أرجع fDelegate->آخری();
}


/**
 * Concrete implementation of builder class.
 */
صنف SimpleFilteredBreakIteratorBuilder : عام FilteredBreakIteratorBuilder {
عام:
  مجازی ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_کوڈ &حالت);
  SimpleFilteredBreakIteratorBuilder(UError_کوڈ &حالت);
  مجازی UBool suppressBreakAfter(ثابت UnicodeString& exception, UError_کوڈ& حالت) override;
  مجازی UBool unsuppressBreakAfter(ثابت UnicodeString& exception, UError_کوڈ& حالت) override;
  مجازی BreakIterator *build(BreakIterator* adoptBreakIterator, UError_کوڈ& حالت) override;
خاص:
  UString_سیٹ f_سیٹ;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_کوڈ &حالت) 
  : f_سیٹ(حالت)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_کوڈ &حالت)
  : f_سیٹ(حالت)
{
  إذا(U_SUCCESS(حالت)) {
    UError_کوڈ sub_حالت = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارہ_گر b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_ادھار_Name(), &sub_حالت));
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_ادھار_Name(), "[exit]", u_errorName(حالت));
#جے_انت
      أرجع;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشارہ_گر exceptions(ures_get_By_کلید_With_Fallback(b.getAlias(), "exceptions", خالی_سوچک, &sub_حالت));
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_ادھار_Name(), "[exit]", u_errorName(حالت));
#جے_انت
      أرجع;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشارہ_گر breaks(ures_get_By_کلید_With_Fallback(exceptions.getAlias(), "SentenceBreak", خالی_سوچک, &sub_حالت));

#إذا FB_DEBUG
    {
      UError_کوڈ subsub = sub_حالت;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_ادھار_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_حالت));
    }
#جے_انت
    
    إذا (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت = sub_حالت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_ادھار_Name(), "[exit]", u_errorName(حالت));
#جے_انت
      أرجع;  // leaves the builder empty, if you try to use it.
    }

    Local_UResource_Bundle_اشارہ_گر strs;
    sub_حالت = حالت; // Pick up inherited warning status now 
    افعل {
      strs.adoptInstead(ures_get_اگلا_Resource(breaks.getAlias(), strs.orphan(), &sub_حالت));
      إذا(strs.isValid() && U_SUCCESS(sub_حالت)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &حالت));
        suppressBreakAfter(str, حالت); // load the string
      }
    } طالما (strs.isValid() && U_SUCCESS(sub_حالت));
    إذا(U_FAILURE(sub_حالت)&&sub_حالت!=U_اشاریہ_OUTOFBOUNDS_ERROR&&U_SUCCESS(حالت)) {
      حالت = sub_حالت;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ثابت UnicodeString& exception, UError_کوڈ& حالت)
{
  UBool r = f_سیٹ.جوڑو(exception, حالت);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  أرجع r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ثابت UnicodeString& exception, UError_کوڈ& حالت)
{
  UBool r = f_سیٹ.remove(exception, حالت);
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
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_کوڈ& حالت) {
  Local_اشارہ_گر<BreakIterator> adopt(adoptBreakIterator);

  Local_اشارہ_گر<UCharsTrieBuilder> builder(جديد UCharsTrieBuilder(حالت), حالت);
  Local_اشارہ_گر<UCharsTrieBuilder> builder2(جديد UCharsTrieBuilder(حالت), حالت);
  إذا(U_FAILURE(حالت)) {
    أرجع خالی_سوچک;
  }

  int32_t rev_گنتی = 0;
  int32_t fwd_گنتی = 0;

  int32_t sub_گنتی = f_سیٹ.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_گنتی);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_حافظہ<صحيح> partials;
  partials.allocateInsteadAndReset(sub_گنتی);

  Local_اشارہ_گر<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  Local_اشارہ_گر<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  صحيح n=0;
  لكل ( int32_t i = 0;
        i<f_سیٹ.size();
        i++) {
    ثابت UnicodeString *abbr = f_سیٹ.getStringAt(i);
    إذا(abbr) {
      FB_TRACE("build",abbr,سچ,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],سچ,i);
    } وإلا {
      FB_TRACE("build",abbr,خطأ,i);
      حالت = U_حافظہ_ونڈ_ERROR;
      أرجع خالی_سوچک;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  لكل(صحيح i=0;i<sub_گنتی;i++) {
    صحيح nn = ustrs[i].اشاریہ_دا(kFULLSTOP); // TODO: non-'.' abbreviations
    إذا(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],خطأ,i);
      // is partial.
      // is it unique?
      صحيح sameAs = -1;
      لكل(صحيح j=0;j<sub_گنتی;j++) {
        إذا(j==i) استمر;
        إذا(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],خطأ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          إذا(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_وچ_الٹ | k_جوڑو_منزل_Forward;
            FB_TRACE("suppressing",&ustrs[j],خطأ,j);
          } وإلا إذا(partials[j] & k_Suppress_وچ_الٹ) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],خطأ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],خطأ,partials[i]);
      UnicodeString اگیتر(ustrs[i], 0, nn+1);
      إذا(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        اگیتر.reverse();
        builder->جوڑو(اگیتر, kPARTIAL, حالت);
        rev_گنتی++;
        FB_TRACE("Added partial",&اگیتر,خطأ, i);
        FB_TRACE(u_errorName(حالت),&ustrs[i],خطأ,i);
        partials[i] = k_Suppress_وچ_الٹ | k_جوڑو_منزل_Forward;
      } وإلا {
        FB_TRACE("NOT adding partial",&اگیتر,خطأ, i);
        FB_TRACE(u_errorName(حالت),&ustrs[i],خطأ,i);
      }
    }
  }
  لكل(صحيح i=0;i<sub_گنتی;i++) {
    إذا(partials[i]==0) {
      ustrs[i].reverse();
      builder->جوڑو(ustrs[i], k_میل, حالت);
      rev_گنتی++;
      FB_TRACE(u_errorName(حالت), &ustrs[i], خطأ, i);
    } وإلا {
      FB_TRACE("Adding fwd",&ustrs[i], خطأ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->جوڑو(ustrs[i], k_میل, حالت); // forward
      fwd_گنتی++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",خالی_سوچک,خطأ, sub_گنتی);

  إذا(rev_گنتی>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, حالت));
    إذا(U_FAILURE(حالت)) {
      FB_TRACE(u_errorName(حالت),خالی_سوچک,خطأ, -1);
      أرجع خالی_سوچک;
    }
  }

  إذا(fwd_گنتی>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, حالت));
    إذا(U_FAILURE(حالت)) {
      FB_TRACE(u_errorName(حالت),خالی_سوچک,خطأ, -1);
      أرجع خالی_سوچک;
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
FilteredBreakIteratorBuilder::createInstance(ثابت Locale& where, UError_کوڈ& حالت) {
  إذا(U_FAILURE(حالت)) أرجع خالی_سوچک;
  Local_اشارہ_گر<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(where, حالت), حالت);
  أرجع (U_SUCCESS(حالت))? ret.orphan(): خالی_سوچک;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_کوڈ &حالت) {
  أرجع create_خالی_تھاواں_Instance(حالت);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_خالی_تھاواں_Instance(UError_کوڈ& حالت) {
  إذا(U_FAILURE(حالت)) أرجع خالی_سوچک;
  Local_اشارہ_گر<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(حالت), حالت);
  أرجع (U_SUCCESS(حالت))? ret.orphan(): خالی_سوچک;
}

U_NAMESPACE_سرا

#جے_انت //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

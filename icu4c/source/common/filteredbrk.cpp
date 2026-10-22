// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#شامل "unicode/utypes.h"
#اگر !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#شامل "cmemory.h"

#شامل "unicode/filteredbrk.h"
#شامل "unicode/ucharstriebuilder.h"
#شامل "unicode/ures.h"

#شامل "uresimp.h" // ures_getByKeyWithFallback
#شامل "ubrkimpl.h" // U_ICUDATA_BRKITR
#شامل "uvector.h"
#شامل "cmemory.h"
#شامل "umutex.h"

U_NAMESPACE_BEGIN

#اگر_غیر_تعریف_شدہ FB_DEBUG
#تعریف_کرو FB_DEBUG 0
#اگر_ختم

#اگر FB_DEBUG
#شامل <stdio.h>
ساکن خالی _fb_trace(ثابت حرف *m, ثابت UnicodeString *s, UBool b, int32_t d, ثابت حرف *f, صحیح l) {
  حرف buf[2048];
  اگر(s) {
    s->extract(0,s->length(),buf,2048);
  } ورنہ {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (ثابت خالی*)s, b?'T':'F',(صحیح)d);
}

#تعریف_کرو FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#ورنہ
#تعریف_کرو FB_TRACE(m,s,b,d)
#اگر_ختم

/**
 * Used with sortedInsert()
 */
ساکن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشارہ_گر);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشارہ_گر);
    واپس a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
جماعت UString_سیٹ : عوامی UVector {
 عوامی:
  UString_سیٹ(UError_رمز &حالت_2) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           حالت_2) {}
  مجازی ~UString_سیٹ();
  /**
   * Is this UnicodeSet contained?
   */
  اندرونی UBool شامل_ہے(ثابت UnicodeString& s) {
    واپس شامل_ہے((خالی*) &s);
  }
  استعمال UVector::شامل_ہے;
  /**
   * Return the ith UnicodeString alias
   */
  اندرونی ثابت UnicodeString* getStringAt(int32_t i) ثابت U_LIFETIME_BOUND {
    واپس static_cast<ثابت UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  اندرونی UBool adopt(UnicodeString *str, UError_رمز &حالت_2) {
    اگر(U_FAILURE(حالت_2) || شامل_ہے(*str)) {
      حذف str;
      واپس جھوٹ;
    } ورنہ {
      sorted_داخل_کرو(str, compareUnicodeString, حالت_2);
      اگر(U_FAILURE(حالت_2)) {
        واپس جھوٹ;
      }
      واپس سچ;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  اندرونی UBool جوڑو(ثابت UnicodeString& str, UError_رمز &حالت_2) {
    اگر(U_FAILURE(حالت_2)) واپس جھوٹ;
    UnicodeString *t = نیا UnicodeString(str);
    اگر(t==خالی_اشاریہ) {
      حالت_2 = U_حافظہ_تخصیص_ERROR; واپس جھوٹ;
    }
    واپس adopt(t, حالت_2);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  اندرونی UBool remove(ثابت UnicodeString &s, UError_رمز &حالت_2) {
    اگر(U_FAILURE(حالت_2)) واپس جھوٹ;
    واپس removeElement((خالی*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_سیٹ::~UString_سیٹ() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ساکن ثابت int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ساکن ثابت int32_t k_مطابقت   = (1<<1); //< exact match - skip this one.
ساکن ثابت int32_t k_Suppress_اندر_الٹ = (1<<0);
ساکن ثابت int32_t k_جوڑو_مقصد_Forward = (1<<1);
ساکن ثابت char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
جماعت Simple_Filtered_Sentence_Break_ڈیٹا : عوامی UMemory {
عوامی:
  Simple_Filtered_Sentence_Break_ڈیٹا(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_ڈیٹا* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        واپس یہ;
    }
    Simple_Filtered_Sentence_Break_ڈیٹا *decr() {
        اگر(umtx_atomic_dec(&refcount) <= 0) {
            حذف یہ;
        }
        واپس خالی_اشاریہ;
    }
    مجازی ~Simple_Filtered_Sentence_Break_ڈیٹا();

    منطقی hasForwardsPartialTrie() ثابت { واپس fForwardsPartialTrie.isValid(); }
    منطقی hasBackwardsTrie() ثابت { واپس fBackwardsTrie.isValid(); }

    ثابت UCharsTrie& getForwardsPartialTrie() ثابت U_LIFETIME_BOUND {
        واپس *fForwardsPartialTrie;
    }
    ثابت UCharsTrie& getBackwardsTrie() ثابت U_LIFETIME_BOUND { واپس *fBackwardsTrie; }

نجی:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    Local_اشارہ_گر<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    Local_اشارہ_گر<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_ڈیٹا::~Simple_Filtered_Sentence_Break_ڈیٹا() {}

/**
 * Concrete implementation
 */
جماعت SimpleFilteredSentenceBreakIterator : عوامی BreakIterator {
عوامی:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_رمز &حالت_2);
  SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other);
  مجازی ~SimpleFilteredSentenceBreakIterator();
نجی:
  Simple_Filtered_Sentence_Break_ڈیٹا *f_ڈیٹا;
  Local_اشارہ_گر<BreakIterator> fDelegate;
  Local_UText_اشارہ_گر           f_متن;

  /* -- subclass interface -- */
عوامی:
  /* -- cloning and other subclass stuff -- */
  مجازی BreakIterator *  create_بفر_Clone(خالی * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_رمز &حالت_2) override {
    // for now - always deep clone
    حالت_2 = U_SAFECLONE_ALLOCATED_WARNING;
    واپس clone();
  }
  مجازی SimpleFilteredSentenceBreakIterator* clone() ثابت override { واپس نیا SimpleFilteredSentenceBreakIterator(*یہ); }
  مجازی UClassID getDynamicClassID() ثابت override { واپس خالی_اشاریہ; }
  مجازی منطقی عامل==(ثابت BreakIterator& o) ثابت override { اگر(یہ==&o) واپس سچ; واپس جھوٹ; }

  /* -- text modifying -- */
  مجازی خالی سیٹ_متن(UText *متن, UError_رمز &حالت_2) override { fDelegate->سیٹ_متن(متن,حالت_2); }
  مجازی BreakIterator& refresh_داخلہ_متن(UText* input,
                                          UError_رمز& حالت_2) U_LIFETIME_BOUND override {
      fDelegate->refresh_داخلہ_متن(input, حالت_2);
      واپس *یہ;
  }
  مجازی خالی adopt_متن(CharacterIterator* it) override { fDelegate->adopt_متن(it); }
  مجازی خالی سیٹ_متن(ثابت UnicodeString &متن) override { fDelegate->سیٹ_متن(متن); }

  /* -- other functions that are just delegated -- */
  مجازی UText* getUText(UText* fill_اندر U_LIFETIME_BOUND, UError_رمز& حالت_2) ثابت override {
      واپس fDelegate->getUText(fill_اندر, حالت_2);
  }
  مجازی CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override {
      واپس fDelegate->get_متن();
  }

  /* -- ITERATION -- */
  مجازی int32_t first() override;
  مجازی int32_t preceding(int32_t انحراف) override;
  مجازی int32_t پچھلا_2() override;
  مجازی UBool isBoundary(int32_t انحراف) override;
  مجازی int32_t current() ثابت override { واپس fDelegate->current(); } // we keep the delegate current, so this should be correct.

  مجازی int32_t next() override;

  مجازی int32_t next(int32_t n) override;
  مجازی int32_t following(int32_t انحراف) override;
  مجازی int32_t آخری() override;

نجی:
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
    int32_t اندرونی_پچھلا(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    خالی resetState(UError_رمز &حالت_2);
    /**
     * Is there a match  (exception) at this spot?
     */
    شمار EFBMatch_نتیجہ { k_No_Exception_یہاں, k_Exception_یہاں };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    شمار EFBMatch_نتیجہ breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_ڈیٹا(other.f_ڈیٹا->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_رمز &حالت_2) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,حالت_2),adopt->getLocale(ULOC_ACTUAL_LOCALE,حالت_2)),
  f_ڈیٹا(نیا Simple_Filtered_Sentence_Break_ڈیٹا(forwards, backwards)),
  fDelegate(adopt)
{
    اگر (f_ڈیٹا == خالی_اشاریہ) {
        حذف forwards;
        حذف backwards;
        اگر (U_SUCCESS(حالت_2)) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_ڈیٹا = f_ڈیٹا->decr();
}

خالی SimpleFilteredSentenceBreakIterator::resetState(UError_رمز &حالت_2) {
  f_متن.adoptInstead(fDelegate->getUText(f_متن.orphan(), حالت_2));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجہ
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t بہترین_Posn = -1;
    int32_t بہترین_قدر = -1;
    // loops while 'n' points to an exception.
    utext_سیٹ_Native_اشاریہ(f_متن.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    اگر(utext_previous32(f_متن.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } ورنہ {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(f_متن.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(f_ڈیٹا->getBackwardsTrie());
        UChar32 uch;
        جب((uch=utext_previous32(f_متن.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_نتیجہ r = iter.اگلا_For_رمز_نقطہ(uch);
            اگر(USTRINGTRIE_HAS_قدر(r)) { // remember the best match so far
                بہترین_Posn = utext_get_Native_اشاریہ(f_متن.getAlias());
                بہترین_قدر = iter.get_قدر();
            }
            اگر(!USTRINGTRIE_HAS_اگلا(r)) {
                توڑو;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    اگر(بہترین_Posn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      اگر(بہترین_قدر == k_مطابقت) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        واپس k_Exception_یہاں; // See if the next is another exception.
      } ورنہ اگر(بہترین_قدر == kPARTIAL
                && f_ڈیٹا->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_نتیجہ rfwd = USTRINGTRIE_INTERMEDIATE_قدر;
        utext_سیٹ_Native_اشاریہ(f_متن.getAlias(), بہترین_Posn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_ڈیٹا->getForwardsPartialTrie());
        UChar32 uch;
        جب((uch=utext_next32(f_متن.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_اگلا(rfwd=iter.اگلا_For_رمز_نقطہ(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        اگر(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            واپس k_Exception_یہاں;
        } ورنہ {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          واپس k_No_Exception_یہاں;
        }
      } ورنہ {
        واپس k_No_Exception_یہاں; // internal error and/or no forwards trie
      }
    } ورنہ {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      واپس k_No_Exception_یہاں; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::اندرونی_اگلا(int32_t n) {
  اگر(n == UBRK_مکمل || // at end  or
    !f_ڈیٹا->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      واپس n;
  }
  // OK, do we need to break here?
  UError_رمز حالت_2 = U_ZERO_ERROR;
  // refresh text
  resetState(حالت_2);
  اگر(U_FAILURE(حالت_2)) واپس UBRK_مکمل; // bail out
  int64_t utextLen = utext_native_لمبائی(f_متن.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  جب (n != UBRK_مکمل && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجہ m = breakExceptionAt(n);

    انتخاب(m) {
    حالت k_Exception_یہاں:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      جاری;

    طےشدہ:
    حالت k_No_Exception_یہاں:
      واپس n;
    }    
  }
  واپس n;
}

int32_t
SimpleFilteredSentenceBreakIterator::اندرونی_پچھلا(int32_t n) {
  اگر(n == 0 || n == UBRK_مکمل || // at end  or
    !f_ڈیٹا->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      واپس n;
  }
  // OK, do we need to break here?
  UError_رمز حالت_2 = U_ZERO_ERROR;
  // refresh text
  resetState(حالت_2);
  اگر(U_FAILURE(حالت_2)) واپس UBRK_مکمل; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  جب (n != UBRK_مکمل && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجہ m = breakExceptionAt(n);

    انتخاب(m) {
    حالت k_Exception_یہاں:
      n = fDelegate->پچھلا_2(); // skip this one. Find the next lowerlevel break.
      جاری;

    طےشدہ:
    حالت k_No_Exception_یہاں:
      واپس n;
    }    
  }
  واپس n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  واپس اندرونی_اگلا(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  واپس fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t انحراف) {
  واپس اندرونی_پچھلا(fDelegate->preceding(انحراف));
}

int32_t
SimpleFilteredSentenceBreakIterator::پچھلا_2() {
  واپس اندرونی_پچھلا(fDelegate->پچھلا_2());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t انحراف) {
  اگر (!fDelegate->isBoundary(انحراف)) واپس جھوٹ; // no break to suppress

  اگر (!f_ڈیٹا->hasBackwardsTrie()) واپس سچ; // no data = no suppressions

  UError_رمز حالت_2 = U_ZERO_ERROR;
  resetState(حالت_2);

  SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجہ m = breakExceptionAt(انحراف);

  انتخاب(m) {
  حالت k_Exception_یہاں:
    واپس جھوٹ;
  طےشدہ:
  حالت k_No_Exception_یہاں:
    واپس سچ;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t انحراف) {
  واپس اندرونی_اگلا(fDelegate->next(انحراف));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t انحراف) {
  واپس اندرونی_اگلا(fDelegate->following(انحراف));
}

int32_t
SimpleFilteredSentenceBreakIterator::آخری() {
  // Don't suppress a break opportunity at the end of text.
  واپس fDelegate->آخری();
}


/**
 * Concrete implementation of builder class.
 */
جماعت SimpleFilteredBreakIteratorBuilder : عوامی FilteredBreakIteratorBuilder {
عوامی:
  مجازی ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_رمز &حالت_2);
  SimpleFilteredBreakIteratorBuilder(UError_رمز &حالت_2);
  مجازی UBool suppressBreakAfter(ثابت UnicodeString& exception, UError_رمز& حالت_2) override;
  مجازی UBool unsuppressBreakAfter(ثابت UnicodeString& exception, UError_رمز& حالت_2) override;
  مجازی BreakIterator *build(BreakIterator* adoptBreakIterator, UError_رمز& حالت_2) override;
نجی:
  UString_سیٹ f_سیٹ;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_رمز &حالت_2) 
  : f_سیٹ(حالت_2)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_رمز &حالت_2)
  : f_سیٹ(حالت_2)
{
  اگر(U_SUCCESS(حالت_2)) {
    UError_رمز sub_حالت = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارہ_گر b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_بنیاد_Name(), &sub_حالت));
    اگر (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت_2 = sub_حالت; // copy the failing status 
#اگر FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_بنیاد_Name(), "[exit]", u_errorName(حالت_2));
#اگر_ختم
      واپس;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشارہ_گر exceptions(ures_get_By_کلید_With_Fallback(b.getAlias(), "exceptions", خالی_اشاریہ, &sub_حالت));
    اگر (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت_2 = sub_حالت; // copy the failing status 
#اگر FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_بنیاد_Name(), "[exit]", u_errorName(حالت_2));
#اگر_ختم
      واپس;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشارہ_گر breaks(ures_get_By_کلید_With_Fallback(exceptions.getAlias(), "SentenceBreak", خالی_اشاریہ, &sub_حالت));

#اگر FB_DEBUG
    {
      UError_رمز subsub = sub_حالت;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_بنیاد_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_حالت));
    }
#اگر_ختم
    
    اگر (U_FAILURE(sub_حالت) || (sub_حالت == U_USING_DEFAULT_WARNING) ) {    
      حالت_2 = sub_حالت; // copy the failing status 
#اگر FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_بنیاد_Name(), "[exit]", u_errorName(حالت_2));
#اگر_ختم
      واپس;  // leaves the builder empty, if you try to use it.
    }

    Local_UResource_Bundle_اشارہ_گر strs;
    sub_حالت = حالت_2; // Pick up inherited warning status now 
    کرو {
      strs.adoptInstead(ures_get_اگلا_Resource(breaks.getAlias(), strs.orphan(), &sub_حالت));
      اگر(strs.isValid() && U_SUCCESS(sub_حالت)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &حالت_2));
        suppressBreakAfter(str, حالت_2); // load the string
      }
    } جب (strs.isValid() && U_SUCCESS(sub_حالت));
    اگر(U_FAILURE(sub_حالت)&&sub_حالت!=U_اشاریہ_OUTOFBOUNDS_ERROR&&U_SUCCESS(حالت_2)) {
      حالت_2 = sub_حالت;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ثابت UnicodeString& exception, UError_رمز& حالت_2)
{
  UBool r = f_سیٹ.جوڑو(exception, حالت_2);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  واپس r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ثابت UnicodeString& exception, UError_رمز& حالت_2)
{
  UBool r = f_سیٹ.remove(exception, حالت_2);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  واپس r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
ساکن اندرونی UnicodeString* newUnicodeStringArray(size_t count) {
    واپس نیا UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_رمز& حالت_2) {
  Local_اشارہ_گر<BreakIterator> adopt(adoptBreakIterator);

  Local_اشارہ_گر<UCharsTrieBuilder> builder(نیا UCharsTrieBuilder(حالت_2), حالت_2);
  Local_اشارہ_گر<UCharsTrieBuilder> builder2(نیا UCharsTrieBuilder(حالت_2), حالت_2);
  اگر(U_FAILURE(حالت_2)) {
    واپس خالی_اشاریہ;
  }

  int32_t rev_گنتی = 0;
  int32_t fwd_گنتی = 0;

  int32_t sub_گنتی = f_سیٹ.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_گنتی);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_حافظہ<صحیح> partials;
  partials.allocateInsteadAndReset(sub_گنتی);

  Local_اشارہ_گر<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  Local_اشارہ_گر<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  صحیح n=0;
  برائے ( int32_t i = 0;
        i<f_سیٹ.size();
        i++) {
    ثابت UnicodeString *abbr = f_سیٹ.getStringAt(i);
    اگر(abbr) {
      FB_TRACE("build",abbr,سچ,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],سچ,i);
    } ورنہ {
      FB_TRACE("build",abbr,جھوٹ,i);
      حالت_2 = U_حافظہ_تخصیص_ERROR;
      واپس خالی_اشاریہ;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  برائے(صحیح i=0;i<sub_گنتی;i++) {
    صحیح nn = ustrs[i].اشاریہ_کا(kFULLSTOP); // TODO: non-'.' abbreviations
    اگر(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],جھوٹ,i);
      // is partial.
      // is it unique?
      صحیح sameAs = -1;
      برائے(صحیح j=0;j<sub_گنتی;j++) {
        اگر(j==i) جاری;
        اگر(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],جھوٹ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          اگر(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_اندر_الٹ | k_جوڑو_مقصد_Forward;
            FB_TRACE("suppressing",&ustrs[j],جھوٹ,j);
          } ورنہ اگر(partials[j] & k_Suppress_اندر_الٹ) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],جھوٹ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],جھوٹ,partials[i]);
      UnicodeString سابقہ(ustrs[i], 0, nn+1);
      اگر(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        سابقہ.reverse();
        builder->جوڑو(سابقہ, kPARTIAL, حالت_2);
        rev_گنتی++;
        FB_TRACE("Added partial",&سابقہ,جھوٹ, i);
        FB_TRACE(u_errorName(حالت_2),&ustrs[i],جھوٹ,i);
        partials[i] = k_Suppress_اندر_الٹ | k_جوڑو_مقصد_Forward;
      } ورنہ {
        FB_TRACE("NOT adding partial",&سابقہ,جھوٹ, i);
        FB_TRACE(u_errorName(حالت_2),&ustrs[i],جھوٹ,i);
      }
    }
  }
  برائے(صحیح i=0;i<sub_گنتی;i++) {
    اگر(partials[i]==0) {
      ustrs[i].reverse();
      builder->جوڑو(ustrs[i], k_مطابقت, حالت_2);
      rev_گنتی++;
      FB_TRACE(u_errorName(حالت_2), &ustrs[i], جھوٹ, i);
    } ورنہ {
      FB_TRACE("Adding fwd",&ustrs[i], جھوٹ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->جوڑو(ustrs[i], k_مطابقت, حالت_2); // forward
      fwd_گنتی++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",خالی_اشاریہ,جھوٹ, sub_گنتی);

  اگر(rev_گنتی>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, حالت_2));
    اگر(U_FAILURE(حالت_2)) {
      FB_TRACE(u_errorName(حالت_2),خالی_اشاریہ,جھوٹ, -1);
      واپس خالی_اشاریہ;
    }
  }

  اگر(fwd_گنتی>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, حالت_2));
    اگر(U_FAILURE(حالت_2)) {
      FB_TRACE(u_errorName(حالت_2),خالی_اشاریہ,جھوٹ, -1);
      واپس خالی_اشاریہ;
    }
  }

  واپس نیا SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), حالت_2);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ثابت Locale& where, UError_رمز& حالت_2) {
  اگر(U_FAILURE(حالت_2)) واپس خالی_اشاریہ;
  Local_اشارہ_گر<FilteredBreakIteratorBuilder> ret(نیا SimpleFilteredBreakIteratorBuilder(where, حالت_2), حالت_2);
  واپس (U_SUCCESS(حالت_2))? ret.orphan(): خالی_اشاریہ;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_رمز &حالت_2) {
  واپس create_خالی_خانے_Instance(حالت_2);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_خالی_خانے_Instance(UError_رمز& حالت_2) {
  اگر(U_FAILURE(حالت_2)) واپس خالی_اشاریہ;
  Local_اشارہ_گر<FilteredBreakIteratorBuilder> ret(نیا SimpleFilteredBreakIteratorBuilder(حالت_2), حالت_2);
  واپس (U_SUCCESS(حالت_2))? ret.orphan(): خالی_اشاریہ;
}

U_NAMESPACE_انتہا

#اگر_ختم //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

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

#اگر_تعریف_نشده FB_DEBUG
#تعریف_کن FB_DEBUG 0
#پایان_اگر

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

#تعریف_کن FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#وإلا
#تعریف_کن FB_TRACE(m,s,b,d)
#پایان_اگر

/**
 * Used with sortedInsert()
 */
ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشاره_گر);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشاره_گر);
    أرجع a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
صنف UString_مجموعه : عام UVector {
 عام:
  UString_مجموعه(UError_کد &وضعیت) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           وضعیت) {}
  مجازی ~UString_مجموعه();
  /**
   * Is this UnicodeSet contained?
   */
  مضمن UBool شامل_است(ثابت UnicodeString& s) {
    أرجع شامل_است((فراغ*) &s);
  }
  استفاده UVector::شامل_است;
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
  مضمن UBool adopt(UnicodeString *str, UError_کد &وضعیت) {
    إذا(U_FAILURE(وضعیت) || شامل_است(*str)) {
      حذف str;
      أرجع خطأ;
    } وإلا {
      sorted_درج(str, compareUnicodeString, وضعیت);
      إذا(U_FAILURE(وضعیت)) {
        أرجع خطأ;
      }
      أرجع درست;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  مضمن UBool جمع_کن(ثابت UnicodeString& str, UError_کد &وضعیت) {
    إذا(U_FAILURE(وضعیت)) أرجع خطأ;
    UnicodeString *t = جديد UnicodeString(str);
    إذا(t==اشاره_تهی) {
      وضعیت = U_حافظه_تخصیص_ERROR; أرجع خطأ;
    }
    أرجع adopt(t, وضعیت);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  مضمن UBool remove(ثابت UnicodeString &s, UError_کد &وضعیت) {
    إذا(U_FAILURE(وضعیت)) أرجع خطأ;
    أرجع removeElement((فراغ*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UString_مجموعه::~UString_مجموعه() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
ساكن ثابت int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
ساكن ثابت int32_t k_تطابق   = (1<<1); //< exact match - skip this one.
ساكن ثابت int32_t k_Suppress_درون_معکوس = (1<<0);
ساكن ثابت int32_t k_جمع_کن_مقصد_Forward = (1<<1);
ساكن ثابت char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
صنف Simple_Filtered_Sentence_Break_داده : عام UMemory {
عام:
  Simple_Filtered_Sentence_Break_داده(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    Simple_Filtered_Sentence_Break_داده* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        أرجع هذا;
    }
    Simple_Filtered_Sentence_Break_داده *decr() {
        إذا(umtx_atomic_dec(&refcount) <= 0) {
            حذف هذا;
        }
        أرجع اشاره_تهی;
    }
    مجازی ~Simple_Filtered_Sentence_Break_داده();

    منطقي hasForwardsPartialTrie() ثابت { أرجع fForwardsPartialTrie.isValid(); }
    منطقي hasBackwardsTrie() ثابت { أرجع fBackwardsTrie.isValid(); }

    ثابت UCharsTrie& getForwardsPartialTrie() ثابت U_LIFETIME_BOUND {
        أرجع *fForwardsPartialTrie;
    }
    ثابت UCharsTrie& getBackwardsTrie() ثابت U_LIFETIME_BOUND { أرجع *fBackwardsTrie; }

خاص:
    // These tries own their data arrays.
    // They are shared and must therefore not be modified.
    Local_اشاره_گر<UCharsTrie>    fForwardsPartialTrie; //  Has ".a" for "a.M."
    Local_اشاره_گر<UCharsTrie>    fBackwardsTrie; //  i.e. ".srM" for Mrs.
    u_atomic_int32_t            refcount;
};

Simple_Filtered_Sentence_Break_داده::~Simple_Filtered_Sentence_Break_داده() {}

/**
 * Concrete implementation
 */
صنف SimpleFilteredSentenceBreakIterator : عام BreakIterator {
عام:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_کد &وضعیت);
  SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other);
  مجازی ~SimpleFilteredSentenceBreakIterator();
خاص:
  Simple_Filtered_Sentence_Break_داده *f_داده;
  Local_اشاره_گر<BreakIterator> fDelegate;
  Local_UText_اشاره_گر           f_متن;

  /* -- subclass interface -- */
عام:
  /* -- cloning and other subclass stuff -- */
  مجازی BreakIterator *  create_میانگیر_Clone(فراغ * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UError_کد &وضعیت) override {
    // for now - always deep clone
    وضعیت = U_SAFECLONE_ALLOCATED_WARNING;
    أرجع clone();
  }
  مجازی SimpleFilteredSentenceBreakIterator* clone() ثابت override { أرجع جديد SimpleFilteredSentenceBreakIterator(*هذا); }
  مجازی UClassID getDynamicClassID() ثابت override { أرجع اشاره_تهی; }
  مجازی منطقي عملگر==(ثابت BreakIterator& o) ثابت override { إذا(هذا==&o) أرجع درست; أرجع خطأ; }

  /* -- text modifying -- */
  مجازی فراغ مجموعه_متن(UText *متن, UError_کد &وضعیت) override { fDelegate->مجموعه_متن(متن,وضعیت); }
  مجازی BreakIterator& refresh_درون_داد_متن(UText* input,
                                          UError_کد& وضعیت) U_LIFETIME_BOUND override {
      fDelegate->refresh_درون_داد_متن(input, وضعیت);
      أرجع *هذا;
  }
  مجازی فراغ adopt_متن(CharacterIterator* it) override { fDelegate->adopt_متن(it); }
  مجازی فراغ مجموعه_متن(ثابت UnicodeString &متن) override { fDelegate->مجموعه_متن(متن); }

  /* -- other functions that are just delegated -- */
  مجازی UText* getUText(UText* fill_درون U_LIFETIME_BOUND, UError_کد& وضعیت) ثابت override {
      أرجع fDelegate->getUText(fill_درون, وضعیت);
  }
  مجازی CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override {
      أرجع fDelegate->get_متن();
  }

  /* -- ITERATION -- */
  مجازی int32_t first() override;
  مجازی int32_t preceding(int32_t جابه_جایی) override;
  مجازی int32_t قبلی() override;
  مجازی UBool isBoundary(int32_t جابه_جایی) override;
  مجازی int32_t current() ثابت override { أرجع fDelegate->current(); } // we keep the delegate current, so this should be correct.

  مجازی int32_t next() override;

  مجازی int32_t next(int32_t n) override;
  مجازی int32_t following(int32_t جابه_جایی) override;
  مجازی int32_t آخرین() override;

خاص:
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the NEXT actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t داخلی_بعدی(int32_t n);
    /**
     * Given that the fDelegate has already given its "initial" answer,
     * find the PREV actual (non-excepted) break.
     * @param n initial position from delegate
     * @return new break position or UBRK_DONE
     */
    int32_t داخلی_Prev(int32_t n);
    /**
     * set up the UText with the value of the fDelegate.
     * Call this before calling breakExceptionAt. 
     * May be able to avoid excess calls
     */
    فراغ resetState(UError_کد &وضعیت);
    /**
     * Is there a match  (exception) at this spot?
     */
    تعداد EFBMatch_نتیجه { k_No_Exception_اینجا, k_Exception_اینجا };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    تعداد EFBMatch_نتیجه breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(ثابت SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), f_داده(other.f_داده->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UError_کد &وضعیت) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,وضعیت),adopt->getLocale(ULOC_ACTUAL_LOCALE,وضعیت)),
  f_داده(جديد Simple_Filtered_Sentence_Break_داده(forwards, backwards)),
  fDelegate(adopt)
{
    إذا (f_داده == اشاره_تهی) {
        حذف forwards;
        حذف backwards;
        إذا (U_SUCCESS(وضعیت)) {
            وضعیت = U_حافظه_تخصیص_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    f_داده = f_داده->decr();
}

فراغ SimpleFilteredSentenceBreakIterator::resetState(UError_کد &وضعیت) {
  f_متن.adoptInstead(fDelegate->getUText(f_متن.orphan(), وضعیت));
}

SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجه
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t بهترین_Posn = -1;
    int32_t بهترین_مقدار = -1;
    // loops while 'n' points to an exception.
    utext_مجموعه_Native_شاخص(f_متن.getAlias(), n); // from n..

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
        UCharsTrie iter(f_داده->getBackwardsTrie());
        UChar32 uch;
        طالما((uch=utext_previous32(f_متن.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UString_Trie_نتیجه r = iter.بعدی_For_کد_نقطه(uch);
            إذا(USTRINGTRIE_HAS_مقدار(r)) { // remember the best match so far
                بهترین_Posn = utext_get_Native_شاخص(f_متن.getAlias());
                بهترین_مقدار = iter.get_مقدار();
            }
            إذا(!USTRINGTRIE_HAS_بعدی(r)) {
                اكسر;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    إذا(بهترین_Posn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      إذا(بهترین_مقدار == k_تطابق) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        أرجع k_Exception_اینجا; // See if the next is another exception.
      } وإلا إذا(بهترین_مقدار == kPARTIAL
                && f_داده->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UString_Trie_نتیجه rfwd = USTRINGTRIE_INTERMEDIATE_مقدار;
        utext_مجموعه_Native_شاخص(f_متن.getAlias(), بهترین_Posn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(f_داده->getForwardsPartialTrie());
        UChar32 uch;
        طالما((uch=utext_next32(f_متن.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_بعدی(rfwd=iter.بعدی_For_کد_نقطه(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        إذا(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            أرجع k_Exception_اینجا;
        } وإلا {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          أرجع k_No_Exception_اینجا;
        }
      } وإلا {
        أرجع k_No_Exception_اینجا; // internal error and/or no forwards trie
      }
    } وإلا {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      أرجع k_No_Exception_اینجا; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::داخلی_بعدی(int32_t n) {
  إذا(n == UBRK_انجامشده || // at end  or
    !f_داده->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      أرجع n;
  }
  // OK, do we need to break here?
  UError_کد وضعیت = U_ZERO_ERROR;
  // refresh text
  resetState(وضعیت);
  إذا(U_FAILURE(وضعیت)) أرجع UBRK_انجامشده; // bail out
  int64_t utextLen = utext_native_درازی(f_متن.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  طالما (n != UBRK_انجامشده && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجه m = breakExceptionAt(n);

    بدل(m) {
    حالة k_Exception_اینجا:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة k_No_Exception_اینجا:
      أرجع n;
    }    
  }
  أرجع n;
}

int32_t
SimpleFilteredSentenceBreakIterator::داخلی_Prev(int32_t n) {
  إذا(n == 0 || n == UBRK_انجامشده || // at end  or
    !f_داده->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      أرجع n;
  }
  // OK, do we need to break here?
  UError_کد وضعیت = U_ZERO_ERROR;
  // refresh text
  resetState(وضعیت);
  إذا(U_FAILURE(وضعیت)) أرجع UBRK_انجامشده; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  طالما (n != UBRK_انجامشده && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجه m = breakExceptionAt(n);

    بدل(m) {
    حالة k_Exception_اینجا:
      n = fDelegate->قبلی(); // skip this one. Find the next lowerlevel break.
      استمر;

    افتراضي:
    حالة k_No_Exception_اینجا:
      أرجع n;
    }    
  }
  أرجع n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  أرجع داخلی_بعدی(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  أرجع fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t جابه_جایی) {
  أرجع داخلی_Prev(fDelegate->preceding(جابه_جایی));
}

int32_t
SimpleFilteredSentenceBreakIterator::قبلی() {
  أرجع داخلی_Prev(fDelegate->قبلی());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t جابه_جایی) {
  إذا (!fDelegate->isBoundary(جابه_جایی)) أرجع خطأ; // no break to suppress

  إذا (!f_داده->hasBackwardsTrie()) أرجع درست; // no data = no suppressions

  UError_کد وضعیت = U_ZERO_ERROR;
  resetState(وضعیت);

  SimpleFilteredSentenceBreakIterator::EFBMatch_نتیجه m = breakExceptionAt(جابه_جایی);

  بدل(m) {
  حالة k_Exception_اینجا:
    أرجع خطأ;
  افتراضي:
  حالة k_No_Exception_اینجا:
    أرجع درست;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t جابه_جایی) {
  أرجع داخلی_بعدی(fDelegate->next(جابه_جایی));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t جابه_جایی) {
  أرجع داخلی_بعدی(fDelegate->following(جابه_جایی));
}

int32_t
SimpleFilteredSentenceBreakIterator::آخرین() {
  // Don't suppress a break opportunity at the end of text.
  أرجع fDelegate->آخرین();
}


/**
 * Concrete implementation of builder class.
 */
صنف SimpleFilteredBreakIteratorBuilder : عام FilteredBreakIteratorBuilder {
عام:
  مجازی ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_کد &وضعیت);
  SimpleFilteredBreakIteratorBuilder(UError_کد &وضعیت);
  مجازی UBool suppressBreakAfter(ثابت UnicodeString& exception, UError_کد& وضعیت) override;
  مجازی UBool unsuppressBreakAfter(ثابت UnicodeString& exception, UError_کد& وضعیت) override;
  مجازی BreakIterator *build(BreakIterator* adoptBreakIterator, UError_کد& وضعیت) override;
خاص:
  UString_مجموعه f_مجموعه;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UError_کد &وضعیت) 
  : f_مجموعه(وضعیت)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(ثابت Locale &fromLocale, UError_کد &وضعیت)
  : f_مجموعه(وضعیت)
{
  إذا(U_SUCCESS(وضعیت)) {
    UError_کد sub_وضعیت = U_ZERO_ERROR;
    Local_UResource_Bundle_اشاره_گر b(ures_open(U_ICUDATA_BRKITR, fromLocale.get_پایه_Name(), &sub_وضعیت));
    إذا (U_FAILURE(sub_وضعیت) || (sub_وضعیت == U_USING_DEFAULT_WARNING) ) {    
      وضعیت = sub_وضعیت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.get_پایه_Name(), "[exit]", u_errorName(وضعیت));
#پایان_اگر
      أرجع;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشاره_گر exceptions(ures_get_By_کلید_With_Fallback(b.getAlias(), "exceptions", اشاره_تهی, &sub_وضعیت));
    إذا (U_FAILURE(sub_وضعیت) || (sub_وضعیت == U_USING_DEFAULT_WARNING) ) {    
      وضعیت = sub_وضعیت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.get_پایه_Name(), "[exit]", u_errorName(وضعیت));
#پایان_اگر
      أرجع;  // leaves the builder empty, if you try to use it.
    }
    Local_UResource_Bundle_اشاره_گر breaks(ures_get_By_کلید_With_Fallback(exceptions.getAlias(), "SentenceBreak", اشاره_تهی, &sub_وضعیت));

#إذا FB_DEBUG
    {
      UError_کد subsub = sub_وضعیت;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.get_پایه_Name(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(sub_وضعیت));
    }
#پایان_اگر
    
    إذا (U_FAILURE(sub_وضعیت) || (sub_وضعیت == U_USING_DEFAULT_WARNING) ) {    
      وضعیت = sub_وضعیت; // copy the failing status 
#إذا FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.get_پایه_Name(), "[exit]", u_errorName(وضعیت));
#پایان_اگر
      أرجع;  // leaves the builder empty, if you try to use it.
    }

    Local_UResource_Bundle_اشاره_گر strs;
    sub_وضعیت = وضعیت; // Pick up inherited warning status now 
    افعل {
      strs.adoptInstead(ures_get_بعدی_Resource(breaks.getAlias(), strs.orphan(), &sub_وضعیت));
      إذا(strs.isValid() && U_SUCCESS(sub_وضعیت)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &وضعیت));
        suppressBreakAfter(str, وضعیت); // load the string
      }
    } طالما (strs.isValid() && U_SUCCESS(sub_وضعیت));
    إذا(U_FAILURE(sub_وضعیت)&&sub_وضعیت!=U_شاخص_OUTOFBOUNDS_ERROR&&U_SUCCESS(وضعیت)) {
      وضعیت = sub_وضعیت;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(ثابت UnicodeString& exception, UError_کد& وضعیت)
{
  UBool r = f_مجموعه.جمع_کن(exception, وضعیت);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  أرجع r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(ثابت UnicodeString& exception, UError_کد& وضعیت)
{
  UBool r = f_مجموعه.remove(exception, وضعیت);
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
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UError_کد& وضعیت) {
  Local_اشاره_گر<BreakIterator> adopt(adoptBreakIterator);

  Local_اشاره_گر<UCharsTrieBuilder> builder(جديد UCharsTrieBuilder(وضعیت), وضعیت);
  Local_اشاره_گر<UCharsTrieBuilder> builder2(جديد UCharsTrieBuilder(وضعیت), وضعیت);
  إذا(U_FAILURE(وضعیت)) {
    أرجع اشاره_تهی;
  }

  int32_t rev_شمار = 0;
  int32_t fwd_شمار = 0;

  int32_t sub_شمار = f_مجموعه.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(sub_شمار);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  Local_حافظه<صحيح> partials;
  partials.allocateInsteadAndReset(sub_شمار);

  Local_اشاره_گر<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  Local_اشاره_گر<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  صحيح n=0;
  لكل ( int32_t i = 0;
        i<f_مجموعه.size();
        i++) {
    ثابت UnicodeString *abbr = f_مجموعه.getStringAt(i);
    إذا(abbr) {
      FB_TRACE("build",abbr,درست,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],درست,i);
    } وإلا {
      FB_TRACE("build",abbr,خطأ,i);
      وضعیت = U_حافظه_تخصیص_ERROR;
      أرجع اشاره_تهی;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  لكل(صحيح i=0;i<sub_شمار;i++) {
    صحيح nn = ustrs[i].شاخص_از(kFULLSTOP); // TODO: non-'.' abbreviations
    إذا(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],خطأ,i);
      // is partial.
      // is it unique?
      صحيح sameAs = -1;
      لكل(صحيح j=0;j<sub_شمار;j++) {
        إذا(j==i) استمر;
        إذا(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],خطأ,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          إذا(partials[j]==0) { // hasn't been processed yet
            partials[j] = k_Suppress_درون_معکوس | k_جمع_کن_مقصد_Forward;
            FB_TRACE("suppressing",&ustrs[j],خطأ,j);
          } وإلا إذا(partials[j] & k_Suppress_درون_معکوس) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],خطأ,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],خطأ,partials[i]);
      UnicodeString پیشوند(ustrs[i], 0, nn+1);
      إذا(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        پیشوند.reverse();
        builder->جمع_کن(پیشوند, kPARTIAL, وضعیت);
        rev_شمار++;
        FB_TRACE("Added partial",&پیشوند,خطأ, i);
        FB_TRACE(u_errorName(وضعیت),&ustrs[i],خطأ,i);
        partials[i] = k_Suppress_درون_معکوس | k_جمع_کن_مقصد_Forward;
      } وإلا {
        FB_TRACE("NOT adding partial",&پیشوند,خطأ, i);
        FB_TRACE(u_errorName(وضعیت),&ustrs[i],خطأ,i);
      }
    }
  }
  لكل(صحيح i=0;i<sub_شمار;i++) {
    إذا(partials[i]==0) {
      ustrs[i].reverse();
      builder->جمع_کن(ustrs[i], k_تطابق, وضعیت);
      rev_شمار++;
      FB_TRACE(u_errorName(وضعیت), &ustrs[i], خطأ, i);
    } وإلا {
      FB_TRACE("Adding fwd",&ustrs[i], خطأ, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->جمع_کن(ustrs[i], k_تطابق, وضعیت); // forward
      fwd_شمار++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",اشاره_تهی,خطأ, sub_شمار);

  إذا(rev_شمار>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, وضعیت));
    إذا(U_FAILURE(وضعیت)) {
      FB_TRACE(u_errorName(وضعیت),اشاره_تهی,خطأ, -1);
      أرجع اشاره_تهی;
    }
  }

  إذا(fwd_شمار>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, وضعیت));
    إذا(U_FAILURE(وضعیت)) {
      FB_TRACE(u_errorName(وضعیت),اشاره_تهی,خطأ, -1);
      أرجع اشاره_تهی;
    }
  }

  أرجع جديد SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), وضعیت);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(ثابت Locale& where, UError_کد& وضعیت) {
  إذا(U_FAILURE(وضعیت)) أرجع اشاره_تهی;
  Local_اشاره_گر<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(where, وضعیت), وضعیت);
  أرجع (U_SUCCESS(وضعیت))? ret.orphan(): اشاره_تهی;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UError_کد &وضعیت) {
  أرجع create_خالی_Instance(وضعیت);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::create_خالی_Instance(UError_کد& وضعیت) {
  إذا(U_FAILURE(وضعیت)) أرجع اشاره_تهی;
  Local_اشاره_گر<FilteredBreakIteratorBuilder> ret(جديد SimpleFilteredBreakIteratorBuilder(وضعیت), وضعیت);
  أرجع (U_SUCCESS(وضعیت))? ret.orphan(): اشاره_تهی;
}

U_NAMESPACE_انتها

#پایان_اگر //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

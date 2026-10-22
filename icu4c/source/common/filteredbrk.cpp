// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#include "unicode/utypes.h"
#ပေႃး !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#include "cmemory.h"

#include "unicode/filteredbrk.h"
#include "unicode/ucharstriebuilder.h"
#include "unicode/ures.h"

#include "uresimp.h" // ures_getByKeyWithFallback
#include "ubrkimpl.h" // U_ICUDATA_BRKITR
#include "uvector.h"
#include "cmemory.h"
#include "umutex.h"

U_NAMESPACE_BEGIN

#ifndef FB_DEBUG
#define FB_DEBUG 0
#endif

#ပေႃး FB_DEBUG
#include <stdio.h>
static void _fb_trace(const var_na *m, const UnicodeString *s, UBool b, int32_t d, const var_na *f, puur_naa_nka l) {
  var_na buf[2048];
  ပေႃး(s) {
    s->extract(0,s->length(),buf,2048);
  } ပေႃးဢမ်ႇ {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (const void*)s, b?'T':'F',(puur_naa_nka)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#ပေႃးဢမ်ႇ
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
static int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    const UnicodeString& a = *static_cast<const UnicodeString*>(t1.pointer);
    const UnicodeString& b = *static_cast<const UnicodeString*>(t2.pointer);
    ပွၵ်ႈ a.compare(b);
}

/**
 * A UVector which implements a set of strings.
 */
class UStringSet : public UVector {
 public:
  UStringSet(UErrorCode &status) : UVector(uprv_deleteUObject,
                                           uhash_compareUnicodeString,
                                           1,
                                           status) {}
  virtual ~UStringSet();
  /**
   * Is this UnicodeSet contained?
   */
  anta_hstha UBool contains(const UnicodeString& s) {
    ပွၵ်ႈ contains((void*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  anta_hstha const UnicodeString* getStringAt(int32_t i) const U_LIFETIME_BOUND {
    ပွၵ်ႈ static_cast<const UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  anta_hstha UBool adopt(UnicodeString *str, UErrorCode &status) {
    ပေႃး(U_FAILURE(status) || contains(*str)) {
      delete str;
      ပွၵ်ႈ false;
    } ပေႃးဢမ်ႇ {
      sortedInsert(str, compareUnicodeString, status);
      ပေႃး(U_FAILURE(status)) {
        ပွၵ်ႈ false;
      }
      ပွၵ်ႈ true;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  anta_hstha UBool add(const UnicodeString& str, UErrorCode &status) {
    ပေႃး(U_FAILURE(status)) ပွၵ်ႈ false;
    UnicodeString *t = new UnicodeString(str);
    ပေႃး(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; ပွၵ်ႈ false;
    }
    ပွၵ်ႈ adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  anta_hstha UBool remove(const UnicodeString &s, UErrorCode &status) {
    ပေႃး(U_FAILURE(status)) ပွၵ်ႈ false;
    ပွၵ်ႈ removeElement((void*) &s);
  }
};

/**
 * Virtual, won't be inlined
 */
UStringSet::~UStringSet() {}

/* ----------------------------------------------------------- */


/* Filtered Break constants */
static const int32_t kPARTIAL = (1<<0); //< partial - need to run through forward trie
static const int32_t kMATCH   = (1<<1); //< exact match - skip this one.
static const int32_t kSuppressInReverse = (1<<0);
static const int32_t kAddToForward = (1<<1);
static const char16_t kFULLSTOP = 0x002E; // '.'

/**
 * Shared data for SimpleFilteredSentenceBreakIterator
 */
class SimpleFilteredSentenceBreakData : public UMemory {
public:
  SimpleFilteredSentenceBreakData(UCharsTrie *forwards, UCharsTrie *backwards ) 
      : fForwardsPartialTrie(forwards), fBackwardsTrie(backwards), refcount(1) { }
    SimpleFilteredSentenceBreakData* incr() U_LIFETIME_BOUND {
        umtx_atomic_inc(&refcount);
        ပွၵ်ႈ this;
    }
    SimpleFilteredSentenceBreakData *decr() {
        ပေႃး(umtx_atomic_dec(&refcount) <= 0) {
            delete this;
        }
        ပွၵ်ႈ nullptr;
    }
    virtual ~SimpleFilteredSentenceBreakData();

    bool hasForwardsPartialTrie() const { ပွၵ်ႈ fForwardsPartialTrie.isValid(); }
    bool hasBackwardsTrie() const { ပွၵ်ႈ fBackwardsTrie.isValid(); }

    const UCharsTrie& getForwardsPartialTrie() const U_LIFETIME_BOUND {
        ပွၵ်ႈ *fForwardsPartialTrie;
    }
    const UCharsTrie& getBackwardsTrie() const U_LIFETIME_BOUND { ပွၵ်ႈ *fBackwardsTrie; }

private:
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
class SimpleFilteredSentenceBreakIterator : public BreakIterator {
public:
  SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status);
  SimpleFilteredSentenceBreakIterator(const SimpleFilteredSentenceBreakIterator& other);
  virtual ~SimpleFilteredSentenceBreakIterator();
private:
  SimpleFilteredSentenceBreakData *fData;
  LocalPointer<BreakIterator> fDelegate;
  LocalUTextPointer           fText;

  /* -- subclass interface -- */
public:
  /* -- cloning and other subclass stuff -- */
  virtual BreakIterator *  createBufferClone(void * /*stackBuffer*/,
                                             int32_t &/*BufferSize*/,
                                             UErrorCode &status) override {
    // for now - always deep clone
    status = U_SAFECLONE_ALLOCATED_WARNING;
    ပွၵ်ႈ clone();
  }
  virtual SimpleFilteredSentenceBreakIterator* clone() const override { ပွၵ်ႈ new SimpleFilteredSentenceBreakIterator(*this); }
  virtual UClassID getDynamicClassID() const override { ပွၵ်ႈ nullptr; }
  virtual bool operator==(const BreakIterator& o) const override { ပေႃး(this==&o) ပွၵ်ႈ true; ပွၵ်ႈ false; }

  /* -- text modifying -- */
  virtual void setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  virtual BreakIterator& refreshInputText(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refreshInputText(input, status);
      ပွၵ်ႈ *this;
  }
  virtual void adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  virtual void setText(const UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  virtual UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) const override {
      ပွၵ်ႈ fDelegate->getUText(fillIn, status);
  }
  virtual CharacterIterator& getText() const U_LIFETIME_BOUND override {
      ပွၵ်ႈ fDelegate->getText();
  }

  /* -- ITERATION -- */
  virtual int32_t first() override;
  virtual int32_t preceding(int32_t offset) override;
  virtual int32_t previous() override;
  virtual UBool isBoundary(int32_t offset) override;
  virtual int32_t current() const override { ပွၵ်ႈ fDelegate->current(); } // we keep the delegate current, so this should be correct.

  virtual int32_t next() override;

  virtual int32_t next(int32_t n) override;
  virtual int32_t following(int32_t offset) override;
  virtual int32_t last() override;

private:
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
    void resetState(UErrorCode &status);
    /**
     * Is there a match  (exception) at this spot?
     */
    ga_nanaa EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    ga_nanaa EFBMatchResult breakExceptionAt(int32_t n);
};

SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(const SimpleFilteredSentenceBreakIterator& other)
  : BreakIterator(other), fData(other.fData->incr()), fDelegate(other.fDelegate->clone())
{
}


SimpleFilteredSentenceBreakIterator::SimpleFilteredSentenceBreakIterator(BreakIterator *adopt, UCharsTrie *forwards, UCharsTrie *backwards, UErrorCode &status) :
  BreakIterator(adopt->getLocale(ULOC_VALID_LOCALE,status),adopt->getLocale(ULOC_ACTUAL_LOCALE,status)),
  fData(new SimpleFilteredSentenceBreakData(forwards, backwards)),
  fDelegate(adopt)
{
    ပေႃး (fData == nullptr) {
        delete forwards;
        delete backwards;
        ပေႃး (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }
}

SimpleFilteredSentenceBreakIterator::~SimpleFilteredSentenceBreakIterator() {
    fData = fData->decr();
}

void SimpleFilteredSentenceBreakIterator::resetState(UErrorCode &status) {
  fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
}

SimpleFilteredSentenceBreakIterator::EFBMatchResult
SimpleFilteredSentenceBreakIterator::breakExceptionAt(int32_t n) {
    int64_t bestPosn = -1;
    int32_t bestValue = -1;
    // loops while 'n' points to an exception.
    utext_setNativeIndex(fText.getAlias(), n); // from n..

    //if(debug2) u_printf(" n@ %d\n", n);
    // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
    ပေႃး(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } ပေႃးဢမ်ႇ {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        ၶၢဝ်း((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            ပေႃး(USTRINGTRIE_HAS_VALUE(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                bestValue = iter.getValue();
            }
            ပေႃး(!USTRINGTRIE_HAS_NEXT(r)) {
                bha_nga;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    ပေႃး(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      ပေႃး(bestValue == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        ပွၵ်ႈ kExceptionHere; // See if the next is another exception.
      } ပေႃးဢမ်ႇ ပေႃး(bestValue == kPARTIAL
                && fData->hasForwardsPartialTrie()) { // make sure there's a forward trie
        //if(debug2) u_printf(" partial backward match\n");
        // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
        // to see if it matches something going forward.
        UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_VALUE;
        utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close ..
        //if(debug2) u_printf("Retrying at %d\n", bestPosn);
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getForwardsPartialTrie());
        UChar32 uch;
        ၶၢဝ်း((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        ပေႃး(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            ပွၵ်ႈ kExceptionHere;
        } ပေႃးဢမ်ႇ {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          ပွၵ်ႈ kNoExceptionHere;
        }
      } ပေႃးဢမ်ႇ {
        ပွၵ်ႈ kNoExceptionHere; // internal error and/or no forwards trie
      }
    } ပေႃးဢမ်ႇ {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      ပွၵ်ႈ kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  ပေႃး(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ပွၵ်ႈ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  ပေႃး(U_FAILURE(status)) ပွၵ်ႈ UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ၶၢဝ်း (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    switch(m) {
    pak_sa kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      continue;

    default:
    pak_sa kNoExceptionHere:
      ပွၵ်ႈ n;
    }    
  }
  ပွၵ်ႈ n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  ပေႃး(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      ပွၵ်ႈ n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  ပေႃး(U_FAILURE(status)) ပွၵ်ႈ UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  ၶၢဝ်း (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    switch(m) {
    pak_sa kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      continue;

    default:
    pak_sa kNoExceptionHere:
      ပွၵ်ႈ n;
    }    
  }
  ပွၵ်ႈ n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  ပွၵ်ႈ internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  ပွၵ်ႈ fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  ပွၵ်ႈ internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  ပွၵ်ႈ internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  ပေႃး (!fDelegate->isBoundary(offset)) ပွၵ်ႈ false; // no break to suppress

  ပေႃး (!fData->hasBackwardsTrie()) ပွၵ်ႈ true; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  switch(m) {
  pak_sa kExceptionHere:
    ပွၵ်ႈ false;
  default:
  pak_sa kNoExceptionHere:
    ပွၵ်ႈ true;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  ပွၵ်ႈ internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  ပွၵ်ႈ internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  ပွၵ်ႈ fDelegate->last();
}


/**
 * Concrete implementation of builder class.
 */
class SimpleFilteredBreakIteratorBuilder : public FilteredBreakIteratorBuilder {
public:
  virtual ~SimpleFilteredBreakIteratorBuilder();
  SimpleFilteredBreakIteratorBuilder(const Locale &fromLocale, UErrorCode &status);
  SimpleFilteredBreakIteratorBuilder(UErrorCode &status);
  virtual UBool suppressBreakAfter(const UnicodeString& exception, UErrorCode& status) override;
  virtual UBool unsuppressBreakAfter(const UnicodeString& exception, UErrorCode& status) override;
  virtual BreakIterator *build(BreakIterator* adoptBreakIterator, UErrorCode& status) override;
private:
  UStringSet fSet;
};

SimpleFilteredBreakIteratorBuilder::~SimpleFilteredBreakIteratorBuilder()
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(UErrorCode &status) 
  : fSet(status)
{
}

SimpleFilteredBreakIteratorBuilder::SimpleFilteredBreakIteratorBuilder(const Locale &fromLocale, UErrorCode &status)
  : fSet(status)
{
  ပေႃး(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    ပေႃး (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ပေႃး FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ပွၵ်ႈ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_getByKeyWithFallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    ပေႃး (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ပေႃး FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ပွၵ်ႈ;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_getByKeyWithFallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#ပေႃး FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    ပေႃး (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#ပေႃး FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      ပွၵ်ႈ;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    do {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      ပေႃး(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } ၶၢဝ်း (strs.isValid() && U_SUCCESS(subStatus));
    ပေႃး(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(const UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  ပွၵ်ႈ r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(const UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  ပွၵ်ႈ r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
static anta_hstha UnicodeString* newUnicodeStringArray(size_t count) {
    ပွၵ်ႈ new UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(new UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(new UCharsTrieBuilder(status), status);
  ပေႃး(U_FAILURE(status)) {
    ပွၵ်ႈ nullptr;
  }

  int32_t revCount = 0;
  int32_t fwdCount = 0;

  int32_t subCount = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(subCount);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<puur_naa_nka> partials;
  partials.allocateInsteadAndReset(subCount);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  puur_naa_nka n=0;
  for ( int32_t i = 0;
        i<fSet.size();
        i++) {
    const UnicodeString *abbr = fSet.getStringAt(i);
    ပေႃး(abbr) {
      FB_TRACE("build",abbr,true,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],true,i);
    } ပေႃးဢမ်ႇ {
      FB_TRACE("build",abbr,false,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      ပွၵ်ႈ nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  for(puur_naa_nka i=0;i<subCount;i++) {
    puur_naa_nka nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    ပေႃး(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],false,i);
      // is partial.
      // is it unique?
      puur_naa_nka sameAs = -1;
      for(puur_naa_nka j=0;j<subCount;j++) {
        ပေႃး(j==i) continue;
        ပေႃး(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],false,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          ပေႃး(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],false,j);
          } ပေႃးဢမ်ႇ ပေႃး(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],false,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],false,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      ပေႃး(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        revCount++;
        FB_TRACE("Added partial",&prefix,false, i);
        FB_TRACE(u_errorName(status),&ustrs[i],false,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } ပေႃးဢမ်ႇ {
        FB_TRACE("NOT adding partial",&prefix,false, i);
        FB_TRACE(u_errorName(status),&ustrs[i],false,i);
      }
    }
  }
  for(puur_naa_nka i=0;i<subCount;i++) {
    ပေႃး(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      revCount++;
      FB_TRACE(u_errorName(status), &ustrs[i], false, i);
    } ပေႃးဢမ်ႇ {
      FB_TRACE("Adding fwd",&ustrs[i], false, i);

      // an optimization would be to only add the portion after the '.'
      // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward,
      // instead of "Ph.D." since we already know the "Ph." part is a match.
      // would need the trie to be able to hold 0-length strings, though.
      builder2->add(ustrs[i], kMATCH, status); // forward
      fwdCount++;
      //ustrs[i].reverse();
      ////if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
    }
  }
  FB_TRACE("AbbrCount",nullptr,false, subCount);

  ပေႃး(revCount>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    ပေႃး(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,false, -1);
      ပွၵ်ႈ nullptr;
    }
  }

  ပေႃး(fwdCount>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    ပေႃး(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,false, -1);
      ပွၵ်ႈ nullptr;
    }
  }

  ပွၵ်ႈ new SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(const Locale& where, UErrorCode& status) {
  ပေႃး(U_FAILURE(status)) ပွၵ်ႈ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(new SimpleFilteredBreakIteratorBuilder(where, status), status);
  ပွၵ်ႈ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  ပွၵ်ႈ createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  ပေႃး(U_FAILURE(status)) ပွၵ်ႈ nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(new SimpleFilteredBreakIteratorBuilder(status), status);
  ပွၵ်ႈ (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

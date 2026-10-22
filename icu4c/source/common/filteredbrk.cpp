// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#include "unicode/utypes.h"
#如果 !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

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

#如果 FB_DEBUG
#include <stdio.h>
static void _fb_trace(const char *m, const UnicodeString *s, UBool b, int32_t d, const char *f, 整数 l) {
  char buf[2048];
  如果(s) {
    s->extract(0,s->length(),buf,2048);
  } 否则 {
    strcpy(buf,"nullptr");
  }
  fprintf(stderr,"%s:%d: %s. s='%s'(%p), b=%c, d=%d\n",
          f, l, m, buf, (const void*)s, b?'T':'F',(整数)d);
}

#define FB_TRACE(m,s,b,d) _fb_trace(m,s,b,d,__FILE__,__LINE__)
#否则
#define FB_TRACE(m,s,b,d)
#endif

/**
 * Used with sortedInsert()
 */
static int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    const UnicodeString& a = *static_cast<const UnicodeString*>(t1.pointer);
    const UnicodeString& b = *static_cast<const UnicodeString*>(t2.pointer);
    返回 a.compare(b);
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
  inline UBool contains(const UnicodeString& s) {
    返回 contains((void*) &s);
  }
  using UVector::contains;
  /**
   * Return the ith UnicodeString alias
   */
  inline const UnicodeString* getStringAt(int32_t i) const U_LIFETIME_BOUND {
    返回 static_cast<const UnicodeString*>(elementAt(i));
  }
  /**
   * Adopt the UnicodeString if not already contained.
   * Caller no longer owns the pointer in any case.
   * @return true if adopted successfully, false otherwise (error, or else duplicate)
   */
  inline UBool adopt(UnicodeString *str, UErrorCode &status) {
    如果(U_FAILURE(status) || contains(*str)) {
      delete str;
      返回 false;
    } 否则 {
      sortedInsert(str, compareUnicodeString, status);
      如果(U_FAILURE(status)) {
        返回 false;
      }
      返回 true;
    }
  }
  /**
   * Add by value.
   * @return true if successfully adopted.
   */
  inline UBool add(const UnicodeString& str, UErrorCode &status) {
    如果(U_FAILURE(status)) 返回 false;
    UnicodeString *t = new UnicodeString(str);
    如果(t==nullptr) {
      status = U_MEMORY_ALLOCATION_ERROR; 返回 false;
    }
    返回 adopt(t, status);
  }
  /**
   * Remove this string.
   * @return true if successfully removed, false otherwise (error, or else it wasn't there)
   */
  inline UBool remove(const UnicodeString &s, UErrorCode &status) {
    如果(U_FAILURE(status)) 返回 false;
    返回 removeElement((void*) &s);
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
        返回 this;
    }
    SimpleFilteredSentenceBreakData *decr() {
        如果(umtx_atomic_dec(&refcount) <= 0) {
            delete this;
        }
        返回 nullptr;
    }
    virtual ~SimpleFilteredSentenceBreakData();

    bool hasForwardsPartialTrie() const { 返回 fForwardsPartialTrie.isValid(); }
    bool hasBackwardsTrie() const { 返回 fBackwardsTrie.isValid(); }

    const UCharsTrie& getForwardsPartialTrie() const U_LIFETIME_BOUND {
        返回 *fForwardsPartialTrie;
    }
    const UCharsTrie& getBackwardsTrie() const U_LIFETIME_BOUND { 返回 *fBackwardsTrie; }

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
    返回 clone();
  }
  virtual SimpleFilteredSentenceBreakIterator* clone() const override { 返回 new SimpleFilteredSentenceBreakIterator(*this); }
  virtual UClassID getDynamicClassID() const override { 返回 nullptr; }
  virtual bool operator==(const BreakIterator& o) const override { 如果(this==&o) 返回 true; 返回 false; }

  /* -- text modifying -- */
  virtual void setText(UText *text, UErrorCode &status) override { fDelegate->setText(text,status); }
  virtual BreakIterator& refreshInputText(UText* input,
                                          UErrorCode& status) U_LIFETIME_BOUND override {
      fDelegate->refreshInputText(input, status);
      返回 *this;
  }
  virtual void adoptText(CharacterIterator* it) override { fDelegate->adoptText(it); }
  virtual void setText(const UnicodeString &text) override { fDelegate->setText(text); }

  /* -- other functions that are just delegated -- */
  virtual UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& status) const override {
      返回 fDelegate->getUText(fillIn, status);
  }
  virtual CharacterIterator& getText() const U_LIFETIME_BOUND override {
      返回 fDelegate->getText();
  }

  /* -- ITERATION -- */
  virtual int32_t first() override;
  virtual int32_t preceding(int32_t offset) override;
  virtual int32_t previous() override;
  virtual UBool isBoundary(int32_t offset) override;
  virtual int32_t current() const override { 返回 fDelegate->current(); } // we keep the delegate current, so this should be correct.

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
    enum EFBMatchResult { kNoExceptionHere, kExceptionHere };
    /**
     * Determine if there is an exception at this spot
     * @param n spot to check
     * @return kNoExceptionHere or kExceptionHere
     **/
    enum EFBMatchResult breakExceptionAt(int32_t n);
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
    如果 (fData == nullptr) {
        delete forwards;
        delete backwards;
        如果 (U_SUCCESS(status)) {
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
    如果(utext_previous32(fText.getAlias())==u' ') {  // TODO: skip a class of chars here??
      // TODO only do this the 1st time?
      //if(debug2) u_printf("skipping prev: |%C| \n", (char16_t)uch);
    } 否则 {
      //if(debug2) u_printf("not skipping prev: |%C| \n", (char16_t)uch);
      utext_next32(fText.getAlias());
      //if(debug2) u_printf(" -> : |%C| \n", (char16_t)uch);
    }

    {
        // Do not modify the shared trie!
        UCharsTrie iter(fData->getBackwardsTrie());
        UChar32 uch;
        当((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL) {  // more to consume backwards
            UStringTrieResult r = iter.nextForCodePoint(uch);
            如果(USTRINGTRIE_HAS_VALUE(r)) { // remember the best match so far
                bestPosn = utext_getNativeIndex(fText.getAlias());
                bestValue = iter.getValue();
            }
            如果(!USTRINGTRIE_HAS_NEXT(r)) {
                break;
            }
            //if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (char16_t)uch, r, utext_getNativeIndex(fText.getAlias()));
        }
    }

    //if(bestValue >= 0) {
        //if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);
    //}

    如果(bestPosn>=0) {
      //if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (char16_t)uch, r, bestPosn, bestValue);

      //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
      //int32_t bestValue = iter.getValue();
      ////if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (char16_t)uch, r, bestValue);

      如果(bestValue == kMATCH) { // exact match!
        //if(debug2) u_printf(" exact backward match\n");
        返回 kExceptionHere; // See if the next is another exception.
      } 否则 如果(bestValue == kPARTIAL
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
        当((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
              USTRINGTRIE_HAS_NEXT(rfwd=iter.nextForCodePoint(uch))) {
          //if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (char16_t)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
        }
        如果(USTRINGTRIE_MATCHES(rfwd)) {
          //if(debug2) u_printf("fwd> /%C/ == forward match!\n", (char16_t)uch);
          // only full matches here, nothing to check
          // skip the next:
            返回 kExceptionHere;
        } 否则 {
          //if(debug2) u_printf("fwd> /%C/ no match.\n", (char16_t)uch);
          // no match (no exception) -return the 'underlying' break
          返回 kNoExceptionHere;
        }
      } 否则 {
        返回 kNoExceptionHere; // internal error and/or no forwards trie
      }
    } 否则 {
      //if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (char16_t)uch, r);  // no best match
      返回 kNoExceptionHere; // No match - so exit. Not an exception.
    }
}

// the workhorse single next.
int32_t
SimpleFilteredSentenceBreakIterator::internalNext(int32_t n) {
  如果(n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      返回 n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  如果(U_FAILURE(status)) 返回 UBRK_DONE; // bail out
  int64_t utextLen = utext_nativeLength(fText.getAlias());

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  当 (n != UBRK_DONE && n != utextLen) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    switch(m) {
    case kExceptionHere:
      n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
      continue;

    default:
    case kNoExceptionHere:
      返回 n;
    }    
  }
  返回 n;
}

int32_t
SimpleFilteredSentenceBreakIterator::internalPrev(int32_t n) {
  如果(n == 0 || n == UBRK_DONE || // at end  or
    !fData->hasBackwardsTrie()) { // .. no backwards table loaded == no exceptions
      返回 n;
  }
  // OK, do we need to break here?
  UErrorCode status = U_ZERO_ERROR;
  // refresh text
  resetState(status);
  如果(U_FAILURE(status)) 返回 UBRK_DONE; // bail out

  //if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
  当 (n != UBRK_DONE && n != 0) { // outer loop runs once per underlying break (from fDelegate).
    SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(n);

    switch(m) {
    case kExceptionHere:
      n = fDelegate->previous(); // skip this one. Find the next lowerlevel break.
      continue;

    default:
    case kNoExceptionHere:
      返回 n;
    }    
  }
  返回 n;
}


int32_t
SimpleFilteredSentenceBreakIterator::next() {
  返回 internalNext(fDelegate->next());
}

int32_t
SimpleFilteredSentenceBreakIterator::first() {
  // Don't suppress a break opportunity at the beginning of text.
  返回 fDelegate->first();
}

int32_t
SimpleFilteredSentenceBreakIterator::preceding(int32_t offset) {
  返回 internalPrev(fDelegate->preceding(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::previous() {
  返回 internalPrev(fDelegate->previous());
}

UBool SimpleFilteredSentenceBreakIterator::isBoundary(int32_t offset) {
  如果 (!fDelegate->isBoundary(offset)) 返回 false; // no break to suppress

  如果 (!fData->hasBackwardsTrie()) 返回 true; // no data = no suppressions

  UErrorCode status = U_ZERO_ERROR;
  resetState(status);

  SimpleFilteredSentenceBreakIterator::EFBMatchResult m = breakExceptionAt(offset);

  switch(m) {
  case kExceptionHere:
    返回 false;
  default:
  case kNoExceptionHere:
    返回 true;
  }    
}
 
int32_t
SimpleFilteredSentenceBreakIterator::next(int32_t offset) {
  返回 internalNext(fDelegate->next(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::following(int32_t offset) {
  返回 internalNext(fDelegate->following(offset));
}

int32_t
SimpleFilteredSentenceBreakIterator::last() {
  // Don't suppress a break opportunity at the end of text.
  返回 fDelegate->last();
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
  如果(U_SUCCESS(status)) {
    UErrorCode subStatus = U_ZERO_ERROR;
    LocalUResourceBundlePointer b(ures_open(U_ICUDATA_BRKITR, fromLocale.getBaseName(), &subStatus));
    如果 (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#如果 FB_DEBUG
      fprintf(stderr, "open BUNDLE %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      返回;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer exceptions(ures_getByKeyWithFallback(b.getAlias(), "exceptions", nullptr, &subStatus));
    如果 (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#如果 FB_DEBUG
      fprintf(stderr, "open EXCEPTIONS %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      返回;  // leaves the builder empty, if you try to use it.
    }
    LocalUResourceBundlePointer breaks(ures_getByKeyWithFallback(exceptions.getAlias(), "SentenceBreak", nullptr, &subStatus));

#如果 FB_DEBUG
    {
      UErrorCode subsub = subStatus;
      fprintf(stderr, "open SentenceBreak %s => %s, %s\n", fromLocale.getBaseName(), ures_getLocale(breaks.getAlias(), &subsub), u_errorName(subStatus));
    }
#endif
    
    如果 (U_FAILURE(subStatus) || (subStatus == U_USING_DEFAULT_WARNING) ) {    
      status = subStatus; // copy the failing status 
#如果 FB_DEBUG
      fprintf(stderr, "open %s : %s, %s\n", fromLocale.getBaseName(), "[exit]", u_errorName(status));
#endif
      返回;  // leaves the builder empty, if you try to use it.
    }

    LocalUResourceBundlePointer strs;
    subStatus = status; // Pick up inherited warning status now 
    do {
      strs.adoptInstead(ures_getNextResource(breaks.getAlias(), strs.orphan(), &subStatus));
      如果(strs.isValid() && U_SUCCESS(subStatus)) {
        UnicodeString str(ures_getUnicodeString(strs.getAlias(), &status));
        suppressBreakAfter(str, status); // load the string
      }
    } 当 (strs.isValid() && U_SUCCESS(subStatus));
    如果(U_FAILURE(subStatus)&&subStatus!=U_INDEX_OUTOFBOUNDS_ERROR&&U_SUCCESS(status)) {
      status = subStatus;
    }
  }
}

UBool
SimpleFilteredBreakIteratorBuilder::suppressBreakAfter(const UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.add(exception, status);
  FB_TRACE("suppressBreakAfter",&exception,r,0);
  返回 r;
}

UBool
SimpleFilteredBreakIteratorBuilder::unsuppressBreakAfter(const UnicodeString& exception, UErrorCode& status)
{
  UBool r = fSet.remove(exception, status);
  FB_TRACE("unsuppressBreakAfter",&exception,r,0);
  返回 r;
}

/**
 * Jitterbug 2974: MSVC has a bug whereby new X[0] behaves badly.
 * Work around this.
 *
 * Note: "new UnicodeString[subCount]" ends up calling global operator new
 * on MSVC2012 for some reason.
 */
static inline UnicodeString* newUnicodeStringArray(size_t count) {
    返回 new UnicodeString[count ? count : 1];
}

BreakIterator *
SimpleFilteredBreakIteratorBuilder::build(BreakIterator* adoptBreakIterator, UErrorCode& status) {
  LocalPointer<BreakIterator> adopt(adoptBreakIterator);

  LocalPointer<UCharsTrieBuilder> builder(new UCharsTrieBuilder(status), status);
  LocalPointer<UCharsTrieBuilder> builder2(new UCharsTrieBuilder(status), status);
  如果(U_FAILURE(status)) {
    返回 nullptr;
  }

  int32_t revCount = 0;
  int32_t fwdCount = 0;

  int32_t subCount = fSet.size();

  UnicodeString *ustrs_ptr = newUnicodeStringArray(subCount);
  
  LocalArray<UnicodeString> ustrs(ustrs_ptr);

  LocalMemory<整数> partials;
  partials.allocateInsteadAndReset(subCount);

  LocalPointer<UCharsTrie>    backwardsTrie; //  i.e. ".srM" for Mrs.
  LocalPointer<UCharsTrie>    forwardsPartialTrie; //  Has ".a" for "a.M."

  整数 n=0;
  为 ( int32_t i = 0;
        i<fSet.size();
        i++) {
    const UnicodeString *abbr = fSet.getStringAt(i);
    如果(abbr) {
      FB_TRACE("build",abbr,true,i);
      ustrs[n] = *abbr; // copy by value
      FB_TRACE("ustrs[n]",&ustrs[n],true,i);
    } 否则 {
      FB_TRACE("build",abbr,false,i);
      status = U_MEMORY_ALLOCATION_ERROR;
      返回 nullptr;
    }
    partials[n] = 0; // default: not partial
    n++;
  }
  // first pass - find partials.
  为(整数 i=0;i<subCount;i++) {
    整数 nn = ustrs[i].indexOf(kFULLSTOP); // TODO: non-'.' abbreviations
    如果(nn>-1 && (nn+1)!=ustrs[i].length()) {
      FB_TRACE("partial",&ustrs[i],false,i);
      // is partial.
      // is it unique?
      整数 sameAs = -1;
      为(整数 j=0;j<subCount;j++) {
        如果(j==i) continue;
        如果(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
          FB_TRACE("prefix",&ustrs[j],false,nn+1);
          //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
          如果(partials[j]==0) { // hasn't been processed yet
            partials[j] = kSuppressInReverse | kAddToForward;
            FB_TRACE("suppressing",&ustrs[j],false,j);
          } 否则 如果(partials[j] & kSuppressInReverse) {
            sameAs = j; // the other entry is already in the reverse table.
          }
        }
      }
      FB_TRACE("for partial same-",&ustrs[i],false,sameAs);
      FB_TRACE(" == partial #",&ustrs[i],false,partials[i]);
      UnicodeString prefix(ustrs[i], 0, nn+1);
      如果(sameAs == -1 && partials[i] == 0) {
        // first one - add the prefix to the reverse table.
        prefix.reverse();
        builder->add(prefix, kPARTIAL, status);
        revCount++;
        FB_TRACE("Added partial",&prefix,false, i);
        FB_TRACE(u_errorName(status),&ustrs[i],false,i);
        partials[i] = kSuppressInReverse | kAddToForward;
      } 否则 {
        FB_TRACE("NOT adding partial",&prefix,false, i);
        FB_TRACE(u_errorName(status),&ustrs[i],false,i);
      }
    }
  }
  为(整数 i=0;i<subCount;i++) {
    如果(partials[i]==0) {
      ustrs[i].reverse();
      builder->add(ustrs[i], kMATCH, status);
      revCount++;
      FB_TRACE(u_errorName(status), &ustrs[i], false, i);
    } 否则 {
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

  如果(revCount>0) {
    backwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
    如果(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,false, -1);
      返回 nullptr;
    }
  }

  如果(fwdCount>0) {
    forwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
    如果(U_FAILURE(status)) {
      FB_TRACE(u_errorName(status),nullptr,false, -1);
      返回 nullptr;
    }
  }

  返回 new SimpleFilteredSentenceBreakIterator(adopt.orphan(), forwardsPartialTrie.orphan(), backwardsTrie.orphan(), status);
}


// ----------- Base class implementation

FilteredBreakIteratorBuilder::FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder::~FilteredBreakIteratorBuilder() {
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(const Locale& where, UErrorCode& status) {
  如果(U_FAILURE(status)) 返回 nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(new SimpleFilteredBreakIteratorBuilder(where, status), status);
  返回 (U_SUCCESS(status))? ret.orphan(): nullptr;
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createInstance(UErrorCode &status) {
  返回 createEmptyInstance(status);
}

FilteredBreakIteratorBuilder *
FilteredBreakIteratorBuilder::createEmptyInstance(UErrorCode& status) {
  如果(U_FAILURE(status)) 返回 nullptr;
  LocalPointer<FilteredBreakIteratorBuilder> ret(new SimpleFilteredBreakIteratorBuilder(status), status);
  返回 (U_SUCCESS(status))? ret.orphan(): nullptr;
}

U_NAMESPACE_END

#endif //#if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_NORMALIZATION

#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/chariter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/schriter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchriter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/normlzr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "normalizer2impl.h"
#ଅନ୍ତର୍ଭାଵ "uprops.h"  // for uniset_getUnicode32Instance()

#ଯଦି defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ଅଚଳ UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(ନୂତନ StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(ନୂତନ UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ଅଚଳ CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ଅଚଳ Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

ଶୂନ୍ୟ
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    ଯଦି(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            ନୂତନ FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    ଯଦି(U_FAILURE(errorCode)) {
        errorCode=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(errorCode);
    }
}

Normalizer::~Normalizer()
{
    delete fFilteredNorm2;
    delete text;
}

Normalizer* 
Normalizer::clone() ଅଚଳ
{
    ଫେରନ୍ତୁ ନୂତନ Normalizer(*ଏହି);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() ଅଚଳ
{
    ଫେରନ୍ତୁ text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
ତାର୍କିକ Normalizer::operator==(ଅଚଳ Normalizer& that) ଅଚଳ
{
    ଫେରନ୍ତୁ
        ଏହି==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *text==*that.text &&
        buffer==that.buffer &&
        bufferPos==that.bufferPos &&
        nextIndex==that.nextIndex);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

ଶୂନ୍ୟ U_EXPORT2
Normalizer::normalize(ଅଚଳ UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    ଯଦି(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        ଯଦି(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } ଅନ୍ୟଥା {
        UnicodeString localDest;
        UnicodeString *dest;

        ଯଦି(&source!=&result) {
            dest=&result;
        } ଅନ୍ୟଥା {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        ଯଦି(U_SUCCESS(status)) {
            ଯଦି(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } ଅନ୍ୟଥା {
                n2->normalize(source, *dest, status);
            }
        }
        ଯଦି(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

ଶୂନ୍ୟ U_EXPORT2
Normalizer::compose(ଅଚଳ UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

ଶୂନ୍ୟ U_EXPORT2
Normalizer::decompose(ଅଚଳ UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(ଅଚଳ UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    ଯଦି(U_SUCCESS(status)) {
        ଯଦି(options&UNORM_UNICODE_3_2) {
            ଫେରନ୍ତୁ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ n2->quickCheck(source, status);
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ଅଚଳ UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    ଯଦି(U_SUCCESS(status)) {
        ଯଦି(options&UNORM_UNICODE_3_2) {
            ଫେରନ୍ତୁ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ n2->isNormalized(source, status);
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ଅଚଳ UnicodeString& left,
                        ଅଚଳ UnicodeString& ଡାହାଣ,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    ଯଦି(left.isBogus() || ଡାହାଣ.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        ଯଦି(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } ଅନ୍ୟଥା {
        UnicodeString localDest;
        UnicodeString *dest;

        ଯଦି(&ଡାହାଣ!=&result) {
            dest=&result;
        } ଅନ୍ୟଥା {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        ଯଦି(U_SUCCESS(errorCode)) {
            ଯଦି(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, ଡାହାଣ, errorCode);
            } ଅନ୍ୟଥା {
                n2->append(*dest, ଡାହାଣ, errorCode);
            }
        }
        ଯଦି(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    ଫେରନ୍ତୁ result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    ଯଦି(bufferPos<buffer.length() || nextNormalize()) {
        ଫେରନ୍ତୁ buffer.char32At(bufferPos);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    ଯଦି(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    ଯଦି(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ DONE;
    }
}

ଶୂନ୍ୟ Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

ଶୂନ୍ୟ
Normalizer::setIndexOnly(int32_t index) {
    text->setIndex(index);  // pins index
    currentIndex=nextIndex=text->getIndex();
    clearBuffer();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    ଫେରନ୍ତୁ next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    ଫେରନ୍ତୁ previous();
}

/**
 * Retrieve the current iteration position in the input text that is
 * being normalized.  This method is useful in applications such as
 * searching, where you need to be able to determine the position in
 * the input text that corresponds to a given normalized output character.
 * <p>
 * <b>Note:</b> This method sets the position in the <em>input</em>, while
 * {@link #next} and {@link #previous} iterate through characters in the
 * <em>output</em>.  This means that there is not necessarily a one-to-one
 * correspondence between characters returned by <tt>next</tt> and
 * <tt>previous</tt> and the indices passed to and returned from
 * <tt>setIndex</tt> and {@link #getIndex}.
 *
 */
int32_t Normalizer::getIndex() ଅଚଳ {
    ଯଦି(bufferPos<buffer.length()) {
        ଫେରନ୍ତୁ currentIndex;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() ଅଚଳ {
    ଫେରନ୍ତୁ text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() ଅଚଳ {
    ଫେରନ୍ତୁ text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

ଶୂନ୍ୟ
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() ଅଚଳ
{
    ଫେରନ୍ତୁ fUMode;
}

ଶୂନ୍ୟ
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    ଯଦି (value) {
        fOptions |= option;
    } ଅନ୍ୟଥା {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ଅଚଳ
{
    ଫେରନ୍ତୁ (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
ଶୂନ୍ୟ
Normalizer::setText(ଅଚଳ UnicodeString& newText, 
                    UErrorCode &status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    CharacterIterator *newIter = ନୂତନ StringCharacterIterator(newText);
    ଯଦି (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
ଶୂନ୍ୟ
Normalizer::setText(ଅଚଳ CharacterIterator& newText, 
                    UErrorCode &status) 
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    CharacterIterator *newIter = newText.clone();
    ଯଦି (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    delete text;
    text = newIter;
    reset();
}

ଶୂନ୍ୟ
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    CharacterIterator *newIter = ନୂତନ UCharCharacterIterator(newText, length);
    ଯଦି (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
ଶୂନ୍ୟ
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

ଶୂନ୍ୟ Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    ଯଦି(!text->hasNext()) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    ଯେତେବେଳେ(text->hasNext()) {
        UChar32 c;
        ଯଦି(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            ଭାଙ୍ଗ;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    ଯଦି(!text->hasPrevious()) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    UnicodeString segment;
    ଯେତେବେଳେ(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        ଯଦି(fNorm2->hasBoundaryBefore(c)) {
            ଭାଙ୍ଗ;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#include "unicode/utypes.h"

#nếu !UCONFIG_NO_NORMALIZATION

#include "unicode/uniset.h"
#include "unicode/unistr.h"
#include "unicode/chariter.h"
#include "unicode/schriter.h"
#include "unicode/uchriter.h"
#include "unicode/normlzr.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "normalizer2impl.h"
#include "uprops.h"  // for uniset_getUnicode32Instance()

#nếu defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(const UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(new StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(new UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(const CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(const Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

void
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    nếu(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            new FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    nếu(U_FAILURE(errorCode)) {
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
Normalizer::clone() const
{
    trả_về new Normalizer(*this);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() const
{
    trả_về text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
bool Normalizer::operator==(const Normalizer& that) const
{
    trả_về
        this==&that ||
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

void U_EXPORT2
Normalizer::normalize(const UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    nếu(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        nếu(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } không_thì {
        UnicodeString localDest;
        UnicodeString *dest;

        nếu(&source!=&result) {
            dest=&result;
        } không_thì {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        nếu(U_SUCCESS(status)) {
            nếu(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } không_thì {
                n2->normalize(source, *dest, status);
            }
        }
        nếu(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

void U_EXPORT2
Normalizer::compose(const UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

void U_EXPORT2
Normalizer::decompose(const UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(const UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    nếu(U_SUCCESS(status)) {
        nếu(options&UNORM_UNICODE_3_2) {
            trả_về FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } không_thì {
            trả_về n2->quickCheck(source, status);
        }
    } không_thì {
        trả_về UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(const UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    nếu(U_SUCCESS(status)) {
        nếu(options&UNORM_UNICODE_3_2) {
            trả_về FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } không_thì {
            trả_về n2->isNormalized(source, status);
        }
    } không_thì {
        trả_về false;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(const UnicodeString& left,
                        const UnicodeString& right,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    nếu(left.isBogus() || right.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        nếu(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } không_thì {
        UnicodeString localDest;
        UnicodeString *dest;

        nếu(&right!=&result) {
            dest=&result;
        } không_thì {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        nếu(U_SUCCESS(errorCode)) {
            nếu(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, right, errorCode);
            } không_thì {
                n2->append(*dest, right, errorCode);
            }
        }
        nếu(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    trả_về result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    nếu(bufferPos<buffer.length() || nextNormalize()) {
        trả_về buffer.char32At(bufferPos);
    } không_thì {
        trả_về DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    nếu(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        trả_về c;
    } không_thì {
        trả_về DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    nếu(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        trả_về c;
    } không_thì {
        trả_về DONE;
    }
}

void Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

void
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
    trả_về next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    trả_về previous();
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
int32_t Normalizer::getIndex() const {
    nếu(bufferPos<buffer.length()) {
        trả_về currentIndex;
    } không_thì {
        trả_về nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() const {
    trả_về text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() const {
    trả_về text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

void
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() const
{
    trả_về fUMode;
}

void
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    nếu (value) {
        fOptions |= option;
    } không_thì {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) const
{
    trả_về (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
void
Normalizer::setText(const UnicodeString& newText, 
                    UErrorCode &status)
{
    nếu (U_FAILURE(status)) {
        trả_về;
    }
    CharacterIterator *newIter = new StringCharacterIterator(newText);
    nếu (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
void
Normalizer::setText(const CharacterIterator& newText, 
                    UErrorCode &status) 
{
    nếu (U_FAILURE(status)) {
        trả_về;
    }
    CharacterIterator *newIter = newText.clone();
    nếu (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
    }
    delete text;
    text = newIter;
    reset();
}

void
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    nếu (U_FAILURE(status)) {
        trả_về;
    }
    CharacterIterator *newIter = new UCharCharacterIterator(newText, length);
    nếu (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
void
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

void Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    nếu(!text->hasNext()) {
        trả_về false;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    trong_khi(text->hasNext()) {
        UChar32 c;
        nếu(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            break;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    trả_về U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    nếu(!text->hasPrevious()) {
        trả_về false;
    }
    UnicodeString segment;
    trong_khi(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        nếu(fNorm2->hasBoundaryBefore(c)) {
            break;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    trả_về U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

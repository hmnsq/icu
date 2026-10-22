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

#གལ་སྲིད !UCONFIG_NO_NORMALIZATION

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

#གལ་སྲིད defined(move32)
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
    གལ་སྲིད(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            new FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    གལ་སྲིད(U_FAILURE(errorCode)) {
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
    ལོག new Normalizer(*this);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() const
{
    ལོག text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
bool Normalizer::operator==(const Normalizer& that) const
{
    ལོག
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
    གལ་སྲིད(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        གལ་སྲིད(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } དེ་མིན {
        UnicodeString localDest;
        UnicodeString *dest;

        གལ་སྲིད(&source!=&result) {
            dest=&result;
        } དེ་མིན {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        གལ་སྲིད(U_SUCCESS(status)) {
            གལ་སྲིད(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } དེ་མིན {
                n2->normalize(source, *dest, status);
            }
        }
        གལ་སྲིད(dest==&localDest && U_SUCCESS(status)) {
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
    གལ་སྲིད(U_SUCCESS(status)) {
        གལ་སྲིད(options&UNORM_UNICODE_3_2) {
            ལོག FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } དེ་མིན {
            ལོག n2->quickCheck(source, status);
        }
    } དེ་མིན {
        ལོག UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(const UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    གལ་སྲིད(U_SUCCESS(status)) {
        གལ་སྲིད(options&UNORM_UNICODE_3_2) {
            ལོག FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } དེ་མིན {
            ལོག n2->isNormalized(source, status);
        }
    } དེ་མིན {
        ལོག false;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(const UnicodeString& left,
                        const UnicodeString& right,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    གལ་སྲིད(left.isBogus() || right.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        གལ་སྲིད(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } དེ་མིན {
        UnicodeString localDest;
        UnicodeString *dest;

        གལ་སྲིད(&right!=&result) {
            dest=&result;
        } དེ་མིན {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        གལ་སྲིད(U_SUCCESS(errorCode)) {
            གལ་སྲིད(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, right, errorCode);
            } དེ་མིན {
                n2->append(*dest, right, errorCode);
            }
        }
        གལ་སྲིད(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    ལོག result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    གལ་སྲིད(bufferPos<buffer.length() || nextNormalize()) {
        ལོག buffer.char32At(bufferPos);
    } དེ་མིན {
        ལོག DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    གལ་སྲིད(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        ལོག c;
    } དེ་མིན {
        ལོག DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    གལ་སྲིད(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        ལོག c;
    } དེ་མིན {
        ལོག DONE;
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
    ལོག next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    ལོག previous();
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
    གལ་སྲིད(bufferPos<buffer.length()) {
        ལོག currentIndex;
    } དེ་མིན {
        ལོག nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() const {
    ལོག text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() const {
    ལོག text->endIndex();
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
    ལོག fUMode;
}

void
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    གལ་སྲིད (value) {
        fOptions |= option;
    } དེ་མིན {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) const
{
    ལོག (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
void
Normalizer::setText(const UnicodeString& newText, 
                    UErrorCode &status)
{
    གལ་སྲིད (U_FAILURE(status)) {
        ལོག;
    }
    CharacterIterator *newIter = new StringCharacterIterator(newText);
    གལ་སྲིད (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ལོག;
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
    གལ་སྲིད (U_FAILURE(status)) {
        ལོག;
    }
    CharacterIterator *newIter = newText.clone();
    གལ་སྲིད (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ལོག;
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
    གལ་སྲིད (U_FAILURE(status)) {
        ལོག;
    }
    CharacterIterator *newIter = new UCharCharacterIterator(newText, length);
    གལ་སྲིད (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ལོག;
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
    གལ་སྲིད(!text->hasNext()) {
        ལོག false;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    སྐབས(text->hasNext()) {
        UChar32 c;
        གལ་སྲིད(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            bha_nga;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    ལོག U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    གལ་སྲིད(!text->hasPrevious()) {
        ལོག false;
    }
    UnicodeString segment;
    སྐབས(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        གལ་སྲིད(fNorm2->hasBoundaryBefore(c)) {
            bha_nga;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    ལོག U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

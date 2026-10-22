// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION

#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/chariter.h"
#അന്തര്ഭാവ "unicode/schriter.h"
#അന്തര്ഭാവ "unicode/uchriter.h"
#അന്തര്ഭാവ "unicode/normlzr.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "normalizer2impl.h"
#അന്തര്ഭാവ "uprops.h"  // for uniset_getUnicode32Instance()

#എങ്കിൽ defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(സ്ഥിരം UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(പുതിയ StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(പുതിയ UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(സ്ഥിരം CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(സ്ഥിരം Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

ശൂന്യം
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    എങ്കിൽ(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            പുതിയ FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    എങ്കിൽ(U_FAILURE(errorCode)) {
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
Normalizer::clone() സ്ഥിരം
{
    തിരികെ പുതിയ Normalizer(*ഇത്);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() സ്ഥിരം
{
    തിരികെ text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
തർക്കം Normalizer::operator==(സ്ഥിരം Normalizer& that) സ്ഥിരം
{
    തിരികെ
        ഇത്==&that ||
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

ശൂന്യം U_EXPORT2
Normalizer::normalize(സ്ഥിരം UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    എങ്കിൽ(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        എങ്കിൽ(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } അല്ലെങ്കിൽ {
        UnicodeString localDest;
        UnicodeString *dest;

        എങ്കിൽ(&source!=&result) {
            dest=&result;
        } അല്ലെങ്കിൽ {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        എങ്കിൽ(U_SUCCESS(status)) {
            എങ്കിൽ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } അല്ലെങ്കിൽ {
                n2->normalize(source, *dest, status);
            }
        }
        എങ്കിൽ(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

ശൂന്യം U_EXPORT2
Normalizer::compose(സ്ഥിരം UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

ശൂന്യം U_EXPORT2
Normalizer::decompose(സ്ഥിരം UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(സ്ഥിരം UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    എങ്കിൽ(U_SUCCESS(status)) {
        എങ്കിൽ(options&UNORM_UNICODE_3_2) {
            തിരികെ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } അല്ലെങ്കിൽ {
            തിരികെ n2->quickCheck(source, status);
        }
    } അല്ലെങ്കിൽ {
        തിരികെ UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(സ്ഥിരം UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    എങ്കിൽ(U_SUCCESS(status)) {
        എങ്കിൽ(options&UNORM_UNICODE_3_2) {
            തിരികെ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } അല്ലെങ്കിൽ {
            തിരികെ n2->isNormalized(source, status);
        }
    } അല്ലെങ്കിൽ {
        തിരികെ അസത്യം;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(സ്ഥിരം UnicodeString& left,
                        സ്ഥിരം UnicodeString& വലത്,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    എങ്കിൽ(left.isBogus() || വലത്.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        എങ്കിൽ(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } അല്ലെങ്കിൽ {
        UnicodeString localDest;
        UnicodeString *dest;

        എങ്കിൽ(&വലത്!=&result) {
            dest=&result;
        } അല്ലെങ്കിൽ {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        എങ്കിൽ(U_SUCCESS(errorCode)) {
            എങ്കിൽ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, വലത്, errorCode);
            } അല്ലെങ്കിൽ {
                n2->append(*dest, വലത്, errorCode);
            }
        }
        എങ്കിൽ(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    തിരികെ result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    എങ്കിൽ(bufferPos<buffer.length() || nextNormalize()) {
        തിരികെ buffer.char32At(bufferPos);
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    എങ്കിൽ(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    എങ്കിൽ(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        തിരികെ c;
    } അല്ലെങ്കിൽ {
        തിരികെ DONE;
    }
}

ശൂന്യം Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

ശൂന്യം
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
    തിരികെ next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    തിരികെ previous();
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
int32_t Normalizer::getIndex() സ്ഥിരം {
    എങ്കിൽ(bufferPos<buffer.length()) {
        തിരികെ currentIndex;
    } അല്ലെങ്കിൽ {
        തിരികെ nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() സ്ഥിരം {
    തിരികെ text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() സ്ഥിരം {
    തിരികെ text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

ശൂന്യം
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() സ്ഥിരം
{
    തിരികെ fUMode;
}

ശൂന്യം
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    എങ്കിൽ (value) {
        fOptions |= option;
    } അല്ലെങ്കിൽ {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) സ്ഥിരം
{
    തിരികെ (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
ശൂന്യം
Normalizer::setText(സ്ഥിരം UnicodeString& newText, 
                    UErrorCode &status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    CharacterIterator *newIter = പുതിയ StringCharacterIterator(newText);
    എങ്കിൽ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
ശൂന്യം
Normalizer::setText(സ്ഥിരം CharacterIterator& newText, 
                    UErrorCode &status) 
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    CharacterIterator *newIter = newText.clone();
    എങ്കിൽ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    delete text;
    text = newIter;
    reset();
}

ശൂന്യം
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    CharacterIterator *newIter = പുതിയ UCharCharacterIterator(newText, length);
    എങ്കിൽ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
ശൂന്യം
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

ശൂന്യം Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    എങ്കിൽ(!text->hasNext()) {
        തിരികെ അസത്യം;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    ആകുമ്പോൾ(text->hasNext()) {
        UChar32 c;
        എങ്കിൽ(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            നിർത്തുക;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    തിരികെ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    എങ്കിൽ(!text->hasPrevious()) {
        തിരികെ അസത്യം;
    }
    UnicodeString segment;
    ആകുമ്പോൾ(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        എങ്കിൽ(fNorm2->hasBoundaryBefore(c)) {
            നിർത്തുക;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    തിരികെ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

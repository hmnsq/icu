// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/schriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/normlzr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"  // for uniset_getUnicode32Instance()

#ಯದಿ defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ಧ್ರುವ UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(ನೂತನ StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(ನೂತನ UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ಧ್ರುವ CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ಧ್ರುವ Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

ಶೂನ್ಯ
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    ಯದಿ(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            ನೂತನ FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    ಯದಿ(U_FAILURE(errorCode)) {
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
Normalizer::clone() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ ನೂತನ Normalizer(*ಏತತ್);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
ತಾರ್ಕಿಕ Normalizer::operator==(ಧ್ರುವ Normalizer& that) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ
        ಏತತ್==&that ||
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

ಶೂನ್ಯ U_EXPORT2
Normalizer::normalize(ಧ್ರುವ UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    ಯದಿ(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        ಯದಿ(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } ಅನ್ಯಥಾ {
        UnicodeString localDest;
        UnicodeString *dest;

        ಯದಿ(&source!=&result) {
            dest=&result;
        } ಅನ್ಯಥಾ {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        ಯದಿ(U_SUCCESS(status)) {
            ಯದಿ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } ಅನ್ಯಥಾ {
                n2->normalize(source, *dest, status);
            }
        }
        ಯದಿ(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

ಶೂನ್ಯ U_EXPORT2
Normalizer::compose(ಧ್ರುವ UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

ಶೂನ್ಯ U_EXPORT2
Normalizer::decompose(ಧ್ರುವ UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(ಧ್ರುವ UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    ಯದಿ(U_SUCCESS(status)) {
        ಯದಿ(options&UNORM_UNICODE_3_2) {
            ಪ್ರತ್ಯಾಗಮ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ n2->quickCheck(source, status);
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ಧ್ರುವ UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    ಯದಿ(U_SUCCESS(status)) {
        ಯದಿ(options&UNORM_UNICODE_3_2) {
            ಪ್ರತ್ಯಾಗಮ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ n2->isNormalized(source, status);
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ಧ್ರುವ UnicodeString& left,
                        ಧ್ರುವ UnicodeString& ಬಲ,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    ಯದಿ(left.isBogus() || ಬಲ.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        ಯದಿ(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } ಅನ್ಯಥಾ {
        UnicodeString localDest;
        UnicodeString *dest;

        ಯದಿ(&ಬಲ!=&result) {
            dest=&result;
        } ಅನ್ಯಥಾ {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        ಯದಿ(U_SUCCESS(errorCode)) {
            ಯದಿ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, ಬಲ, errorCode);
            } ಅನ್ಯಥಾ {
                n2->append(*dest, ಬಲ, errorCode);
            }
        }
        ಯದಿ(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    ಯದಿ(bufferPos<buffer.length() || nextNormalize()) {
        ಪ್ರತ್ಯಾಗಮ buffer.char32At(bufferPos);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    ಯದಿ(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    ಯದಿ(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ DONE;
    }
}

ಶೂನ್ಯ Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

ಶೂನ್ಯ
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
    ಪ್ರತ್ಯಾಗಮ next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    ಪ್ರತ್ಯಾಗಮ previous();
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
int32_t Normalizer::getIndex() ಧ್ರುವ {
    ಯದಿ(bufferPos<buffer.length()) {
        ಪ್ರತ್ಯಾಗಮ currentIndex;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

ಶೂನ್ಯ
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ fUMode;
}

ಶೂನ್ಯ
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    ಯದಿ (value) {
        fOptions |= option;
    } ಅನ್ಯಥಾ {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
ಶೂನ್ಯ
Normalizer::setText(ಧ್ರುವ UnicodeString& newText, 
                    UErrorCode &status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    CharacterIterator *newIter = ನೂತನ StringCharacterIterator(newText);
    ಯದಿ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
ಶೂನ್ಯ
Normalizer::setText(ಧ್ರುವ CharacterIterator& newText, 
                    UErrorCode &status) 
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    CharacterIterator *newIter = newText.clone();
    ಯದಿ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    delete text;
    text = newIter;
    reset();
}

ಶೂನ್ಯ
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    CharacterIterator *newIter = ನೂತನ UCharCharacterIterator(newText, length);
    ಯದಿ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
ಶೂನ್ಯ
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

ಶೂನ್ಯ Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    ಯದಿ(!text->hasNext()) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    ಯಾವತ್(text->hasNext()) {
        UChar32 c;
        ಯದಿ(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            ಭಙ್ಗ;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    ಯದಿ(!text->hasPrevious()) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    UnicodeString segment;
    ಯಾವತ್(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        ಯದಿ(fNorm2->hasBoundaryBefore(c)) {
            ಭಙ್ಗ;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

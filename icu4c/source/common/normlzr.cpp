// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/chariter.h"
#अन्तर्भाव "unicode/schriter.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/normlzr.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "uprops.h"  // for uniset_getUnicode32Instance()

#यदि defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(अचर UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(नूतन StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(नूतन UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(अचर CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(अचर Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

शून्य
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    यदि(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            नूतन FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    यदि(U_FAILURE(errorCode)) {
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
Normalizer::clone() अचर
{
    फर्काऊ नूतन Normalizer(*एतत्);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() अचर
{
    फर्काऊ text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
तार्किक Normalizer::चालक==(अचर Normalizer& that) अचर
{
    फर्काऊ
        एतत्==&that ||
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

शून्य U_EXPORT2
Normalizer::normalize(अचर UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    यदि(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        यदि(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } अन्यथा {
        UnicodeString localDest;
        UnicodeString *dest;

        यदि(&source!=&result) {
            dest=&result;
        } अन्यथा {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        यदि(U_SUCCESS(status)) {
            यदि(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } अन्यथा {
                n2->normalize(source, *dest, status);
            }
        }
        यदि(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

शून्य U_EXPORT2
Normalizer::compose(अचर UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

शून्य U_EXPORT2
Normalizer::decompose(अचर UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(अचर UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    यदि(U_SUCCESS(status)) {
        यदि(options&UNORM_UNICODE_3_2) {
            फर्काऊ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } अन्यथा {
            फर्काऊ n2->quickCheck(source, status);
        }
    } अन्यथा {
        फर्काऊ UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(अचर UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    यदि(U_SUCCESS(status)) {
        यदि(options&UNORM_UNICODE_3_2) {
            फर्काऊ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } अन्यथा {
            फर्काऊ n2->isNormalized(source, status);
        }
    } अन्यथा {
        फर्काऊ असत्य;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(अचर UnicodeString& left,
                        अचर UnicodeString& दायाँ,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    यदि(left.isBogus() || दायाँ.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        यदि(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } अन्यथा {
        UnicodeString localDest;
        UnicodeString *dest;

        यदि(&दायाँ!=&result) {
            dest=&result;
        } अन्यथा {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        यदि(U_SUCCESS(errorCode)) {
            यदि(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, दायाँ, errorCode);
            } अन्यथा {
                n2->append(*dest, दायाँ, errorCode);
            }
        }
        यदि(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    फर्काऊ result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    यदि(bufferPos<buffer.length() || nextNormalize()) {
        फर्काऊ buffer.char32At(bufferPos);
    } अन्यथा {
        फर्काऊ DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    यदि(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    यदि(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ DONE;
    }
}

शून्य Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

शून्य
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
    फर्काऊ next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    फर्काऊ previous();
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
int32_t Normalizer::getIndex() अचर {
    यदि(bufferPos<buffer.length()) {
        फर्काऊ currentIndex;
    } अन्यथा {
        फर्काऊ nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() अचर {
    फर्काऊ text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() अचर {
    फर्काऊ text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

शून्य
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() अचर
{
    फर्काऊ fUMode;
}

शून्य
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    यदि (value) {
        fOptions |= option;
    } अन्यथा {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) अचर
{
    फर्काऊ (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
शून्य
Normalizer::setText(अचर UnicodeString& newText, 
                    UErrorCode &status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    CharacterIterator *newIter = नूतन StringCharacterIterator(newText);
    यदि (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
शून्य
Normalizer::setText(अचर CharacterIterator& newText, 
                    UErrorCode &status) 
{
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    CharacterIterator *newIter = newText.clone();
    यदि (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    delete text;
    text = newIter;
    reset();
}

शून्य
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    CharacterIterator *newIter = नूतन UCharCharacterIterator(newText, length);
    यदि (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
शून्य
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

शून्य Normalizer::clearBuffer() {
    buffer.हटाओ();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    यदि(!text->hasNext()) {
        फर्काऊ असत्य;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    जबसम्म(text->hasNext()) {
        UChar32 c;
        यदि(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            रोक;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    फर्काऊ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    यदि(!text->hasPrevious()) {
        फर्काऊ असत्य;
    }
    UnicodeString segment;
    जबसम्म(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        यदि(fNorm2->hasBoundaryBefore(c)) {
            रोक;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    फर्काऊ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

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

#जर !UCONFIG_NO_NORMALIZATION

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

#जर defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(स्थिरांक UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(नवीन StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(नवीन UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(स्थिरांक CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(स्थिरांक Normalizer &copy) :
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
    जर(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            नवीन FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    जर(U_FAILURE(errorCode)) {
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
Normalizer::clone() स्थिरांक
{
    परत नवीन Normalizer(*हे);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() स्थिरांक
{
    परत text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
तार्किक Normalizer::चालक==(स्थिरांक Normalizer& that) स्थिरांक
{
    परत
        हे==&that ||
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
Normalizer::normalize(स्थिरांक UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    जर(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        जर(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } अन्यथा {
        UnicodeString localDest;
        UnicodeString *dest;

        जर(&source!=&result) {
            dest=&result;
        } अन्यथा {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        जर(U_SUCCESS(status)) {
            जर(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } अन्यथा {
                n2->normalize(source, *dest, status);
            }
        }
        जर(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

शून्य U_EXPORT2
Normalizer::compose(स्थिरांक UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

शून्य U_EXPORT2
Normalizer::decompose(स्थिरांक UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(स्थिरांक UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    जर(U_SUCCESS(status)) {
        जर(options&UNORM_UNICODE_3_2) {
            परत FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } अन्यथा {
            परत n2->quickCheck(source, status);
        }
    } अन्यथा {
        परत UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(स्थिरांक UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    जर(U_SUCCESS(status)) {
        जर(options&UNORM_UNICODE_3_2) {
            परत FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } अन्यथा {
            परत n2->isNormalized(source, status);
        }
    } अन्यथा {
        परत असत्य;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(स्थिरांक UnicodeString& left,
                        स्थिरांक UnicodeString& उजवा,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    जर(left.isBogus() || उजवा.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        जर(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } अन्यथा {
        UnicodeString localDest;
        UnicodeString *dest;

        जर(&उजवा!=&result) {
            dest=&result;
        } अन्यथा {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        जर(U_SUCCESS(errorCode)) {
            जर(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, उजवा, errorCode);
            } अन्यथा {
                n2->append(*dest, उजवा, errorCode);
            }
        }
        जर(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    परत result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    जर(bufferPos<buffer.length() || nextNormalize()) {
        परत buffer.char32At(bufferPos);
    } अन्यथा {
        परत DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    जर(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        परत c;
    } अन्यथा {
        परत DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    जर(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        परत c;
    } अन्यथा {
        परत DONE;
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
    परत next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    परत previous();
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
int32_t Normalizer::getIndex() स्थिरांक {
    जर(bufferPos<buffer.length()) {
        परत currentIndex;
    } अन्यथा {
        परत nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() स्थिरांक {
    परत text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() स्थिरांक {
    परत text->endIndex();
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
Normalizer::getUMode() स्थिरांक
{
    परत fUMode;
}

शून्य
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    जर (value) {
        fOptions |= option;
    } अन्यथा {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) स्थिरांक
{
    परत (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
शून्य
Normalizer::setText(स्थिरांक UnicodeString& newText, 
                    UErrorCode &status)
{
    जर (U_FAILURE(status)) {
        परत;
    }
    CharacterIterator *newIter = नवीन StringCharacterIterator(newText);
    जर (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
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
Normalizer::setText(स्थिरांक CharacterIterator& newText, 
                    UErrorCode &status) 
{
    जर (U_FAILURE(status)) {
        परत;
    }
    CharacterIterator *newIter = newText.clone();
    जर (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
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
    जर (U_FAILURE(status)) {
        परत;
    }
    CharacterIterator *newIter = नवीन UCharCharacterIterator(newText, length);
    जर (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
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
    जर(!text->hasNext()) {
        परत असत्य;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    पर्यंत(text->hasNext()) {
        UChar32 c;
        जर(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            थांबा;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    परत U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    जर(!text->hasPrevious()) {
        परत असत्य;
    }
    UnicodeString segment;
    पर्यंत(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        जर(fNorm2->hasBoundaryBefore(c)) {
            थांबा;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    परत U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

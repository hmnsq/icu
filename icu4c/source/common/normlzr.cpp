// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_NORMALIZATION

#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/chariter.h"
#அந்தர்பாவ "unicode/schriter.h"
#அந்தர்பாவ "unicode/uchriter.h"
#அந்தர்பாவ "unicode/normlzr.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "normalizer2impl.h"
#அந்தர்பாவ "uprops.h"  // for uniset_getUnicode32Instance()

#எனில் defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(மாறிலி UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(புதிய StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(புதிய UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(மாறிலி CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(மாறிலி Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

வெறுமை
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    எனில்(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            புதிய FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    எனில்(U_FAILURE(errorCode)) {
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
Normalizer::clone() மாறிலி
{
    திரும்பு புதிய Normalizer(*இது);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() மாறிலி
{
    திரும்பு text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
தருக்க Normalizer::operator==(மாறிலி Normalizer& that) மாறிலி
{
    திரும்பு
        இது==&that ||
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

வெறுமை U_EXPORT2
Normalizer::normalize(மாறிலி UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    எனில்(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        எனில்(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } இல்லையெனில் {
        UnicodeString localDest;
        UnicodeString *dest;

        எனில்(&source!=&result) {
            dest=&result;
        } இல்லையெனில் {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        எனில்(U_SUCCESS(status)) {
            எனில்(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } இல்லையெனில் {
                n2->normalize(source, *dest, status);
            }
        }
        எனில்(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

வெறுமை U_EXPORT2
Normalizer::compose(மாறிலி UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

வெறுமை U_EXPORT2
Normalizer::decompose(மாறிலி UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(மாறிலி UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    எனில்(U_SUCCESS(status)) {
        எனில்(options&UNORM_UNICODE_3_2) {
            திரும்பு FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } இல்லையெனில் {
            திரும்பு n2->quickCheck(source, status);
        }
    } இல்லையெனில் {
        திரும்பு UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(மாறிலி UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    எனில்(U_SUCCESS(status)) {
        எனில்(options&UNORM_UNICODE_3_2) {
            திரும்பு FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } இல்லையெனில் {
            திரும்பு n2->isNormalized(source, status);
        }
    } இல்லையெனில் {
        திரும்பு பொய்;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(மாறிலி UnicodeString& left,
                        மாறிலி UnicodeString& வலது,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    எனில்(left.isBogus() || வலது.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        எனில்(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } இல்லையெனில் {
        UnicodeString localDest;
        UnicodeString *dest;

        எனில்(&வலது!=&result) {
            dest=&result;
        } இல்லையெனில் {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        எனில்(U_SUCCESS(errorCode)) {
            எனில்(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, வலது, errorCode);
            } இல்லையெனில் {
                n2->append(*dest, வலது, errorCode);
            }
        }
        எனில்(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    திரும்பு result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    எனில்(bufferPos<buffer.length() || nextNormalize()) {
        திரும்பு buffer.char32At(bufferPos);
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    எனில்(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    எனில்(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு DONE;
    }
}

வெறுமை Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

வெறுமை
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
    திரும்பு next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    திரும்பு previous();
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
int32_t Normalizer::getIndex() மாறிலி {
    எனில்(bufferPos<buffer.length()) {
        திரும்பு currentIndex;
    } இல்லையெனில் {
        திரும்பு nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() மாறிலி {
    திரும்பு text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() மாறிலி {
    திரும்பு text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

வெறுமை
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() மாறிலி
{
    திரும்பு fUMode;
}

வெறுமை
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    எனில் (value) {
        fOptions |= option;
    } இல்லையெனில் {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) மாறிலி
{
    திரும்பு (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
வெறுமை
Normalizer::setText(மாறிலி UnicodeString& newText, 
                    UErrorCode &status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    CharacterIterator *newIter = புதிய StringCharacterIterator(newText);
    எனில் (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
வெறுமை
Normalizer::setText(மாறிலி CharacterIterator& newText, 
                    UErrorCode &status) 
{
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    CharacterIterator *newIter = newText.clone();
    எனில் (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    delete text;
    text = newIter;
    reset();
}

வெறுமை
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    CharacterIterator *newIter = புதிய UCharCharacterIterator(newText, length);
    எனில் (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
வெறுமை
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

வெறுமை Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    எனில்(!text->hasNext()) {
        திரும்பு பொய்;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    போது(text->hasNext()) {
        UChar32 c;
        எனில்(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            நிறுத்து;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    திரும்பு U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    எனில்(!text->hasPrevious()) {
        திரும்பு பொய்;
    }
    UnicodeString segment;
    போது(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        எனில்(fNorm2->hasBoundaryBefore(c)) {
            நிறுத்து;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    திரும்பு U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

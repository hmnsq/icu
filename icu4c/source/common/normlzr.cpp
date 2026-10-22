// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/uniset.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/chariter.h"
#ضمن "unicode/schriter.h"
#ضمن "unicode/uchriter.h"
#ضمن "unicode/normlzr.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "normalizer2impl.h"
#ضمن "uprops.h"  // for uniset_getUnicode32Instance()

#ܐܢ defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ثابت UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(جديد StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(جديد UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ثابت CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ثابت Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

فراغ
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    ܐܢ(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            جديد FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    ܐܢ(U_FAILURE(errorCode)) {
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
Normalizer::clone() ثابت
{
    ܦܢܐ جديد Normalizer(*هذا);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() ثابت
{
    ܦܢܐ text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
منطقي Normalizer::operator==(ثابت Normalizer& that) ثابت
{
    ܦܢܐ
        هذا==&that ||
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

فراغ U_EXPORT2
Normalizer::normalize(ثابت UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    ܐܢ(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        ܐܢ(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } ܐܚܪܢܐ {
        UnicodeString localDest;
        UnicodeString *dest;

        ܐܢ(&source!=&result) {
            dest=&result;
        } ܐܚܪܢܐ {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        ܐܢ(U_SUCCESS(status)) {
            ܐܢ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } ܐܚܪܢܐ {
                n2->normalize(source, *dest, status);
            }
        }
        ܐܢ(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

فراغ U_EXPORT2
Normalizer::compose(ثابت UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

فراغ U_EXPORT2
Normalizer::decompose(ثابت UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(ثابت UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    ܐܢ(U_SUCCESS(status)) {
        ܐܢ(options&UNORM_UNICODE_3_2) {
            ܦܢܐ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } ܐܚܪܢܐ {
            ܦܢܐ n2->quickCheck(source, status);
        }
    } ܐܚܪܢܐ {
        ܦܢܐ UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ثابت UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    ܐܢ(U_SUCCESS(status)) {
        ܐܢ(options&UNORM_UNICODE_3_2) {
            ܦܢܐ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } ܐܚܪܢܐ {
            ܦܢܐ n2->isNormalized(source, status);
        }
    } ܐܚܪܢܐ {
        ܦܢܐ ܕܓܠܐ;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ثابت UnicodeString& left,
                        ثابت UnicodeString& right,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    ܐܢ(left.isBogus() || right.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        ܐܢ(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } ܐܚܪܢܐ {
        UnicodeString localDest;
        UnicodeString *dest;

        ܐܢ(&right!=&result) {
            dest=&result;
        } ܐܚܪܢܐ {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        ܐܢ(U_SUCCESS(errorCode)) {
            ܐܢ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, right, errorCode);
            } ܐܚܪܢܐ {
                n2->append(*dest, right, errorCode);
            }
        }
        ܐܢ(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    ܦܢܐ result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    ܐܢ(bufferPos<buffer.length() || nextNormalize()) {
        ܦܢܐ buffer.char32At(bufferPos);
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    ܐܢ(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    ܐܢ(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        ܦܢܐ c;
    } ܐܚܪܢܐ {
        ܦܢܐ DONE;
    }
}

فراغ Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

فراغ
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
    ܦܢܐ next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    ܦܢܐ previous();
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
int32_t Normalizer::getIndex() ثابت {
    ܐܢ(bufferPos<buffer.length()) {
        ܦܢܐ currentIndex;
    } ܐܚܪܢܐ {
        ܦܢܐ nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() ثابت {
    ܦܢܐ text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() ثابت {
    ܦܢܐ text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

فراغ
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() ثابت
{
    ܦܢܐ fUMode;
}

فراغ
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    ܐܢ (value) {
        fOptions |= option;
    } ܐܚܪܢܐ {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ثابت
{
    ܦܢܐ (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
فراغ
Normalizer::setText(ثابت UnicodeString& newText, 
                    UErrorCode &status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    CharacterIterator *newIter = جديد StringCharacterIterator(newText);
    ܐܢ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
فراغ
Normalizer::setText(ثابت CharacterIterator& newText, 
                    UErrorCode &status) 
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    CharacterIterator *newIter = newText.clone();
    ܐܢ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    delete text;
    text = newIter;
    reset();
}

فراغ
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    CharacterIterator *newIter = جديد UCharCharacterIterator(newText, length);
    ܐܢ (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
فراغ
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

فراغ Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    ܐܢ(!text->hasNext()) {
        ܦܢܐ ܕܓܠܐ;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    ܟܡܐ(text->hasNext()) {
        UChar32 c;
        ܐܢ(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            اكسر;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    ܦܢܐ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    ܐܢ(!text->hasPrevious()) {
        ܦܢܐ ܕܓܠܐ;
    }
    UnicodeString segment;
    ܟܡܐ(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        ܐܢ(fNorm2->hasBoundaryBefore(c)) {
            اكسر;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    ܦܢܐ U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

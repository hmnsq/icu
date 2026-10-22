// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/uniset.h"
#شامل "unicode/unistr.h"
#شامل "unicode/chariter.h"
#شامل "unicode/schriter.h"
#شامل "unicode/uchriter.h"
#شامل "unicode/normlzr.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "normalizer2impl.h"
#شامل "uprops.h"  // for uniset_getUnicode32Instance()

#اگر defined(move32)
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
    text(جدید StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(جدید UCharCharacterIterator(str, length)),
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

تهی
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    اگر(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            جدید FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    اگر(U_FAILURE(errorCode)) {
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
    بازگشت جدید Normalizer(*این);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() ثابت
{
    بازگشت text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
منطقی Normalizer::operator==(ثابت Normalizer& that) ثابت
{
    بازگشت
        این==&that ||
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

تهی U_EXPORT2
Normalizer::normalize(ثابت UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    اگر(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        اگر(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } وگرنه {
        UnicodeString localDest;
        UnicodeString *dest;

        اگر(&source!=&result) {
            dest=&result;
        } وگرنه {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        اگر(U_SUCCESS(status)) {
            اگر(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } وگرنه {
                n2->normalize(source, *dest, status);
            }
        }
        اگر(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

تهی U_EXPORT2
Normalizer::compose(ثابت UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

تهی U_EXPORT2
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
    اگر(U_SUCCESS(status)) {
        اگر(options&UNORM_UNICODE_3_2) {
            بازگشت FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } وگرنه {
            بازگشت n2->quickCheck(source, status);
        }
    } وگرنه {
        بازگشت UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ثابت UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    اگر(U_SUCCESS(status)) {
        اگر(options&UNORM_UNICODE_3_2) {
            بازگشت FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } وگرنه {
            بازگشت n2->isNormalized(source, status);
        }
    } وگرنه {
        بازگشت نادرست;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ثابت UnicodeString& left,
                        ثابت UnicodeString& راست,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    اگر(left.isBogus() || راست.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        اگر(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } وگرنه {
        UnicodeString localDest;
        UnicodeString *dest;

        اگر(&راست!=&result) {
            dest=&result;
        } وگرنه {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        اگر(U_SUCCESS(errorCode)) {
            اگر(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, راست, errorCode);
            } وگرنه {
                n2->append(*dest, راست, errorCode);
            }
        }
        اگر(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    بازگشت result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    اگر(bufferPos<buffer.length() || nextNormalize()) {
        بازگشت buffer.char32At(bufferPos);
    } وگرنه {
        بازگشت DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    اگر(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        بازگشت c;
    } وگرنه {
        بازگشت DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    اگر(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        بازگشت c;
    } وگرنه {
        بازگشت DONE;
    }
}

تهی Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

تهی
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
    بازگشت next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    بازگشت previous();
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
    اگر(bufferPos<buffer.length()) {
        بازگشت currentIndex;
    } وگرنه {
        بازگشت nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() ثابت {
    بازگشت text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() ثابت {
    بازگشت text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

تهی
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() ثابت
{
    بازگشت fUMode;
}

تهی
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    اگر (value) {
        fOptions |= option;
    } وگرنه {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ثابت
{
    بازگشت (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
تهی
Normalizer::setText(ثابت UnicodeString& newText, 
                    UErrorCode &status)
{
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    CharacterIterator *newIter = جدید StringCharacterIterator(newText);
    اگر (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
تهی
Normalizer::setText(ثابت CharacterIterator& newText, 
                    UErrorCode &status) 
{
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    CharacterIterator *newIter = newText.clone();
    اگر (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    delete text;
    text = newIter;
    reset();
}

تهی
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    CharacterIterator *newIter = جدید UCharCharacterIterator(newText, length);
    اگر (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
تهی
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

تهی Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    اگر(!text->hasNext()) {
        بازگشت نادرست;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    وقتی(text->hasNext()) {
        UChar32 c;
        اگر(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            بشکن;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    بازگشت U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    اگر(!text->hasPrevious()) {
        بازگشت نادرست;
    }
    UnicodeString segment;
    وقتی(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        اگر(fNorm2->hasBoundaryBefore(c)) {
            بشکن;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    بازگشت U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

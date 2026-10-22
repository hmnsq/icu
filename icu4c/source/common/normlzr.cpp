// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/chariter.h"
#অন্তর্ভাব "unicode/schriter.h"
#অন্তর্ভাব "unicode/uchriter.h"
#অন্তর্ভাব "unicode/normlzr.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "normalizer2impl.h"
#অন্তর্ভাব "uprops.h"  // for uniset_getUnicode32Instance()

#যদি defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ধ্রুব UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(নূতন StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(নূতন UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ধ্রুব CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ধ্রুব Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

শূন্য
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    যদি(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            নূতন FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    যদি(U_FAILURE(errorCode)) {
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
Normalizer::clone() ধ্রুব
{
    প্রত্যাগম নূতন Normalizer(*এতত্);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() ধ্রুব
{
    প্রত্যাগম text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
তার্কিক Normalizer::operator==(ধ্রুব Normalizer& that) ধ্রুব
{
    প্রত্যাগম
        এতত্==&that ||
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

শূন্য U_EXPORT2
Normalizer::normalize(ধ্রুব UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    যদি(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        যদি(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } অন্যথা {
        UnicodeString localDest;
        UnicodeString *dest;

        যদি(&source!=&result) {
            dest=&result;
        } অন্যথা {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ধ্রুব Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        যদি(U_SUCCESS(status)) {
            যদি(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } অন্যথা {
                n2->normalize(source, *dest, status);
            }
        }
        যদি(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

শূন্য U_EXPORT2
Normalizer::compose(ধ্রুব UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

শূন্য U_EXPORT2
Normalizer::decompose(ধ্রুব UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(ধ্রুব UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    ধ্রুব Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    যদি(U_SUCCESS(status)) {
        যদি(options&UNORM_UNICODE_3_2) {
            প্রত্যাগম FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } অন্যথা {
            প্রত্যাগম n2->quickCheck(source, status);
        }
    } অন্যথা {
        প্রত্যাগম UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ধ্রুব UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    ধ্রুব Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    যদি(U_SUCCESS(status)) {
        যদি(options&UNORM_UNICODE_3_2) {
            প্রত্যাগম FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } অন্যথা {
            প্রত্যাগম n2->isNormalized(source, status);
        }
    } অন্যথা {
        প্রত্যাগম অসত্য;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ধ্রুব UnicodeString& left,
                        ধ্রুব UnicodeString& ডান,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    যদি(left.isBogus() || ডান.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        যদি(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } অন্যথা {
        UnicodeString localDest;
        UnicodeString *dest;

        যদি(&ডান!=&result) {
            dest=&result;
        } অন্যথা {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        ধ্রুব Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        যদি(U_SUCCESS(errorCode)) {
            যদি(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, ডান, errorCode);
            } অন্যথা {
                n2->append(*dest, ডান, errorCode);
            }
        }
        যদি(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    প্রত্যাগম result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    যদি(bufferPos<buffer.length() || nextNormalize()) {
        প্রত্যাগম buffer.char32At(bufferPos);
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    যদি(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        প্রত্যাগম c;
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    যদি(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        প্রত্যাগম c;
    } অন্যথা {
        প্রত্যাগম DONE;
    }
}

শূন্য Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

শূন্য
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
    প্রত্যাগম next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    প্রত্যাগম previous();
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
int32_t Normalizer::getIndex() ধ্রুব {
    যদি(bufferPos<buffer.length()) {
        প্রত্যাগম currentIndex;
    } অন্যথা {
        প্রত্যাগম nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() ধ্রুব {
    প্রত্যাগম text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() ধ্রুব {
    প্রত্যাগম text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

শূন্য
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() ধ্রুব
{
    প্রত্যাগম fUMode;
}

শূন্য
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    যদি (value) {
        fOptions |= option;
    } অন্যথা {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ধ্রুব
{
    প্রত্যাগম (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
শূন্য
Normalizer::setText(ধ্রুব UnicodeString& newText, 
                    UErrorCode &status)
{
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    CharacterIterator *newIter = নূতন StringCharacterIterator(newText);
    যদি (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
শূন্য
Normalizer::setText(ধ্রুব CharacterIterator& newText, 
                    UErrorCode &status) 
{
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    CharacterIterator *newIter = newText.clone();
    যদি (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    delete text;
    text = newIter;
    reset();
}

শূন্য
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    CharacterIterator *newIter = নূতন UCharCharacterIterator(newText, length);
    যদি (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
শূন্য
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

শূন্য Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    যদি(!text->hasNext()) {
        প্রত্যাগম অসত্য;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    যাবত্(text->hasNext()) {
        UChar32 c;
        যদি(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            ভঙ্গ;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    প্রত্যাগম U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    যদি(!text->hasPrevious()) {
        প্রত্যাগম অসত্য;
    }
    UnicodeString segment;
    যাবত্(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        যদি(fNorm2->hasBoundaryBefore(c)) {
            ভঙ্গ;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    প্রত্যাগম U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

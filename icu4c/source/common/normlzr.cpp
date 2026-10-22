// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_NORMALIZATION

#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/chariter.h"
#અન્તર્ભાવ "unicode/schriter.h"
#અન્તર્ભાવ "unicode/uchriter.h"
#અન્તર્ભાવ "unicode/normlzr.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "normalizer2impl.h"
#અન્તર્ભાવ "uprops.h"  // for uniset_getUnicode32Instance()

#જો defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(અચલ UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(નવું StringCharacterIterator(str)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(નવું UCharCharacterIterator(str, length)),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(અચલ CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    currentIndex(0), nextIndex(0),
    buffer(), bufferPos(0)
{
    init();
}

Normalizer::Normalizer(અચલ Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    currentIndex(copy.currentIndex), nextIndex(copy.nextIndex),
    buffer(copy.buffer), bufferPos(copy.bufferPos)
{
    init();
}

શૂન્ય
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    જો(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            નવું FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    જો(U_FAILURE(errorCode)) {
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
Normalizer::clone() અચલ
{
    પાછા નવું Normalizer(*આ);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() અચલ
{
    પાછા text->hashCode() + fUMode + fOptions + buffer.hashCode() + bufferPos + currentIndex + nextIndex;
}
    
તાર્કિક Normalizer::operator==(અચલ Normalizer& that) અચલ
{
    પાછા
        આ==&that ||
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

શૂન્ય U_EXPORT2
Normalizer::normalize(અચલ UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    જો(source.isBogus() || U_FAILURE(status)) {
        result.setToBogus();
        જો(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } નહીંતર {
        UnicodeString localDest;
        UnicodeString *dest;

        જો(&source!=&result) {
            dest=&result;
        } નહીંતર {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        જો(U_SUCCESS(status)) {
            જો(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } નહીંતર {
                n2->normalize(source, *dest, status);
            }
        }
        જો(dest==&localDest && U_SUCCESS(status)) {
            result=*dest;
        }
    }
}

શૂન્ય U_EXPORT2
Normalizer::compose(અચલ UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& result, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, result, status);
}

શૂન્ય U_EXPORT2
Normalizer::decompose(અચલ UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& result, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, result, status);
}

UNormalizationCheckResult
Normalizer::quickCheck(અચલ UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    જો(U_SUCCESS(status)) {
        જો(options&UNORM_UNICODE_3_2) {
            પાછા FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } નહીંતર {
            પાછા n2->quickCheck(source, status);
        }
    } નહીંતર {
        પાછા UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(અચલ UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    જો(U_SUCCESS(status)) {
        જો(options&UNORM_UNICODE_3_2) {
            પાછા FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } નહીંતર {
            પાછા n2->isNormalized(source, status);
        }
    } નહીંતર {
        પાછા ખોટું;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(અચલ UnicodeString& left,
                        અચલ UnicodeString& જમણું,
                        UnicodeString& result U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    જો(left.isBogus() || જમણું.isBogus() || U_FAILURE(errorCode)) {
        result.setToBogus();
        જો(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } નહીંતર {
        UnicodeString localDest;
        UnicodeString *dest;

        જો(&જમણું!=&result) {
            dest=&result;
        } નહીંતર {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        જો(U_SUCCESS(errorCode)) {
            જો(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, જમણું, errorCode);
            } નહીંતર {
                n2->append(*dest, જમણું, errorCode);
            }
        }
        જો(dest==&localDest && U_SUCCESS(errorCode)) {
            result=*dest;
        }
    }
    પાછા result;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    જો(bufferPos<buffer.length() || nextNormalize()) {
        પાછા buffer.char32At(bufferPos);
    } નહીંતર {
        પાછા DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    જો(bufferPos<buffer.length() ||  nextNormalize()) {
        UChar32 c=buffer.char32At(bufferPos);
        bufferPos+=U16_LENGTH(c);
        પાછા c;
    } નહીંતર {
        પાછા DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    જો(bufferPos>0 || previousNormalize()) {
        UChar32 c=buffer.char32At(bufferPos-1);
        bufferPos-=U16_LENGTH(c);
        પાછા c;
    } નહીંતર {
        પાછા DONE;
    }
}

શૂન્ય Normalizer::reset() {
    currentIndex=nextIndex=text->setToStart();
    clearBuffer();
}

શૂન્ય
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
    પાછા next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    currentIndex=nextIndex=text->setToEnd();
    clearBuffer();
    પાછા previous();
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
int32_t Normalizer::getIndex() અચલ {
    જો(bufferPos<buffer.length()) {
        પાછા currentIndex;
    } નહીંતર {
        પાછા nextIndex;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::startIndex() અચલ {
    પાછા text->startIndex();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::endIndex() અચલ {
    પાછા text->endIndex();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

શૂન્ય
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() અચલ
{
    પાછા fUMode;
}

શૂન્ય
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    જો (value) {
        fOptions |= option;
    } નહીંતર {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) અચલ
{
    પાછા (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
શૂન્ય
Normalizer::setText(અચલ UnicodeString& newText, 
                    UErrorCode &status)
{
    જો (U_FAILURE(status)) {
        પાછા;
    }
    CharacterIterator *newIter = નવું StringCharacterIterator(newText);
    જો (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
શૂન્ય
Normalizer::setText(અચલ CharacterIterator& newText, 
                    UErrorCode &status) 
{
    જો (U_FAILURE(status)) {
        પાછા;
    }
    CharacterIterator *newIter = newText.clone();
    જો (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    delete text;
    text = newIter;
    reset();
}

શૂન્ય
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    જો (U_FAILURE(status)) {
        પાછા;
    }
    CharacterIterator *newIter = નવું UCharCharacterIterator(newText, length);
    જો (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
શૂન્ય
Normalizer::getText(UnicodeString&  result) 
{
    text->getText(result);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

શૂન્ય Normalizer::clearBuffer() {
    buffer.remove();
    bufferPos=0;
}

UBool
Normalizer::nextNormalize() {
    clearBuffer();
    currentIndex=nextIndex;
    text->setIndex(nextIndex);
    જો(!text->hasNext()) {
        પાછા ખોટું;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    જ્યારે(text->hasNext()) {
        UChar32 c;
        જો(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            તોડો;
        }
        segment.append(c);
    }
    nextIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    પાછા U_SUCCESS(errorCode) && !buffer.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clearBuffer();
    nextIndex=currentIndex;
    text->setIndex(currentIndex);
    જો(!text->hasPrevious()) {
        પાછા ખોટું;
    }
    UnicodeString segment;
    જ્યારે(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        જો(fNorm2->hasBoundaryBefore(c)) {
            તોડો;
        }
    }
    currentIndex=text->getIndex();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, buffer, errorCode);
    bufferPos=buffer.length();
    પાછા U_SUCCESS(errorCode) && !buffer.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

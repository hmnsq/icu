// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/uniset.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/chariter.h"
#समावेश "unicode/schriter.h"
#समावेश "unicode/uchriter.h"
#समावेश "unicode/normlzr.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "normalizer2impl.h"
#समावेश "uprops.h"  // for uniset_getUnicode32Instance()

#अगर defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#अघोषित move32
#पूर्ण_अगर

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(स्थिर UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(नया StringCharacterIterator(str)),
    current_सूचकांक(0), अग्र_सूचकांक(0),
    बफर(), बफर_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(नया UCharCharacterIterator(str, length)),
    current_सूचकांक(0), अग्र_सूचकांक(0),
    बफर(), बफर_Pos(0)
{
    init();
}

Normalizer::Normalizer(स्थिर CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    current_सूचकांक(0), अग्र_सूचकांक(0),
    बफर(), बफर_Pos(0)
{
    init();
}

Normalizer::Normalizer(स्थिर Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    current_सूचकांक(copy.current_सूचकांक), अग्र_सूचकांक(copy.अग्र_सूचकांक),
    बफर(copy.बफर), बफर_Pos(copy.बफर_Pos)
{
    init();
}

व्योम
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    अगर(fOptions&UNORM_UNICODE_3_2) {
        मिटाओ fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            नया FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    अगर(U_FAILURE(errorCode)) {
        errorCode=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(errorCode);
    }
}

Normalizer::~Normalizer()
{
    मिटाओ fFilteredNorm2;
    मिटाओ text;
}

Normalizer* 
Normalizer::clone() स्थिर
{
    वापस नया Normalizer(*यह);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() स्थिर
{
    वापस text->hashCode() + fUMode + fOptions + बफर.hashCode() + बफर_Pos + current_सूचकांक + अग्र_सूचकांक;
}
    
bool Normalizer::चालक==(स्थिर Normalizer& that) स्थिर
{
    वापस
        यह==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *text==*that.text &&
        बफर==that.बफर &&
        बफर_Pos==that.बफर_Pos &&
        अग्र_सूचकांक==that.अग्र_सूचकांक);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

व्योम U_EXPORT2
Normalizer::normalize(स्थिर UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& परिणाम, 
                      UErrorCode &status) {
    अगर(source.isBogus() || U_FAILURE(status)) {
        परिणाम.setToBogus();
        अगर(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } अन्यथा {
        UnicodeString localDest;
        UnicodeString *dest;

        अगर(&source!=&परिणाम) {
            dest=&परिणाम;
        } अन्यथा {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        अगर(U_SUCCESS(status)) {
            अगर(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } अन्यथा {
                n2->normalize(source, *dest, status);
            }
        }
        अगर(dest==&localDest && U_SUCCESS(status)) {
            परिणाम=*dest;
        }
    }
}

व्योम U_EXPORT2
Normalizer::compose(स्थिर UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& परिणाम, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, परिणाम, status);
}

व्योम U_EXPORT2
Normalizer::decompose(स्थिर UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& परिणाम, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, परिणाम, status);
}

UNormalization_Check_परिणाम
Normalizer::quickCheck(स्थिर UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    अगर(U_SUCCESS(status)) {
        अगर(options&UNORM_UNICODE_3_2) {
            वापस FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } अन्यथा {
            वापस n2->quickCheck(source, status);
        }
    } अन्यथा {
        वापस UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(स्थिर UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    अगर(U_SUCCESS(status)) {
        अगर(options&UNORM_UNICODE_3_2) {
            वापस FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } अन्यथा {
            वापस n2->isNormalized(source, status);
        }
    } अन्यथा {
        वापस false;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(स्थिर UnicodeString& बाँए,
                        स्थिर UnicodeString& दाँए,
                        UnicodeString& परिणाम U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    अगर(बाँए.isBogus() || दाँए.isBogus() || U_FAILURE(errorCode)) {
        परिणाम.setToBogus();
        अगर(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } अन्यथा {
        UnicodeString localDest;
        UnicodeString *dest;

        अगर(&दाँए!=&परिणाम) {
            dest=&परिणाम;
        } अन्यथा {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=बाँए;
        स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        अगर(U_SUCCESS(errorCode)) {
            अगर(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, दाँए, errorCode);
            } अन्यथा {
                n2->append(*dest, दाँए, errorCode);
            }
        }
        अगर(dest==&localDest && U_SUCCESS(errorCode)) {
            परिणाम=*dest;
        }
    }
    वापस परिणाम;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    अगर(बफर_Pos<बफर.length() || अग्र_Normalize()) {
        वापस बफर.char32At(बफर_Pos);
    } अन्यथा {
        वापस DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    अगर(बफर_Pos<बफर.length() ||  अग्र_Normalize()) {
        UChar32 c=बफर.char32At(बफर_Pos);
        बफर_Pos+=U16_लंबाई(c);
        वापस c;
    } अन्यथा {
        वापस DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    अगर(बफर_Pos>0 || previousNormalize()) {
        UChar32 c=बफर.char32At(बफर_Pos-1);
        बफर_Pos-=U16_लंबाई(c);
        वापस c;
    } अन्यथा {
        वापस DONE;
    }
}

व्योम Normalizer::reset() {
    current_सूचकांक=अग्र_सूचकांक=text->setToStart();
    clear_बफर();
}

व्योम
Normalizer::set_सूचकांक_Only(int32_t सूचकांक) {
    text->set_सूचकांक(सूचकांक);  // pins index
    current_सूचकांक=अग्र_सूचकांक=text->get_सूचकांक();
    clear_बफर();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    वापस next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    current_सूचकांक=अग्र_सूचकांक=text->setToEnd();
    clear_बफर();
    वापस previous();
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
int32_t Normalizer::get_सूचकांक() स्थिर {
    अगर(बफर_Pos<बफर.length()) {
        वापस current_सूचकांक;
    } अन्यथा {
        वापस अग्र_सूचकांक;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::start_सूचकांक() स्थिर {
    वापस text->start_सूचकांक();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::end_सूचकांक() स्थिर {
    वापस text->end_सूचकांक();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

व्योम
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() स्थिर
{
    वापस fUMode;
}

व्योम
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    अगर (value) {
        fOptions |= option;
    } अन्यथा {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) स्थिर
{
    वापस (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
व्योम
Normalizer::setText(स्थिर UnicodeString& newText, 
                    UErrorCode &status)
{
    अगर (U_FAILURE(status)) {
        वापस;
    }
    CharacterIterator *newIter = नया StringCharacterIterator(newText);
    अगर (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    मिटाओ text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
व्योम
Normalizer::setText(स्थिर CharacterIterator& newText, 
                    UErrorCode &status) 
{
    अगर (U_FAILURE(status)) {
        वापस;
    }
    CharacterIterator *newIter = newText.clone();
    अगर (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    मिटाओ text;
    text = newIter;
    reset();
}

व्योम
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    अगर (U_FAILURE(status)) {
        वापस;
    }
    CharacterIterator *newIter = नया UCharCharacterIterator(newText, length);
    अगर (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    मिटाओ text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
व्योम
Normalizer::getText(UnicodeString&  परिणाम) 
{
    text->getText(परिणाम);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

व्योम Normalizer::clear_बफर() {
    बफर.हटाओ();
    बफर_Pos=0;
}

UBool
Normalizer::अग्र_Normalize() {
    clear_बफर();
    current_सूचकांक=अग्र_सूचकांक;
    text->set_सूचकांक(अग्र_सूचकांक);
    अगर(!text->has_अग्र()) {
        वापस false;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    जबतक(text->has_अग्र()) {
        UChar32 c;
        अगर(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            अवरोध;
        }
        segment.append(c);
    }
    अग्र_सूचकांक=text->get_सूचकांक();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, बफर, errorCode);
    वापस U_SUCCESS(errorCode) && !बफर.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clear_बफर();
    अग्र_सूचकांक=current_सूचकांक;
    text->set_सूचकांक(current_सूचकांक);
    अगर(!text->hasPrevious()) {
        वापस false;
    }
    UnicodeString segment;
    जबतक(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        अगर(fNorm2->hasBoundaryBefore(c)) {
            अवरोध;
        }
    }
    current_सूचकांक=text->get_सूचकांक();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, बफर, errorCode);
    बफर_Pos=बफर.length();
    वापस U_SUCCESS(errorCode) && !बफर.isEmpty();
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_NORMALIZATION */

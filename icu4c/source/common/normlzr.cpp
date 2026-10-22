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
#अपरिभाषय move32
#यदि_अन्तः

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ध्रुव UnicodeString& str, UNormalization_बहुलकम् बहुलकम्) :
    UObject(), fFilteredNorm2(शून्यसूचकः), fNorm2(शून्यसूचकः), fUMode(बहुलकम्), fOptions(0),
    text(नूतन StringCharacterIterator(str)),
    current_सूचकांक(0), अग्र_सूचकांक(0),
    बफर(), बफर_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_बहुलकम् बहुलकम्) :
    UObject(), fFilteredNorm2(शून्यसूचकः), fNorm2(शून्यसूचकः), fUMode(बहुलकम्), fOptions(0),
    text(नूतन UCharCharacterIterator(str, length)),
    current_सूचकांक(0), अग्र_सूचकांक(0),
    बफर(), बफर_Pos(0)
{
    init();
}

Normalizer::Normalizer(ध्रुव CharacterIterator& iter, UNormalization_बहुलकम् बहुलकम्) :
    UObject(), fFilteredNorm2(शून्यसूचकः), fNorm2(शून्यसूचकः), fUMode(बहुलकम्), fOptions(0),
    text(iter.clone()),
    current_सूचकांक(0), अग्र_सूचकांक(0),
    बफर(), बफर_Pos(0)
{
    init();
}

Normalizer::Normalizer(ध्रुव Normalizer &copy) :
    UObject(copy), fFilteredNorm2(शून्यसूचकः), fNorm2(शून्यसूचकः), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    current_सूचकांक(copy.current_सूचकांक), अग्र_सूचकांक(copy.अग्र_सूचकांक),
    बफर(copy.बफर), बफर_Pos(copy.बफर_Pos)
{
    init();
}

शून्य
Normalizer::init() {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_संहिता);
    यदि(fOptions&UNORM_UNICODE_3_2) {
        विलोपय fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            नूतन FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_संहिता));
    }
    यदि(U_FAILURE(error_संहिता)) {
        error_संहिता=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_संहिता);
    }
}

Normalizer::~Normalizer()
{
    विलोपय fFilteredNorm2;
    विलोपय text;
}

Normalizer* 
Normalizer::clone() ध्रुव
{
    प्रत्यागम नूतन Normalizer(*एतत्);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_संहिता() ध्रुव
{
    प्रत्यागम text->hash_संहिता() + fUMode + fOptions + बफर.hash_संहिता() + बफर_Pos + current_सूचकांक + अग्र_सूचकांक;
}
    
तार्किक Normalizer::संक्रियकः==(ध्रुव Normalizer& that) ध्रुव
{
    प्रत्यागम
        एतत्==&that ||
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

शून्य U_EXPORT2
Normalizer::normalize(ध्रुव UnicodeString& source, 
                      UNormalization_बहुलकम् बहुलकम्, int32_t options,
                      UnicodeString& परिणाम, 
                      UError_संहिता &स्थितिः) {
    यदि(source.isBogus() || U_FAILURE(स्थितिः)) {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
        यदि(U_SUCCESS(स्थितिः)) {
            स्थितिः=U_ILLEGAL_तर्कः_ERROR;
        }
    } अन्यथा {
        UnicodeString localDest;
        UnicodeString *dest;

        यदि(&source!=&परिणाम) {
            dest=&परिणाम;
        } अन्यथा {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, स्थितिः);
        यदि(U_SUCCESS(स्थितिः)) {
            यदि(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(स्थितिः)).
                    normalize(source, *dest, स्थितिः);
            } अन्यथा {
                n2->normalize(source, *dest, स्थितिः);
            }
        }
        यदि(dest==&localDest && U_SUCCESS(स्थितिः)) {
            परिणाम=*dest;
        }
    }
}

शून्य U_EXPORT2
Normalizer::compose(ध्रुव UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& परिणाम, 
                    UError_संहिता &स्थितिः) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, परिणाम, स्थितिः);
}

शून्य U_EXPORT2
Normalizer::decompose(ध्रुव UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& परिणाम, 
                      UError_संहिता &स्थितिः) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, परिणाम, स्थितिः);
}

UNormalization_Check_परिणाम
Normalizer::शीघ्र_Check(ध्रुव UnicodeString& source,
                       UNormalization_बहुलकम् बहुलकम्, int32_t options,
                       UError_संहिता &स्थितिः) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, स्थितिः);
    यदि(U_SUCCESS(स्थितिः)) {
        यदि(options&UNORM_UNICODE_3_2) {
            प्रत्यागम FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(स्थितिः)).
                शीघ्र_Check(source, स्थितिः);
        } अन्यथा {
            प्रत्यागम n2->शीघ्र_Check(source, स्थितिः);
        }
    } अन्यथा {
        प्रत्यागम UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ध्रुव UnicodeString& source,
                         UNormalization_बहुलकम् बहुलकम्, int32_t options,
                         UError_संहिता &स्थितिः) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, स्थितिः);
    यदि(U_SUCCESS(स्थितिः)) {
        यदि(options&UNORM_UNICODE_3_2) {
            प्रत्यागम FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(स्थितिः)).
                isNormalized(source, स्थितिः);
        } अन्यथा {
            प्रत्यागम n2->isNormalized(source, स्थितिः);
        }
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ध्रुव UnicodeString& वामतः,
                        ध्रुव UnicodeString& दक्षिणतः,
                        UnicodeString& परिणाम U_LIFETIME_BOUND,
                        UNormalization_बहुलकम् बहुलकम्,
                        int32_t options,
                        UError_संहिता& error_संहिता) {
    यदि(वामतः.isBogus() || दक्षिणतः.isBogus() || U_FAILURE(error_संहिता)) {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
        यदि(U_SUCCESS(error_संहिता)) {
            error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        }
    } अन्यथा {
        UnicodeString localDest;
        UnicodeString *dest;

        यदि(&दक्षिणतः!=&परिणाम) {
            dest=&परिणाम;
        } अन्यथा {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=वामतः;
        ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, error_संहिता);
        यदि(U_SUCCESS(error_संहिता)) {
            यदि(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_संहिता)).
                    append(*dest, दक्षिणतः, error_संहिता);
            } अन्यथा {
                n2->append(*dest, दक्षिणतः, error_संहिता);
            }
        }
        यदि(dest==&localDest && U_SUCCESS(error_संहिता)) {
            परिणाम=*dest;
        }
    }
    प्रत्यागम परिणाम;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    यदि(बफर_Pos<बफर.length() || अग्र_Normalize()) {
        प्रत्यागम बफर.char32At(बफर_Pos);
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    यदि(बफर_Pos<बफर.length() ||  अग्र_Normalize()) {
        UChar32 c=बफर.char32At(बफर_Pos);
        बफर_Pos+=U16_दीर्घता(c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    यदि(बफर_Pos>0 || previousNormalize()) {
        UChar32 c=बफर.char32At(बफर_Pos-1);
        बफर_Pos-=U16_दीर्घता(c);
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम समाप्तम्_2;
    }
}

शून्य Normalizer::reset() {
    current_सूचकांक=अग्र_सूचकांक=text->समुच्चयः_गन्तव्यम्_Start();
    clear_बफर();
}

शून्य
Normalizer::समुच्चयः_सूचकांक_Only(int32_t सूचकांक) {
    text->समुच्चयः_सूचकांक(सूचकांक);  // pins index
    current_सूचकांक=अग्र_सूचकांक=text->get_सूचकांक();
    clear_बफर();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    प्रत्यागम next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::चरमः() {
    current_सूचकांक=अग्र_सूचकांक=text->समुच्चयः_गन्तव्यम्_अन्तः();
    clear_बफर();
    प्रत्यागम previous();
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
int32_t Normalizer::get_सूचकांक() ध्रुव {
    यदि(बफर_Pos<बफर.length()) {
        प्रत्यागम current_सूचकांक;
    } अन्यथा {
        प्रत्यागम अग्र_सूचकांक;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::start_सूचकांक() ध्रुव {
    प्रत्यागम text->start_सूचकांक();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::अन्तः_सूचकांक() ध्रुव {
    प्रत्यागम text->अन्तः_सूचकांक();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

शून्य
Normalizer::समुच्चयः_बहुलकम्(UNormalization_बहुलकम् new_बहुलकम्) 
{
    fUMode = new_बहुलकम्;
    init();
}

UNormalization_बहुलकम्
Normalizer::getUMode() ध्रुव
{
    प्रत्यागम fUMode;
}

शून्य
Normalizer::समुच्चयः_Option(int32_t option, 
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
Normalizer::getOption(int32_t option) ध्रुव
{
    प्रत्यागम (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
शून्य
Normalizer::समुच्चयः_Text(ध्रुव UnicodeString& newText, 
                    UError_संहिता &स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    CharacterIterator *newIter = नूतन StringCharacterIterator(newText);
    यदि (newIter == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    विलोपय text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
शून्य
Normalizer::समुच्चयः_Text(ध्रुव CharacterIterator& newText, 
                    UError_संहिता &स्थितिः) 
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    CharacterIterator *newIter = newText.clone();
    यदि (newIter == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    विलोपय text;
    text = newIter;
    reset();
}

शून्य
Normalizer::समुच्चयः_Text(ConstChar16Ptr newText,
                    int32_t length,
                    UError_संहिता &स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    CharacterIterator *newIter = नूतन UCharCharacterIterator(newText, length);
    यदि (newIter == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    विलोपय text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
शून्य
Normalizer::getText(UnicodeString&  परिणाम) 
{
    text->getText(परिणाम);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

शून्य Normalizer::clear_बफर() {
    बफर.remove();
    बफर_Pos=0;
}

UBool
Normalizer::अग्र_Normalize() {
    clear_बफर();
    current_सूचकांक=अग्र_सूचकांक;
    text->समुच्चयः_सूचकांक(अग्र_सूचकांक);
    यदि(!text->has_अग्र()) {
        प्रत्यागम असत्य;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    यावत्(text->has_अग्र()) {
        UChar32 c;
        यदि(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            भङ्ग;
        }
        segment.append(c);
    }
    अग्र_सूचकांक=text->get_सूचकांक();
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    fNorm2->normalize(segment, बफर, error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता) && !बफर.is_रिक्तानि();
}

UBool
Normalizer::previousNormalize() {
    clear_बफर();
    अग्र_सूचकांक=current_सूचकांक;
    text->समुच्चयः_सूचकांक(current_सूचकांक);
    यदि(!text->hasPrevious()) {
        प्रत्यागम असत्य;
    }
    UnicodeString segment;
    यावत्(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        यदि(fNorm2->hasBoundaryBefore(c)) {
            भङ्ग;
        }
    }
    current_सूचकांक=text->get_सूचकांक();
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    fNorm2->normalize(segment, बफर, error_संहिता);
    बफर_Pos=बफर.length();
    प्रत्यागम U_SUCCESS(error_संहिता) && !बफर.is_रिक्तानि();
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_NORMALIZATION */

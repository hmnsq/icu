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

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

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

#ಆದರೆ defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು move32
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ಸ್ಥಿರಾಂಕ UnicodeString& str, UNormalization_ಬಹುಲಕ ಬಹುಲಕ) :
    UObject(), fFilteredNorm2(ಶೂನ್ಯಸೂಚಕ), fNorm2(ಶೂನ್ಯಸೂಚಕ), fUMode(ಬಹುಲಕ), fOptions(0),
    ಪಠ್ಯ(ಹೊಸ StringCharacterIterator(str)),
    current_ಸೂಚ್ಯಂಕ(0), ಮುಂದಿನ_ಸೂಚ್ಯಂಕ(0),
    ಬಫರ್(), ಬಫರ್_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_ಬಹುಲಕ ಬಹುಲಕ) :
    UObject(), fFilteredNorm2(ಶೂನ್ಯಸೂಚಕ), fNorm2(ಶೂನ್ಯಸೂಚಕ), fUMode(ಬಹುಲಕ), fOptions(0),
    ಪಠ್ಯ(ಹೊಸ UCharCharacterIterator(str, length)),
    current_ಸೂಚ್ಯಂಕ(0), ಮುಂದಿನ_ಸೂಚ್ಯಂಕ(0),
    ಬಫರ್(), ಬಫರ್_Pos(0)
{
    init();
}

Normalizer::Normalizer(ಸ್ಥಿರಾಂಕ CharacterIterator& iter, UNormalization_ಬಹುಲಕ ಬಹುಲಕ) :
    UObject(), fFilteredNorm2(ಶೂನ್ಯಸೂಚಕ), fNorm2(ಶೂನ್ಯಸೂಚಕ), fUMode(ಬಹುಲಕ), fOptions(0),
    ಪಠ್ಯ(iter.clone()),
    current_ಸೂಚ್ಯಂಕ(0), ಮುಂದಿನ_ಸೂಚ್ಯಂಕ(0),
    ಬಫರ್(), ಬಫರ್_Pos(0)
{
    init();
}

Normalizer::Normalizer(ಸ್ಥಿರಾಂಕ Normalizer &copy) :
    UObject(copy), fFilteredNorm2(ಶೂನ್ಯಸೂಚಕ), fNorm2(ಶೂನ್ಯಸೂಚಕ), fUMode(copy.fUMode), fOptions(copy.fOptions),
    ಪಠ್ಯ(copy.ಪಠ್ಯ->clone()),
    current_ಸೂಚ್ಯಂಕ(copy.current_ಸೂಚ್ಯಂಕ), ಮುಂದಿನ_ಸೂಚ್ಯಂಕ(copy.ಮುಂದಿನ_ಸೂಚ್ಯಂಕ),
    ಬಫರ್(copy.ಬಫರ್), ಬಫರ್_Pos(copy.ಬಫರ್_Pos)
{
    init();
}

ಶೂನ್ಯ
Normalizer::init() {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_ಸಂಕೇತ);
    ಆದರೆ(fOptions&UNORM_UNICODE_3_2) {
        ಅಳಿಸು fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            ಹೊಸ FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_ಸಂಕೇತ));
    }
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        error_ಸಂಕೇತ=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_ಸಂಕೇತ);
    }
}

Normalizer::~Normalizer()
{
    ಅಳಿಸು fFilteredNorm2;
    ಅಳಿಸು ಪಠ್ಯ;
}

Normalizer* 
Normalizer::clone() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಹೊಸ Normalizer(*ಇದು);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಪಠ್ಯ->hash_ಸಂಕೇತ() + fUMode + fOptions + ಬಫರ್.hash_ಸಂಕೇತ() + ಬಫರ್_Pos + current_ಸೂಚ್ಯಂಕ + ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
}
    
ತಾರ್ಕಿಕ Normalizer::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Normalizer& that) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ
        ಇದು==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *ಪಠ್ಯ==*that.ಪಠ್ಯ &&
        ಬಫರ್==that.ಬಫರ್ &&
        ಬಫರ್_Pos==that.ಬಫರ್_Pos &&
        ಮುಂದಿನ_ಸೂಚ್ಯಂಕ==that.ಮುಂದಿನ_ಸೂಚ್ಯಂಕ);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

ಶೂನ್ಯ U_EXPORT2
Normalizer::normalize(ಸ್ಥಿರಾಂಕ UnicodeString& ಮೂಲ, 
                      UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
                      UnicodeString& ಫಲಿತಾಂಶ, 
                      UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ(ಮೂಲ.isBogus() || U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
        ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        UnicodeString localDest;
        UnicodeString *dest;

        ಆದರೆ(&ಮೂಲ!=&ಫಲಿತಾಂಶ) {
            dest=&ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಆದರೆ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(ಸ್ಥಿತಿ_2)).
                    normalize(ಮೂಲ, *dest, ಸ್ಥಿತಿ_2);
            } ಇಲ್ಲದಿದ್ದರೆ {
                n2->normalize(ಮೂಲ, *dest, ಸ್ಥಿತಿ_2);
            }
        }
        ಆದರೆ(dest==&localDest && U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಫಲಿತಾಂಶ=*dest;
        }
    }
}

ಶೂನ್ಯ U_EXPORT2
Normalizer::compose(ಸ್ಥಿರಾಂಕ UnicodeString& ಮೂಲ, 
                    UBool compat, int32_t options,
                    UnicodeString& ಫಲಿತಾಂಶ, 
                    UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    normalize(ಮೂಲ, compat ? UNORM_NFKC : UNORM_NFC, options, ಫಲಿತಾಂಶ, ಸ್ಥಿತಿ_2);
}

ಶೂನ್ಯ U_EXPORT2
Normalizer::decompose(ಸ್ಥಿರಾಂಕ UnicodeString& ಮೂಲ, 
                      UBool compat, int32_t options,
                      UnicodeString& ಫಲಿತಾಂಶ, 
                      UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    normalize(ಮೂಲ, compat ? UNORM_NFKD : UNORM_NFD, options, ಫಲಿತಾಂಶ, ಸ್ಥಿತಿ_2);
}

UNormalization_Check_ಫಲಿತಾಂಶ
Normalizer::ಶೀಘ್ರ_Check(ಸ್ಥಿರಾಂಕ UnicodeString& ಮೂಲ,
                       UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
                       UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ(options&UNORM_UNICODE_3_2) {
            ಹಿಂತಿರುಗಿ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(ಸ್ಥಿತಿ_2)).
                ಶೀಘ್ರ_Check(ಮೂಲ, ಸ್ಥಿತಿ_2);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ n2->ಶೀಘ್ರ_Check(ಮೂಲ, ಸ್ಥಿತಿ_2);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ಸ್ಥಿರಾಂಕ UnicodeString& ಮೂಲ,
                         UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
                         UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ(options&UNORM_UNICODE_3_2) {
            ಹಿಂತಿರುಗಿ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(ಸ್ಥಿತಿ_2)).
                isNormalized(ಮೂಲ, ಸ್ಥಿತಿ_2);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ n2->isNormalized(ಮೂಲ, ಸ್ಥಿತಿ_2);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ಸ್ಥಿರಾಂಕ UnicodeString& ಎಡ,
                        ಸ್ಥಿರಾಂಕ UnicodeString& ಬಲ,
                        UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                        UNormalization_ಬಹುಲಕ ಬಹುಲಕ,
                        int32_t options,
                        UError_ಸಂಕೇತ& error_ಸಂಕೇತ) {
    ಆದರೆ(ಎಡ.isBogus() || ಬಲ.isBogus() || U_FAILURE(error_ಸಂಕೇತ)) {
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        UnicodeString localDest;
        UnicodeString *dest;

        ಆದರೆ(&ಬಲ!=&ಫಲಿತಾಂಶ) {
            dest=&ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=ಎಡ;
        ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            ಆದರೆ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_ಸಂಕೇತ)).
                    append(*dest, ಬಲ, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                n2->append(*dest, ಬಲ, error_ಸಂಕೇತ);
            }
        }
        ಆದರೆ(dest==&localDest && U_SUCCESS(error_ಸಂಕೇತ)) {
            ಫಲಿತಾಂಶ=*dest;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    ಆದರೆ(ಬಫರ್_Pos<ಬಫರ್.length() || ಮುಂದಿನ_Normalize()) {
        ಹಿಂತಿರುಗಿ ಬಫರ್.char32At(ಬಫರ್_Pos);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    ಆದರೆ(ಬಫರ್_Pos<ಬಫರ್.length() ||  ಮುಂದಿನ_Normalize()) {
        UChar32 c=ಬಫರ್.char32At(ಬಫರ್_Pos);
        ಬಫರ್_Pos+=U16_ಉದ್ದಳತೆ(c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::ಹಿಂದಿನ() {
    ಆದರೆ(ಬಫರ್_Pos>0 || ಹಿಂದಿನ_Normalize()) {
        UChar32 c=ಬಫರ್.char32At(ಬಫರ್_Pos-1);
        ಬಫರ್_Pos-=U16_ಉದ್ದಳತೆ(c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

ಶೂನ್ಯ Normalizer::reset() {
    current_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಸೂಚ್ಯಂಕ=ಪಠ್ಯ->ಗಣ_ಗಮ್ಯ_ಆರಂಭ();
    clear_ಬಫರ್();
}

ಶೂನ್ಯ
Normalizer::ಗಣ_ಸೂಚ್ಯಂಕ_Only(int32_t ಸೂಚ್ಯಂಕ) {
    ಪಠ್ಯ->ಗಣ_ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ);  // pins index
    current_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಸೂಚ್ಯಂಕ=ಪಠ್ಯ->get_ಸೂಚ್ಯಂಕ();
    clear_ಬಫರ್();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    ಹಿಂತಿರುಗಿ next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::ಕೊನೆಯ() {
    current_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಸೂಚ್ಯಂಕ=ಪಠ್ಯ->ಗಣ_ಗಮ್ಯ_ಕೊನೆ();
    clear_ಬಫರ್();
    ಹಿಂತಿರುಗಿ ಹಿಂದಿನ();
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
int32_t Normalizer::get_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಬಫರ್_Pos<ಬಫರ್.length()) {
        ಹಿಂತಿರುಗಿ current_ಸೂಚ್ಯಂಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::ಆರಂಭ_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಪಠ್ಯ->ಆರಂಭ_ಸೂಚ್ಯಂಕ();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::ಕೊನೆ_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಪಠ್ಯ->ಕೊನೆ_ಸೂಚ್ಯಂಕ();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

ಶೂನ್ಯ
Normalizer::ಗಣ_ಬಹುಲಕ(UNormalization_ಬಹುಲಕ new_ಬಹುಲಕ) 
{
    fUMode = new_ಬಹುಲಕ;
    init();
}

UNormalization_ಬಹುಲಕ
Normalizer::getUMode() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ fUMode;
}

ಶೂನ್ಯ
Normalizer::ಗಣ_Option(int32_t option, 
                      UBool value) 
{
    ಆದರೆ (value) {
        fOptions |= option;
    } ಇಲ್ಲದಿದ್ದರೆ {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
ಶೂನ್ಯ
Normalizer::ಗಣ_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString& new_ಪಠ್ಯ, 
                    UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    CharacterIterator *newIter = ಹೊಸ StringCharacterIterator(new_ಪಠ್ಯ);
    ಆದರೆ (newIter == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಅಳಿಸು ಪಠ್ಯ;
    ಪಠ್ಯ = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
ಶೂನ್ಯ
Normalizer::ಗಣ_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ CharacterIterator& new_ಪಠ್ಯ, 
                    UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) 
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    CharacterIterator *newIter = new_ಪಠ್ಯ.clone();
    ಆದರೆ (newIter == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಅಳಿಸು ಪಠ್ಯ;
    ಪಠ್ಯ = newIter;
    reset();
}

ಶೂನ್ಯ
Normalizer::ಗಣ_ಪಠ್ಯ(ConstChar16Ptr new_ಪಠ್ಯ,
                    int32_t length,
                    UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    CharacterIterator *newIter = ಹೊಸ UCharCharacterIterator(new_ಪಠ್ಯ, length);
    ಆದರೆ (newIter == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಅಳಿಸು ಪಠ್ಯ;
    ಪಠ್ಯ = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
ಶೂನ್ಯ
Normalizer::get_ಪಠ್ಯ(UnicodeString&  ಫಲಿತಾಂಶ) 
{
    ಪಠ್ಯ->get_ಪಠ್ಯ(ಫಲಿತಾಂಶ);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

ಶೂನ್ಯ Normalizer::clear_ಬಫರ್() {
    ಬಫರ್.remove();
    ಬಫರ್_Pos=0;
}

UBool
Normalizer::ಮುಂದಿನ_Normalize() {
    clear_ಬಫರ್();
    current_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
    ಪಠ್ಯ->ಗಣ_ಸೂಚ್ಯಂಕ(ಮುಂದಿನ_ಸೂಚ್ಯಂಕ);
    ಆದರೆ(!ಪಠ್ಯ->has_ಮುಂದಿನ()) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(ಪಠ್ಯ->next32PostInc());
    ಆಗ(ಪಠ್ಯ->has_ಮುಂದಿನ()) {
        UChar32 c;
        ಆದರೆ(fNorm2->hasBoundaryBefore(c=ಪಠ್ಯ->next32PostInc())) {
            ಪಠ್ಯ->move32(-1, CharacterIterator::kCurrent);
            ನಿಲ್ಲಿಸು;
        }
        segment.append(c);
    }
    ಮುಂದಿನ_ಸೂಚ್ಯಂಕ=ಪಠ್ಯ->get_ಸೂಚ್ಯಂಕ();
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    fNorm2->normalize(segment, ಬಫರ್, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && !ಬಫರ್.is_ಖಾಲಿಗಳು();
}

UBool
Normalizer::ಹಿಂದಿನ_Normalize() {
    clear_ಬಫರ್();
    ಮುಂದಿನ_ಸೂಚ್ಯಂಕ=current_ಸೂಚ್ಯಂಕ;
    ಪಠ್ಯ->ಗಣ_ಸೂಚ್ಯಂಕ(current_ಸೂಚ್ಯಂಕ);
    ಆದರೆ(!ಪಠ್ಯ->has_ಹಿಂದಿನ()) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    UnicodeString segment;
    ಆಗ(ಪಠ್ಯ->has_ಹಿಂದಿನ()) {
        UChar32 c=ಪಠ್ಯ->previous32();
        segment.insert(0, c);
        ಆದರೆ(fNorm2->hasBoundaryBefore(c)) {
            ನಿಲ್ಲಿಸು;
        }
    }
    current_ಸೂಚ್ಯಂಕ=ಪಠ್ಯ->get_ಸೂಚ್ಯಂಕ();
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    fNorm2->normalize(segment, ಬಫರ್, error_ಸಂಕೇತ);
    ಬಫರ್_Pos=ಬಫರ್.length();
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && !ಬಫರ್.is_ಖಾಲಿಗಳು();
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_NORMALIZATION */

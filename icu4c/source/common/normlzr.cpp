// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_NORMALIZATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/chariter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/schriter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchriter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normlzr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"  // for uniset_getUnicode32Instance()

#ਜੇ defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ move32
#ਜੇ_ਅੰਤ

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ਅਚਲ UnicodeString& str, UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ) :
    UObject(), fFilteredNorm2(ਖਾਲੀ_ਸੂਚਕ), fNorm2(ਖਾਲੀ_ਸੂਚਕ), fUMode(ਬਹੁਲਕ), fOptions(0),
    ਪਾਠ(ਨਵਾਂ StringCharacterIterator(str)),
    current_ਸੂਚਕਾਂਕ(0), ਅਗਲਾ_ਸੂਚਕਾਂਕ(0),
    ਬਫਰ(), ਬਫਰ_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ) :
    UObject(), fFilteredNorm2(ਖਾਲੀ_ਸੂਚਕ), fNorm2(ਖਾਲੀ_ਸੂਚਕ), fUMode(ਬਹੁਲਕ), fOptions(0),
    ਪਾਠ(ਨਵਾਂ UCharCharacterIterator(str, length)),
    current_ਸੂਚਕਾਂਕ(0), ਅਗਲਾ_ਸੂਚਕਾਂਕ(0),
    ਬਫਰ(), ਬਫਰ_Pos(0)
{
    init();
}

Normalizer::Normalizer(ਅਚਲ CharacterIterator& iter, UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ) :
    UObject(), fFilteredNorm2(ਖਾਲੀ_ਸੂਚਕ), fNorm2(ਖਾਲੀ_ਸੂਚਕ), fUMode(ਬਹੁਲਕ), fOptions(0),
    ਪਾਠ(iter.clone()),
    current_ਸੂਚਕਾਂਕ(0), ਅਗਲਾ_ਸੂਚਕਾਂਕ(0),
    ਬਫਰ(), ਬਫਰ_Pos(0)
{
    init();
}

Normalizer::Normalizer(ਅਚਲ Normalizer &copy) :
    UObject(copy), fFilteredNorm2(ਖਾਲੀ_ਸੂਚਕ), fNorm2(ਖਾਲੀ_ਸੂਚਕ), fUMode(copy.fUMode), fOptions(copy.fOptions),
    ਪਾਠ(copy.ਪਾਠ->clone()),
    current_ਸੂਚਕਾਂਕ(copy.current_ਸੂਚਕਾਂਕ), ਅਗਲਾ_ਸੂਚਕਾਂਕ(copy.ਅਗਲਾ_ਸੂਚਕਾਂਕ),
    ਬਫਰ(copy.ਬਫਰ), ਬਫਰ_Pos(copy.ਬਫਰ_Pos)
{
    init();
}

ਖਾਲੀ
Normalizer::init() {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_ਕੋਡ);
    ਜੇ(fOptions&UNORM_UNICODE_3_2) {
        ਮਿਟਾਓ fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            ਨਵਾਂ FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_ਕੋਡ));
    }
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        error_ਕੋਡ=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_ਕੋਡ);
    }
}

Normalizer::~Normalizer()
{
    ਮਿਟਾਓ fFilteredNorm2;
    ਮਿਟਾਓ ਪਾਠ;
}

Normalizer* 
Normalizer::clone() ਅਚਲ
{
    ਵਾਪਸ ਨਵਾਂ Normalizer(*ਇਹ);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_ਕੋਡ() ਅਚਲ
{
    ਵਾਪਸ ਪਾਠ->hash_ਕੋਡ() + fUMode + fOptions + ਬਫਰ.hash_ਕੋਡ() + ਬਫਰ_Pos + current_ਸੂਚਕਾਂਕ + ਅਗਲਾ_ਸੂਚਕਾਂਕ;
}
    
ਤਾਰਕਿਕ Normalizer::ਸੰਚਾਲਕ==(ਅਚਲ Normalizer& that) ਅਚਲ
{
    ਵਾਪਸ
        ਇਹ==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *ਪਾਠ==*that.ਪਾਠ &&
        ਬਫਰ==that.ਬਫਰ &&
        ਬਫਰ_Pos==that.ਬਫਰ_Pos &&
        ਅਗਲਾ_ਸੂਚਕਾਂਕ==that.ਅਗਲਾ_ਸੂਚਕਾਂਕ);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

ਖਾਲੀ U_EXPORT2
Normalizer::normalize(ਅਚਲ UnicodeString& ਸਰੋਤ, 
                      UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
                      UnicodeString& ਨਤੀਜਾ, 
                      UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ(ਸਰੋਤ.isBogus() || U_FAILURE(ਸਥਿਤੀ)) {
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
            ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
        UnicodeString localDest;
        UnicodeString *dest;

        ਜੇ(&ਸਰੋਤ!=&ਨਤੀਜਾ) {
            dest=&ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, ਸਥਿਤੀ);
        ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
            ਜੇ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(ਸਥਿਤੀ)).
                    normalize(ਸਰੋਤ, *dest, ਸਥਿਤੀ);
            } ਨਹੀਂਤਾਂ {
                n2->normalize(ਸਰੋਤ, *dest, ਸਥਿਤੀ);
            }
        }
        ਜੇ(dest==&localDest && U_SUCCESS(ਸਥਿਤੀ)) {
            ਨਤੀਜਾ=*dest;
        }
    }
}

ਖਾਲੀ U_EXPORT2
Normalizer::compose(ਅਚਲ UnicodeString& ਸਰੋਤ, 
                    UBool compat, int32_t options,
                    UnicodeString& ਨਤੀਜਾ, 
                    UError_ਕੋਡ &ਸਥਿਤੀ) {
    normalize(ਸਰੋਤ, compat ? UNORM_NFKC : UNORM_NFC, options, ਨਤੀਜਾ, ਸਥਿਤੀ);
}

ਖਾਲੀ U_EXPORT2
Normalizer::decompose(ਅਚਲ UnicodeString& ਸਰੋਤ, 
                      UBool compat, int32_t options,
                      UnicodeString& ਨਤੀਜਾ, 
                      UError_ਕੋਡ &ਸਥਿਤੀ) {
    normalize(ਸਰੋਤ, compat ? UNORM_NFKD : UNORM_NFD, options, ਨਤੀਜਾ, ਸਥਿਤੀ);
}

UNormalization_Check_ਨਤੀਜਾ
Normalizer::ਤੇਜ਼_Check(ਅਚਲ UnicodeString& ਸਰੋਤ,
                       UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
                       UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, ਸਥਿਤੀ);
    ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
        ਜੇ(options&UNORM_UNICODE_3_2) {
            ਵਾਪਸ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(ਸਥਿਤੀ)).
                ਤੇਜ਼_Check(ਸਰੋਤ, ਸਥਿਤੀ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ n2->ਤੇਜ਼_Check(ਸਰੋਤ, ਸਥਿਤੀ);
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ਅਚਲ UnicodeString& ਸਰੋਤ,
                         UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
                         UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, ਸਥਿਤੀ);
    ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
        ਜੇ(options&UNORM_UNICODE_3_2) {
            ਵਾਪਸ FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(ਸਥਿਤੀ)).
                isNormalized(ਸਰੋਤ, ਸਥਿਤੀ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ n2->isNormalized(ਸਰੋਤ, ਸਥਿਤੀ);
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਝੂਠ;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ਅਚਲ UnicodeString& ਖੱਬੇ,
                        ਅਚਲ UnicodeString& ਸੱਜੇ,
                        UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                        UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ,
                        int32_t options,
                        UError_ਕੋਡ& error_ਕੋਡ) {
    ਜੇ(ਖੱਬੇ.isBogus() || ਸੱਜੇ.isBogus() || U_FAILURE(error_ਕੋਡ)) {
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
        UnicodeString localDest;
        UnicodeString *dest;

        ਜੇ(&ਸੱਜੇ!=&ਨਤੀਜਾ) {
            dest=&ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=ਖੱਬੇ;
        ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            ਜੇ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_ਕੋਡ)).
                    append(*dest, ਸੱਜੇ, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                n2->append(*dest, ਸੱਜੇ, error_ਕੋਡ);
            }
        }
        ਜੇ(dest==&localDest && U_SUCCESS(error_ਕੋਡ)) {
            ਨਤੀਜਾ=*dest;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    ਜੇ(ਬਫਰ_Pos<ਬਫਰ.length() || ਅਗਲਾ_Normalize()) {
        ਵਾਪਸ ਬਫਰ.char32At(ਬਫਰ_Pos);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    ਜੇ(ਬਫਰ_Pos<ਬਫਰ.length() ||  ਅਗਲਾ_Normalize()) {
        UChar32 c=ਬਫਰ.char32At(ਬਫਰ_Pos);
        ਬਫਰ_Pos+=U16_ਲੰਮਾਈ(c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::ਪਿਛਲਾ() {
    ਜੇ(ਬਫਰ_Pos>0 || ਪਿਛਲਾ_Normalize()) {
        UChar32 c=ਬਫਰ.char32At(ਬਫਰ_Pos-1);
        ਬਫਰ_Pos-=U16_ਲੰਮਾਈ(c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

ਖਾਲੀ Normalizer::reset() {
    current_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸੂਚਕਾਂਕ=ਪਾਠ->ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ();
    clear_ਬਫਰ();
}

ਖਾਲੀ
Normalizer::ਸੈੱਟ_ਸੂਚਕਾਂਕ_Only(int32_t ਸੂਚਕਾਂਕ) {
    ਪਾਠ->ਸੈੱਟ_ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ);  // pins index
    current_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸੂਚਕਾਂਕ=ਪਾਠ->get_ਸੂਚਕਾਂਕ();
    clear_ਬਫਰ();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    ਵਾਪਸ next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::ਆਖਰੀ() {
    current_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸੂਚਕਾਂਕ=ਪਾਠ->ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਸਿਰਾ();
    clear_ਬਫਰ();
    ਵਾਪਸ ਪਿਛਲਾ();
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
int32_t Normalizer::get_ਸੂਚਕਾਂਕ() ਅਚਲ {
    ਜੇ(ਬਫਰ_Pos<ਬਫਰ.length()) {
        ਵਾਪਸ current_ਸੂਚਕਾਂਕ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਅਗਲਾ_ਸੂਚਕਾਂਕ;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ() ਅਚਲ {
    ਵਾਪਸ ਪਾਠ->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::ਸਿਰਾ_ਸੂਚਕਾਂਕ() ਅਚਲ {
    ਵਾਪਸ ਪਾਠ->ਸਿਰਾ_ਸੂਚਕਾਂਕ();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

ਖਾਲੀ
Normalizer::ਸੈੱਟ_ਬਹੁਲਕ(UNormalization_ਬਹੁਲਕ new_ਬਹੁਲਕ) 
{
    fUMode = new_ਬਹੁਲਕ;
    init();
}

UNormalization_ਬਹੁਲਕ
Normalizer::getUMode() ਅਚਲ
{
    ਵਾਪਸ fUMode;
}

ਖਾਲੀ
Normalizer::ਸੈੱਟ_Option(int32_t option, 
                      UBool value) 
{
    ਜੇ (value) {
        fOptions |= option;
    } ਨਹੀਂਤਾਂ {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ਅਚਲ
{
    ਵਾਪਸ (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
ਖਾਲੀ
Normalizer::ਸੈੱਟ_ਪਾਠ(ਅਚਲ UnicodeString& new_ਪਾਠ, 
                    UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    CharacterIterator *newIter = ਨਵਾਂ StringCharacterIterator(new_ਪਾਠ);
    ਜੇ (newIter == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਮਿਟਾਓ ਪਾਠ;
    ਪਾਠ = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
ਖਾਲੀ
Normalizer::ਸੈੱਟ_ਪਾਠ(ਅਚਲ CharacterIterator& new_ਪਾਠ, 
                    UError_ਕੋਡ &ਸਥਿਤੀ) 
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    CharacterIterator *newIter = new_ਪਾਠ.clone();
    ਜੇ (newIter == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਮਿਟਾਓ ਪਾਠ;
    ਪਾਠ = newIter;
    reset();
}

ਖਾਲੀ
Normalizer::ਸੈੱਟ_ਪਾਠ(ConstChar16Ptr new_ਪਾਠ,
                    int32_t length,
                    UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    CharacterIterator *newIter = ਨਵਾਂ UCharCharacterIterator(new_ਪਾਠ, length);
    ਜੇ (newIter == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਮਿਟਾਓ ਪਾਠ;
    ਪਾਠ = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
ਖਾਲੀ
Normalizer::get_ਪਾਠ(UnicodeString&  ਨਤੀਜਾ) 
{
    ਪਾਠ->get_ਪਾਠ(ਨਤੀਜਾ);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

ਖਾਲੀ Normalizer::clear_ਬਫਰ() {
    ਬਫਰ.remove();
    ਬਫਰ_Pos=0;
}

UBool
Normalizer::ਅਗਲਾ_Normalize() {
    clear_ਬਫਰ();
    current_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸੂਚਕਾਂਕ;
    ਪਾਠ->ਸੈੱਟ_ਸੂਚਕਾਂਕ(ਅਗਲਾ_ਸੂਚਕਾਂਕ);
    ਜੇ(!ਪਾਠ->has_ਅਗਲਾ()) {
        ਵਾਪਸ ਝੂਠ;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(ਪਾਠ->next32PostInc());
    ਜਦੋਂ(ਪਾਠ->has_ਅਗਲਾ()) {
        UChar32 c;
        ਜੇ(fNorm2->hasBoundaryBefore(c=ਪਾਠ->next32PostInc())) {
            ਪਾਠ->move32(-1, CharacterIterator::kCurrent);
            ਰੋਕੋ;
        }
        segment.append(c);
    }
    ਅਗਲਾ_ਸੂਚਕਾਂਕ=ਪਾਠ->get_ਸੂਚਕਾਂਕ();
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    fNorm2->normalize(segment, ਬਫਰ, error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && !ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ();
}

UBool
Normalizer::ਪਿਛਲਾ_Normalize() {
    clear_ਬਫਰ();
    ਅਗਲਾ_ਸੂਚਕਾਂਕ=current_ਸੂਚਕਾਂਕ;
    ਪਾਠ->ਸੈੱਟ_ਸੂਚਕਾਂਕ(current_ਸੂਚਕਾਂਕ);
    ਜੇ(!ਪਾਠ->has_ਪਿਛਲਾ()) {
        ਵਾਪਸ ਝੂਠ;
    }
    UnicodeString segment;
    ਜਦੋਂ(ਪਾਠ->has_ਪਿਛਲਾ()) {
        UChar32 c=ਪਾਠ->previous32();
        segment.insert(0, c);
        ਜੇ(fNorm2->hasBoundaryBefore(c)) {
            ਰੋਕੋ;
        }
    }
    current_ਸੂਚਕਾਂਕ=ਪਾਠ->get_ਸੂਚਕਾਂਕ();
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    fNorm2->normalize(segment, ਬਫਰ, error_ਕੋਡ);
    ਬਫਰ_Pos=ਬਫਰ.length();
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && !ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ();
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_NORMALIZATION */

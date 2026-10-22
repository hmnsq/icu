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

#إذا !UCONFIG_NO_NORMALIZATION

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

#إذا defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#تعریف_لغوه move32
#د_که_پای

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ثابت UnicodeString& str, UNormalization_ډېرپېښ ډېرپېښ) :
    UObject(), fFilteredNorm2(تش_اشاره), fNorm2(تش_اشاره), fUMode(ډېرپېښ), fOptions(0),
    متن(جديد StringCharacterIterator(str)),
    current_شاخص(0), بل_شاخص(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_ډېرپېښ ډېرپېښ) :
    UObject(), fFilteredNorm2(تش_اشاره), fNorm2(تش_اشاره), fUMode(ډېرپېښ), fOptions(0),
    متن(جديد UCharCharacterIterator(str, length)),
    current_شاخص(0), بل_شاخص(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت CharacterIterator& iter, UNormalization_ډېرپېښ ډېرپېښ) :
    UObject(), fFilteredNorm2(تش_اشاره), fNorm2(تش_اشاره), fUMode(ډېرپېښ), fOptions(0),
    متن(iter.clone()),
    current_شاخص(0), بل_شاخص(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت Normalizer &copy) :
    UObject(copy), fFilteredNorm2(تش_اشاره), fNorm2(تش_اشاره), fUMode(copy.fUMode), fOptions(copy.fOptions),
    متن(copy.متن->clone()),
    current_شاخص(copy.current_شاخص), بل_شاخص(copy.بل_شاخص),
    بفر(copy.بفر), بفر_Pos(copy.بفر_Pos)
{
    init();
}

فراغ
Normalizer::init() {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_کوډ);
    إذا(fOptions&UNORM_UNICODE_3_2) {
        ړنګول fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            جديد FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_کوډ));
    }
    إذا(U_FAILURE(error_کوډ)) {
        error_کوډ=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_کوډ);
    }
}

Normalizer::~Normalizer()
{
    ړنګول fFilteredNorm2;
    ړنګول متن;
}

Normalizer* 
Normalizer::clone() ثابت
{
    أرجع جديد Normalizer(*هذا);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_کوډ() ثابت
{
    أرجع متن->hash_کوډ() + fUMode + fOptions + بفر.hash_کوډ() + بفر_Pos + current_شاخص + بل_شاخص;
}
    
منطقي Normalizer::چلوونکی==(ثابت Normalizer& that) ثابت
{
    أرجع
        هذا==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *متن==*that.متن &&
        بفر==that.بفر &&
        بفر_Pos==that.بفر_Pos &&
        بل_شاخص==that.بل_شاخص);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

فراغ U_EXPORT2
Normalizer::normalize(ثابت UnicodeString& سرچینه, 
                      UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
                      UnicodeString& پایله, 
                      UError_کوډ &حالت) {
    إذا(سرچینه.isBogus() || U_FAILURE(حالت)) {
        پایله.سېټ_منزل_Bogus();
        إذا(U_SUCCESS(حالت)) {
            حالت=U_ILLEGAL_دلیل_ERROR;
        }
    } وإلا {
        UnicodeString localDest;
        UnicodeString *dest;

        إذا(&سرچینه!=&پایله) {
            dest=&پایله;
        } وإلا {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, حالت);
        إذا(U_SUCCESS(حالت)) {
            إذا(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                    normalize(سرچینه, *dest, حالت);
            } وإلا {
                n2->normalize(سرچینه, *dest, حالت);
            }
        }
        إذا(dest==&localDest && U_SUCCESS(حالت)) {
            پایله=*dest;
        }
    }
}

فراغ U_EXPORT2
Normalizer::compose(ثابت UnicodeString& سرچینه, 
                    UBool compat, int32_t options,
                    UnicodeString& پایله, 
                    UError_کوډ &حالت) {
    normalize(سرچینه, compat ? UNORM_NFKC : UNORM_NFC, options, پایله, حالت);
}

فراغ U_EXPORT2
Normalizer::decompose(ثابت UnicodeString& سرچینه, 
                      UBool compat, int32_t options,
                      UnicodeString& پایله, 
                      UError_کوډ &حالت) {
    normalize(سرچینه, compat ? UNORM_NFKD : UNORM_NFD, options, پایله, حالت);
}

UNormalization_Check_پایله
Normalizer::چټک_Check(ثابت UnicodeString& سرچینه,
                       UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
                       UError_کوډ &حالت) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, حالت);
    إذا(U_SUCCESS(حالت)) {
        إذا(options&UNORM_UNICODE_3_2) {
            أرجع FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                چټک_Check(سرچینه, حالت);
        } وإلا {
            أرجع n2->چټک_Check(سرچینه, حالت);
        }
    } وإلا {
        أرجع UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ثابت UnicodeString& سرچینه,
                         UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
                         UError_کوډ &حالت) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, حالت);
    إذا(U_SUCCESS(حالت)) {
        إذا(options&UNORM_UNICODE_3_2) {
            أرجع FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                isNormalized(سرچینه, حالت);
        } وإلا {
            أرجع n2->isNormalized(سرچینه, حالت);
        }
    } وإلا {
        أرجع خطأ;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ثابت UnicodeString& کیڼ,
                        ثابت UnicodeString& ښی,
                        UnicodeString& پایله U_LIFETIME_BOUND,
                        UNormalization_ډېرپېښ ډېرپېښ,
                        int32_t options,
                        UError_کوډ& error_کوډ) {
    إذا(کیڼ.isBogus() || ښی.isBogus() || U_FAILURE(error_کوډ)) {
        پایله.سېټ_منزل_Bogus();
        إذا(U_SUCCESS(error_کوډ)) {
            error_کوډ=U_ILLEGAL_دلیل_ERROR;
        }
    } وإلا {
        UnicodeString localDest;
        UnicodeString *dest;

        إذا(&ښی!=&پایله) {
            dest=&پایله;
        } وإلا {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=کیڼ;
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, error_کوډ);
        إذا(U_SUCCESS(error_کوډ)) {
            إذا(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_کوډ)).
                    append(*dest, ښی, error_کوډ);
            } وإلا {
                n2->append(*dest, ښی, error_کوډ);
            }
        }
        إذا(dest==&localDest && U_SUCCESS(error_کوډ)) {
            پایله=*dest;
        }
    }
    أرجع پایله;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    إذا(بفر_Pos<بفر.length() || بل_Normalize()) {
        أرجع بفر.char32At(بفر_Pos);
    } وإلا {
        أرجع بشپړ_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    إذا(بفر_Pos<بفر.length() ||  بل_Normalize()) {
        UChar32 c=بفر.char32At(بفر_Pos);
        بفر_Pos+=U16_اوږدتیا(c);
        أرجع c;
    } وإلا {
        أرجع بشپړ_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::پخوانی() {
    إذا(بفر_Pos>0 || پخوانی_Normalize()) {
        UChar32 c=بفر.char32At(بفر_Pos-1);
        بفر_Pos-=U16_اوږدتیا(c);
        أرجع c;
    } وإلا {
        أرجع بشپړ_2;
    }
}

فراغ Normalizer::reset() {
    current_شاخص=بل_شاخص=متن->سېټ_منزل_پیل();
    clear_بفر();
}

فراغ
Normalizer::سېټ_شاخص_Only(int32_t شاخص) {
    متن->سېټ_شاخص(شاخص);  // pins index
    current_شاخص=بل_شاخص=متن->get_شاخص();
    clear_بفر();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    أرجع next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::وروستی() {
    current_شاخص=بل_شاخص=متن->سېټ_منزل_پای_ټکی();
    clear_بفر();
    أرجع پخوانی();
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
int32_t Normalizer::get_شاخص() ثابت {
    إذا(بفر_Pos<بفر.length()) {
        أرجع current_شاخص;
    } وإلا {
        أرجع بل_شاخص;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::پیل_شاخص() ثابت {
    أرجع متن->پیل_شاخص();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::پای_ټکی_شاخص() ثابت {
    أرجع متن->پای_ټکی_شاخص();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

فراغ
Normalizer::سېټ_ډېرپېښ(UNormalization_ډېرپېښ new_ډېرپېښ) 
{
    fUMode = new_ډېرپېښ;
    init();
}

UNormalization_ډېرپېښ
Normalizer::getUMode() ثابت
{
    أرجع fUMode;
}

فراغ
Normalizer::سېټ_Option(int32_t option, 
                      UBool value) 
{
    إذا (value) {
        fOptions |= option;
    } وإلا {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ثابت
{
    أرجع (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
فراغ
Normalizer::سېټ_متن(ثابت UnicodeString& new_متن, 
                    UError_کوډ &حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = جديد StringCharacterIterator(new_متن);
    إذا (newIter == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    ړنګول متن;
    متن = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
فراغ
Normalizer::سېټ_متن(ثابت CharacterIterator& new_متن, 
                    UError_کوډ &حالت) 
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = new_متن.clone();
    إذا (newIter == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    ړنګول متن;
    متن = newIter;
    reset();
}

فراغ
Normalizer::سېټ_متن(ConstChar16Ptr new_متن,
                    int32_t length,
                    UError_کوډ &حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = جديد UCharCharacterIterator(new_متن, length);
    إذا (newIter == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    ړنګول متن;
    متن = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
فراغ
Normalizer::get_متن(UnicodeString&  پایله) 
{
    متن->get_متن(پایله);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

فراغ Normalizer::clear_بفر() {
    بفر.remove();
    بفر_Pos=0;
}

UBool
Normalizer::بل_Normalize() {
    clear_بفر();
    current_شاخص=بل_شاخص;
    متن->سېټ_شاخص(بل_شاخص);
    إذا(!متن->has_بل()) {
        أرجع خطأ;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(متن->next32PostInc());
    طالما(متن->has_بل()) {
        UChar32 c;
        إذا(fNorm2->hasBoundaryBefore(c=متن->next32PostInc())) {
            متن->move32(-1, CharacterIterator::kCurrent);
            اكسر;
        }
        segment.append(c);
    }
    بل_شاخص=متن->get_شاخص();
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    fNorm2->normalize(segment, بفر, error_کوډ);
    أرجع U_SUCCESS(error_کوډ) && !بفر.is_تشه();
}

UBool
Normalizer::پخوانی_Normalize() {
    clear_بفر();
    بل_شاخص=current_شاخص;
    متن->سېټ_شاخص(current_شاخص);
    إذا(!متن->has_پخوانی()) {
        أرجع خطأ;
    }
    UnicodeString segment;
    طالما(متن->has_پخوانی()) {
        UChar32 c=متن->previous32();
        segment.insert(0, c);
        إذا(fNorm2->hasBoundaryBefore(c)) {
            اكسر;
        }
    }
    current_شاخص=متن->get_شاخص();
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    fNorm2->normalize(segment, بفر, error_کوډ);
    بفر_Pos=بفر.length();
    أرجع U_SUCCESS(error_کوډ) && !بفر.is_تشه();
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_NORMALIZATION */

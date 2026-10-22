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
#تعریف_منسوخ move32
#اگر_ختم

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ثابت UnicodeString& str, UNormalization_منوال منوال) :
    UObject(), fFilteredNorm2(خالی_اشاریہ), fNorm2(خالی_اشاریہ), fUMode(منوال), fOptions(0),
    متن(نیا StringCharacterIterator(str)),
    current_اشاریہ(0), اگلا_اشاریہ(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_منوال منوال) :
    UObject(), fFilteredNorm2(خالی_اشاریہ), fNorm2(خالی_اشاریہ), fUMode(منوال), fOptions(0),
    متن(نیا UCharCharacterIterator(str, length)),
    current_اشاریہ(0), اگلا_اشاریہ(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت CharacterIterator& iter, UNormalization_منوال منوال) :
    UObject(), fFilteredNorm2(خالی_اشاریہ), fNorm2(خالی_اشاریہ), fUMode(منوال), fOptions(0),
    متن(iter.clone()),
    current_اشاریہ(0), اگلا_اشاریہ(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت Normalizer &copy) :
    UObject(copy), fFilteredNorm2(خالی_اشاریہ), fNorm2(خالی_اشاریہ), fUMode(copy.fUMode), fOptions(copy.fOptions),
    متن(copy.متن->clone()),
    current_اشاریہ(copy.current_اشاریہ), اگلا_اشاریہ(copy.اگلا_اشاریہ),
    بفر(copy.بفر), بفر_Pos(copy.بفر_Pos)
{
    init();
}

خالی
Normalizer::init() {
    UError_رمز error_رمز=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_رمز);
    اگر(fOptions&UNORM_UNICODE_3_2) {
        حذف fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            نیا FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_رمز));
    }
    اگر(U_FAILURE(error_رمز)) {
        error_رمز=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_رمز);
    }
}

Normalizer::~Normalizer()
{
    حذف fFilteredNorm2;
    حذف متن;
}

Normalizer* 
Normalizer::clone() ثابت
{
    واپس نیا Normalizer(*یہ);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_رمز() ثابت
{
    واپس متن->hash_رمز() + fUMode + fOptions + بفر.hash_رمز() + بفر_Pos + current_اشاریہ + اگلا_اشاریہ;
}
    
منطقی Normalizer::عامل==(ثابت Normalizer& that) ثابت
{
    واپس
        یہ==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *متن==*that.متن &&
        بفر==that.بفر &&
        بفر_Pos==that.بفر_Pos &&
        اگلا_اشاریہ==that.اگلا_اشاریہ);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

خالی U_EXPORT2
Normalizer::normalize(ثابت UnicodeString& ماخذ, 
                      UNormalization_منوال منوال, int32_t options,
                      UnicodeString& نتیجہ, 
                      UError_رمز &حالت_2) {
    اگر(ماخذ.isBogus() || U_FAILURE(حالت_2)) {
        نتیجہ.سیٹ_مقصد_Bogus();
        اگر(U_SUCCESS(حالت_2)) {
            حالت_2=U_ILLEGAL_دلیل_ERROR;
        }
    } ورنہ {
        UnicodeString localDest;
        UnicodeString *dest;

        اگر(&ماخذ!=&نتیجہ) {
            dest=&نتیجہ;
        } ورنہ {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, حالت_2);
        اگر(U_SUCCESS(حالت_2)) {
            اگر(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت_2)).
                    normalize(ماخذ, *dest, حالت_2);
            } ورنہ {
                n2->normalize(ماخذ, *dest, حالت_2);
            }
        }
        اگر(dest==&localDest && U_SUCCESS(حالت_2)) {
            نتیجہ=*dest;
        }
    }
}

خالی U_EXPORT2
Normalizer::compose(ثابت UnicodeString& ماخذ, 
                    UBool compat, int32_t options,
                    UnicodeString& نتیجہ, 
                    UError_رمز &حالت_2) {
    normalize(ماخذ, compat ? UNORM_NFKC : UNORM_NFC, options, نتیجہ, حالت_2);
}

خالی U_EXPORT2
Normalizer::decompose(ثابت UnicodeString& ماخذ, 
                      UBool compat, int32_t options,
                      UnicodeString& نتیجہ, 
                      UError_رمز &حالت_2) {
    normalize(ماخذ, compat ? UNORM_NFKD : UNORM_NFD, options, نتیجہ, حالت_2);
}

UNormalization_Check_نتیجہ
Normalizer::تیز_Check(ثابت UnicodeString& ماخذ,
                       UNormalization_منوال منوال, int32_t options,
                       UError_رمز &حالت_2) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, حالت_2);
    اگر(U_SUCCESS(حالت_2)) {
        اگر(options&UNORM_UNICODE_3_2) {
            واپس FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت_2)).
                تیز_Check(ماخذ, حالت_2);
        } ورنہ {
            واپس n2->تیز_Check(ماخذ, حالت_2);
        }
    } ورنہ {
        واپس UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ثابت UnicodeString& ماخذ,
                         UNormalization_منوال منوال, int32_t options,
                         UError_رمز &حالت_2) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, حالت_2);
    اگر(U_SUCCESS(حالت_2)) {
        اگر(options&UNORM_UNICODE_3_2) {
            واپس FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت_2)).
                isNormalized(ماخذ, حالت_2);
        } ورنہ {
            واپس n2->isNormalized(ماخذ, حالت_2);
        }
    } ورنہ {
        واپس جھوٹ;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ثابت UnicodeString& بائیں,
                        ثابت UnicodeString& دائیں,
                        UnicodeString& نتیجہ U_LIFETIME_BOUND,
                        UNormalization_منوال منوال,
                        int32_t options,
                        UError_رمز& error_رمز) {
    اگر(بائیں.isBogus() || دائیں.isBogus() || U_FAILURE(error_رمز)) {
        نتیجہ.سیٹ_مقصد_Bogus();
        اگر(U_SUCCESS(error_رمز)) {
            error_رمز=U_ILLEGAL_دلیل_ERROR;
        }
    } ورنہ {
        UnicodeString localDest;
        UnicodeString *dest;

        اگر(&دائیں!=&نتیجہ) {
            dest=&نتیجہ;
        } ورنہ {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=بائیں;
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, error_رمز);
        اگر(U_SUCCESS(error_رمز)) {
            اگر(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_رمز)).
                    append(*dest, دائیں, error_رمز);
            } ورنہ {
                n2->append(*dest, دائیں, error_رمز);
            }
        }
        اگر(dest==&localDest && U_SUCCESS(error_رمز)) {
            نتیجہ=*dest;
        }
    }
    واپس نتیجہ;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    اگر(بفر_Pos<بفر.length() || اگلا_Normalize()) {
        واپس بفر.char32At(بفر_Pos);
    } ورنہ {
        واپس مکمل_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    اگر(بفر_Pos<بفر.length() ||  اگلا_Normalize()) {
        UChar32 c=بفر.char32At(بفر_Pos);
        بفر_Pos+=U16_لمبائی(c);
        واپس c;
    } ورنہ {
        واپس مکمل_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::پچھلا_2() {
    اگر(بفر_Pos>0 || پچھلا_Normalize()) {
        UChar32 c=بفر.char32At(بفر_Pos-1);
        بفر_Pos-=U16_لمبائی(c);
        واپس c;
    } ورنہ {
        واپس مکمل_2;
    }
}

خالی Normalizer::reset() {
    current_اشاریہ=اگلا_اشاریہ=متن->سیٹ_مقصد_آغاز();
    clear_بفر();
}

خالی
Normalizer::سیٹ_اشاریہ_Only(int32_t اشاریہ) {
    متن->سیٹ_اشاریہ(اشاریہ);  // pins index
    current_اشاریہ=اگلا_اشاریہ=متن->get_اشاریہ();
    clear_بفر();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    واپس next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::آخری() {
    current_اشاریہ=اگلا_اشاریہ=متن->سیٹ_مقصد_انتہا();
    clear_بفر();
    واپس پچھلا_2();
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
int32_t Normalizer::get_اشاریہ() ثابت {
    اگر(بفر_Pos<بفر.length()) {
        واپس current_اشاریہ;
    } ورنہ {
        واپس اگلا_اشاریہ;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::آغاز_اشاریہ() ثابت {
    واپس متن->آغاز_اشاریہ();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::انتہا_اشاریہ() ثابت {
    واپس متن->انتہا_اشاریہ();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

خالی
Normalizer::سیٹ_منوال(UNormalization_منوال new_منوال) 
{
    fUMode = new_منوال;
    init();
}

UNormalization_منوال
Normalizer::getUMode() ثابت
{
    واپس fUMode;
}

خالی
Normalizer::سیٹ_Option(int32_t option, 
                      UBool value) 
{
    اگر (value) {
        fOptions |= option;
    } ورنہ {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ثابت
{
    واپس (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
خالی
Normalizer::سیٹ_متن(ثابت UnicodeString& new_متن, 
                    UError_رمز &حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    CharacterIterator *newIter = نیا StringCharacterIterator(new_متن);
    اگر (newIter == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    حذف متن;
    متن = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
خالی
Normalizer::سیٹ_متن(ثابت CharacterIterator& new_متن, 
                    UError_رمز &حالت_2) 
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    CharacterIterator *newIter = new_متن.clone();
    اگر (newIter == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    حذف متن;
    متن = newIter;
    reset();
}

خالی
Normalizer::سیٹ_متن(ConstChar16Ptr new_متن,
                    int32_t length,
                    UError_رمز &حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    CharacterIterator *newIter = نیا UCharCharacterIterator(new_متن, length);
    اگر (newIter == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    حذف متن;
    متن = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
خالی
Normalizer::get_متن(UnicodeString&  نتیجہ) 
{
    متن->get_متن(نتیجہ);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

خالی Normalizer::clear_بفر() {
    بفر.remove();
    بفر_Pos=0;
}

UBool
Normalizer::اگلا_Normalize() {
    clear_بفر();
    current_اشاریہ=اگلا_اشاریہ;
    متن->سیٹ_اشاریہ(اگلا_اشاریہ);
    اگر(!متن->has_اگلا()) {
        واپس جھوٹ;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(متن->next32PostInc());
    جب(متن->has_اگلا()) {
        UChar32 c;
        اگر(fNorm2->hasBoundaryBefore(c=متن->next32PostInc())) {
            متن->move32(-1, CharacterIterator::kCurrent);
            توڑو;
        }
        segment.append(c);
    }
    اگلا_اشاریہ=متن->get_اشاریہ();
    UError_رمز error_رمز=U_ZERO_ERROR;
    fNorm2->normalize(segment, بفر, error_رمز);
    واپس U_SUCCESS(error_رمز) && !بفر.is_خالی_خانے();
}

UBool
Normalizer::پچھلا_Normalize() {
    clear_بفر();
    اگلا_اشاریہ=current_اشاریہ;
    متن->سیٹ_اشاریہ(current_اشاریہ);
    اگر(!متن->has_پچھلا()) {
        واپس جھوٹ;
    }
    UnicodeString segment;
    جب(متن->has_پچھلا()) {
        UChar32 c=متن->previous32();
        segment.insert(0, c);
        اگر(fNorm2->hasBoundaryBefore(c)) {
            توڑو;
        }
    }
    current_اشاریہ=متن->get_اشاریہ();
    UError_رمز error_رمز=U_ZERO_ERROR;
    fNorm2->normalize(segment, بفر, error_رمز);
    بفر_Pos=بفر.length();
    واپس U_SUCCESS(error_رمز) && !بفر.is_خالی_خانے();
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_NORMALIZATION */

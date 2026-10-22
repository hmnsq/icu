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
#تعریف_رد move32
#جے_انت

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ثابت UnicodeString& str, UNormalization_منوال منوال) :
    UObject(), fFilteredNorm2(خالی_سوچک), fNorm2(خالی_سوچک), fUMode(منوال), fOptions(0),
    متن(جديد StringCharacterIterator(str)),
    current_اشاریہ(0), اگلا_اشاریہ(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_منوال منوال) :
    UObject(), fFilteredNorm2(خالی_سوچک), fNorm2(خالی_سوچک), fUMode(منوال), fOptions(0),
    متن(جديد UCharCharacterIterator(str, length)),
    current_اشاریہ(0), اگلا_اشاریہ(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت CharacterIterator& iter, UNormalization_منوال منوال) :
    UObject(), fFilteredNorm2(خالی_سوچک), fNorm2(خالی_سوچک), fUMode(منوال), fOptions(0),
    متن(iter.clone()),
    current_اشاریہ(0), اگلا_اشاریہ(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت Normalizer &copy) :
    UObject(copy), fFilteredNorm2(خالی_سوچک), fNorm2(خالی_سوچک), fUMode(copy.fUMode), fOptions(copy.fOptions),
    متن(copy.متن->clone()),
    current_اشاریہ(copy.current_اشاریہ), اگلا_اشاریہ(copy.اگلا_اشاریہ),
    بفر(copy.بفر), بفر_Pos(copy.بفر_Pos)
{
    init();
}

فراغ
Normalizer::init() {
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_کوڈ);
    إذا(fOptions&UNORM_UNICODE_3_2) {
        مٹاؤ fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            جديد FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_کوڈ));
    }
    إذا(U_FAILURE(error_کوڈ)) {
        error_کوڈ=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_کوڈ);
    }
}

Normalizer::~Normalizer()
{
    مٹاؤ fFilteredNorm2;
    مٹاؤ متن;
}

Normalizer* 
Normalizer::clone() ثابت
{
    أرجع جديد Normalizer(*هذا);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_کوڈ() ثابت
{
    أرجع متن->hash_کوڈ() + fUMode + fOptions + بفر.hash_کوڈ() + بفر_Pos + current_اشاریہ + اگلا_اشاریہ;
}
    
منطقي Normalizer::عامل==(ثابت Normalizer& that) ثابت
{
    أرجع
        هذا==&that ||
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

فراغ U_EXPORT2
Normalizer::normalize(ثابت UnicodeString& سروت, 
                      UNormalization_منوال منوال, int32_t options,
                      UnicodeString& نتیجہ, 
                      UError_کوڈ &حالت) {
    إذا(سروت.isBogus() || U_FAILURE(حالت)) {
        نتیجہ.سیٹ_منزل_Bogus();
        إذا(U_SUCCESS(حالت)) {
            حالت=U_ILLEGAL_دلیل_ERROR;
        }
    } وإلا {
        UnicodeString localDest;
        UnicodeString *dest;

        إذا(&سروت!=&نتیجہ) {
            dest=&نتیجہ;
        } وإلا {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, حالت);
        إذا(U_SUCCESS(حالت)) {
            إذا(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                    normalize(سروت, *dest, حالت);
            } وإلا {
                n2->normalize(سروت, *dest, حالت);
            }
        }
        إذا(dest==&localDest && U_SUCCESS(حالت)) {
            نتیجہ=*dest;
        }
    }
}

فراغ U_EXPORT2
Normalizer::compose(ثابت UnicodeString& سروت, 
                    UBool compat, int32_t options,
                    UnicodeString& نتیجہ, 
                    UError_کوڈ &حالت) {
    normalize(سروت, compat ? UNORM_NFKC : UNORM_NFC, options, نتیجہ, حالت);
}

فراغ U_EXPORT2
Normalizer::decompose(ثابت UnicodeString& سروت, 
                      UBool compat, int32_t options,
                      UnicodeString& نتیجہ, 
                      UError_کوڈ &حالت) {
    normalize(سروت, compat ? UNORM_NFKD : UNORM_NFD, options, نتیجہ, حالت);
}

UNormalization_Check_نتیجہ
Normalizer::تیز_Check(ثابت UnicodeString& سروت,
                       UNormalization_منوال منوال, int32_t options,
                       UError_کوڈ &حالت) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, حالت);
    إذا(U_SUCCESS(حالت)) {
        إذا(options&UNORM_UNICODE_3_2) {
            أرجع FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                تیز_Check(سروت, حالت);
        } وإلا {
            أرجع n2->تیز_Check(سروت, حالت);
        }
    } وإلا {
        أرجع UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ثابت UnicodeString& سروت,
                         UNormalization_منوال منوال, int32_t options,
                         UError_کوڈ &حالت) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, حالت);
    إذا(U_SUCCESS(حالت)) {
        إذا(options&UNORM_UNICODE_3_2) {
            أرجع FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                isNormalized(سروت, حالت);
        } وإلا {
            أرجع n2->isNormalized(سروت, حالت);
        }
    } وإلا {
        أرجع خطأ;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ثابت UnicodeString& کھبے,
                        ثابت UnicodeString& سجے,
                        UnicodeString& نتیجہ U_LIFETIME_BOUND,
                        UNormalization_منوال منوال,
                        int32_t options,
                        UError_کوڈ& error_کوڈ) {
    إذا(کھبے.isBogus() || سجے.isBogus() || U_FAILURE(error_کوڈ)) {
        نتیجہ.سیٹ_منزل_Bogus();
        إذا(U_SUCCESS(error_کوڈ)) {
            error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        }
    } وإلا {
        UnicodeString localDest;
        UnicodeString *dest;

        إذا(&سجے!=&نتیجہ) {
            dest=&نتیجہ;
        } وإلا {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=کھبے;
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, error_کوڈ);
        إذا(U_SUCCESS(error_کوڈ)) {
            إذا(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_کوڈ)).
                    append(*dest, سجے, error_کوڈ);
            } وإلا {
                n2->append(*dest, سجے, error_کوڈ);
            }
        }
        إذا(dest==&localDest && U_SUCCESS(error_کوڈ)) {
            نتیجہ=*dest;
        }
    }
    أرجع نتیجہ;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    إذا(بفر_Pos<بفر.length() || اگلا_Normalize()) {
        أرجع بفر.char32At(بفر_Pos);
    } وإلا {
        أرجع پورا_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    إذا(بفر_Pos<بفر.length() ||  اگلا_Normalize()) {
        UChar32 c=بفر.char32At(بفر_Pos);
        بفر_Pos+=U16_لمائی(c);
        أرجع c;
    } وإلا {
        أرجع پورا_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::پچھلا() {
    إذا(بفر_Pos>0 || پچھلا_Normalize()) {
        UChar32 c=بفر.char32At(بفر_Pos-1);
        بفر_Pos-=U16_لمائی(c);
        أرجع c;
    } وإلا {
        أرجع پورا_2;
    }
}

فراغ Normalizer::reset() {
    current_اشاریہ=اگلا_اشاریہ=متن->سیٹ_منزل_شروع();
    clear_بفر();
}

فراغ
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
    أرجع next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::آخری() {
    current_اشاریہ=اگلا_اشاریہ=متن->سیٹ_منزل_سرا();
    clear_بفر();
    أرجع پچھلا();
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
    إذا(بفر_Pos<بفر.length()) {
        أرجع current_اشاریہ;
    } وإلا {
        أرجع اگلا_اشاریہ;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::شروع_اشاریہ() ثابت {
    أرجع متن->شروع_اشاریہ();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::سرا_اشاریہ() ثابت {
    أرجع متن->سرا_اشاریہ();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

فراغ
Normalizer::سیٹ_منوال(UNormalization_منوال new_منوال) 
{
    fUMode = new_منوال;
    init();
}

UNormalization_منوال
Normalizer::getUMode() ثابت
{
    أرجع fUMode;
}

فراغ
Normalizer::سیٹ_Option(int32_t option, 
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
Normalizer::سیٹ_متن(ثابت UnicodeString& new_متن, 
                    UError_کوڈ &حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = جديد StringCharacterIterator(new_متن);
    إذا (newIter == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    مٹاؤ متن;
    متن = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
فراغ
Normalizer::سیٹ_متن(ثابت CharacterIterator& new_متن, 
                    UError_کوڈ &حالت) 
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = new_متن.clone();
    إذا (newIter == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    مٹاؤ متن;
    متن = newIter;
    reset();
}

فراغ
Normalizer::سیٹ_متن(ConstChar16Ptr new_متن,
                    int32_t length,
                    UError_کوڈ &حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = جديد UCharCharacterIterator(new_متن, length);
    إذا (newIter == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    مٹاؤ متن;
    متن = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
فراغ
Normalizer::get_متن(UnicodeString&  نتیجہ) 
{
    متن->get_متن(نتیجہ);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

فراغ Normalizer::clear_بفر() {
    بفر.remove();
    بفر_Pos=0;
}

UBool
Normalizer::اگلا_Normalize() {
    clear_بفر();
    current_اشاریہ=اگلا_اشاریہ;
    متن->سیٹ_اشاریہ(اگلا_اشاریہ);
    إذا(!متن->has_اگلا()) {
        أرجع خطأ;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(متن->next32PostInc());
    طالما(متن->has_اگلا()) {
        UChar32 c;
        إذا(fNorm2->hasBoundaryBefore(c=متن->next32PostInc())) {
            متن->move32(-1, CharacterIterator::kCurrent);
            اكسر;
        }
        segment.append(c);
    }
    اگلا_اشاریہ=متن->get_اشاریہ();
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    fNorm2->normalize(segment, بفر, error_کوڈ);
    أرجع U_SUCCESS(error_کوڈ) && !بفر.is_خالی_تھاواں();
}

UBool
Normalizer::پچھلا_Normalize() {
    clear_بفر();
    اگلا_اشاریہ=current_اشاریہ;
    متن->سیٹ_اشاریہ(current_اشاریہ);
    إذا(!متن->has_پچھلا()) {
        أرجع خطأ;
    }
    UnicodeString segment;
    طالما(متن->has_پچھلا()) {
        UChar32 c=متن->previous32();
        segment.insert(0, c);
        إذا(fNorm2->hasBoundaryBefore(c)) {
            اكسر;
        }
    }
    current_اشاریہ=متن->get_اشاریہ();
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    fNorm2->normalize(segment, بفر, error_کوڈ);
    بفر_Pos=بفر.length();
    أرجع U_SUCCESS(error_کوڈ) && !بفر.is_خالی_تھاواں();
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_NORMALIZATION */

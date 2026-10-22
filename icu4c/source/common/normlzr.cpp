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
#لغو_تعریف move32
#پایان_اگر

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ثابت UnicodeString& str, UNormalization_نما نما) :
    UObject(), fFilteredNorm2(اشاره_تهی), fNorm2(اشاره_تهی), fUMode(نما), fOptions(0),
    متن(جديد StringCharacterIterator(str)),
    current_شاخص(0), بعدی_شاخص(0),
    میانگیر(), میانگیر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_نما نما) :
    UObject(), fFilteredNorm2(اشاره_تهی), fNorm2(اشاره_تهی), fUMode(نما), fOptions(0),
    متن(جديد UCharCharacterIterator(str, length)),
    current_شاخص(0), بعدی_شاخص(0),
    میانگیر(), میانگیر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت CharacterIterator& iter, UNormalization_نما نما) :
    UObject(), fFilteredNorm2(اشاره_تهی), fNorm2(اشاره_تهی), fUMode(نما), fOptions(0),
    متن(iter.clone()),
    current_شاخص(0), بعدی_شاخص(0),
    میانگیر(), میانگیر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت Normalizer &copy) :
    UObject(copy), fFilteredNorm2(اشاره_تهی), fNorm2(اشاره_تهی), fUMode(copy.fUMode), fOptions(copy.fOptions),
    متن(copy.متن->clone()),
    current_شاخص(copy.current_شاخص), بعدی_شاخص(copy.بعدی_شاخص),
    میانگیر(copy.میانگیر), میانگیر_Pos(copy.میانگیر_Pos)
{
    init();
}

فراغ
Normalizer::init() {
    UError_کد error_کد=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_کد);
    إذا(fOptions&UNORM_UNICODE_3_2) {
        حذف fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            جديد FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_کد));
    }
    إذا(U_FAILURE(error_کد)) {
        error_کد=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_کد);
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
    أرجع جديد Normalizer(*هذا);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_کد() ثابت
{
    أرجع متن->hash_کد() + fUMode + fOptions + میانگیر.hash_کد() + میانگیر_Pos + current_شاخص + بعدی_شاخص;
}
    
منطقي Normalizer::عملگر==(ثابت Normalizer& that) ثابت
{
    أرجع
        هذا==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *متن==*that.متن &&
        میانگیر==that.میانگیر &&
        میانگیر_Pos==that.میانگیر_Pos &&
        بعدی_شاخص==that.بعدی_شاخص);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

فراغ U_EXPORT2
Normalizer::normalize(ثابت UnicodeString& منبع, 
                      UNormalization_نما نما, int32_t options,
                      UnicodeString& نتیجه, 
                      UError_کد &وضعیت) {
    إذا(منبع.isBogus() || U_FAILURE(وضعیت)) {
        نتیجه.مجموعه_مقصد_Bogus();
        إذا(U_SUCCESS(وضعیت)) {
            وضعیت=U_ILLEGAL_آرگومان_ERROR;
        }
    } وإلا {
        UnicodeString localDest;
        UnicodeString *dest;

        إذا(&منبع!=&نتیجه) {
            dest=&نتیجه;
        } وإلا {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, وضعیت);
        إذا(U_SUCCESS(وضعیت)) {
            إذا(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(وضعیت)).
                    normalize(منبع, *dest, وضعیت);
            } وإلا {
                n2->normalize(منبع, *dest, وضعیت);
            }
        }
        إذا(dest==&localDest && U_SUCCESS(وضعیت)) {
            نتیجه=*dest;
        }
    }
}

فراغ U_EXPORT2
Normalizer::compose(ثابت UnicodeString& منبع, 
                    UBool compat, int32_t options,
                    UnicodeString& نتیجه, 
                    UError_کد &وضعیت) {
    normalize(منبع, compat ? UNORM_NFKC : UNORM_NFC, options, نتیجه, وضعیت);
}

فراغ U_EXPORT2
Normalizer::decompose(ثابت UnicodeString& منبع, 
                      UBool compat, int32_t options,
                      UnicodeString& نتیجه, 
                      UError_کد &وضعیت) {
    normalize(منبع, compat ? UNORM_NFKD : UNORM_NFD, options, نتیجه, وضعیت);
}

UNormalization_Check_نتیجه
Normalizer::سریع_Check(ثابت UnicodeString& منبع,
                       UNormalization_نما نما, int32_t options,
                       UError_کد &وضعیت) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, وضعیت);
    إذا(U_SUCCESS(وضعیت)) {
        إذا(options&UNORM_UNICODE_3_2) {
            أرجع FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(وضعیت)).
                سریع_Check(منبع, وضعیت);
        } وإلا {
            أرجع n2->سریع_Check(منبع, وضعیت);
        }
    } وإلا {
        أرجع UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ثابت UnicodeString& منبع,
                         UNormalization_نما نما, int32_t options,
                         UError_کد &وضعیت) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, وضعیت);
    إذا(U_SUCCESS(وضعیت)) {
        إذا(options&UNORM_UNICODE_3_2) {
            أرجع FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(وضعیت)).
                isNormalized(منبع, وضعیت);
        } وإلا {
            أرجع n2->isNormalized(منبع, وضعیت);
        }
    } وإلا {
        أرجع خطأ;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ثابت UnicodeString& چپ,
                        ثابت UnicodeString& راست,
                        UnicodeString& نتیجه U_LIFETIME_BOUND,
                        UNormalization_نما نما,
                        int32_t options,
                        UError_کد& error_کد) {
    إذا(چپ.isBogus() || راست.isBogus() || U_FAILURE(error_کد)) {
        نتیجه.مجموعه_مقصد_Bogus();
        إذا(U_SUCCESS(error_کد)) {
            error_کد=U_ILLEGAL_آرگومان_ERROR;
        }
    } وإلا {
        UnicodeString localDest;
        UnicodeString *dest;

        إذا(&راست!=&نتیجه) {
            dest=&نتیجه;
        } وإلا {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=چپ;
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, error_کد);
        إذا(U_SUCCESS(error_کد)) {
            إذا(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_کد)).
                    append(*dest, راست, error_کد);
            } وإلا {
                n2->append(*dest, راست, error_کد);
            }
        }
        إذا(dest==&localDest && U_SUCCESS(error_کد)) {
            نتیجه=*dest;
        }
    }
    أرجع نتیجه;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    إذا(میانگیر_Pos<میانگیر.length() || بعدی_Normalize()) {
        أرجع میانگیر.char32At(میانگیر_Pos);
    } وإلا {
        أرجع انجامشده_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    إذا(میانگیر_Pos<میانگیر.length() ||  بعدی_Normalize()) {
        UChar32 c=میانگیر.char32At(میانگیر_Pos);
        میانگیر_Pos+=U16_درازی(c);
        أرجع c;
    } وإلا {
        أرجع انجامشده_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::قبلی() {
    إذا(میانگیر_Pos>0 || قبلی_Normalize()) {
        UChar32 c=میانگیر.char32At(میانگیر_Pos-1);
        میانگیر_Pos-=U16_درازی(c);
        أرجع c;
    } وإلا {
        أرجع انجامشده_2;
    }
}

فراغ Normalizer::reset() {
    current_شاخص=بعدی_شاخص=متن->مجموعه_مقصد_شروع();
    clear_میانگیر();
}

فراغ
Normalizer::مجموعه_شاخص_Only(int32_t شاخص) {
    متن->مجموعه_شاخص(شاخص);  // pins index
    current_شاخص=بعدی_شاخص=متن->get_شاخص();
    clear_میانگیر();
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
UChar32 Normalizer::آخرین() {
    current_شاخص=بعدی_شاخص=متن->مجموعه_مقصد_انتها();
    clear_میانگیر();
    أرجع قبلی();
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
    إذا(میانگیر_Pos<میانگیر.length()) {
        أرجع current_شاخص;
    } وإلا {
        أرجع بعدی_شاخص;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::شروع_شاخص() ثابت {
    أرجع متن->شروع_شاخص();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::انتها_شاخص() ثابت {
    أرجع متن->انتها_شاخص();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

فراغ
Normalizer::مجموعه_نما(UNormalization_نما new_نما) 
{
    fUMode = new_نما;
    init();
}

UNormalization_نما
Normalizer::getUMode() ثابت
{
    أرجع fUMode;
}

فراغ
Normalizer::مجموعه_Option(int32_t option, 
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
Normalizer::مجموعه_متن(ثابت UnicodeString& new_متن, 
                    UError_کد &وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    CharacterIterator *newIter = جديد StringCharacterIterator(new_متن);
    إذا (newIter == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    حذف متن;
    متن = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
فراغ
Normalizer::مجموعه_متن(ثابت CharacterIterator& new_متن, 
                    UError_کد &وضعیت) 
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    CharacterIterator *newIter = new_متن.clone();
    إذا (newIter == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    حذف متن;
    متن = newIter;
    reset();
}

فراغ
Normalizer::مجموعه_متن(ConstChar16Ptr new_متن,
                    int32_t length,
                    UError_کد &وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    CharacterIterator *newIter = جديد UCharCharacterIterator(new_متن, length);
    إذا (newIter == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    حذف متن;
    متن = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
فراغ
Normalizer::get_متن(UnicodeString&  نتیجه) 
{
    متن->get_متن(نتیجه);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

فراغ Normalizer::clear_میانگیر() {
    میانگیر.remove();
    میانگیر_Pos=0;
}

UBool
Normalizer::بعدی_Normalize() {
    clear_میانگیر();
    current_شاخص=بعدی_شاخص;
    متن->مجموعه_شاخص(بعدی_شاخص);
    إذا(!متن->has_بعدی()) {
        أرجع خطأ;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(متن->next32PostInc());
    طالما(متن->has_بعدی()) {
        UChar32 c;
        إذا(fNorm2->hasBoundaryBefore(c=متن->next32PostInc())) {
            متن->move32(-1, CharacterIterator::kCurrent);
            اكسر;
        }
        segment.append(c);
    }
    بعدی_شاخص=متن->get_شاخص();
    UError_کد error_کد=U_ZERO_ERROR;
    fNorm2->normalize(segment, میانگیر, error_کد);
    أرجع U_SUCCESS(error_کد) && !میانگیر.is_خالی();
}

UBool
Normalizer::قبلی_Normalize() {
    clear_میانگیر();
    بعدی_شاخص=current_شاخص;
    متن->مجموعه_شاخص(current_شاخص);
    إذا(!متن->has_قبلی()) {
        أرجع خطأ;
    }
    UnicodeString segment;
    طالما(متن->has_قبلی()) {
        UChar32 c=متن->previous32();
        segment.insert(0, c);
        إذا(fNorm2->hasBoundaryBefore(c)) {
            اكسر;
        }
    }
    current_شاخص=متن->get_شاخص();
    UError_کد error_کد=U_ZERO_ERROR;
    fNorm2->normalize(segment, میانگیر, error_کد);
    میانگیر_Pos=میانگیر.length();
    أرجع U_SUCCESS(error_کد) && !میانگیر.is_خالی();
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_NORMALIZATION */

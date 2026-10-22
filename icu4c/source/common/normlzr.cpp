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
#وصف_رد move32
#جيڪڏهن_پورو

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ثابت UnicodeString& str, UNormalization_گهڻائي گهڻائي) :
    UObject(), fFilteredNorm2(خالي_اشارو), fNorm2(خالي_اشارو), fUMode(گهڻائي), fOptions(0),
    متن(جديد StringCharacterIterator(str)),
    current_اشاريو(0), ايندڙ_اشاريو(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_گهڻائي گهڻائي) :
    UObject(), fFilteredNorm2(خالي_اشارو), fNorm2(خالي_اشارو), fUMode(گهڻائي), fOptions(0),
    متن(جديد UCharCharacterIterator(str, length)),
    current_اشاريو(0), ايندڙ_اشاريو(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت CharacterIterator& iter, UNormalization_گهڻائي گهڻائي) :
    UObject(), fFilteredNorm2(خالي_اشارو), fNorm2(خالي_اشارو), fUMode(گهڻائي), fOptions(0),
    متن(iter.clone()),
    current_اشاريو(0), ايندڙ_اشاريو(0),
    بفر(), بفر_Pos(0)
{
    init();
}

Normalizer::Normalizer(ثابت Normalizer &copy) :
    UObject(copy), fFilteredNorm2(خالي_اشارو), fNorm2(خالي_اشارو), fUMode(copy.fUMode), fOptions(copy.fOptions),
    متن(copy.متن->clone()),
    current_اشاريو(copy.current_اشاريو), ايندڙ_اشاريو(copy.ايندڙ_اشاريو),
    بفر(copy.بفر), بفر_Pos(copy.بفر_Pos)
{
    init();
}

فراغ
Normalizer::init() {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_ڪوڊ);
    إذا(fOptions&UNORM_UNICODE_3_2) {
        ميٽيو fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            جديد FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_ڪوڊ));
    }
    إذا(U_FAILURE(error_ڪوڊ)) {
        error_ڪوڊ=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_ڪوڊ);
    }
}

Normalizer::~Normalizer()
{
    ميٽيو fFilteredNorm2;
    ميٽيو متن;
}

Normalizer* 
Normalizer::clone() ثابت
{
    أرجع جديد Normalizer(*هذا);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_ڪوڊ() ثابت
{
    أرجع متن->hash_ڪوڊ() + fUMode + fOptions + بفر.hash_ڪوڊ() + بفر_Pos + current_اشاريو + ايندڙ_اشاريو;
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
        ايندڙ_اشاريو==that.ايندڙ_اشاريو);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

فراغ U_EXPORT2
Normalizer::normalize(ثابت UnicodeString& ذريعو, 
                      UNormalization_گهڻائي گهڻائي, int32_t options,
                      UnicodeString& نتيجو, 
                      UError_ڪوڊ &حالت) {
    إذا(ذريعو.isBogus() || U_FAILURE(حالت)) {
        نتيجو.سيٽ_منزل_Bogus();
        إذا(U_SUCCESS(حالت)) {
            حالت=U_ILLEGAL_دليل_ERROR;
        }
    } وإلا {
        UnicodeString localDest;
        UnicodeString *dest;

        إذا(&ذريعو!=&نتيجو) {
            dest=&نتيجو;
        } وإلا {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, حالت);
        إذا(U_SUCCESS(حالت)) {
            إذا(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                    normalize(ذريعو, *dest, حالت);
            } وإلا {
                n2->normalize(ذريعو, *dest, حالت);
            }
        }
        إذا(dest==&localDest && U_SUCCESS(حالت)) {
            نتيجو=*dest;
        }
    }
}

فراغ U_EXPORT2
Normalizer::compose(ثابت UnicodeString& ذريعو, 
                    UBool compat, int32_t options,
                    UnicodeString& نتيجو, 
                    UError_ڪوڊ &حالت) {
    normalize(ذريعو, compat ? UNORM_NFKC : UNORM_NFC, options, نتيجو, حالت);
}

فراغ U_EXPORT2
Normalizer::decompose(ثابت UnicodeString& ذريعو, 
                      UBool compat, int32_t options,
                      UnicodeString& نتيجو, 
                      UError_ڪوڊ &حالت) {
    normalize(ذريعو, compat ? UNORM_NFKD : UNORM_NFD, options, نتيجو, حالت);
}

UNormalization_Check_نتيجو
Normalizer::تيز_Check(ثابت UnicodeString& ذريعو,
                       UNormalization_گهڻائي گهڻائي, int32_t options,
                       UError_ڪوڊ &حالت) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, حالت);
    إذا(U_SUCCESS(حالت)) {
        إذا(options&UNORM_UNICODE_3_2) {
            أرجع FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                تيز_Check(ذريعو, حالت);
        } وإلا {
            أرجع n2->تيز_Check(ذريعو, حالت);
        }
    } وإلا {
        أرجع UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ثابت UnicodeString& ذريعو,
                         UNormalization_گهڻائي گهڻائي, int32_t options,
                         UError_ڪوڊ &حالت) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, حالت);
    إذا(U_SUCCESS(حالت)) {
        إذا(options&UNORM_UNICODE_3_2) {
            أرجع FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(حالت)).
                isNormalized(ذريعو, حالت);
        } وإلا {
            أرجع n2->isNormalized(ذريعو, حالت);
        }
    } وإلا {
        أرجع خطأ;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ثابت UnicodeString& کاٻي,
                        ثابت UnicodeString& ساڄي,
                        UnicodeString& نتيجو U_LIFETIME_BOUND,
                        UNormalization_گهڻائي گهڻائي,
                        int32_t options,
                        UError_ڪوڊ& error_ڪوڊ) {
    إذا(کاٻي.isBogus() || ساڄي.isBogus() || U_FAILURE(error_ڪوڊ)) {
        نتيجو.سيٽ_منزل_Bogus();
        إذا(U_SUCCESS(error_ڪوڊ)) {
            error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        }
    } وإلا {
        UnicodeString localDest;
        UnicodeString *dest;

        إذا(&ساڄي!=&نتيجو) {
            dest=&نتيجو;
        } وإلا {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=کاٻي;
        ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, error_ڪوڊ);
        إذا(U_SUCCESS(error_ڪوڊ)) {
            إذا(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_ڪوڊ)).
                    append(*dest, ساڄي, error_ڪوڊ);
            } وإلا {
                n2->append(*dest, ساڄي, error_ڪوڊ);
            }
        }
        إذا(dest==&localDest && U_SUCCESS(error_ڪوڊ)) {
            نتيجو=*dest;
        }
    }
    أرجع نتيجو;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    إذا(بفر_Pos<بفر.length() || ايندڙ_Normalize()) {
        أرجع بفر.char32At(بفر_Pos);
    } وإلا {
        أرجع پورو_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    إذا(بفر_Pos<بفر.length() ||  ايندڙ_Normalize()) {
        UChar32 c=بفر.char32At(بفر_Pos);
        بفر_Pos+=U16_ڊيگهائي(c);
        أرجع c;
    } وإلا {
        أرجع پورو_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::اڳيون() {
    إذا(بفر_Pos>0 || اڳيون_Normalize()) {
        UChar32 c=بفر.char32At(بفر_Pos-1);
        بفر_Pos-=U16_ڊيگهائي(c);
        أرجع c;
    } وإلا {
        أرجع پورو_2;
    }
}

فراغ Normalizer::reset() {
    current_اشاريو=ايندڙ_اشاريو=متن->سيٽ_منزل_شروعات();
    clear_بفر();
}

فراغ
Normalizer::سيٽ_اشاريو_Only(int32_t اشاريو) {
    متن->سيٽ_اشاريو(اشاريو);  // pins index
    current_اشاريو=ايندڙ_اشاريو=متن->get_اشاريو();
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
UChar32 Normalizer::آخري() {
    current_اشاريو=ايندڙ_اشاريو=متن->سيٽ_منزل_پڇاڙي();
    clear_بفر();
    أرجع اڳيون();
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
int32_t Normalizer::get_اشاريو() ثابت {
    إذا(بفر_Pos<بفر.length()) {
        أرجع current_اشاريو;
    } وإلا {
        أرجع ايندڙ_اشاريو;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::شروعات_اشاريو() ثابت {
    أرجع متن->شروعات_اشاريو();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::پڇاڙي_اشاريو() ثابت {
    أرجع متن->پڇاڙي_اشاريو();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

فراغ
Normalizer::سيٽ_گهڻائي(UNormalization_گهڻائي new_گهڻائي) 
{
    fUMode = new_گهڻائي;
    init();
}

UNormalization_گهڻائي
Normalizer::getUMode() ثابت
{
    أرجع fUMode;
}

فراغ
Normalizer::سيٽ_Option(int32_t option, 
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
Normalizer::سيٽ_متن(ثابت UnicodeString& new_متن, 
                    UError_ڪوڊ &حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = جديد StringCharacterIterator(new_متن);
    إذا (newIter == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    ميٽيو متن;
    متن = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
فراغ
Normalizer::سيٽ_متن(ثابت CharacterIterator& new_متن, 
                    UError_ڪوڊ &حالت) 
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = new_متن.clone();
    إذا (newIter == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    ميٽيو متن;
    متن = newIter;
    reset();
}

فراغ
Normalizer::سيٽ_متن(ConstChar16Ptr new_متن,
                    int32_t length,
                    UError_ڪوڊ &حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    CharacterIterator *newIter = جديد UCharCharacterIterator(new_متن, length);
    إذا (newIter == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    ميٽيو متن;
    متن = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
فراغ
Normalizer::get_متن(UnicodeString&  نتيجو) 
{
    متن->get_متن(نتيجو);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

فراغ Normalizer::clear_بفر() {
    بفر.remove();
    بفر_Pos=0;
}

UBool
Normalizer::ايندڙ_Normalize() {
    clear_بفر();
    current_اشاريو=ايندڙ_اشاريو;
    متن->سيٽ_اشاريو(ايندڙ_اشاريو);
    إذا(!متن->has_ايندڙ()) {
        أرجع خطأ;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(متن->next32PostInc());
    طالما(متن->has_ايندڙ()) {
        UChar32 c;
        إذا(fNorm2->hasBoundaryBefore(c=متن->next32PostInc())) {
            متن->move32(-1, CharacterIterator::kCurrent);
            اكسر;
        }
        segment.append(c);
    }
    ايندڙ_اشاريو=متن->get_اشاريو();
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    fNorm2->normalize(segment, بفر, error_ڪوڊ);
    أرجع U_SUCCESS(error_ڪوڊ) && !بفر.is_خالي_جايون();
}

UBool
Normalizer::اڳيون_Normalize() {
    clear_بفر();
    ايندڙ_اشاريو=current_اشاريو;
    متن->سيٽ_اشاريو(current_اشاريو);
    إذا(!متن->has_اڳيون()) {
        أرجع خطأ;
    }
    UnicodeString segment;
    طالما(متن->has_اڳيون()) {
        UChar32 c=متن->previous32();
        segment.insert(0, c);
        إذا(fNorm2->hasBoundaryBefore(c)) {
            اكسر;
        }
    }
    current_اشاريو=متن->get_اشاريو();
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    fNorm2->normalize(segment, بفر, error_ڪوڊ);
    بفر_Pos=بفر.length();
    أرجع U_SUCCESS(error_ڪوڊ) && !بفر.is_خالي_جايون();
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_NORMALIZATION */

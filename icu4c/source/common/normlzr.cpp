// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/chariter.h"
#অন্তর্ভাব "unicode/schriter.h"
#অন্তর্ভাব "unicode/uchriter.h"
#অন্তর্ভাব "unicode/normlzr.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "normalizer2impl.h"
#অন্তর্ভাব "uprops.h"  // for uniset_getUnicode32Instance()

#যদি defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#undef move32
#endif

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(ধ্রুবক UnicodeString& str, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(নতুন StringCharacterIterator(str)),
    current_সূচক(0), পরবর্তী_সূচক(0),
    বাফার(), বাফার_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(নতুন UCharCharacterIterator(str, length)),
    current_সূচক(0), পরবর্তী_সূচক(0),
    বাফার(), বাফার_Pos(0)
{
    init();
}

Normalizer::Normalizer(ধ্রুবক CharacterIterator& iter, UNormalizationMode mode) :
    UObject(), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(mode), fOptions(0),
    text(iter.clone()),
    current_সূচক(0), পরবর্তী_সূচক(0),
    বাফার(), বাফার_Pos(0)
{
    init();
}

Normalizer::Normalizer(ধ্রুবক Normalizer &copy) :
    UObject(copy), fFilteredNorm2(nullptr), fNorm2(nullptr), fUMode(copy.fUMode), fOptions(copy.fOptions),
    text(copy.text->clone()),
    current_সূচক(copy.current_সূচক), পরবর্তী_সূচক(copy.পরবর্তী_সূচক),
    বাফার(copy.বাফার), বাফার_Pos(copy.বাফার_Pos)
{
    init();
}

শূন্য
Normalizer::init() {
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, errorCode);
    যদি(fOptions&UNORM_UNICODE_3_2) {
        delete fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            নতুন FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(errorCode));
    }
    যদি(U_FAILURE(errorCode)) {
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
Normalizer::clone() ধ্রুবক
{
    ফেরত নতুন Normalizer(*এই);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hashCode() ধ্রুবক
{
    ফেরত text->hashCode() + fUMode + fOptions + বাফার.hashCode() + বাফার_Pos + current_সূচক + পরবর্তী_সূচক;
}
    
যৌক্তিক Normalizer::operator==(ধ্রুবক Normalizer& that) ধ্রুবক
{
    ফেরত
        এই==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *text==*that.text &&
        বাফার==that.বাফার &&
        বাফার_Pos==that.বাফার_Pos &&
        পরবর্তী_সূচক==that.পরবর্তী_সূচক);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

শূন্য U_EXPORT2
Normalizer::normalize(ধ্রুবক UnicodeString& source, 
                      UNormalizationMode mode, int32_t options,
                      UnicodeString& ফলাফল, 
                      UErrorCode &status) {
    যদি(source.isBogus() || U_FAILURE(status)) {
        ফলাফল.setToBogus();
        যদি(U_SUCCESS(status)) {
            status=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } অন্যথা {
        UnicodeString localDest;
        UnicodeString *dest;

        যদি(&source!=&ফলাফল) {
            dest=&ফলাফল;
        } অন্যথা {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
        যদি(U_SUCCESS(status)) {
            যদি(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                    normalize(source, *dest, status);
            } অন্যথা {
                n2->normalize(source, *dest, status);
            }
        }
        যদি(dest==&localDest && U_SUCCESS(status)) {
            ফলাফল=*dest;
        }
    }
}

শূন্য U_EXPORT2
Normalizer::compose(ধ্রুবক UnicodeString& source, 
                    UBool compat, int32_t options,
                    UnicodeString& ফলাফল, 
                    UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKC : UNORM_NFC, options, ফলাফল, status);
}

শূন্য U_EXPORT2
Normalizer::decompose(ধ্রুবক UnicodeString& source, 
                      UBool compat, int32_t options,
                      UnicodeString& ফলাফল, 
                      UErrorCode &status) {
    normalize(source, compat ? UNORM_NFKD : UNORM_NFD, options, ফলাফল, status);
}

UNormalization_Check_ফলাফল
Normalizer::quickCheck(ধ্রুবক UnicodeString& source,
                       UNormalizationMode mode, int32_t options,
                       UErrorCode &status) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    যদি(U_SUCCESS(status)) {
        যদি(options&UNORM_UNICODE_3_2) {
            ফেরত FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                quickCheck(source, status);
        } অন্যথা {
            ফেরত n2->quickCheck(source, status);
        }
    } অন্যথা {
        ফেরত UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(ধ্রুবক UnicodeString& source,
                         UNormalizationMode mode, int32_t options,
                         UErrorCode &status) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, status);
    যদি(U_SUCCESS(status)) {
        যদি(options&UNORM_UNICODE_3_2) {
            ফেরত FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(status)).
                isNormalized(source, status);
        } অন্যথা {
            ফেরত n2->isNormalized(source, status);
        }
    } অন্যথা {
        ফেরত মিথ্যা;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(ধ্রুবক UnicodeString& left,
                        ধ্রুবক UnicodeString& ডান,
                        UnicodeString& ফলাফল U_LIFETIME_BOUND,
                        UNormalizationMode mode,
                        int32_t options,
                        UErrorCode& errorCode) {
    যদি(left.isBogus() || ডান.isBogus() || U_FAILURE(errorCode)) {
        ফলাফল.setToBogus();
        যদি(U_SUCCESS(errorCode)) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
    } অন্যথা {
        UnicodeString localDest;
        UnicodeString *dest;

        যদি(&ডান!=&ফলাফল) {
            dest=&ফলাফল;
        } অন্যথা {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=left;
        ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, errorCode);
        যদি(U_SUCCESS(errorCode)) {
            যদি(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(errorCode)).
                    append(*dest, ডান, errorCode);
            } অন্যথা {
                n2->append(*dest, ডান, errorCode);
            }
        }
        যদি(dest==&localDest && U_SUCCESS(errorCode)) {
            ফলাফল=*dest;
        }
    }
    ফেরত ফলাফল;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    যদি(বাফার_Pos<বাফার.length() || পরবর্তী_Normalize()) {
        ফেরত বাফার.char32At(বাফার_Pos);
    } অন্যথা {
        ফেরত DONE;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    যদি(বাফার_Pos<বাফার.length() ||  পরবর্তী_Normalize()) {
        UChar32 c=বাফার.char32At(বাফার_Pos);
        বাফার_Pos+=U16_দৈর্ঘ্য(c);
        ফেরত c;
    } অন্যথা {
        ফেরত DONE;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::previous() {
    যদি(বাফার_Pos>0 || previousNormalize()) {
        UChar32 c=বাফার.char32At(বাফার_Pos-1);
        বাফার_Pos-=U16_দৈর্ঘ্য(c);
        ফেরত c;
    } অন্যথা {
        ফেরত DONE;
    }
}

শূন্য Normalizer::reset() {
    current_সূচক=পরবর্তী_সূচক=text->setToStart();
    clear_বাফার();
}

শূন্য
Normalizer::set_সূচক_Only(int32_t সূচক) {
    text->set_সূচক(সূচক);  // pins index
    current_সূচক=পরবর্তী_সূচক=text->get_সূচক();
    clear_বাফার();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    ফেরত next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::last() {
    current_সূচক=পরবর্তী_সূচক=text->setToEnd();
    clear_বাফার();
    ফেরত previous();
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
int32_t Normalizer::get_সূচক() ধ্রুবক {
    যদি(বাফার_Pos<বাফার.length()) {
        ফেরত current_সূচক;
    } অন্যথা {
        ফেরত পরবর্তী_সূচক;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::start_সূচক() ধ্রুবক {
    ফেরত text->start_সূচক();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::end_সূচক() ধ্রুবক {
    ফেরত text->end_সূচক();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

শূন্য
Normalizer::setMode(UNormalizationMode newMode) 
{
    fUMode = newMode;
    init();
}

UNormalizationMode
Normalizer::getUMode() ধ্রুবক
{
    ফেরত fUMode;
}

শূন্য
Normalizer::setOption(int32_t option, 
                      UBool value) 
{
    যদি (value) {
        fOptions |= option;
    } অন্যথা {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) ধ্রুবক
{
    ফেরত (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
শূন্য
Normalizer::setText(ধ্রুবক UnicodeString& newText, 
                    UErrorCode &status)
{
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    CharacterIterator *newIter = নতুন StringCharacterIterator(newText);
    যদি (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
শূন্য
Normalizer::setText(ধ্রুবক CharacterIterator& newText, 
                    UErrorCode &status) 
{
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    CharacterIterator *newIter = newText.clone();
    যদি (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    delete text;
    text = newIter;
    reset();
}

শূন্য
Normalizer::setText(ConstChar16Ptr newText,
                    int32_t length,
                    UErrorCode &status)
{
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    CharacterIterator *newIter = নতুন UCharCharacterIterator(newText, length);
    যদি (newIter == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    delete text;
    text = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
শূন্য
Normalizer::getText(UnicodeString&  ফলাফল) 
{
    text->getText(ফলাফল);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

শূন্য Normalizer::clear_বাফার() {
    বাফার.remove();
    বাফার_Pos=0;
}

UBool
Normalizer::পরবর্তী_Normalize() {
    clear_বাফার();
    current_সূচক=পরবর্তী_সূচক;
    text->set_সূচক(পরবর্তী_সূচক);
    যদি(!text->has_পরবর্তী()) {
        ফেরত মিথ্যা;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(text->next32PostInc());
    যখন(text->has_পরবর্তী()) {
        UChar32 c;
        যদি(fNorm2->hasBoundaryBefore(c=text->next32PostInc())) {
            text->move32(-1, CharacterIterator::kCurrent);
            বিরতি;
        }
        segment.append(c);
    }
    পরবর্তী_সূচক=text->get_সূচক();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, বাফার, errorCode);
    ফেরত U_SUCCESS(errorCode) && !বাফার.isEmpty();
}

UBool
Normalizer::previousNormalize() {
    clear_বাফার();
    পরবর্তী_সূচক=current_সূচক;
    text->set_সূচক(current_সূচক);
    যদি(!text->hasPrevious()) {
        ফেরত মিথ্যা;
    }
    UnicodeString segment;
    যখন(text->hasPrevious()) {
        UChar32 c=text->previous32();
        segment.insert(0, c);
        যদি(fNorm2->hasBoundaryBefore(c)) {
            বিরতি;
        }
    }
    current_সূচক=text->get_সূচক();
    UErrorCode errorCode=U_ZERO_ERROR;
    fNorm2->normalize(segment, বাফার, errorCode);
    বাফার_Pos=বাফার.length();
    ফেরত U_SUCCESS(errorCode) && !বাফার.isEmpty();
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */

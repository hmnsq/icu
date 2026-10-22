// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *************************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1996-2012, International Business Machines Corporation and
 * others. All Rights Reserved.
 *************************************************************************
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/chariter.h"
#అన్తర్భావ "unicode/schriter.h"
#అన్తర్భావ "unicode/uchriter.h"
#అన్తర్భావ "unicode/normlzr.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "normalizer2impl.h"
#అన్తర్భావ "uprops.h"  // for uniset_getUnicode32Instance()

#ఒకవేళ defined(move32)
 // System can define move32 intrinsics, but the char iters define move32 method
 // using same undef trick in headers, so undef here to re-enable the method.
#నిర్వచనం_రద్దు move32
#అయితే_ముగింపు

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Normalizer)

//-------------------------------------------------------------------------
// Constructors and other boilerplate
//-------------------------------------------------------------------------

Normalizer::Normalizer(స్థిరరాశి UnicodeString& str, UNormalization_బహుళకం బహుళకం) :
    UObject(), fFilteredNorm2(శూన్యసూచిక), fNorm2(శూన్యసూచిక), fUMode(బహుళకం), fOptions(0),
    పాఠ్యం(కొత్త StringCharacterIterator(str)),
    current_సూచీ(0), తదుపరి_సూచీ(0),
    బఫరు(), బఫరు_Pos(0)
{
    init();
}

Normalizer::Normalizer(ConstChar16Ptr str U_LIFETIME_BOUND, int32_t length, UNormalization_బహుళకం బహుళకం) :
    UObject(), fFilteredNorm2(శూన్యసూచిక), fNorm2(శూన్యసూచిక), fUMode(బహుళకం), fOptions(0),
    పాఠ్యం(కొత్త UCharCharacterIterator(str, length)),
    current_సూచీ(0), తదుపరి_సూచీ(0),
    బఫరు(), బఫరు_Pos(0)
{
    init();
}

Normalizer::Normalizer(స్థిరరాశి CharacterIterator& iter, UNormalization_బహుళకం బహుళకం) :
    UObject(), fFilteredNorm2(శూన్యసూచిక), fNorm2(శూన్యసూచిక), fUMode(బహుళకం), fOptions(0),
    పాఠ్యం(iter.clone()),
    current_సూచీ(0), తదుపరి_సూచీ(0),
    బఫరు(), బఫరు_Pos(0)
{
    init();
}

Normalizer::Normalizer(స్థిరరాశి Normalizer &copy) :
    UObject(copy), fFilteredNorm2(శూన్యసూచిక), fNorm2(శూన్యసూచిక), fUMode(copy.fUMode), fOptions(copy.fOptions),
    పాఠ్యం(copy.పాఠ్యం->clone()),
    current_సూచీ(copy.current_సూచీ), తదుపరి_సూచీ(copy.తదుపరి_సూచీ),
    బఫరు(copy.బఫరు), బఫరు_Pos(copy.బఫరు_Pos)
{
    init();
}

శూన్య
Normalizer::init() {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    fNorm2=Normalizer2Factory::getInstance(fUMode, error_సంకేతం);
    ఒకవేళ(fOptions&UNORM_UNICODE_3_2) {
        తొలగించు fFilteredNorm2;
        fNorm2=fFilteredNorm2=
            కొత్త FilteredNormalizer2(*fNorm2, *uniset_getUnicode32Instance(error_సంకేతం));
    }
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        error_సంకేతం=U_ZERO_ERROR;
        fNorm2=Normalizer2Factory::getNoopInstance(error_సంకేతం);
    }
}

Normalizer::~Normalizer()
{
    తొలగించు fFilteredNorm2;
    తొలగించు పాఠ్యం;
}

Normalizer* 
Normalizer::clone() స్థిరరాశి
{
    తిరిగి కొత్త Normalizer(*ఇది);
}

/**
 * Generates a hash code for this iterator.
 */
int32_t Normalizer::hash_సంకేతం() స్థిరరాశి
{
    తిరిగి పాఠ్యం->hash_సంకేతం() + fUMode + fOptions + బఫరు.hash_సంకేతం() + బఫరు_Pos + current_సూచీ + తదుపరి_సూచీ;
}
    
తార్కిక Normalizer::క్రియాకారకం==(స్థిరరాశి Normalizer& that) స్థిరరాశి
{
    తిరిగి
        ఇది==&that ||
        (fUMode==that.fUMode &&
        fOptions==that.fOptions &&
        *పాఠ్యం==*that.పాఠ్యం &&
        బఫరు==that.బఫరు &&
        బఫరు_Pos==that.బఫరు_Pos &&
        తదుపరి_సూచీ==that.తదుపరి_సూచీ);
}

//-------------------------------------------------------------------------
// Static utility methods
//-------------------------------------------------------------------------

శూన్య U_EXPORT2
Normalizer::normalize(స్థిరరాశి UnicodeString& ఆధారం, 
                      UNormalization_బహుళకం బహుళకం, int32_t options,
                      UnicodeString& ఫలితం, 
                      UError_సంకేతం &స్థితి_2) {
    ఒకవేళ(ఆధారం.isBogus() || U_FAILURE(స్థితి_2)) {
        ఫలితం.సమితి_గమ్యం_Bogus();
        ఒకవేళ(U_SUCCESS(స్థితి_2)) {
            స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        }
    } లేకపోతే {
        UnicodeString localDest;
        UnicodeString *dest;

        ఒకవేళ(&ఆధారం!=&ఫలితం) {
            dest=&ఫలితం;
        } లేకపోతే {
            // the source and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, స్థితి_2);
        ఒకవేళ(U_SUCCESS(స్థితి_2)) {
            ఒకవేళ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(స్థితి_2)).
                    normalize(ఆధారం, *dest, స్థితి_2);
            } లేకపోతే {
                n2->normalize(ఆధారం, *dest, స్థితి_2);
            }
        }
        ఒకవేళ(dest==&localDest && U_SUCCESS(స్థితి_2)) {
            ఫలితం=*dest;
        }
    }
}

శూన్య U_EXPORT2
Normalizer::compose(స్థిరరాశి UnicodeString& ఆధారం, 
                    UBool compat, int32_t options,
                    UnicodeString& ఫలితం, 
                    UError_సంకేతం &స్థితి_2) {
    normalize(ఆధారం, compat ? UNORM_NFKC : UNORM_NFC, options, ఫలితం, స్థితి_2);
}

శూన్య U_EXPORT2
Normalizer::decompose(స్థిరరాశి UnicodeString& ఆధారం, 
                      UBool compat, int32_t options,
                      UnicodeString& ఫలితం, 
                      UError_సంకేతం &స్థితి_2) {
    normalize(ఆధారం, compat ? UNORM_NFKD : UNORM_NFD, options, ఫలితం, స్థితి_2);
}

UNormalization_Check_ఫలితం
Normalizer::శీఘ్ర_Check(స్థిరరాశి UnicodeString& ఆధారం,
                       UNormalization_బహుళకం బహుళకం, int32_t options,
                       UError_సంకేతం &స్థితి_2) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, స్థితి_2);
    ఒకవేళ(U_SUCCESS(స్థితి_2)) {
        ఒకవేళ(options&UNORM_UNICODE_3_2) {
            తిరిగి FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(స్థితి_2)).
                శీఘ్ర_Check(ఆధారం, స్థితి_2);
        } లేకపోతే {
            తిరిగి n2->శీఘ్ర_Check(ఆధారం, స్థితి_2);
        }
    } లేకపోతే {
        తిరిగి UNORM_MAYBE;
    }
}

UBool
Normalizer::isNormalized(స్థిరరాశి UnicodeString& ఆధారం,
                         UNormalization_బహుళకం బహుళకం, int32_t options,
                         UError_సంకేతం &స్థితి_2) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, స్థితి_2);
    ఒకవేళ(U_SUCCESS(స్థితి_2)) {
        ఒకవేళ(options&UNORM_UNICODE_3_2) {
            తిరిగి FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(స్థితి_2)).
                isNormalized(ఆధారం, స్థితి_2);
        } లేకపోతే {
            తిరిగి n2->isNormalized(ఆధారం, స్థితి_2);
        }
    } లేకపోతే {
        తిరిగి అబద్ధం;
    }
}

UnicodeString& U_EXPORT2
Normalizer::concatenate(స్థిరరాశి UnicodeString& ఎడమ,
                        స్థిరరాశి UnicodeString& కుడి,
                        UnicodeString& ఫలితం U_LIFETIME_BOUND,
                        UNormalization_బహుళకం బహుళకం,
                        int32_t options,
                        UError_సంకేతం& error_సంకేతం) {
    ఒకవేళ(ఎడమ.isBogus() || కుడి.isBogus() || U_FAILURE(error_సంకేతం)) {
        ఫలితం.సమితి_గమ్యం_Bogus();
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        }
    } లేకపోతే {
        UnicodeString localDest;
        UnicodeString *dest;

        ఒకవేళ(&కుడి!=&ఫలితం) {
            dest=&ఫలితం;
        } లేకపోతే {
            // the right and result strings are the same object, use a temporary one
            dest=&localDest;
        }
        *dest=ఎడమ;
        స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            ఒకవేళ(options&UNORM_UNICODE_3_2) {
                FilteredNormalizer2(*n2, *uniset_getUnicode32Instance(error_సంకేతం)).
                    append(*dest, కుడి, error_సంకేతం);
            } లేకపోతే {
                n2->append(*dest, కుడి, error_సంకేతం);
            }
        }
        ఒకవేళ(dest==&localDest && U_SUCCESS(error_సంకేతం)) {
            ఫలితం=*dest;
        }
    }
    తిరిగి ఫలితం;
}

//-------------------------------------------------------------------------
// Iteration API
//-------------------------------------------------------------------------

/**
 * Return the current character in the normalized text.
 */
UChar32 Normalizer::current() {
    ఒకవేళ(బఫరు_Pos<బఫరు.length() || తదుపరి_Normalize()) {
        తిరిగి బఫరు.char32At(బఫరు_Pos);
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

/**
 * Return the next character in the normalized text and advance
 * the iteration position by one.  If the end
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::next() {
    ఒకవేళ(బఫరు_Pos<బఫరు.length() ||  తదుపరి_Normalize()) {
        UChar32 c=బఫరు.char32At(బఫరు_Pos);
        బఫరు_Pos+=U16_నిడివి(c);
        తిరిగి c;
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

/**
 * Return the previous character in the normalized text and decrement
 * the iteration position by one.  If the beginning
 * of the text has already been reached, {@link #DONE} is returned.
 */
UChar32 Normalizer::మునుపటి() {
    ఒకవేళ(బఫరు_Pos>0 || మునుపటి_Normalize()) {
        UChar32 c=బఫరు.char32At(బఫరు_Pos-1);
        బఫరు_Pos-=U16_నిడివి(c);
        తిరిగి c;
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

శూన్య Normalizer::reset() {
    current_సూచీ=తదుపరి_సూచీ=పాఠ్యం->సమితి_గమ్యం_ప్రారంభం();
    clear_బఫరు();
}

శూన్య
Normalizer::సమితి_సూచీ_Only(int32_t సూచీ) {
    పాఠ్యం->సమితి_సూచీ(సూచీ);  // pins index
    current_సూచీ=తదుపరి_సూచీ=పాఠ్యం->get_సూచీ();
    clear_బఫరు();
}

/**
 * Return the first character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to the beginning of the text.
 */
UChar32 Normalizer::first() {
    reset();
    తిరిగి next();
}

/**
 * Return the last character in the normalized text.  This resets
 * the <tt>Normalizer's</tt> position to be just before the
 * the input text corresponding to that normalized character.
 */
UChar32 Normalizer::చివరి() {
    current_సూచీ=తదుపరి_సూచీ=పాఠ్యం->సమితి_గమ్యం_చివర();
    clear_బఫరు();
    తిరిగి మునుపటి();
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
int32_t Normalizer::get_సూచీ() స్థిరరాశి {
    ఒకవేళ(బఫరు_Pos<బఫరు.length()) {
        తిరిగి current_సూచీ;
    } లేకపోతే {
        తిరిగి తదుపరి_సూచీ;
    }
}

/**
 * Retrieve the index of the start of the input text.  This is the begin index
 * of the <tt>CharacterIterator</tt> or the start (i.e. 0) of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::ప్రారంభం_సూచీ() స్థిరరాశి {
    తిరిగి పాఠ్యం->ప్రారంభం_సూచీ();
}

/**
 * Retrieve the index of the end of the input text.  This is the end index
 * of the <tt>CharacterIterator</tt> or the length of the <tt>String</tt>
 * over which this <tt>Normalizer</tt> is iterating
 */
int32_t Normalizer::చివర_సూచీ() స్థిరరాశి {
    తిరిగి పాఠ్యం->చివర_సూచీ();
}

//-------------------------------------------------------------------------
// Property access methods
//-------------------------------------------------------------------------

శూన్య
Normalizer::సమితి_బహుళకం(UNormalization_బహుళకం new_బహుళకం) 
{
    fUMode = new_బహుళకం;
    init();
}

UNormalization_బహుళకం
Normalizer::getUMode() స్థిరరాశి
{
    తిరిగి fUMode;
}

శూన్య
Normalizer::సమితి_Option(int32_t option, 
                      UBool value) 
{
    ఒకవేళ (value) {
        fOptions |= option;
    } లేకపోతే {
        fOptions &= (~option);
    }
    init();
}

UBool
Normalizer::getOption(int32_t option) స్థిరరాశి
{
    తిరిగి (fOptions & option) != 0;
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the input text.
 */
శూన్య
Normalizer::సమితి_పాఠ్యం(స్థిరరాశి UnicodeString& new_పాఠ్యం, 
                    UError_సంకేతం &స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    CharacterIterator *newIter = కొత్త StringCharacterIterator(new_పాఠ్యం);
    ఒకవేళ (newIter == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    తొలగించు పాఠ్యం;
    పాఠ్యం = newIter;
    reset();
}

/**
 * Set the input text over which this <tt>Normalizer</tt> will iterate.
 * The iteration position is set to the beginning of the string.
 */
శూన్య
Normalizer::సమితి_పాఠ్యం(స్థిరరాశి CharacterIterator& new_పాఠ్యం, 
                    UError_సంకేతం &స్థితి_2) 
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    CharacterIterator *newIter = new_పాఠ్యం.clone();
    ఒకవేళ (newIter == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    తొలగించు పాఠ్యం;
    పాఠ్యం = newIter;
    reset();
}

శూన్య
Normalizer::సమితి_పాఠ్యం(ConstChar16Ptr new_పాఠ్యం,
                    int32_t length,
                    UError_సంకేతం &స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    CharacterIterator *newIter = కొత్త UCharCharacterIterator(new_పాఠ్యం, length);
    ఒకవేళ (newIter == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    తొలగించు పాఠ్యం;
    పాఠ్యం = newIter;
    reset();
}

/**
 * Copies the text under iteration into the UnicodeString referred to by "result".
 * @param result Receives a copy of the text under iteration.
 */
శూన్య
Normalizer::get_పాఠ్యం(UnicodeString&  ఫలితం) 
{
    పాఠ్యం->get_పాఠ్యం(ఫలితం);
}

//-------------------------------------------------------------------------
// Private utility methods
//-------------------------------------------------------------------------

శూన్య Normalizer::clear_బఫరు() {
    బఫరు.remove();
    బఫరు_Pos=0;
}

UBool
Normalizer::తదుపరి_Normalize() {
    clear_బఫరు();
    current_సూచీ=తదుపరి_సూచీ;
    పాఠ్యం->సమితి_సూచీ(తదుపరి_సూచీ);
    ఒకవేళ(!పాఠ్యం->has_తదుపరి()) {
        తిరిగి అబద్ధం;
    }
    // Skip at least one character so we make progress.
    UnicodeString segment(పాఠ్యం->next32PostInc());
    అయితే(పాఠ్యం->has_తదుపరి()) {
        UChar32 c;
        ఒకవేళ(fNorm2->hasBoundaryBefore(c=పాఠ్యం->next32PostInc())) {
            పాఠ్యం->move32(-1, CharacterIterator::kCurrent);
            ఆపు;
        }
        segment.append(c);
    }
    తదుపరి_సూచీ=పాఠ్యం->get_సూచీ();
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    fNorm2->normalize(segment, బఫరు, error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం) && !బఫరు.is_ఖాళీలు();
}

UBool
Normalizer::మునుపటి_Normalize() {
    clear_బఫరు();
    తదుపరి_సూచీ=current_సూచీ;
    పాఠ్యం->సమితి_సూచీ(current_సూచీ);
    ఒకవేళ(!పాఠ్యం->has_మునుపటి()) {
        తిరిగి అబద్ధం;
    }
    UnicodeString segment;
    అయితే(పాఠ్యం->has_మునుపటి()) {
        UChar32 c=పాఠ్యం->previous32();
        segment.insert(0, c);
        ఒకవేళ(fNorm2->hasBoundaryBefore(c)) {
            ఆపు;
        }
    }
    current_సూచీ=పాఠ్యం->get_సూచీ();
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    fNorm2->normalize(segment, బఫరు, error_సంకేతం);
    బఫరు_Pos=బఫరు.length();
    తిరిగి U_SUCCESS(error_సంకేతం) && !బఫరు.is_ఖాళీలు();
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_NORMALIZATION */

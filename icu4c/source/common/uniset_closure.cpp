// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uniset_closure.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011may30
*   created by: Markus W. Scherer
*
*   UnicodeSet::closeOver() and related methods moved here from uniset_props.cpp
*   to simplify dependencies.
*   In particular, this depends on the BreakIterator, but the BreakIterator
*   code also builds UnicodeSets from patterns and needs uniset_props.
*/

#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/parsepos.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ruleiter.h"
#అన్తర్భావ "ucase.h"
#అన్తర్భావ "uprops.h"
#అన్తర్భావ "util.h"
#అన్తర్భావ "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#నిర్వచించు _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_సమితి::Unicode_సమితి(స్థిరరాశి UnicodeString& నమూనా,
                       uint32_t options,
                       స్థిరరాశి చిహ్నం_పట్టిక* symbols,
                       UError_సంకేతం& స్థితి_2) {
    apply_నమూనా(నమూనా, options, symbols, స్థితి_2);
    _dbgct(ఇది);
}

Unicode_సమితి::Unicode_సమితి(స్థిరరాశి UnicodeString& నమూనా, ParsePosition& pos,
                       uint32_t options,
                       స్థిరరాశి చిహ్నం_పట్టిక* symbols,
                       UError_సంకేతం& స్థితి_2) {
    apply_నమూనా(నమూనా, pos, options, symbols, స్థితి_2);
    _dbgct(ఇది);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_సమితి& Unicode_సమితి::apply_నమూనా(స్థిరరాశి UnicodeString& నమూనా,
                                     uint32_t options,
                                     స్థిరరాశి చిహ్నం_పట్టిక* symbols,
                                     UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_నమూనా(నమూనా, pos, options, symbols, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి *ఇది;

    int32_t i = pos.get_సూచీ();

    ఒకవేళ (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(నమూనా, i, నిజం);
    }

    ఒకవేళ (i != నమూనా.length()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }
    తిరిగి *ఇది;
}

Unicode_సమితి& Unicode_సమితి::apply_నమూనా(స్థిరరాశి UnicodeString& నమూనా,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     స్థిరరాశి చిహ్నం_పట్టిక* symbols,
                                     UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి *ఇది;
    }
    ఒకవేళ (isFrozen()) {
        స్థితి_2 = U_NO_WRITE_PERMISSION;
        తిరిగి *ఇది;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    నియమం_Character_Iterator chars(నమూనా, symbols, pos);
    apply_నమూనా(నమూనా, pos, chars, symbols, rebuiltPat, options, &Unicode_సమితి::closeOver, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి *ఇది;
    ఒకవేళ (chars.లోపల_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        స్థితి_2 = U_MALFORMED_సమితి;
        తిరిగి *ఇది;
    }
    సమితి_నమూనా(rebuiltPat);
    తిరిగి *ఇది;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
స్థిర శూన్య U_CALLCONV
సమితి_కూడు(USet *సమితి, UChar32 c) {
    reinterpret_cast<Unicode_సమితి*>(సమితి)->కూడు(c);
}

స్థిర శూన్య U_CALLCONV
సమితి_కూడు_Range(USet *సమితి, UChar32 ప్రారంభం, UChar32 చివర) {
    reinterpret_cast<Unicode_సమితి*>(సమితి)->కూడు(ప్రారంభం, చివర);
}

స్థిర శూన్య U_CALLCONV
సమితి_కూడు_String(USet *సమితి, స్థిరరాశి char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_సమితి*>(సమితి)->కూడు(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
స్థిర అంతఃవరుస శూన్య
కూడు_Case_Mapping(Unicode_సమితి &సమితి, int32_t ఫలితం, స్థిరరాశి char16_t *నిండిన, UnicodeString &str) {
    ఒకవేళ(ఫలితం >= 0) {
        ఒకవేళ(ఫలితం > UCASE_MAX_STRING_నిడివి) {
            // add a single-code point case mapping
            సమితి.కూడు(ఫలితం);
        } లేకపోతే {
            // add a string case mapping from full with length result
            str.సమితి_గమ్యం(static_cast<UBool>(అబద్ధం), నిండిన, ఫలితం);
            సమితి.కూడు(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

నామాకాశ {

/** For case closure on a large set, look only at code points with relevant properties. */
స్థిరరాశి Unicode_సమితి &maybeOnlyCaseSensitive(స్థిరరాశి Unicode_సమితి &src U_LIFETIME_BOUND,
                                         Unicode_సమితి &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.కలిగియుంది(0, 0x10ffff));
    ఒకవేళ (src.size() < 30) {
        తిరిగి src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    స్థిరరాశి Unicode_సమితి *sensitive =
        CharacterProperties::get_ద్విమాన_Property_సమితి(UCHAR_CASE_SENSITIVE, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    ఒకవేళ (src.get_Range_లెక్క() > sensitive->get_Range_లెక్క()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } లేకపోతే {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    తిరిగి subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
తార్కిక scfString(స్థిరరాశి UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    స్థిరరాశి char16_t *p = s.get_బఫరు();
    int32_t length = s.length();
    // Loop while not needing modification.
    కొరకు (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_తదుపరి(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        ఒకవేళ (scfChar != c) {
            // Copy the characters before c.
            scf.సమితి_గమ్యం(p, i - U16_నిడివి(c));
            // Loop over the rest of the string and keep case-folding.
            కొరకు (;;) {
                scf.append(scfChar);
                ఒకవేళ (i == length) {
                    తిరిగి నిజం;
                }
                U16_తదుపరి(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    తిరిగి అబద్ధం;
}

}  // namespace

Unicode_సమితి& Unicode_సమితి::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి *ఇది;
    }
    ఎంపిక (attribute & USET_CASE_ముసుగు) {
    స్థితి 0:
        ఆపు;
    స్థితి USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ అబద్ధం);
        ఆపు;
    స్థితి USET_కూడు_CASE_MAPPINGS:
        close_Over_కూడు_Case_Mappings();
        ఆపు;
    స్థితి USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ నిజం);
        ఆపు;
    అప్రమేయ:
        // bad option (unreachable)
        ఆపు;
    }
    తిరిగి *ఇది;
}

శూన్య Unicode_సమితి::closeOverCaseInsensitive(తార్కిక simple) {
    // Start with input set to guarantee inclusion.
    Unicode_సమితి fold_సమితి(*ఇది);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    ఒకవేళ (!simple && fold_సమితి.has_అక్షరశ్రేణులు()) {
        fold_సమితి.అక్షరశ్రేణులు_2->removeAllElements();
    }

    USetAdder sa = {
        fold_సమితి.గమ్యం_USet(),
        సమితి_కూడు,
        సమితి_కూడు_Range,
        సమితి_కూడు_String,
        శూన్యసూచిక, // don't need remove()
        శూన్యసూచిక // don't need removeRange()
    };

    Unicode_సమితి subset(0, 0x10ffff);
    స్థిరరాశి Unicode_సమితి &సంకేతం_Points = maybeOnlyCaseSensitive(*ఇది, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = సంకేతం_Points.get_Range_లెక్క();

    కొరకు (int32_t i=0; i<n; ++i) {
        UChar32 ప్రారంభం = సంకేతం_Points.get_Range_ప్రారంభం(i);
        UChar32 చివర   = సంకేతం_Points.get_Range_చివర(i);

        ఒకవేళ (simple) {
            కొరకు (UChar32 cp=ప్రారంభం; cp<=చివర; ++cp) {
                ucase_కూడు_Simple_Case_సంవృతం(cp, &sa);
            }
        } లేకపోతే {
            కొరకు (UChar32 cp=ప్రారంభం; cp<=చివర; ++cp) {
                ucase_కూడు_Case_సంవృతం(cp, &sa);
            }
        }
    }
    ఒకవేళ (has_అక్షరశ్రేణులు()) {
        UnicodeString str;
        కొరకు (int32_t j=0; j<అక్షరశ్రేణులు_2->size(); ++j) {
            స్థిరరాశి UnicodeString* pStr = static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు_2->elementAt(j));
            ఒకవేళ (simple) {
                ఒకవేళ (scfString(*pStr, str)) {
                    fold_సమితి.remove(*pStr).కూడు(str);
                }
            } లేకపోతే {
                str = *pStr;
                str.foldCase();
                ఒకవేళ(!ucase_కూడు_String_Case_సంవృతం(str.get_బఫరు(), str.length(), &sa)) {
                    fold_సమితి.కూడు(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *ఇది = fold_సమితి;
}

శూన్య Unicode_సమితి::close_Over_కూడు_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_సమితి fold_సమితి(*ఇది);

    Unicode_సమితి subset(0, 0x10ffff);
    స్థిరరాశి Unicode_సమితి &సంకేతం_Points = maybeOnlyCaseSensitive(*ఇది, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = సంకేతం_Points.get_Range_లెక్క();
    UChar32 ఫలితం;
    స్థిరరాశి char16_t *నిండిన;
    UnicodeString str;

    కొరకు (int32_t i=0; i<n; ++i) {
        UChar32 ప్రారంభం = సంకేతం_Points.get_Range_ప్రారంభం(i);
        UChar32 చివర   = సంకేతం_Points.get_Range_చివర(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        కొరకు (UChar32 cp=ప్రారంభం; cp<=చివర; ++cp) {
            ఫలితం = ucase_గమ్యం_నిండిన_క్రింది(cp, శూన్యసూచిక, శూన్యసూచిక, &నిండిన, UCASE_LOC_మూలం);
            కూడు_Case_Mapping(fold_సమితి, ఫలితం, నిండిన, str);

            ఫలితం = ucase_గమ్యం_నిండిన_Title(cp, శూన్యసూచిక, శూన్యసూచిక, &నిండిన, UCASE_LOC_మూలం);
            కూడు_Case_Mapping(fold_సమితి, ఫలితం, నిండిన, str);

            ఫలితం = ucase_గమ్యం_నిండిన_పై(cp, శూన్యసూచిక, శూన్యసూచిక, &నిండిన, UCASE_LOC_మూలం);
            కూడు_Case_Mapping(fold_సమితి, ఫలితం, నిండిన, str);

            ఫలితం = ucase_గమ్యం_నిండిన_Folding(cp, &నిండిన, 0);
            కూడు_Case_Mapping(fold_సమితి, ఫలితం, నిండిన, str);
        }
    }
    ఒకవేళ (has_అక్షరశ్రేణులు()) {
        Locale మూలం("");
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::create_మాట_Instance(మూలం, స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
#అయితే_ముగింపు
            కొరకు (int32_t j=0; j<అక్షరశ్రేణులు_2->size(); ++j) {
                స్థిరరాశి UnicodeString* pStr = static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు_2->elementAt(j));
                (str = *pStr).గమ్యం_క్రింది(మూలం);
                fold_సమితి.కూడు(str);
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).గమ్యం_Title(bi, మూలం);
                fold_సమితి.కూడు(str);
#అయితే_ముగింపు
                (str = *pStr).గమ్యం_పై(మూలం);
                fold_సమితి.కూడు(str);
                (str = *pStr).foldCase();
                fold_సమితి.కూడు(str);
            }
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
        }
        తొలగించు bi;
#అయితే_ముగింపు
    }
    *ఇది = fold_సమితి;
}

U_NAMESPACE_చివర

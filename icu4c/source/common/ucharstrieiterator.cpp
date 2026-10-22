// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrieiterator.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov15
*   created by: Markus W. Scherer
*/

#समावेश "unicode/utypes.h"
#समावेश "unicode/ucharstrie.h"
#समावेश "unicode/unistr.h"
#समावेश "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_लंबाई,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remaining_Match_लंबाई(-1), initial_Remaining_Match_लंबाई(-1),
          skip_मूल्य_2(false),
          max_लंबाई(max_String_लंबाई), मूल्य(0), ढेर(nullptr) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ढेर=नया UVector32(errorCode);
    अगर(ढेर==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(स्थिर UCharsTrie &trie, int32_t max_String_लंबाई,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remaining_Match_लंबाई(trie.remaining_Match_लंबाई),
          initial_Remaining_Match_लंबाई(trie.remaining_Match_लंबाई),
          skip_मूल्य_2(false),
          max_लंबाई(max_String_लंबाई), मूल्य(0), ढेर(nullptr) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    ढेर=नया UVector32(errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    अगर(ढेर==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    int32_t length=remaining_Match_लंबाई;  // Actual remaining match length minus 1.
    अगर(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        अगर(max_लंबाई>0 && length>max_लंबाई) {
            length=max_लंबाई;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remaining_Match_लंबाई-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    मिटाओ ढेर;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remaining_Match_लंबाई=initial_Remaining_Match_लंबाई;
    skip_मूल्य_2=false;
    int32_t length=remaining_Match_लंबाई+1;  // Remaining match length.
    अगर(max_लंबाई>0 && length>max_लंबाई) {
        length=max_लंबाई;
    }
    str_.truncate(length);
    pos_+=length;
    remaining_Match_लंबाई-=length;
    ढेर->set_आकार(0);
    वापस *यह;
}

UBool
UCharsTrie::Iterator::has_अग्र() स्थिर { वापस pos_!=nullptr || !ढेर->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    स्थिर char16_t *pos=pos_;
    अगर(pos==nullptr) {
        अगर(ढेर->isEmpty()) {
            वापस false;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ढेर_आकार=ढेर->size();
        int32_t length=ढेर->elementAti(ढेर_आकार-1);
        pos=uchars_+ढेर->elementAti(ढेर_आकार-2);
        ढेर->set_आकार(ढेर_आकार-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        अगर(length>1) {
            pos=branch_अग्र(pos, length, errorCode);
            अगर(pos==nullptr) {
                वापस true;  // Reached a final value.
            }
        } अन्यथा {
            str_.append(*pos++);
        }
    }
    अगर(remaining_Match_लंबाई>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        वापस truncateAndStop();
    }
    क्रम(;;) {
        int32_t गाँठ=*pos++;
        अगर(गाँठ>=k_Min_मूल्य_Lead) {
            अगर(skip_मूल्य_2) {
                pos=skip_गाँठ_मूल्य(pos, गाँठ);
                गाँठ&=k_गाँठ_Type_Mask;
                skip_मूल्य_2=false;
            } अन्यथा {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(गाँठ >> 15);
                अगर(isFinal) {
                    मूल्य=read_मूल्य(pos, गाँठ&0x7fff);
                } अन्यथा {
                    मूल्य=read_गाँठ_मूल्य(pos, गाँठ);
                }
                अगर(isFinal || (max_लंबाई>0 && str_.length()==max_लंबाई)) {
                    pos_=nullptr;
                } अन्यथा {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_मूल्य_2=true;
                }
                वापस true;
            }
        }
        अगर(max_लंबाई>0 && str_.length()==max_लंबाई) {
            वापस truncateAndStop();
        }
        अगर(गाँठ<kMinLinearMatch) {
            अगर(गाँठ==0) {
                गाँठ=*pos++;
            }
            pos=branch_अग्र(pos, गाँठ+1, errorCode);
            अगर(pos==nullptr) {
                वापस true;  // Reached a final value.
            }
        } अन्यथा {
            // Linear-match node, append length units to str_.
            int32_t length=गाँठ-kMinLinearMatch+1;
            अगर(max_लंबाई>0 && str_.length()+length>max_लंबाई) {
                str_.append(pos, max_लंबाई-str_.length());
                वापस truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
स्थिर char16_t*
UCharsTrie::Iterator::branch_अग्र(स्थिर char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    जबतक(length>k_Max_Branch_Linear_Sub_गाँठ_लंबाई) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ढेर->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        ढेर->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t गाँठ=*pos++;
    UBool isFinal = static_cast<UBool>(गाँठ >> 15);
    int32_t value=read_मूल्य(pos, गाँठ&=0x7fff);
    pos=skip_मूल्य(pos, गाँठ);
    ढेर->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    ढेर->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    अगर(isFinal) {
        pos_=nullptr;
        मूल्य=value;
        वापस nullptr;
    } अन्यथा {
        वापस pos+value;
    }
}

U_NAMESPACE_END

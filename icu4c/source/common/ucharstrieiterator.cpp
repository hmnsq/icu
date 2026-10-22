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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ucharstrie.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_দৈর্ঘ্য,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remaining_Match_দৈর্ঘ্য(-1), initial_Remaining_Match_দৈর্ঘ্য(-1),
          skip_মান_2(মিথ্যা),
          max_দৈর্ঘ্য(max_String_দৈর্ঘ্য), মান(0), স্তূপ(nullptr) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    স্তূপ=নতুন UVector32(errorCode);
    যদি(স্তূপ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ধ্রুবক UCharsTrie &trie, int32_t max_String_দৈর্ঘ্য,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remaining_Match_দৈর্ঘ্য(trie.remaining_Match_দৈর্ঘ্য),
          initial_Remaining_Match_দৈর্ঘ্য(trie.remaining_Match_দৈর্ঘ্য),
          skip_মান_2(মিথ্যা),
          max_দৈর্ঘ্য(max_String_দৈর্ঘ্য), মান(0), স্তূপ(nullptr) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    স্তূপ=নতুন UVector32(errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি(স্তূপ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    int32_t length=remaining_Match_দৈর্ঘ্য;  // Actual remaining match length minus 1.
    যদি(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        যদি(max_দৈর্ঘ্য>0 && length>max_দৈর্ঘ্য) {
            length=max_দৈর্ঘ্য;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remaining_Match_দৈর্ঘ্য-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete স্তূপ;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remaining_Match_দৈর্ঘ্য=initial_Remaining_Match_দৈর্ঘ্য;
    skip_মান_2=মিথ্যা;
    int32_t length=remaining_Match_দৈর্ঘ্য+1;  // Remaining match length.
    যদি(max_দৈর্ঘ্য>0 && length>max_দৈর্ঘ্য) {
        length=max_দৈর্ঘ্য;
    }
    str_.truncate(length);
    pos_+=length;
    remaining_Match_দৈর্ঘ্য-=length;
    স্তূপ->set_আকার(0);
    ফেরত *এই;
}

UBool
UCharsTrie::Iterator::has_পরবর্তী() ধ্রুবক { ফেরত pos_!=nullptr || !স্তূপ->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক char16_t *pos=pos_;
    যদি(pos==nullptr) {
        যদি(স্তূপ->isEmpty()) {
            ফেরত মিথ্যা;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t স্তূপ_আকার=স্তূপ->size();
        int32_t length=স্তূপ->elementAti(স্তূপ_আকার-1);
        pos=uchars_+স্তূপ->elementAti(স্তূপ_আকার-2);
        স্তূপ->set_আকার(স্তূপ_আকার-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        যদি(length>1) {
            pos=branch_পরবর্তী(pos, length, errorCode);
            যদি(pos==nullptr) {
                ফেরত সত্য;  // Reached a final value.
            }
        } অন্যথা {
            str_.append(*pos++);
        }
    }
    যদি(remaining_Match_দৈর্ঘ্য>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ফেরত truncateAndStop();
    }
    জন্য(;;) {
        int32_t গ্রন্থি=*pos++;
        যদি(গ্রন্থি>=k_Min_মান_Lead) {
            যদি(skip_মান_2) {
                pos=skip_গ্রন্থি_মান(pos, গ্রন্থি);
                গ্রন্থি&=k_গ্রন্থি_Type_Mask;
                skip_মান_2=মিথ্যা;
            } অন্যথা {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(গ্রন্থি >> 15);
                যদি(isFinal) {
                    মান=read_মান(pos, গ্রন্থি&0x7fff);
                } অন্যথা {
                    মান=read_গ্রন্থি_মান(pos, গ্রন্থি);
                }
                যদি(isFinal || (max_দৈর্ঘ্য>0 && str_.length()==max_দৈর্ঘ্য)) {
                    pos_=nullptr;
                } অন্যথা {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_মান_2=সত্য;
                }
                ফেরত সত্য;
            }
        }
        যদি(max_দৈর্ঘ্য>0 && str_.length()==max_দৈর্ঘ্য) {
            ফেরত truncateAndStop();
        }
        যদি(গ্রন্থি<kMinLinearMatch) {
            যদি(গ্রন্থি==0) {
                গ্রন্থি=*pos++;
            }
            pos=branch_পরবর্তী(pos, গ্রন্থি+1, errorCode);
            যদি(pos==nullptr) {
                ফেরত সত্য;  // Reached a final value.
            }
        } অন্যথা {
            // Linear-match node, append length units to str_.
            int32_t length=গ্রন্থি-kMinLinearMatch+1;
            যদি(max_দৈর্ঘ্য>0 && str_.length()+length>max_দৈর্ঘ্য) {
                str_.append(pos, max_দৈর্ঘ্য-str_.length());
                ফেরত truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ধ্রুবক char16_t*
UCharsTrie::Iterator::branch_পরবর্তী(ধ্রুবক char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    যখন(length>k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        স্তূপ->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        স্তূপ->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t গ্রন্থি=*pos++;
    UBool isFinal = static_cast<UBool>(গ্রন্থি >> 15);
    int32_t value=read_মান(pos, গ্রন্থি&=0x7fff);
    pos=skip_মান(pos, গ্রন্থি);
    স্তূপ->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    স্তূপ->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    যদি(isFinal) {
        pos_=nullptr;
        মান=value;
        ফেরত nullptr;
    } অন্যথা {
        ফেরত pos+value;
    }
}

U_NAMESPACE_END

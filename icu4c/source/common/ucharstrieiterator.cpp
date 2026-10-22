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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/ucharstrie.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_মান_2(মিছা),
          maxLength_(maxStringLength), মান(0), স্তূপ(nullptr) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    স্তূপ=নূতন UVector32(errorCode);
    যদি(স্তূপ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ধ্রুৱ UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_মান_2(মিছা),
          maxLength_(maxStringLength), মান(0), স্তূপ(nullptr) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    স্তূপ=নূতন UVector32(errorCode);
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    যদি(স্তূপ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    যদি(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        যদি(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete স্তূপ;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_মান_2=মিছা;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    যদি(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    স্তূপ->setSize(0);
    ঘূৰাই *এতত্;
}

UBool
UCharsTrie::Iterator::hasNext() ধ্রুৱ { ঘূৰাই pos_!=nullptr || !স্তূপ->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ char16_t *pos=pos_;
    যদি(pos==nullptr) {
        যদি(স্তূপ->isEmpty()) {
            ঘূৰাই মিছা;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t স্তূপ_Size=স্তূপ->size();
        int32_t length=স্তূপ->elementAti(স্তূপ_Size-1);
        pos=uchars_+স্তূপ->elementAti(স্তূপ_Size-2);
        স্তূপ->setSize(স্তূপ_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        যদি(length>1) {
            pos=branchNext(pos, length, errorCode);
            যদি(pos==nullptr) {
                ঘূৰাই সঁচা;  // Reached a final value.
            }
        } অন্যথা {
            str_.append(*pos++);
        }
    }
    যদি(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ঘূৰাই truncateAndStop();
    }
    বাবে(;;) {
        int32_t গ্ৰন্থি=*pos++;
        যদি(গ্ৰন্থি>=k_Min_মান_Lead) {
            যদি(skip_মান_2) {
                pos=skip_গ্ৰন্থি_মান(pos, গ্ৰন্থি);
                গ্ৰন্থি&=k_গ্ৰন্থি_Type_Mask;
                skip_মান_2=মিছা;
            } অন্যথা {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(গ্ৰন্থি >> 15);
                যদি(isFinal) {
                    মান=read_মান(pos, গ্ৰন্থি&0x7fff);
                } অন্যথা {
                    মান=read_গ্ৰন্থি_মান(pos, গ্ৰন্থি);
                }
                যদি(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } অন্যথা {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_মান_2=সঁচা;
                }
                ঘূৰাই সঁচা;
            }
        }
        যদি(maxLength_>0 && str_.length()==maxLength_) {
            ঘূৰাই truncateAndStop();
        }
        যদি(গ্ৰন্থি<kMinLinearMatch) {
            যদি(গ্ৰন্থি==0) {
                গ্ৰন্থি=*pos++;
            }
            pos=branchNext(pos, গ্ৰন্থি+1, errorCode);
            যদি(pos==nullptr) {
                ঘূৰাই সঁচা;  // Reached a final value.
            }
        } অন্যথা {
            // Linear-match node, append length units to str_.
            int32_t length=গ্ৰন্থি-kMinLinearMatch+1;
            যদি(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                ঘূৰাই truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ধ্রুৱ char16_t*
UCharsTrie::Iterator::branchNext(ধ্রুৱ char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    যেতিয়ালৈ(length>k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length) {
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
    int32_t গ্ৰন্থি=*pos++;
    UBool isFinal = static_cast<UBool>(গ্ৰন্থি >> 15);
    int32_t value=read_মান(pos, গ্ৰন্থি&=0x7fff);
    pos=skip_মান(pos, গ্ৰন্থি);
    স্তূপ->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    স্তূপ->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    যদি(isFinal) {
        pos_=nullptr;
        মান=value;
        ঘূৰাই nullptr;
    } অন্যথা {
        ঘূৰাই pos+value;
    }
}

U_NAMESPACE_END

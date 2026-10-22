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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_मान_2(असत्य),
          maxLength_(maxStringLength), मान(0), थाक(nullptr) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    थाक=नूतन UVector32(errorCode);
    यदि(थाक==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(अचर UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_मान_2(असत्य),
          maxLength_(maxStringLength), मान(0), थाक(nullptr) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    थाक=नूतन UVector32(errorCode);
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    यदि(थाक==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        यदि(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete थाक;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_मान_2=असत्य;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    यदि(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    थाक->setSize(0);
    फर्काऊ *एतत्;
}

UBool
UCharsTrie::Iterator::hasNext() अचर { फर्काऊ pos_!=nullptr || !थाक->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ असत्य;
    }
    अचर char16_t *pos=pos_;
    यदि(pos==nullptr) {
        यदि(थाक->isEmpty()) {
            फर्काऊ असत्य;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t थाक_Size=थाक->size();
        int32_t length=थाक->elementAti(थाक_Size-1);
        pos=uchars_+थाक->elementAti(थाक_Size-2);
        थाक->setSize(थाक_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        यदि(length>1) {
            pos=branchNext(pos, length, errorCode);
            यदि(pos==nullptr) {
                फर्काऊ सत्य;  // Reached a final value.
            }
        } अन्यथा {
            str_.append(*pos++);
        }
    }
    यदि(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        फर्काऊ truncateAndStop();
    }
    लागि(;;) {
        int32_t गाँठो=*pos++;
        यदि(गाँठो>=k_Min_मान_Lead) {
            यदि(skip_मान_2) {
                pos=skip_गाँठो_मान(pos, गाँठो);
                गाँठो&=k_गाँठो_Type_Mask;
                skip_मान_2=असत्य;
            } अन्यथा {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(गाँठो >> 15);
                यदि(isFinal) {
                    मान=read_मान(pos, गाँठो&0x7fff);
                } अन्यथा {
                    मान=read_गाँठो_मान(pos, गाँठो);
                }
                यदि(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } अन्यथा {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_मान_2=सत्य;
                }
                फर्काऊ सत्य;
            }
        }
        यदि(maxLength_>0 && str_.length()==maxLength_) {
            फर्काऊ truncateAndStop();
        }
        यदि(गाँठो<kMinLinearMatch) {
            यदि(गाँठो==0) {
                गाँठो=*pos++;
            }
            pos=branchNext(pos, गाँठो+1, errorCode);
            यदि(pos==nullptr) {
                फर्काऊ सत्य;  // Reached a final value.
            }
        } अन्यथा {
            // Linear-match node, append length units to str_.
            int32_t length=गाँठो-kMinLinearMatch+1;
            यदि(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                फर्काऊ truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
अचर char16_t*
UCharsTrie::Iterator::branchNext(अचर char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    जबसम्म(length>k_Max_Branch_Linear_Sub_गाँठो_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        थाक->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        थाक->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t गाँठो=*pos++;
    UBool isFinal = static_cast<UBool>(गाँठो >> 15);
    int32_t value=read_मान(pos, गाँठो&=0x7fff);
    pos=skip_मान(pos, गाँठो);
    थाक->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    थाक->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    यदि(isFinal) {
        pos_=nullptr;
        मान=value;
        फर्काऊ nullptr;
    } अन्यथा {
        फर्काऊ pos+value;
    }
}

U_NAMESPACE_END

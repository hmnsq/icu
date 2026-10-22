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

#شامل "unicode/utypes.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/unistr.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_مقدار_2(نادرست),
          maxLength_(maxStringLength), مقدار(0), پشته(nullptr) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    پشته=جدید UVector32(errorCode);
    اگر(پشته==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ثابت UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_مقدار_2(نادرست),
          maxLength_(maxStringLength), مقدار(0), پشته(nullptr) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    پشته=جدید UVector32(errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    اگر(پشته==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    اگر(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        اگر(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete پشته;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_مقدار_2=نادرست;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    اگر(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    پشته->setSize(0);
    بازگشت *این;
}

UBool
UCharsTrie::Iterator::hasNext() ثابت { بازگشت pos_!=nullptr || !پشته->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    ثابت char16_t *pos=pos_;
    اگر(pos==nullptr) {
        اگر(پشته->isEmpty()) {
            بازگشت نادرست;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t پشته_Size=پشته->size();
        int32_t length=پشته->elementAti(پشته_Size-1);
        pos=uchars_+پشته->elementAti(پشته_Size-2);
        پشته->setSize(پشته_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        اگر(length>1) {
            pos=branchNext(pos, length, errorCode);
            اگر(pos==nullptr) {
                بازگشت درست;  // Reached a final value.
            }
        } وگرنه {
            str_.append(*pos++);
        }
    }
    اگر(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        بازگشت truncateAndStop();
    }
    برای(;;) {
        int32_t گره=*pos++;
        اگر(گره>=k_Min_مقدار_Lead) {
            اگر(skip_مقدار_2) {
                pos=skip_گره_مقدار(pos, گره);
                گره&=k_گره_Type_Mask;
                skip_مقدار_2=نادرست;
            } وگرنه {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(گره >> 15);
                اگر(isFinal) {
                    مقدار=read_مقدار(pos, گره&0x7fff);
                } وگرنه {
                    مقدار=read_گره_مقدار(pos, گره);
                }
                اگر(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } وگرنه {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_مقدار_2=درست;
                }
                بازگشت درست;
            }
        }
        اگر(maxLength_>0 && str_.length()==maxLength_) {
            بازگشت truncateAndStop();
        }
        اگر(گره<kMinLinearMatch) {
            اگر(گره==0) {
                گره=*pos++;
            }
            pos=branchNext(pos, گره+1, errorCode);
            اگر(pos==nullptr) {
                بازگشت درست;  // Reached a final value.
            }
        } وگرنه {
            // Linear-match node, append length units to str_.
            int32_t length=گره-kMinLinearMatch+1;
            اگر(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                بازگشت truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت char16_t*
UCharsTrie::Iterator::branchNext(ثابت char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    وقتی(length>k_Max_Branch_Linear_Sub_گره_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        پشته->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        پشته->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t گره=*pos++;
    UBool isFinal = static_cast<UBool>(گره >> 15);
    int32_t value=read_مقدار(pos, گره&=0x7fff);
    pos=skip_مقدار(pos, گره);
    پشته->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    پشته->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    اگر(isFinal) {
        pos_=nullptr;
        مقدار=value;
        بازگشت nullptr;
    } وگرنه {
        بازگشت pos+value;
    }
}

U_NAMESPACE_END

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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ucharstrie.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_மதிப்பு_2(பொய்),
          maxLength_(maxStringLength), மதிப்பு(0), அடுக்கு(nullptr) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    அடுக்கு=புதிய UVector32(errorCode);
    எனில்(அடுக்கு==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(மாறிலி UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_மதிப்பு_2(பொய்),
          maxLength_(maxStringLength), மதிப்பு(0), அடுக்கு(nullptr) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    அடுக்கு=புதிய UVector32(errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில்(அடுக்கு==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    எனில்(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        எனில்(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete அடுக்கு;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_மதிப்பு_2=பொய்;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    எனில்(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    அடுக்கு->setSize(0);
    திரும்பு *இது;
}

UBool
UCharsTrie::Iterator::hasNext() மாறிலி { திரும்பு pos_!=nullptr || !அடுக்கு->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    மாறிலி char16_t *pos=pos_;
    எனில்(pos==nullptr) {
        எனில்(அடுக்கு->isEmpty()) {
            திரும்பு பொய்;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t அடுக்கு_Size=அடுக்கு->size();
        int32_t length=அடுக்கு->elementAti(அடுக்கு_Size-1);
        pos=uchars_+அடுக்கு->elementAti(அடுக்கு_Size-2);
        அடுக்கு->setSize(அடுக்கு_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        எனில்(length>1) {
            pos=branchNext(pos, length, errorCode);
            எனில்(pos==nullptr) {
                திரும்பு உண்மை;  // Reached a final value.
            }
        } இல்லையெனில் {
            str_.append(*pos++);
        }
    }
    எனில்(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        திரும்பு truncateAndStop();
    }
    க்கு(;;) {
        int32_t முனை=*pos++;
        எனில்(முனை>=k_Min_மதிப்பு_Lead) {
            எனில்(skip_மதிப்பு_2) {
                pos=skip_முனை_மதிப்பு(pos, முனை);
                முனை&=k_முனை_Type_Mask;
                skip_மதிப்பு_2=பொய்;
            } இல்லையெனில் {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(முனை >> 15);
                எனில்(isFinal) {
                    மதிப்பு=read_மதிப்பு(pos, முனை&0x7fff);
                } இல்லையெனில் {
                    மதிப்பு=read_முனை_மதிப்பு(pos, முனை);
                }
                எனில்(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } இல்லையெனில் {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_மதிப்பு_2=உண்மை;
                }
                திரும்பு உண்மை;
            }
        }
        எனில்(maxLength_>0 && str_.length()==maxLength_) {
            திரும்பு truncateAndStop();
        }
        எனில்(முனை<kMinLinearMatch) {
            எனில்(முனை==0) {
                முனை=*pos++;
            }
            pos=branchNext(pos, முனை+1, errorCode);
            எனில்(pos==nullptr) {
                திரும்பு உண்மை;  // Reached a final value.
            }
        } இல்லையெனில் {
            // Linear-match node, append length units to str_.
            int32_t length=முனை-kMinLinearMatch+1;
            எனில்(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                திரும்பு truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
மாறிலி char16_t*
UCharsTrie::Iterator::branchNext(மாறிலி char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    போது(length>k_Max_Branch_Linear_Sub_முனை_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        அடுக்கு->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        அடுக்கு->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t முனை=*pos++;
    UBool isFinal = static_cast<UBool>(முனை >> 15);
    int32_t value=read_மதிப்பு(pos, முனை&=0x7fff);
    pos=skip_மதிப்பு(pos, முனை);
    அடுக்கு->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    அடுக்கு->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    எனில்(isFinal) {
        pos_=nullptr;
        மதிப்பு=value;
        திரும்பு nullptr;
    } இல்லையெனில் {
        திரும்பு pos+value;
    }
}

U_NAMESPACE_END

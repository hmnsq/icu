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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/ucharstrie.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_કિંમત_2(ખોટું),
          maxLength_(maxStringLength), કિંમત(0), ઢગલો(nullptr) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ઢગલો=નવું UVector32(errorCode);
    જો(ઢગલો==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(અચલ UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_કિંમત_2(ખોટું),
          maxLength_(maxStringLength), કિંમત(0), ઢગલો(nullptr) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    ઢગલો=નવું UVector32(errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    જો(ઢગલો==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    જો(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        જો(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete ઢગલો;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_કિંમત_2=ખોટું;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    જો(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    ઢગલો->setSize(0);
    પાછા *આ;
}

UBool
UCharsTrie::Iterator::hasNext() અચલ { પાછા pos_!=nullptr || !ઢગલો->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    અચલ char16_t *pos=pos_;
    જો(pos==nullptr) {
        જો(ઢગલો->isEmpty()) {
            પાછા ખોટું;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ઢગલો_Size=ઢગલો->size();
        int32_t length=ઢગલો->elementAti(ઢગલો_Size-1);
        pos=uchars_+ઢગલો->elementAti(ઢગલો_Size-2);
        ઢગલો->setSize(ઢગલો_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        જો(length>1) {
            pos=branchNext(pos, length, errorCode);
            જો(pos==nullptr) {
                પાછા સાચું;  // Reached a final value.
            }
        } નહીંતર {
            str_.append(*pos++);
        }
    }
    જો(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        પાછા truncateAndStop();
    }
    માટે(;;) {
        int32_t ગાંઠ=*pos++;
        જો(ગાંઠ>=k_Min_કિંમત_Lead) {
            જો(skip_કિંમત_2) {
                pos=skip_ગાંઠ_કિંમત(pos, ગાંઠ);
                ગાંઠ&=k_ગાંઠ_Type_Mask;
                skip_કિંમત_2=ખોટું;
            } નહીંતર {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(ગાંઠ >> 15);
                જો(isFinal) {
                    કિંમત=read_કિંમત(pos, ગાંઠ&0x7fff);
                } નહીંતર {
                    કિંમત=read_ગાંઠ_કિંમત(pos, ગાંઠ);
                }
                જો(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } નહીંતર {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_કિંમત_2=સાચું;
                }
                પાછા સાચું;
            }
        }
        જો(maxLength_>0 && str_.length()==maxLength_) {
            પાછા truncateAndStop();
        }
        જો(ગાંઠ<kMinLinearMatch) {
            જો(ગાંઠ==0) {
                ગાંઠ=*pos++;
            }
            pos=branchNext(pos, ગાંઠ+1, errorCode);
            જો(pos==nullptr) {
                પાછા સાચું;  // Reached a final value.
            }
        } નહીંતર {
            // Linear-match node, append length units to str_.
            int32_t length=ગાંઠ-kMinLinearMatch+1;
            જો(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                પાછા truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
અચલ char16_t*
UCharsTrie::Iterator::branchNext(અચલ char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    જ્યારે(length>k_Max_Branch_Linear_Sub_ગાંઠ_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ઢગલો->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        ઢગલો->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t ગાંઠ=*pos++;
    UBool isFinal = static_cast<UBool>(ગાંઠ >> 15);
    int32_t value=read_કિંમત(pos, ગાંઠ&=0x7fff);
    pos=skip_કિંમત(pos, ગાંઠ);
    ઢગલો->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    ઢગલો->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    જો(isFinal) {
        pos_=nullptr;
        કિંમત=value;
        પાછા nullptr;
    } નહીંતર {
        પાછા pos+value;
    }
}

U_NAMESPACE_END

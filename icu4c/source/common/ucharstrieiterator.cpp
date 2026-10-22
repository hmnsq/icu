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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/ucharstrie.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_മൂല്യം_2(അസത്യം),
          maxLength_(maxStringLength), മൂല്യം(0), അട്ടി(nullptr) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    അട്ടി=പുതിയ UVector32(errorCode);
    എങ്കിൽ(അട്ടി==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(സ്ഥിരം UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_മൂല്യം_2(അസത്യം),
          maxLength_(maxStringLength), മൂല്യം(0), അട്ടി(nullptr) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    അട്ടി=പുതിയ UVector32(errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(അട്ടി==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    എങ്കിൽ(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        എങ്കിൽ(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete അട്ടി;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_മൂല്യം_2=അസത്യം;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    എങ്കിൽ(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    അട്ടി->setSize(0);
    തിരികെ *ഇത്;
}

UBool
UCharsTrie::Iterator::hasNext() സ്ഥിരം { തിരികെ pos_!=nullptr || !അട്ടി->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം char16_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        എങ്കിൽ(അട്ടി->isEmpty()) {
            തിരികെ അസത്യം;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t അട്ടി_Size=അട്ടി->size();
        int32_t length=അട്ടി->elementAti(അട്ടി_Size-1);
        pos=uchars_+അട്ടി->elementAti(അട്ടി_Size-2);
        അട്ടി->setSize(അട്ടി_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        എങ്കിൽ(length>1) {
            pos=branchNext(pos, length, errorCode);
            എങ്കിൽ(pos==nullptr) {
                തിരികെ സത്യം;  // Reached a final value.
            }
        } അല്ലെങ്കിൽ {
            str_.append(*pos++);
        }
    }
    എങ്കിൽ(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        തിരികെ truncateAndStop();
    }
    വേണ്ടി(;;) {
        int32_t കണ്ണി=*pos++;
        എങ്കിൽ(കണ്ണി>=k_Min_മൂല്യം_Lead) {
            എങ്കിൽ(skip_മൂല്യം_2) {
                pos=skip_കണ്ണി_മൂല്യം(pos, കണ്ണി);
                കണ്ണി&=k_കണ്ണി_Type_Mask;
                skip_മൂല്യം_2=അസത്യം;
            } അല്ലെങ്കിൽ {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(കണ്ണി >> 15);
                എങ്കിൽ(isFinal) {
                    മൂല്യം=read_മൂല്യം(pos, കണ്ണി&0x7fff);
                } അല്ലെങ്കിൽ {
                    മൂല്യം=read_കണ്ണി_മൂല്യം(pos, കണ്ണി);
                }
                എങ്കിൽ(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } അല്ലെങ്കിൽ {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_മൂല്യം_2=സത്യം;
                }
                തിരികെ സത്യം;
            }
        }
        എങ്കിൽ(maxLength_>0 && str_.length()==maxLength_) {
            തിരികെ truncateAndStop();
        }
        എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
            എങ്കിൽ(കണ്ണി==0) {
                കണ്ണി=*pos++;
            }
            pos=branchNext(pos, കണ്ണി+1, errorCode);
            എങ്കിൽ(pos==nullptr) {
                തിരികെ സത്യം;  // Reached a final value.
            }
        } അല്ലെങ്കിൽ {
            // Linear-match node, append length units to str_.
            int32_t length=കണ്ണി-kMinLinearMatch+1;
            എങ്കിൽ(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                തിരികെ truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
സ്ഥിരം char16_t*
UCharsTrie::Iterator::branchNext(സ്ഥിരം char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    ആകുമ്പോൾ(length>k_Max_Branch_Linear_Sub_കണ്ണി_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        അട്ടി->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        അട്ടി->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t കണ്ണി=*pos++;
    UBool isFinal = static_cast<UBool>(കണ്ണി >> 15);
    int32_t value=read_മൂല്യം(pos, കണ്ണി&=0x7fff);
    pos=skip_മൂല്യം(pos, കണ്ണി);
    അട്ടി->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    അട്ടി->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    എങ്കിൽ(isFinal) {
        pos_=nullptr;
        മൂല്യം=value;
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        തിരികെ pos+value;
    }
}

U_NAMESPACE_END

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrieiterator.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov03
*   created by: Markus W. Scherer
*/

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(സ്ഥിരം ശൂന്യം* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<സ്ഥിരം uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), മൂല്യം(0), അട്ടി(nullptr) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=പുതിയ CharString();
    അട്ടി=പുതിയ UVector32(errorCode);
    എങ്കിൽ(U_SUCCESS(errorCode) && (str_==nullptr || അട്ടി==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(സ്ഥിരം BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), മൂല്യം(0), അട്ടി(nullptr) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    str_=പുതിയ CharString();
    അട്ടി=പുതിയ UVector32(errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(str_==nullptr || അട്ടി==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    എങ്കിൽ(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        എങ്കിൽ(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<സ്ഥിരം അക്ഷരം *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete അട്ടി;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    എങ്കിൽ(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    അട്ടി->setSize(0);
    തിരികെ *ഇത്;
}

UBool
BytesTrie::Iterator::hasNext() സ്ഥിരം { തിരികെ pos_!=nullptr || !അട്ടി->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം uint8_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        എങ്കിൽ(അട്ടി->isEmpty()) {
            തിരികെ അസത്യം;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t അട്ടി_Size=അട്ടി->size();
        int32_t length=അട്ടി->elementAti(അട്ടി_Size-1);
        pos=bytes_+അട്ടി->elementAti(അട്ടി_Size-2);
        അട്ടി->setSize(അട്ടി_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        എങ്കിൽ(length>1) {
            pos=branchNext(pos, length, errorCode);
            എങ്കിൽ(pos==nullptr) {
                തിരികെ സത്യം;  // Reached a final value.
            }
        } അല്ലെങ്കിൽ {
            str_->append(static_cast<അക്ഷരം>(*pos++), errorCode);
        }
    }
    എങ്കിൽ(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        തിരികെ truncateAndStop();
    }
    വേണ്ടി(;;) {
        int32_t കണ്ണി=*pos++;
        എങ്കിൽ(കണ്ണി>=k_Min_മൂല്യം_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(കണ്ണി & k_മൂല്യം_Is_Final);
            മൂല്യം=read_മൂല്യം(pos, കണ്ണി>>1);
            എങ്കിൽ(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } അല്ലെങ്കിൽ {
                pos_=skip_മൂല്യം(pos, കണ്ണി);
            }
            തിരികെ സത്യം;
        }
        എങ്കിൽ(maxLength_>0 && str_->length()==maxLength_) {
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
            // Linear-match node, append length bytes to str_.
            int32_t length=കണ്ണി-kMinLinearMatch+1;
            എങ്കിൽ(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<സ്ഥിരം അക്ഷരം *>(pos),
                            maxLength_-str_->length(), errorCode);
                തിരികെ truncateAndStop();
            }
            str_->append(reinterpret_cast<സ്ഥിരം അക്ഷരം *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    മൂല്യം=-1;  // no real value for str
    തിരികെ സത്യം;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
സ്ഥിരം uint8_t*
BytesTrie::Iterator::branchNext(സ്ഥിരം uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    ആകുമ്പോൾ(length>k_Max_Branch_Linear_Sub_കണ്ണി_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        അട്ടി->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        അട്ടി->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t കണ്ണി=*pos++;
    UBool isFinal = static_cast<UBool>(കണ്ണി & k_മൂല്യം_Is_Final);
    int32_t value=read_മൂല്യം(pos, കണ്ണി>>1);
    pos=skip_മൂല്യം(pos, കണ്ണി);
    അട്ടി->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    അട്ടി->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<അക്ഷരം>(trieByte), errorCode);
    എങ്കിൽ(isFinal) {
        pos_=nullptr;
        മൂല്യം=value;
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        തിരികെ pos+value;
    }
}

U_NAMESPACE_END

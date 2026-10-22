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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"

U_NAMESPACE_BEGIN

ਬਾਈਟਾਂ_Trie::Iterator::Iterator(ਅਚਲ ਖਾਲੀ* trie_ਬਾਈਟਾਂ U_LIFETIME_BOUND,
                              int32_t max_String_ਲੰਮਾਈ,
                              UError_ਕੋਡ& error_ਕੋਡ)
        : ਬਾਈਟਾਂ_2(static_cast<ਅਚਲ uint8_t *>(trie_ਬਾਈਟਾਂ)),
          pos_(ਬਾਈਟਾਂ_2), initialPos_(ਬਾਈਟਾਂ_2),
          ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ(-1), initial_ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ(-1),
          str_(ਖਾਲੀ_ਸੂਚਕ), max_ਲੰਮਾਈ(max_String_ਲੰਮਾਈ), ਮੁੱਲ(0), ਢੇਰ(ਖਾਲੀ_ਸੂਚਕ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=ਨਵਾਂ CharString();
    ਢੇਰ=ਨਵਾਂ UVector32(error_ਕੋਡ);
    ਜੇ(U_SUCCESS(error_ਕੋਡ) && (str_==ਖਾਲੀ_ਸੂਚਕ || ਢੇਰ==ਖਾਲੀ_ਸੂਚਕ)) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}

ਬਾਈਟਾਂ_Trie::Iterator::Iterator(ਅਚਲ ਬਾਈਟਾਂ_Trie &trie, int32_t max_String_ਲੰਮਾਈ,
                              UError_ਕੋਡ &error_ਕੋਡ)
        : ਬਾਈਟਾਂ_2(trie.ਬਾਈਟਾਂ_2), pos_(trie.pos_), initialPos_(trie.pos_),
          ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ(trie.ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ),
          initial_ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ(trie.ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ),
          str_(ਖਾਲੀ_ਸੂਚਕ), max_ਲੰਮਾਈ(max_String_ਲੰਮਾਈ), ਮੁੱਲ(0), ਢੇਰ(ਖਾਲੀ_ਸੂਚਕ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    str_=ਨਵਾਂ CharString();
    ਢੇਰ=ਨਵਾਂ UVector32(error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(str_==ਖਾਲੀ_ਸੂਚਕ || ਢੇਰ==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    int32_t length=ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ;  // Actual remaining match length minus 1.
    ਜੇ(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        ਜੇ(max_ਲੰਮਾਈ>0 && length>max_ਲੰਮਾਈ) {
            length=max_ਲੰਮਾਈ;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ਅਚਲ ਅੱਖਰ *>(pos_), length, error_ਕੋਡ);
        pos_+=length;
        ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ-=length;
    }
}

ਬਾਈਟਾਂ_Trie::Iterator::~Iterator() {
    ਮਿਟਾਓ str_;
    ਮਿਟਾਓ ਢੇਰ;
}

ਬਾਈਟਾਂ_Trie::Iterator&
ਬਾਈਟਾਂ_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=initial_ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ;
    int32_t length=ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ+1;  // Remaining match length.
    ਜੇ(max_ਲੰਮਾਈ>0 && length>max_ਲੰਮਾਈ) {
        length=max_ਲੰਮਾਈ;
    }
    str_->truncate(length);
    pos_+=length;
    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ-=length;
    ਢੇਰ->ਸੈੱਟ_ਮਾਪ(0);
    ਵਾਪਸ *ਇਹ;
}

UBool
ਬਾਈਟਾਂ_Trie::Iterator::has_ਅਗਲਾ() ਅਚਲ { ਵਾਪਸ pos_!=ਖਾਲੀ_ਸੂਚਕ || !ਢੇਰ->is_ਖਾਲੀ_ਥਾਵਾਂ(); }

UBool
ਬਾਈਟਾਂ_Trie::Iterator::next(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ uint8_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(ਢੇਰ->is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ਵਾਪਸ ਝੂਠ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ਢੇਰ_ਮਾਪ=ਢੇਰ->size();
        int32_t length=ਢੇਰ->elementAti(ਢੇਰ_ਮਾਪ-1);
        pos=ਬਾਈਟਾਂ_2+ਢੇਰ->elementAti(ਢੇਰ_ਮਾਪ-2);
        ਢੇਰ->ਸੈੱਟ_ਮਾਪ(ਢੇਰ_ਮਾਪ-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ਜੇ(length>1) {
            pos=branch_ਅਗਲਾ(pos, length, error_ਕੋਡ);
            ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਸੱਚ;  // Reached a final value.
            }
        } ਨਹੀਂਤਾਂ {
            str_->append(static_cast<ਅੱਖਰ>(*pos++), error_ਕੋਡ);
        }
    }
    ਜੇ(ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        ਵਾਪਸ truncateAndStop();
    }
    ਲਈ(;;) {
        int32_t ਗੰਢ=*pos++;
        ਜੇ(ਗੰਢ>=k_Min_ਮੁੱਲ_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(ਗੰਢ & k_ਮੁੱਲ_Is_Final);
            ਮੁੱਲ=read_ਮੁੱਲ(pos, ਗੰਢ>>1);
            ਜੇ(isFinal || (max_ਲੰਮਾਈ>0 && str_->length()==max_ਲੰਮਾਈ)) {
                pos_=ਖਾਲੀ_ਸੂਚਕ;
            } ਨਹੀਂਤਾਂ {
                pos_=skip_ਮੁੱਲ(pos, ਗੰਢ);
            }
            ਵਾਪਸ ਸੱਚ;
        }
        ਜੇ(max_ਲੰਮਾਈ>0 && str_->length()==max_ਲੰਮਾਈ) {
            ਵਾਪਸ truncateAndStop();
        }
        ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
            ਜੇ(ਗੰਢ==0) {
                ਗੰਢ=*pos++;
            }
            pos=branch_ਅਗਲਾ(pos, ਗੰਢ+1, error_ਕੋਡ);
            ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਸੱਚ;  // Reached a final value.
            }
        } ਨਹੀਂਤਾਂ {
            // Linear-match node, append length bytes to str_.
            int32_t length=ਗੰਢ-k_Min_ਰੇਖਿਕ_ਮੇਲ+1;
            ਜੇ(max_ਲੰਮਾਈ>0 && str_->length()+length>max_ਲੰਮਾਈ) {
                str_->append(reinterpret_cast<ਅਚਲ ਅੱਖਰ *>(pos),
                            max_ਲੰਮਾਈ-str_->length(), error_ਕੋਡ);
                ਵਾਪਸ truncateAndStop();
            }
            str_->append(reinterpret_cast<ਅਚਲ ਅੱਖਰ *>(pos), length, error_ਕੋਡ);
            pos+=length;
        }
    }
}

StringPiece
ਬਾਈਟਾਂ_Trie::Iterator::getString() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ str_ == ਖਾਲੀ_ਸੂਚਕ ? StringPiece() : str_->ਮੰਜ਼ਿਲ_String_Piece();
}

UBool
ਬਾਈਟਾਂ_Trie::Iterator::truncateAndStop() {
    pos_=ਖਾਲੀ_ਸੂਚਕ;
    ਮੁੱਲ=-1;  // no real value for str
    ਵਾਪਸ ਸੱਚ;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ਅਚਲ uint8_t*
ਬਾਈਟਾਂ_Trie::Iterator::branch_ਅਗਲਾ(ਅਚਲ uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_ਕੋਡ& error_ਕੋਡ) {
    ਜਦੋਂ(length>k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ਢੇਰ->ਜੋੜੋ_Element(static_cast<int32_t>(skipDelta(pos) - ਬਾਈਟਾਂ_2), error_ਕੋਡ);
        ਢੇਰ->ਜੋੜੋ_Element(((length-(length>>1))<<16)|str_->length(), error_ਕੋਡ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t ਗੰਢ=*pos++;
    UBool isFinal = static_cast<UBool>(ਗੰਢ & k_ਮੁੱਲ_Is_Final);
    int32_t value=read_ਮੁੱਲ(pos, ਗੰਢ>>1);
    pos=skip_ਮੁੱਲ(pos, ਗੰਢ);
    ਢੇਰ->ਜੋੜੋ_Element(static_cast<int32_t>(pos - ਬਾਈਟਾਂ_2), error_ਕੋਡ);
    ਢੇਰ->ਜੋੜੋ_Element(((length-1)<<16)|str_->length(), error_ਕੋਡ);
    str_->append(static_cast<ਅੱਖਰ>(trieByte), error_ਕੋਡ);
    ਜੇ(isFinal) {
        pos_=ਖਾਲੀ_ਸੂਚਕ;
        ਮੁੱਲ=value;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pos+value;
    }
}

U_NAMESPACE_ਸਿਰਾ

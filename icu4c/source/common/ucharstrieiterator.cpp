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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_ਲੰਮਾਈ,
                               UError_ਕੋਡ &error_ਕੋਡ)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ(-1), initial_ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ(-1),
          skip_ਮੁੱਲ_2(ਝੂਠ),
          max_ਲੰਮਾਈ(max_String_ਲੰਮਾਈ), ਮੁੱਲ(0), ਢੇਰ(ਖਾਲੀ_ਸੂਚਕ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ਢੇਰ=ਨਵਾਂ UVector32(error_ਕੋਡ);
    ਜੇ(ਢੇਰ==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ਅਚਲ UCharsTrie &trie, int32_t max_String_ਲੰਮਾਈ,
                               UError_ਕੋਡ &error_ਕੋਡ)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ(trie.ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ),
          initial_ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ(trie.ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ),
          skip_ਮੁੱਲ_2(ਝੂਠ),
          max_ਲੰਮਾਈ(max_String_ਲੰਮਾਈ), ਮੁੱਲ(0), ਢੇਰ(ਖਾਲੀ_ਸੂਚਕ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਢੇਰ=ਨਵਾਂ UVector32(error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(ਢੇਰ==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    int32_t length=ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ;  // Actual remaining match length minus 1.
    ਜੇ(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        ਜੇ(max_ਲੰਮਾਈ>0 && length>max_ਲੰਮਾਈ) {
            length=max_ਲੰਮਾਈ;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    ਮਿਟਾਓ ਢੇਰ;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=initial_ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ;
    skip_ਮੁੱਲ_2=ਝੂਠ;
    int32_t length=ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ+1;  // Remaining match length.
    ਜੇ(max_ਲੰਮਾਈ>0 && length>max_ਲੰਮਾਈ) {
        length=max_ਲੰਮਾਈ;
    }
    str_.truncate(length);
    pos_+=length;
    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ-=length;
    ਢੇਰ->ਸੈੱਟ_ਮਾਪ(0);
    ਵਾਪਸ *ਇਹ;
}

UBool
UCharsTrie::Iterator::has_ਅਗਲਾ() ਅਚਲ { ਵਾਪਸ pos_!=ਖਾਲੀ_ਸੂਚਕ || !ਢੇਰ->is_ਖਾਲੀ_ਥਾਵਾਂ(); }

UBool
UCharsTrie::Iterator::next(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ char16_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(ਢੇਰ->is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ਵਾਪਸ ਝੂਠ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ਢੇਰ_ਮਾਪ=ਢੇਰ->size();
        int32_t length=ਢੇਰ->elementAti(ਢੇਰ_ਮਾਪ-1);
        pos=uchars_+ਢੇਰ->elementAti(ਢੇਰ_ਮਾਪ-2);
        ਢੇਰ->ਸੈੱਟ_ਮਾਪ(ਢੇਰ_ਮਾਪ-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ਜੇ(length>1) {
            pos=branch_ਅਗਲਾ(pos, length, error_ਕੋਡ);
            ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਸੱਚ;  // Reached a final value.
            }
        } ਨਹੀਂਤਾਂ {
            str_.append(*pos++);
        }
    }
    ਜੇ(ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ਵਾਪਸ truncateAndStop();
    }
    ਲਈ(;;) {
        int32_t ਗੰਢ=*pos++;
        ਜੇ(ਗੰਢ>=k_Min_ਮੁੱਲ_Lead) {
            ਜੇ(skip_ਮੁੱਲ_2) {
                pos=skip_ਗੰਢ_ਮੁੱਲ(pos, ਗੰਢ);
                ਗੰਢ&=k_ਗੰਢ_Type_ਨਕਾਬ;
                skip_ਮੁੱਲ_2=ਝੂਠ;
            } ਨਹੀਂਤਾਂ {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(ਗੰਢ >> 15);
                ਜੇ(isFinal) {
                    ਮੁੱਲ=read_ਮੁੱਲ(pos, ਗੰਢ&0x7fff);
                } ਨਹੀਂਤਾਂ {
                    ਮੁੱਲ=read_ਗੰਢ_ਮੁੱਲ(pos, ਗੰਢ);
                }
                ਜੇ(isFinal || (max_ਲੰਮਾਈ>0 && str_.length()==max_ਲੰਮਾਈ)) {
                    pos_=ਖਾਲੀ_ਸੂਚਕ;
                } ਨਹੀਂਤਾਂ {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_ਮੁੱਲ_2=ਸੱਚ;
                }
                ਵਾਪਸ ਸੱਚ;
            }
        }
        ਜੇ(max_ਲੰਮਾਈ>0 && str_.length()==max_ਲੰਮਾਈ) {
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
            // Linear-match node, append length units to str_.
            int32_t length=ਗੰਢ-k_Min_ਰੇਖਿਕ_ਮੇਲ+1;
            ਜੇ(max_ਲੰਮਾਈ>0 && str_.length()+length>max_ਲੰਮਾਈ) {
                str_.append(pos, max_ਲੰਮਾਈ-str_.length());
                ਵਾਪਸ truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ਅਚਲ char16_t*
UCharsTrie::Iterator::branch_ਅਗਲਾ(ਅਚਲ char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UError_ਕੋਡ& error_ਕੋਡ) {
    ਜਦੋਂ(length>k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ਢੇਰ->ਜੋੜੋ_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_ਕੋਡ);
        ਢੇਰ->ਜੋੜੋ_Element(((length-(length>>1))<<16)|str_.length(), error_ਕੋਡ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t ਗੰਢ=*pos++;
    UBool isFinal = static_cast<UBool>(ਗੰਢ >> 15);
    int32_t value=read_ਮੁੱਲ(pos, ਗੰਢ&=0x7fff);
    pos=skip_ਮੁੱਲ(pos, ਗੰਢ);
    ਢੇਰ->ਜੋੜੋ_Element(static_cast<int32_t>(pos - uchars_), error_ਕੋਡ);
    ਢੇਰ->ਜੋੜੋ_Element(((length-1)<<16)|str_.length(), error_ਕੋਡ);
    str_.append(trieUnit);
    ਜੇ(isFinal) {
        pos_=ਖਾਲੀ_ਸੂਚਕ;
        ਮੁੱਲ=value;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pos+value;
    }
}

U_NAMESPACE_ਸਿਰਾ

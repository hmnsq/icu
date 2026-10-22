// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrie.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_NAMESPACE_BEGIN

ਬਾਈਟਾਂ_Trie::~ਬਾਈਟਾਂ_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
ਬਾਈਟਾਂ_Trie::read_ਮੁੱਲ(ਅਚਲ uint8_t *pos, int32_t leadByte) {
    int32_t value;
    ਜੇ(leadByte<k_Min_Two_Byte_ਮੁੱਲ_Lead) {
        value=leadByte-k_Min_One_Byte_ਮੁੱਲ_Lead;
    } ਨਹੀਂਤਾਂ ਜੇ(leadByte<k_Min_Three_Byte_ਮੁੱਲ_Lead) {
        value=((leadByte-k_Min_Two_Byte_ਮੁੱਲ_Lead)<<8)|*pos;
    } ਨਹੀਂਤਾਂ ਜੇ(leadByte<k_Four_Byte_ਮੁੱਲ_Lead) {
        value=((leadByte-k_Min_Three_Byte_ਮੁੱਲ_Lead)<<16)|(pos[0]<<8)|pos[1];
    } ਨਹੀਂਤਾਂ ਜੇ(leadByte==k_Four_Byte_ਮੁੱਲ_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ਨਹੀਂਤਾਂ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ਵਾਪਸ value;
}

ਅਚਲ uint8_t*
ਬਾਈਟਾਂ_Trie::jumpByDelta(ਅਚਲ uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ਜੇ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ਨਹੀਂਤਾਂ ਜੇ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ਨਹੀਂਤਾਂ ਜੇ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ਨਹੀਂਤਾਂ ਜੇ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ਨਹੀਂਤਾਂ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ਵਾਪਸ pos+delta;
}

UString_Trie_ਨਤੀਜਾ
ਬਾਈਟਾਂ_Trie::current() ਅਚਲ {
    ਅਚਲ uint8_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
    } ਨਹੀਂਤਾਂ {
        int32_t ਗੰਢ;
        ਵਾਪਸ (ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
    }
}

UString_Trie_ਨਤੀਜਾ
ਬਾਈਟਾਂ_Trie::branch_ਅਗਲਾ(ਅਚਲ uint8_t *pos, int32_t length, int32_t ਵਿੱਚ_Byte) {
    // Branch according to the current byte.
    ਜੇ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ਜਦੋਂ(length>k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ) {
        ਜੇ(ਵਿੱਚ_Byte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ਨਹੀਂਤਾਂ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ਕਰੋ {
        ਜੇ(ਵਿੱਚ_Byte==*pos++) {
            UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ;
            int32_t ਗੰਢ=*pos;
            U_ASSERT(ਗੰਢ>=k_Min_ਮੁੱਲ_Lead);
            ਜੇ(ਗੰਢ&k_ਮੁੱਲ_Is_Final) {
                // Leave the final value for getValue() to read.
                ਨਤੀਜਾ=USTRINGTRIE_FINAL_ਮੁੱਲ;
            } ਨਹੀਂਤਾਂ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ਗੰਢ>>=1;
                int32_t delta;
                ਜੇ(ਗੰਢ<k_Min_Two_Byte_ਮੁੱਲ_Lead) {
                    delta=ਗੰਢ-k_Min_One_Byte_ਮੁੱਲ_Lead;
                } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Min_Three_Byte_ਮੁੱਲ_Lead) {
                    delta=((ਗੰਢ-k_Min_Two_Byte_ਮੁੱਲ_Lead)<<8)|*pos++;
                } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Four_Byte_ਮੁੱਲ_Lead) {
                    delta=((ਗੰਢ-k_Min_Three_Byte_ਮੁੱਲ_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ==k_Four_Byte_ਮੁੱਲ_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ਨਹੀਂਤਾਂ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ਗੰਢ=*pos;
                ਨਤੀਜਾ= ਗੰਢ>=k_Min_ਮੁੱਲ_Lead ? ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
            }
            pos_=pos;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        --length;
        pos=skip_ਮੁੱਲ(pos);
    } ਜਦੋਂ(length>1);
    ਜੇ(ਵਿੱਚ_Byte==*pos++) {
        pos_=pos;
        int32_t ਗੰਢ=*pos;
        ਵਾਪਸ ਗੰਢ>=k_Min_ਮੁੱਲ_Lead ? ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
    } ਨਹੀਂਤਾਂ {
        stop();
        ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
    }
}

UString_Trie_ਨਤੀਜਾ
ਬਾਈਟਾਂ_Trie::ਅਗਲਾ_Impl(ਅਚਲ uint8_t *pos, int32_t ਵਿੱਚ_Byte) {
    ਲਈ(;;) {
        int32_t ਗੰਢ=*pos++;
        ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
            ਵਾਪਸ branch_ਅਗਲਾ(pos, ਗੰਢ, ਵਿੱਚ_Byte);
        } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Min_ਮੁੱਲ_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ਗੰਢ-k_Min_ਰੇਖਿਕ_ਮੇਲ;  // Actual match length minus 1.
            ਜੇ(ਵਿੱਚ_Byte==*pos++) {
                ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=--length;
                pos_=pos;
                ਵਾਪਸ (length<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                        ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
            } ਨਹੀਂਤਾਂ {
                // No match.
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ&k_ਮੁੱਲ_Is_Final) {
            // No further matching bytes.
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ {
            // Skip intermediate value.
            pos=skip_ਮੁੱਲ(pos, ਗੰਢ);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_ਮੁੱਲ_Lead);
        }
    }
    stop();
    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
}

UString_Trie_ਨਤੀਜਾ
ਬਾਈਟਾਂ_Trie::next(int32_t ਵਿੱਚ_Byte) {
    ਅਚਲ uint8_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
    }
    ਜੇ(ਵਿੱਚ_Byte<0) {
        ਵਿੱਚ_Byte+=0x100;
    }
    int32_t length=ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ;  // Actual remaining match length minus 1.
    ਜੇ(length>=0) {
        // Remaining part of a linear-match node.
        ਜੇ(ਵਿੱਚ_Byte==*pos++) {
            ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=--length;
            pos_=pos;
            int32_t ਗੰਢ;
            ਵਾਪਸ (length<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                    ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
        } ਨਹੀਂਤਾਂ {
            stop();
            ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
        }
    }
    ਵਾਪਸ ਅਗਲਾ_Impl(pos, ਵਿੱਚ_Byte);
}

UString_Trie_ਨਤੀਜਾ
ਬਾਈਟਾਂ_Trie::next(ਅਚਲ ਅੱਖਰ *s, int32_t s_ਲੰਮਾਈ) {
    ਜੇ(s_ਲੰਮਾਈ<0 ? *s==0 : s_ਲੰਮਾਈ==0) {
        // Empty input.
        ਵਾਪਸ current();
    }
    ਅਚਲ uint8_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
    }
    int32_t length=ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ;  // Actual remaining match length minus 1.
    ਲਈ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t ਵਿੱਚ_Byte;
        ਜੇ(s_ਲੰਮਾਈ<0) {
            ਲਈ(;;) {
                ਜੇ((ਵਿੱਚ_Byte=*s++)==0) {
                    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=length;
                    pos_=pos;
                    int32_t ਗੰਢ;
                    ਵਾਪਸ (length<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                            ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
                }
                ਜੇ(length<0) {
                    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=length;
                    ਰੋਕੋ;
                }
                ਜੇ(ਵਿੱਚ_Byte!=*pos) {
                    stop();
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                ++pos;
                --length;
            }
        } ਨਹੀਂਤਾਂ {
            ਲਈ(;;) {
                ਜੇ(s_ਲੰਮਾਈ==0) {
                    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=length;
                    pos_=pos;
                    int32_t ਗੰਢ;
                    ਵਾਪਸ (length<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                            ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
                }
                ਵਿੱਚ_Byte=*s++;
                --s_ਲੰਮਾਈ;
                ਜੇ(length<0) {
                    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=length;
                    ਰੋਕੋ;
                }
                ਜੇ(ਵਿੱਚ_Byte!=*pos) {
                    stop();
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                ++pos;
                --length;
            }
        }
        ਲਈ(;;) {
            int32_t ਗੰਢ=*pos++;
            ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
                UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ=branch_ਅਗਲਾ(pos, ਗੰਢ, ਵਿੱਚ_Byte);
                ਜੇ(ਨਤੀਜਾ==USTRINGTRIE_NO_ਮੇਲ) {
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                // Fetch the next input byte, if there is one.
                ਜੇ(s_ਲੰਮਾਈ<0) {
                    ਜੇ((ਵਿੱਚ_Byte=*s++)==0) {
                        ਵਾਪਸ ਨਤੀਜਾ;
                    }
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(s_ਲੰਮਾਈ==0) {
                        ਵਾਪਸ ਨਤੀਜਾ;
                    }
                    ਵਿੱਚ_Byte=*s++;
                    --s_ਲੰਮਾਈ;
                }
                ਜੇ(ਨਤੀਜਾ==USTRINGTRIE_FINAL_ਮੁੱਲ) {
                    // No further matching bytes.
                    stop();
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Min_ਮੁੱਲ_Lead) {
                // Match length+1 bytes.
                length=ਗੰਢ-k_Min_ਰੇਖਿਕ_ਮੇਲ;  // Actual match length minus 1.
                ਜੇ(ਵਿੱਚ_Byte!=*pos) {
                    stop();
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                ++pos;
                --length;
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ&k_ਮੁੱਲ_Is_Final) {
                // No further matching bytes.
                stop();
                ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
            } ਨਹੀਂਤਾਂ {
                // Skip intermediate value.
                pos=skip_ਮੁੱਲ(pos, ਗੰਢ);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_ਮੁੱਲ_Lead);
            }
        }
    }
}

ਅਚਲ uint8_t*
ਬਾਈਟਾਂ_Trie::ਲੱਭੋ_Unique_ਮੁੱਲ_From_Branch(ਅਚਲ uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_ਮੁੱਲ,
                                     int32_t& unique_ਮੁੱਲ) {
    ਜਦੋਂ(length>k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ) {
        ++pos;  // ignore the comparison byte
        ਜੇ(ਖਾਲੀ_ਸੂਚਕ==ਲੱਭੋ_Unique_ਮੁੱਲ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ਮੁੱਲ, unique_ਮੁੱਲ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ਕਰੋ {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ਗੰਢ=*pos++;
        UBool isFinal = static_cast<UBool>(ਗੰਢ & k_ਮੁੱਲ_Is_Final);
        int32_t value=read_ਮੁੱਲ(pos, ਗੰਢ>>1);
        pos=skip_ਮੁੱਲ(pos, ਗੰਢ);
        ਜੇ(isFinal) {
            ਜੇ(have_Unique_ਮੁੱਲ) {
                ਜੇ(value!=unique_ਮੁੱਲ) {
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
            } ਨਹੀਂਤਾਂ {
                unique_ਮੁੱਲ=value;
                have_Unique_ਮੁੱਲ=ਸੱਚ;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ(!ਲੱਭੋ_Unique_ਮੁੱਲ(pos+value, have_Unique_ਮੁੱਲ, unique_ਮੁੱਲ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            have_Unique_ਮੁੱਲ=ਸੱਚ;
        }
    } ਜਦੋਂ(--length>1);
    ਵਾਪਸ pos+1;  // ignore the last comparison byte
}

UBool
ਬਾਈਟਾਂ_Trie::ਲੱਭੋ_Unique_ਮੁੱਲ(ਅਚਲ uint8_t *pos, UBool have_Unique_ਮੁੱਲ, int32_t &unique_ਮੁੱਲ) {
    ਲਈ(;;) {
        int32_t ਗੰਢ=*pos++;
        ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
            ਜੇ(ਗੰਢ==0) {
                ਗੰਢ=*pos++;
            }
            pos=ਲੱਭੋ_Unique_ਮੁੱਲ_From_Branch(pos, ਗੰਢ+1, have_Unique_ਮੁੱਲ, unique_ਮੁੱਲ);
            ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਝੂਠ;
            }
            have_Unique_ਮੁੱਲ=ਸੱਚ;
        } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Min_ਮੁੱਲ_Lead) {
            // linear-match node
            pos+=ਗੰਢ-k_Min_ਰੇਖਿਕ_ਮੇਲ+1;  // Ignore the match bytes.
        } ਨਹੀਂਤਾਂ {
            UBool isFinal = static_cast<UBool>(ਗੰਢ & k_ਮੁੱਲ_Is_Final);
            int32_t value=read_ਮੁੱਲ(pos, ਗੰਢ>>1);
            ਜੇ(have_Unique_ਮੁੱਲ) {
                ਜੇ(value!=unique_ਮੁੱਲ) {
                    ਵਾਪਸ ਝੂਠ;
                }
            } ਨਹੀਂਤਾਂ {
                unique_ਮੁੱਲ=value;
                have_Unique_ਮੁੱਲ=ਸੱਚ;
            }
            ਜੇ(isFinal) {
                ਵਾਪਸ ਸੱਚ;
            }
            pos=skip_ਮੁੱਲ(pos, ਗੰਢ);
        }
    }
}

int32_t
ਬਾਈਟਾਂ_Trie::get_ਅਗਲਾ_ਬਾਈਟਾਂ(ByteSink &out) ਅਚਲ {
    ਅਚਲ uint8_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ਵਾਪਸ 1;
    }
    int32_t ਗੰਢ=*pos++;
    ਜੇ(ਗੰਢ>=k_Min_ਮੁੱਲ_Lead) {
        ਜੇ(ਗੰਢ&k_ਮੁੱਲ_Is_Final) {
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ {
            pos=skip_ਮੁੱਲ(pos, ਗੰਢ);
            ਗੰਢ=*pos++;
            U_ASSERT(ਗੰਢ<k_Min_ਮੁੱਲ_Lead);
        }
    }
    ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
        ਜੇ(ਗੰਢ==0) {
            ਗੰਢ=*pos++;
        }
        get_ਅਗਲਾ_Branch_ਬਾਈਟਾਂ(pos, ++ਗੰਢ, out);
        ਵਾਪਸ ਗੰਢ;
    } ਨਹੀਂਤਾਂ {
        // First byte of the linear-match node.
        append(out, *pos);
        ਵਾਪਸ 1;
    }
}

ਖਾਲੀ
ਬਾਈਟਾਂ_Trie::get_ਅਗਲਾ_Branch_ਬਾਈਟਾਂ(ਅਚਲ uint8_t *pos, int32_t length, ByteSink &out) {
    ਜਦੋਂ(length>k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ) {
        ++pos;  // ignore the comparison byte
        get_ਅਗਲਾ_Branch_ਬਾਈਟਾਂ(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ਕਰੋ {
        append(out, *pos++);
        pos=skip_ਮੁੱਲ(pos);
    } ਜਦੋਂ(--length>1);
    append(out, *pos);
}

ਖਾਲੀ
ਬਾਈਟਾਂ_Trie::append(ByteSink &out, ਪੂਰਨ c) {
    ਅੱਖਰ ch = static_cast<ਅੱਖਰ>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_ਸਿਰਾ

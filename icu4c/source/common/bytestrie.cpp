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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN

ಬೈಟುಗಳು_Trie::~ಬೈಟುಗಳು_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
ಬೈಟುಗಳು_Trie::read_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ uint8_t *pos, int32_t leadByte) {
    int32_t value;
    ಆದರೆ(leadByte<k_Min_Two_Byte_ಮೌಲ್ಯ_Lead) {
        value=leadByte-k_Min_One_Byte_ಮೌಲ್ಯ_Lead;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(leadByte<k_Min_Three_Byte_ಮೌಲ್ಯ_Lead) {
        value=((leadByte-k_Min_Two_Byte_ಮೌಲ್ಯ_Lead)<<8)|*pos;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(leadByte<k_Four_Byte_ಮೌಲ್ಯ_Lead) {
        value=((leadByte-k_Min_Three_Byte_ಮೌಲ್ಯ_Lead)<<16)|(pos[0]<<8)|pos[1];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(leadByte==k_Four_Byte_ಮೌಲ್ಯ_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ಇಲ್ಲದಿದ್ದರೆ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ಹಿಂತಿರುಗಿ value;
}

ಸ್ಥಿರಾಂಕ uint8_t*
ಬೈಟುಗಳು_Trie::jumpByDelta(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ಆದರೆ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ಇಲ್ಲದಿದ್ದರೆ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ಹಿಂತಿರುಗಿ pos+delta;
}

UString_Trie_ಫಲಿತಾಂಶ
ಬೈಟುಗಳು_Trie::current() ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ uint8_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t ಗಂಟು;
        ಹಿಂತಿರುಗಿ (ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
    }
}

UString_Trie_ಫಲಿತಾಂಶ
ಬೈಟುಗಳು_Trie::branch_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ uint8_t *pos, int32_t length, int32_t ಒಳ_Byte) {
    // Branch according to the current byte.
    ಆದರೆ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ಆಗ(length>k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ) {
        ಆದರೆ(ಒಳ_Byte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ಇಲ್ಲದಿದ್ದರೆ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ಮಾಡು {
        ಆದರೆ(ಒಳ_Byte==*pos++) {
            UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ;
            int32_t ಗಂಟು=*pos;
            U_ASSERT(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead);
            ಆದರೆ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
                // Leave the final value for getValue() to read.
                ಫಲಿತಾಂಶ=USTRINGTRIE_FINAL_ಮೌಲ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ಗಂಟು>>=1;
                int32_t delta;
                ಆದರೆ(ಗಂಟು<k_Min_Two_Byte_ಮೌಲ್ಯ_Lead) {
                    delta=ಗಂಟು-k_Min_One_Byte_ಮೌಲ್ಯ_Lead;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Min_Three_Byte_ಮೌಲ್ಯ_Lead) {
                    delta=((ಗಂಟು-k_Min_Two_Byte_ಮೌಲ್ಯ_Lead)<<8)|*pos++;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Four_Byte_ಮೌಲ್ಯ_Lead) {
                    delta=((ಗಂಟು-k_Min_Three_Byte_ಮೌಲ್ಯ_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು==k_Four_Byte_ಮೌಲ್ಯ_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ಗಂಟು=*pos;
                ಫಲಿತಾಂಶ= ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead ? ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
            }
            pos_=pos;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        --length;
        pos=skip_ಮೌಲ್ಯ(pos);
    } ಆಗ(length>1);
    ಆದರೆ(ಒಳ_Byte==*pos++) {
        pos_=pos;
        int32_t ಗಂಟು=*pos;
        ಹಿಂತಿರುಗಿ ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead ? ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        stop();
        ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
    }
}

UString_Trie_ಫಲಿತಾಂಶ
ಬೈಟುಗಳು_Trie::ಮುಂದಿನ_Impl(ಸ್ಥಿರಾಂಕ uint8_t *pos, int32_t ಒಳ_Byte) {
    ಗಾಗಿ(;;) {
        int32_t ಗಂಟು=*pos++;
        ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
            ಹಿಂತಿರುಗಿ branch_ಮುಂದಿನ(pos, ಗಂಟು, ಒಳ_Byte);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ಗಂಟು-k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ;  // Actual match length minus 1.
            ಆದರೆ(ಒಳ_Byte==*pos++) {
                ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=--length;
                pos_=pos;
                ಹಿಂತಿರುಗಿ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                        ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // No match.
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
            // No further matching bytes.
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Skip intermediate value.
            pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_ಮೌಲ್ಯ_Lead);
        }
    }
    stop();
    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
}

UString_Trie_ಫಲಿತಾಂಶ
ಬೈಟುಗಳು_Trie::next(int32_t ಒಳ_Byte) {
    ಸ್ಥಿರಾಂಕ uint8_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
    }
    ಆದರೆ(ಒಳ_Byte<0) {
        ಒಳ_Byte+=0x100;
    }
    int32_t length=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;  // Actual remaining match length minus 1.
    ಆದರೆ(length>=0) {
        // Remaining part of a linear-match node.
        ಆದರೆ(ಒಳ_Byte==*pos++) {
            ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=--length;
            pos_=pos;
            int32_t ಗಂಟು;
            ಹಿಂತಿರುಗಿ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                    ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            stop();
            ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
        }
    }
    ಹಿಂತಿರುಗಿ ಮುಂದಿನ_Impl(pos, ಒಳ_Byte);
}

UString_Trie_ಫಲಿತಾಂಶ
ಬೈಟುಗಳು_Trie::next(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t s_ಉದ್ದಳತೆ) {
    ಆದರೆ(s_ಉದ್ದಳತೆ<0 ? *s==0 : s_ಉದ್ದಳತೆ==0) {
        // Empty input.
        ಹಿಂತಿರುಗಿ current();
    }
    ಸ್ಥಿರಾಂಕ uint8_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
    }
    int32_t length=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;  // Actual remaining match length minus 1.
    ಗಾಗಿ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t ಒಳ_Byte;
        ಆದರೆ(s_ಉದ್ದಳತೆ<0) {
            ಗಾಗಿ(;;) {
                ಆದರೆ((ಒಳ_Byte=*s++)==0) {
                    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=length;
                    pos_=pos;
                    int32_t ಗಂಟು;
                    ಹಿಂತಿರುಗಿ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                            ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
                }
                ಆದರೆ(length<0) {
                    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=length;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(ಒಳ_Byte!=*pos) {
                    stop();
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                ++pos;
                --length;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಾಗಿ(;;) {
                ಆದರೆ(s_ಉದ್ದಳತೆ==0) {
                    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=length;
                    pos_=pos;
                    int32_t ಗಂಟು;
                    ಹಿಂತಿರುಗಿ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                            ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
                }
                ಒಳ_Byte=*s++;
                --s_ಉದ್ದಳತೆ;
                ಆದರೆ(length<0) {
                    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=length;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(ಒಳ_Byte!=*pos) {
                    stop();
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                ++pos;
                --length;
            }
        }
        ಗಾಗಿ(;;) {
            int32_t ಗಂಟು=*pos++;
            ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
                UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ=branch_ಮುಂದಿನ(pos, ಗಂಟು, ಒಳ_Byte);
                ಆದರೆ(ಫಲಿತಾಂಶ==USTRINGTRIE_NO_ಹೊಂದಿಕೆ) {
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                // Fetch the next input byte, if there is one.
                ಆದರೆ(s_ಉದ್ದಳತೆ<0) {
                    ಆದರೆ((ಒಳ_Byte=*s++)==0) {
                        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(s_ಉದ್ದಳತೆ==0) {
                        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
                    }
                    ಒಳ_Byte=*s++;
                    --s_ಉದ್ದಳತೆ;
                }
                ಆದರೆ(ಫಲಿತಾಂಶ==USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                    // No further matching bytes.
                    stop();
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
                // Match length+1 bytes.
                length=ಗಂಟು-k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ;  // Actual match length minus 1.
                ಆದರೆ(ಒಳ_Byte!=*pos) {
                    stop();
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                ++pos;
                --length;
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
                // No further matching bytes.
                stop();
                ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Skip intermediate value.
                pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_ಮೌಲ್ಯ_Lead);
            }
        }
    }
}

ಸ್ಥಿರಾಂಕ uint8_t*
ಬೈಟುಗಳು_Trie::ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ_From_Branch(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_ಮೌಲ್ಯ,
                                     int32_t& unique_ಮೌಲ್ಯ) {
    ಆಗ(length>k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ) {
        ++pos;  // ignore the comparison byte
        ಆದರೆ(ಶೂನ್ಯಸೂಚಕ==ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ಮಾಡು {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ಗಂಟು=*pos++;
        UBool isFinal = static_cast<UBool>(ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final);
        int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು>>1);
        pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
        ಆದರೆ(isFinal) {
            ಆದರೆ(have_Unique_ಮೌಲ್ಯ) {
                ಆದರೆ(value!=unique_ಮೌಲ್ಯ) {
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                unique_ಮೌಲ್ಯ=value;
                have_Unique_ಮೌಲ್ಯ=ನಿಜ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(!ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ(pos+value, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            have_Unique_ಮೌಲ್ಯ=ನಿಜ;
        }
    } ಆಗ(--length>1);
    ಹಿಂತಿರುಗಿ pos+1;  // ignore the last comparison byte
}

UBool
ಬೈಟುಗಳು_Trie::ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ uint8_t *pos, UBool have_Unique_ಮೌಲ್ಯ, int32_t &unique_ಮೌಲ್ಯ) {
    ಗಾಗಿ(;;) {
        int32_t ಗಂಟು=*pos++;
        ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
            ಆದರೆ(ಗಂಟು==0) {
                ಗಂಟು=*pos++;
            }
            pos=ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ_From_Branch(pos, ಗಂಟು+1, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ);
            ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            have_Unique_ಮೌಲ್ಯ=ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
            // linear-match node
            pos+=ಗಂಟು-k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ+1;  // Ignore the match bytes.
        } ಇಲ್ಲದಿದ್ದರೆ {
            UBool isFinal = static_cast<UBool>(ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final);
            int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು>>1);
            ಆದರೆ(have_Unique_ಮೌಲ್ಯ) {
                ಆದರೆ(value!=unique_ಮೌಲ್ಯ) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                unique_ಮೌಲ್ಯ=value;
                have_Unique_ಮೌಲ್ಯ=ನಿಜ;
            }
            ಆದರೆ(isFinal) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
            pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
        }
    }
}

int32_t
ಬೈಟುಗಳು_Trie::get_ಮುಂದಿನ_ಬೈಟುಗಳು(ByteSink &out) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ uint8_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ಹಿಂತಿರುಗಿ 1;
    }
    int32_t ಗಂಟು=*pos++;
    ಆದರೆ(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead) {
        ಆದರೆ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
            ಗಂಟು=*pos++;
            U_ASSERT(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead);
        }
    }
    ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
        ಆದರೆ(ಗಂಟು==0) {
            ಗಂಟು=*pos++;
        }
        get_ಮುಂದಿನ_Branch_ಬೈಟುಗಳು(pos, ++ಗಂಟು, out);
        ಹಿಂತಿರುಗಿ ಗಂಟು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // First byte of the linear-match node.
        append(out, *pos);
        ಹಿಂತಿರುಗಿ 1;
    }
}

ಶೂನ್ಯ
ಬೈಟುಗಳು_Trie::get_ಮುಂದಿನ_Branch_ಬೈಟುಗಳು(ಸ್ಥಿರಾಂಕ uint8_t *pos, int32_t length, ByteSink &out) {
    ಆಗ(length>k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ) {
        ++pos;  // ignore the comparison byte
        get_ಮುಂದಿನ_Branch_ಬೈಟುಗಳು(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ಮಾಡು {
        append(out, *pos++);
        pos=skip_ಮೌಲ್ಯ(pos);
    } ಆಗ(--length>1);
    append(out, *pos);
}

ಶೂನ್ಯ
ಬೈಟುಗಳು_Trie::append(ByteSink &out, ಪೂರ್ಣಾಂಕ c) {
    ಅಕ್ಷರ ch = static_cast<ಅಕ್ಷರ>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_ಕೊನೆ

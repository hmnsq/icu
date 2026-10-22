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

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_ಮೌಲ್ಯ(ಧ್ರುವ uint8_t *pos, int32_t leadByte) {
    int32_t value;
    ಯದಿ(leadByte<k_Min_Two_Byte_ಮೌಲ್ಯ_Lead) {
        value=leadByte-k_Min_One_Byte_ಮೌಲ್ಯ_Lead;
    } ಅನ್ಯಥಾ ಯದಿ(leadByte<k_Min_Three_Byte_ಮೌಲ್ಯ_Lead) {
        value=((leadByte-k_Min_Two_Byte_ಮೌಲ್ಯ_Lead)<<8)|*pos;
    } ಅನ್ಯಥಾ ಯದಿ(leadByte<k_Four_Byte_ಮೌಲ್ಯ_Lead) {
        value=((leadByte-k_Min_Three_Byte_ಮೌಲ್ಯ_Lead)<<16)|(pos[0]<<8)|pos[1];
    } ಅನ್ಯಥಾ ಯದಿ(leadByte==k_Four_Byte_ಮೌಲ್ಯ_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ಅನ್ಯಥಾ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ಪ್ರತ್ಯಾಗಮ value;
}

ಧ್ರುವ uint8_t*
BytesTrie::jumpByDelta(ಧ್ರುವ uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ಯದಿ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ಅನ್ಯಥಾ ಯದಿ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ಅನ್ಯಥಾ ಯದಿ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ಅನ್ಯಥಾ ಯದಿ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ಅನ್ಯಥಾ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ಪ್ರತ್ಯಾಗಮ pos+delta;
}

UStringTrieResult
BytesTrie::current() ಧ್ರುವ {
    ಧ್ರುವ uint8_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
    } ಅನ್ಯಥಾ {
        int32_t ಗಂಟು;
        ಪ್ರತ್ಯಾಗಮ (remainingMatchLength_<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
    }
}

UStringTrieResult
BytesTrie::branchNext(ಧ್ರುವ uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ಯದಿ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ಯಾವತ್(length>k_Max_Branch_Linear_Sub_ಗಂಟು_Length) {
        ಯದಿ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ಅನ್ಯಥಾ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ಕುರು {
        ಯದಿ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t ಗಂಟು=*pos;
            U_ASSERT(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead);
            ಯದಿ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_ಮೌಲ್ಯ;
            } ಅನ್ಯಥಾ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ಗಂಟು>>=1;
                int32_t delta;
                ಯದಿ(ಗಂಟು<k_Min_Two_Byte_ಮೌಲ್ಯ_Lead) {
                    delta=ಗಂಟು-k_Min_One_Byte_ಮೌಲ್ಯ_Lead;
                } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Min_Three_Byte_ಮೌಲ್ಯ_Lead) {
                    delta=((ಗಂಟು-k_Min_Two_Byte_ಮೌಲ್ಯ_Lead)<<8)|*pos++;
                } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Four_Byte_ಮೌಲ್ಯ_Lead) {
                    delta=((ಗಂಟು-k_Min_Three_Byte_ಮೌಲ್ಯ_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು==k_Four_Byte_ಮೌಲ್ಯ_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ಅನ್ಯಥಾ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ಗಂಟು=*pos;
                result= ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead ? ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
            }
            pos_=pos;
            ಪ್ರತ್ಯಾಗಮ result;
        }
        --length;
        pos=skip_ಮೌಲ್ಯ(pos);
    } ಯಾವತ್(length>1);
    ಯದಿ(inByte==*pos++) {
        pos_=pos;
        int32_t ಗಂಟು=*pos;
        ಪ್ರತ್ಯಾಗಮ ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead ? ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
    } ಅನ್ಯಥಾ {
        stop();
        ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ಧ್ರುವ uint8_t *pos, int32_t inByte) {
    ಪ್ರತಿ(;;) {
        int32_t ಗಂಟು=*pos++;
        ಯದಿ(ಗಂಟು<kMinLinearMatch) {
            ಪ್ರತ್ಯಾಗಮ branchNext(pos, ಗಂಟು, inByte);
        } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ಗಂಟು-kMinLinearMatch;  // Actual match length minus 1.
            ಯದಿ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ಪ್ರತ್ಯಾಗಮ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                        ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
            } ಅನ್ಯಥಾ {
                // No match.
                ಭಙ್ಗ;
            }
        } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
            // No further matching bytes.
            ಭಙ್ಗ;
        } ಅನ್ಯಥಾ {
            // Skip intermediate value.
            pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_ಮೌಲ್ಯ_Lead);
        }
    }
    stop();
    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ಧ್ರುವ uint8_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
    }
    ಯದಿ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ಯದಿ(length>=0) {
        // Remaining part of a linear-match node.
        ಯದಿ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ಗಂಟು;
            ಪ್ರತ್ಯಾಗಮ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                    ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
        } ಅನ್ಯಥಾ {
            stop();
            ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
        }
    }
    ಪ್ರತ್ಯಾಗಮ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ಧ್ರುವ ವರ್ಣ *s, int32_t sLength) {
    ಯದಿ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ಪ್ರತ್ಯಾಗಮ current();
    }
    ಧ್ರುವ uint8_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ಪ್ರತಿ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ಯದಿ(sLength<0) {
            ಪ್ರತಿ(;;) {
                ಯದಿ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ಗಂಟು;
                    ಪ್ರತ್ಯಾಗಮ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                            ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
                }
                ಯದಿ(length<0) {
                    remainingMatchLength_=length;
                    ಭಙ್ಗ;
                }
                ಯದಿ(inByte!=*pos) {
                    stop();
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ಅನ್ಯಥಾ {
            ಪ್ರತಿ(;;) {
                ಯದಿ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ಗಂಟು;
                    ಪ್ರತ್ಯಾಗಮ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                            ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
                }
                inByte=*s++;
                --sLength;
                ಯದಿ(length<0) {
                    remainingMatchLength_=length;
                    ಭಙ್ಗ;
                }
                ಯದಿ(inByte!=*pos) {
                    stop();
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        ಪ್ರತಿ(;;) {
            int32_t ಗಂಟು=*pos++;
            ಯದಿ(ಗಂಟು<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ಗಂಟು, inByte);
                ಯದಿ(result==USTRINGTRIE_NO_MATCH) {
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ಯದಿ(sLength<0) {
                    ಯದಿ((inByte=*s++)==0) {
                        ಪ್ರತ್ಯಾಗಮ result;
                    }
                } ಅನ್ಯಥಾ {
                    ಯದಿ(sLength==0) {
                        ಪ್ರತ್ಯಾಗಮ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ಯದಿ(result==USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                    // No further matching bytes.
                    stop();
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
                // Match length+1 bytes.
                length=ಗಂಟು-kMinLinearMatch;  // Actual match length minus 1.
                ಯದಿ(inByte!=*pos) {
                    stop();
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                ಭಙ್ಗ;
            } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
                // No further matching bytes.
                stop();
                ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
            } ಅನ್ಯಥಾ {
                // Skip intermediate value.
                pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_ಮೌಲ್ಯ_Lead);
            }
        }
    }
}

ಧ್ರುವ uint8_t*
BytesTrie::find_Unique_ಮೌಲ್ಯ_From_Branch(ಧ್ರುವ uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_ಮೌಲ್ಯ,
                                     int32_t& unique_ಮೌಲ್ಯ) {
    ಯಾವತ್(length>k_Max_Branch_Linear_Sub_ಗಂಟು_Length) {
        ++pos;  // ignore the comparison byte
        ಯದಿ(nullptr==find_Unique_ಮೌಲ್ಯ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ಕುರು {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ಗಂಟು=*pos++;
        UBool isFinal = static_cast<UBool>(ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final);
        int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು>>1);
        pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
        ಯದಿ(isFinal) {
            ಯದಿ(have_Unique_ಮೌಲ್ಯ) {
                ಯದಿ(value!=unique_ಮೌಲ್ಯ) {
                    ಪ್ರತ್ಯಾಗಮ nullptr;
                }
            } ಅನ್ಯಥಾ {
                unique_ಮೌಲ್ಯ=value;
                have_Unique_ಮೌಲ್ಯ=ಸತ್ಯ;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ(!find_Unique_ಮೌಲ್ಯ(pos+value, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            have_Unique_ಮೌಲ್ಯ=ಸತ್ಯ;
        }
    } ಯಾವತ್(--length>1);
    ಪ್ರತ್ಯಾಗಮ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_ಮೌಲ್ಯ(ಧ್ರುವ uint8_t *pos, UBool have_Unique_ಮೌಲ್ಯ, int32_t &unique_ಮೌಲ್ಯ) {
    ಪ್ರತಿ(;;) {
        int32_t ಗಂಟು=*pos++;
        ಯದಿ(ಗಂಟು<kMinLinearMatch) {
            ಯದಿ(ಗಂಟು==0) {
                ಗಂಟು=*pos++;
            }
            pos=find_Unique_ಮೌಲ್ಯ_From_Branch(pos, ಗಂಟು+1, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ);
            ಯದಿ(pos==nullptr) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            have_Unique_ಮೌಲ್ಯ=ಸತ್ಯ;
        } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
            // linear-match node
            pos+=ಗಂಟು-kMinLinearMatch+1;  // Ignore the match bytes.
        } ಅನ್ಯಥಾ {
            UBool isFinal = static_cast<UBool>(ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final);
            int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು>>1);
            ಯದಿ(have_Unique_ಮೌಲ್ಯ) {
                ಯದಿ(value!=unique_ಮೌಲ್ಯ) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            } ಅನ್ಯಥಾ {
                unique_ಮೌಲ್ಯ=value;
                have_Unique_ಮೌಲ್ಯ=ಸತ್ಯ;
            }
            ಯದಿ(isFinal) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
            pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ಧ್ರುವ {
    ಧ್ರುವ uint8_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    int32_t ಗಂಟು=*pos++;
    ಯದಿ(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead) {
        ಯದಿ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
            ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ {
            pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
            ಗಂಟು=*pos++;
            U_ASSERT(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead);
        }
    }
    ಯದಿ(ಗಂಟು<kMinLinearMatch) {
        ಯದಿ(ಗಂಟು==0) {
            ಗಂಟು=*pos++;
        }
        getNextBranchBytes(pos, ++ಗಂಟು, out);
        ಪ್ರತ್ಯಾಗಮ ಗಂಟು;
    } ಅನ್ಯಥಾ {
        // First byte of the linear-match node.
        append(out, *pos);
        ಪ್ರತ್ಯಾಗಮ 1;
    }
}

ಶೂನ್ಯ
BytesTrie::getNextBranchBytes(ಧ್ರುವ uint8_t *pos, int32_t length, ByteSink &out) {
    ಯಾವತ್(length>k_Max_Branch_Linear_Sub_ಗಂಟು_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ಕುರು {
        append(out, *pos++);
        pos=skip_ಮೌಲ್ಯ(pos);
    } ಯಾವತ್(--length>1);
    append(out, *pos);
}

ಶೂನ್ಯ
BytesTrie::append(ByteSink &out, ಪೂರ್ಣಾಙ್ಕ c) {
    ವರ್ಣ ch = static_cast<ವರ್ಣ>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END

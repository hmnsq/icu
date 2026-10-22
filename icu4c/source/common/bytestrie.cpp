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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_மதிப்பு(மாறிலி uint8_t *pos, int32_t leadByte) {
    int32_t value;
    எனில்(leadByte<k_Min_Two_Byte_மதிப்பு_Lead) {
        value=leadByte-k_Min_One_Byte_மதிப்பு_Lead;
    } இல்லையெனில் எனில்(leadByte<k_Min_Three_Byte_மதிப்பு_Lead) {
        value=((leadByte-k_Min_Two_Byte_மதிப்பு_Lead)<<8)|*pos;
    } இல்லையெனில் எனில்(leadByte<k_Four_Byte_மதிப்பு_Lead) {
        value=((leadByte-k_Min_Three_Byte_மதிப்பு_Lead)<<16)|(pos[0]<<8)|pos[1];
    } இல்லையெனில் எனில்(leadByte==k_Four_Byte_மதிப்பு_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } இல்லையெனில் {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    திரும்பு value;
}

மாறிலி uint8_t*
BytesTrie::jumpByDelta(மாறிலி uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    எனில்(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } இல்லையெனில் எனில்(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } இல்லையெனில் எனில்(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } இல்லையெனில் எனில்(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } இல்லையெனில் {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    திரும்பு pos+delta;
}

UStringTrieResult
BytesTrie::current() மாறிலி {
    மாறிலி uint8_t *pos=pos_;
    எனில்(pos==nullptr) {
        திரும்பு USTRINGTRIE_NO_MATCH;
    } இல்லையெனில் {
        int32_t முனை;
        திரும்பு (remainingMatchLength_<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
    }
}

UStringTrieResult
BytesTrie::branchNext(மாறிலி uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    எனில்(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    போது(length>k_Max_Branch_Linear_Sub_முனை_Length) {
        எனில்(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } இல்லையெனில் {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    செய் {
        எனில்(inByte==*pos++) {
            UStringTrieResult result;
            int32_t முனை=*pos;
            U_ASSERT(முனை>=k_Min_மதிப்பு_Lead);
            எனில்(முனை&k_மதிப்பு_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_மதிப்பு;
            } இல்லையெனில் {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                முனை>>=1;
                int32_t delta;
                எனில்(முனை<k_Min_Two_Byte_மதிப்பு_Lead) {
                    delta=முனை-k_Min_One_Byte_மதிப்பு_Lead;
                } இல்லையெனில் எனில்(முனை<k_Min_Three_Byte_மதிப்பு_Lead) {
                    delta=((முனை-k_Min_Two_Byte_மதிப்பு_Lead)<<8)|*pos++;
                } இல்லையெனில் எனில்(முனை<k_Four_Byte_மதிப்பு_Lead) {
                    delta=((முனை-k_Min_Three_Byte_மதிப்பு_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } இல்லையெனில் எனில்(முனை==k_Four_Byte_மதிப்பு_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } இல்லையெனில் {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                முனை=*pos;
                result= முனை>=k_Min_மதிப்பு_Lead ? மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
            }
            pos_=pos;
            திரும்பு result;
        }
        --length;
        pos=skip_மதிப்பு(pos);
    } போது(length>1);
    எனில்(inByte==*pos++) {
        pos_=pos;
        int32_t முனை=*pos;
        திரும்பு முனை>=k_Min_மதிப்பு_Lead ? மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
    } இல்லையெனில் {
        stop();
        திரும்பு USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(மாறிலி uint8_t *pos, int32_t inByte) {
    க்கு(;;) {
        int32_t முனை=*pos++;
        எனில்(முனை<kMinLinearMatch) {
            திரும்பு branchNext(pos, முனை, inByte);
        } இல்லையெனில் எனில்(முனை<k_Min_மதிப்பு_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=முனை-kMinLinearMatch;  // Actual match length minus 1.
            எனில்(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                திரும்பு (length<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                        மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
            } இல்லையெனில் {
                // No match.
                நிறுத்து;
            }
        } இல்லையெனில் எனில்(முனை&k_மதிப்பு_Is_Final) {
            // No further matching bytes.
            நிறுத்து;
        } இல்லையெனில் {
            // Skip intermediate value.
            pos=skip_மதிப்பு(pos, முனை);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_மதிப்பு_Lead);
        }
    }
    stop();
    திரும்பு USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    மாறிலி uint8_t *pos=pos_;
    எனில்(pos==nullptr) {
        திரும்பு USTRINGTRIE_NO_MATCH;
    }
    எனில்(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    எனில்(length>=0) {
        // Remaining part of a linear-match node.
        எனில்(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t முனை;
            திரும்பு (length<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                    மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
        } இல்லையெனில் {
            stop();
            திரும்பு USTRINGTRIE_NO_MATCH;
        }
    }
    திரும்பு nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(மாறிலி எழுத்து *s, int32_t sLength) {
    எனில்(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        திரும்பு current();
    }
    மாறிலி uint8_t *pos=pos_;
    எனில்(pos==nullptr) {
        திரும்பு USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    க்கு(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        எனில்(sLength<0) {
            க்கு(;;) {
                எனில்((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t முனை;
                    திரும்பு (length<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                            மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
                }
                எனில்(length<0) {
                    remainingMatchLength_=length;
                    நிறுத்து;
                }
                எனில்(inByte!=*pos) {
                    stop();
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } இல்லையெனில் {
            க்கு(;;) {
                எனில்(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t முனை;
                    திரும்பு (length<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                            மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
                }
                inByte=*s++;
                --sLength;
                எனில்(length<0) {
                    remainingMatchLength_=length;
                    நிறுத்து;
                }
                எனில்(inByte!=*pos) {
                    stop();
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        க்கு(;;) {
            int32_t முனை=*pos++;
            எனில்(முனை<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, முனை, inByte);
                எனில்(result==USTRINGTRIE_NO_MATCH) {
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                எனில்(sLength<0) {
                    எனில்((inByte=*s++)==0) {
                        திரும்பு result;
                    }
                } இல்லையெனில் {
                    எனில்(sLength==0) {
                        திரும்பு result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                எனில்(result==USTRINGTRIE_FINAL_மதிப்பு) {
                    // No further matching bytes.
                    stop();
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } இல்லையெனில் எனில்(முனை<k_Min_மதிப்பு_Lead) {
                // Match length+1 bytes.
                length=முனை-kMinLinearMatch;  // Actual match length minus 1.
                எனில்(inByte!=*pos) {
                    stop();
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                நிறுத்து;
            } இல்லையெனில் எனில்(முனை&k_மதிப்பு_Is_Final) {
                // No further matching bytes.
                stop();
                திரும்பு USTRINGTRIE_NO_MATCH;
            } இல்லையெனில் {
                // Skip intermediate value.
                pos=skip_மதிப்பு(pos, முனை);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_மதிப்பு_Lead);
            }
        }
    }
}

மாறிலி uint8_t*
BytesTrie::find_Unique_மதிப்பு_From_Branch(மாறிலி uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_மதிப்பு,
                                     int32_t& unique_மதிப்பு) {
    போது(length>k_Max_Branch_Linear_Sub_முனை_Length) {
        ++pos;  // ignore the comparison byte
        எனில்(nullptr==find_Unique_மதிப்பு_From_Branch(jumpByDelta(pos), length>>1, have_Unique_மதிப்பு, unique_மதிப்பு)) {
            திரும்பு nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    செய் {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t முனை=*pos++;
        UBool isFinal = static_cast<UBool>(முனை & k_மதிப்பு_Is_Final);
        int32_t value=read_மதிப்பு(pos, முனை>>1);
        pos=skip_மதிப்பு(pos, முனை);
        எனில்(isFinal) {
            எனில்(have_Unique_மதிப்பு) {
                எனில்(value!=unique_மதிப்பு) {
                    திரும்பு nullptr;
                }
            } இல்லையெனில் {
                unique_மதிப்பு=value;
                have_Unique_மதிப்பு=உண்மை;
            }
        } இல்லையெனில் {
            எனில்(!find_Unique_மதிப்பு(pos+value, have_Unique_மதிப்பு, unique_மதிப்பு)) {
                திரும்பு nullptr;
            }
            have_Unique_மதிப்பு=உண்மை;
        }
    } போது(--length>1);
    திரும்பு pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_மதிப்பு(மாறிலி uint8_t *pos, UBool have_Unique_மதிப்பு, int32_t &unique_மதிப்பு) {
    க்கு(;;) {
        int32_t முனை=*pos++;
        எனில்(முனை<kMinLinearMatch) {
            எனில்(முனை==0) {
                முனை=*pos++;
            }
            pos=find_Unique_மதிப்பு_From_Branch(pos, முனை+1, have_Unique_மதிப்பு, unique_மதிப்பு);
            எனில்(pos==nullptr) {
                திரும்பு பொய்;
            }
            have_Unique_மதிப்பு=உண்மை;
        } இல்லையெனில் எனில்(முனை<k_Min_மதிப்பு_Lead) {
            // linear-match node
            pos+=முனை-kMinLinearMatch+1;  // Ignore the match bytes.
        } இல்லையெனில் {
            UBool isFinal = static_cast<UBool>(முனை & k_மதிப்பு_Is_Final);
            int32_t value=read_மதிப்பு(pos, முனை>>1);
            எனில்(have_Unique_மதிப்பு) {
                எனில்(value!=unique_மதிப்பு) {
                    திரும்பு பொய்;
                }
            } இல்லையெனில் {
                unique_மதிப்பு=value;
                have_Unique_மதிப்பு=உண்மை;
            }
            எனில்(isFinal) {
                திரும்பு உண்மை;
            }
            pos=skip_மதிப்பு(pos, முனை);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) மாறிலி {
    மாறிலி uint8_t *pos=pos_;
    எனில்(pos==nullptr) {
        திரும்பு 0;
    }
    எனில்(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        திரும்பு 1;
    }
    int32_t முனை=*pos++;
    எனில்(முனை>=k_Min_மதிப்பு_Lead) {
        எனில்(முனை&k_மதிப்பு_Is_Final) {
            திரும்பு 0;
        } இல்லையெனில் {
            pos=skip_மதிப்பு(pos, முனை);
            முனை=*pos++;
            U_ASSERT(முனை<k_Min_மதிப்பு_Lead);
        }
    }
    எனில்(முனை<kMinLinearMatch) {
        எனில்(முனை==0) {
            முனை=*pos++;
        }
        getNextBranchBytes(pos, ++முனை, out);
        திரும்பு முனை;
    } இல்லையெனில் {
        // First byte of the linear-match node.
        append(out, *pos);
        திரும்பு 1;
    }
}

வெறுமை
BytesTrie::getNextBranchBytes(மாறிலி uint8_t *pos, int32_t length, ByteSink &out) {
    போது(length>k_Max_Branch_Linear_Sub_முனை_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    செய் {
        append(out, *pos++);
        pos=skip_மதிப்பு(pos);
    } போது(--length>1);
    append(out, *pos);
}

வெறுமை
BytesTrie::append(ByteSink &out, முழு c) {
    எழுத்து ch = static_cast<எழுத்து>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END

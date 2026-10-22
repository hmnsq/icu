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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_मान(अचर uint8_t *pos, int32_t leadByte) {
    int32_t value;
    यदि(leadByte<k_Min_Two_Byte_मान_Lead) {
        value=leadByte-k_Min_One_Byte_मान_Lead;
    } अन्यथा यदि(leadByte<k_Min_Three_Byte_मान_Lead) {
        value=((leadByte-k_Min_Two_Byte_मान_Lead)<<8)|*pos;
    } अन्यथा यदि(leadByte<k_Four_Byte_मान_Lead) {
        value=((leadByte-k_Min_Three_Byte_मान_Lead)<<16)|(pos[0]<<8)|pos[1];
    } अन्यथा यदि(leadByte==k_Four_Byte_मान_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } अन्यथा {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    फर्काऊ value;
}

अचर uint8_t*
BytesTrie::jumpByDelta(अचर uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    यदि(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } अन्यथा यदि(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } अन्यथा यदि(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } अन्यथा यदि(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } अन्यथा {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    फर्काऊ pos+delta;
}

UStringTrieResult
BytesTrie::current() अचर {
    अचर uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        फर्काऊ USTRINGTRIE_NO_MATCH;
    } अन्यथा {
        int32_t गाँठो;
        फर्काऊ (remainingMatchLength_<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
    }
}

UStringTrieResult
BytesTrie::branchNext(अचर uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    यदि(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    जबसम्म(length>k_Max_Branch_Linear_Sub_गाँठो_Length) {
        यदि(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अन्यथा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    गर {
        यदि(inByte==*pos++) {
            UStringTrieResult result;
            int32_t गाँठो=*pos;
            U_ASSERT(गाँठो>=k_Min_मान_Lead);
            यदि(गाँठो&k_मान_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मान;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                गाँठो>>=1;
                int32_t delta;
                यदि(गाँठो<k_Min_Two_Byte_मान_Lead) {
                    delta=गाँठो-k_Min_One_Byte_मान_Lead;
                } अन्यथा यदि(गाँठो<k_Min_Three_Byte_मान_Lead) {
                    delta=((गाँठो-k_Min_Two_Byte_मान_Lead)<<8)|*pos++;
                } अन्यथा यदि(गाँठो<k_Four_Byte_मान_Lead) {
                    delta=((गाँठो-k_Min_Three_Byte_मान_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } अन्यथा यदि(गाँठो==k_Four_Byte_मान_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } अन्यथा {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                गाँठो=*pos;
                result= गाँठो>=k_Min_मान_Lead ? मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
            }
            pos_=pos;
            फर्काऊ result;
        }
        --length;
        pos=skip_मान(pos);
    } जबसम्म(length>1);
    यदि(inByte==*pos++) {
        pos_=pos;
        int32_t गाँठो=*pos;
        फर्काऊ गाँठो>=k_Min_मान_Lead ? मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
    } अन्यथा {
        stop();
        फर्काऊ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(अचर uint8_t *pos, int32_t inByte) {
    लागि(;;) {
        int32_t गाँठो=*pos++;
        यदि(गाँठो<kMinLinearMatch) {
            फर्काऊ branchNext(pos, गाँठो, inByte);
        } अन्यथा यदि(गाँठो<k_Min_मान_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=गाँठो-kMinLinearMatch;  // Actual match length minus 1.
            यदि(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                फर्काऊ (length<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                        मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
            } अन्यथा {
                // No match.
                रोक;
            }
        } अन्यथा यदि(गाँठो&k_मान_Is_Final) {
            // No further matching bytes.
            रोक;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_मान(pos, गाँठो);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_मान_Lead);
        }
    }
    stop();
    फर्काऊ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    अचर uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        फर्काऊ USTRINGTRIE_NO_MATCH;
    }
    यदि(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Remaining part of a linear-match node.
        यदि(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t गाँठो;
            फर्काऊ (length<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                    मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
        } अन्यथा {
            stop();
            फर्काऊ USTRINGTRIE_NO_MATCH;
        }
    }
    फर्काऊ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(अचर वर्ण *s, int32_t sLength) {
    यदि(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        फर्काऊ current();
    }
    अचर uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        फर्काऊ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    लागि(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        यदि(sLength<0) {
            लागि(;;) {
                यदि((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाँठो;
                    फर्काऊ (length<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                            मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
                }
                यदि(length<0) {
                    remainingMatchLength_=length;
                    रोक;
                }
                यदि(inByte!=*pos) {
                    stop();
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            लागि(;;) {
                यदि(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाँठो;
                    फर्काऊ (length<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                            मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
                }
                inByte=*s++;
                --sLength;
                यदि(length<0) {
                    remainingMatchLength_=length;
                    रोक;
                }
                यदि(inByte!=*pos) {
                    stop();
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        लागि(;;) {
            int32_t गाँठो=*pos++;
            यदि(गाँठो<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, गाँठो, inByte);
                यदि(result==USTRINGTRIE_NO_MATCH) {
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                यदि(sLength<0) {
                    यदि((inByte=*s++)==0) {
                        फर्काऊ result;
                    }
                } अन्यथा {
                    यदि(sLength==0) {
                        फर्काऊ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                यदि(result==USTRINGTRIE_FINAL_मान) {
                    // No further matching bytes.
                    stop();
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } अन्यथा यदि(गाँठो<k_Min_मान_Lead) {
                // Match length+1 bytes.
                length=गाँठो-kMinLinearMatch;  // Actual match length minus 1.
                यदि(inByte!=*pos) {
                    stop();
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                रोक;
            } अन्यथा यदि(गाँठो&k_मान_Is_Final) {
                // No further matching bytes.
                stop();
                फर्काऊ USTRINGTRIE_NO_MATCH;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_मान(pos, गाँठो);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_मान_Lead);
            }
        }
    }
}

अचर uint8_t*
BytesTrie::find_Unique_मान_From_Branch(अचर uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_मान,
                                     int32_t& unique_मान) {
    जबसम्म(length>k_Max_Branch_Linear_Sub_गाँठो_Length) {
        ++pos;  // ignore the comparison byte
        यदि(nullptr==find_Unique_मान_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मान, unique_मान)) {
            फर्काऊ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    गर {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t गाँठो=*pos++;
        UBool isFinal = static_cast<UBool>(गाँठो & k_मान_Is_Final);
        int32_t value=read_मान(pos, गाँठो>>1);
        pos=skip_मान(pos, गाँठो);
        यदि(isFinal) {
            यदि(have_Unique_मान) {
                यदि(value!=unique_मान) {
                    फर्काऊ nullptr;
                }
            } अन्यथा {
                unique_मान=value;
                have_Unique_मान=सत्य;
            }
        } अन्यथा {
            यदि(!find_Unique_मान(pos+value, have_Unique_मान, unique_मान)) {
                फर्काऊ nullptr;
            }
            have_Unique_मान=सत्य;
        }
    } जबसम्म(--length>1);
    फर्काऊ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_मान(अचर uint8_t *pos, UBool have_Unique_मान, int32_t &unique_मान) {
    लागि(;;) {
        int32_t गाँठो=*pos++;
        यदि(गाँठो<kMinLinearMatch) {
            यदि(गाँठो==0) {
                गाँठो=*pos++;
            }
            pos=find_Unique_मान_From_Branch(pos, गाँठो+1, have_Unique_मान, unique_मान);
            यदि(pos==nullptr) {
                फर्काऊ असत्य;
            }
            have_Unique_मान=सत्य;
        } अन्यथा यदि(गाँठो<k_Min_मान_Lead) {
            // linear-match node
            pos+=गाँठो-kMinLinearMatch+1;  // Ignore the match bytes.
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(गाँठो & k_मान_Is_Final);
            int32_t value=read_मान(pos, गाँठो>>1);
            यदि(have_Unique_मान) {
                यदि(value!=unique_मान) {
                    फर्काऊ असत्य;
                }
            } अन्यथा {
                unique_मान=value;
                have_Unique_मान=सत्य;
            }
            यदि(isFinal) {
                फर्काऊ सत्य;
            }
            pos=skip_मान(pos, गाँठो);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) अचर {
    अचर uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        फर्काऊ 0;
    }
    यदि(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        फर्काऊ 1;
    }
    int32_t गाँठो=*pos++;
    यदि(गाँठो>=k_Min_मान_Lead) {
        यदि(गाँठो&k_मान_Is_Final) {
            फर्काऊ 0;
        } अन्यथा {
            pos=skip_मान(pos, गाँठो);
            गाँठो=*pos++;
            U_ASSERT(गाँठो<k_Min_मान_Lead);
        }
    }
    यदि(गाँठो<kMinLinearMatch) {
        यदि(गाँठो==0) {
            गाँठो=*pos++;
        }
        getNextBranchBytes(pos, ++गाँठो, out);
        फर्काऊ गाँठो;
    } अन्यथा {
        // First byte of the linear-match node.
        append(out, *pos);
        फर्काऊ 1;
    }
}

शून्य
BytesTrie::getNextBranchBytes(अचर uint8_t *pos, int32_t length, ByteSink &out) {
    जबसम्म(length>k_Max_Branch_Linear_Sub_गाँठो_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    गर {
        append(out, *pos++);
        pos=skip_मान(pos);
    } जबसम्म(--length>1);
    append(out, *pos);
}

शून्य
BytesTrie::append(ByteSink &out, पूर्णाङ्क c) {
    वर्ण ch = static_cast<वर्ण>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END

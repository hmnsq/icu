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

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestream.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/uobject.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_نيلاي(ثابت uint8_t *pos, int32_t leadByte) {
    int32_t value;
    إذا(leadByte<k_Min_Two_Byte_نيلاي_Lead) {
        value=leadByte-k_Min_One_Byte_نيلاي_Lead;
    } وإلا إذا(leadByte<k_Min_Three_Byte_نيلاي_Lead) {
        value=((leadByte-k_Min_Two_Byte_نيلاي_Lead)<<8)|*pos;
    } وإلا إذا(leadByte<k_Four_Byte_نيلاي_Lead) {
        value=((leadByte-k_Min_Three_Byte_نيلاي_Lead)<<16)|(pos[0]<<8)|pos[1];
    } وإلا إذا(leadByte==k_Four_Byte_نيلاي_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } وإلا {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    أرجع value;
}

ثابت uint8_t*
BytesTrie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    إذا(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } وإلا إذا(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } وإلا إذا(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } وإلا إذا(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } وإلا {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    أرجع pos+delta;
}

UStringTrieResult
BytesTrie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    } وإلا {
        int32_t نود;
        أرجع (remainingMatchLength_<0 && (نود=*pos)>=k_Min_نيلاي_Lead) ?
                نيلاي_Result(نود) : USTRINGTRIE_NO_نيلاي;
    }
}

UStringTrieResult
BytesTrie::branchNext(ثابت uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_Linear_Sub_نود_Length) {
        إذا(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } وإلا {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    افعل {
        إذا(inByte==*pos++) {
            UStringTrieResult result;
            int32_t نود=*pos;
            U_ASSERT(نود>=k_Min_نيلاي_Lead);
            إذا(نود&k_نيلاي_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_نيلاي;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                نود>>=1;
                int32_t delta;
                إذا(نود<k_Min_Two_Byte_نيلاي_Lead) {
                    delta=نود-k_Min_One_Byte_نيلاي_Lead;
                } وإلا إذا(نود<k_Min_Three_Byte_نيلاي_Lead) {
                    delta=((نود-k_Min_Two_Byte_نيلاي_Lead)<<8)|*pos++;
                } وإلا إذا(نود<k_Four_Byte_نيلاي_Lead) {
                    delta=((نود-k_Min_Three_Byte_نيلاي_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } وإلا إذا(نود==k_Four_Byte_نيلاي_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } وإلا {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                نود=*pos;
                result= نود>=k_Min_نيلاي_Lead ? نيلاي_Result(نود) : USTRINGTRIE_NO_نيلاي;
            }
            pos_=pos;
            أرجع result;
        }
        --length;
        pos=skip_نيلاي(pos);
    } طالما(length>1);
    إذا(inByte==*pos++) {
        pos_=pos;
        int32_t نود=*pos;
        أرجع نود>=k_Min_نيلاي_Lead ? نيلاي_Result(نود) : USTRINGTRIE_NO_نيلاي;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ثابت uint8_t *pos, int32_t inByte) {
    لكل(;;) {
        int32_t نود=*pos++;
        إذا(نود<kMinLinearMatch) {
            أرجع branchNext(pos, نود, inByte);
        } وإلا إذا(نود<k_Min_نيلاي_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=نود-kMinLinearMatch;  // Actual match length minus 1.
            إذا(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                أرجع (length<0 && (نود=*pos)>=k_Min_نيلاي_Lead) ?
                        نيلاي_Result(نود) : USTRINGTRIE_NO_نيلاي;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(نود&k_نيلاي_Is_Final) {
            // No further matching bytes.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_نيلاي(pos, نود);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_نيلاي_Lead);
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ثابت uint8_t *pos=pos_;
    إذا(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    }
    إذا(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t نود;
            أرجع (length<0 && (نود=*pos)>=k_Min_نيلاي_Lead) ?
                    نيلاي_Result(نود) : USTRINGTRIE_NO_نيلاي;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_MATCH;
        }
    }
    أرجع nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ثابت حرف *s, int32_t sLength) {
    إذا(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        أرجع current();
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        إذا(sLength<0) {
            لكل(;;) {
                إذا((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t نود;
                    أرجع (length<0 && (نود=*pos)>=k_Min_نيلاي_Lead) ?
                            نيلاي_Result(نود) : USTRINGTRIE_NO_نيلاي;
                }
                إذا(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                إذا(inByte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t نود;
                    أرجع (length<0 && (نود=*pos)>=k_Min_نيلاي_Lead) ?
                            نيلاي_Result(نود) : USTRINGTRIE_NO_نيلاي;
                }
                inByte=*s++;
                --sLength;
                إذا(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                إذا(inByte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        لكل(;;) {
            int32_t نود=*pos++;
            إذا(نود<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, نود, inByte);
                إذا(result==USTRINGTRIE_NO_MATCH) {
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                إذا(sLength<0) {
                    إذا((inByte=*s++)==0) {
                        أرجع result;
                    }
                } وإلا {
                    إذا(sLength==0) {
                        أرجع result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                إذا(result==USTRINGTRIE_FINAL_نيلاي) {
                    // No further matching bytes.
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } وإلا إذا(نود<k_Min_نيلاي_Lead) {
                // Match length+1 bytes.
                length=نود-kMinLinearMatch;  // Actual match length minus 1.
                إذا(inByte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(نود&k_نيلاي_Is_Final) {
                // No further matching bytes.
                stop();
                أرجع USTRINGTRIE_NO_MATCH;
            } وإلا {
                // Skip intermediate value.
                pos=skip_نيلاي(pos, نود);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_نيلاي_Lead);
            }
        }
    }
}

ثابت uint8_t*
BytesTrie::find_Unique_نيلاي_From_Branch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_نيلاي,
                                     int32_t& unique_نيلاي) {
    طالما(length>k_Max_Branch_Linear_Sub_نود_Length) {
        ++pos;  // ignore the comparison byte
        إذا(nullptr==find_Unique_نيلاي_From_Branch(jumpByDelta(pos), length>>1, have_Unique_نيلاي, unique_نيلاي)) {
            أرجع nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t نود=*pos++;
        UBool isFinal = static_cast<UBool>(نود & k_نيلاي_Is_Final);
        int32_t value=read_نيلاي(pos, نود>>1);
        pos=skip_نيلاي(pos, نود);
        إذا(isFinal) {
            إذا(have_Unique_نيلاي) {
                إذا(value!=unique_نيلاي) {
                    أرجع nullptr;
                }
            } وإلا {
                unique_نيلاي=value;
                have_Unique_نيلاي=true;
            }
        } وإلا {
            إذا(!find_Unique_نيلاي(pos+value, have_Unique_نيلاي, unique_نيلاي)) {
                أرجع nullptr;
            }
            have_Unique_نيلاي=true;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_نيلاي(ثابت uint8_t *pos, UBool have_Unique_نيلاي, int32_t &unique_نيلاي) {
    لكل(;;) {
        int32_t نود=*pos++;
        إذا(نود<kMinLinearMatch) {
            إذا(نود==0) {
                نود=*pos++;
            }
            pos=find_Unique_نيلاي_From_Branch(pos, نود+1, have_Unique_نيلاي, unique_نيلاي);
            إذا(pos==nullptr) {
                أرجع خطأ;
            }
            have_Unique_نيلاي=true;
        } وإلا إذا(نود<k_Min_نيلاي_Lead) {
            // linear-match node
            pos+=نود-kMinLinearMatch+1;  // Ignore the match bytes.
        } وإلا {
            UBool isFinal = static_cast<UBool>(نود & k_نيلاي_Is_Final);
            int32_t value=read_نيلاي(pos, نود>>1);
            إذا(have_Unique_نيلاي) {
                إذا(value!=unique_نيلاي) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_نيلاي=value;
                have_Unique_نيلاي=true;
            }
            إذا(isFinal) {
                أرجع true;
            }
            pos=skip_نيلاي(pos, نود);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==nullptr) {
        أرجع 0;
    }
    إذا(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        أرجع 1;
    }
    int32_t نود=*pos++;
    إذا(نود>=k_Min_نيلاي_Lead) {
        إذا(نود&k_نيلاي_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_نيلاي(pos, نود);
            نود=*pos++;
            U_ASSERT(نود<k_Min_نيلاي_Lead);
        }
    }
    إذا(نود<kMinLinearMatch) {
        إذا(نود==0) {
            نود=*pos++;
        }
        getNextBranchBytes(pos, ++نود, out);
        أرجع نود;
    } وإلا {
        // First byte of the linear-match node.
        append(out, *pos);
        أرجع 1;
    }
}

فراغ
BytesTrie::getNextBranchBytes(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    طالما(length>k_Max_Branch_Linear_Sub_نود_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        append(out, *pos++);
        pos=skip_نيلاي(pos);
    } طالما(--length>1);
    append(out, *pos);
}

فراغ
BytesTrie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END

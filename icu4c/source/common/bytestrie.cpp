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
BytesTrie::read_قيمة(ثابت uint8_t *pos, int32_t leadByte) {
    int32_t value;
    إذا(leadByte<k_Min_Two_Byte_قيمة_Lead) {
        value=leadByte-k_Min_One_Byte_قيمة_Lead;
    } وإلا إذا(leadByte<k_Min_Three_Byte_قيمة_Lead) {
        value=((leadByte-k_Min_Two_Byte_قيمة_Lead)<<8)|*pos;
    } وإلا إذا(leadByte<k_Four_Byte_قيمة_Lead) {
        value=((leadByte-k_Min_Three_Byte_قيمة_Lead)<<16)|(pos[0]<<8)|pos[1];
    } وإلا إذا(leadByte==k_Four_Byte_قيمة_Lead) {
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
        int32_t عقدة;
        أرجع (remainingMatchLength_<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
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
    طالما(length>k_Max_Branch_Linear_Sub_عقدة_Length) {
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
            int32_t عقدة=*pos;
            U_ASSERT(عقدة>=k_Min_قيمة_Lead);
            إذا(عقدة&k_قيمة_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_قيمة;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                عقدة>>=1;
                int32_t delta;
                إذا(عقدة<k_Min_Two_Byte_قيمة_Lead) {
                    delta=عقدة-k_Min_One_Byte_قيمة_Lead;
                } وإلا إذا(عقدة<k_Min_Three_Byte_قيمة_Lead) {
                    delta=((عقدة-k_Min_Two_Byte_قيمة_Lead)<<8)|*pos++;
                } وإلا إذا(عقدة<k_Four_Byte_قيمة_Lead) {
                    delta=((عقدة-k_Min_Three_Byte_قيمة_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } وإلا إذا(عقدة==k_Four_Byte_قيمة_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } وإلا {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                عقدة=*pos;
                result= عقدة>=k_Min_قيمة_Lead ? قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
            }
            pos_=pos;
            أرجع result;
        }
        --length;
        pos=skip_قيمة(pos);
    } طالما(length>1);
    إذا(inByte==*pos++) {
        pos_=pos;
        int32_t عقدة=*pos;
        أرجع عقدة>=k_Min_قيمة_Lead ? قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ثابت uint8_t *pos, int32_t inByte) {
    لكل(;;) {
        int32_t عقدة=*pos++;
        إذا(عقدة<kMinLinearMatch) {
            أرجع branchNext(pos, عقدة, inByte);
        } وإلا إذا(عقدة<k_Min_قيمة_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=عقدة-kMinLinearMatch;  // Actual match length minus 1.
            إذا(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                أرجع (length<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                        قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(عقدة&k_قيمة_Is_Final) {
            // No further matching bytes.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_قيمة(pos, عقدة);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_قيمة_Lead);
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
            int32_t عقدة;
            أرجع (length<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                    قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
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
                    int32_t عقدة;
                    أرجع (length<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                            قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
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
                    int32_t عقدة;
                    أرجع (length<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                            قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
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
            int32_t عقدة=*pos++;
            إذا(عقدة<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, عقدة, inByte);
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
                إذا(result==USTRINGTRIE_FINAL_قيمة) {
                    // No further matching bytes.
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } وإلا إذا(عقدة<k_Min_قيمة_Lead) {
                // Match length+1 bytes.
                length=عقدة-kMinLinearMatch;  // Actual match length minus 1.
                إذا(inByte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(عقدة&k_قيمة_Is_Final) {
                // No further matching bytes.
                stop();
                أرجع USTRINGTRIE_NO_MATCH;
            } وإلا {
                // Skip intermediate value.
                pos=skip_قيمة(pos, عقدة);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_قيمة_Lead);
            }
        }
    }
}

ثابت uint8_t*
BytesTrie::find_Unique_قيمة_From_Branch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_قيمة,
                                     int32_t& unique_قيمة) {
    طالما(length>k_Max_Branch_Linear_Sub_عقدة_Length) {
        ++pos;  // ignore the comparison byte
        إذا(nullptr==find_Unique_قيمة_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قيمة, unique_قيمة)) {
            أرجع nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t عقدة=*pos++;
        UBool isFinal = static_cast<UBool>(عقدة & k_قيمة_Is_Final);
        int32_t value=read_قيمة(pos, عقدة>>1);
        pos=skip_قيمة(pos, عقدة);
        إذا(isFinal) {
            إذا(have_Unique_قيمة) {
                إذا(value!=unique_قيمة) {
                    أرجع nullptr;
                }
            } وإلا {
                unique_قيمة=value;
                have_Unique_قيمة=true;
            }
        } وإلا {
            إذا(!find_Unique_قيمة(pos+value, have_Unique_قيمة, unique_قيمة)) {
                أرجع nullptr;
            }
            have_Unique_قيمة=true;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_قيمة(ثابت uint8_t *pos, UBool have_Unique_قيمة, int32_t &unique_قيمة) {
    لكل(;;) {
        int32_t عقدة=*pos++;
        إذا(عقدة<kMinLinearMatch) {
            إذا(عقدة==0) {
                عقدة=*pos++;
            }
            pos=find_Unique_قيمة_From_Branch(pos, عقدة+1, have_Unique_قيمة, unique_قيمة);
            إذا(pos==nullptr) {
                أرجع خطأ;
            }
            have_Unique_قيمة=true;
        } وإلا إذا(عقدة<k_Min_قيمة_Lead) {
            // linear-match node
            pos+=عقدة-kMinLinearMatch+1;  // Ignore the match bytes.
        } وإلا {
            UBool isFinal = static_cast<UBool>(عقدة & k_قيمة_Is_Final);
            int32_t value=read_قيمة(pos, عقدة>>1);
            إذا(have_Unique_قيمة) {
                إذا(value!=unique_قيمة) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_قيمة=value;
                have_Unique_قيمة=true;
            }
            إذا(isFinal) {
                أرجع true;
            }
            pos=skip_قيمة(pos, عقدة);
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
    int32_t عقدة=*pos++;
    إذا(عقدة>=k_Min_قيمة_Lead) {
        إذا(عقدة&k_قيمة_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_قيمة(pos, عقدة);
            عقدة=*pos++;
            U_ASSERT(عقدة<k_Min_قيمة_Lead);
        }
    }
    إذا(عقدة<kMinLinearMatch) {
        إذا(عقدة==0) {
            عقدة=*pos++;
        }
        getNextBranchBytes(pos, ++عقدة, out);
        أرجع عقدة;
    } وإلا {
        // First byte of the linear-match node.
        append(out, *pos);
        أرجع 1;
    }
}

فراغ
BytesTrie::getNextBranchBytes(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    طالما(length>k_Max_Branch_Linear_Sub_عقدة_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        append(out, *pos++);
        pos=skip_قيمة(pos);
    } طالما(--length>1);
    append(out, *pos);
}

فراغ
BytesTrie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END

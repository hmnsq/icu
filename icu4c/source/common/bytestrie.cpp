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

بائٹاں_Trie::~بائٹاں_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
بائٹاں_Trie::read_قدر(ثابت uint8_t *pos, int32_t leadByte) {
    int32_t value;
    إذا(leadByte<k_Min_Two_Byte_قدر_Lead) {
        value=leadByte-k_Min_One_Byte_قدر_Lead;
    } وإلا إذا(leadByte<k_Min_Three_Byte_قدر_Lead) {
        value=((leadByte-k_Min_Two_Byte_قدر_Lead)<<8)|*pos;
    } وإلا إذا(leadByte<k_Four_Byte_قدر_Lead) {
        value=((leadByte-k_Min_Three_Byte_قدر_Lead)<<16)|(pos[0]<<8)|pos[1];
    } وإلا إذا(leadByte==k_Four_Byte_قدر_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } وإلا {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    أرجع value;
}

ثابت uint8_t*
بائٹاں_Trie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
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

UString_Trie_نتیجہ
بائٹاں_Trie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        أرجع USTRINGTRIE_NO_میل;
    } وإلا {
        int32_t گرہ;
        أرجع (بچیا_میل_لمائی<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
    }
}

UString_Trie_نتیجہ
بائٹاں_Trie::branch_اگلا(ثابت uint8_t *pos, int32_t length, int32_t وچ_Byte) {
    // Branch according to the current byte.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_خطی_Sub_گرہ_لمائی) {
        إذا(وچ_Byte<*pos++) {
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
        إذا(وچ_Byte==*pos++) {
            UString_Trie_نتیجہ نتیجہ;
            int32_t گرہ=*pos;
            U_ASSERT(گرہ>=k_Min_قدر_Lead);
            إذا(گرہ&k_قدر_Is_Final) {
                // Leave the final value for getValue() to read.
                نتیجہ=USTRINGTRIE_FINAL_قدر;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                گرہ>>=1;
                int32_t delta;
                إذا(گرہ<k_Min_Two_Byte_قدر_Lead) {
                    delta=گرہ-k_Min_One_Byte_قدر_Lead;
                } وإلا إذا(گرہ<k_Min_Three_Byte_قدر_Lead) {
                    delta=((گرہ-k_Min_Two_Byte_قدر_Lead)<<8)|*pos++;
                } وإلا إذا(گرہ<k_Four_Byte_قدر_Lead) {
                    delta=((گرہ-k_Min_Three_Byte_قدر_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } وإلا إذا(گرہ==k_Four_Byte_قدر_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } وإلا {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                گرہ=*pos;
                نتیجہ= گرہ>=k_Min_قدر_Lead ? قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
            }
            pos_=pos;
            أرجع نتیجہ;
        }
        --length;
        pos=skip_قدر(pos);
    } طالما(length>1);
    إذا(وچ_Byte==*pos++) {
        pos_=pos;
        int32_t گرہ=*pos;
        أرجع گرہ>=k_Min_قدر_Lead ? قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_میل;
    }
}

UString_Trie_نتیجہ
بائٹاں_Trie::اگلا_Impl(ثابت uint8_t *pos, int32_t وچ_Byte) {
    لكل(;;) {
        int32_t گرہ=*pos++;
        إذا(گرہ<k_Min_خطی_میل) {
            أرجع branch_اگلا(pos, گرہ, وچ_Byte);
        } وإلا إذا(گرہ<k_Min_قدر_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=گرہ-k_Min_خطی_میل;  // Actual match length minus 1.
            إذا(وچ_Byte==*pos++) {
                بچیا_میل_لمائی=--length;
                pos_=pos;
                أرجع (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                        قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(گرہ&k_قدر_Is_Final) {
            // No further matching bytes.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_قدر(pos, گرہ);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_قدر_Lead);
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_میل;
}

UString_Trie_نتیجہ
بائٹاں_Trie::next(int32_t وچ_Byte) {
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        أرجع USTRINGTRIE_NO_میل;
    }
    إذا(وچ_Byte<0) {
        وچ_Byte+=0x100;
    }
    int32_t length=بچیا_میل_لمائی;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(وچ_Byte==*pos++) {
            بچیا_میل_لمائی=--length;
            pos_=pos;
            int32_t گرہ;
            أرجع (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                    قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_میل;
        }
    }
    أرجع اگلا_Impl(pos, وچ_Byte);
}

UString_Trie_نتیجہ
بائٹاں_Trie::next(ثابت حرف *s, int32_t s_لمائی) {
    إذا(s_لمائی<0 ? *s==0 : s_لمائی==0) {
        // Empty input.
        أرجع current();
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        أرجع USTRINGTRIE_NO_میل;
    }
    int32_t length=بچیا_میل_لمائی;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t وچ_Byte;
        إذا(s_لمائی<0) {
            لكل(;;) {
                إذا((وچ_Byte=*s++)==0) {
                    بچیا_میل_لمائی=length;
                    pos_=pos;
                    int32_t گرہ;
                    أرجع (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
                }
                إذا(length<0) {
                    بچیا_میل_لمائی=length;
                    اكسر;
                }
                إذا(وچ_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_میل;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(s_لمائی==0) {
                    بچیا_میل_لمائی=length;
                    pos_=pos;
                    int32_t گرہ;
                    أرجع (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
                }
                وچ_Byte=*s++;
                --s_لمائی;
                إذا(length<0) {
                    بچیا_میل_لمائی=length;
                    اكسر;
                }
                إذا(وچ_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_میل;
                }
                ++pos;
                --length;
            }
        }
        لكل(;;) {
            int32_t گرہ=*pos++;
            إذا(گرہ<k_Min_خطی_میل) {
                UString_Trie_نتیجہ نتیجہ=branch_اگلا(pos, گرہ, وچ_Byte);
                إذا(نتیجہ==USTRINGTRIE_NO_میل) {
                    أرجع USTRINGTRIE_NO_میل;
                }
                // Fetch the next input byte, if there is one.
                إذا(s_لمائی<0) {
                    إذا((وچ_Byte=*s++)==0) {
                        أرجع نتیجہ;
                    }
                } وإلا {
                    إذا(s_لمائی==0) {
                        أرجع نتیجہ;
                    }
                    وچ_Byte=*s++;
                    --s_لمائی;
                }
                إذا(نتیجہ==USTRINGTRIE_FINAL_قدر) {
                    // No further matching bytes.
                    stop();
                    أرجع USTRINGTRIE_NO_میل;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } وإلا إذا(گرہ<k_Min_قدر_Lead) {
                // Match length+1 bytes.
                length=گرہ-k_Min_خطی_میل;  // Actual match length minus 1.
                إذا(وچ_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_میل;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(گرہ&k_قدر_Is_Final) {
                // No further matching bytes.
                stop();
                أرجع USTRINGTRIE_NO_میل;
            } وإلا {
                // Skip intermediate value.
                pos=skip_قدر(pos, گرہ);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_قدر_Lead);
            }
        }
    }
}

ثابت uint8_t*
بائٹاں_Trie::لبھو_Unique_قدر_From_Branch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_قدر,
                                     int32_t& unique_قدر) {
    طالما(length>k_Max_Branch_خطی_Sub_گرہ_لمائی) {
        ++pos;  // ignore the comparison byte
        إذا(خالی_سوچک==لبھو_Unique_قدر_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قدر, unique_قدر)) {
            أرجع خالی_سوچک;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t گرہ=*pos++;
        UBool isFinal = static_cast<UBool>(گرہ & k_قدر_Is_Final);
        int32_t value=read_قدر(pos, گرہ>>1);
        pos=skip_قدر(pos, گرہ);
        إذا(isFinal) {
            إذا(have_Unique_قدر) {
                إذا(value!=unique_قدر) {
                    أرجع خالی_سوچک;
                }
            } وإلا {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
        } وإلا {
            إذا(!لبھو_Unique_قدر(pos+value, have_Unique_قدر, unique_قدر)) {
                أرجع خالی_سوچک;
            }
            have_Unique_قدر=سچ;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison byte
}

UBool
بائٹاں_Trie::لبھو_Unique_قدر(ثابت uint8_t *pos, UBool have_Unique_قدر, int32_t &unique_قدر) {
    لكل(;;) {
        int32_t گرہ=*pos++;
        إذا(گرہ<k_Min_خطی_میل) {
            إذا(گرہ==0) {
                گرہ=*pos++;
            }
            pos=لبھو_Unique_قدر_From_Branch(pos, گرہ+1, have_Unique_قدر, unique_قدر);
            إذا(pos==خالی_سوچک) {
                أرجع خطأ;
            }
            have_Unique_قدر=سچ;
        } وإلا إذا(گرہ<k_Min_قدر_Lead) {
            // linear-match node
            pos+=گرہ-k_Min_خطی_میل+1;  // Ignore the match bytes.
        } وإلا {
            UBool isFinal = static_cast<UBool>(گرہ & k_قدر_Is_Final);
            int32_t value=read_قدر(pos, گرہ>>1);
            إذا(have_Unique_قدر) {
                إذا(value!=unique_قدر) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
            إذا(isFinal) {
                أرجع سچ;
            }
            pos=skip_قدر(pos, گرہ);
        }
    }
}

int32_t
بائٹاں_Trie::get_اگلا_بائٹاں(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        أرجع 0;
    }
    إذا(بچیا_میل_لمائی>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        أرجع 1;
    }
    int32_t گرہ=*pos++;
    إذا(گرہ>=k_Min_قدر_Lead) {
        إذا(گرہ&k_قدر_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_قدر(pos, گرہ);
            گرہ=*pos++;
            U_ASSERT(گرہ<k_Min_قدر_Lead);
        }
    }
    إذا(گرہ<k_Min_خطی_میل) {
        إذا(گرہ==0) {
            گرہ=*pos++;
        }
        get_اگلا_Branch_بائٹاں(pos, ++گرہ, out);
        أرجع گرہ;
    } وإلا {
        // First byte of the linear-match node.
        append(out, *pos);
        أرجع 1;
    }
}

فراغ
بائٹاں_Trie::get_اگلا_Branch_بائٹاں(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    طالما(length>k_Max_Branch_خطی_Sub_گرہ_لمائی) {
        ++pos;  // ignore the comparison byte
        get_اگلا_Branch_بائٹاں(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        append(out, *pos++);
        pos=skip_قدر(pos);
    } طالما(--length>1);
    append(out, *pos);
}

فراغ
بائٹاں_Trie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_سرا

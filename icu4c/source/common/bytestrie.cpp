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

بائيٽون_Trie::~بائيٽون_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
بائيٽون_Trie::read_قدر(ثابت uint8_t *pos, int32_t leadByte) {
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
بائيٽون_Trie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
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

UString_Trie_نتيجو
بائيٽون_Trie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        أرجع USTRINGTRIE_NO_ميلاپ;
    } وإلا {
        int32_t ڳنڍ;
        أرجع (بچيل_ميلاپ_ڊيگهائي<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
    }
}

UString_Trie_نتيجو
بائيٽون_Trie::branch_ايندڙ(ثابت uint8_t *pos, int32_t length, int32_t اندريون_Byte) {
    // Branch according to the current byte.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي) {
        إذا(اندريون_Byte<*pos++) {
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
        إذا(اندريون_Byte==*pos++) {
            UString_Trie_نتيجو نتيجو;
            int32_t ڳنڍ=*pos;
            U_ASSERT(ڳنڍ>=k_Min_قدر_Lead);
            إذا(ڳنڍ&k_قدر_Is_Final) {
                // Leave the final value for getValue() to read.
                نتيجو=USTRINGTRIE_FINAL_قدر;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ڳنڍ>>=1;
                int32_t delta;
                إذا(ڳنڍ<k_Min_Two_Byte_قدر_Lead) {
                    delta=ڳنڍ-k_Min_One_Byte_قدر_Lead;
                } وإلا إذا(ڳنڍ<k_Min_Three_Byte_قدر_Lead) {
                    delta=((ڳنڍ-k_Min_Two_Byte_قدر_Lead)<<8)|*pos++;
                } وإلا إذا(ڳنڍ<k_Four_Byte_قدر_Lead) {
                    delta=((ڳنڍ-k_Min_Three_Byte_قدر_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } وإلا إذا(ڳنڍ==k_Four_Byte_قدر_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } وإلا {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ڳنڍ=*pos;
                نتيجو= ڳنڍ>=k_Min_قدر_Lead ? قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
            }
            pos_=pos;
            أرجع نتيجو;
        }
        --length;
        pos=skip_قدر(pos);
    } طالما(length>1);
    إذا(اندريون_Byte==*pos++) {
        pos_=pos;
        int32_t ڳنڍ=*pos;
        أرجع ڳنڍ>=k_Min_قدر_Lead ? قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_ميلاپ;
    }
}

UString_Trie_نتيجو
بائيٽون_Trie::ايندڙ_Impl(ثابت uint8_t *pos, int32_t اندريون_Byte) {
    لكل(;;) {
        int32_t ڳنڍ=*pos++;
        إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
            أرجع branch_ايندڙ(pos, ڳنڍ, اندريون_Byte);
        } وإلا إذا(ڳنڍ<k_Min_قدر_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ڳنڍ-k_Min_لڪيري_ميلاپ;  // Actual match length minus 1.
            إذا(اندريون_Byte==*pos++) {
                بچيل_ميلاپ_ڊيگهائي=--length;
                pos_=pos;
                أرجع (length<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                        قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(ڳنڍ&k_قدر_Is_Final) {
            // No further matching bytes.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_قدر(pos, ڳنڍ);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_قدر_Lead);
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_ميلاپ;
}

UString_Trie_نتيجو
بائيٽون_Trie::next(int32_t اندريون_Byte) {
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        أرجع USTRINGTRIE_NO_ميلاپ;
    }
    إذا(اندريون_Byte<0) {
        اندريون_Byte+=0x100;
    }
    int32_t length=بچيل_ميلاپ_ڊيگهائي;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(اندريون_Byte==*pos++) {
            بچيل_ميلاپ_ڊيگهائي=--length;
            pos_=pos;
            int32_t ڳنڍ;
            أرجع (length<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                    قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_ميلاپ;
        }
    }
    أرجع ايندڙ_Impl(pos, اندريون_Byte);
}

UString_Trie_نتيجو
بائيٽون_Trie::next(ثابت حرف *s, int32_t s_ڊيگهائي) {
    إذا(s_ڊيگهائي<0 ? *s==0 : s_ڊيگهائي==0) {
        // Empty input.
        أرجع current();
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        أرجع USTRINGTRIE_NO_ميلاپ;
    }
    int32_t length=بچيل_ميلاپ_ڊيگهائي;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t اندريون_Byte;
        إذا(s_ڊيگهائي<0) {
            لكل(;;) {
                إذا((اندريون_Byte=*s++)==0) {
                    بچيل_ميلاپ_ڊيگهائي=length;
                    pos_=pos;
                    int32_t ڳنڍ;
                    أرجع (length<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
                }
                إذا(length<0) {
                    بچيل_ميلاپ_ڊيگهائي=length;
                    اكسر;
                }
                إذا(اندريون_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(s_ڊيگهائي==0) {
                    بچيل_ميلاپ_ڊيگهائي=length;
                    pos_=pos;
                    int32_t ڳنڍ;
                    أرجع (length<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
                }
                اندريون_Byte=*s++;
                --s_ڊيگهائي;
                إذا(length<0) {
                    بچيل_ميلاپ_ڊيگهائي=length;
                    اكسر;
                }
                إذا(اندريون_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                ++pos;
                --length;
            }
        }
        لكل(;;) {
            int32_t ڳنڍ=*pos++;
            إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
                UString_Trie_نتيجو نتيجو=branch_ايندڙ(pos, ڳنڍ, اندريون_Byte);
                إذا(نتيجو==USTRINGTRIE_NO_ميلاپ) {
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                // Fetch the next input byte, if there is one.
                إذا(s_ڊيگهائي<0) {
                    إذا((اندريون_Byte=*s++)==0) {
                        أرجع نتيجو;
                    }
                } وإلا {
                    إذا(s_ڊيگهائي==0) {
                        أرجع نتيجو;
                    }
                    اندريون_Byte=*s++;
                    --s_ڊيگهائي;
                }
                إذا(نتيجو==USTRINGTRIE_FINAL_قدر) {
                    // No further matching bytes.
                    stop();
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } وإلا إذا(ڳنڍ<k_Min_قدر_Lead) {
                // Match length+1 bytes.
                length=ڳنڍ-k_Min_لڪيري_ميلاپ;  // Actual match length minus 1.
                إذا(اندريون_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(ڳنڍ&k_قدر_Is_Final) {
                // No further matching bytes.
                stop();
                أرجع USTRINGTRIE_NO_ميلاپ;
            } وإلا {
                // Skip intermediate value.
                pos=skip_قدر(pos, ڳنڍ);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_قدر_Lead);
            }
        }
    }
}

ثابت uint8_t*
بائيٽون_Trie::ڳوليو_Unique_قدر_From_Branch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_قدر,
                                     int32_t& unique_قدر) {
    طالما(length>k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي) {
        ++pos;  // ignore the comparison byte
        إذا(خالي_اشارو==ڳوليو_Unique_قدر_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قدر, unique_قدر)) {
            أرجع خالي_اشارو;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ڳنڍ=*pos++;
        UBool isFinal = static_cast<UBool>(ڳنڍ & k_قدر_Is_Final);
        int32_t value=read_قدر(pos, ڳنڍ>>1);
        pos=skip_قدر(pos, ڳنڍ);
        إذا(isFinal) {
            إذا(have_Unique_قدر) {
                إذا(value!=unique_قدر) {
                    أرجع خالي_اشارو;
                }
            } وإلا {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
        } وإلا {
            إذا(!ڳوليو_Unique_قدر(pos+value, have_Unique_قدر, unique_قدر)) {
                أرجع خالي_اشارو;
            }
            have_Unique_قدر=سچ;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison byte
}

UBool
بائيٽون_Trie::ڳوليو_Unique_قدر(ثابت uint8_t *pos, UBool have_Unique_قدر, int32_t &unique_قدر) {
    لكل(;;) {
        int32_t ڳنڍ=*pos++;
        إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
            إذا(ڳنڍ==0) {
                ڳنڍ=*pos++;
            }
            pos=ڳوليو_Unique_قدر_From_Branch(pos, ڳنڍ+1, have_Unique_قدر, unique_قدر);
            إذا(pos==خالي_اشارو) {
                أرجع خطأ;
            }
            have_Unique_قدر=سچ;
        } وإلا إذا(ڳنڍ<k_Min_قدر_Lead) {
            // linear-match node
            pos+=ڳنڍ-k_Min_لڪيري_ميلاپ+1;  // Ignore the match bytes.
        } وإلا {
            UBool isFinal = static_cast<UBool>(ڳنڍ & k_قدر_Is_Final);
            int32_t value=read_قدر(pos, ڳنڍ>>1);
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
            pos=skip_قدر(pos, ڳنڍ);
        }
    }
}

int32_t
بائيٽون_Trie::get_ايندڙ_بائيٽون(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        أرجع 0;
    }
    إذا(بچيل_ميلاپ_ڊيگهائي>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        أرجع 1;
    }
    int32_t ڳنڍ=*pos++;
    إذا(ڳنڍ>=k_Min_قدر_Lead) {
        إذا(ڳنڍ&k_قدر_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_قدر(pos, ڳنڍ);
            ڳنڍ=*pos++;
            U_ASSERT(ڳنڍ<k_Min_قدر_Lead);
        }
    }
    إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
        إذا(ڳنڍ==0) {
            ڳنڍ=*pos++;
        }
        get_ايندڙ_Branch_بائيٽون(pos, ++ڳنڍ, out);
        أرجع ڳنڍ;
    } وإلا {
        // First byte of the linear-match node.
        append(out, *pos);
        أرجع 1;
    }
}

فراغ
بائيٽون_Trie::get_ايندڙ_Branch_بائيٽون(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    طالما(length>k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي) {
        ++pos;  // ignore the comparison byte
        get_ايندڙ_Branch_بائيٽون(jumpByDelta(pos), length>>1, out);
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
بائيٽون_Trie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_پڇاڙي

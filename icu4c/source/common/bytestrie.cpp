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

#شامل "unicode/utypes.h"
#شامل "unicode/bytestream.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/uobject.h"
#شامل "cmemory.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN

بائٹیں_Trie::~بائٹیں_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
بائٹیں_Trie::read_قدر(ثابت uint8_t *pos, int32_t leadByte) {
    int32_t value;
    اگر(leadByte<k_Min_Two_Byte_قدر_Lead) {
        value=leadByte-k_Min_One_Byte_قدر_Lead;
    } ورنہ اگر(leadByte<k_Min_Three_Byte_قدر_Lead) {
        value=((leadByte-k_Min_Two_Byte_قدر_Lead)<<8)|*pos;
    } ورنہ اگر(leadByte<k_Four_Byte_قدر_Lead) {
        value=((leadByte-k_Min_Three_Byte_قدر_Lead)<<16)|(pos[0]<<8)|pos[1];
    } ورنہ اگر(leadByte==k_Four_Byte_قدر_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ورنہ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    واپس value;
}

ثابت uint8_t*
بائٹیں_Trie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    اگر(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ورنہ اگر(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ورنہ اگر(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ورنہ اگر(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ورنہ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    واپس pos+delta;
}

UString_Trie_نتیجہ
بائٹیں_Trie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        واپس USTRINGTRIE_NO_مطابقت;
    } ورنہ {
        int32_t گرہ;
        واپس (بقایا_مطابقت_لمبائی<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
    }
}

UString_Trie_نتیجہ
بائٹیں_Trie::branch_اگلا(ثابت uint8_t *pos, int32_t length, int32_t اندر_Byte) {
    // Branch according to the current byte.
    اگر(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    جب(length>k_Max_Branch_خطی_Sub_گرہ_لمبائی) {
        اگر(اندر_Byte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ورنہ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    کرو {
        اگر(اندر_Byte==*pos++) {
            UString_Trie_نتیجہ نتیجہ;
            int32_t گرہ=*pos;
            U_ASSERT(گرہ>=k_Min_قدر_Lead);
            اگر(گرہ&k_قدر_Is_Final) {
                // Leave the final value for getValue() to read.
                نتیجہ=USTRINGTRIE_FINAL_قدر;
            } ورنہ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                گرہ>>=1;
                int32_t delta;
                اگر(گرہ<k_Min_Two_Byte_قدر_Lead) {
                    delta=گرہ-k_Min_One_Byte_قدر_Lead;
                } ورنہ اگر(گرہ<k_Min_Three_Byte_قدر_Lead) {
                    delta=((گرہ-k_Min_Two_Byte_قدر_Lead)<<8)|*pos++;
                } ورنہ اگر(گرہ<k_Four_Byte_قدر_Lead) {
                    delta=((گرہ-k_Min_Three_Byte_قدر_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ورنہ اگر(گرہ==k_Four_Byte_قدر_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ورنہ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                گرہ=*pos;
                نتیجہ= گرہ>=k_Min_قدر_Lead ? قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
            }
            pos_=pos;
            واپس نتیجہ;
        }
        --length;
        pos=skip_قدر(pos);
    } جب(length>1);
    اگر(اندر_Byte==*pos++) {
        pos_=pos;
        int32_t گرہ=*pos;
        واپس گرہ>=k_Min_قدر_Lead ? قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
    } ورنہ {
        stop();
        واپس USTRINGTRIE_NO_مطابقت;
    }
}

UString_Trie_نتیجہ
بائٹیں_Trie::اگلا_Impl(ثابت uint8_t *pos, int32_t اندر_Byte) {
    برائے(;;) {
        int32_t گرہ=*pos++;
        اگر(گرہ<k_Min_خطی_مطابقت) {
            واپس branch_اگلا(pos, گرہ, اندر_Byte);
        } ورنہ اگر(گرہ<k_Min_قدر_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=گرہ-k_Min_خطی_مطابقت;  // Actual match length minus 1.
            اگر(اندر_Byte==*pos++) {
                بقایا_مطابقت_لمبائی=--length;
                pos_=pos;
                واپس (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                        قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
            } ورنہ {
                // No match.
                توڑو;
            }
        } ورنہ اگر(گرہ&k_قدر_Is_Final) {
            // No further matching bytes.
            توڑو;
        } ورنہ {
            // Skip intermediate value.
            pos=skip_قدر(pos, گرہ);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_قدر_Lead);
        }
    }
    stop();
    واپس USTRINGTRIE_NO_مطابقت;
}

UString_Trie_نتیجہ
بائٹیں_Trie::next(int32_t اندر_Byte) {
    ثابت uint8_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        واپس USTRINGTRIE_NO_مطابقت;
    }
    اگر(اندر_Byte<0) {
        اندر_Byte+=0x100;
    }
    int32_t length=بقایا_مطابقت_لمبائی;  // Actual remaining match length minus 1.
    اگر(length>=0) {
        // Remaining part of a linear-match node.
        اگر(اندر_Byte==*pos++) {
            بقایا_مطابقت_لمبائی=--length;
            pos_=pos;
            int32_t گرہ;
            واپس (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                    قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
        } ورنہ {
            stop();
            واپس USTRINGTRIE_NO_مطابقت;
        }
    }
    واپس اگلا_Impl(pos, اندر_Byte);
}

UString_Trie_نتیجہ
بائٹیں_Trie::next(ثابت حرف *s, int32_t s_لمبائی) {
    اگر(s_لمبائی<0 ? *s==0 : s_لمبائی==0) {
        // Empty input.
        واپس current();
    }
    ثابت uint8_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        واپس USTRINGTRIE_NO_مطابقت;
    }
    int32_t length=بقایا_مطابقت_لمبائی;  // Actual remaining match length minus 1.
    برائے(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t اندر_Byte;
        اگر(s_لمبائی<0) {
            برائے(;;) {
                اگر((اندر_Byte=*s++)==0) {
                    بقایا_مطابقت_لمبائی=length;
                    pos_=pos;
                    int32_t گرہ;
                    واپس (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
                }
                اگر(length<0) {
                    بقایا_مطابقت_لمبائی=length;
                    توڑو;
                }
                اگر(اندر_Byte!=*pos) {
                    stop();
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                ++pos;
                --length;
            }
        } ورنہ {
            برائے(;;) {
                اگر(s_لمبائی==0) {
                    بقایا_مطابقت_لمبائی=length;
                    pos_=pos;
                    int32_t گرہ;
                    واپس (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
                }
                اندر_Byte=*s++;
                --s_لمبائی;
                اگر(length<0) {
                    بقایا_مطابقت_لمبائی=length;
                    توڑو;
                }
                اگر(اندر_Byte!=*pos) {
                    stop();
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                ++pos;
                --length;
            }
        }
        برائے(;;) {
            int32_t گرہ=*pos++;
            اگر(گرہ<k_Min_خطی_مطابقت) {
                UString_Trie_نتیجہ نتیجہ=branch_اگلا(pos, گرہ, اندر_Byte);
                اگر(نتیجہ==USTRINGTRIE_NO_مطابقت) {
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                // Fetch the next input byte, if there is one.
                اگر(s_لمبائی<0) {
                    اگر((اندر_Byte=*s++)==0) {
                        واپس نتیجہ;
                    }
                } ورنہ {
                    اگر(s_لمبائی==0) {
                        واپس نتیجہ;
                    }
                    اندر_Byte=*s++;
                    --s_لمبائی;
                }
                اگر(نتیجہ==USTRINGTRIE_FINAL_قدر) {
                    // No further matching bytes.
                    stop();
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ورنہ اگر(گرہ<k_Min_قدر_Lead) {
                // Match length+1 bytes.
                length=گرہ-k_Min_خطی_مطابقت;  // Actual match length minus 1.
                اگر(اندر_Byte!=*pos) {
                    stop();
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                ++pos;
                --length;
                توڑو;
            } ورنہ اگر(گرہ&k_قدر_Is_Final) {
                // No further matching bytes.
                stop();
                واپس USTRINGTRIE_NO_مطابقت;
            } ورنہ {
                // Skip intermediate value.
                pos=skip_قدر(pos, گرہ);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_قدر_Lead);
            }
        }
    }
}

ثابت uint8_t*
بائٹیں_Trie::ڈھونڈو_Unique_قدر_From_Branch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_قدر,
                                     int32_t& unique_قدر) {
    جب(length>k_Max_Branch_خطی_Sub_گرہ_لمبائی) {
        ++pos;  // ignore the comparison byte
        اگر(خالی_اشاریہ==ڈھونڈو_Unique_قدر_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قدر, unique_قدر)) {
            واپس خالی_اشاریہ;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    کرو {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t گرہ=*pos++;
        UBool isFinal = static_cast<UBool>(گرہ & k_قدر_Is_Final);
        int32_t value=read_قدر(pos, گرہ>>1);
        pos=skip_قدر(pos, گرہ);
        اگر(isFinal) {
            اگر(have_Unique_قدر) {
                اگر(value!=unique_قدر) {
                    واپس خالی_اشاریہ;
                }
            } ورنہ {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
        } ورنہ {
            اگر(!ڈھونڈو_Unique_قدر(pos+value, have_Unique_قدر, unique_قدر)) {
                واپس خالی_اشاریہ;
            }
            have_Unique_قدر=سچ;
        }
    } جب(--length>1);
    واپس pos+1;  // ignore the last comparison byte
}

UBool
بائٹیں_Trie::ڈھونڈو_Unique_قدر(ثابت uint8_t *pos, UBool have_Unique_قدر, int32_t &unique_قدر) {
    برائے(;;) {
        int32_t گرہ=*pos++;
        اگر(گرہ<k_Min_خطی_مطابقت) {
            اگر(گرہ==0) {
                گرہ=*pos++;
            }
            pos=ڈھونڈو_Unique_قدر_From_Branch(pos, گرہ+1, have_Unique_قدر, unique_قدر);
            اگر(pos==خالی_اشاریہ) {
                واپس جھوٹ;
            }
            have_Unique_قدر=سچ;
        } ورنہ اگر(گرہ<k_Min_قدر_Lead) {
            // linear-match node
            pos+=گرہ-k_Min_خطی_مطابقت+1;  // Ignore the match bytes.
        } ورنہ {
            UBool isFinal = static_cast<UBool>(گرہ & k_قدر_Is_Final);
            int32_t value=read_قدر(pos, گرہ>>1);
            اگر(have_Unique_قدر) {
                اگر(value!=unique_قدر) {
                    واپس جھوٹ;
                }
            } ورنہ {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
            اگر(isFinal) {
                واپس سچ;
            }
            pos=skip_قدر(pos, گرہ);
        }
    }
}

int32_t
بائٹیں_Trie::get_اگلا_بائٹیں(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        واپس 0;
    }
    اگر(بقایا_مطابقت_لمبائی>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        واپس 1;
    }
    int32_t گرہ=*pos++;
    اگر(گرہ>=k_Min_قدر_Lead) {
        اگر(گرہ&k_قدر_Is_Final) {
            واپس 0;
        } ورنہ {
            pos=skip_قدر(pos, گرہ);
            گرہ=*pos++;
            U_ASSERT(گرہ<k_Min_قدر_Lead);
        }
    }
    اگر(گرہ<k_Min_خطی_مطابقت) {
        اگر(گرہ==0) {
            گرہ=*pos++;
        }
        get_اگلا_Branch_بائٹیں(pos, ++گرہ, out);
        واپس گرہ;
    } ورنہ {
        // First byte of the linear-match node.
        append(out, *pos);
        واپس 1;
    }
}

خالی
بائٹیں_Trie::get_اگلا_Branch_بائٹیں(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    جب(length>k_Max_Branch_خطی_Sub_گرہ_لمبائی) {
        ++pos;  // ignore the comparison byte
        get_اگلا_Branch_بائٹیں(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    کرو {
        append(out, *pos++);
        pos=skip_قدر(pos);
    } جب(--length>1);
    append(out, *pos);
}

خالی
بائٹیں_Trie::append(ByteSink &out, صحیح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_انتہا

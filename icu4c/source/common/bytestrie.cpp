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

بایتها_Trie::~بایتها_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
بایتها_Trie::read_مقدار(ثابت uint8_t *pos, int32_t leadByte) {
    int32_t value;
    إذا(leadByte<k_Min_Two_Byte_مقدار_Lead) {
        value=leadByte-k_Min_One_Byte_مقدار_Lead;
    } وإلا إذا(leadByte<k_Min_Three_Byte_مقدار_Lead) {
        value=((leadByte-k_Min_Two_Byte_مقدار_Lead)<<8)|*pos;
    } وإلا إذا(leadByte<k_Four_Byte_مقدار_Lead) {
        value=((leadByte-k_Min_Three_Byte_مقدار_Lead)<<16)|(pos[0]<<8)|pos[1];
    } وإلا إذا(leadByte==k_Four_Byte_مقدار_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } وإلا {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    أرجع value;
}

ثابت uint8_t*
بایتها_Trie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
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

UString_Trie_نتیجه
بایتها_Trie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        أرجع USTRINGTRIE_NO_تطابق;
    } وإلا {
        int32_t گره;
        أرجع (مانده_تطابق_درازی<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
    }
}

UString_Trie_نتیجه
بایتها_Trie::branch_بعدی(ثابت uint8_t *pos, int32_t length, int32_t درون_Byte) {
    // Branch according to the current byte.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_خطی_Sub_گره_درازی) {
        إذا(درون_Byte<*pos++) {
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
        إذا(درون_Byte==*pos++) {
            UString_Trie_نتیجه نتیجه;
            int32_t گره=*pos;
            U_ASSERT(گره>=k_Min_مقدار_Lead);
            إذا(گره&k_مقدار_Is_Final) {
                // Leave the final value for getValue() to read.
                نتیجه=USTRINGTRIE_FINAL_مقدار;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                گره>>=1;
                int32_t delta;
                إذا(گره<k_Min_Two_Byte_مقدار_Lead) {
                    delta=گره-k_Min_One_Byte_مقدار_Lead;
                } وإلا إذا(گره<k_Min_Three_Byte_مقدار_Lead) {
                    delta=((گره-k_Min_Two_Byte_مقدار_Lead)<<8)|*pos++;
                } وإلا إذا(گره<k_Four_Byte_مقدار_Lead) {
                    delta=((گره-k_Min_Three_Byte_مقدار_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } وإلا إذا(گره==k_Four_Byte_مقدار_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } وإلا {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                گره=*pos;
                نتیجه= گره>=k_Min_مقدار_Lead ? مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
            }
            pos_=pos;
            أرجع نتیجه;
        }
        --length;
        pos=skip_مقدار(pos);
    } طالما(length>1);
    إذا(درون_Byte==*pos++) {
        pos_=pos;
        int32_t گره=*pos;
        أرجع گره>=k_Min_مقدار_Lead ? مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_تطابق;
    }
}

UString_Trie_نتیجه
بایتها_Trie::بعدی_Impl(ثابت uint8_t *pos, int32_t درون_Byte) {
    لكل(;;) {
        int32_t گره=*pos++;
        إذا(گره<k_Min_خطی_تطابق) {
            أرجع branch_بعدی(pos, گره, درون_Byte);
        } وإلا إذا(گره<k_Min_مقدار_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=گره-k_Min_خطی_تطابق;  // Actual match length minus 1.
            إذا(درون_Byte==*pos++) {
                مانده_تطابق_درازی=--length;
                pos_=pos;
                أرجع (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                        مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(گره&k_مقدار_Is_Final) {
            // No further matching bytes.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_مقدار(pos, گره);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_مقدار_Lead);
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_تطابق;
}

UString_Trie_نتیجه
بایتها_Trie::next(int32_t درون_Byte) {
    ثابت uint8_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        أرجع USTRINGTRIE_NO_تطابق;
    }
    إذا(درون_Byte<0) {
        درون_Byte+=0x100;
    }
    int32_t length=مانده_تطابق_درازی;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(درون_Byte==*pos++) {
            مانده_تطابق_درازی=--length;
            pos_=pos;
            int32_t گره;
            أرجع (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                    مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_تطابق;
        }
    }
    أرجع بعدی_Impl(pos, درون_Byte);
}

UString_Trie_نتیجه
بایتها_Trie::next(ثابت حرف *s, int32_t s_درازی) {
    إذا(s_درازی<0 ? *s==0 : s_درازی==0) {
        // Empty input.
        أرجع current();
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        أرجع USTRINGTRIE_NO_تطابق;
    }
    int32_t length=مانده_تطابق_درازی;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t درون_Byte;
        إذا(s_درازی<0) {
            لكل(;;) {
                إذا((درون_Byte=*s++)==0) {
                    مانده_تطابق_درازی=length;
                    pos_=pos;
                    int32_t گره;
                    أرجع (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                            مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
                }
                إذا(length<0) {
                    مانده_تطابق_درازی=length;
                    اكسر;
                }
                إذا(درون_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(s_درازی==0) {
                    مانده_تطابق_درازی=length;
                    pos_=pos;
                    int32_t گره;
                    أرجع (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                            مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
                }
                درون_Byte=*s++;
                --s_درازی;
                إذا(length<0) {
                    مانده_تطابق_درازی=length;
                    اكسر;
                }
                إذا(درون_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                ++pos;
                --length;
            }
        }
        لكل(;;) {
            int32_t گره=*pos++;
            إذا(گره<k_Min_خطی_تطابق) {
                UString_Trie_نتیجه نتیجه=branch_بعدی(pos, گره, درون_Byte);
                إذا(نتیجه==USTRINGTRIE_NO_تطابق) {
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                // Fetch the next input byte, if there is one.
                إذا(s_درازی<0) {
                    إذا((درون_Byte=*s++)==0) {
                        أرجع نتیجه;
                    }
                } وإلا {
                    إذا(s_درازی==0) {
                        أرجع نتیجه;
                    }
                    درون_Byte=*s++;
                    --s_درازی;
                }
                إذا(نتیجه==USTRINGTRIE_FINAL_مقدار) {
                    // No further matching bytes.
                    stop();
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } وإلا إذا(گره<k_Min_مقدار_Lead) {
                // Match length+1 bytes.
                length=گره-k_Min_خطی_تطابق;  // Actual match length minus 1.
                إذا(درون_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(گره&k_مقدار_Is_Final) {
                // No further matching bytes.
                stop();
                أرجع USTRINGTRIE_NO_تطابق;
            } وإلا {
                // Skip intermediate value.
                pos=skip_مقدار(pos, گره);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_مقدار_Lead);
            }
        }
    }
}

ثابت uint8_t*
بایتها_Trie::پیدا_کن_Unique_مقدار_From_Branch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_مقدار,
                                     int32_t& unique_مقدار) {
    طالما(length>k_Max_Branch_خطی_Sub_گره_درازی) {
        ++pos;  // ignore the comparison byte
        إذا(اشاره_تهی==پیدا_کن_Unique_مقدار_From_Branch(jumpByDelta(pos), length>>1, have_Unique_مقدار, unique_مقدار)) {
            أرجع اشاره_تهی;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t گره=*pos++;
        UBool isFinal = static_cast<UBool>(گره & k_مقدار_Is_Final);
        int32_t value=read_مقدار(pos, گره>>1);
        pos=skip_مقدار(pos, گره);
        إذا(isFinal) {
            إذا(have_Unique_مقدار) {
                إذا(value!=unique_مقدار) {
                    أرجع اشاره_تهی;
                }
            } وإلا {
                unique_مقدار=value;
                have_Unique_مقدار=درست;
            }
        } وإلا {
            إذا(!پیدا_کن_Unique_مقدار(pos+value, have_Unique_مقدار, unique_مقدار)) {
                أرجع اشاره_تهی;
            }
            have_Unique_مقدار=درست;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison byte
}

UBool
بایتها_Trie::پیدا_کن_Unique_مقدار(ثابت uint8_t *pos, UBool have_Unique_مقدار, int32_t &unique_مقدار) {
    لكل(;;) {
        int32_t گره=*pos++;
        إذا(گره<k_Min_خطی_تطابق) {
            إذا(گره==0) {
                گره=*pos++;
            }
            pos=پیدا_کن_Unique_مقدار_From_Branch(pos, گره+1, have_Unique_مقدار, unique_مقدار);
            إذا(pos==اشاره_تهی) {
                أرجع خطأ;
            }
            have_Unique_مقدار=درست;
        } وإلا إذا(گره<k_Min_مقدار_Lead) {
            // linear-match node
            pos+=گره-k_Min_خطی_تطابق+1;  // Ignore the match bytes.
        } وإلا {
            UBool isFinal = static_cast<UBool>(گره & k_مقدار_Is_Final);
            int32_t value=read_مقدار(pos, گره>>1);
            إذا(have_Unique_مقدار) {
                إذا(value!=unique_مقدار) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_مقدار=value;
                have_Unique_مقدار=درست;
            }
            إذا(isFinal) {
                أرجع درست;
            }
            pos=skip_مقدار(pos, گره);
        }
    }
}

int32_t
بایتها_Trie::get_بعدی_بایتها(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        أرجع 0;
    }
    إذا(مانده_تطابق_درازی>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        أرجع 1;
    }
    int32_t گره=*pos++;
    إذا(گره>=k_Min_مقدار_Lead) {
        إذا(گره&k_مقدار_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_مقدار(pos, گره);
            گره=*pos++;
            U_ASSERT(گره<k_Min_مقدار_Lead);
        }
    }
    إذا(گره<k_Min_خطی_تطابق) {
        إذا(گره==0) {
            گره=*pos++;
        }
        get_بعدی_Branch_بایتها(pos, ++گره, out);
        أرجع گره;
    } وإلا {
        // First byte of the linear-match node.
        append(out, *pos);
        أرجع 1;
    }
}

فراغ
بایتها_Trie::get_بعدی_Branch_بایتها(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    طالما(length>k_Max_Branch_خطی_Sub_گره_درازی) {
        ++pos;  // ignore the comparison byte
        get_بعدی_Branch_بایتها(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        append(out, *pos++);
        pos=skip_مقدار(pos);
    } طالما(--length>1);
    append(out, *pos);
}

فراغ
بایتها_Trie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_انتها

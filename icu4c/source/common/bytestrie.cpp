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

بایټونه_Trie::~بایټونه_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
بایټونه_Trie::read_ارزښت(ثابت uint8_t *pos, int32_t leadByte) {
    int32_t value;
    إذا(leadByte<k_Min_Two_Byte_ارزښت_Lead) {
        value=leadByte-k_Min_One_Byte_ارزښت_Lead;
    } وإلا إذا(leadByte<k_Min_Three_Byte_ارزښت_Lead) {
        value=((leadByte-k_Min_Two_Byte_ارزښت_Lead)<<8)|*pos;
    } وإلا إذا(leadByte<k_Four_Byte_ارزښت_Lead) {
        value=((leadByte-k_Min_Three_Byte_ارزښت_Lead)<<16)|(pos[0]<<8)|pos[1];
    } وإلا إذا(leadByte==k_Four_Byte_ارزښت_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } وإلا {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    أرجع value;
}

ثابت uint8_t*
بایټونه_Trie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
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

UString_Trie_پایله
بایټونه_Trie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        أرجع USTRINGTRIE_NO_سمون;
    } وإلا {
        int32_t غوټه;
        أرجع (پاتې_شوی_سمون_اوږدتیا<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
    }
}

UString_Trie_پایله
بایټونه_Trie::branch_بل(ثابت uint8_t *pos, int32_t length, int32_t دننه_Byte) {
    // Branch according to the current byte.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_خطي_Sub_غوټه_اوږدتیا) {
        إذا(دننه_Byte<*pos++) {
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
        إذا(دننه_Byte==*pos++) {
            UString_Trie_پایله پایله;
            int32_t غوټه=*pos;
            U_ASSERT(غوټه>=k_Min_ارزښت_Lead);
            إذا(غوټه&k_ارزښت_Is_Final) {
                // Leave the final value for getValue() to read.
                پایله=USTRINGTRIE_FINAL_ارزښت;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                غوټه>>=1;
                int32_t delta;
                إذا(غوټه<k_Min_Two_Byte_ارزښت_Lead) {
                    delta=غوټه-k_Min_One_Byte_ارزښت_Lead;
                } وإلا إذا(غوټه<k_Min_Three_Byte_ارزښت_Lead) {
                    delta=((غوټه-k_Min_Two_Byte_ارزښت_Lead)<<8)|*pos++;
                } وإلا إذا(غوټه<k_Four_Byte_ارزښت_Lead) {
                    delta=((غوټه-k_Min_Three_Byte_ارزښت_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } وإلا إذا(غوټه==k_Four_Byte_ارزښت_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } وإلا {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                غوټه=*pos;
                پایله= غوټه>=k_Min_ارزښت_Lead ? ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
            }
            pos_=pos;
            أرجع پایله;
        }
        --length;
        pos=skip_ارزښت(pos);
    } طالما(length>1);
    إذا(دننه_Byte==*pos++) {
        pos_=pos;
        int32_t غوټه=*pos;
        أرجع غوټه>=k_Min_ارزښت_Lead ? ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_سمون;
    }
}

UString_Trie_پایله
بایټونه_Trie::بل_Impl(ثابت uint8_t *pos, int32_t دننه_Byte) {
    لكل(;;) {
        int32_t غوټه=*pos++;
        إذا(غوټه<k_Min_خطي_سمون) {
            أرجع branch_بل(pos, غوټه, دننه_Byte);
        } وإلا إذا(غوټه<k_Min_ارزښت_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=غوټه-k_Min_خطي_سمون;  // Actual match length minus 1.
            إذا(دننه_Byte==*pos++) {
                پاتې_شوی_سمون_اوږدتیا=--length;
                pos_=pos;
                أرجع (length<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                        ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(غوټه&k_ارزښت_Is_Final) {
            // No further matching bytes.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_ارزښت(pos, غوټه);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_ارزښت_Lead);
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_سمون;
}

UString_Trie_پایله
بایټونه_Trie::next(int32_t دننه_Byte) {
    ثابت uint8_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        أرجع USTRINGTRIE_NO_سمون;
    }
    إذا(دننه_Byte<0) {
        دننه_Byte+=0x100;
    }
    int32_t length=پاتې_شوی_سمون_اوږدتیا;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(دننه_Byte==*pos++) {
            پاتې_شوی_سمون_اوږدتیا=--length;
            pos_=pos;
            int32_t غوټه;
            أرجع (length<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                    ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_سمون;
        }
    }
    أرجع بل_Impl(pos, دننه_Byte);
}

UString_Trie_پایله
بایټونه_Trie::next(ثابت حرف *s, int32_t s_اوږدتیا) {
    إذا(s_اوږدتیا<0 ? *s==0 : s_اوږدتیا==0) {
        // Empty input.
        أرجع current();
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        أرجع USTRINGTRIE_NO_سمون;
    }
    int32_t length=پاتې_شوی_سمون_اوږدتیا;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t دننه_Byte;
        إذا(s_اوږدتیا<0) {
            لكل(;;) {
                إذا((دننه_Byte=*s++)==0) {
                    پاتې_شوی_سمون_اوږدتیا=length;
                    pos_=pos;
                    int32_t غوټه;
                    أرجع (length<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                            ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
                }
                إذا(length<0) {
                    پاتې_شوی_سمون_اوږدتیا=length;
                    اكسر;
                }
                إذا(دننه_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_سمون;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(s_اوږدتیا==0) {
                    پاتې_شوی_سمون_اوږدتیا=length;
                    pos_=pos;
                    int32_t غوټه;
                    أرجع (length<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                            ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
                }
                دننه_Byte=*s++;
                --s_اوږدتیا;
                إذا(length<0) {
                    پاتې_شوی_سمون_اوږدتیا=length;
                    اكسر;
                }
                إذا(دننه_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_سمون;
                }
                ++pos;
                --length;
            }
        }
        لكل(;;) {
            int32_t غوټه=*pos++;
            إذا(غوټه<k_Min_خطي_سمون) {
                UString_Trie_پایله پایله=branch_بل(pos, غوټه, دننه_Byte);
                إذا(پایله==USTRINGTRIE_NO_سمون) {
                    أرجع USTRINGTRIE_NO_سمون;
                }
                // Fetch the next input byte, if there is one.
                إذا(s_اوږدتیا<0) {
                    إذا((دننه_Byte=*s++)==0) {
                        أرجع پایله;
                    }
                } وإلا {
                    إذا(s_اوږدتیا==0) {
                        أرجع پایله;
                    }
                    دننه_Byte=*s++;
                    --s_اوږدتیا;
                }
                إذا(پایله==USTRINGTRIE_FINAL_ارزښت) {
                    // No further matching bytes.
                    stop();
                    أرجع USTRINGTRIE_NO_سمون;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } وإلا إذا(غوټه<k_Min_ارزښت_Lead) {
                // Match length+1 bytes.
                length=غوټه-k_Min_خطي_سمون;  // Actual match length minus 1.
                إذا(دننه_Byte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_سمون;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(غوټه&k_ارزښت_Is_Final) {
                // No further matching bytes.
                stop();
                أرجع USTRINGTRIE_NO_سمون;
            } وإلا {
                // Skip intermediate value.
                pos=skip_ارزښت(pos, غوټه);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_ارزښت_Lead);
            }
        }
    }
}

ثابت uint8_t*
بایټونه_Trie::پیدا_کړه_Unique_ارزښت_From_Branch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_ارزښت,
                                     int32_t& unique_ارزښت) {
    طالما(length>k_Max_Branch_خطي_Sub_غوټه_اوږدتیا) {
        ++pos;  // ignore the comparison byte
        إذا(تش_اشاره==پیدا_کړه_Unique_ارزښت_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ارزښت, unique_ارزښت)) {
            أرجع تش_اشاره;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t غوټه=*pos++;
        UBool isFinal = static_cast<UBool>(غوټه & k_ارزښت_Is_Final);
        int32_t value=read_ارزښت(pos, غوټه>>1);
        pos=skip_ارزښت(pos, غوټه);
        إذا(isFinal) {
            إذا(have_Unique_ارزښت) {
                إذا(value!=unique_ارزښت) {
                    أرجع تش_اشاره;
                }
            } وإلا {
                unique_ارزښت=value;
                have_Unique_ارزښت=ریښتیا;
            }
        } وإلا {
            إذا(!پیدا_کړه_Unique_ارزښت(pos+value, have_Unique_ارزښت, unique_ارزښت)) {
                أرجع تش_اشاره;
            }
            have_Unique_ارزښت=ریښتیا;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison byte
}

UBool
بایټونه_Trie::پیدا_کړه_Unique_ارزښت(ثابت uint8_t *pos, UBool have_Unique_ارزښت, int32_t &unique_ارزښت) {
    لكل(;;) {
        int32_t غوټه=*pos++;
        إذا(غوټه<k_Min_خطي_سمون) {
            إذا(غوټه==0) {
                غوټه=*pos++;
            }
            pos=پیدا_کړه_Unique_ارزښت_From_Branch(pos, غوټه+1, have_Unique_ارزښت, unique_ارزښت);
            إذا(pos==تش_اشاره) {
                أرجع خطأ;
            }
            have_Unique_ارزښت=ریښتیا;
        } وإلا إذا(غوټه<k_Min_ارزښت_Lead) {
            // linear-match node
            pos+=غوټه-k_Min_خطي_سمون+1;  // Ignore the match bytes.
        } وإلا {
            UBool isFinal = static_cast<UBool>(غوټه & k_ارزښت_Is_Final);
            int32_t value=read_ارزښت(pos, غوټه>>1);
            إذا(have_Unique_ارزښت) {
                إذا(value!=unique_ارزښت) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_ارزښت=value;
                have_Unique_ارزښت=ریښتیا;
            }
            إذا(isFinal) {
                أرجع ریښتیا;
            }
            pos=skip_ارزښت(pos, غوټه);
        }
    }
}

int32_t
بایټونه_Trie::get_بل_بایټونه(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        أرجع 0;
    }
    إذا(پاتې_شوی_سمون_اوږدتیا>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        أرجع 1;
    }
    int32_t غوټه=*pos++;
    إذا(غوټه>=k_Min_ارزښت_Lead) {
        إذا(غوټه&k_ارزښت_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_ارزښت(pos, غوټه);
            غوټه=*pos++;
            U_ASSERT(غوټه<k_Min_ارزښت_Lead);
        }
    }
    إذا(غوټه<k_Min_خطي_سمون) {
        إذا(غوټه==0) {
            غوټه=*pos++;
        }
        get_بل_Branch_بایټونه(pos, ++غوټه, out);
        أرجع غوټه;
    } وإلا {
        // First byte of the linear-match node.
        append(out, *pos);
        أرجع 1;
    }
}

فراغ
بایټونه_Trie::get_بل_Branch_بایټونه(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    طالما(length>k_Max_Branch_خطي_Sub_غوټه_اوږدتیا) {
        ++pos;  // ignore the comparison byte
        get_بل_Branch_بایټونه(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        append(out, *pos++);
        pos=skip_ارزښت(pos);
    } طالما(--length>1);
    append(out, *pos);
}

فراغ
بایټونه_Trie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_پای_ټکی

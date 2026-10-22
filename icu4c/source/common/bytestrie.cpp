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

अष्टकानि_Trie::~अष्टकानि_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
अष्टकानि_Trie::read_मानम्(ध्रुव uint8_t *pos, int32_t leadByte) {
    int32_t value;
    यदि(leadByte<k_Min_Two_Byte_मानम्_Lead) {
        value=leadByte-k_Min_One_Byte_मानम्_Lead;
    } अन्यथा यदि(leadByte<k_Min_Three_Byte_मानम्_Lead) {
        value=((leadByte-k_Min_Two_Byte_मानम्_Lead)<<8)|*pos;
    } अन्यथा यदि(leadByte<k_Four_Byte_मानम्_Lead) {
        value=((leadByte-k_Min_Three_Byte_मानम्_Lead)<<16)|(pos[0]<<8)|pos[1];
    } अन्यथा यदि(leadByte==k_Four_Byte_मानम्_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } अन्यथा {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    प्रत्यागम value;
}

ध्रुव uint8_t*
अष्टकानि_Trie::jumpByDelta(ध्रुव uint8_t* pos U_LIFETIME_BOUND) {
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
    प्रत्यागम pos+delta;
}

UString_Trie_परिणाम
अष्टकानि_Trie::current() ध्रुव {
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        प्रत्यागम USTRINGTRIE_NO_मेलनम्;
    } अन्यथा {
        int32_t ग्रन्थिः;
        प्रत्यागम (remaining_मेलनम्_दीर्घता<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    }
}

UString_Trie_परिणाम
अष्टकानि_Trie::branch_अग्र(ध्रुव uint8_t *pos, int32_t length, int32_t अन्तर्_Byte) {
    // Branch according to the current byte.
    यदि(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    यावत्(length>k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता) {
        यदि(अन्तर्_Byte<*pos++) {
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
    कुरु {
        यदि(अन्तर्_Byte==*pos++) {
            UString_Trie_परिणाम परिणाम;
            int32_t ग्रन्थिः=*pos;
            U_ASSERT(ग्रन्थिः>=k_Min_मानम्_Lead);
            यदि(ग्रन्थिः&k_मानम्_Is_Final) {
                // Leave the final value for getValue() to read.
                परिणाम=USTRINGTRIE_FINAL_मानम्;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ग्रन्थिः>>=1;
                int32_t delta;
                यदि(ग्रन्थिः<k_Min_Two_Byte_मानम्_Lead) {
                    delta=ग्रन्थिः-k_Min_One_Byte_मानम्_Lead;
                } अन्यथा यदि(ग्रन्थिः<k_Min_Three_Byte_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Two_Byte_मानम्_Lead)<<8)|*pos++;
                } अन्यथा यदि(ग्रन्थिः<k_Four_Byte_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Three_Byte_मानम्_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } अन्यथा यदि(ग्रन्थिः==k_Four_Byte_मानम्_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } अन्यथा {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ग्रन्थिः=*pos;
                परिणाम= ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            }
            pos_=pos;
            प्रत्यागम परिणाम;
        }
        --length;
        pos=skip_मानम्(pos);
    } यावत्(length>1);
    यदि(अन्तर्_Byte==*pos++) {
        pos_=pos;
        int32_t ग्रन्थिः=*pos;
        प्रत्यागम ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    } अन्यथा {
        stop();
        प्रत्यागम USTRINGTRIE_NO_मेलनम्;
    }
}

UString_Trie_परिणाम
अष्टकानि_Trie::अग्र_Impl(ध्रुव uint8_t *pos, int32_t अन्तर्_Byte) {
    प्रति(;;) {
        int32_t ग्रन्थिः=*pos++;
        यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
            प्रत्यागम branch_अग्र(pos, ग्रन्थिः, अन्तर्_Byte);
        } अन्यथा यदि(ग्रन्थिः<k_Min_मानम्_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ग्रन्थिः-k_Min_रैखिकम्_मेलनम्;  // Actual match length minus 1.
            यदि(अन्तर्_Byte==*pos++) {
                remaining_मेलनम्_दीर्घता=--length;
                pos_=pos;
                प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                        मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            } अन्यथा {
                // No match.
                भङ्ग;
            }
        } अन्यथा यदि(ग्रन्थिः&k_मानम्_Is_Final) {
            // No further matching bytes.
            भङ्ग;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_मानम्(pos, ग्रन्थिः);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_मानम्_Lead);
        }
    }
    stop();
    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
}

UString_Trie_परिणाम
अष्टकानि_Trie::next(int32_t अन्तर्_Byte) {
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        प्रत्यागम USTRINGTRIE_NO_मेलनम्;
    }
    यदि(अन्तर्_Byte<0) {
        अन्तर्_Byte+=0x100;
    }
    int32_t length=remaining_मेलनम्_दीर्घता;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Remaining part of a linear-match node.
        यदि(अन्तर्_Byte==*pos++) {
            remaining_मेलनम्_दीर्घता=--length;
            pos_=pos;
            int32_t ग्रन्थिः;
            प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                    मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
        } अन्यथा {
            stop();
            प्रत्यागम USTRINGTRIE_NO_मेलनम्;
        }
    }
    प्रत्यागम अग्र_Impl(pos, अन्तर्_Byte);
}

UString_Trie_परिणाम
अष्टकानि_Trie::next(ध्रुव वर्ण *s, int32_t s_दीर्घता) {
    यदि(s_दीर्घता<0 ? *s==0 : s_दीर्घता==0) {
        // Empty input.
        प्रत्यागम current();
    }
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        प्रत्यागम USTRINGTRIE_NO_मेलनम्;
    }
    int32_t length=remaining_मेलनम्_दीर्घता;  // Actual remaining match length minus 1.
    प्रति(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t अन्तर्_Byte;
        यदि(s_दीर्घता<0) {
            प्रति(;;) {
                यदि((अन्तर्_Byte=*s++)==0) {
                    remaining_मेलनम्_दीर्घता=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                यदि(length<0) {
                    remaining_मेलनम्_दीर्घता=length;
                    भङ्ग;
                }
                यदि(अन्तर्_Byte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            प्रति(;;) {
                यदि(s_दीर्घता==0) {
                    remaining_मेलनम्_दीर्घता=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                अन्तर्_Byte=*s++;
                --s_दीर्घता;
                यदि(length<0) {
                    remaining_मेलनम्_दीर्घता=length;
                    भङ्ग;
                }
                यदि(अन्तर्_Byte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                ++pos;
                --length;
            }
        }
        प्रति(;;) {
            int32_t ग्रन्थिः=*pos++;
            यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
                UString_Trie_परिणाम परिणाम=branch_अग्र(pos, ग्रन्थिः, अन्तर्_Byte);
                यदि(परिणाम==USTRINGTRIE_NO_मेलनम्) {
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                // Fetch the next input byte, if there is one.
                यदि(s_दीर्घता<0) {
                    यदि((अन्तर्_Byte=*s++)==0) {
                        प्रत्यागम परिणाम;
                    }
                } अन्यथा {
                    यदि(s_दीर्घता==0) {
                        प्रत्यागम परिणाम;
                    }
                    अन्तर्_Byte=*s++;
                    --s_दीर्घता;
                }
                यदि(परिणाम==USTRINGTRIE_FINAL_मानम्) {
                    // No further matching bytes.
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } अन्यथा यदि(ग्रन्थिः<k_Min_मानम्_Lead) {
                // Match length+1 bytes.
                length=ग्रन्थिः-k_Min_रैखिकम्_मेलनम्;  // Actual match length minus 1.
                यदि(अन्तर्_Byte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                ++pos;
                --length;
                भङ्ग;
            } अन्यथा यदि(ग्रन्थिः&k_मानम्_Is_Final) {
                // No further matching bytes.
                stop();
                प्रत्यागम USTRINGTRIE_NO_मेलनम्;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_मानम्(pos, ग्रन्थिः);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_मानम्_Lead);
            }
        }
    }
}

ध्रुव uint8_t*
अष्टकानि_Trie::find_Unique_मानम्_From_Branch(ध्रुव uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_मानम्,
                                     int32_t& unique_मानम्) {
    यावत्(length>k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता) {
        ++pos;  // ignore the comparison byte
        यदि(शून्यसूचकः==find_Unique_मानम्_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मानम्, unique_मानम्)) {
            प्रत्यागम शून्यसूचकः;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    कुरु {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ग्रन्थिः=*pos++;
        UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
        int32_t value=read_मानम्(pos, ग्रन्थिः>>1);
        pos=skip_मानम्(pos, ग्रन्थिः);
        यदि(isFinal) {
            यदि(have_Unique_मानम्) {
                यदि(value!=unique_मानम्) {
                    प्रत्यागम शून्यसूचकः;
                }
            } अन्यथा {
                unique_मानम्=value;
                have_Unique_मानम्=सत्य;
            }
        } अन्यथा {
            यदि(!find_Unique_मानम्(pos+value, have_Unique_मानम्, unique_मानम्)) {
                प्रत्यागम शून्यसूचकः;
            }
            have_Unique_मानम्=सत्य;
        }
    } यावत्(--length>1);
    प्रत्यागम pos+1;  // ignore the last comparison byte
}

UBool
अष्टकानि_Trie::find_Unique_मानम्(ध्रुव uint8_t *pos, UBool have_Unique_मानम्, int32_t &unique_मानम्) {
    प्रति(;;) {
        int32_t ग्रन्थिः=*pos++;
        यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
            यदि(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=find_Unique_मानम्_From_Branch(pos, ग्रन्थिः+1, have_Unique_मानम्, unique_मानम्);
            यदि(pos==शून्यसूचकः) {
                प्रत्यागम असत्य;
            }
            have_Unique_मानम्=सत्य;
        } अन्यथा यदि(ग्रन्थिः<k_Min_मानम्_Lead) {
            // linear-match node
            pos+=ग्रन्थिः-k_Min_रैखिकम्_मेलनम्+1;  // Ignore the match bytes.
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
            int32_t value=read_मानम्(pos, ग्रन्थिः>>1);
            यदि(have_Unique_मानम्) {
                यदि(value!=unique_मानम्) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                unique_मानम्=value;
                have_Unique_मानम्=सत्य;
            }
            यदि(isFinal) {
                प्रत्यागम सत्य;
            }
            pos=skip_मानम्(pos, ग्रन्थिः);
        }
    }
}

int32_t
अष्टकानि_Trie::get_अग्र_अष्टकानि(ByteSink &out) ध्रुव {
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        प्रत्यागम 0;
    }
    यदि(remaining_मेलनम्_दीर्घता>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        प्रत्यागम 1;
    }
    int32_t ग्रन्थिः=*pos++;
    यदि(ग्रन्थिः>=k_Min_मानम्_Lead) {
        यदि(ग्रन्थिः&k_मानम्_Is_Final) {
            प्रत्यागम 0;
        } अन्यथा {
            pos=skip_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः=*pos++;
            U_ASSERT(ग्रन्थिः<k_Min_मानम्_Lead);
        }
    }
    यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
        यदि(ग्रन्थिः==0) {
            ग्रन्थिः=*pos++;
        }
        get_अग्र_Branch_अष्टकानि(pos, ++ग्रन्थिः, out);
        प्रत्यागम ग्रन्थिः;
    } अन्यथा {
        // First byte of the linear-match node.
        append(out, *pos);
        प्रत्यागम 1;
    }
}

शून्य
अष्टकानि_Trie::get_अग्र_Branch_अष्टकानि(ध्रुव uint8_t *pos, int32_t length, ByteSink &out) {
    यावत्(length>k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता) {
        ++pos;  // ignore the comparison byte
        get_अग्र_Branch_अष्टकानि(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    कुरु {
        append(out, *pos++);
        pos=skip_मानम्(pos);
    } यावत्(--length>1);
    append(out, *pos);
}

शून्य
अष्टकानि_Trie::append(ByteSink &out, पूर्णाङ्क c) {
    वर्ण ch = static_cast<वर्ण>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_अन्तः

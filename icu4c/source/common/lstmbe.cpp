// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل <complex>
#شامل <utility>

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "brkeng.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "lstmbe.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل "ubrkimpl.h"
#شامل "uresimp.h"
#شامل "uvectr32.h"
#شامل "uvector.h"

#شامل "unicode/brkiter.h"
#شامل "unicode/resbund.h"
#شامل "unicode/ubrk.h"
#شامل "unicode/uniset.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
جماعت ReadArray1D {
عوامی:
    مجازی ~ReadArray1D();
    مجازی int32_t d1() ثابت = 0;
    مجازی اعشاری get(int32_t i) ثابت = 0;

#اگر_تعریف_شدہ LSTM_DEBUG
    خالی print() ثابت {
        لکھو("\n[");
        برائے (int32_t i = 0; i < d1(); i++) {
           لکھو("%0.8e ", get(i));
           اگر (i % 4 == 3) لکھو("\n");
        }
        لکھو("]\n");
    }
#اگر_ختم
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
جماعت ReadArray2D {
عوامی:
    مجازی ~ReadArray2D();
    مجازی int32_t d1() ثابت = 0;
    مجازی int32_t d2() ثابت = 0;
    مجازی اعشاری get(int32_t i, int32_t j) ثابت = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
جماعت ConstArray1D : عوامی ReadArray1D {
عوامی:
    ConstArray1D() : ڈیٹا_2(خالی_اشاریہ), d1_(0) {}

    ConstArray1D(ثابت اعشاری* ڈیٹا, int32_t d1) : ڈیٹا_2(ڈیٹا), d1_(d1) {}

    مجازی ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    خالی init(ثابت int32_t* ڈیٹا, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        ڈیٹا_2 = reinterpret_cast<ثابت اعشاری*>(ڈیٹا);
        d1_ = d1;
    }

    // ReadArray1D methods.
    مجازی int32_t d1() ثابت override { واپس d1_; }
    مجازی اعشاری get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        واپس ڈیٹا_2[i];
    }

نجی:
    ثابت اعشاری* ڈیٹا_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
جماعت ConstArray2D : عوامی ReadArray2D {
عوامی:
    ConstArray2D() : ڈیٹا_2(خالی_اشاریہ), d1_(0), d2_(0) {}

    ConstArray2D(ثابت اعشاری* ڈیٹا, int32_t d1, int32_t d2)
        : ڈیٹا_2(ڈیٹا), d1_(d1), d2_(d2) {}

    مجازی ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    خالی init(ثابت int32_t* ڈیٹا, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        ڈیٹا_2 = reinterpret_cast<ثابت اعشاری*>(ڈیٹا);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    اندرونی int32_t d1() ثابت override { واپس d1_; }
    اندرونی int32_t d2() ثابت override { واپس d2_; }
    اعشاری get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        واپس ڈیٹا_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    اندرونی ConstArray1D صف(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        واپس ConstArray1D(ڈیٹا_2 + i * d2_, d2_);
    }

نجی:
    ثابت اعشاری* ڈیٹا_2;
    int32_t d1_;
    int32_t d2_;
};

ConstArray2D::~ConstArray2D()
{
}

/**
 * A class to allocate data as a writable 1D array.
 * This is the main class implement matrix operation.
 */
جماعت Array1D : عوامی ReadArray1D {
عوامی:
    Array1D() : حافظہ_2(خالی_اشاریہ), ڈیٹا_2(خالی_اشاریہ), d1_(0) {}
    Array1D(int32_t d1, UError_رمز &حالت_2)
        : حافظہ_2(uprv_malloc(d1 * حجم(اعشاری))),
          ڈیٹا_2(static_cast<اعشاری*>(حافظہ_2)), d1_(d1) {
        اگر (U_SUCCESS(حالت_2)) {
            اگر (حافظہ_2 == خالی_اشاریہ) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            clear();
        }
    }

    مجازی ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(اعشاری* ڈیٹا, int32_t d1)
        : حافظہ_2(خالی_اشاریہ), ڈیٹا_2(ڈیٹا), d1_(d1) {}

    // ReadArray1D methods.
    مجازی int32_t d1() ثابت override { واپس d1_; }
    مجازی اعشاری get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        واپس ڈیٹا_2[i];
    }

    // Return the index which point to the max data in the array.
    اندرونی int32_t max_اشاریہ() ثابت {
        int32_t اشاریہ = 0;
        اعشاری max = ڈیٹا_2[0];
        برائے (int32_t i = 1; i < d1_; i++) {
            اگر (ڈیٹا_2[i] > max) {
                max = ڈیٹا_2[i];
                اشاریہ = i;
            }
        }
        واپس اشاریہ;
    }

    // Slice part of the array to a new one.
    اندرونی Array1D قاش(int32_t from, int32_t size) ثابت {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        واپس Array1D(ڈیٹا_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    اندرونی Array1D& جوڑو_Dot_حاصل_ضرب(ثابت ReadArray1D& a, ثابت ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        برائے (int32_t i = 0; i < d1(); i++) {
            برائے (int32_t j = 0; j < a.d1(); j++) {
                ڈیٹا_2[i] += a.get(j) * b.get(j, i);
            }
        }
        واپس *یہ;
    }

    // Hadamard Product the values of another array of the same size into this one.
    اندرونی Array1D& hadamard_حاصل_ضرب(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        برائے (int32_t i = 0; i < d1(); i++) {
            ڈیٹا_2[i] *= a.get(i);
        }
        واپس *یہ;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    اندرونی Array1D& جوڑو_Hadamard_حاصل_ضرب(ثابت ReadArray1D& a, ثابت ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        برائے (int32_t i = 0; i < d1(); i++) {
            ڈیٹا_2[i] += a.get(i) * b.get(i);
        }
        واپس *یہ;
    }

    // Add the values of another array of the same size into this one.
    اندرونی Array1D& جوڑو(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        برائے (int32_t i = 0; i < d1(); i++) {
            ڈیٹا_2[i] += a.get(i);
        }
        واپس *یہ;
    }

    // Assign the values of another array of the same size into this one.
    اندرونی Array1D& assign(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        برائے (int32_t i = 0; i < d1(); i++) {
            ڈیٹا_2[i] = a.get(i);
        }
        واپس *یہ;
    }

    // Apply tanh to all the elements in the array.
    اندرونی Array1D& tanh() {
        واپس tanh(*یہ);
    }

    // Apply tanh of a and store into this array.
    اندرونی Array1D& tanh(ثابت Array1D& a) {
        U_ASSERT(a.d1() == d1());
        برائے (int32_t i = 0; i < d1_; i++) {
            ڈیٹا_2[i] = std::tanh(a.get(i));
        }
        واپس *یہ;
    }

    // Apply sigmoid to all the elements in the array.
    اندرونی Array1D& sigmoid() {
        برائے (int32_t i = 0; i < d1_; i++) {
            ڈیٹا_2[i] = 1.0f/(1.0f + expf(-ڈیٹا_2[i]));
        }
        واپس *یہ;
    }

    اندرونی Array1D& clear() {
        uprv_memset(ڈیٹا_2, 0, d1_ * حجم(اعشاری));
        واپس *یہ;
    }

نجی:
    خالی* حافظہ_2;
    اعشاری* ڈیٹا_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(حافظہ_2);
}

جماعت Array2D : عوامی ReadArray2D {
عوامی:
    Array2D() : حافظہ_2(خالی_اشاریہ), ڈیٹا_2(خالی_اشاریہ), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_رمز &حالت_2)
        : حافظہ_2(uprv_malloc(d1 * d2 * حجم(اعشاری))),
          ڈیٹا_2(static_cast<اعشاری*>(حافظہ_2)), d1_(d1), d2_(d2) {
        اگر (U_SUCCESS(حالت_2)) {
            اگر (حافظہ_2 == خالی_اشاریہ) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            clear();
        }
    }
    مجازی ~Array2D();

    // ReadArray2D methods.
    مجازی int32_t d1() ثابت override { واپس d1_; }
    مجازی int32_t d2() ثابت override { واپس d2_; }
    مجازی اعشاری get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        واپس ڈیٹا_2[i * d2_ + j];
    }

    اندرونی Array1D صف(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        واپس Array1D(ڈیٹا_2 + i * d2_, d2_);
    }

    اندرونی Array2D& clear() {
        uprv_memset(ڈیٹا_2, 0, d1_ * d2_ * حجم(اعشاری));
        واپس *یہ;
    }

نجی:
    خالی* حافظہ_2;
    اعشاری* ڈیٹا_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(حافظہ_2);
}

قسمتعریف شمار {
    BEGIN,
    INSIDE,
    انتہا_2,
    SINGLE
} LSTMClass;

قسمتعریف شمار {
    UNKNOWN,
    رمز_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

ساخت LSTMData : عوامی UMemory {
    LSTMData(UResourceBundle* rb, UError_رمز &حالت_2);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ثابت char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

نجی:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UError_رمز &حالت_2)
    : fDict(خالی_اشاریہ), fType(UNKNOWN), fName(خالی_اشاریہ),
      fBundle(rb)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (IEEE_754 != 1) {
        حالت_2 = U_UNSUPPORTED_ERROR;
        واپس;
    }
    Local_UResource_Bundle_اشارہ_گر embeddings_res(
        ures_get_By_کلید(rb, "embeddings", خالی_اشاریہ, &حالت_2));
    int32_t embedding_سائز = ures_getInt(embeddings_res.getAlias(), &حالت_2);
    Local_UResource_Bundle_اشارہ_گر hunits_res(
        ures_get_By_کلید(rb, "hunits", خالی_اشاریہ, &حالت_2));
    اگر (U_FAILURE(حالت_2)) واپس;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &حالت_2);
    ثابت char16_t* type = ures_get_String_By_کلید(rb, "type", خالی_اشاریہ, &حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس;
    اگر (u_strCompare(type, -1, u"codepoints", -1, جھوٹ) == 0) {
        fType = رمز_POINTS;
    } ورنہ اگر (u_strCompare(type, -1, u"graphclust", -1, جھوٹ) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_کلید(rb, "model", خالی_اشاریہ, &حالت_2);
    Local_UResource_Bundle_اشارہ_گر ڈیٹا_Res(ures_get_By_کلید(rb, "data", خالی_اشاریہ, &حالت_2));
    اگر (U_FAILURE(حالت_2)) واپس;
    int32_t ڈیٹا_len = 0;
    ثابت int32_t* ڈیٹا = ures_get_Int_سمتیہ(ڈیٹا_Res.getAlias(), &ڈیٹا_len, &حالت_2);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, خالی_اشاریہ, &حالت_2);

    ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
    Resource_ڈیٹا_قدر value;
    ures_get_قدر_With_Fallback(rb, "dict", ڈھیر_عارضی_Bundle.getAlias(), value, حالت_2);
    ResourceArray stringArray = value.getArray(حالت_2);
    int32_t عدد_اشاریہ = stringArray.get_سائز();
    اگر (U_FAILURE(حالت_2)) { واپس; }

    // put dict into hash
    int32_t string_لمبائی;
    برائے (int32_t idx = 0; idx < عدد_اشاریہ; idx++) {
        stringArray.get_قدر(idx, value);
        ثابت char16_t* str = value.getString(string_لمبائی, حالت_2);
        uhash_putiAllowZero(fDict, (خالی*)str, idx, &حالت_2);
        اگر (U_FAILURE(حالت_2)) واپس;
#اگر_تعریف_شدہ LSTM_VECTORIZER_DEBUG
        لکھو("Assign [");
        جب (*str != 0x0000) {
            لکھو("U+%04x ", *str);
            str++;
        }
        لکھو("] map to %d\n", idx-1);
#اگر_ختم
    }
    int32_t mat1_سائز = (عدد_اشاریہ + 1) * embedding_سائز;
    int32_t mat2_سائز = embedding_سائز * 4 * hunits;
    int32_t mat3_سائز = hunits * 4 * hunits;
    int32_t mat4_سائز = 4 * hunits;
    int32_t mat5_سائز = mat2_سائز;
    int32_t mat6_سائز = mat3_سائز;
    int32_t mat7_سائز = mat4_سائز;
    int32_t mat8_سائز = 2 * hunits * 4;
#اگر U_DEBUG
    int32_t mat9_سائز = 4;
    U_ASSERT(ڈیٹا_len == mat1_سائز + mat2_سائز + mat3_سائز + mat4_سائز + mat5_سائز +
        mat6_سائز + mat7_سائز + mat8_سائز + mat9_سائز);
#اگر_ختم

    fEmbedding.init(ڈیٹا, (عدد_اشاریہ + 1), embedding_سائز);
    ڈیٹا += mat1_سائز;
    fForwardW.init(ڈیٹا, embedding_سائز, 4 * hunits);
    ڈیٹا += mat2_سائز;
    fForwardU.init(ڈیٹا, hunits, 4 * hunits);
    ڈیٹا += mat3_سائز;
    fForwardB.init(ڈیٹا, 4 * hunits);
    ڈیٹا += mat4_سائز;
    fBackwardW.init(ڈیٹا, embedding_سائز, 4 * hunits);
    ڈیٹا += mat5_سائز;
    fBackwardU.init(ڈیٹا, hunits, 4 * hunits);
    ڈیٹا += mat6_سائز;
    fBackwardB.init(ڈیٹا, 4 * hunits);
    ڈیٹا += mat7_سائز;
    fOutputW.init(ڈیٹا, 2 * hunits, 4);
    ڈیٹا += mat8_سائز;
    fOutputB.init(ڈیٹا, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

جماعت Vectorizer : عوامی UMemory {
عوامی:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    مجازی ~Vectorizer();
    مجازی خالی vectorize(UText *متن, int32_t آغاز_Pos, int32_t انتہا_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_رمز &حالت_2) ثابت = 0;
محفوظ:
    int32_t string_مقصد_اشاریہ(ثابت char16_t* str) ثابت {
        UBool ملا = جھوٹ;
        int32_t ret = uhash_geti_And_ملا(fDict, (ثابت خالی*)str, &ملا);
        اگر (!ملا) {
            ret = fDict->count;
        }
#اگر_تعریف_شدہ LSTM_VECTORIZER_DEBUG
        لکھو("[");
        جب (*str != 0x0000) {
            لکھو("U+%04x ", *str);
            str++;
        }
        لکھو("] map to %d\n", ret);
#اگر_ختم
        واپس ret;
    }

نجی:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

جماعت رمز_Points_Vectorizer : عوامی Vectorizer {
عوامی:
    رمز_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    مجازی ~رمز_Points_Vectorizer();
    مجازی خالی vectorize(UText *متن, int32_t آغاز_Pos, int32_t انتہا_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_رمز &حالت_2) ثابت override;
};

رمز_Points_Vectorizer::~رمز_Points_Vectorizer()
{
}

خالی رمز_Points_Vectorizer::vectorize(
    UText *متن, int32_t آغاز_Pos, int32_t انتہا_Pos,
    UVector32 &offsets, UVector32 &indices, UError_رمز &حالت_2) ثابت
{
    اگر (offsets.ensure_استعداد(انتہا_Pos - آغاز_Pos, حالت_2) &&
            indices.ensure_استعداد(انتہا_Pos - آغاز_Pos, حالت_2)) {
        اگر (U_FAILURE(حالت_2)) واپس;
        utext_سیٹ_Native_اشاریہ(متن, آغاز_Pos);
        int32_t current;
        char16_t str[2] = {0, 0};
        جب (U_SUCCESS(حالت_2) &&
               (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < انتہا_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(متن));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.جوڑو_Element(current, حالت_2);
            indices.جوڑو_Element(string_مقصد_اشاریہ(str), حالت_2);
        }
    }
}

جماعت GraphemeClusterVectorizer : عوامی Vectorizer {
عوامی:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    مجازی ~GraphemeClusterVectorizer();
    مجازی خالی vectorize(UText *متن, int32_t آغاز_Pos, int32_t انتہا_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_رمز &حالت_2) ثابت override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_لمبائی = 10;

خالی GraphemeClusterVectorizer::vectorize(
    UText *متن, int32_t آغاز_Pos, int32_t انتہا_Pos,
    UVector32 &offsets, UVector32 &indices, UError_رمز &حالت_2) ثابت
{
    اگر (U_FAILURE(حالت_2)) واپس;
    اگر (!offsets.ensure_استعداد(انتہا_Pos - آغاز_Pos, حالت_2) ||
            !indices.ensure_استعداد(انتہا_Pos - آغاز_Pos, حالت_2)) {
        واپس;
    }
    اگر (U_FAILURE(حالت_2)) واپس;
    Local_اشارہ_گر<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), حالت_2));
    اگر (U_FAILURE(حالت_2)) واپس;
    graphemeIter->سیٹ_متن(متن, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس;

    اگر (آغاز_Pos != 0) {
        graphemeIter->preceding(آغاز_Pos);
    }
    int32_t آخری = آغاز_Pos;
    int32_t current = آغاز_Pos;
    char16_t str[MAX_GRAPHEME_CLSTER_لمبائی];
    جب ((current = graphemeIter->next()) != BreakIterator::مکمل_2) {
        اگر (current >= انتہا_Pos) {
            توڑو;
        }
        اگر (current > آغاز_Pos) {
            utext_نکالو(متن, آخری, current, str, MAX_GRAPHEME_CLSTER_لمبائی, &حالت_2);
            اگر (U_FAILURE(حالت_2)) واپس;
            offsets.جوڑو_Element(آخری, حالت_2);
            indices.جوڑو_Element(string_مقصد_اشاریہ(str), حالت_2);
            اگر (U_FAILURE(حالت_2)) واپس;
        }
        آخری = current;
    }
    اگر (U_FAILURE(حالت_2) || آخری >= انتہا_Pos) {
        واپس;
    }
    utext_نکالو(متن, آخری, انتہا_Pos, str, MAX_GRAPHEME_CLSTER_لمبائی, &حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        offsets.جوڑو_Element(آخری, حالت_2);
        indices.جوڑو_Element(string_مقصد_اشاریہ(str), حالت_2);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
خالی compute(
    int32_t hunits,
    ثابت ReadArray2D& W, ثابت ReadArray2D& U, ثابت ReadArray1D& b,
    ثابت ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .جوڑو_Dot_حاصل_ضرب(x, W)
        .جوڑو_Dot_حاصل_ضرب(h, U);

    ifco.قاش(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.قاش(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.قاش(2*hunits, hunits).tanh(); // c_: tanh
    ifco.قاش(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_حاصل_ضرب(ifco.قاش(hunits, hunits))
        .جوڑو_Hadamard_حاصل_ضرب(ifco.قاش(0, hunits), ifco.قاش(2*hunits, hunits));

    h.tanh(c)
        .hadamard_حاصل_ضرب(ifco.قاش(3*hunits, hunits));
}

// Minimum word size
ساکن ثابت int32_t MIN_لفظ = 2;

// Minimum number of characters for two words
ساکن ثابت int32_t MIN_لفظ_وسعت = MIN_لفظ * 2;

int32_t
LSTMBreakEngine::divide_اوپر_Dictionary_Range( UText *متن,
                                                int32_t آغاز_Pos,
                                                int32_t انتہا_Pos,
                                                UVector32 &ملا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    int32_t begin_ملا_Break_سائز = ملا_Breaks.size();
    utext_سیٹ_Native_اشاریہ(متن, آغاز_Pos);
    utext_moveIndex32(متن, MIN_لفظ_وسعت);
    اگر (utext_get_Native_اشاریہ(متن) >= انتہا_Pos) {
        واپس 0;       // Not enough characters for two words
    }
    utext_سیٹ_Native_اشاریہ(متن, آغاز_Pos);

    UVector32 offsets(حالت_2);
    UVector32 indices(حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس 0;
    fVectorizer->vectorize(متن, آغاز_Pos, انتہا_Pos, offsets, indices, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس 0;
    int32_t* offsetsBuf = offsets.get_بفر();
    int32_t* indicesBuf = indices.get_بفر();

    int32_t داخلہ_seq_len = indices.size();
    int32_t hunits = f_ڈیٹا->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, حالت_2);

    Array1D c(hunits, حالت_2);
    Array1D logp(4, حالت_2);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(داخلہ_seq_len, hunits, حالت_2);

    // Allocate fbRow and slice the internal array in two.
    Array1D fb_صف(2 * hunits, حالت_2);

    // ----- End of all the Array memory allocation needed for this function
    اگر (U_FAILURE(حالت_2)) واپس 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    برائے (int32_t i = داخلہ_seq_len - 1; i >= 0; i--) {
        Array1D h_صف = hBackward.صف(i);
        اگر (i != داخلہ_seq_len - 1) {
            h_صف.assign(hBackward.صف(i+1));
        }
#اگر_تعریف_شدہ LSTM_DEBUG
        لکھو("hRow %d\n", i);
        h_صف.print();
        لکھو("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        لکھو("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_ڈیٹا->fEmbedding.صف(indicesBuf[i]).print();
#اگر_ختم  // LSTM_DEBUG
        compute(hunits,
                f_ڈیٹا->fBackwardW, f_ڈیٹا->fBackwardU, f_ڈیٹا->fBackwardB,
                f_ڈیٹا->fEmbedding.صف(indicesBuf[i]),
                h_صف, c, ifco);
    }


    Array1D forward_صف = fb_صف.قاش(0, hunits);  // point to first half of data in fbRow.
    Array1D backward_صف = fb_صف.قاش(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    برائے (int32_t i = 0; i < داخلہ_seq_len; i++) {
#اگر_تعریف_شدہ LSTM_DEBUG
        لکھو("forwardRow %d\n", i);
        forward_صف.print();
#اگر_ختم  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_ڈیٹا->fForwardW, f_ڈیٹا->fForwardU, f_ڈیٹا->fForwardB,
                f_ڈیٹا->fEmbedding.صف(indicesBuf[i]),
                forward_صف, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backward_صف.assign(hBackward.صف(i));

        logp.assign(f_ڈیٹا->fOutputB).جوڑو_Dot_حاصل_ضرب(fb_صف, f_ڈیٹا->fOutputW);
#اگر_تعریف_شدہ LSTM_DEBUG
        لکھو("backwardRow %d\n", i);
        backward_صف.print();
        لکھو("logp %d\n", i);
        logp.print();
#اگر_ختم  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_اشاریہ());
        // BIES logic.
        اگر (current == BEGIN || current == SINGLE) {
            اگر (i != 0) {
                ملا_Breaks.جوڑو_Element(offsetsBuf[i], حالت_2);
                اگر (U_FAILURE(حالت_2)) واپس 0;
            }
        }
    }
    واپس ملا_Breaks.size() - begin_ملا_Break_سائز;
}

Vectorizer* createVectorizer(ثابت LSTMData* ڈیٹا, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    انتخاب (ڈیٹا->fType) {
        حالت رمز_POINTS:
            واپس نیا رمز_Points_Vectorizer(ڈیٹا->fDict);
            توڑو;
        حالت GRAPHEME_CLUSTER:
            واپس نیا GraphemeClusterVectorizer(ڈیٹا->fDict);
            توڑو;
        طےشدہ:
            توڑو;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ثابت LSTMData* ڈیٹا, ثابت Unicode_سیٹ& سیٹ, UError_رمز &حالت_2)
    : DictionaryBreakEngine(), f_ڈیٹا(ڈیٹا), fVectorizer(createVectorizer(f_ڈیٹا, حالت_2))
{
    اگر (U_FAILURE(حالت_2)) {
      f_ڈیٹا = خالی_اشاریہ;  // If failure, we should not delete fData in destructor because the caller will do so.
      واپس;
    }
    سیٹ_Characters(سیٹ);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    حذف f_ڈیٹا;
    حذف fVectorizer;
}

ثابت char16_t* LSTMBreakEngine::name() ثابت {
    واپس f_ڈیٹا->fName;
}

UnicodeString defaultLSTM(UScript_رمز script, UError_رمز& حالت_2) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &حالت_2);
    b = ures_get_By_کلید_With_Fallback(b, "lstm", b, &حالت_2);
    UnicodeString نتیجہ = ures_get_Unicode_String_By_کلید(b, uscript_getShortName(script), &حالت_2);
    ures_close(b);
    واپس نتیجہ;
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_رمز script, UError_رمز& حالت_2)
{
    اگر (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        واپس خالی_اشاریہ;
    }
    UnicodeString name = defaultLSTM(script, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس خالی_اشاریہ;
    CharString namebuf;
    namebuf.appendInvariantChars(name, حالت_2).truncate(namebuf.آخری_اشاریہ_کا('.'));

    Local_UResource_Bundle_اشارہ_گر rb(
        ures_open_براہ_راست(U_ICUDATA_BRKITR, namebuf.ڈیٹا(), &حالت_2));
    اگر (U_FAILURE(حالت_2)) واپس خالی_اشاریہ;

    واپس CreateLSTMData(rb.orphan(), حالت_2);
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    ثابت LSTMData* نتیجہ = نیا LSTMData(rb, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        حذف نتیجہ;
        واپس خالی_اشاریہ;
    }
    واپس نتیجہ;
}

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_رمز script, ثابت LSTMData* ڈیٹا, UError_رمز& حالت_2)
{
    UnicodeString unicode_سیٹ_String;
    انتخاب(script) {
        حالت USCRIPT_THAI:
            unicode_سیٹ_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            توڑو;
        حالت USCRIPT_MYANMAR:
            unicode_سیٹ_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            توڑو;
        طےشدہ:
            حذف ڈیٹا;
            واپس خالی_اشاریہ;
    }
    Unicode_سیٹ unicode_سیٹ;
    unicode_سیٹ.apply_نمونہ(unicode_سیٹ_String, حالت_2);
    ثابت LanguageBreakEngine* engine = نیا LSTMBreakEngine(ڈیٹا, unicode_سیٹ, حالت_2);
    اگر (U_FAILURE(حالت_2) || engine == خالی_اشاریہ) {
        اگر (engine != خالی_اشاریہ) {
            حذف engine;
        } ورنہ {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
        }
        واپس خالی_اشاریہ;
    }
    واپس engine;
}

U_CAPI خالی U_EXPORT2 DeleteLSTMData(ثابت LSTMData* ڈیٹا)
{
    حذف ڈیٹا;
}

U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* ڈیٹا)
{
    واپس ڈیٹا->fName;
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */

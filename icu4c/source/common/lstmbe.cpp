// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن <complex>
#ضمن <utility>

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "brkeng.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "lstmbe.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"
#ضمن "ubrkimpl.h"
#ضمن "uresimp.h"
#ضمن "uvectr32.h"
#ضمن "uvector.h"

#ضمن "unicode/brkiter.h"
#ضمن "unicode/resbund.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
صنف ReadArray1D {
عام:
    مجازی ~ReadArray1D();
    مجازی int32_t d1() ثابت = 0;
    مجازی عائم get(int32_t i) ثابت = 0;

#اگر_تعریف_شده LSTM_DEBUG
    فراغ print() ثابت {
        چاپ_کن("\n[");
        لكل (int32_t i = 0; i < d1(); i++) {
           چاپ_کن("%0.8e ", get(i));
           إذا (i % 4 == 3) چاپ_کن("\n");
        }
        چاپ_کن("]\n");
    }
#پایان_اگر
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
صنف ReadArray2D {
عام:
    مجازی ~ReadArray2D();
    مجازی int32_t d1() ثابت = 0;
    مجازی int32_t d2() ثابت = 0;
    مجازی عائم get(int32_t i, int32_t j) ثابت = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
صنف ConstArray1D : عام ReadArray1D {
عام:
    ConstArray1D() : داده_2(اشاره_تهی), d1_(0) {}

    ConstArray1D(ثابت عائم* داده, int32_t d1) : داده_2(داده), d1_(d1) {}

    مجازی ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    فراغ init(ثابت int32_t* داده, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        داده_2 = reinterpret_cast<ثابت عائم*>(داده);
        d1_ = d1;
    }

    // ReadArray1D methods.
    مجازی int32_t d1() ثابت override { أرجع d1_; }
    مجازی عائم get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        أرجع داده_2[i];
    }

خاص:
    ثابت عائم* داده_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
صنف ConstArray2D : عام ReadArray2D {
عام:
    ConstArray2D() : داده_2(اشاره_تهی), d1_(0), d2_(0) {}

    ConstArray2D(ثابت عائم* داده, int32_t d1, int32_t d2)
        : داده_2(داده), d1_(d1), d2_(d2) {}

    مجازی ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    فراغ init(ثابت int32_t* داده, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        داده_2 = reinterpret_cast<ثابت عائم*>(داده);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    مضمن int32_t d1() ثابت override { أرجع d1_; }
    مضمن int32_t d2() ثابت override { أرجع d2_; }
    عائم get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        أرجع داده_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    مضمن ConstArray1D ردیف(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        أرجع ConstArray1D(داده_2 + i * d2_, d2_);
    }

خاص:
    ثابت عائم* داده_2;
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
صنف Array1D : عام ReadArray1D {
عام:
    Array1D() : حافظه_2(اشاره_تهی), داده_2(اشاره_تهی), d1_(0) {}
    Array1D(int32_t d1, UError_کد &وضعیت)
        : حافظه_2(uprv_malloc(d1 * حجم(عائم))),
          داده_2(static_cast<عائم*>(حافظه_2)), d1_(d1) {
        إذا (U_SUCCESS(وضعیت)) {
            إذا (حافظه_2 == اشاره_تهی) {
                وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            clear();
        }
    }

    مجازی ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(عائم* داده, int32_t d1)
        : حافظه_2(اشاره_تهی), داده_2(داده), d1_(d1) {}

    // ReadArray1D methods.
    مجازی int32_t d1() ثابت override { أرجع d1_; }
    مجازی عائم get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        أرجع داده_2[i];
    }

    // Return the index which point to the max data in the array.
    مضمن int32_t max_شاخص() ثابت {
        int32_t شاخص = 0;
        عائم max = داده_2[0];
        لكل (int32_t i = 1; i < d1_; i++) {
            إذا (داده_2[i] > max) {
                max = داده_2[i];
                شاخص = i;
            }
        }
        أرجع شاخص;
    }

    // Slice part of the array to a new one.
    مضمن Array1D برش(int32_t from, int32_t size) ثابت {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        أرجع Array1D(داده_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    مضمن Array1D& جمع_کن_Dot_حاصلضرب(ثابت ReadArray1D& a, ثابت ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            لكل (int32_t j = 0; j < a.d1(); j++) {
                داده_2[i] += a.get(j) * b.get(j, i);
            }
        }
        أرجع *هذا;
    }

    // Hadamard Product the values of another array of the same size into this one.
    مضمن Array1D& hadamard_حاصلضرب(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            داده_2[i] *= a.get(i);
        }
        أرجع *هذا;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    مضمن Array1D& جمع_کن_Hadamard_حاصلضرب(ثابت ReadArray1D& a, ثابت ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            داده_2[i] += a.get(i) * b.get(i);
        }
        أرجع *هذا;
    }

    // Add the values of another array of the same size into this one.
    مضمن Array1D& جمع_کن(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            داده_2[i] += a.get(i);
        }
        أرجع *هذا;
    }

    // Assign the values of another array of the same size into this one.
    مضمن Array1D& assign(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            داده_2[i] = a.get(i);
        }
        أرجع *هذا;
    }

    // Apply tanh to all the elements in the array.
    مضمن Array1D& tanh() {
        أرجع tanh(*هذا);
    }

    // Apply tanh of a and store into this array.
    مضمن Array1D& tanh(ثابت Array1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1_; i++) {
            داده_2[i] = std::tanh(a.get(i));
        }
        أرجع *هذا;
    }

    // Apply sigmoid to all the elements in the array.
    مضمن Array1D& sigmoid() {
        لكل (int32_t i = 0; i < d1_; i++) {
            داده_2[i] = 1.0f/(1.0f + expf(-داده_2[i]));
        }
        أرجع *هذا;
    }

    مضمن Array1D& clear() {
        uprv_memset(داده_2, 0, d1_ * حجم(عائم));
        أرجع *هذا;
    }

خاص:
    فراغ* حافظه_2;
    عائم* داده_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(حافظه_2);
}

صنف Array2D : عام ReadArray2D {
عام:
    Array2D() : حافظه_2(اشاره_تهی), داده_2(اشاره_تهی), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_کد &وضعیت)
        : حافظه_2(uprv_malloc(d1 * d2 * حجم(عائم))),
          داده_2(static_cast<عائم*>(حافظه_2)), d1_(d1), d2_(d2) {
        إذا (U_SUCCESS(وضعیت)) {
            إذا (حافظه_2 == اشاره_تهی) {
                وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            clear();
        }
    }
    مجازی ~Array2D();

    // ReadArray2D methods.
    مجازی int32_t d1() ثابت override { أرجع d1_; }
    مجازی int32_t d2() ثابت override { أرجع d2_; }
    مجازی عائم get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        أرجع داده_2[i * d2_ + j];
    }

    مضمن Array1D ردیف(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        أرجع Array1D(داده_2 + i * d2_, d2_);
    }

    مضمن Array2D& clear() {
        uprv_memset(داده_2, 0, d1_ * d2_ * حجم(عائم));
        أرجع *هذا;
    }

خاص:
    فراغ* حافظه_2;
    عائم* داده_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(حافظه_2);
}

تعريفنوع تعداد {
    BEGIN,
    INSIDE,
    انتها_2,
    SINGLE
} LSTMClass;

تعريفنوع تعداد {
    UNKNOWN,
    کد_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

بنية LSTMData : عام UMemory {
    LSTMData(UResourceBundle* rb, UError_کد &وضعیت);
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

خاص:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UError_کد &وضعیت)
    : fDict(اشاره_تهی), fType(UNKNOWN), fName(اشاره_تهی),
      fBundle(rb)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    إذا (IEEE_754 != 1) {
        وضعیت = U_UNSUPPORTED_ERROR;
        أرجع;
    }
    Local_UResource_Bundle_اشاره_گر embeddings_res(
        ures_get_By_کلید(rb, "embeddings", اشاره_تهی, &وضعیت));
    int32_t embedding_اندازه = ures_getInt(embeddings_res.getAlias(), &وضعیت);
    Local_UResource_Bundle_اشاره_گر hunits_res(
        ures_get_By_کلید(rb, "hunits", اشاره_تهی, &وضعیت));
    إذا (U_FAILURE(وضعیت)) أرجع;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &وضعیت);
    ثابت char16_t* type = ures_get_String_By_کلید(rb, "type", اشاره_تهی, &وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع;
    إذا (u_strCompare(type, -1, u"codepoints", -1, خطأ) == 0) {
        fType = کد_POINTS;
    } وإلا إذا (u_strCompare(type, -1, u"graphclust", -1, خطأ) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_کلید(rb, "model", اشاره_تهی, &وضعیت);
    Local_UResource_Bundle_اشاره_گر داده_Res(ures_get_By_کلید(rb, "data", اشاره_تهی, &وضعیت));
    إذا (U_FAILURE(وضعیت)) أرجع;
    int32_t داده_len = 0;
    ثابت int32_t* داده = ures_get_Int_وکتور(داده_Res.getAlias(), &داده_len, &وضعیت);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, اشاره_تهی, &وضعیت);

    پشته_UResource_Bundle پشته_موقت_Bundle;
    Resource_داده_مقدار value;
    ures_get_مقدار_With_Fallback(rb, "dict", پشته_موقت_Bundle.getAlias(), value, وضعیت);
    ResourceArray stringArray = value.getArray(وضعیت);
    int32_t رقم_شاخص = stringArray.get_اندازه();
    إذا (U_FAILURE(وضعیت)) { أرجع; }

    // put dict into hash
    int32_t string_درازی;
    لكل (int32_t idx = 0; idx < رقم_شاخص; idx++) {
        stringArray.get_مقدار(idx, value);
        ثابت char16_t* str = value.getString(string_درازی, وضعیت);
        uhash_putiAllowZero(fDict, (فراغ*)str, idx, &وضعیت);
        إذا (U_FAILURE(وضعیت)) أرجع;
#اگر_تعریف_شده LSTM_VECTORIZER_DEBUG
        چاپ_کن("Assign [");
        طالما (*str != 0x0000) {
            چاپ_کن("U+%04x ", *str);
            str++;
        }
        چاپ_کن("] map to %d\n", idx-1);
#پایان_اگر
    }
    int32_t mat1_اندازه = (رقم_شاخص + 1) * embedding_اندازه;
    int32_t mat2_اندازه = embedding_اندازه * 4 * hunits;
    int32_t mat3_اندازه = hunits * 4 * hunits;
    int32_t mat4_اندازه = 4 * hunits;
    int32_t mat5_اندازه = mat2_اندازه;
    int32_t mat6_اندازه = mat3_اندازه;
    int32_t mat7_اندازه = mat4_اندازه;
    int32_t mat8_اندازه = 2 * hunits * 4;
#إذا U_DEBUG
    int32_t mat9_اندازه = 4;
    U_ASSERT(داده_len == mat1_اندازه + mat2_اندازه + mat3_اندازه + mat4_اندازه + mat5_اندازه +
        mat6_اندازه + mat7_اندازه + mat8_اندازه + mat9_اندازه);
#پایان_اگر

    fEmbedding.init(داده, (رقم_شاخص + 1), embedding_اندازه);
    داده += mat1_اندازه;
    fForwardW.init(داده, embedding_اندازه, 4 * hunits);
    داده += mat2_اندازه;
    fForwardU.init(داده, hunits, 4 * hunits);
    داده += mat3_اندازه;
    fForwardB.init(داده, 4 * hunits);
    داده += mat4_اندازه;
    fBackwardW.init(داده, embedding_اندازه, 4 * hunits);
    داده += mat5_اندازه;
    fBackwardU.init(داده, hunits, 4 * hunits);
    داده += mat6_اندازه;
    fBackwardB.init(داده, 4 * hunits);
    داده += mat7_اندازه;
    fOutputW.init(داده, 2 * hunits, 4);
    داده += mat8_اندازه;
    fOutputB.init(داده, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

صنف Vectorizer : عام UMemory {
عام:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    مجازی ~Vectorizer();
    مجازی فراغ vectorize(UText *متن, int32_t شروع_Pos, int32_t انتها_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کد &وضعیت) ثابت = 0;
محمي:
    int32_t string_مقصد_شاخص(ثابت char16_t* str) ثابت {
        UBool یافت = خطأ;
        int32_t ret = uhash_geti_And_یافت(fDict, (ثابت فراغ*)str, &یافت);
        إذا (!یافت) {
            ret = fDict->count;
        }
#اگر_تعریف_شده LSTM_VECTORIZER_DEBUG
        چاپ_کن("[");
        طالما (*str != 0x0000) {
            چاپ_کن("U+%04x ", *str);
            str++;
        }
        چاپ_کن("] map to %d\n", ret);
#پایان_اگر
        أرجع ret;
    }

خاص:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

صنف کد_Points_Vectorizer : عام Vectorizer {
عام:
    کد_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    مجازی ~کد_Points_Vectorizer();
    مجازی فراغ vectorize(UText *متن, int32_t شروع_Pos, int32_t انتها_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کد &وضعیت) ثابت override;
};

کد_Points_Vectorizer::~کد_Points_Vectorizer()
{
}

فراغ کد_Points_Vectorizer::vectorize(
    UText *متن, int32_t شروع_Pos, int32_t انتها_Pos,
    UVector32 &offsets, UVector32 &indices, UError_کد &وضعیت) ثابت
{
    إذا (offsets.ensure_ظرفیت(انتها_Pos - شروع_Pos, وضعیت) &&
            indices.ensure_ظرفیت(انتها_Pos - شروع_Pos, وضعیت)) {
        إذا (U_FAILURE(وضعیت)) أرجع;
        utext_مجموعه_Native_شاخص(متن, شروع_Pos);
        int32_t current;
        char16_t str[2] = {0, 0};
        طالما (U_SUCCESS(وضعیت) &&
               (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < انتها_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(متن));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.جمع_کن_Element(current, وضعیت);
            indices.جمع_کن_Element(string_مقصد_شاخص(str), وضعیت);
        }
    }
}

صنف GraphemeClusterVectorizer : عام Vectorizer {
عام:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    مجازی ~GraphemeClusterVectorizer();
    مجازی فراغ vectorize(UText *متن, int32_t شروع_Pos, int32_t انتها_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کد &وضعیت) ثابت override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_درازی = 10;

فراغ GraphemeClusterVectorizer::vectorize(
    UText *متن, int32_t شروع_Pos, int32_t انتها_Pos,
    UVector32 &offsets, UVector32 &indices, UError_کد &وضعیت) ثابت
{
    إذا (U_FAILURE(وضعیت)) أرجع;
    إذا (!offsets.ensure_ظرفیت(انتها_Pos - شروع_Pos, وضعیت) ||
            !indices.ensure_ظرفیت(انتها_Pos - شروع_Pos, وضعیت)) {
        أرجع;
    }
    إذا (U_FAILURE(وضعیت)) أرجع;
    Local_اشاره_گر<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), وضعیت));
    إذا (U_FAILURE(وضعیت)) أرجع;
    graphemeIter->مجموعه_متن(متن, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع;

    إذا (شروع_Pos != 0) {
        graphemeIter->preceding(شروع_Pos);
    }
    int32_t آخرین = شروع_Pos;
    int32_t current = شروع_Pos;
    char16_t str[MAX_GRAPHEME_CLSTER_درازی];
    طالما ((current = graphemeIter->next()) != BreakIterator::انجامشده_2) {
        إذا (current >= انتها_Pos) {
            اكسر;
        }
        إذا (current > شروع_Pos) {
            utext_استخراج(متن, آخرین, current, str, MAX_GRAPHEME_CLSTER_درازی, &وضعیت);
            إذا (U_FAILURE(وضعیت)) أرجع;
            offsets.جمع_کن_Element(آخرین, وضعیت);
            indices.جمع_کن_Element(string_مقصد_شاخص(str), وضعیت);
            إذا (U_FAILURE(وضعیت)) أرجع;
        }
        آخرین = current;
    }
    إذا (U_FAILURE(وضعیت) || آخرین >= انتها_Pos) {
        أرجع;
    }
    utext_استخراج(متن, آخرین, انتها_Pos, str, MAX_GRAPHEME_CLSTER_درازی, &وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        offsets.جمع_کن_Element(آخرین, وضعیت);
        indices.جمع_کن_Element(string_مقصد_شاخص(str), وضعیت);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
فراغ compute(
    int32_t hunits,
    ثابت ReadArray2D& W, ثابت ReadArray2D& U, ثابت ReadArray1D& b,
    ثابت ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .جمع_کن_Dot_حاصلضرب(x, W)
        .جمع_کن_Dot_حاصلضرب(h, U);

    ifco.برش(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.برش(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.برش(2*hunits, hunits).tanh(); // c_: tanh
    ifco.برش(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_حاصلضرب(ifco.برش(hunits, hunits))
        .جمع_کن_Hadamard_حاصلضرب(ifco.برش(0, hunits), ifco.برش(2*hunits, hunits));

    h.tanh(c)
        .hadamard_حاصلضرب(ifco.برش(3*hunits, hunits));
}

// Minimum word size
ساكن ثابت int32_t MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t MIN_کلمه_بازه = MIN_کلمه * 2;

int32_t
LSTMBreakEngine::divide_به_بالا_Dictionary_Range( UText *متن,
                                                int32_t شروع_Pos,
                                                int32_t انتها_Pos,
                                                UVector32 &یافت_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    int32_t begin_یافت_Break_اندازه = یافت_Breaks.size();
    utext_مجموعه_Native_شاخص(متن, شروع_Pos);
    utext_moveIndex32(متن, MIN_کلمه_بازه);
    إذا (utext_get_Native_شاخص(متن) >= انتها_Pos) {
        أرجع 0;       // Not enough characters for two words
    }
    utext_مجموعه_Native_شاخص(متن, شروع_Pos);

    UVector32 offsets(وضعیت);
    UVector32 indices(وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    fVectorizer->vectorize(متن, شروع_Pos, انتها_Pos, offsets, indices, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    int32_t* offsetsBuf = offsets.get_میانگیر();
    int32_t* indicesBuf = indices.get_میانگیر();

    int32_t درون_داد_seq_len = indices.size();
    int32_t hunits = f_داده->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, وضعیت);

    Array1D c(hunits, وضعیت);
    Array1D logp(4, وضعیت);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(درون_داد_seq_len, hunits, وضعیت);

    // Allocate fbRow and slice the internal array in two.
    Array1D fb_ردیف(2 * hunits, وضعیت);

    // ----- End of all the Array memory allocation needed for this function
    إذا (U_FAILURE(وضعیت)) أرجع 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    لكل (int32_t i = درون_داد_seq_len - 1; i >= 0; i--) {
        Array1D h_ردیف = hBackward.ردیف(i);
        إذا (i != درون_داد_seq_len - 1) {
            h_ردیف.assign(hBackward.ردیف(i+1));
        }
#اگر_تعریف_شده LSTM_DEBUG
        چاپ_کن("hRow %d\n", i);
        h_ردیف.print();
        چاپ_کن("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        چاپ_کن("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_داده->fEmbedding.ردیف(indicesBuf[i]).print();
#پایان_اگر  // LSTM_DEBUG
        compute(hunits,
                f_داده->fBackwardW, f_داده->fBackwardU, f_داده->fBackwardB,
                f_داده->fEmbedding.ردیف(indicesBuf[i]),
                h_ردیف, c, ifco);
    }


    Array1D forward_ردیف = fb_ردیف.برش(0, hunits);  // point to first half of data in fbRow.
    Array1D backward_ردیف = fb_ردیف.برش(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    لكل (int32_t i = 0; i < درون_داد_seq_len; i++) {
#اگر_تعریف_شده LSTM_DEBUG
        چاپ_کن("forwardRow %d\n", i);
        forward_ردیف.print();
#پایان_اگر  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_داده->fForwardW, f_داده->fForwardU, f_داده->fForwardB,
                f_داده->fEmbedding.ردیف(indicesBuf[i]),
                forward_ردیف, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backward_ردیف.assign(hBackward.ردیف(i));

        logp.assign(f_داده->fOutputB).جمع_کن_Dot_حاصلضرب(fb_ردیف, f_داده->fOutputW);
#اگر_تعریف_شده LSTM_DEBUG
        چاپ_کن("backwardRow %d\n", i);
        backward_ردیف.print();
        چاپ_کن("logp %d\n", i);
        logp.print();
#پایان_اگر  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_شاخص());
        // BIES logic.
        إذا (current == BEGIN || current == SINGLE) {
            إذا (i != 0) {
                یافت_Breaks.جمع_کن_Element(offsetsBuf[i], وضعیت);
                إذا (U_FAILURE(وضعیت)) أرجع 0;
            }
        }
    }
    أرجع یافت_Breaks.size() - begin_یافت_Break_اندازه;
}

Vectorizer* createVectorizer(ثابت LSTMData* داده, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    بدل (داده->fType) {
        حالة کد_POINTS:
            أرجع جديد کد_Points_Vectorizer(داده->fDict);
            اكسر;
        حالة GRAPHEME_CLUSTER:
            أرجع جديد GraphemeClusterVectorizer(داده->fDict);
            اكسر;
        افتراضي:
            اكسر;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ثابت LSTMData* داده, ثابت Unicode_مجموعه& مجموعه, UError_کد &وضعیت)
    : DictionaryBreakEngine(), f_داده(داده), fVectorizer(createVectorizer(f_داده, وضعیت))
{
    إذا (U_FAILURE(وضعیت)) {
      f_داده = اشاره_تهی;  // If failure, we should not delete fData in destructor because the caller will do so.
      أرجع;
    }
    مجموعه_Characters(مجموعه);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    حذف f_داده;
    حذف fVectorizer;
}

ثابت char16_t* LSTMBreakEngine::name() ثابت {
    أرجع f_داده->fName;
}

UnicodeString defaultLSTM(UScript_کد script, UError_کد& وضعیت) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &وضعیت);
    b = ures_get_By_کلید_With_Fallback(b, "lstm", b, &وضعیت);
    UnicodeString نتیجه = ures_get_Unicode_String_By_کلید(b, uscript_getShortName(script), &وضعیت);
    ures_close(b);
    أرجع نتیجه;
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_کد script, UError_کد& وضعیت)
{
    إذا (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        أرجع اشاره_تهی;
    }
    UnicodeString name = defaultLSTM(script, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع اشاره_تهی;
    CharString namebuf;
    namebuf.appendInvariantChars(name, وضعیت).truncate(namebuf.آخرین_شاخص_از('.'));

    Local_UResource_Bundle_اشاره_گر rb(
        ures_open_مستقیم(U_ICUDATA_BRKITR, namebuf.داده(), &وضعیت));
    إذا (U_FAILURE(وضعیت)) أرجع اشاره_تهی;

    أرجع CreateLSTMData(rb.orphan(), وضعیت);
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    ثابت LSTMData* نتیجه = جديد LSTMData(rb, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        حذف نتیجه;
        أرجع اشاره_تهی;
    }
    أرجع نتیجه;
}

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_کد script, ثابت LSTMData* داده, UError_کد& وضعیت)
{
    UnicodeString unicode_مجموعه_String;
    بدل(script) {
        حالة USCRIPT_THAI:
            unicode_مجموعه_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            اكسر;
        حالة USCRIPT_MYANMAR:
            unicode_مجموعه_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            اكسر;
        افتراضي:
            حذف داده;
            أرجع اشاره_تهی;
    }
    Unicode_مجموعه unicode_مجموعه;
    unicode_مجموعه.apply_الگو(unicode_مجموعه_String, وضعیت);
    ثابت LanguageBreakEngine* engine = جديد LSTMBreakEngine(داده, unicode_مجموعه, وضعیت);
    إذا (U_FAILURE(وضعیت) || engine == اشاره_تهی) {
        إذا (engine != اشاره_تهی) {
            حذف engine;
        } وإلا {
            وضعیت = U_حافظه_تخصیص_ERROR;
        }
        أرجع اشاره_تهی;
    }
    أرجع engine;
}

U_CAPI فراغ U_EXPORT2 DeleteLSTMData(ثابت LSTMData* داده)
{
    حذف داده;
}

U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* داده)
{
    أرجع داده->fName;
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */

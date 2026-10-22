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
    مجازي ~ReadArray1D();
    مجازي int32_t d1() ثابت = 0;
    مجازي عائم get(int32_t i) ثابت = 0;

#که_تعریف_شوی LSTM_DEBUG
    فراغ print() ثابت {
        چاپ_کړه("\n[");
        لكل (int32_t i = 0; i < d1(); i++) {
           چاپ_کړه("%0.8e ", get(i));
           إذا (i % 4 == 3) چاپ_کړه("\n");
        }
        چاپ_کړه("]\n");
    }
#د_که_پای
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
صنف ReadArray2D {
عام:
    مجازي ~ReadArray2D();
    مجازي int32_t d1() ثابت = 0;
    مجازي int32_t d2() ثابت = 0;
    مجازي عائم get(int32_t i, int32_t j) ثابت = 0;
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
    ConstArray1D() : معلومات_2(تش_اشاره), d1_(0) {}

    ConstArray1D(ثابت عائم* معلومات, int32_t d1) : معلومات_2(معلومات), d1_(d1) {}

    مجازي ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    فراغ init(ثابت int32_t* معلومات, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        معلومات_2 = reinterpret_cast<ثابت عائم*>(معلومات);
        d1_ = d1;
    }

    // ReadArray1D methods.
    مجازي int32_t d1() ثابت override { أرجع d1_; }
    مجازي عائم get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        أرجع معلومات_2[i];
    }

خاص:
    ثابت عائم* معلومات_2;
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
    ConstArray2D() : معلومات_2(تش_اشاره), d1_(0), d2_(0) {}

    ConstArray2D(ثابت عائم* معلومات, int32_t d1, int32_t d2)
        : معلومات_2(معلومات), d1_(d1), d2_(d2) {}

    مجازي ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    فراغ init(ثابت int32_t* معلومات, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        معلومات_2 = reinterpret_cast<ثابت عائم*>(معلومات);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    مضمن int32_t d1() ثابت override { أرجع d1_; }
    مضمن int32_t d2() ثابت override { أرجع d2_; }
    عائم get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        أرجع معلومات_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    مضمن ConstArray1D لیکه(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        أرجع ConstArray1D(معلومات_2 + i * d2_, d2_);
    }

خاص:
    ثابت عائم* معلومات_2;
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
    Array1D() : حافظه_2(تش_اشاره), معلومات_2(تش_اشاره), d1_(0) {}
    Array1D(int32_t d1, UError_کوډ &حالت)
        : حافظه_2(uprv_malloc(d1 * حجم(عائم))),
          معلومات_2(static_cast<عائم*>(حافظه_2)), d1_(d1) {
        إذا (U_SUCCESS(حالت)) {
            إذا (حافظه_2 == تش_اشاره) {
                حالت = U_حافظه_ویش_ERROR;
                أرجع;
            }
            clear();
        }
    }

    مجازي ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(عائم* معلومات, int32_t d1)
        : حافظه_2(تش_اشاره), معلومات_2(معلومات), d1_(d1) {}

    // ReadArray1D methods.
    مجازي int32_t d1() ثابت override { أرجع d1_; }
    مجازي عائم get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        أرجع معلومات_2[i];
    }

    // Return the index which point to the max data in the array.
    مضمن int32_t max_شاخص() ثابت {
        int32_t شاخص = 0;
        عائم max = معلومات_2[0];
        لكل (int32_t i = 1; i < d1_; i++) {
            إذا (معلومات_2[i] > max) {
                max = معلومات_2[i];
                شاخص = i;
            }
        }
        أرجع شاخص;
    }

    // Slice part of the array to a new one.
    مضمن Array1D ټوټه(int32_t from, int32_t size) ثابت {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        أرجع Array1D(معلومات_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    مضمن Array1D& جمع_کړه_Dot_حاصل_ضرب(ثابت ReadArray1D& a, ثابت ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            لكل (int32_t j = 0; j < a.d1(); j++) {
                معلومات_2[i] += a.get(j) * b.get(j, i);
            }
        }
        أرجع *هذا;
    }

    // Hadamard Product the values of another array of the same size into this one.
    مضمن Array1D& hadamard_حاصل_ضرب(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            معلومات_2[i] *= a.get(i);
        }
        أرجع *هذا;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    مضمن Array1D& جمع_کړه_Hadamard_حاصل_ضرب(ثابت ReadArray1D& a, ثابت ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            معلومات_2[i] += a.get(i) * b.get(i);
        }
        أرجع *هذا;
    }

    // Add the values of another array of the same size into this one.
    مضمن Array1D& جمع_کړه(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            معلومات_2[i] += a.get(i);
        }
        أرجع *هذا;
    }

    // Assign the values of another array of the same size into this one.
    مضمن Array1D& assign(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            معلومات_2[i] = a.get(i);
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
            معلومات_2[i] = std::tanh(a.get(i));
        }
        أرجع *هذا;
    }

    // Apply sigmoid to all the elements in the array.
    مضمن Array1D& sigmoid() {
        لكل (int32_t i = 0; i < d1_; i++) {
            معلومات_2[i] = 1.0f/(1.0f + expf(-معلومات_2[i]));
        }
        أرجع *هذا;
    }

    مضمن Array1D& clear() {
        uprv_memset(معلومات_2, 0, d1_ * حجم(عائم));
        أرجع *هذا;
    }

خاص:
    فراغ* حافظه_2;
    عائم* معلومات_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(حافظه_2);
}

صنف Array2D : عام ReadArray2D {
عام:
    Array2D() : حافظه_2(تش_اشاره), معلومات_2(تش_اشاره), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_کوډ &حالت)
        : حافظه_2(uprv_malloc(d1 * d2 * حجم(عائم))),
          معلومات_2(static_cast<عائم*>(حافظه_2)), d1_(d1), d2_(d2) {
        إذا (U_SUCCESS(حالت)) {
            إذا (حافظه_2 == تش_اشاره) {
                حالت = U_حافظه_ویش_ERROR;
                أرجع;
            }
            clear();
        }
    }
    مجازي ~Array2D();

    // ReadArray2D methods.
    مجازي int32_t d1() ثابت override { أرجع d1_; }
    مجازي int32_t d2() ثابت override { أرجع d2_; }
    مجازي عائم get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        أرجع معلومات_2[i * d2_ + j];
    }

    مضمن Array1D لیکه(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        أرجع Array1D(معلومات_2 + i * d2_, d2_);
    }

    مضمن Array2D& clear() {
        uprv_memset(معلومات_2, 0, d1_ * d2_ * حجم(عائم));
        أرجع *هذا;
    }

خاص:
    فراغ* حافظه_2;
    عائم* معلومات_2;
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
    پای_ټکی_2,
    SINGLE
} LSTMClass;

تعريفنوع تعداد {
    UNKNOWN,
    کوډ_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

بنية LSTMData : عام UMemory {
    LSTMData(UResourceBundle* rb, UError_کوډ &حالت);
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

LSTMData::LSTMData(UResourceBundle* rb, UError_کوډ &حالت)
    : fDict(تش_اشاره), fType(UNKNOWN), fName(تش_اشاره),
      fBundle(rb)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (IEEE_754 != 1) {
        حالت = U_UNSUPPORTED_ERROR;
        أرجع;
    }
    Local_UResource_Bundle_اشاره_ګر embeddings_res(
        ures_get_By_کلی(rb, "embeddings", تش_اشاره, &حالت));
    int32_t embedding_اندازه = ures_getInt(embeddings_res.getAlias(), &حالت);
    Local_UResource_Bundle_اشاره_ګر hunits_res(
        ures_get_By_کلی(rb, "hunits", تش_اشاره, &حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &حالت);
    ثابت char16_t* type = ures_get_String_By_کلی(rb, "type", تش_اشاره, &حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (u_strCompare(type, -1, u"codepoints", -1, خطأ) == 0) {
        fType = کوډ_POINTS;
    } وإلا إذا (u_strCompare(type, -1, u"graphclust", -1, خطأ) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_کلی(rb, "model", تش_اشاره, &حالت);
    Local_UResource_Bundle_اشاره_ګر معلومات_Res(ures_get_By_کلی(rb, "data", تش_اشاره, &حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    int32_t معلومات_len = 0;
    ثابت int32_t* معلومات = ures_get_Int_ویکټور(معلومات_Res.getAlias(), &معلومات_len, &حالت);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, تش_اشاره, &حالت);

    ډېری_UResource_Bundle ډېری_لنډمهاله_Bundle;
    Resource_معلومات_ارزښت value;
    ures_get_ارزښت_With_Fallback(rb, "dict", ډېری_لنډمهاله_Bundle.getAlias(), value, حالت);
    ResourceArray stringArray = value.getArray(حالت);
    int32_t عدد_شاخص = stringArray.get_اندازه();
    إذا (U_FAILURE(حالت)) { أرجع; }

    // put dict into hash
    int32_t string_اوږدتیا;
    لكل (int32_t idx = 0; idx < عدد_شاخص; idx++) {
        stringArray.get_ارزښت(idx, value);
        ثابت char16_t* str = value.getString(string_اوږدتیا, حالت);
        uhash_putiAllowZero(fDict, (فراغ*)str, idx, &حالت);
        إذا (U_FAILURE(حالت)) أرجع;
#که_تعریف_شوی LSTM_VECTORIZER_DEBUG
        چاپ_کړه("Assign [");
        طالما (*str != 0x0000) {
            چاپ_کړه("U+%04x ", *str);
            str++;
        }
        چاپ_کړه("] map to %d\n", idx-1);
#د_که_پای
    }
    int32_t mat1_اندازه = (عدد_شاخص + 1) * embedding_اندازه;
    int32_t mat2_اندازه = embedding_اندازه * 4 * hunits;
    int32_t mat3_اندازه = hunits * 4 * hunits;
    int32_t mat4_اندازه = 4 * hunits;
    int32_t mat5_اندازه = mat2_اندازه;
    int32_t mat6_اندازه = mat3_اندازه;
    int32_t mat7_اندازه = mat4_اندازه;
    int32_t mat8_اندازه = 2 * hunits * 4;
#إذا U_DEBUG
    int32_t mat9_اندازه = 4;
    U_ASSERT(معلومات_len == mat1_اندازه + mat2_اندازه + mat3_اندازه + mat4_اندازه + mat5_اندازه +
        mat6_اندازه + mat7_اندازه + mat8_اندازه + mat9_اندازه);
#د_که_پای

    fEmbedding.init(معلومات, (عدد_شاخص + 1), embedding_اندازه);
    معلومات += mat1_اندازه;
    fForwardW.init(معلومات, embedding_اندازه, 4 * hunits);
    معلومات += mat2_اندازه;
    fForwardU.init(معلومات, hunits, 4 * hunits);
    معلومات += mat3_اندازه;
    fForwardB.init(معلومات, 4 * hunits);
    معلومات += mat4_اندازه;
    fBackwardW.init(معلومات, embedding_اندازه, 4 * hunits);
    معلومات += mat5_اندازه;
    fBackwardU.init(معلومات, hunits, 4 * hunits);
    معلومات += mat6_اندازه;
    fBackwardB.init(معلومات, 4 * hunits);
    معلومات += mat7_اندازه;
    fOutputW.init(معلومات, 2 * hunits, 4);
    معلومات += mat8_اندازه;
    fOutputB.init(معلومات, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

صنف Vectorizer : عام UMemory {
عام:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    مجازي ~Vectorizer();
    مجازي فراغ vectorize(UText *متن, int32_t پیل_Pos, int32_t پای_ټکی_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کوډ &حالت) ثابت = 0;
محمي:
    int32_t string_منزل_شاخص(ثابت char16_t* str) ثابت {
        UBool وموندل = خطأ;
        int32_t ret = uhash_geti_And_وموندل(fDict, (ثابت فراغ*)str, &وموندل);
        إذا (!وموندل) {
            ret = fDict->count;
        }
#که_تعریف_شوی LSTM_VECTORIZER_DEBUG
        چاپ_کړه("[");
        طالما (*str != 0x0000) {
            چاپ_کړه("U+%04x ", *str);
            str++;
        }
        چاپ_کړه("] map to %d\n", ret);
#د_که_پای
        أرجع ret;
    }

خاص:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

صنف کوډ_Points_Vectorizer : عام Vectorizer {
عام:
    کوډ_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    مجازي ~کوډ_Points_Vectorizer();
    مجازي فراغ vectorize(UText *متن, int32_t پیل_Pos, int32_t پای_ټکی_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کوډ &حالت) ثابت override;
};

کوډ_Points_Vectorizer::~کوډ_Points_Vectorizer()
{
}

فراغ کوډ_Points_Vectorizer::vectorize(
    UText *متن, int32_t پیل_Pos, int32_t پای_ټکی_Pos,
    UVector32 &offsets, UVector32 &indices, UError_کوډ &حالت) ثابت
{
    إذا (offsets.ensure_ظرفیت(پای_ټکی_Pos - پیل_Pos, حالت) &&
            indices.ensure_ظرفیت(پای_ټکی_Pos - پیل_Pos, حالت)) {
        إذا (U_FAILURE(حالت)) أرجع;
        utext_سېټ_Native_شاخص(متن, پیل_Pos);
        int32_t current;
        char16_t str[2] = {0, 0};
        طالما (U_SUCCESS(حالت) &&
               (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < پای_ټکی_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(متن));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.جمع_کړه_Element(current, حالت);
            indices.جمع_کړه_Element(string_منزل_شاخص(str), حالت);
        }
    }
}

صنف GraphemeClusterVectorizer : عام Vectorizer {
عام:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    مجازي ~GraphemeClusterVectorizer();
    مجازي فراغ vectorize(UText *متن, int32_t پیل_Pos, int32_t پای_ټکی_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کوډ &حالت) ثابت override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_اوږدتیا = 10;

فراغ GraphemeClusterVectorizer::vectorize(
    UText *متن, int32_t پیل_Pos, int32_t پای_ټکی_Pos,
    UVector32 &offsets, UVector32 &indices, UError_کوډ &حالت) ثابت
{
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (!offsets.ensure_ظرفیت(پای_ټکی_Pos - پیل_Pos, حالت) ||
            !indices.ensure_ظرفیت(پای_ټکی_Pos - پیل_Pos, حالت)) {
        أرجع;
    }
    إذا (U_FAILURE(حالت)) أرجع;
    Local_اشاره_ګر<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    graphemeIter->سېټ_متن(متن, حالت);
    إذا (U_FAILURE(حالت)) أرجع;

    إذا (پیل_Pos != 0) {
        graphemeIter->preceding(پیل_Pos);
    }
    int32_t وروستی = پیل_Pos;
    int32_t current = پیل_Pos;
    char16_t str[MAX_GRAPHEME_CLSTER_اوږدتیا];
    طالما ((current = graphemeIter->next()) != BreakIterator::بشپړ_2) {
        إذا (current >= پای_ټکی_Pos) {
            اكسر;
        }
        إذا (current > پیل_Pos) {
            utext_ایستل(متن, وروستی, current, str, MAX_GRAPHEME_CLSTER_اوږدتیا, &حالت);
            إذا (U_FAILURE(حالت)) أرجع;
            offsets.جمع_کړه_Element(وروستی, حالت);
            indices.جمع_کړه_Element(string_منزل_شاخص(str), حالت);
            إذا (U_FAILURE(حالت)) أرجع;
        }
        وروستی = current;
    }
    إذا (U_FAILURE(حالت) || وروستی >= پای_ټکی_Pos) {
        أرجع;
    }
    utext_ایستل(متن, وروستی, پای_ټکی_Pos, str, MAX_GRAPHEME_CLSTER_اوږدتیا, &حالت);
    إذا (U_SUCCESS(حالت)) {
        offsets.جمع_کړه_Element(وروستی, حالت);
        indices.جمع_کړه_Element(string_منزل_شاخص(str), حالت);
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
        .جمع_کړه_Dot_حاصل_ضرب(x, W)
        .جمع_کړه_Dot_حاصل_ضرب(h, U);

    ifco.ټوټه(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.ټوټه(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.ټوټه(2*hunits, hunits).tanh(); // c_: tanh
    ifco.ټوټه(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_حاصل_ضرب(ifco.ټوټه(hunits, hunits))
        .جمع_کړه_Hadamard_حاصل_ضرب(ifco.ټوټه(0, hunits), ifco.ټوټه(2*hunits, hunits));

    h.tanh(c)
        .hadamard_حاصل_ضرب(ifco.ټوټه(3*hunits, hunits));
}

// Minimum word size
ساكن ثابت int32_t MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t MIN_کلمه_پراختیا = MIN_کلمه * 2;

int32_t
LSTMBreakEngine::divide_پورته_Dictionary_Range( UText *متن,
                                                int32_t پیل_Pos,
                                                int32_t پای_ټکی_Pos,
                                                UVector32 &وموندل_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t begin_وموندل_Break_اندازه = وموندل_Breaks.size();
    utext_سېټ_Native_شاخص(متن, پیل_Pos);
    utext_moveIndex32(متن, MIN_کلمه_پراختیا);
    إذا (utext_get_Native_شاخص(متن) >= پای_ټکی_Pos) {
        أرجع 0;       // Not enough characters for two words
    }
    utext_سېټ_Native_شاخص(متن, پیل_Pos);

    UVector32 offsets(حالت);
    UVector32 indices(حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    fVectorizer->vectorize(متن, پیل_Pos, پای_ټکی_Pos, offsets, indices, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t* offsetsBuf = offsets.get_بفر();
    int32_t* indicesBuf = indices.get_بفر();

    int32_t ننوت_seq_len = indices.size();
    int32_t hunits = f_معلومات->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, حالت);

    Array1D c(hunits, حالت);
    Array1D logp(4, حالت);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(ننوت_seq_len, hunits, حالت);

    // Allocate fbRow and slice the internal array in two.
    Array1D fb_لیکه(2 * hunits, حالت);

    // ----- End of all the Array memory allocation needed for this function
    إذا (U_FAILURE(حالت)) أرجع 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    لكل (int32_t i = ننوت_seq_len - 1; i >= 0; i--) {
        Array1D h_لیکه = hBackward.لیکه(i);
        إذا (i != ننوت_seq_len - 1) {
            h_لیکه.assign(hBackward.لیکه(i+1));
        }
#که_تعریف_شوی LSTM_DEBUG
        چاپ_کړه("hRow %d\n", i);
        h_لیکه.print();
        چاپ_کړه("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        چاپ_کړه("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_معلومات->fEmbedding.لیکه(indicesBuf[i]).print();
#د_که_پای  // LSTM_DEBUG
        compute(hunits,
                f_معلومات->fBackwardW, f_معلومات->fBackwardU, f_معلومات->fBackwardB,
                f_معلومات->fEmbedding.لیکه(indicesBuf[i]),
                h_لیکه, c, ifco);
    }


    Array1D forward_لیکه = fb_لیکه.ټوټه(0, hunits);  // point to first half of data in fbRow.
    Array1D backward_لیکه = fb_لیکه.ټوټه(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    لكل (int32_t i = 0; i < ننوت_seq_len; i++) {
#که_تعریف_شوی LSTM_DEBUG
        چاپ_کړه("forwardRow %d\n", i);
        forward_لیکه.print();
#د_که_پای  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_معلومات->fForwardW, f_معلومات->fForwardU, f_معلومات->fForwardB,
                f_معلومات->fEmbedding.لیکه(indicesBuf[i]),
                forward_لیکه, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backward_لیکه.assign(hBackward.لیکه(i));

        logp.assign(f_معلومات->fOutputB).جمع_کړه_Dot_حاصل_ضرب(fb_لیکه, f_معلومات->fOutputW);
#که_تعریف_شوی LSTM_DEBUG
        چاپ_کړه("backwardRow %d\n", i);
        backward_لیکه.print();
        چاپ_کړه("logp %d\n", i);
        logp.print();
#د_که_پای  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_شاخص());
        // BIES logic.
        إذا (current == BEGIN || current == SINGLE) {
            إذا (i != 0) {
                وموندل_Breaks.جمع_کړه_Element(offsetsBuf[i], حالت);
                إذا (U_FAILURE(حالت)) أرجع 0;
            }
        }
    }
    أرجع وموندل_Breaks.size() - begin_وموندل_Break_اندازه;
}

Vectorizer* createVectorizer(ثابت LSTMData* معلومات, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    بدل (معلومات->fType) {
        حالة کوډ_POINTS:
            أرجع جديد کوډ_Points_Vectorizer(معلومات->fDict);
            اكسر;
        حالة GRAPHEME_CLUSTER:
            أرجع جديد GraphemeClusterVectorizer(معلومات->fDict);
            اكسر;
        افتراضي:
            اكسر;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ثابت LSTMData* معلومات, ثابت Unicode_سېټ& سېټ, UError_کوډ &حالت)
    : DictionaryBreakEngine(), f_معلومات(معلومات), fVectorizer(createVectorizer(f_معلومات, حالت))
{
    إذا (U_FAILURE(حالت)) {
      f_معلومات = تش_اشاره;  // If failure, we should not delete fData in destructor because the caller will do so.
      أرجع;
    }
    سېټ_Characters(سېټ);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    ړنګول f_معلومات;
    ړنګول fVectorizer;
}

ثابت char16_t* LSTMBreakEngine::name() ثابت {
    أرجع f_معلومات->fName;
}

UnicodeString defaultLSTM(UScript_کوډ script, UError_کوډ& حالت) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &حالت);
    b = ures_get_By_کلی_With_Fallback(b, "lstm", b, &حالت);
    UnicodeString پایله = ures_get_Unicode_String_By_کلی(b, uscript_getShortName(script), &حالت);
    ures_close(b);
    أرجع پایله;
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_کوډ script, UError_کوډ& حالت)
{
    إذا (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        أرجع تش_اشاره;
    }
    UnicodeString name = defaultLSTM(script, حالت);
    إذا (U_FAILURE(حالت)) أرجع تش_اشاره;
    CharString namebuf;
    namebuf.appendInvariantChars(name, حالت).truncate(namebuf.وروستی_شاخص_د('.'));

    Local_UResource_Bundle_اشاره_ګر rb(
        ures_open_مستقیم(U_ICUDATA_BRKITR, namebuf.معلومات(), &حالت));
    إذا (U_FAILURE(حالت)) أرجع تش_اشاره;

    أرجع CreateLSTMData(rb.orphan(), حالت);
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    ثابت LSTMData* پایله = جديد LSTMData(rb, حالت);
    إذا (U_FAILURE(حالت)) {
        ړنګول پایله;
        أرجع تش_اشاره;
    }
    أرجع پایله;
}

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_کوډ script, ثابت LSTMData* معلومات, UError_کوډ& حالت)
{
    UnicodeString unicode_سېټ_String;
    بدل(script) {
        حالة USCRIPT_THAI:
            unicode_سېټ_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            اكسر;
        حالة USCRIPT_MYANMAR:
            unicode_سېټ_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            اكسر;
        افتراضي:
            ړنګول معلومات;
            أرجع تش_اشاره;
    }
    Unicode_سېټ unicode_سېټ;
    unicode_سېټ.apply_نمونه(unicode_سېټ_String, حالت);
    ثابت LanguageBreakEngine* engine = جديد LSTMBreakEngine(معلومات, unicode_سېټ, حالت);
    إذا (U_FAILURE(حالت) || engine == تش_اشاره) {
        إذا (engine != تش_اشاره) {
            ړنګول engine;
        } وإلا {
            حالت = U_حافظه_ویش_ERROR;
        }
        أرجع تش_اشاره;
    }
    أرجع engine;
}

U_CAPI فراغ U_EXPORT2 DeleteLSTMData(ثابت LSTMData* معلومات)
{
    ړنګول معلومات;
}

U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* معلومات)
{
    أرجع معلومات->fName;
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */

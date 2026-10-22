// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل "unicode/utypes.h"

#اگر U_ENABLE_TRACING

#شامل "restrace.h"
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "utracimp.h"
#شامل "uresimp.h"
#شامل "uassert.h"
#شامل "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = طےشدہ;

خالی ResourceTracer::trace(ثابت حرف* resType) ثابت {
    U_ASSERT(fResB || f_والد);
    UTRACE_اندراج(UTRACE_UDATA_RESOURCE);
    UError_رمز حالت_2 = U_ZERO_ERROR;

    CharString file_راستہ;
    get_File_راستہ(file_راستہ, حالت_2);

    CharString res_راستہ;
    get_Res_راستہ(res_راستہ, حالت_2);

    // The longest type ("intvector") is 9 chars
    ثابت حرف kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, حجم(kSpaces) - 1 - uprv_strlen(resType), حالت_2);
    format.append("(%s) %s @ %s", حالت_2);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.ڈیٹا(),
        resType,
        file_راستہ.ڈیٹا(),
        res_راستہ.ڈیٹا());
    UTRACE_EXIT_حالت(حالت_2);
}

خالی ResourceTracer::traceOpen() ثابت {
    U_ASSERT(fResB);
    UTRACE_اندراج(UTRACE_UDATA_BUNDLE);
    UError_رمز حالت_2 = U_ZERO_ERROR;

    CharString file_راستہ;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_راستہ(file_راستہ, حالت_2).ڈیٹا());
    UTRACE_EXIT_حالت(حالت_2);
}

CharString& ResourceTracer::get_File_راستہ(CharString& output, UError_رمز& حالت_2) ثابت {
    اگر (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_ڈیٹا->f_راستہ, حالت_2);
        output.append('/', حالت_2);
        output.append(fResB->f_ڈیٹا->fName, حالت_2);
        output.append(".res", حالت_2);
    } ورنہ {
        f_والد->get_File_راستہ(output, حالت_2);
    }
    واپس output;
}

CharString& ResourceTracer::get_Res_راستہ(CharString& output, UError_رمز& حالت_2) ثابت {
    اگر (fResB) {
        output.append('/', حالت_2);
        output.append(fResB->f_Res_راستہ, حالت_2);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } ورنہ {
        f_والد->get_Res_راستہ(output, حالت_2);
    }
    اگر (f_کلید) {
        output.append('/', حالت_2);
        output.append(f_کلید, حالت_2);
    }
    اگر (f_اشاریہ != -1) {
        output.append('[', حالت_2);
        UnicodeString اشاریہ_String;
        ICU_Utility::append_نمبر(اشاریہ_String, f_اشاریہ);
        output.appendInvariantChars(اشاریہ_String, حالت_2);
        output.append(']', حالت_2);
    }
    واپس output;
}

خالی FileTracer::traceOpen(ثابت حرف* راستہ, ثابت حرف* type, ثابت حرف* name) {
    اگر (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(راستہ, name);
    } ورنہ {
        trace_Open_ڈیٹا_File(راستہ, type, name);
    }
}

خالی FileTracer::trace_Open_ڈیٹا_File(ثابت حرف* راستہ, ثابت حرف* type, ثابت حرف* name) {
    UTRACE_اندراج(UTRACE_UDATA_ڈیٹا_FILE);
    UError_رمز حالت_2 = U_ZERO_ERROR;

    CharString file_راستہ;
    file_راستہ.append(راستہ, حالت_2);
    file_راستہ.append('/', حالت_2);
    file_راستہ.append(name, حالت_2);
    file_راستہ.append('.', حالت_2);
    file_راستہ.append(type, حالت_2);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_راستہ.ڈیٹا());
    UTRACE_EXIT_حالت(حالت_2);
}

خالی FileTracer::traceOpenResFile(ثابت حرف* راستہ, ثابت حرف* name) {
    UTRACE_اندراج(UTRACE_UDATA_RES_FILE);
    UError_رمز حالت_2 = U_ZERO_ERROR;

    CharString file_راستہ;
    file_راستہ.append(راستہ, حالت_2);
    file_راستہ.append('/', حالت_2);
    file_راستہ.append(name, حالت_2);
    file_راستہ.append(".res", حالت_2);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_راستہ.ڈیٹا());
    UTRACE_EXIT_حالت(حالت_2);
}

U_NAMESPACE_انتہا

#اگر_ختم // U_ENABLE_TRACING

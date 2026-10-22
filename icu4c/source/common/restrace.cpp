// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا U_ENABLE_TRACING

#ضمن "restrace.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "utracimp.h"
#ضمن "uresimp.h"
#ضمن "uassert.h"
#ضمن "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = افتراضي;

فراغ ResourceTracer::trace(ثابت حرف* resType) ثابت {
    U_ASSERT(fResB || f_ماپا);
    UTRACE_اندراج(UTRACE_UDATA_RESOURCE);
    UError_کوڈ حالت = U_ZERO_ERROR;

    CharString file_رستہ;
    get_File_رستہ(file_رستہ, حالت);

    CharString res_رستہ;
    get_Res_رستہ(res_رستہ, حالت);

    // The longest type ("intvector") is 9 chars
    ثابت حرف kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, حجم(kSpaces) - 1 - uprv_strlen(resType), حالت);
    format.append("(%s) %s @ %s", حالت);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.انکڑے(),
        resType,
        file_رستہ.انکڑے(),
        res_رستہ.انکڑے());
    UTRACE_EXIT_حالت(حالت);
}

فراغ ResourceTracer::traceOpen() ثابت {
    U_ASSERT(fResB);
    UTRACE_اندراج(UTRACE_UDATA_BUNDLE);
    UError_کوڈ حالت = U_ZERO_ERROR;

    CharString file_رستہ;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_رستہ(file_رستہ, حالت).انکڑے());
    UTRACE_EXIT_حالت(حالت);
}

CharString& ResourceTracer::get_File_رستہ(CharString& output, UError_کوڈ& حالت) ثابت {
    إذا (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_انکڑے->f_رستہ, حالت);
        output.append('/', حالت);
        output.append(fResB->f_انکڑے->fName, حالت);
        output.append(".res", حالت);
    } وإلا {
        f_ماپا->get_File_رستہ(output, حالت);
    }
    أرجع output;
}

CharString& ResourceTracer::get_Res_رستہ(CharString& output, UError_کوڈ& حالت) ثابت {
    إذا (fResB) {
        output.append('/', حالت);
        output.append(fResB->f_Res_رستہ, حالت);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } وإلا {
        f_ماپا->get_Res_رستہ(output, حالت);
    }
    إذا (f_کلید) {
        output.append('/', حالت);
        output.append(f_کلید, حالت);
    }
    إذا (f_اشاریہ != -1) {
        output.append('[', حالت);
        UnicodeString اشاریہ_String;
        ICU_Utility::append_نمبر(اشاریہ_String, f_اشاریہ);
        output.appendInvariantChars(اشاریہ_String, حالت);
        output.append(']', حالت);
    }
    أرجع output;
}

فراغ FileTracer::traceOpen(ثابت حرف* رستہ, ثابت حرف* type, ثابت حرف* name) {
    إذا (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(رستہ, name);
    } وإلا {
        trace_Open_انکڑے_File(رستہ, type, name);
    }
}

فراغ FileTracer::trace_Open_انکڑے_File(ثابت حرف* رستہ, ثابت حرف* type, ثابت حرف* name) {
    UTRACE_اندراج(UTRACE_UDATA_انکڑے_FILE);
    UError_کوڈ حالت = U_ZERO_ERROR;

    CharString file_رستہ;
    file_رستہ.append(رستہ, حالت);
    file_رستہ.append('/', حالت);
    file_رستہ.append(name, حالت);
    file_رستہ.append('.', حالت);
    file_رستہ.append(type, حالت);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_رستہ.انکڑے());
    UTRACE_EXIT_حالت(حالت);
}

فراغ FileTracer::traceOpenResFile(ثابت حرف* رستہ, ثابت حرف* name) {
    UTRACE_اندراج(UTRACE_UDATA_RES_FILE);
    UError_کوڈ حالت = U_ZERO_ERROR;

    CharString file_رستہ;
    file_رستہ.append(رستہ, حالت);
    file_رستہ.append('/', حالت);
    file_رستہ.append(name, حالت);
    file_رستہ.append(".res", حالت);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_رستہ.انکڑے());
    UTRACE_EXIT_حالت(حالت);
}

U_NAMESPACE_سرا

#جے_انت // U_ENABLE_TRACING

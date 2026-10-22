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

ResourceTracer::~ResourceTracer() = پیشفرض;

تهی ResourceTracer::trace(ثابت نویسه* resType) ثابت {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    ثابت نویسه kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, اندازه(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

تهی ResourceTracer::traceOpen() ثابت {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) ثابت {
    اگر (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } وگرنه {
        fParent->getFilePath(output, status);
    }
    بازگشت output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) ثابت {
    اگر (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } وگرنه {
        fParent->getResPath(output, status);
    }
    اگر (f_کلید) {
        output.append('/', status);
        output.append(f_کلید, status);
    }
    اگر (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_عدد(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    بازگشت output;
}

تهی FileTracer::traceOpen(ثابت نویسه* path, ثابت نویسه* type, ثابت نویسه* name) {
    اگر (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } وگرنه {
        traceOpenDataFile(path, type, name);
    }
}

تهی FileTracer::traceOpenDataFile(ثابت نویسه* path, ثابت نویسه* type, ثابت نویسه* name) {
    UTRACE_ENTRY(UTRACE_UDATA_DATA_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append('.', status);
    filePath.append(type, status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

تهی FileTracer::traceOpenResFile(ثابت نویسه* path, ثابت نویسه* name) {
    UTRACE_ENTRY(UTRACE_UDATA_RES_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append(".res", status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

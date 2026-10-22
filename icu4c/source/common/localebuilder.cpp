// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن <optional>
#ضمن <string_view>
#ضمن <utility>

#ضمن "bytesinkutil.h"  // StringByteSink<CharString>
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "fixedstring.h"
#ضمن "ulocimp.h"
#ضمن "unicode/localebuilder.h"
#ضمن "unicode/locid.h"

نطاق {

مضمن منطقي UPRV_ISDIGIT(حرف c) { أرجع c >= '0' && c <= '9'; }
مضمن منطقي UPRV_ISALPHANUM(حرف c) { أرجع uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ثابت حرف* k_Attribute_ڪنجي = "attribute";

منطقي _isExtensionSubtags(حرف ڪنجي, ثابت حرف* s, int32_t len) {
    بدل (uprv_tolower(ڪنجي)) {
        حالة 'u':
            أرجع ultag_isUnicodeExtensionSubtags(s, len);
        حالة 't':
            أرجع ultag_isTransformedExtensionSubtags(s, len);
        حالة 'x':
            أرجع ultag_is_Privateuse_قدر_Subtags(s, len);
        افتراضي:
            أرجع ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), حالت_2(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(خالي_اشارو), extensions_(خالي_اشارو)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    ميٽيو variant_;
    ميٽيو extensions_;
}

LocaleBuilder& LocaleBuilder::سيٽ_Locale(ثابت Locale& locale) U_LIFETIME_BOUND
{
    clear();
    سيٽ_Language(locale.getLanguage());
    سيٽ_Script(locale.getScript());
    سيٽ_Region(locale.getCountry());
    سيٽ_Variant(locale.getVariant());
    extensions_ = locale.clone();
    إذا (extensions_ == خالي_اشارو) {
        حالت_2 = U_يادگيري_ورهاست_ERROR;
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سيٽ_Language_ٽيگ(StringPiece ٽيگ) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_ٽيگ(ٽيگ, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    سيٽ_Locale(l);
    أرجع *هذا;
}

نطاق {

فراغ سيٽ_ميدان(StringPiece input, حرف* dest, UError_ڪوڊ& error_ڪوڊ,
              منطقي (*test)(ثابت حرف*, int32_t)) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    إذا (input.empty()) {
        dest[0] = '\0';
    } وإلا إذا (test(input.معلومات(), input.length())) {
        uprv_memcpy(dest, input.معلومات(), input.length());
        dest[input.length()] = '\0';
    } وإلا {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::سيٽ_Language(StringPiece language) U_LIFETIME_BOUND
{
    سيٽ_ميدان(language, language_, حالت_2, &ultag_isLanguageSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سيٽ_Script(StringPiece script) U_LIFETIME_BOUND
{
    سيٽ_ميدان(script, script_, حالت_2, &ultag_isScriptSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سيٽ_Region(StringPiece region) U_LIFETIME_BOUND
{
    سيٽ_ميدان(region, region_, حالت_2, &ultag_isRegionSubtag);
    أرجع *هذا;
}

نطاق {

فراغ transform(حرف* معلومات, int32_t len) {
    لكل (int32_t i = 0; i < len; i++, معلومات++) {
        إذا (*معلومات == '_') {
            *معلومات = '-';
        } وإلا {
            *معلومات = uprv_tolower(*معلومات);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::سيٽ_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (variant.empty()) {
        ميٽيو variant_;
        variant_ = خالي_اشارو;
        أرجع *هذا;
    }
    FixedString* new_variant = جديد FixedString(variant);
    إذا (new_variant == خالي_اشارو || new_variant->is_خالي_جايون()) {
        حالت_2 = U_يادگيري_ورهاست_ERROR;
        أرجع *هذا;
    }
    transform(new_variant->getAlias(), variant.length());
    إذا (!ultag_isVariantSubtags(new_variant->معلومات(), variant.length())) {
        ميٽيو new_variant;
        حالت_2 = U_ILLEGAL_دليل_ERROR;
        أرجع *هذا;
    }
    ميٽيو variant_;
    variant_ = new_variant;
    أرجع *هذا;
}

نطاق {

منطقي
is_Keyword_قدر(ثابت حرف* ڪنجي, ثابت حرف* value, int32_t قدر_len)
{
    إذا (ڪنجي[1] == '\0') {
        // one char key
        أرجع (UPRV_ISALPHANUM(uprv_tolower(ڪنجي[0])) &&
                _isExtensionSubtags(ڪنجي[0], value, قدر_len));
    } وإلا إذا (uprv_strcmp(ڪنجي, k_Attribute_ڪنجي) == 0) {
        // unicode attributes
        أرجع ultag_is_Unicode_Locale_خاصيتون(value, قدر_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_ڪنجي = ulocimp_منزل_Bcp_ڪنجي_With_Fallback(ڪنجي);
    std::optional<std::string_view> unicode_locale_type = ulocimp_منزل_Bcp_Type_With_Fallback(ڪنجي, value);

    أرجع unicode_locale_ڪنجي.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_ڪنجي(unicode_locale_ڪنجي->معلومات(),
                                    static_cast<int32_t>(unicode_locale_ڪنجي->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->معلومات(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

فراغ
_copyExtensions(ثابت Locale& from, icu::StringEnumeration *keywords,
                Locale& منزل, منطقي validate, UError_ڪوڊ& error_ڪوڊ)
{
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    Local_اشارو<icu::StringEnumeration> ownedKeywords;
    إذا (keywords == خالي_اشارو) {
        ownedKeywords.adoptInstead(from.createKeywords(error_ڪوڊ));
        إذا (U_FAILURE(error_ڪوڊ) || ownedKeywords.isNull()) { أرجع; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت حرف* ڪنجي;
    طالما ((ڪنجي = keywords->next(خالي_اشارو, error_ڪوڊ)) != خالي_اشارو) {
        خودڪار value = from.get_Keyword_قدر<CharString>(ڪنجي, error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        إذا (uprv_strcmp(ڪنجي, k_Attribute_ڪنجي) == 0) {
            transform(value.معلومات(), value.length());
        }
        إذا (validate &&
            !is_Keyword_قدر(ڪنجي, value.معلومات(), value.length())) {
            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
            أرجع;
        }
        منزل.سيٽ_Keyword_قدر(ڪنجي, value.معلومات(), error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    }
}

فراغ
clear_UAttributes_And_ڪنجي_Type(Locale& locale, UError_ڪوڊ& error_ڪوڊ)
{
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    // Clear Unicode attributes
    locale.سيٽ_Keyword_قدر(k_Attribute_ڪنجي, "", error_ڪوڊ);

    // Clear all Unicode keyword values
    Local_اشارو<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_ڪوڊ));
    إذا (U_FAILURE(error_ڪوڊ) || iter.isNull()) { أرجع; }
    ثابت حرف* ڪنجي;
    طالما ((ڪنجي = iter->next(خالي_اشارو, error_ڪوڊ)) != خالي_اشارو) {
        locale.سيٽ_Unicode_Keyword_قدر(ڪنجي, خالي_اشارو, error_ڪوڊ);
    }
}

فراغ
سيٽ_Unicode_Extensions(Locale& locale, ثابت CharString& value, UError_ڪوڊ& error_ڪوڊ)
{
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_ڪوڊ);
    locale_str.append(value, error_ڪوڊ);
    _copyExtensions(
        Locale::for_Language_ٽيگ(locale_str.معلومات(), error_ڪوڊ), خالي_اشارو,
        locale, خطأ, error_ڪوڊ);
}

}  // namespace

LocaleBuilder& LocaleBuilder::سيٽ_Extension(حرف ڪنجي, StringPiece value) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!UPRV_ISALPHANUM(ڪنجي)) {
        حالت_2 = U_ILLEGAL_دليل_ERROR;
        أرجع *هذا;
    }
    CharString قدر_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(قدر_str.معلومات(), قدر_str.length());
    إذا (!قدر_str.is_خالي_جايون() &&
            !_isExtensionSubtags(ڪنجي, قدر_str.معلومات(), قدر_str.length())) {
        حالت_2 = U_ILLEGAL_دليل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == خالي_اشارو) {
        extensions_ = Locale::get_پاڙ().clone();
        إذا (extensions_ == خالي_اشارو) {
            حالت_2 = U_يادگيري_ورهاست_ERROR;
            أرجع *هذا;
        }
    }
    إذا (uprv_tolower(ڪنجي) != 'u') {
        // for t, x and others extension.
        extensions_->سيٽ_Keyword_قدر(StringPiece(&ڪنجي, 1), قدر_str.معلومات(),
                                     حالت_2);
        أرجع *هذا;
    }
    clear_UAttributes_And_ڪنجي_Type(*extensions_, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!value.empty()) {
        سيٽ_Unicode_Extensions(*extensions_, قدر_str, حالت_2);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سيٽ_Unicode_Locale_Keyword(StringPiece ڪنجي,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!ultag_is_Unicode_Locale_ڪنجي(ڪنجي.معلومات(), ڪنجي.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.معلومات(), type.length()))) {
      حالت_2 = U_ILLEGAL_دليل_ERROR;
      أرجع *هذا;
    }
    إذا (extensions_ == خالي_اشارو) {
        extensions_ = Locale::get_پاڙ().clone();
        إذا (extensions_ == خالي_اشارو) {
            حالت_2 = U_يادگيري_ورهاست_ERROR;
            أرجع *هذا;
        }
    }
    extensions_->سيٽ_Unicode_Keyword_قدر(ڪنجي, type, حالت_2);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::جوڙيو_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString قدر_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(قدر_str.معلومات(), قدر_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(قدر_str.معلومات(), قدر_str.length())) {
        حالت_2 = U_ILLEGAL_دليل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == خالي_اشارو) {
        extensions_ = Locale::get_پاڙ().clone();
        إذا (extensions_ == خالي_اشارو) {
            حالت_2 = U_يادگيري_ورهاست_ERROR;
            أرجع *هذا;
        }
        extensions_->سيٽ_Keyword_قدر(k_Attribute_ڪنجي, قدر_str.معلومات(), حالت_2);
        أرجع *هذا;
    }

    UError_ڪوڊ local_Error_ڪوڊ = U_ZERO_ERROR;
    خودڪار خاصيتون = extensions_->get_Keyword_قدر<CharString>(k_Attribute_ڪنجي, local_Error_ڪوڊ);
    إذا (U_FAILURE(local_Error_ڪوڊ)) {
        CharString new_خاصيتون(قدر_str.معلومات(), حالت_2);
        // No attributes, set the attribute.
        extensions_->سيٽ_Keyword_قدر(k_Attribute_ڪنجي, new_خاصيتون.معلومات(), حالت_2);
        أرجع *هذا;
    }

    transform(خاصيتون.معلومات(),خاصيتون.length());
    ثابت حرف* شروعات = خاصيتون.معلومات();
    ثابت حرف* حد_بندي = خاصيتون.معلومات() + خاصيتون.length();
    CharString new_خاصيتون;
    منطقي inserted = خطأ;
    طالما (شروعات < حد_بندي) {
        إذا (!inserted) {
            صحيح cmp = uprv_strcmp(شروعات, قدر_str.معلومات());
            إذا (cmp == 0) { أرجع *هذا; }  // Found it in attributes: Just return
            إذا (cmp > 0) {
                إذا (!new_خاصيتون.is_خالي_جايون()) new_خاصيتون.append('_', حالت_2);
                new_خاصيتون.append(قدر_str.معلومات(), حالت_2);
                inserted = سچ;
            }
        }
        إذا (!new_خاصيتون.is_خالي_جايون()) {
            new_خاصيتون.append('_', حالت_2);
        }
        new_خاصيتون.append(شروعات, حالت_2);
        شروعات += uprv_strlen(شروعات) + 1;
    }
    إذا (!inserted) {
        إذا (!new_خاصيتون.is_خالي_جايون()) {
            new_خاصيتون.append('_', حالت_2);
        }
        new_خاصيتون.append(قدر_str.معلومات(), حالت_2);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->سيٽ_Keyword_قدر(k_Attribute_ڪنجي, new_خاصيتون.معلومات(), حالت_2);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString قدر_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(قدر_str.معلومات(), قدر_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(قدر_str.معلومات(), قدر_str.length())) {
        حالت_2 = U_ILLEGAL_دليل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == خالي_اشارو) { أرجع *هذا; }
    UError_ڪوڊ local_Error_ڪوڊ = U_ZERO_ERROR;
    خودڪار خاصيتون = extensions_->get_Keyword_قدر<CharString>(k_Attribute_ڪنجي, local_Error_ڪوڊ);
    // get failure, just return
    إذا (U_FAILURE(local_Error_ڪوڊ)) { أرجع *هذا; }
    // Do not have any attributes, just return.
    إذا (خاصيتون.is_خالي_جايون()) { أرجع *هذا; }

    حرف* p = خاصيتون.معلومات();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    لكل (int32_t i = 0; i < خاصيتون.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ثابت حرف* شروعات = خاصيتون.معلومات();
    ثابت حرف* حد_بندي = خاصيتون.معلومات() + خاصيتون.length();
    CharString new_خاصيتون;
    منطقي مليو = خطأ;
    طالما (شروعات < حد_بندي) {
        إذا (uprv_strcmp(شروعات, قدر_str.معلومات()) == 0) {
            مليو = سچ;
        } وإلا {
            إذا (!new_خاصيتون.is_خالي_جايون()) {
                new_خاصيتون.append('_', حالت_2);
            }
            new_خاصيتون.append(شروعات, حالت_2);
        }
        شروعات += uprv_strlen(شروعات) + 1;
    }
    // Found the value in attributes, set the attribute.
    إذا (مليو) {
        extensions_->سيٽ_Keyword_قدر(k_Attribute_ڪنجي, new_خاصيتون.معلومات(), حالت_2);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    حالت_2 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    ميٽيو variant_;
    variant_ = خالي_اشارو;
    clearExtensions();
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    ميٽيو extensions_;
    extensions_ = خالي_اشارو;
    أرجع *هذا;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.سيٽ_منزل_Bogus();
  أرجع bogus;
}

فراغ LocaleBuilder::copyExtensionsFrom(ثابت Locale& src, UError_ڪوڊ& error_ڪوڊ)
{
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    Local_اشارو<icu::StringEnumeration> keywords(src.createKeywords(error_ڪوڊ));
    إذا (U_FAILURE(error_ڪوڊ) || keywords.isNull() || keywords->count(error_ڪوڊ) == 0) {
        // Error, or no extensions to copy.
        أرجع;
    }
    إذا (extensions_ == خالي_اشارو) {
        extensions_ = Locale::get_پاڙ().clone();
        إذا (extensions_ == خالي_اشارو) {
            حالت_2 = U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, خطأ, error_ڪوڊ);
}

Locale LocaleBuilder::build(UError_ڪوڊ& error_ڪوڊ)
{
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع makeBogusLocale();
    }
    إذا (U_FAILURE(حالت_2)) {
        error_ڪوڊ = حالت_2;
        أرجع makeBogusLocale();
    }
    CharString locale_str(language_, error_ڪوڊ);
    إذا (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_ڪوڊ).append(StringPiece(script_), error_ڪوڊ);
    }
    إذا (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_ڪوڊ).append(StringPiece(region_), error_ڪوڊ);
    }
    إذا (variant_ != خالي_اشارو) {
        locale_str.append('-', error_ڪوڊ).append(StringPiece(variant_->معلومات()), error_ڪوڊ);
    }
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع makeBogusLocale();
    }
    Locale حاصل_ضرب(locale_str.معلومات());
    إذا (extensions_ != خالي_اشارو) {
        _copyExtensions(*extensions_, خالي_اشارو, حاصل_ضرب, سچ, error_ڪوڊ);
    }
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع makeBogusLocale();
    }
    أرجع حاصل_ضرب;
}

UBool LocaleBuilder::copy_Error_منزل(UError_ڪوڊ &ٻاهريون_Error_ڪوڊ) ثابت {
    إذا (U_FAILURE(ٻاهريون_Error_ڪوڊ)) {
        // Do not overwrite the older error code
        أرجع سچ;
    }
    ٻاهريون_Error_ڪوڊ = حالت_2;
    أرجع U_FAILURE(ٻاهريون_Error_ڪوڊ);
}

U_NAMESPACE_پڇاڙي

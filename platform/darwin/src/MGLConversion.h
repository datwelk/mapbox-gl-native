#include <mbgl/style/conversion.hpp>

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
namespace style {
namespace conversion {

class Holder {
public:
    Holder(const id v) : value(v) {}
    const id value;
};

template <>
class ValueTraits<Holder> {
public:
    static bool isUndefined(const Holder& holder) {
        const id value = holder.value;
        return !value || value == [NSNull null];
    }

    static bool isArray(const Holder& holder) {
        const id value = holder.value;
        return [value isKindOfClass:[NSArray class]];
    }

    static bool isObject(const Holder& holder) {
        const id value = holder.value;
        return [value isKindOfClass:[NSDictionary class]];
    }

    static std::size_t arrayLength(const Holder& holder) {
        const id value = holder.value;
        NSCAssert([value isKindOfClass:[NSArray class]], @"Value must be an NSArray for getLength().");
        NSArray *array = value;
        auto length = [array count];
        NSCAssert(length <= std::numeric_limits<size_t>::max(), @"Array length out of bounds.");
        return length;
    }

    static Holder arrayMember(const Holder& holder, std::size_t i) {
        const id value = holder.value;
        NSCAssert([value isKindOfClass:[NSArray class]], @"Value must be an NSArray for get(int).");
        NSCAssert(i < NSUIntegerMax, @"Index must be less than NSUIntegerMax");
        return {[value objectAtIndex: i]};
    }

    static optional<Holder> objectMember(const Holder& holder, const char *key) {
        const id value = holder.value;
        NSCAssert([value isKindOfClass:[NSDictionary class]], @"Value must be an NSDictionary for get(string).");
        NSObject *member = [value objectForKey: @(key)];
        if (member && member != [NSNull null]) {
            return {member};
        } else {
            return {};
        }
    }

    static optional<Error> eachMember(const Holder& holder, const std::function<optional<Error> (const std::string&, const Holder&)>& fn) {
        // Not implemented (unneeded for MGLStyleFunction conversion).
        NSCAssert(NO, @"eachMember not implemented");
        return {};
    }

    static optional<bool> toBool(const Holder& holder) {
        const id value = holder.value;
        if (_isBool(value)) {
            return ((NSNumber *)value).boolValue;
        } else {
            return {};
        }
    }

    static optional<float> toNumber(const Holder& holder) {
        const id value = holder.value;
        if (_isNumber(value)) {
            return ((NSNumber *)value).floatValue;
        } else {
            return {};
        }
    }

    static optional<double> toDouble(const Holder& holder) {
        const id value = holder.value;
        if (_isNumber(value)) {
            return ((NSNumber *)value).doubleValue;
        } else {
            return {};
        }
    }

    static optional<std::string> toString(const Holder& holder) {
        const id value = holder.value;
        if (_isString(value)) {
            return std::string(static_cast<const char *>([value UTF8String]));
        } else {
            return {};
        }
    }

    static optional<mbgl::Value> toValue(const Holder& holder) {
        const id value = holder.value;
        if (isUndefined(value)) {
            return {};
        } else if (_isBool(value)) {
            return { *toBool(holder) };
        } else if ( _isString(value)) {
            return { *toString(holder) };
        } else if (_isNumber(value)) {
            // Need to cast to a double here as the float is otherwise considered a bool...
           return { static_cast<double>(*toNumber(holder)) };
        } else {
            return {};
        }
    }

    static optional<GeoJSON> toGeoJSON(const Holder& holder, Error& error) {
        error = { "toGeoJSON not implemented" };
        return {};
    }

private:
    static bool _isBool(const id value) {
        if (![value isKindOfClass:[NSNumber class]]) return false;
        // char: 32-bit boolean
        // BOOL: 64-bit boolean
        NSNumber *number = value;
        return ((strcmp([number objCType], @encode(char)) == 0) ||
                (strcmp([number objCType], @encode(BOOL)) == 0));
    }

    static bool _isNumber(const id value) {
        return [value isKindOfClass:[NSNumber class]] && !_isBool(value);
    }

    static bool _isString(const id value) {
        return [value isKindOfClass:[NSString class]];
    }
};

inline Value makeValue(const id value) {
    return {Holder(value)};
}

} // namespace conversion
} // namespace style
} // namespace mbgl

NS_ASSUME_NONNULL_END

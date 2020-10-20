typedef struct JsonObject {
    struct {
        char *name;
        JsonValue *value;
    } *fields;
} JsonObject;

typedef struct JsonArray {
    JsonValue *values;
} JsonArray;

typedef struct JsonValue {
    enum {
        JSON_OBJECT, JSON_ARRAY, JSON_STRING, JSON_NUMBER, JSON_BOOLEAN
    } type;

    union {
        JsonObject *object;
        JsonArray *array;
        char *string;
        char *number;
        unsigned char boolean;
    };
} JsonValue;
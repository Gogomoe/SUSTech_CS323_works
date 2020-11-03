#ifndef JSON_H
#define JSON_H

typedef struct JsonObject JsonObject;
typedef struct JsonArray JsonArray;
typedef struct JsonValue JsonValue;
typedef struct JsonObjectField JsonObjectField;
typedef struct JsonArrayField JsonArrayField;

struct JsonObjectField {
    char *name;
    JsonValue *value;
    JsonObjectField *next;
};

struct JsonObject {
    JsonObjectField *fields;
};

struct JsonArrayField {
    JsonValue *value;
    JsonArrayField *next;
};

struct JsonArray {
    JsonArrayField *values;
};

struct JsonValue {
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
};

#endif
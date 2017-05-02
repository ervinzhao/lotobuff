#define LUA_LIB

extern "C" {
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>
}
#include "proto_codec.h"

const char *meta_table_name = "lotobuf.ProtoCodec";
static void message_to_table(lua_State *L, const protobuf::Message *message);
static bool table_to_message(lua_State *L, protobuf::Message *message);

static void simple_field_to_stack(lua_State *L,
                                  const protobuf::Message *message,
                                  const protobuf::FieldDescriptor *field,
                                  const protobuf::Reflection *reflection,
                                  bool repeated,
                                  int index)

{
    protobuf::FieldDescriptor::Type field_type = field->type();
    lua_Number number = 0;
    std::string scratch;
    switch(field_type) {
    case protobuf::FieldDescriptor::TYPE_BOOL:
        bool ret;
        if(repeated)
            ret = reflection->GetRepeatedBool(*message, field, index);
        else
            ret = reflection->GetBool(*message, field);
        lua_pushboolean(L, ret?1:0);
        break;
    case protobuf::FieldDescriptor::TYPE_STRING:
    case protobuf::FieldDescriptor::TYPE_BYTES:
        const char *str;
        size_t len;
        if(repeated) {
            const std::string &ret =
                    reflection->GetRepeatedStringReference
                    (*message, field, index, &scratch);
            str = ret.c_str();
            len = ret.length();
        } else {
            const std::string &ret =
                    reflection->GetStringReference
                    (*message, field, &scratch);
            str = ret.c_str();
            len = ret.length();
        }
        lua_pushlstring(L, str, len);
        break;
    case protobuf::FieldDescriptor::TYPE_INT32:
    case protobuf::FieldDescriptor::TYPE_SINT32:
    case protobuf::FieldDescriptor::TYPE_FIXED32:
    case protobuf::FieldDescriptor::TYPE_SFIXED32:
        if(repeated)
            number = (lua_Number) reflection->GetRepeatedInt32(*message, field, index);
        else
            number = (lua_Number) reflection->GetInt32(*message, field);
        lua_pushnumber(L, number);
        break;
    case protobuf::FieldDescriptor::TYPE_INT64:
    case protobuf::FieldDescriptor::TYPE_SINT64:
    case protobuf::FieldDescriptor::TYPE_FIXED64:
    case protobuf::FieldDescriptor::TYPE_SFIXED64:
        if(repeated)
            number = (lua_Number) reflection->GetRepeatedInt64(*message, field, index);
        else
            number = (lua_Number) reflection->GetInt64(*message, field);
        lua_pushnumber(L, number);
        break;
    case protobuf::FieldDescriptor::TYPE_UINT32:
        if(repeated)
            number = (lua_Number) reflection->GetRepeatedUInt32(*message, field, index);
        else
            number = (lua_Number) reflection->GetUInt32(*message, field);
        lua_pushnumber(L, number);
        break;
    case protobuf::FieldDescriptor::TYPE_UINT64:
        if(repeated)
            number = (lua_Number) reflection->GetRepeatedUInt64(*message, field, index);
        else
            number = (lua_Number) reflection->GetUInt64(*message, field);
        lua_pushnumber(L, number);
        break;
    case protobuf::FieldDescriptor::TYPE_FLOAT:
        if(repeated)
            number = (lua_Number) reflection->GetRepeatedFloat(*message, field, index);
        else
            number = (lua_Number) reflection->GetFloat(*message, field);
        lua_pushnumber(L, number);
        break;
    case protobuf::FieldDescriptor::TYPE_DOUBLE:
        if(repeated)
            number = (lua_Number) reflection->GetRepeatedDouble(*message, field, index);
        else
            number = (lua_Number) reflection->GetDouble(*message, field);
        lua_pushnumber(L, number);
        break;
    case protobuf::FieldDescriptor::TYPE_ENUM:
        if(repeated)
            number = (lua_Number) reflection->GetRepeatedEnumValue(*message, field, index);
        else
            number = (lua_Number) reflection->GetEnumValue(*message, field);
        lua_pushnumber(L, number);
        break;
    }
}

static void field_to_stack(lua_State *L,
                           const protobuf::Message *message,
                           const protobuf::FieldDescriptor *field,
                           const protobuf::Reflection *reflection)
{
    protobuf::FieldDescriptor::Type field_type = field->type();
    if(field->is_repeated()) {
        int count = reflection->FieldSize(*message, field);

        lua_newtable(L);
        for(int i = 0; i < count; i++) {
            lua_pushinteger(L, i + 1);

            if(field_type == protobuf::FieldDescriptor::TYPE_GROUP
                    || field_type == protobuf::FieldDescriptor::TYPE_MESSAGE) {
                lua_newtable(L);
                const protobuf::Message &sub_message =
                        reflection->GetRepeatedMessage(*message, field, i);
                message_to_table(L, &sub_message);
            } else {
                simple_field_to_stack(L, message, field, reflection, true, i);
            }
            lua_settable(L, -3);
        }
    } else {
        if(field_type == protobuf::FieldDescriptor::TYPE_GROUP
                || field_type == protobuf::FieldDescriptor::TYPE_MESSAGE) {
            lua_newtable(L);
            const protobuf::Message &sub_message =
                    reflection->GetMessage(*message, field);
            message_to_table(L, &sub_message);
        } else {
            simple_field_to_stack(L, message, field, reflection, false, 0);
        }
    }
}

void simple_field_to_message(lua_State *L,
                             protobuf::Message *message,
                             const protobuf::FieldDescriptor *field,
                             const protobuf::Reflection *reflection,
                             bool repeated)
{}

void field_to_message(lua_State *L,
                      protobuf::Message *message,
                      const protobuf::FieldDescriptor *field,
                      const protobuf::Reflection *reflection)
{
    protobuf::FieldDescriptor::Type field_type = field->type();
    if(field->is_repeated()) {
        if(lua_istable(L, -1)) {
            int count = lua_objlen(L, -1);
            for(int i = 0; i < count; i++) {
                lua_pushinteger(L, i + 1);
                lua_gettable(L, -2);
                if(field_type == protobuf::FieldDescriptor::TYPE_GROUP
                        || field_type == protobuf::FieldDescriptor::TYPE_MESSAGE) {
                    protobuf::Message *sub_message =
                            reflection->MutableRepeatedMessage(message, field, i);
                    table_to_message(L, sub_message);
                } else {
                    simple_field_to_message(L, message, field, reflection, true);
                }
            }
        } else {
            protobuf::Message *sub_message =
                    reflection->MutableRepeatedMessage(message, field, 0);
            table_to_message(L, sub_message);
        }
    } else {
        if(field_type == protobuf::FieldDescriptor::TYPE_GROUP
                || field_type == protobuf::FieldDescriptor::TYPE_MESSAGE) {
            protobuf::Message *sub_message =
                    reflection->MutableMessage(message, field);
            table_to_message(L, sub_message);
        } else {
            simple_field_to_message(L, message, field, reflection, false);
        }
    }
}

bool table_to_message(lua_State *L, protobuf::Message *message)
{
    const protobuf::Descriptor *descriptor = message->GetDescriptor();
    const protobuf::Reflection *reflection = message->GetReflection();
    int count = descriptor->field_count();
    for(int i = 0; i < count; i ++) {
        const protobuf::FieldDescriptor *field = descriptor->field(i);
        const std::string &field_name = field->name();

        int top = lua_gettop(L);
        lua_pushstring(L, field_name.c_str());
        lua_gettable(L, -2);
        if(lua_isnil(L, -1)) {

        } else {
            field_to_message(L, message, field, reflection);
        }
        lua_settop(L, top);
    }
}

void message_to_table(lua_State *L, const protobuf::Message *message)
{
    const protobuf::Descriptor *descriptor = message->GetDescriptor();
    const protobuf::Reflection *reflection = message->GetReflection();
    int count = descriptor->field_count();
    for(int i = 0; i < count; i ++) {
        const protobuf::FieldDescriptor *field = descriptor->field(i);
        const std::string &field_name = field->name();
        lua_pushstring(L, field_name.c_str());
        field_to_stack(L, message, field, reflection);
        lua_settable(L, -3);
    }
}

static int lotobuf_new(lua_State *L) {
    lua_settop(L, 0);

    ProtoCodec *codec = new ProtoCodec;
    void * ud = lua_newuserdata(L, sizeof(codec));
    *((ProtoCodec **) ud) = codec;

    luaL_getmetatable(L, meta_table_name);
    lua_setmetatable(L, -2);
    return 1;
}

static int lotobuf_map(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, meta_table_name);
    ProtoCodec *codec = *((ProtoCodec **) ud);

    const char *vpath = luaL_checkstring(L, 2);
    const char *dpath = luaL_checkstring(L, 3);

    codec->map(vpath, dpath);
    lua_pushboolean(L, 1);
    return 1;
}

static int lotobuf_load(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, meta_table_name);
    ProtoCodec *codec = *((ProtoCodec **) ud);
    const char *filename = luaL_checkstring(L, 2);
    bool ret = codec->load(filename);
    int b = ret ? 1:0;
    int rcount = 1;
    lua_pushboolean(L, b);
    if(ret == false) {
        const std::string &err = codec->lastError();
        lua_pushstring(L, err.c_str());
        rcount ++;
    }
    return rcount;
}


static int lotobuf_decode(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, meta_table_name);
    ProtoCodec *codec = *((ProtoCodec **) ud);

    const char *message_name = luaL_checkstring(L, 2);
    const protobuf::Message *message_type = codec->getMessageType(message_name);
    if(message_type == NULL) {
        lua_settop(L, 0);
        lua_pushnil(L);
        lua_pushstring(L, "unknown message");
        return 2;
    }

    size_t len=0;
    const char *message_content = luaL_checklstring(L, 3, &len);
    std::string data(message_content, len);
    protobuf::Message *message = message_type->New();
    bool ret = message->ParseFromString(data);
    if(ret == false) {
        delete message;
        lua_settop(L, 0);
        lua_pushnil(L);
        lua_pushstring(L, "ParseFromString failed");
        return 2;
    }
    lua_settop(L, 0);
    lua_newtable(L);
    message_to_table(L, message);
    delete message;
    return 1;
}

static int lotobuf_encode(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, meta_table_name);
    ProtoCodec *codec = *((ProtoCodec **) ud);

    const char *message_name = luaL_checkstring(L, 2);
    const protobuf::Message *message_type = codec->getMessageType(message_name);
    if(message_type == NULL) {
        return luaL_error(L, "Unknown message: %s", message_name);
    }

    int ret = lua_istable(L, 3);
    if(ret == 0) {
        return luaL_error(L, "Unknown message: %s", message_name);
    }
    lua_settop(L, 3);
    protobuf::Message *message = message_type->New();
    bool result = table_to_message(L, message);

    result = message->IsInitialized();
    if(result == false) {
        std::string err = message->InitializationErrorString();
        return luaL_error(L, "Lost necessary field: ", err.c_str());
    }

    std::string output;
    result = message->SerializeToString(&output);
    lua_settop(L, 0);
    if(result == false) {
        lua_pushnil(L);
    } else {
        delete message;
        lua_pushlstring(L, output.c_str(), output.length());
    }
    return 1;
}


static luaL_Reg lotobuf [] =
{
    {"new",     lotobuf_new},
    {NULL, NULL}
};

static luaL_Reg lotobuf_meta [] =
{
    {"map",      lotobuf_map},
    {"load",     lotobuf_load},
    {"decode",   lotobuf_decode},
    {"encode",   lotobuf_encode},
    {NULL, NULL}
};


extern "C" {
LUALIB_API int luaopen_lotobuf(lua_State *L) {
    luaL_newmetatable(L, meta_table_name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, lotobuf_meta);

    luaL_register(L, "lotobuf", lotobuf);
    return 1;
}
}

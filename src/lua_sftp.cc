#include "lua_sftp.hpp"
#include "config_manager.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "log_mgr.hpp"
#include "sftp_session.h"
#include "ykm22lib/symbol_lua.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace lua_sftp;

// TAG: lib api
static int lua_new_sftp_session(lua_State* L)
{
    const char* target = luaL_checkstring(L, 1);

    if (!target){
        luaL_error(L,fmt::format("fatal error: can't read target_name", target).data());
        return 0;
    }

    auto session = SFTPManager::Ins().getSFTPSession(target);

    if(!session){
        luaL_error(L,fmt::format("fatal error: {} not defined in config", target).data());
        return 0;
    }
    auto userdata = lua_newuserdata(L, sizeof(SFTPSession*));

    memcpy(userdata, &session, sizeof(SFTPSession*));

    luaL_getmetatable(L, "SFTPSession");
    lua_setmetatable(L, -2);

    return 1;
}

static int lua_handle_logs(lua_State* L)
{
    auto messages = Log::Ins().get_messages();

    if (!lua_isfunction(L, 1)) { return -1; }

    for (auto& m : messages)
    {
        lua_pushvalue(L, 1);
        lua_pushinteger(L, int(m.level));
        lua_pushstring(L, m.message.c_str());
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) { 
            lua_pop(L, 1); 
        }
    }

    lua_pop(L, 1);
    return 1;
}

static int test_1(lua_State* L)
{
    usleep(50 * 1000);
    // std::cout << "50ms:" << lua_gettop(L) << std::endl;
    return 1;
}

static int test_2(lua_State* L)
{
    std::cout << "test_2:" << lua_gettop(L) << std::endl;
    return 1;
}

// TAG: sftp_session api

static int lua_sftp_test(lua_State *L){
    auto session = *(SFTPSession**)luaL_checkudata(L, 1, "SFTPSession");
    session->test();
    return 1;
}

static int sftp_gc(lua_State* L)
{
    auto session = *(SFTPSession**)luaL_checkudata(L, 1, "SFTPSession");
    SFTPManager::Ins().releaseSFTPSession(session->get_target_name());
    return 1;
}

// session:download(localPath,remotePath)
static int lua_sftp_upload(lua_State* L)
{
    auto session = *(SFTPSession**)luaL_checkudata(L, 1, "SFTPSession");
    const char* path = luaL_checkstring(L, 2);

    session->send_cmd(SFTPSession::upload_file, {path});
    return 1;
}

// session:download(remotePath,localPath) -> void
static int lua_sftp_download(lua_State* L)
{
    auto session = *(SFTPSession**)luaL_checkudata(L, 1, "SFTPSession");
    const char* remotePath = luaL_checkstring(L, 2);
    const char* localPath = luaL_checkstring(L, 3);

    session->send_cmd(SFTPSession::download_file, {remotePath, localPath});
    return 1;
}

static int lua_check_load_config(lua_State* L)
{
    return ConfigManager::Ins().check_load_config(L);
}

extern "C"
{
    int luaopen_lua_sftp(lua_State* L)
    {
        Log::Buf log;
        log.info("load lua_sftp").apply();
        LuaBindings luaBind = LuaBindings();

        int r = luaL_newmetatable(L, "SFTPSession");
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        const luaL_Reg session_f[]={
            {"upload", lua_sftp_upload},
            {"download", lua_sftp_download},
            {"__gc", sftp_gc},
            {NULL, NULL},
        };
        luaL_setfuncs(L, session_f, 0);
        lua_pop(L, 1);

        luaBind.add_regs({//
                          {"handle_logs", lua_handle_logs},
                          {"test_1", test_1},
                          {"test_2", test_2},
                          {"new_sftp_session", lua_new_sftp_session},
                          {"check_load_config", lua_check_load_config}});
        luaBind.bind(L);
        return 1;
    }
}

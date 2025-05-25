package.cpath = package.cpath 
    -- .. ";./build/macosx/x86_64/release/liblua_sftp.dylib"
    .. ";./build/windows/x64/release/lua_sftp.dll"

local ok, mylib = pcall(require, "lua_sftp")
if not ok then print(mylib) end

local sftp = require("lua_sftp")

local success
local err
local cfg={}
success,cfg,err = sftp.check_load_config("example/config.lua", cfg)

if not success then
    print(err)
end

if cfg then
    for k,v in pairs(cfg) do
        print(k,v)
    end
end

local session = sftp.new_sftp_session("test")
session:upload("src/ykm22lib/symbol_loader.hpp")

while true do
    sftp.handle_logs(function (_,msg)
        print(msg);
    end)
end

print("end");

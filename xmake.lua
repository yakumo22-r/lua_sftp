add_requires("libssh")
add_requires("fmt")
add_requires("openssl")

target("test_ssh")
    set_kind("binary")
    add_files("example/test_ssh.cc")
    add_includedirs("include")
    add_packages("libssh")

target("lua_sftp")
    set_kind("shared")
    set_languages("cxx17")
    add_files(
        -- "src/config_manager.cc", 
        -- "src/log_mgr.cc",
        -- "src/ssh_session.cc",
        -- "src/sftp_session.cc",
        -- "src/sftp_manager.cc",
        -- "src/lua_sftp.cc"
        "src/*.cc"
    )
    if is_plat("macosx") then
        set_arch("x86_64")
    end

    add_includedirs("include")
    add_packages("libssh")
    add_packages("openssl")
    add_packages("fmt")

-- target("mylib")
--     set_kind("shared")
--     set_languages("cxx17")
--     add_files("src/mylib.cc")
--     add_includedirs("include")
    -- add_packages("libssh")

-- target("sftp")
--     set_kind("shared")
--     add_files("src/*.cc")
--     -- add_packages("luajit")
--     add_includedirs("/usr/local/opt/luajit/include/luajit-2.1/")
--     add_linkdirs("/usr/local/opt/luajit/lib/")
--     add_links("luajit")

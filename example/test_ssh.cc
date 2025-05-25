#include <cstdlib>

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <iostream>

void error_exit(ssh_session session, const char* message) {
    std::cerr << message << ": " << ssh_get_error(session) << std::endl;
    ssh_free(session);
    exit(EXIT_FAILURE);
} 



// test ssh with sftp
int main(){
    // ssh_init();
    const char* remote_host = "local.yakumo22.home";

    ssh_session session = ssh_new();

    int timeout=10;
    ssh_options_set(session, SSH_OPTIONS_TIMEOUT, &timeout);

    if (session == nullptr) {
        std::cerr << "Error creating SSH session" << std::endl;
        return 1;
    }

    ssh_options_set(session, SSH_OPTIONS_HOST, remote_host);
    ssh_options_parse_config(session, nullptr);

    if(ssh_connect(session) != SSH_OK){
        error_exit(session, "Error connecting to remote host");
    }

    if (ssh_userauth_publickey_auto(session, nullptr, nullptr) != SSH_AUTH_SUCCESS) {
        error_exit(session, "Authentication failed");
    }

    ssh_disconnect(session);
    ssh_free(session);
    // ssh_finalize();
    return 0;
}



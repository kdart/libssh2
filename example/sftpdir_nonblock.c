/*
 * Sample doing an SFTP directory listing.
 *
 * The sample code has default values for host name, user name, password and
 * path, but you can specify them on the command line like:
 *
 * "sftpdir 192.168.0.1 user password /tmp/secretdir"
 */

#ifdef WIN32
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#endif

#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
# ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#if defined(_MSC_VER) && _MSC_VER < 1900
#pragma warning(disable:4127)
#endif

#ifdef WIN32
#define __FILESIZE "I64u"
#else
#define __FILESIZE "llu"
#endif

int main(int argc, char *argv[])
{
    unsigned long hostaddr;
    libssh2_socket_t sock;
    int i, auth_pw = 1;
    struct sockaddr_in sin;
    const char *fingerprint;
    LIBSSH2_SESSION *session;
    const char *username = "username";
    const char *password = "password";
    const char *sftppath = "/tmp/secretdir";
    const char *pubkey = "/home/username/.ssh/id_rsa.pub";
    const char *privkey = "/home/username/.ssh/id_rsa";
    int rc;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;

#ifdef WIN32
    WSADATA wsadata;
    int err;

    err = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if(err != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
        return 1;
    }
#endif

    if(argc > 1) {
        hostaddr = inet_addr(argv[1]);
    }
    else {
        hostaddr = htonl(0x7F000001);
    }

    if(argc > 2) {
        username = argv[2];
    }
    if(argc > 3) {
        password = argv[3];
    }
    if(argc > 4) {
        sftppath = argv[4];
    }

    rc = libssh2_init(0);
    if(rc != 0) {
        fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /*
     * The application code is responsible for creating the socket
     * and establishing the connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if(connect(sock, (struct sockaddr*)(&sin),
               sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance
     */
    session = libssh2_session_init();
    if(!session)
        return -1;

    /* Since we have set non-blocking, tell libssh2 we are non-blocking */
    libssh2_session_set_blocking(session, 0);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while((rc = libssh2_session_handshake(session, sock)) ==
          LIBSSH2_ERROR_EAGAIN);
    if(rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return -1;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Fingerprint: ");
    for(i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stderr, "\n");

    if(auth_pw) {
        /* We could authenticate via password */
        while((rc = libssh2_userauth_password(session, username, password)) ==
              LIBSSH2_ERROR_EAGAIN);
        if(rc) {
            fprintf(stderr, "Authentication by password failed.\n");
            goto shutdown;
        }
    }
    else {
        /* Or by public key */
        while((rc = libssh2_userauth_publickey_fromfile(session, username,
                                                        pubkey, privkey,
                                                        password)) ==
              LIBSSH2_ERROR_EAGAIN);
        if(rc) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
            goto shutdown;
        }
    }

    fprintf(stderr, "libssh2_sftp_init()!\n");
    do {
        sftp_session = libssh2_sftp_init(session);

        if((!sftp_session) && (libssh2_session_last_errno(session) !=
                               LIBSSH2_ERROR_EAGAIN)) {
            fprintf(stderr, "Unable to init SFTP session\n");
            goto shutdown;
        }
    } while(!sftp_session);

    fprintf(stderr, "libssh2_sftp_opendir()!\n");
    /* Request a dir listing via SFTP */
    do {
        sftp_handle = libssh2_sftp_opendir(sftp_session, sftppath);

        if((!sftp_handle) && (libssh2_session_last_errno(session) !=
                              LIBSSH2_ERROR_EAGAIN)) {
            fprintf(stderr, "Unable to open dir with SFTP\n");
            goto shutdown;
        }
    } while(!sftp_handle);

    fprintf(stderr, "libssh2_sftp_opendir() is done, now receive listing!\n");
    do {
        char mem[512];
        LIBSSH2_SFTP_ATTRIBUTES attrs;

        /* loop until we fail */
        while((rc = libssh2_sftp_readdir(sftp_handle, mem, sizeof(mem),
                                         &attrs)) == LIBSSH2_ERROR_EAGAIN) {
            ;
        }
        if(rc > 0) {
            /* rc is the length of the file name in the mem
               buffer */

            if(attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) {
                /* this should check what permissions it
                   is and print the output accordingly */
                printf("--fix----- ");
            }
            else {
                printf("---------- ");
            }

            if(attrs.flags & LIBSSH2_SFTP_ATTR_UIDGID) {
                printf("%4d %4d ", (int) attrs.uid, (int) attrs.gid);
            }
            else {
                printf("   -    - ");
            }

            if(attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) {
                printf("%8" __FILESIZE " ", attrs.filesize);
            }

            printf("%s\n", mem);
        }
        else if(rc == LIBSSH2_ERROR_EAGAIN) {
            /* blocking */
            fprintf(stderr, "Blocking\n");
        }
        else {
            break;
        }

    } while(1);

    libssh2_sftp_closedir(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);

  shutdown:

    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    fprintf(stderr, "all done\n");

    libssh2_exit();

    return 0;
}

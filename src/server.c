#include "server.h"

int main(int argc, char **argv) {
    auxts_run_tests(argc, argv);

    auxts_conn conn;
    auxts_conn_init(&conn, 3000);

    int i, j;
    int rc, len;
    char buffer[1024];
    int current_size = conn.nfds;
    int desc_ready, end_server = false, compress_array = false;

    do
    {
        printf("Waiting on poll()...\n");
        rc = poll(conn.fds, conn.nfds, conn.timeout);

        if (rc < 0) {
            perror("  poll() failed");
            break;
        }

        if (rc == 0) {
            printf("  poll() timed out.  End program.\n");
            break;
        }


        for (i = 0; i < current_size; i++) {
        if(conn.fds[i].revents == 0) continue;

        if(conn.fds[i].revents != POLLIN) {
            printf("  Error! revents = %d\n", conn.fds[i].revents);
            end_server = true;
            break;
        }

        if (conn.fds[i].fd == conn.listen_sd) {
            printf("  Listening socket is readable\n");
            do
            {
                conn.new_sd = accept(conn.listen_sd, NULL, NULL);
                if (conn.new_sd < 0) {
                    if (errno != EWOULDBLOCK) {
                        perror("  accept() failed");
                        end_server = true;
                    }
                    break;
                }

                printf("  New incoming connection - %d\n", conn.new_sd);
                conn.fds[conn.nfds].fd = conn.new_sd;
                conn.fds[conn.nfds].events = POLLIN;
                conn.nfds++;
            } while (conn.new_sd != -1);
        } else {
            printf("  Descriptor %d is readable\n", conn.fds[i].fd);
            conn.close = false;

            do
            {
                rc = recv(conn.fds[i].fd, buffer, sizeof(buffer), 0);
                if (rc < 0) {
                    if (errno != EWOULDBLOCK) {
                        perror("  recv() failed");
                        conn.close = true;
                    }
                    break;
                }

                if (rc == 0) {
                    printf("  Connection closed\n");
                    conn.close = true;
                    break;
                }

                len = rc;
                printf("  %d bytes received\n", len);
                rc = send(conn.fds[i].fd, buffer, len, 0);
                if (rc < 0)
                {
                    perror("  send() failed");
                    conn.close = true;
                    break;
                }

            } while(true);

            if (conn.close) {
                close(conn.fds[i].fd);
                conn.fds[i].fd = -1;
                compress_array = true;
            }
        }
    }

    if (compress_array) {
        compress_array = false;
        for (i = 0; i < conn.nfds; i++) {
            if (conn.fds[i].fd == -1) {
                for(j = i; j < conn.nfds-1; j++)
                {
                    conn.fds[j].fd = conn.fds[j+1].fd;
                }
                i--;
                conn.nfds--;
            }
        }
    }

    } while (end_server == false);

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    for (i = 0; i < conn.nfds; i++)
    {
        if(conn.fds[i].fd >= 0)
            close(conn.fds[i].fd);
    }
}


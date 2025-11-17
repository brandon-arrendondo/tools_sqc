/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

typedef struct {
    int user_id;
    char username[64];
    char session_token[128];
    time_t login_time;
    time_t last_activity;
    int privilege_level;
    int failed_attempts;
    int is_authenticated;
} user_session_t;

typedef struct {
    user_session_t sessions[10];
    int active_sessions;
    int total_logins;
    int failed_logins;
    char auth_server[64];
    int session_timeout;
    char encryption_key[256];
} auth_state_t;

auth_state_t global_auth_state = {0};

void init_session(user_session_t *session, int id) {
    session->user_id = id;
    sprintf(session->username, "user_%d", id);
    strcpy(session->session_token, "");
    session->login_time = 0;
    session->last_activity = 0;
    session->privilege_level = 1;
    session->failed_attempts = 0;
    session->is_authenticated = 0;
}

void unsafe_handler(int sig) {
    /* Violation: Accessing shared authentication and session data in signal handler */

    if (sig == SIGUSR1) {
        /* Emergency logout all users */
        for (int i = 0; i < 10; i++) {
            user_session_t *session = &global_auth_state.sessions[i];
            if (session->is_authenticated) {
                session->is_authenticated = 0;
                strcpy(session->session_token, "EMERGENCY_LOGOUT");
                session->privilege_level = 0;
                global_auth_state.active_sessions--;
            }
        }
        strcpy(global_auth_state.auth_server, "emergency_auth");
        global_auth_state.session_timeout = 0;
    } else if (sig == SIGUSR2) {
        /* Privilege escalation due to signal */
        for (int i = 0; i < 10; i++) {
            user_session_t *session = &global_auth_state.sessions[i];
            if (session->is_authenticated) {
                session->privilege_level = 99;  /* Admin privileges */
                session->last_activity = time(NULL);
                sprintf(session->session_token, "ESCALATED_BY_SIG_%d", sig);
            }
        }
        global_auth_state.failed_logins = 0;  /* Reset failed attempts */
    }

    /* Update global authentication state */
    strcpy(global_auth_state.encryption_key, "SIGNAL_COMPROMISED_KEY");

    printf("Handler: active_sessions=%d, total_logins=%d, failed=%d, server=%s\n",
           global_auth_state.active_sessions, global_auth_state.total_logins,
           global_auth_state.failed_logins, global_auth_state.auth_server);
}

int main() {
    printf("Demonstrating unsafe authentication/session data access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize authentication state */
    strcpy(global_auth_state.auth_server, "primary_auth_server");
    global_auth_state.session_timeout = 3600;  /* 1 hour */
    strcpy(global_auth_state.encryption_key, "secure_encryption_key_123");
    global_auth_state.active_sessions = 0;
    global_auth_state.total_logins = 0;
    global_auth_state.failed_logins = 0;

    for (int i = 0; i < 10; i++) {
        init_session(&global_auth_state.sessions[i], i);
    }

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 30; i++) {
        int session_index = i % 10;
        user_session_t *session = &global_auth_state.sessions[session_index];

        /* Simulate authentication events */
        if (i % 5 == 0) {  /* Login attempt */
            if (!session->is_authenticated) {
                session->is_authenticated = 1;
                session->login_time = time(NULL);
                session->last_activity = session->login_time;
                sprintf(session->session_token, "TOKEN_%d_%ld", session->user_id, session->login_time);
                session->privilege_level = (i % 3) + 1;
                session->failed_attempts = 0;
                global_auth_state.active_sessions++;
                global_auth_state.total_logins++;
            }
        } else if (i % 7 == 6) {  /* Failed login */
            session->failed_attempts++;
            global_auth_state.failed_logins++;
        } else if (i % 8 == 7) {  /* Logout */
            if (session->is_authenticated) {
                session->is_authenticated = 0;
                strcpy(session->session_token, "");
                session->privilege_level = 0;
                global_auth_state.active_sessions--;
            }
        } else {  /* Activity update */
            if (session->is_authenticated) {
                session->last_activity = time(NULL);
            }
        }

        printf("Main: active_sessions=%d, total_logins=%d, failed=%d, user%d_auth=%d, priv=%d\n",
               global_auth_state.active_sessions, global_auth_state.total_logins,
               global_auth_state.failed_logins, session_index,
               session->is_authenticated, session->privilege_level);

        usleep(100000);
    }

    return 0;
}
#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}:$(pwd)/libs/myteams" && echo $LD_LIBRARY_PATH
CLIENT_BIN="$ROOT_DIR/myteams_cli"
HOST="127.0.0.1"
PORT="${1:-2555}"
RUN_ID="$(date +%s)-$$"
WORK_DIR="$(mktemp -d)"

PASS_COUNT=0
FAIL_COUNT=0
ALICE_IN_FD=""
ALICE_OUT_FD=""
ALICE_PID=""
ALICE_LAST_OUTPUT=""
BOB_IN_FD=""
BOB_OUT_FD=""
BOB_PID=""
BOB_LAST_OUTPUT=""

cleanup() {
    local exit_code=$?

    if [[ -n "${ALICE_PID:-}" ]] && kill -0 "$ALICE_PID" 2>/dev/null; then
        kill "$ALICE_PID" 2>/dev/null || true
        wait "$ALICE_PID" 2>/dev/null || true
    fi

    if [[ -n "${BOB_PID:-}" ]] && kill -0 "$BOB_PID" 2>/dev/null; then
        kill "$BOB_PID" 2>/dev/null || true
        wait "$BOB_PID" 2>/dev/null || true
    fi

    rm -rf "$WORK_DIR"

    exit "$exit_code"
}
trap cleanup EXIT INT TERM

wait_for_server() {
    local tries=0
    while [[ $tries -lt 100 ]]; do
        if (exec 3<>/dev/tcp/$HOST/$PORT) 2>/dev/null; then
            exec 3<&- 3>&-
            return 0
        fi
        sleep 0.1
        tries=$((tries + 1))
    done
    return 1
}

collect_output() {
    local fd="$1"
    local output=""
    local line=""
    local idle_rounds=0

    while true; do
        if IFS= read -r -t 0.3 -u "$fd" line; then
            output+="$line"$'\n'
            idle_rounds=0
        else
            idle_rounds=$((idle_rounds + 1))
            if [[ $idle_rounds -ge 5 ]]; then
                break
            fi
        fi
    done

    printf '%s' "$output"
}

extract_first_uuid() {
    printf '%s' "$1" | grep -oE '[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}' | head -n1
}

client_alive() {
    local client_name="$1"
    local pid_var="${client_name}_PID"
    local -n client_pid="$pid_var"

    [[ -n "${client_pid:-}" ]] && kill -0 "$client_pid" 2>/dev/null
}

start_client() {
    local client_name="$1"
    local in_fifo="$2"
    local out_fifo="$3"
    local in_var="${client_name}_IN_FD"
    local out_var="${client_name}_OUT_FD"
    local pid_var="${client_name}_PID"
    local client_in_fd=""
    local client_out_fd=""

    exec {client_in_fd}<>"$in_fifo"
    exec {client_out_fd}<>"$out_fifo"

    printf -v "$in_var" '%s' "$client_in_fd"
    printf -v "$out_var" '%s' "$client_out_fd"

    stdbuf -oL -eL "$CLIENT_BIN" "$HOST" "$PORT" <"$in_fifo" >"$out_fifo" 2>&1 &
    printf -v "$pid_var" '%s' "$!"
}

stop_client() {
    local client_name="$1"
    local pid_var="${client_name}_PID"
    local -n client_pid="$pid_var"

    if [[ -n "${client_pid:-}" ]] && kill -0 "$client_pid" 2>/dev/null; then
        kill "$client_pid" 2>/dev/null || true
        wait "$client_pid" 2>/dev/null || true
    fi
}

run_step() {
    local client_name="$1"
    local test_name="$2"
    local command_to_send="$3"
    local expected_regex="${4:-}"
    local in_var="${client_name}_IN_FD"
    local out_var="${client_name}_OUT_FD"
    local last_var="${client_name}_LAST_OUTPUT"
    local -n client_in_fd="$in_var"
    local -n client_out_fd="$out_var"
    local -n client_last_output="$last_var"
    local output=""

    if ! client_alive "$client_name"; then
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo "[FAIL] $test_name"
        echo "       client: $client_name is not running"
        return 1
    fi

    printf '%s\n' "$command_to_send" >&"$client_in_fd"
    sleep 0.4
    output="$(collect_output "$client_out_fd")"
    client_last_output="$output"

    if [[ -z "$expected_regex" ]]; then
        PASS_COUNT=$((PASS_COUNT + 1))
        echo "[PASS] $test_name"
        return 0
    fi

    if printf '%s' "$output" | grep -Eq "$expected_regex"; then
        PASS_COUNT=$((PASS_COUNT + 1))
        echo "[PASS] $test_name"
    else
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo "[FAIL] $test_name"
        echo "       client: $client_name"
        echo "       command: $command_to_send"
        if [[ -n "$output" ]]; then
            echo "       output:"
            printf '%s\n' "$output" | sed 's/^/       | /'
        else
            echo "       output: <empty>"
        fi
    fi

    return 0
}

expect_output() {
    local client_name="$1"
    local test_name="$2"
    local expected_regex="${3:-}"
    local out_var="${client_name}_LAST_OUTPUT"
    local out_fd_var="${client_name}_OUT_FD"
    local -n client_last_output="$out_var"
    local -n client_out_fd="$out_fd_var"
    local output=""

    if ! client_alive "$client_name"; then
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo "[FAIL] $test_name"
        echo "       client: $client_name is not running"
        return 1
    fi

    sleep 0.4
    output="$(collect_output "$client_out_fd")"
    client_last_output="$output"

    if [[ -z "$expected_regex" ]]; then
        PASS_COUNT=$((PASS_COUNT + 1))
        echo "[PASS] $test_name"
        return 0
    fi

    if printf '%s' "$output" | grep -Eq "$expected_regex"; then
        PASS_COUNT=$((PASS_COUNT + 1))
        echo "[PASS] $test_name"
    else
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo "[FAIL] $test_name"
        if [[ -n "$output" ]]; then
            echo "       output:"
            printf '%s\n' "$output" | sed 's/^/       | /'
        else
            echo "       output: <empty>"
        fi
    fi

    return 0
}

if [[ ! -x "$CLIENT_BIN" ]]; then
    (cd "$ROOT_DIR" && make -j >/dev/null)
fi

if ! wait_for_server; then
    echo "Server not reachable on ${HOST}:${PORT}. Lance myteams_server de ton côté puis relance tester.sh."
    exit 1
fi

ALICE_USERNAME="alice_${RUN_ID}"
BOB_USERNAME="bob_${RUN_ID}"
TEAM_NAME="team_${RUN_ID}"
TEAM_DESC="description_${RUN_ID}"
CHANNEL_NAME="channel_${RUN_ID}"
CHANNEL_DESC="channel_description_${RUN_ID}"
THREAD_TITLE="thread_${RUN_ID}"
THREAD_BODY="thread_body_${RUN_ID}"
REPLY_BODY="reply_body_${RUN_ID}"
MESSAGE_BODY="message_${RUN_ID}"

ALICE_IN_FIFO_P1="$WORK_DIR/alice_phase1.in"
ALICE_OUT_FIFO_P1="$WORK_DIR/alice_phase1.out"
mkfifo "$ALICE_IN_FIFO_P1" "$ALICE_OUT_FIFO_P1"
start_client ALICE "$ALICE_IN_FIFO_P1" "$ALICE_OUT_FIFO_P1"

# Drop the startup banner before starting the actual tests.
sleep 0.5
collect_output "$ALICE_OUT_FD" >/dev/null

run_step ALICE "help" "/help" 'Available commands:.*login.*logout.*users.*user.*send.*messages.*subscribe.*subscribed.*unsubscribe.*help'
run_step ALICE "login alice" "/login \"${ALICE_USERNAME}\"" 'Login success|logged in'

ALICE_UUID="$(extract_first_uuid "$ALICE_LAST_OUTPUT")"

run_step ALICE "users" "/users"

run_step ALICE "logout alice phase 1" "/logout" 'logged out|logout'
stop_client ALICE

rm -f "$ALICE_IN_FIFO_P1" "$ALICE_OUT_FIFO_P1"

ALICE_IN_FIFO_P1B="$WORK_DIR/alice_phase1b.in"
ALICE_OUT_FIFO_P1B="$WORK_DIR/alice_phase1b.out"
mkfifo "$ALICE_IN_FIFO_P1B" "$ALICE_OUT_FIFO_P1B"
start_client ALICE "$ALICE_IN_FIFO_P1B" "$ALICE_OUT_FIFO_P1B"

sleep 0.5
collect_output "$ALICE_OUT_FD" >/dev/null

run_step ALICE "login alice for user" "/login \"${ALICE_USERNAME}\"" 'Login success|logged in'
run_step ALICE "user info" "/user ${ALICE_UUID}"

stop_client ALICE
rm -f "$ALICE_IN_FIFO_P1B" "$ALICE_OUT_FIFO_P1B"

ALICE_IN_FIFO_P2="$WORK_DIR/alice_phase2.in"
ALICE_OUT_FIFO_P2="$WORK_DIR/alice_phase2.out"
BOB_IN_FIFO_P2="$WORK_DIR/bob_phase2.in"
BOB_OUT_FIFO_P2="$WORK_DIR/bob_phase2.out"
mkfifo "$ALICE_IN_FIFO_P2" "$ALICE_OUT_FIFO_P2" "$BOB_IN_FIFO_P2" "$BOB_OUT_FIFO_P2"
start_client ALICE "$ALICE_IN_FIFO_P2" "$ALICE_OUT_FIFO_P2"
start_client BOB "$BOB_IN_FIFO_P2" "$BOB_OUT_FIFO_P2"

sleep 0.5
collect_output "$ALICE_OUT_FD" >/dev/null
collect_output "$BOB_OUT_FD" >/dev/null

run_step ALICE "login alice phase 2" "/login \"${ALICE_USERNAME}\"" 'Login success|logged in'
run_step BOB "login bob" "/login \"${BOB_USERNAME}\"" 'Login success|logged in'

ALICE_UUID="$(extract_first_uuid "$ALICE_LAST_OUTPUT")"
BOB_UUID="$(extract_first_uuid "$BOB_LAST_OUTPUT")"

run_step ALICE "messages before send" "/messages ${BOB_UUID}"

run_step ALICE "create team" "/create \"${TEAM_NAME}\" \"${TEAM_DESC}\"" "${TEAM_NAME}"
TEAM_UUID="$(extract_first_uuid "$ALICE_LAST_OUTPUT")"
if [[ -z "$TEAM_UUID" ]]; then
    TEAM_UUID="$(extract_first_uuid "$BOB_LAST_OUTPUT")"
fi

expect_output BOB "team created broadcast" "${TEAM_NAME}"

run_step ALICE "list teams" "/list" "${TEAM_NAME}"
run_step ALICE "info user context" "/info" "${ALICE_USERNAME}"

run_step ALICE "use team" "/use ${TEAM_UUID}" 'Team context set'
run_step ALICE "info team context" "/info" "${TEAM_NAME}"
run_step ALICE "list channels empty" "/list" "No items available"

run_step ALICE "create channel" "/create \"${CHANNEL_NAME}\" \"${CHANNEL_DESC}\"" "${CHANNEL_NAME}"
CHANNEL_UUID="$(extract_first_uuid "$ALICE_LAST_OUTPUT")"

run_step ALICE "list channels" "/list" "${CHANNEL_NAME}"
run_step ALICE "info channel context" "/info" "${CHANNEL_NAME}"

run_step ALICE "use channel" "/use ${TEAM_UUID} ${CHANNEL_UUID}" 'Channel context set'
run_step ALICE "create thread" "/create \"${THREAD_TITLE}\" \"${THREAD_BODY}\"" "${THREAD_TITLE}"
THREAD_UUID="$(extract_first_uuid "$ALICE_LAST_OUTPUT")"

run_step ALICE "list threads" "/list" "${THREAD_TITLE}"
run_step ALICE "info thread context" "/info" "${THREAD_TITLE}"

run_step ALICE "use thread" "/use ${TEAM_UUID} ${CHANNEL_UUID} ${THREAD_UUID}" 'Thread context set'
run_step ALICE "create reply" "/create \"${REPLY_BODY}\"" "${REPLY_BODY}"

run_step ALICE "list replies" "/list" "${REPLY_BODY}"

run_step ALICE "clear use context" "/use" 'Use context cleared'
run_step ALICE "info after clear" "/info" "${ALICE_USERNAME}"
run_step ALICE "list teams after clear" "/list" "${TEAM_NAME}"

run_step BOB "subscribe team" "/subscribe ${TEAM_UUID}" "${TEAM_UUID}"
run_step BOB "subscribed teams" "/subscribed" "${TEAM_NAME}"
run_step BOB "subscribed team users" "/subscribed ${TEAM_UUID}" "${BOB_USERNAME}"
run_step BOB "unsubscribe team" "/unsubscribe ${TEAM_UUID}" "${TEAM_UUID}"
run_step BOB "subscribed after unsubscribe" "/subscribed"

run_step ALICE "send private message" "/send ${BOB_UUID} ${MESSAGE_BODY}" "${MESSAGE_BODY}"
expect_output BOB "private message received" "${MESSAGE_BODY}"

run_step BOB "logout bob" "/logout" 'logged out|logout'
run_step ALICE "logout alice phase 2" "/logout" 'logged out|logout'

echo ""
echo "Tests passed: $PASS_COUNT"
echo "Tests failed: $FAIL_COUNT"

if [[ $FAIL_COUNT -gt 0 ]]; then
    exit 1
fi
exit 0

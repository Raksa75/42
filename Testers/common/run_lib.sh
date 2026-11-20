#!/bin/bash

source ../common/common.sh
ID="$(basename "$PWD")"
load_conf "$ID"
SRC="${PROJECT_DIR:-$TESTERS_DIR/../$REPO_DIR}"
TMP="$(mktemp -d)"
parse_args "$@"

HAD_LIB=0
[ -f "$SRC/$LIB" ] && HAD_LIB=1
cleanup() {
	[ $HAD_LIB -eq 0 ] && make -C "$SRC" fclean >/dev/null 2>&1
	rm -rf "$TMP"
}
trap cleanup EXIT

log_init "$ID" "$SRC" "$OPTS"
intro

[ -z "$FILTER" ] && check_delivery "$SRC"
check_makefile "$SRC" "$LIB" || { log_end "$TITLE"; exit 1; }

if [ -z "$FILTER" ]; then
	check_norm $(src_files "$SRC")
	title "Fonctions"
	check_symbols "$SRC/$LIB"
	check_externals "$ALLOWED" "$SRC/$LIB"
fi

if ! cc -Wall -Wextra -g $TEST_FLAGS -I "$SRC" "$TEST_SRC" "$SRC/$LIB" -o "$TMP/test" \
	2>"$TMP/cc.log"; then
	title "Compilation du testeur"
	ko "le testeur ne compile pas avec ton projet (prototype different ou fonction manquante ?) :"
	show_file "$TMP/cc.log" 30
	log_end "$TITLE"; exit 1
fi

run_tests "$TMP/test" "Tests"

title "Fuites et erreurs memoire"
run_valgrind "$TMP/test" ""

log_end "$TITLE"
exit $STATUS

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

MISSING_SYMBOLS="$(missing_symbols "$SRC/$LIB")"
BONUS_DONE=0
if [ -n "${MISSING_SYMBOLS// /}" ] && make -C "$SRC" -n bonus >/dev/null 2>&1; then
	make -C "$SRC" bonus >"$TMP/bonus.log" 2>&1 && BONUS_DONE=1
	MISSING_SYMBOLS="$(missing_symbols "$SRC/$LIB")"
fi

if [ -z "$FILTER" ]; then
	check_norm $(src_files "$SRC")
	title "Fonctions"
	[ $BONUS_DONE -eq 1 ] && skip "des fonctions manquaient apres make : make bonus a ete lance (ancien sujet ?)"
	check_symbols "$SRC/$LIB"
	check_externals "$ALLOWED" "$SRC/$LIB"
fi

include_flags "$SRC"
EXTRA_OBJ=()
make_stubs "$TMP" && EXTRA_OBJ=("$TMP/stubs.o")
for h in $DETECT; do
	hp="$(find_in "$SRC" "$h")"
	[ -n "$hp" ] && [ -n "$NEEDS_TYPE" ] && ! grep -q "$NEEDS_TYPE" "$hp" && TEST_FLAGS="$TEST_FLAGS $NEEDS_FLAG"
done

if ! cc -Wall -Wextra -g -Wno-implicit-function-declaration -Wno-int-conversion $TEST_FLAGS \
	"${INC[@]}" "$TEST_SRC" "$SRC/$LIB" "${EXTRA_OBJ[@]}" -o "$TMP/test" 2>"$TMP/cc.log"; then
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

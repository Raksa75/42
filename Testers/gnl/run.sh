#!/bin/bash

cd "$(dirname "$0")" || exit 1
source ../common/common.sh
SRC="${PROJECT_DIR:-../../GetNextLine}"
TMP="$(mktemp -d)"
[ "$1" = "--no-valgrind" ] && NO_VALGRIND=1
SIZES="0 1 2 3 5 10 42 1000"
trap 'rm -rf "$TMP"' EXIT
USE_VG=1
{ [ "$NO_VALGRIND" = 1 ] || ! command -v valgrind >/dev/null 2>&1; } && USE_VG=0

log_init "gnl" "$SRC" "$1"
show_project "GET_NEXT_LINE" "$(cd "$SRC" && pwd)"

title "Fichiers"
for f in get_next_line.c get_next_line_utils.c get_next_line.h; do
	if [ -f "$SRC/$f" ]; then ok "$f present"
	else ko "$f manquant"; log_end "GET_NEXT_LINE"; exit 1; fi
done
FILES="$SRC/get_next_line.c $SRC/get_next_line_utils.c"

check_norm $FILES "$SRC/get_next_line.h"

title "Compilation"
if cc -Wall -Wextra -Werror -c "$SRC/get_next_line.c" -o "$TMP/a.o" -I "$SRC" 2>"$TMP/cc.log" \
	&& cc -Wall -Wextra -Werror -c "$SRC/get_next_line_utils.c" -o "$TMP/b.o" -I "$SRC" 2>>"$TMP/cc.log"; then
	ok "compile avec -Wall -Wextra -Werror sans -D BUFFER_SIZE"
	check_externals "read malloc free" "$TMP/a.o" "$TMP/b.o"
else
	ko "ne compile pas sans -D BUFFER_SIZE :"
	show_file "$TMP/cc.log" 30
fi

title "Tests pour chaque BUFFER_SIZE"
[ $USE_VG -eq 0 ] && skip "valgrind non utilise (absent ou --no-valgrind)"
for size in $SIZES; do
	label="$(printf 'BUFFER_SIZE = %-5s' "$size")"
	if ! cc -Wall -Wextra -Werror -g -D BUFFER_SIZE=$size -I "$SRC" -c \
		"$SRC/get_next_line.c" -o "$TMP/gnl.o" 2>"$TMP/cc.log" \
		|| ! cc -Wall -Wextra -Werror -g -D BUFFER_SIZE=$size -I "$SRC" -c \
		"$SRC/get_next_line_utils.c" -o "$TMP/gnl_utils.o" 2>>"$TMP/cc.log" \
		|| ! cc -Wall -Wextra -g -D BUFFER_SIZE=$size -I "$SRC" test_gnl.c \
		"$TMP/gnl.o" "$TMP/gnl_utils.o" -o "$TMP/test_$size" 2>>"$TMP/cc.log"; then
		ko "$label ne compile pas :"
		show_file "$TMP/cc.log" 30
		continue
	fi
	[ -t 1 ] && printf "   ${DIM}…  %s  en cours...${RESET}\r" "$label"
	export TESTER_COMPACT=1
	run_tests "$TMP/test_$size" "BUFFER_SIZE=$size" >"$TMP/out" 2>&1
	rc=$?
	unset TESTER_COMPACT
	vg="" vg_ok=1
	if [ $rc -eq 0 ] && [ $USE_VG -eq 1 ]; then
		if valgrind_check "$TMP/test_$size"; then vg="   valgrind ${GREEN}✔${RESET}"
		else vg="   valgrind ${RED}✘ fuite / erreur${RESET}"; vg_ok=0; fi
	fi
	line="$label $(bar "$RES_OK" "$RES_TOTAL" 20)  ${DIM}$(printf '%2s/%-2s tests  %4s/%-4s verifs' "$RES_OK" "$RES_TOTAL" "$CHK_OK" "$CHK_TOTAL")${RESET}$vg"
	[ -t 1 ] && printf "\033[K"
	if [ $rc -eq 0 ] && [ $vg_ok -eq 1 ]; then
		ok "$line"
	else
		say "   ${RED}✘${RESET} $line"
		STATUS=1
		log "$(cat "$TMP/out")"
		if [ -z "$SHOWN" ]; then
			cat "$TMP/out"
			[ $vg_ok -eq 0 ] && show_valgrind
			SHOWN=1
		else
			grep -v "↳" "$TMP/out"
			[ $vg_ok -eq 0 ] && LOG_FILE="" show_valgrind | grep -E "tests concernes" \
				&& log "$(sed 's/^/       │ /' "$VG_ERR")"
			echo -e "            ${DIM}↳ detail dans le log${RESET}"
		fi
		[ $vg_ok -eq 0 ] && FAILED_SECTIONS+=("valgrind (BUFFER_SIZE=$size) : fuite ou erreur memoire")
	fi
	[ -n "$VG_ERR" ] && rm -f "$VG_ERR"
	VG_ERR=""
done

log_end "GET_NEXT_LINE"
exit $STATUS

#!/bin/bash
# Testeur get_next_line
# Usage : ./run.sh [--no-valgrind]
# Le dossier teste est ../../GetNextLine, ou celui donne par PROJECT_DIR.

cd "$(dirname "$0")" || exit 1
source ../common/common.sh
SRC="${PROJECT_DIR:-../../GetNextLine}"
TMP="$(mktemp -d)"
[ "$1" = "--no-valgrind" ] && NO_VALGRIND=1
SIZES="0 1 2 3 5 10 42 1000"
trap 'rm -rf "$TMP"' EXIT

log_init "gnl" "$SRC" "$1"
say "${BOLD}${CYAN}get_next_line${RESET} ${DIM}($(cd "$SRC" && pwd))${RESET}"

title "Fichiers"
for f in get_next_line.c get_next_line_utils.c get_next_line.h; do
	if [ -f "$SRC/$f" ]; then ok "$f present"
	else ko "$f manquant"; log_end; exit 1; fi
done
FILES="$SRC/get_next_line.c $SRC/get_next_line_utils.c"

check_norm $FILES "$SRC/get_next_line.h"

title "Compilation"
if cc -Wall -Wextra -Werror -c "$SRC/get_next_line.c" -o "$TMP/a.o" -I "$SRC" 2>"$TMP/cc.log" \
	&& cc -Wall -Wextra -Werror -c "$SRC/get_next_line_utils.c" -o "$TMP/b.o" -I "$SRC" 2>>"$TMP/cc.log"; then
	ok "compile avec -Wall -Wextra -Werror sans -D BUFFER_SIZE (valeur par defaut)"
	check_externals "read malloc free" "$TMP/a.o" "$TMP/b.o"
else
	ko "ne compile pas sans -D BUFFER_SIZE :"
	show_file "$TMP/cc.log" 30
fi

for size in $SIZES; do
	# ton code est compile avec -Werror, le testeur sans
	if ! cc -Wall -Wextra -Werror -g -D BUFFER_SIZE=$size -I "$SRC" -c \
		"$SRC/get_next_line.c" -o "$TMP/gnl.o" 2>"$TMP/cc.log" \
		|| ! cc -Wall -Wextra -Werror -g -D BUFFER_SIZE=$size -I "$SRC" -c \
		"$SRC/get_next_line_utils.c" -o "$TMP/gnl_utils.o" 2>>"$TMP/cc.log" \
		|| ! cc -Wall -Wextra -g -D BUFFER_SIZE=$size -I "$SRC" test_gnl.c \
		"$TMP/gnl.o" "$TMP/gnl_utils.o" -o "$TMP/test_$size" 2>>"$TMP/cc.log"; then
		title "BUFFER_SIZE = $size"
		ko "ne compile pas avec BUFFER_SIZE=$size :"
		show_file "$TMP/cc.log" 30
		continue
	fi
	if "$TMP/test_$size"; then
		run_valgrind "$TMP/test_$size" " (BUFFER_SIZE=$size)"
	else
		STATUS=1
	fi
done

echo
if [ $STATUS -eq 0 ]; then
	say "${GREEN}${BOLD}get_next_line : tout est OK pour tous les BUFFER_SIZE${RESET}"
else
	say "${RED}${BOLD}get_next_line : des tests ont echoue (voir ci-dessus et le log)${RESET}"
fi
log_end
exit $STATUS

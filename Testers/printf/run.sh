#!/bin/bash

cd "$(dirname "$0")" || exit 1
source ../common/common.sh
SRC="${PROJECT_DIR:-../../Printf}"
TMP="$(mktemp -d)"
[ "$1" = "--no-valgrind" ] && NO_VALGRIND=1

HAD_LIB=0
[ -f "$SRC/libftprintf.a" ] && HAD_LIB=1
cleanup() {
	[ $HAD_LIB -eq 0 ] && make -C "$SRC" fclean >/dev/null 2>&1
	rm -rf "$TMP"
}
trap cleanup EXIT

log_init "printf" "$SRC" "$1"
show_project "FT_PRINTF" "$(cd "$SRC" && pwd)"

check_makefile "$SRC" "libftprintf.a" || { log_end "FT_PRINTF"; exit 1; }
NORM_FILES=$(find "$SRC" \( -name '*.c' -o -name '*.h' \) -not -path '*/.git/*')
check_norm $NORM_FILES

title "Fonctions externes"
check_externals "malloc free write" "$SRC/libftprintf.a"

if ! cc -Wall -Wextra -Wno-format -Wno-format-security -g -I "$SRC" test_printf.c \
	"$SRC/libftprintf.a" -o "$TMP/test" 2>"$TMP/cc.log"; then
	title "Compilation du testeur"
	ko "le testeur ne compile pas avec ton ft_printf :"
	show_file "$TMP/cc.log" 30
	log_end "FT_PRINTF"; exit 1
fi

run_tests "$TMP/test" "Tests"

title "Fuites et erreurs memoire"
run_valgrind "$TMP/test" ""

log_end "FT_PRINTF"
exit $STATUS

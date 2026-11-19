#!/bin/bash

cd "$(dirname "$0")" || exit 1
source ../common/common.sh
SRC="${PROJECT_DIR:-../../Libft}"
TMP="$(mktemp -d)"
[ "$1" = "--no-valgrind" ] && NO_VALGRIND=1

HAD_LIB=0
[ -f "$SRC/libft.a" ] && HAD_LIB=1
cleanup() {
	[ $HAD_LIB -eq 0 ] && make -C "$SRC" fclean >/dev/null 2>&1
	rm -rf "$TMP"
}
trap cleanup EXIT

log_init "libft" "$SRC" "$1"
show_project "LIBFT" "$(cd "$SRC" && pwd)"

check_makefile "$SRC" "libft.a" || { log_end "LIBFT"; exit 1; }
check_norm "$SRC"/*.c "$SRC"/*.h

title "Fonctions externes"
check_externals "malloc free write" "$SRC/libft.a"

if ! cc -Wall -Wextra -g -I "$SRC" test_libft.c "$SRC/libft.a" -o "$TMP/test" \
	2>"$TMP/cc.log"; then
	title "Compilation du testeur"
	ko "le testeur ne compile pas avec ta libft (prototype different ou fonction manquante ?) :"
	show_file "$TMP/cc.log" 30
	log_end "LIBFT"; exit 1
fi

run_tests "$TMP/test" "Tests"

title "Fuites et erreurs memoire"
run_valgrind "$TMP/test" ""

log_end "LIBFT"
exit $STATUS

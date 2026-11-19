#!/bin/bash
# Testeur Libft
# Usage : ./run.sh [--no-valgrind]
# Le dossier teste est ../../Libft, ou celui donne par PROJECT_DIR.

cd "$(dirname "$0")" || exit 1
source ../common/common.sh
SRC="${PROJECT_DIR:-../../Libft}"
TMP="$(mktemp -d)"
[ "$1" = "--no-valgrind" ] && NO_VALGRIND=1

# si libft.a existait deja avant le test, on le laisse ; sinon on nettoie
HAD_LIB=0
[ -f "$SRC/libft.a" ] && HAD_LIB=1
cleanup() {
	[ $HAD_LIB -eq 0 ] && make -C "$SRC" fclean >/dev/null 2>&1
	rm -rf "$TMP"
}
trap cleanup EXIT

log_init "libft" "$SRC" "$1"
say "${BOLD}${CYAN}Libft${RESET} ${DIM}($(cd "$SRC" && pwd))${RESET}"

check_makefile "$SRC" "libft.a" || { log_end; exit 1; }
check_norm "$SRC"/*.c "$SRC"/*.h

title "Fonctions externes"
check_externals "malloc free write" "$SRC/libft.a"

if ! cc -Wall -Wextra -g -I "$SRC" test_libft.c "$SRC/libft.a" -o "$TMP/test" \
	2>"$TMP/cc.log"; then
	title "Compilation du testeur"
	ko "le testeur ne compile pas avec ta libft (prototype different ou fonction manquante ?) :"
	show_file "$TMP/cc.log" 30
	log_end; exit 1
fi

"$TMP/test" || STATUS=1

title "Fuites et erreurs memoire"
run_valgrind "$TMP/test" ""

log_end
exit $STATUS

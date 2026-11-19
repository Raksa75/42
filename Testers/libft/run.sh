#!/bin/bash
# Testeur Libft
# Usage : ./run.sh              (tests + valgrind si disponible)
#         ./run.sh --no-valgrind

cd "$(dirname "$0")" || exit 1
SRC="../../Libft"
TMP="$(mktemp -d)"
STATUS=0
GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; BOLD="\033[1m"; RESET="\033[0m"

cleanup() { make -C "$SRC" fclean >/dev/null 2>&1; rm -rf "$TMP"; }
trap cleanup EXIT

echo -e "${BOLD}=== Libft - Makefile ===${RESET}"
if ! make -C "$SRC" >"$TMP/make.log" 2>&1; then
	echo -e "  ${RED}[KO]${RESET}    make a echoue :"
	cat "$TMP/make.log"
	exit 1
fi
if [ -f "$SRC/libft.a" ]; then
	echo -e "  ${GREEN}[OK]${RESET}    make cree libft.a"
else
	echo -e "  ${RED}[KO]${RESET}    libft.a n'a pas ete cree a la racine"; exit 1
fi
if make -C "$SRC" 2>&1 | grep -qE "Nothing to be done|is up to date"; then
	echo -e "  ${GREEN}[OK]${RESET}    pas de relink (un 2e make ne refait rien)"
else
	echo -e "  ${RED}[KO]${RESET}    relink : un 2e make recompile quelque chose"; STATUS=1
fi
for rule in clean fclean re all; do
	if make -C "$SRC" -n "$rule" >/dev/null 2>&1; then
		echo -e "  ${GREEN}[OK]${RESET}    regle '$rule' presente"
	else
		echo -e "  ${RED}[KO]${RESET}    regle '$rule' absente"; STATUS=1
	fi
done

echo -e "\n${BOLD}=== Libft - Norminette ===${RESET}"
if command -v norminette >/dev/null 2>&1; then
	if norminette "$SRC"/*.c "$SRC"/*.h >"$TMP/norm.log" 2>&1; then
		echo -e "  ${GREEN}[OK]${RESET}    norminette"
	else
		echo -e "  ${RED}[KO]${RESET}    norminette :"
		grep -v "OK!" "$TMP/norm.log" | sed 's/^/      /'
		STATUS=1
	fi
else
	echo -e "  ${YELLOW}[--]${RESET}    norminette non installee, verification ignoree"
fi

cc -Wall -Wextra -g -I "$SRC" test_libft.c "$SRC/libft.a" -o "$TMP/test" || exit 1

"$TMP/test" || STATUS=1

if [ "$1" != "--no-valgrind" ] && command -v valgrind >/dev/null 2>&1; then
	echo -e "\n${BOLD}=== Fuites et erreurs memoire (valgrind) ===${RESET}"
	if valgrind -q --leak-check=full --errors-for-leak-kinds=definite \
		--error-exitcode=42 "$TMP/test" >"$TMP/vg.log" 2>&1; then
		echo -e "  ${GREEN}[OK]${RESET}    aucune fuite ni erreur memoire"
	else
		echo -e "  ${RED}[KO]${RESET}    valgrind a detecte des problemes :"
		sed 's/\x1b\[[0-9;]*m//g' "$TMP/vg.log" \
			| grep -E "\[(LEAK|CRASH|KO)\]|definitely lost|Invalid|uninitialised|at 0x|by 0x" \
			| head -40 | sed 's/^/      /'
		STATUS=1
	fi
else
	echo -e "\n  ${YELLOW}[--]${RESET}    valgrind non utilise (absent ou --no-valgrind)"
fi
exit $STATUS

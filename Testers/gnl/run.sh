#!/bin/bash
# Testeur get_next_line
# Usage : ./run.sh              (tous les BUFFER_SIZE + valgrind si disponible)
#         ./run.sh --no-valgrind

cd "$(dirname "$0")" || exit 1
SRC="../../GetNextLine"
TMP="$(mktemp -d)"
STATUS=0
GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; BOLD="\033[1m"; RESET="\033[0m"
FILES="$SRC/get_next_line.c $SRC/get_next_line_utils.c"
SIZES="0 1 2 3 5 10 42 1000"

trap 'rm -rf "$TMP"' EXIT

echo -e "${BOLD}=== get_next_line - Fichiers ===${RESET}"
for f in get_next_line.c get_next_line_utils.c get_next_line.h; do
	if [ -f "$SRC/$f" ]; then
		echo -e "  ${GREEN}[OK]${RESET}    $f present"
	else
		echo -e "  ${RED}[KO]${RESET}    $f manquant"; exit 1
	fi
done

echo -e "\n${BOLD}=== get_next_line - Norminette ===${RESET}"
if command -v norminette >/dev/null 2>&1; then
	if norminette $FILES "$SRC/get_next_line.h" >"$TMP/norm.log" 2>&1; then
		echo -e "  ${GREEN}[OK]${RESET}    norminette"
	else
		echo -e "  ${RED}[KO]${RESET}    norminette :"
		grep -v "OK!" "$TMP/norm.log" | sed 's/^/      /'
		STATUS=1
	fi
else
	echo -e "  ${YELLOW}[--]${RESET}    norminette non installee, verification ignoree"
fi

echo -e "\n${BOLD}=== get_next_line - Compilation ===${RESET}"
if cc -Wall -Wextra -Werror -c $FILES -I "$SRC" 2>"$TMP/cc.log"; then
	echo -e "  ${GREEN}[OK]${RESET}    compile sans -D BUFFER_SIZE (valeur par defaut)"
else
	echo -e "  ${RED}[KO]${RESET}    ne compile pas sans -D BUFFER_SIZE :"
	sed 's/^/      /' "$TMP/cc.log"; STATUS=1
fi
FORBIDDEN=$(nm -u get_next_line.o get_next_line_utils.o 2>/dev/null \
	| awk '{print $2}' | grep -vE '^(read|malloc|free|__stack_chk_fail)$' \
	| grep -vE '^(ft_gnl_|get_next_line)' | sort -u)
rm -f get_next_line.o get_next_line_utils.o
if [ -z "$FORBIDDEN" ]; then
	echo -e "  ${GREEN}[OK]${RESET}    seules read, malloc et free sont utilisees"
else
	echo -e "  ${RED}[KO]${RESET}    fonctions externes non autorisees : $(echo $FORBIDDEN)"
	STATUS=1
fi

USE_VG=0
if [ "$1" != "--no-valgrind" ] && command -v valgrind >/dev/null 2>&1; then
	USE_VG=1
fi

for size in $SIZES; do
	# ton code est compile avec -Werror, le testeur sans
	if ! cc -Wall -Wextra -Werror -g -D BUFFER_SIZE=$size -I "$SRC" -c \
		"$SRC/get_next_line.c" -o "$TMP/gnl.o" 2>"$TMP/cc.log" \
		|| ! cc -Wall -Wextra -Werror -g -D BUFFER_SIZE=$size -I "$SRC" -c \
		"$SRC/get_next_line_utils.c" -o "$TMP/gnl_utils.o" 2>>"$TMP/cc.log" \
		|| ! cc -Wall -Wextra -g -D BUFFER_SIZE=$size -I "$SRC" test_gnl.c \
		"$TMP/gnl.o" "$TMP/gnl_utils.o" -o "$TMP/test_$size" 2>>"$TMP/cc.log"; then
		echo -e "\n  ${RED}[KO]${RESET}    ne compile pas avec BUFFER_SIZE=$size :"
		sed 's/^/      /' "$TMP/cc.log"; STATUS=1; continue
	fi
	if ! "$TMP/test_$size"; then
		STATUS=1
	elif [ $USE_VG -eq 1 ]; then
		if valgrind -q --leak-check=full --errors-for-leak-kinds=definite \
			--error-exitcode=42 "$TMP/test_$size" >"$TMP/vg.log" 2>&1; then
			echo -e "  ${GREEN}[OK]${RESET}    valgrind : aucune fuite ni erreur memoire"
		else
			echo -e "  ${RED}[KO]${RESET}    valgrind a detecte des problemes :"
			sed 's/\x1b\[[0-9;]*m//g' "$TMP/vg.log" \
				| grep -E "\\[(LEAK|CRASH|KO)\\]|definitely lost|Invalid|uninitialised|at 0x|by 0x" \
				| head -40 | sed 's/^/      /'
			STATUS=1
		fi
	fi
done

if [ $USE_VG -eq 0 ]; then
	echo -e "\n  ${YELLOW}[--]${RESET}    valgrind non utilise (absent ou --no-valgrind)"
fi
echo
if [ $STATUS -eq 0 ]; then
	echo -e "${GREEN}${BOLD}get_next_line : tout est OK pour tous les BUFFER_SIZE${RESET}"
else
	echo -e "${RED}${BOLD}get_next_line : des tests ont echoue (voir ci-dessus)${RESET}"
fi
exit $STATUS

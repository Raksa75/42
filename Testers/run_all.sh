#!/bin/bash
# Lance les testeurs de tous les projets.
# Usage : ./run_all.sh              (avec valgrind si disponible)
#         ./run_all.sh --no-valgrind

cd "$(dirname "$0")" || exit 1
GREEN="\033[32m"; RED="\033[31m"; BOLD="\033[1m"; RESET="\033[0m"
RESULTS=""
STATUS=0

for t in libft printf gnl; do
	echo -e "\n${BOLD}################ $t ################${RESET}"
	if ./$t/run.sh "$@"; then
		RESULTS="$RESULTS\n  ${GREEN}[OK]${RESET}  $t"
	else
		RESULTS="$RESULTS\n  ${RED}[KO]${RESET}  $t"
		STATUS=1
	fi
done

echo -e "\n${BOLD}================ Recapitulatif ================${RESET}$RESULTS"
exit $STATUS

#!/bin/bash

cd "$(dirname "$0")" || exit 1
source common/common.sh
LINES_RECAP=()
ALL=0

<<<<<<< HEAD
for t in libft printf gnl; do
=======
for t in $(for c in */project.conf; do
	d="${c%/project.conf}"
	[[ "$d" == _* ]] && continue
	echo "$(source "$c"; echo "$ORDER") $d"
done | sort -n | cut -d' ' -f2); do
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
	if ./$t/run.sh "$@"; then
		LINES_RECAP+=("${GREEN}✔${RESET}  $t")
	else
		LINES_RECAP+=("${RED}✘  $t${RESET}")
		ALL=1
	fi
done

echo
if [ $ALL -eq 0 ]; then box "$GREEN" "${BOLD}Recapitulatif${RESET}" "" "${LINES_RECAP[@]}"
else box "$RED" "${BOLD}Recapitulatif${RESET}" "" "${LINES_RECAP[@]}"; fi
echo -e "  ${DIM}Les logs sont dans $LOG_DIR/${RESET}"
exit $ALL

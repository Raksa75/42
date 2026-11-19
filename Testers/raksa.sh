#!/bin/bash
# raksa - menu des testeurs 42 (Libft, ft_printf, get_next_line)
#
#   raksa                     ouvre le menu
#   raksa libft [dossier]     teste directement une Libft (printf, gnl : pareil)
#   raksa all                 teste les trois projets du depot
#   raksa ici                 teste le projet du dossier courant
#   --no-valgrind             plus rapide, sans la recherche de fuites
#   -h                        aide

TESTERS="$(dirname "$(readlink -f "$0")")"
REPO="$(dirname "$TESTERS")"
ART="$TESTERS/assets/raksa.ans"
GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; CYAN="\033[36m"
MAGENTA="\033[35m"; BOLD="\033[1m"; DIM="\033[2m"; RESET="\033[0m"
VG_OPT=""
LAST_PROJECT=""
LAST_DIR=""

# ============================ utilitaires ============================

project_name() {
	case "$1" in
		libft) echo "Libft" ;;
		printf) echo "ft_printf" ;;
		gnl) echo "get_next_line" ;;
	esac
}

project_header() {
	case "$1" in
		libft) echo "libft.h" ;;
		printf) echo "ft_printf.h" ;;
		gnl) echo "get_next_line.h" ;;
	esac
}

default_dir() {
	case "$1" in
		libft) echo "$REPO/Libft" ;;
		printf) echo "$REPO/Printf" ;;
		gnl) echo "$REPO/GetNextLine" ;;
	esac
}

# reconnait le projet d'un dossier grace a son header
detect() {
	if [ -f "$1/libft.h" ]; then echo "libft"
	elif [ -f "$1/ft_printf.h" ]; then echo "printf"
	elif [ -f "$1/get_next_line.h" ]; then echo "gnl"
	fi
}

# remet le curseur visible en quittant (meme avec ctrl+c)
trap 'tput cnorm 2>/dev/null; echo; exit 130' INT
trap 'tput cnorm 2>/dev/null' EXIT

# Menu a fleches.
#   select_menu <choix par defaut> "Question" "option 1" "option 2" ...
# Haut/Bas (ou k/j) pour bouger, Entree pour valider, 1-9 pour aller
# directement a une option. Le numero choisi (a partir de 0) est mis
# dans MENU_RESULT.
select_menu() {
	local selected="$1" title="$2"
	shift 2
	local options=("$@") n=$# key rest i
	echo -e "\n  ${BOLD}$title${RESET}  ${DIM}(fleches + Entree)${RESET}\n"
	tput civis 2>/dev/null
	while true; do
		for i in "${!options[@]}"; do
			if [ "$i" -eq "$selected" ]; then
				echo -e "\r  ${CYAN}${BOLD}❯ ${options[$i]}${RESET}\033[K"
			else
				echo -e "\r    ${options[$i]}${RESET}\033[K"
			fi
		done
		IFS= read -rsn1 key || exit 0
		case "$key" in
			$'\x1b')
				read -rsn2 -t 0.05 rest
				case "$rest" in
					'[A'|'OA') selected=$(( (selected - 1 + n) % n )) ;;
					'[B'|'OB') selected=$(( (selected + 1) % n )) ;;
				esac ;;
			k) selected=$(( (selected - 1 + n) % n )) ;;
			j) selected=$(( (selected + 1) % n )) ;;
			[1-9]) [ "$key" -le "$n" ] && selected=$((key - 1)) ;;
			"") break ;;
		esac
		printf "\033[%dA" "$n"
	done
	tput cnorm 2>/dev/null
	MENU_RESULT=$selected
}

# question oui/non ; $2 = reponse par defaut (o ou n)
ask_yes_no() {
	local def=0
	[ "$2" = "n" ] && def=1
	select_menu "$def" "$1" "Oui" "Non"
	[ "$MENU_RESULT" -eq 0 ]
}

pause() {
	read -r -p "$(echo -e "\n  ${DIM}Appuie sur Entree pour continuer...${RESET}")" _ || exit 0
}

# ============================== affichage ==============================

banner() {
	local title=(
		""
		""
		""
		""
		"${BOLD}${RED} ____      _    _  __ ____    _    ${RESET}"
		"${BOLD}${RED}|  _ \\    / \\  | |/ // ___|  / \\   ${RESET}"
		"${BOLD}${RED}| |_) |  / _ \\ | ' / \\___ \\ / _ \\  ${RESET}"
		"${BOLD}${RED}|  _ <  / ___ \\| . \\  ___) / ___ \\ ${RESET}"
		"${BOLD}${RED}|_| \\_\\/_/   \\_\\_|\\_\\|____/_/   \\_\\ ${RESET}"
		""
		"${DIM}  Testeurs 42${RESET}"
		"${DIM}  Libft - ft_printf - get_next_line${RESET}"
	)
	local i=0 line cols
	cols="$(tput cols 2>/dev/null || echo 80)"
	clear 2>/dev/null
	echo
	if [ -f "$ART" ] && [ "$cols" -ge 84 ]; then
		while IFS= read -r line; do
			echo -e "$line    ${title[$i]}"
			i=$((i + 1))
		done < "$ART"
	else
		[ -f "$ART" ] && [ "$cols" -ge 44 ] && cat "$ART"
		for line in "${title[@]}"; do
			[ -n "$line" ] && echo -e "  $line"
		done
	fi
	echo
}

usage() {
	echo -e "${BOLD}raksa${RESET} - testeurs pour Libft, ft_printf et get_next_line\n"
	echo -e "  ${CYAN}raksa${RESET}                   ouvre le menu"
	echo -e "  ${CYAN}raksa ici${RESET}               teste le projet du dossier courant"
	echo -e "  ${CYAN}raksa libft${RESET} [dossier]   teste une Libft"
	echo -e "  ${CYAN}raksa printf${RESET} [dossier]  teste un ft_printf"
	echo -e "  ${CYAN}raksa gnl${RESET} [dossier]     teste un get_next_line"
	echo -e "  ${CYAN}raksa all${RESET}               teste les trois projets du depot"
	echo -e "  ${CYAN}raksa log${RESET}               affiche le log du dernier test"
	echo -e "\n  Options : ${CYAN}--no-valgrind${RESET} (plus rapide, sans les fuites), ${CYAN}-h${RESET} (aide)"
}

# ============================== lancement ==============================

run_one() {
	local project="$1"
	local dir
	dir="$(readlink -f "$2")"
	if [ ! -d "$dir" ]; then
		echo -e "  ${RED}Le dossier $2 n'existe pas.${RESET}"
		return 1
	fi
	LAST_PROJECT="$project"
	LAST_DIR="$dir"
	echo -e "\n${BOLD}${MAGENTA}>>> $(project_name "$project")${RESET} ${DIM}($dir)${RESET}"
	PROJECT_DIR="$dir" "$TESTERS/$project/run.sh" $VG_OPT
}

run_all() {
	local results="" status=0 p
	for p in libft printf gnl; do
		if run_one "$p" "$(default_dir "$p")"; then
			results="$results\n  ${GREEN}[OK]${RESET}  $(project_name "$p")"
		else
			results="$results\n  ${RED}[KO]${RESET}  $(project_name "$p")"
			status=1
		fi
	done
	echo -e "\n${BOLD}=========== Recapitulatif ===========${RESET}$results"
	return $status
}

# ============================ les questions ============================

ask_valgrind() {
	if ! command -v valgrind >/dev/null 2>&1; then
		VG_OPT="--no-valgrind"
		echo -e "  ${DIM}(valgrind n'est pas installe : pas de recherche de fuites)${RESET}"
	elif ask_yes_no "Chercher les fuites memoire avec valgrind ? (plus lent)" "o"; then
		VG_OPT=""
	else
		VG_OPT="--no-valgrind"
	fi
}

# demande quel projet tester ; ecrit le resultat dans CHOSEN_PROJECT
ask_project() {
	CHOSEN_PROJECT=""
	select_menu 0 "Quel projet veux-tu tester ?" \
		"Libft" "ft_printf" "get_next_line" "${DIM}Retour${RESET}"
	case "$MENU_RESULT" in
		0) CHOSEN_PROJECT="libft" ;;
		1) CHOSEN_PROJECT="printf" ;;
		2) CHOSEN_PROJECT="gnl" ;;
		*) return 1 ;;
	esac
}

# demande quel dossier tester ; ecrit le resultat dans CHOSEN_DIR
ask_dir() {
	local project="$1" path repo_dir
	repo_dir="$(default_dir "$project")"
	CHOSEN_DIR=""
	while true; do
		select_menu 0 "Quel dossier ?" \
			"Le dossier actuel     ${DIM}$PWD${RESET}" \
			"Celui de ton depot    ${DIM}$repo_dir${RESET}" \
			"Un autre dossier      ${DIM}(tu tapes le chemin, Tab pour completer)${RESET}" \
			"${DIM}Retour${RESET}"
		case "$MENU_RESULT" in
			0) path="$PWD" ;;
			1) path="$repo_dir" ;;
			2) echo
			   read -r -e -p "$(echo -e "  ${BOLD}Chemin :${RESET} ")" path || exit 0
			   path="${path/#\~/$HOME}" ;;
			*) return 1 ;;
		esac
		if [ ! -d "$path" ]; then
			echo -e "\n  ${RED}Ce dossier n'existe pas : $path${RESET}"
			continue
		fi
		if [ ! -f "$path/$(project_header "$project")" ]; then
			echo -e "\n  ${YELLOW}Attention : pas de $(project_header "$project") dans ce dossier.${RESET}"
			ask_yes_no "Le tester quand meme ?" "n" || continue
		fi
		CHOSEN_DIR="$(readlink -f "$path")"
		return 0
	done
}

test_wizard() {
	local detected
	CHOSEN_PROJECT=""
	CHOSEN_DIR=""
	detected="$(detect "$PWD")"
	if [ -n "$detected" ]; then
		echo -e "\n  ${GREEN}J'ai trouve un $(project_name "$detected") dans le dossier actuel :${RESET}"
		echo -e "  ${DIM}$PWD${RESET}"
		if ask_yes_no "Tu veux tester celui-la ?" "o"; then
			CHOSEN_PROJECT="$detected"
			CHOSEN_DIR="$PWD"
		fi
	fi
	if [ -z "$CHOSEN_PROJECT" ]; then
		ask_project || return
		ask_dir "$CHOSEN_PROJECT" || return
	fi
	ask_valgrind
	echo -e "\n  ${BOLD}C'est parti :${RESET} $(project_name "$CHOSEN_PROJECT") ${DIM}($CHOSEN_DIR)${RESET}"
	run_one "$CHOSEN_PROJECT" "$CHOSEN_DIR"
	after_test
}

after_test() {
	while true; do
		select_menu 0 "Et maintenant ?" \
			"Voir le log (detail de chaque test)" "Relancer le meme test" \
			"Revenir au menu" "Quitter"
		case "$MENU_RESULT" in
			0) show_log ;;
			1) run_one "$LAST_PROJECT" "$LAST_DIR" ;;
			2) return ;;
			*) goodbye ;;
		esac
	done
}

# affiche le dernier log (less pour defiler, q pour quitter)
show_log() {
	local f="$TESTERS/logs/dernier.log"
	if [ ! -e "$f" ]; then
		echo -e "\n  ${YELLOW}Aucun log pour l'instant : lance d'abord un test.${RESET}"
		return 1
	fi
	echo -e "\n  ${DIM}Log : $(readlink -f "$f")${RESET}"
	if command -v less >/dev/null 2>&1 && [ -t 1 ]; then
		less -R "$f"
	else
		cat "$f"
	fi
}

goodbye() {
	echo -e "\n  ${MAGENTA}A plus !${RESET}\n"
	exit 0
}

main_menu() {
	while true; do
		banner
		select_menu 0 "Que veux-tu faire ?" \
			"Tester un projet" "Tester les trois projets de mon depot" \
			"Voir le dernier log" "Aide" "Quitter"
		case "$MENU_RESULT" in
			0) test_wizard ;;
			1) echo; ask_valgrind; run_all
			   echo -e "\n  ${DIM}Les logs des trois projets sont dans $TESTERS/logs/${RESET}"
			   pause ;;
			2) show_log || pause ;;
			3) echo; usage; pause ;;
			*) goodbye ;;
		esac
	done
}

# ======================= lecture des arguments =======================

ARGS=()
for a in "$@"; do
	case "$a" in
		--no-valgrind) VG_OPT="--no-valgrind" ;;
		-h|--help) usage; exit 0 ;;
		*) ARGS+=("$a") ;;
	esac
done

case "${ARGS[0]}" in
	"") main_menu ;;
	ici)
		detected="$(detect "$PWD")"
		if [ -z "$detected" ]; then
			echo -e "${RED}Aucun projet reconnu dans ce dossier.${RESET}"; exit 1
		fi
		run_one "$detected" "$PWD"
		;;
	libft|printf|gnl)
		run_one "${ARGS[0]}" "${ARGS[1]:-$(default_dir "${ARGS[0]}")}"
		;;
	all) run_all ;;
	log) show_log ;;
	*) echo -e "${RED}Argument inconnu : ${ARGS[0]}${RESET}\n"; usage; exit 1 ;;
esac

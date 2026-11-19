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

# pose une question oui/non ; $2 = reponse par defaut (o ou n)
ask_yes_no() {
	local answer hint
	if [ "$2" = "o" ]; then hint="O/n"; else hint="o/N"; fi
	read -r -p "$(echo -e "  ${BOLD}?${RESET} $1 ($hint) ")" answer || exit 0
	[ -z "$answer" ] && answer="$2"
	[[ "$answer" =~ ^[oOyY] ]]
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
	local choice
	CHOSEN_PROJECT=""
	echo -e "\n  ${BOLD}Quel projet veux-tu tester ?${RESET}\n"
	echo -e "    ${BOLD}1${RESET}) Libft"
	echo -e "    ${BOLD}2${RESET}) ft_printf"
	echo -e "    ${BOLD}3${RESET}) get_next_line"
	echo -e "    ${BOLD}r${RESET}) Retour\n"
	while true; do
		read -r -p "$(echo -e "  ${BOLD}>${RESET} ")" choice || exit 0
		case "$choice" in
			1) CHOSEN_PROJECT="libft"; return 0 ;;
			2) CHOSEN_PROJECT="printf"; return 0 ;;
			3) CHOSEN_PROJECT="gnl"; return 0 ;;
			r|R) return 1 ;;
			*) echo -e "  ${RED}Tape 1, 2, 3 ou r.${RESET}" ;;
		esac
	done
}

# demande quel dossier tester ; ecrit le resultat dans CHOSEN_DIR
ask_dir() {
	local project="$1" choice path repo_dir
	repo_dir="$(default_dir "$project")"
	CHOSEN_DIR=""
	echo -e "\n  ${BOLD}Quel dossier ?${RESET}\n"
	echo -e "    ${BOLD}1${RESET}) Le dossier actuel     ${DIM}$PWD${RESET}"
	echo -e "    ${BOLD}2${RESET}) Celui de ton depot    ${DIM}$repo_dir${RESET}"
	echo -e "    ${BOLD}3${RESET}) Un autre dossier      ${DIM}(tu tapes le chemin, Tab pour completer)${RESET}"
	echo -e "    ${BOLD}r${RESET}) Retour\n"
	while true; do
		read -r -p "$(echo -e "  ${BOLD}>${RESET} ")" choice || exit 0
		case "$choice" in
			1) path="$PWD" ;;
			2) path="$repo_dir" ;;
			3) read -r -e -p "$(echo -e "  ${BOLD}Chemin :${RESET} ")" path || exit 0
			   path="${path/#\~/$HOME}" ;;
			r|R) return 1 ;;
			*) echo -e "  ${RED}Tape 1, 2, 3 ou r.${RESET}"; continue ;;
		esac
		if [ ! -d "$path" ]; then
			echo -e "  ${RED}Ce dossier n'existe pas : $path${RESET}"
			continue
		fi
		if [ ! -f "$path/$(project_header "$project")" ]; then
			echo -e "  ${YELLOW}Attention : pas de $(project_header "$project") dans ce dossier.${RESET}"
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
		echo -e "  ${DIM}$PWD${RESET}\n"
		if ask_yes_no "Tu veux tester celui-la ?" "o"; then
			CHOSEN_PROJECT="$detected"
			CHOSEN_DIR="$PWD"
		fi
	fi
	if [ -z "$CHOSEN_PROJECT" ]; then
		ask_project || return
		ask_dir "$CHOSEN_PROJECT" || return
	fi
	echo
	ask_valgrind
	echo -e "\n  ${BOLD}C'est parti :${RESET} $(project_name "$CHOSEN_PROJECT") ${DIM}($CHOSEN_DIR)${RESET}"
	run_one "$CHOSEN_PROJECT" "$CHOSEN_DIR"
	after_test
}

after_test() {
	local choice
	while true; do
		echo -e "\n  ${BOLD}Et maintenant ?${RESET}\n"
		echo -e "    ${BOLD}1${RESET}) Relancer le meme test"
		echo -e "    ${BOLD}2${RESET}) Revenir au menu"
		echo -e "    ${BOLD}q${RESET}) Quitter\n"
		read -r -p "$(echo -e "  ${BOLD}>${RESET} ")" choice || exit 0
		case "$choice" in
			1) run_one "$LAST_PROJECT" "$LAST_DIR" ;;
			2) return ;;
			q|Q) goodbye ;;
			*) echo -e "  ${RED}Tape 1, 2 ou q.${RESET}" ;;
		esac
	done
}

goodbye() {
	echo -e "\n  ${MAGENTA}A plus !${RESET}\n"
	exit 0
}

main_menu() {
	local choice
	while true; do
		banner
		echo -e "  ${BOLD}Que veux-tu faire ?${RESET}\n"
		echo -e "    ${BOLD}1${RESET}) Tester un projet"
		echo -e "    ${BOLD}2${RESET}) Tester les trois projets de mon depot"
		echo -e "    ${BOLD}3${RESET}) Aide"
		echo -e "    ${BOLD}q${RESET}) Quitter\n"
		read -r -p "$(echo -e "  ${BOLD}>${RESET} ")" choice || goodbye
		case "$choice" in
			1) test_wizard ;;
			2) echo; ask_valgrind; run_all; pause ;;
			3) echo; usage; pause ;;
			q|Q) goodbye ;;
			*) ;;
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
	*) echo -e "${RED}Argument inconnu : ${ARGS[0]}${RESET}\n"; usage; exit 1 ;;
esac

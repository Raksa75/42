#!/bin/bash
# correcteur - lance les testeurs depuis n'importe quel dossier
#
#   correcteur                   teste le projet du dossier courant
#                                (ou ouvre le menu si aucun projet n'est reconnu)
#   correcteur -m                ouvre le menu
#   correcteur libft [dossier]   teste une Libft (printf, gnl : pareil)
#   correcteur all               teste les trois projets du depot
#   --no-valgrind                plus rapide, sans la recherche de fuites
#   -h                           aide

TESTERS="$(dirname "$(readlink -f "$0")")"
REPO="$(dirname "$TESTERS")"
GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; CYAN="\033[36m"
BOLD="\033[1m"; DIM="\033[2m"; RESET="\033[0m"
VG_OPT=""

usage() {
	echo -e "${BOLD}correcteur${RESET} - testeurs pour Libft, ft_printf et get_next_line\n"
	echo -e "  ${CYAN}correcteur${RESET}                   teste le projet du dossier courant"
	echo -e "  ${CYAN}correcteur -m${RESET}                ouvre le menu"
	echo -e "  ${CYAN}correcteur libft${RESET} [dossier]   teste une Libft"
	echo -e "  ${CYAN}correcteur printf${RESET} [dossier]  teste un ft_printf"
	echo -e "  ${CYAN}correcteur gnl${RESET} [dossier]     teste un get_next_line"
	echo -e "  ${CYAN}correcteur all${RESET}               teste les trois projets du depot"
	echo -e "\n  Options : ${CYAN}--no-valgrind${RESET} (plus rapide, sans les fuites), ${CYAN}-h${RESET} (aide)"
}

# reconnait le projet d'un dossier grace a son header
detect() {
	if [ -f "$1/libft.h" ]; then echo "libft"
	elif [ -f "$1/ft_printf.h" ]; then echo "printf"
	elif [ -f "$1/get_next_line.h" ]; then echo "gnl"
	fi
}

default_dir() {
	case "$1" in
		libft) echo "$REPO/Libft" ;;
		printf) echo "$REPO/Printf" ;;
		gnl) echo "$REPO/GetNextLine" ;;
	esac
}

project_name() {
	case "$1" in
		libft) echo "Libft" ;;
		printf) echo "ft_printf" ;;
		gnl) echo "get_next_line" ;;
	esac
}

# lance le testeur d'un projet sur un dossier
run_one() {
	local project="$1"
	local dir
	dir="$(readlink -f "$2")"
	if [ ! -d "$dir" ]; then
		echo -e "${RED}Le dossier $2 n'existe pas.${RESET}"
		return 1
	fi
	echo -e "\n${BOLD}${CYAN}>>> $(project_name "$project")${RESET} ${DIM}($dir)${RESET}"
	PROJECT_DIR="$dir" "$TESTERS/$project/run.sh" $VG_OPT
}

run_all() {
	local results=""
	local status=0
	local p
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

banner() {
	echo -e "${BOLD}${CYAN}"
	echo "   ____                         _                  "
	echo "  / ___|___  _ __ _ __ ___  ___| |_ ___ _   _ _ __ "
	echo " | |   / _ \\| '__| '__/ _ \\/ __| __/ _ \\ | | | '__|"
	echo " | |__| (_) | |  | | |  __/ (__| ||  __/ |_| | |   "
	echo "  \\____\\___/|_|  |_|  \\___|\\___|\\__\\___|\\__,_|_|   "
	echo -e "${RESET}${DIM}        Libft - ft_printf - get_next_line${RESET}\n"
}

menu() {
	local choice project dir answer detected
	while true; do
		banner
		detected="$(detect "$PWD")"
		if [ -n "$detected" ]; then
			echo -e "  ${GREEN}Projet detecte ici :${RESET} $(project_name "$detected") ${DIM}($PWD)${RESET}\n"
		fi
		echo -e "  ${BOLD}1${RESET}) Libft"
		echo -e "  ${BOLD}2${RESET}) ft_printf"
		echo -e "  ${BOLD}3${RESET}) get_next_line"
		echo -e "  ${BOLD}4${RESET}) Les trois projets du depot"
		[ -n "$detected" ] && echo -e "  ${BOLD}5${RESET}) Le projet de ce dossier"
		echo -e "  ${BOLD}q${RESET}) Quitter\n"
		read -r -p "  Ton choix : " choice || exit 0
		case "$choice" in
			1) project="libft" ;;
			2) project="printf" ;;
			3) project="gnl" ;;
			4) project="all" ;;
			5) [ -n "$detected" ] && project="$detected" && dir="$PWD" ;;
			q|Q) exit 0 ;;
			*) echo -e "  ${RED}Choix invalide.${RESET}"; sleep 1; continue ;;
		esac
		[ -z "$project" ] && continue
		read -r -p "  Chercher les fuites avec valgrind ? (O/n) " answer
		if [[ "$answer" =~ ^[nN] ]]; then VG_OPT="--no-valgrind"; else VG_OPT=""; fi
		if [ "$project" = "all" ]; then
			run_all
		else
			if [ "$choice" != "5" ]; then
				dir="$(default_dir "$project")"
				read -r -e -p "  Dossier a tester [$dir] : " answer
				[ -n "$answer" ] && dir="${answer/#\~/$HOME}"
			fi
			run_one "$project" "$dir"
		fi
		echo
		read -r -p "  Revenir au menu ? (o/N) " answer || exit 0
		[[ "$answer" =~ ^[oOyY] ]] || exit 0
		project=""
		dir=""
		clear 2>/dev/null
	done
}

# ---------- lecture des arguments ----------
ARGS=()
for a in "$@"; do
	case "$a" in
		--no-valgrind) VG_OPT="--no-valgrind" ;;
		-h|--help) usage; exit 0 ;;
		*) ARGS+=("$a") ;;
	esac
done

case "${ARGS[0]}" in
	"")
		detected="$(detect "$PWD")"
		if [ -n "$detected" ]; then
			run_one "$detected" "$PWD"
		else
			menu
		fi
		;;
	-m|menu) menu ;;
	libft|printf|gnl)
		run_one "${ARGS[0]}" "${ARGS[1]:-$(default_dir "${ARGS[0]}")}"
		;;
	all) run_all ;;
	*) echo -e "${RED}Argument inconnu : ${ARGS[0]}${RESET}\n"; usage; exit 1 ;;
esac

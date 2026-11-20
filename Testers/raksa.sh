#!/bin/bash

TESTERS="$(dirname "$(readlink -f "$0")")"
REPO="$(dirname "$TESTERS")"
ART="$TESTERS/assets/raksa.ans"
source "$TESTERS/common/common.sh"
GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; CYAN="\033[36m"
MAGENTA="\033[35m"; BOLD="\033[1m"; DIM="\033[2m"; RESET="\033[0m"
VG_OPT=""
<<<<<<< HEAD
LAST_PROJECT=""
LAST_DIR=""

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

detect() {
	if [ -f "$1/libft.h" ]; then echo "libft"
	elif [ -f "$1/ft_printf.h" ]; then echo "printf"
	elif [ -f "$1/get_next_line.h" ]; then echo "gnl"
	fi
=======
FILTER=""
EVAL_MODE=0
LAST_PROJECT=""
LAST_DIR=""

projects() {
	local c d
	for c in "$TESTERS"/*/project.conf; do
		d="$(basename "$(dirname "$c")")"
		[[ "$d" == _* ]] && continue
		echo "$(conf_get "$d" ORDER) $d"
	done | sort -n | cut -d' ' -f2
}

conf_get() {
	( source "$TESTERS/$1/project.conf" 2>/dev/null; eval "echo \"\${$2}\"" )
}

is_project() { [ -f "$TESTERS/$1/project.conf" ] && [[ "$1" != _* ]]; }
project_name() { conf_get "$1" NAME; }
project_header() { conf_get "$1" DETECT; }
default_dir() { echo "$REPO/$(conf_get "$1" REPO_DIR)"; }

detect() {
	local p
	for p in $(projects); do
		[ -f "$1/$(project_header "$p")" ] && { echo "$p"; return; }
	done
}

project_list() {
	local p out=""
	for p in $(projects); do out="$out - $(project_name "$p")"; done
	echo "${out# - }"
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
}

trap 'tput cnorm 2>/dev/null; echo; exit 130' INT
trap 'tput cnorm 2>/dev/null' EXIT

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

ask_yes_no() {
	local def=0
	[ "$2" = "n" ] && def=1
	select_menu "$def" "$1" "Oui" "Non"
	[ "$MENU_RESULT" -eq 0 ]
}

pause() {
	read -r -p "$(echo -e "\n  ${DIM}Appuie sur Entree pour continuer...${RESET}")" _ || exit 0
}

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
<<<<<<< HEAD
		"${DIM}  Libft - ft_printf - get_next_line${RESET}"
=======
		"${DIM}  $(project_list)${RESET}"
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
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
<<<<<<< HEAD
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

run_one() {
	local project="$1"
	local dir
=======
	local p
	echo -e "${BOLD}raksa${RESET} - testeurs pour mes projets 42 ($(project_list))\n"
	echo -e "  ${CYAN}raksa${RESET}                          ouvre le menu"
	echo -e "  ${CYAN}raksa ici${RESET} [fonction]           teste le projet du dossier courant"
	for p in $(projects); do
		printf "  ${CYAN}raksa %-7s${RESET} [dossier] [fonction]  teste un %s\n" "$p" "$(project_name "$p")"
	done
	echo -e "  ${CYAN}raksa eval${RESET} [projet|ici] [dossier]  simule une correction (note estimee)"
	echo -e "  ${CYAN}raksa all${RESET}                      teste tous les projets du depot"
	echo -e "  ${CYAN}raksa log${RESET}                      affiche le log du dernier test"
	echo -e "\n  ${DIM}[fonction] : ne lance que les tests dont le nom la contient (ex : raksa libft ft_split)${RESET}"
	echo -e "  Options : ${CYAN}--no-valgrind${RESET} (plus rapide, sans les fuites), ${CYAN}-h${RESET} (aide)"
}

run_one() {
	local project="$1" dir
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
	dir="$(readlink -f "$2")"
	if [ ! -d "$dir" ]; then
		echo -e "  ${RED}Le dossier $2 n'existe pas.${RESET}"
		return 1
	fi
	LAST_PROJECT="$project"
	LAST_DIR="$dir"
<<<<<<< HEAD
	PROJECT_DIR="$dir" "$TESTERS/$project/run.sh" $VG_OPT
=======
	PROJECT_DIR="$dir" TEST_FILTER="$FILTER" RAKSA_EVAL="$EVAL_MODE" \
		"$TESTERS/$project/run.sh" $VG_OPT
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
}

run_all() {
	local lines=() status=0 p
<<<<<<< HEAD
	for p in libft printf gnl; do
=======
	for p in $(projects); do
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
		if run_one "$p" "$(default_dir "$p")"; then
			lines+=("${GREEN}✔${RESET}  $(project_name "$p")")
		else
			lines+=("${RED}✘  $(project_name "$p")${RESET}")
			status=1
		fi
	done
	echo
	if [ $status -eq 0 ]; then LOG_FILE="" box "$GREEN" "${BOLD}Recapitulatif${RESET}" "" "${lines[@]}"
	else LOG_FILE="" box "$RED" "${BOLD}Recapitulatif${RESET}" "" "${lines[@]}"; fi
	return $status
}

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

ask_project() {
<<<<<<< HEAD
	CHOSEN_PROJECT=""
	select_menu 0 "Quel projet veux-tu tester ?" \
		"Libft" "ft_printf" "get_next_line" "${DIM}Retour${RESET}"
	case "$MENU_RESULT" in
		0) CHOSEN_PROJECT="libft" ;;
		1) CHOSEN_PROJECT="printf" ;;
		2) CHOSEN_PROJECT="gnl" ;;
		*) return 1 ;;
	esac
=======
	local list=() names=() p
	CHOSEN_PROJECT=""
	for p in $(projects); do list+=("$p"); names+=("$(project_name "$p")"); done
	select_menu 0 "Quel projet ?" "${names[@]}" "${DIM}Retour${RESET}"
	[ "$MENU_RESULT" -ge "${#list[@]}" ] && return 1
	CHOSEN_PROJECT="${list[$MENU_RESULT]}"
}

ask_scope() {
	FILTER=""
	select_menu 0 "Quels tests ?" "Tous les tests" \
		"Une seule fonction    ${DIM}(tu tapes son nom, ex : ft_split)${RESET}"
	if [ "$MENU_RESULT" -eq 1 ]; then
		echo
		read -r -e -p "$(echo -e "  ${BOLD}Nom de la fonction :${RESET} ")" FILTER || exit 0
	fi
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
}

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
<<<<<<< HEAD
=======
	EVAL_MODE="${1:-0}"
	FILTER=""
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
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
<<<<<<< HEAD
	ask_valgrind
	echo -e "\n  ${BOLD}C'est parti :${RESET} $(project_name "$CHOSEN_PROJECT") ${DIM}($CHOSEN_DIR)${RESET}"
=======
	if [ "$EVAL_MODE" = 1 ]; then
		VG_OPT=""
		echo -e "\n  ${MAGENTA}${BOLD}Simulation de correction :${RESET} $(project_name "$CHOSEN_PROJECT") ${DIM}($CHOSEN_DIR)${RESET}"
	else
		ask_scope
		ask_valgrind
		echo -e "\n  ${BOLD}C'est parti :${RESET} $(project_name "$CHOSEN_PROJECT") ${DIM}($CHOSEN_DIR)${RESET}"
	fi
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
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
<<<<<<< HEAD
			"Tester un projet" "Tester les trois projets de mon depot" \
			"Voir le dernier log" "Aide" "Quitter"
		case "$MENU_RESULT" in
			0) test_wizard ;;
			1) echo; ask_valgrind; run_all
			   echo -e "\n  ${DIM}Les logs des trois projets sont dans $TESTERS/logs/${RESET}"
			   pause ;;
			2) show_log || pause ;;
			3) echo; usage; pause ;;
=======
			"Tester un projet" \
			"Simuler une correction   ${DIM}(tout est verifie, note estimee)${RESET}" \
			"Tester tous les projets de mon depot" \
			"Voir le dernier log" "Aide" "Quitter"
		case "$MENU_RESULT" in
			0) test_wizard 0 ;;
			1) test_wizard 1 ;;
			2) echo; ask_valgrind; FILTER=""; EVAL_MODE=0; run_all
			   echo -e "\n  ${DIM}Les logs sont dans $TESTERS/logs/${RESET}"
			   pause ;;
			3) show_log || pause ;;
			4) echo; usage; pause ;;
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
			*) goodbye ;;
		esac
	done
}

ARGS=()
for a in "$@"; do
	case "$a" in
		--no-valgrind) VG_OPT="--no-valgrind" ;;
		-h|--help) usage; exit 0 ;;
		*) ARGS+=("$a") ;;
	esac
done

<<<<<<< HEAD
case "${ARGS[0]}" in
=======
pick_dir_filter() {
	local a
	CLI_DIR="$1"
	shift
	for a in "$@"; do
		if [ -d "$a" ]; then CLI_DIR="$a"; else FILTER="$a"; fi
	done
}

CMD="${ARGS[0]}"
case "$CMD" in
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
	"") main_menu ;;
	ici)
		detected="$(detect "$PWD")"
		if [ -z "$detected" ]; then
			echo -e "${RED}Aucun projet reconnu dans ce dossier.${RESET}"; exit 1
		fi
<<<<<<< HEAD
		run_one "$detected" "$PWD"
		;;
	libft|printf|gnl)
		run_one "${ARGS[0]}" "${ARGS[1]:-$(default_dir "${ARGS[0]}")}"
		;;
	all) run_all ;;
	log) show_log ;;
	*) echo -e "${RED}Argument inconnu : ${ARGS[0]}${RESET}\n"; usage; exit 1 ;;
=======
		FILTER="${ARGS[1]}"
		run_one "$detected" "$PWD"
		;;
	eval)
		EVAL_MODE=1
		target="${ARGS[1]}"
		if [ -z "$target" ] || [ "$target" = "ici" ]; then
			detected="$(detect "$PWD")"
			if [ -n "$detected" ]; then run_one "$detected" "$PWD"
			else test_wizard 1; fi
		elif is_project "$target"; then
			run_one "$target" "${ARGS[2]:-$(default_dir "$target")}"
		elif [ -d "$target" ] && [ -n "$(detect "$target")" ]; then
			run_one "$(detect "$target")" "$target"
		else
			echo -e "${RED}Projet ou dossier inconnu : $target${RESET}"; exit 1
		fi
		;;
	all) run_all ;;
	log) show_log ;;
	*)
		if is_project "$CMD"; then
			pick_dir_filter "$(default_dir "$CMD")" "${ARGS[@]:1}"
			run_one "$CMD" "$CLI_DIR"
		else
			echo -e "${RED}Argument inconnu : $CMD${RESET}\n"; usage; exit 1
		fi
		;;
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
esac

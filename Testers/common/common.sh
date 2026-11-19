#!/bin/bash
# common.sh - fonctions partagees par les run.sh (affichage + fichier log)

GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; CYAN="\033[36m"
BOLD="\033[1m"; DIM="\033[2m"; RESET="\033[0m"
TESTERS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LOG_DIR="${RAKSA_LOG_DIR:-$TESTERS_DIR/logs}"
STATUS=0

# cree le fichier log : log_init <projet> <dossier teste> <options>
log_init() {
	mkdir -p "$LOG_DIR"
	LOG_FILE="$LOG_DIR/$1_$(date +%Y-%m-%d_%H-%M-%S).log"
	export LOG_FILE
	{
		echo "============================================================"
		echo " Testeur $1"
		echo "============================================================"
		echo "Date    : $(date '+%d/%m/%Y %H:%M:%S')"
		echo "Dossier : $(cd "$2" 2>/dev/null && pwd)"
		echo "Options : ${3:-aucune}"
		echo
		echo "Legende : [OK] verification reussie, [KO] verification ratee"
		echo "          (avec la valeur testee, l'attendu et l'obtenu)."
	} > "$LOG_FILE"
	ln -sf "$LOG_FILE" "$LOG_DIR/dernier.log"
}

strip_colors() { sed 's/\x1b\[[0-9;]*m//g'; }
log() { [ -n "$LOG_FILE" ] && printf '%b\n' "$*" | strip_colors >> "$LOG_FILE"; }
say() { printf '%b\n' "$*"; log "$*"; }
title() { say "\n${BOLD}=== $* ===${RESET}"; }
ok() { say "  ${GREEN}[OK]${RESET}      $*"; }
ko() { say "  ${RED}[KO]${RESET}      $*"; STATUS=1; }
warn() { say "  ${YELLOW}[!!]${RESET}      $*"; }
skip() { say "  ${YELLOW}[--]${RESET}      $*"; }

# affiche un fichier (indente) a l'ecran et dans le log ; $2 = lignes max
show_file() {
	sed 's/^/      /' "$1" | strip_colors | head -n "${2:-40}"
	[ -n "$LOG_FILE" ] && sed 's/^/      /' "$1" | strip_colors >> "$LOG_FILE"
}

# verifie le Makefile : check_makefile <dossier> <nom de la lib>
check_makefile() {
	local src="$1" lib="$2" tmp
	tmp="$(mktemp)"
	title "Makefile"
	if [ ! -f "$src/Makefile" ]; then
		ko "pas de Makefile dans le dossier"; rm -f "$tmp"; return 1
	fi
	if ! make -C "$src" >"$tmp" 2>&1; then
		ko "make a echoue :"; show_file "$tmp" 40; rm -f "$tmp"; return 1
	fi
	log "      sortie de make :"; sed 's/^/        /' "$tmp" | strip_colors >> "$LOG_FILE"
	if [ -f "$src/$lib" ]; then ok "make cree $lib a la racine"
	else ko "$lib n'a pas ete cree a la racine du dossier"; rm -f "$tmp"; return 1; fi
	make -C "$src" >"$tmp" 2>&1
	if grep -qE "Nothing to be done|is up to date" "$tmp"; then
		ok "pas de relink (un 2e make ne refait rien)"
	else
		ko "relink : un 2e make recompile quelque chose :"; show_file "$tmp" 10
	fi
	local rule
	for rule in all clean fclean re; do
		if make -C "$src" -n "$rule" >/dev/null 2>&1; then ok "regle '$rule' presente"
		else ko "regle '$rule' absente"; fi
	done
	if grep -qE -- '-Wall' "$src/Makefile" && grep -qE -- '-Wextra' "$src/Makefile" \
		&& grep -qE -- '-Werror' "$src/Makefile"; then
		ok "flags -Wall -Wextra -Werror presents"
	else
		ko "il manque un des flags -Wall -Wextra -Werror dans le Makefile"
	fi
	rm -f "$tmp"
	return 0
}

# norminette : check_norm <fichiers...>
check_norm() {
	local tmp
	title "Norminette"
	if [ $# -eq 0 ]; then
		ko "aucun fichier .c ou .h trouve"; return
	fi
	if ! command -v norminette >/dev/null 2>&1; then
		skip "norminette non installee, verification ignoree"; return
	fi
	tmp="$(mktemp)"
	if norminette "$@" >"$tmp" 2>&1; then
		ok "norminette : $(grep -c 'OK!' "$tmp") fichiers OK"
	else
		ko "norminette :"
		grep -v "OK!" "$tmp" > "$tmp.err"
		show_file "$tmp.err" 30
		rm -f "$tmp.err"
	fi
	rm -f "$tmp"
}

# fonctions externes : check_externals "<autorisees>" <fichiers .o ou .a>
check_externals() {
	local allowed="$1" found bad compiler
	shift
	found="$(nm -u "$@" 2>/dev/null | awk 'NF>=2 {print $2}' | sort -u)"
	bad=""
	compiler=""
	for f in $found; do
		case " $allowed __stack_chk_fail _GLOBAL_OFFSET_TABLE_ " in
			*" $f "*) ;;
			*)
				if nm -A "$@" 2>/dev/null | grep -qE " [TtDdBb] $f$"; then continue; fi
				case "$f" in
					memcpy|memset|memmove) compiler="$compiler $f" ;;
					*) bad="$bad $f" ;;
				esac ;;
		esac
	done
	if [ -z "$bad" ]; then ok "fonctions externes : seulement$(for a in $allowed; do echo -n " $a"; done) (autorisees)"
	else ko "fonctions externes non autorisees :$bad"; fi
	[ -n "$compiler" ] && warn "$compiler appele(s) : peut venir du compilateur, verifie ton code"
}

# recherche de fuites : run_valgrind <executable> <nom affiche>
run_valgrind() {
	local exe="$1" name="$2" tmp
	if [ "$NO_VALGRIND" = 1 ] || ! command -v valgrind >/dev/null 2>&1; then
		skip "valgrind non utilise (absent ou --no-valgrind)"; return
	fi
	tmp="$(mktemp)"
	if LOG_FILE="" valgrind -q --leak-check=full --show-leak-kinds=definite \
		--errors-for-leak-kinds=definite --error-exitcode=42 "$exe" >"$tmp" 2>&1; then
		ok "valgrind$name : aucune fuite ni erreur memoire"
	else
		ko "valgrind$name a detecte des problemes :"
		strip_colors < "$tmp" \
			| grep -E "\[(LEAK|CRASH|KO|TIMEOUT)\]|definitely lost|Invalid|uninitialised|Conditional|at 0x|by 0x|Address" \
			> "$tmp.err"
		show_file "$tmp.err" 30
		rm -f "$tmp.err"
	fi
	rm -f "$tmp"
}

log_end() {
	say "\n${DIM}Log complet : $LOG_FILE${RESET}"
}

#!/bin/bash

GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; CYAN="\033[36m"
MAGENTA="\033[35m"; BOLD="\033[1m"; DIM="\033[2m"; RESET="\033[0m"
TESTERS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LOG_DIR="${RAKSA_LOG_DIR:-$TESTERS_DIR/logs}"
STATUS=0
SECTION=""
FAILED_SECTIONS=()

strip_colors() { sed 's/\x1b\[[0-9;]*[a-zA-Z]//g'; }

vislen() {
	printf '%b' "$1" | strip_colors | LC_ALL=C.UTF-8 wc -m 2>/dev/null \
		|| printf '%b' "$1" | strip_colors | wc -c
}

repeat() {
	local i out=""
	for ((i = 0; i < $2; i++)); do out="$out$1"; done
	printf '%s' "$out"
}

log() { [ -n "$LOG_FILE" ] && printf '%b\n' "$*" >> "$LOG_FILE"; }
say() { printf '%b\n' "$*"; log "$*"; }

box() {
	local color="$1" width=0 l len
	shift
	for l in "$@"; do
		len=$(vislen "$l")
		[ "$len" -gt "$width" ] && width=$len
	done
	width=$((width + 4))
	[ $width -lt 50 ] && width=50
	say "  ${color}╭$(repeat ─ $width)╮${RESET}"
	for l in "$@"; do
		len=$(vislen "$l")
		say "  ${color}│${RESET}  $l$(repeat ' ' $((width - len - 2)))${color}│${RESET}"
	done
	say "  ${color}╰$(repeat ─ $width)╯${RESET}"
}

bar() {
	local ok=$1 total=$2 width=${3:-20} full
	if [ "$total" -gt 0 ]; then full=$((ok * width / total)); else full=$width; fi
	[ "$ok" -lt "$total" ] && [ $full -eq $width ] && full=$((width - 1))
	printf '%b' "${GREEN}$(repeat █ $full)${RED}$(repeat ░ $((width - full)))${RESET}"
}

show_project() {
	local name="$1" dir="${2/#$HOME/\~}"
	say ""
	box "$CYAN" "${BOLD}${CYAN}$name${RESET}" "${DIM}$dir${RESET}"
}

title() {
	SECTION="$*"
	local len
	len=$(vislen "$*")
	say "\n ${CYAN}${BOLD}━━ $* ${RESET}${DIM}$(repeat ━ $((56 - len)))${RESET}"
}

ok() { say "   ${GREEN}✔${RESET} $*"; }
ko() {
	say "   ${RED}✘ $*${RESET}"
	STATUS=1
	local s
	for s in "${FAILED_SECTIONS[@]}"; do [ "$s" = "$SECTION" ] && return; done
	FAILED_SECTIONS+=("$SECTION")
}
warn() { say "   ${YELLOW}!${RESET} $*"; }
skip() { say "   ${DIM}–  $*${RESET}"; }

show_file() {
	strip_colors < "$1" | head -n "${2:-40}" | sed "s/^/       $(printf '%b' "$DIM")│$(printf '%b' "$RESET") /"
	[ -n "$LOG_FILE" ] && strip_colors < "$1" | sed 's/^/       │ /' >> "$LOG_FILE"
}

log_init() {
	mkdir -p "$LOG_DIR"
	LOG_FILE="$LOG_DIR/$1_$(date +%Y-%m-%d_%H-%M-%S).log"
	export LOG_FILE
	: > "$LOG_FILE"
	{
		printf '%b\n' "${BOLD}${CYAN}Testeur $1${RESET} ${DIM}- log complet${RESET}"
		printf '%b\n' "${DIM}Date    :${RESET} $(date '+%d/%m/%Y %H:%M:%S')"
		printf '%b\n' "${DIM}Dossier :${RESET} $(cd "$2" 2>/dev/null && pwd)"
		printf '%b\n' "${DIM}Options :${RESET} ${3:-aucune}"
		printf '%b\n' "${DIM}Legende :${RESET} ${GREEN}✔${RESET} verification reussie   ${RED}✘${RESET} verification ratee (valeur testee, obtenu, attendu)"
	} >> "$LOG_FILE"
	ln -sf "$LOG_FILE" "$LOG_DIR/dernier.log"
}

verdict() {
	local s lines=()
	say ""
	if [ $STATUS -eq 0 ]; then
		box "$GREEN" "${GREEN}${BOLD}✔  $1 : TOUT EST OK${RESET}"
	else
		lines+=("${RED}${BOLD}✘  $1 : A CORRIGER${RESET}" "")
		for s in "${FAILED_SECTIONS[@]}"; do lines+=("${RED}•${RESET} $s"); done
		box "$RED" "${lines[@]}"
	fi
}

log_end() {
	verdict "$1"
	printf '%b\n' "  ${DIM}Detail de chaque verification : ${RESET}${BOLD}raksa log${RESET}${DIM}  (ou less -R ${LOG_FILE/#$HOME/\~})${RESET}"
}

run_tests() {
	local res
	res="$(mktemp)"
	RESULT_FILE="$res" "$1"
	local rc=$?
	RES_OK=0; RES_TOTAL=0; CHK_OK=0; CHK_TOTAL=0
	[ -s "$res" ] && read -r RES_OK RES_TOTAL CHK_OK CHK_TOTAL < "$res"
	rm -f "$res"
	if [ $rc -ne 0 ]; then
		STATUS=1
		[ "$RES_TOTAL" -eq 0 ] && say "   ${RED}✘ le testeur s'est arrete avant la fin (code $rc)${RESET}"
		FAILED_SECTIONS+=("${2:-Tests} : $((RES_TOTAL - RES_OK)) test(s) rate(s) sur $RES_TOTAL")
	fi
	return $rc
}

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
	log "     ${DIM}sortie de make :${RESET}"; strip_colors < "$tmp" | sed 's/^/       │ /' >> "$LOG_FILE"
	if [ -f "$src/$lib" ]; then ok "make cree $lib a la racine"
	else ko "$lib n'a pas ete cree a la racine du dossier"; rm -f "$tmp"; return 1; fi
	make -C "$src" >"$tmp" 2>&1
	if grep -qE "Nothing to be done|is up to date" "$tmp"; then
		ok "pas de relink (un 2e make ne refait rien)"
	else
		ko "relink : un 2e make recompile quelque chose :"; show_file "$tmp" 10
	fi
	local rule missing=""
	for rule in all clean fclean re; do
		make -C "$src" -n "$rule" >/dev/null 2>&1 || missing="$missing $rule"
	done
	if [ -z "$missing" ]; then ok "regles all, clean, fclean et re presentes"
	else ko "regle(s) absente(s) :$missing"; fi
	if grep -qE -- '-Wall' "$src/Makefile" && grep -qE -- '-Wextra' "$src/Makefile" \
		&& grep -qE -- '-Werror' "$src/Makefile"; then
		ok "flags -Wall -Wextra -Werror presents"
	else
		ko "il manque un des flags -Wall -Wextra -Werror dans le Makefile"
	fi
	rm -f "$tmp"
	return 0
}

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
		ok "$(grep -c 'OK!' "$tmp") fichiers a la norme"
	else
		ko "$(grep -c 'Error!' "$tmp") fichier(s) hors norme :"
		grep -v "OK!" "$tmp" > "$tmp.err"
		show_file "$tmp.err" 30
		rm -f "$tmp.err"
	fi
	rm -f "$tmp"
}

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
	if [ -z "$bad" ]; then ok "seulement des fonctions autorisees ($allowed)"
	else ko "fonctions non autorisees :$bad"; fi
	[ -n "$compiler" ] && warn "$compiler appele(s) : peut venir du compilateur, verifie ton code"
}

valgrind_check() {
	local tmp
	tmp="$(mktemp)"
	VG_ERR="$(mktemp)"
	if LOG_FILE="" TESTER_COMPACT=1 valgrind -q --leak-check=full --show-leak-kinds=definite \
		--errors-for-leak-kinds=definite --error-exitcode=42 "$1" >"$tmp" 2>&1; then
		rm -f "$tmp"; return 0
	fi
	strip_colors < "$tmp" \
		| grep -E "TIMEOUT|CRASH|LEAK|definitely lost|Invalid|uninitialised|Conditional|at 0x|by 0x|Address" \
		| sed -E "s/^==[0-9]+== ?//" > "$VG_ERR"
	rm -f "$tmp"
	return 1
}

show_valgrind() {
	local names first n
	names="$(grep -E "(LEAK|CRASH|TIMEOUT) \(" "$VG_ERR" \
		| sed -E 's/^ *✘ (.*[^ ]) +[0-9]+\/[0-9]+ +(LEAK|CRASH|TIMEOUT).*/\1 (\2)/')"
	n=$(printf '%s\n' "$names" | grep -c .)
	first="$(awk '/(LEAK|CRASH|TIMEOUT) \(/ {exit} {print}' "$VG_ERR" \
		| grep -vE "test_(libft|printf|gnl)\.c|tester\.h|vgpreload|libc\.so" | head -n 12)"
	{
		[ "$n" -gt 0 ] && echo "tests concernes ($n) :"
		printf '%s\n' "$names" | head -n 5 | sed 's/^/  • /'
		[ "$n" -gt 5 ] && echo "  • ... et $((n - 5)) autre(s)"
		[ -n "$first" ] && echo "premiere erreur (dans ton code) :" && printf '%s\n' "$first" | sed 's/^/  /'
	} > "$VG_ERR.short"
	strip_colors < "$VG_ERR.short" | sed "s/^/       $(printf '%b' "$DIM")│$(printf '%b' "$RESET") /"
	log "       ${DIM}sortie complete de valgrind :${RESET}"
	[ -n "$LOG_FILE" ] && sed 's/^/       │ /' "$VG_ERR" >> "$LOG_FILE"
	rm -f "$VG_ERR.short"
}

run_valgrind() {
	if [ "$NO_VALGRIND" = 1 ] || ! command -v valgrind >/dev/null 2>&1; then
		skip "valgrind non utilise (absent ou --no-valgrind)"; return
	fi
	if valgrind_check "$1"; then
		ok "valgrind$2 : aucune fuite ni erreur memoire"
	else
		ko "valgrind$2 a detecte des problemes :"
		show_valgrind
	fi
	rm -f "$VG_ERR"
}

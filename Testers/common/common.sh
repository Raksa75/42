#!/bin/bash

GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; CYAN="\033[36m"
MAGENTA="\033[35m"; BOLD="\033[1m"; DIM="\033[2m"; RESET="\033[0m"
TESTERS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LOG_DIR="${RAKSA_LOG_DIR:-$TESTERS_DIR/logs}"
STATUS=0
SECTION=""
FAILED_SECTIONS=()
FAILED_TESTS=()
SECTIONS_SEEN=()
RES_OK=0; RES_TOTAL=0; CHK_OK=0; CHK_TOTAL=0; RES_CRASH=0
SUM_OK=0; SUM_TOTAL=0; SUM_CRASH=0
EVAL="${RAKSA_EVAL:-0}"
FILTER="${TEST_FILTER:-}"
export TEST_FILTER="$FILTER"

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
	SECTIONS_SEEN+=("$*")
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
issue() { if [ "$EVAL" = 1 ]; then ko "$@"; else warn "$@"; fi; }
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
		for s in "${FAILED_SECTIONS[@]}" "${FAILED_TESTS[@]}"; do
			lines+=("${RED}•${RESET} $s")
		done
		box "$RED" "${lines[@]}"
	fi
}

log_end() {
	compare_state
	if [ "$EVAL" = 1 ]; then eval_sheet "$1"; else verdict "$1"; fi
	[ "$EVAL" = 1 ] && echo -e "  ${DIM}Le correcteur peut aussi te faire expliquer ton code ou te demander une petite modification.${RESET}"
	printf '%b\n' "  ${DIM}Detail de chaque verification : ${RESET}${BOLD}raksa log${RESET}${DIM}  (ou less -R ${LOG_FILE/#$HOME/\~})${RESET}"
}

run_tests() {
	local res
	res="$(mktemp)"
	RESULT_FILE="$res" "$1"
	local rc=$?
	RES_OK=0; RES_TOTAL=0; CHK_OK=0; CHK_TOTAL=0; RES_CRASH=0
	[ -s "$res" ] && read -r RES_OK RES_TOTAL CHK_OK CHK_TOTAL RES_CRASH < "$res"
	rm -f "$res"
	SUM_OK=$((SUM_OK + RES_OK)); SUM_TOTAL=$((SUM_TOTAL + RES_TOTAL))
	SUM_CRASH=$((SUM_CRASH + ${RES_CRASH:-0}))
	if [ $rc -ne 0 ]; then
		STATUS=1
		if [ "$RES_TOTAL" -eq 0 ]; then
			say "   ${RED}✘ le testeur s'est arrete avant la fin (code $rc)${RESET}"
			SUM_CRASH=$((SUM_CRASH + 1))
		fi
		FAILED_TESTS+=("${2:-Tests} : $((RES_TOTAL - RES_OK)) test(s) rate(s) sur $RES_TOTAL")
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
	local marker relinked
	marker="$(mktemp)"
	touch "$marker"
	make -C "$src" >"$tmp" 2>&1
	relinked="$(find "$src" -newer "$marker" -type f \( -name '*.o' -o -name '*.a' -o -name "$lib" \) \
		-not -path '*/.git/*' 2>/dev/null | sed "s|^$src/||" | head -5 | tr '\n' ' ')"
	rm -f "$marker"
	if [ -z "$relinked" ]; then
		ok "pas de relink (un 2e make ne refait rien)"
	else
		ko "relink : un 2e make refait $relinked:"; show_file "$tmp" 10
	fi
	local rule missing=""
	for rule in "$lib" all clean fclean re; do
		make -C "$src" -n "$rule" >/dev/null 2>&1 || missing="$missing $rule"
	done
	if [ -z "$missing" ]; then ok "regles \$(NAME), all, clean, fclean et re presentes"
	else ko "regle(s) absente(s) :$missing"; fi
	if grep -qE -- '-Wall' "$src/Makefile" && grep -qE -- '-Wextra' "$src/Makefile" \
		&& grep -qE -- '-Werror' "$src/Makefile"; then
		ok "flags -Wall -Wextra -Werror presents"
	else
		ko "il manque un des flags -Wall -Wextra -Werror dans le Makefile"
	fi
	if grep -qE '(^|[^a-z])(gcc|clang)([^a-z]|$)' "$src/Makefile"; then
		issue "le Makefile utilise gcc ou clang : le sujet demande cc"
	fi
	if grep -qE 'wildcard|\*\.c' "$src/Makefile"; then
		issue "le Makefile utilise un wildcard (*.c) : souvent refuse en correction, liste les fichiers"
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
	found="$(nm -u "$@" 2>/dev/null | awk 'NF>=2 {print $2}' | sed 's/@.*//' | sort -u)"
	bad=""
	compiler=""
	for f in $found; do
		case " $allowed __stack_chk_fail _GLOBAL_OFFSET_TABLE_ __libc_start_main __cxa_finalize _ITM_deregisterTMCloneTable _ITM_registerTMCloneTable __gmon_start__ " in
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

load_conf() {
	source "$TESTERS_DIR/$1/project.conf"
}

src_files() {
	find "$1" \( -name '*.c' -o -name '*.h' \) -not -path '*/.git/*' | sort
}

list_files() {
	if git -C "$1" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
		(cd "$1" && git ls-files .)
	else
		(cd "$1" && find . -not -path './.git/*' -not -path './.git' -type f | sed 's|^\./||')
	fi
}

is_junk() {
	case "$(basename "$1")" in
		*.o|*.a|a.out|*.out|*:Zone.Identifier|.DS_Store|*.swp|*.swo|*~|*.gch|vgcore.*) return 0 ;;
	esac
	return 1
}

check_files() {
	local src="$1" f missing="" extra="" junk="" pat rel matched all
	all="$(list_files "$src")"
	git -C "$src" rev-parse >/dev/null 2>&1 && FILES_FROM="git" || FILES_FROM="dossier"
	local untracked=""
	for f in $REQUIRED; do
		if [ ! -e "$src/$f" ]; then missing="$missing $f"
		elif [ "$FILES_FROM" = "git" ] && ! grep -qx "$f" <<< "$all"; then untracked="$untracked $f"; fi
	done
	if [ -z "$missing" ]; then ok "fichiers demandes presents : $REQUIRED"
	else ko "fichier(s) manquant(s) :$missing"; fi
	[ -n "$untracked" ] && issue "pas encore dans git (fais git add) :$untracked"
	while IFS= read -r rel; do
		[ -z "$rel" ] && continue
		if is_junk "$rel" || { [ -x "$src/$rel" ] && [[ "$rel" != *.sh ]] \
			&& head -c 4 "$src/$rel" 2>/dev/null | grep -q ELF; }; then
			junk="$junk $rel"; continue
		fi
		matched=0
		set -f
		for pat in $SUBMIT .gitignore; do
			[[ "$rel" == $pat ]] && { matched=1; break; }
		done
		set +f
		[ $matched -eq 0 ] && extra="$extra $rel"
	done <<< "$all"
	if [ -z "$junk" ]; then
		ok "aucun fichier a ne pas rendre (.o, .a, executable, Zone.Identifier...)"
	elif [ "$FILES_FROM" = "git" ]; then
		issue "fichiers suivis par git a ne pas rendre :$(echo $junk | cut -c1-200)"
		say "     ${DIM}(git rm --cached <fichier> pour les retirer du depot sans les effacer)${RESET}"
	else
		issue "fichiers a ne pas rendre :$(echo $junk | cut -c1-200)"
		[ "$EVAL" = 1 ] || say "     ${DIM}(normal pendant que tu codes ; ne les mets pas dans ton depot)${RESET}"
	fi
	[ -n "$extra" ] && warn "fichier(s) en plus de ceux du sujet :$(echo $extra | cut -c1-200)"
	[ "$FILES_FROM" = "git" ] && say "     ${DIM}(fichiers verifies : ceux suivis par git, c'est ce que le correcteur verra)${RESET}"
}

check_readme() {
	local src="$1" readme="$1/README.md" first login sec
	if [ ! -f "$readme" ]; then
		[[ " $REQUIRED " == *" README.md "* ]] || issue "pas de README.md (obligatoire dans les sujets actuels)"
		return
	fi
	first="$(head -n 1 "$readme" | tr -d '\r')"
	if [[ "$first" =~ ^[*_]This\ activity\ has\ been\ created\ as\ part\ of\ the\ 42\ curriculum\ by\ ([a-z0-9_-]+(,\ [a-z0-9_-]+)*)\.[*_]$ ]]; then
		README_LOGIN="${BASH_REMATCH[1]}"
		ok "README.md : premiere ligne en italique, par $README_LOGIN"
	else
		issue "README.md : la 1re ligne doit etre *This activity has been created as part of the 42 curriculum by <login>.*"
	fi
	for sec in Description Instructions Resources; do
		grep -qiE "^#+ *$sec" "$readme" || issue "README.md : section \"$sec\" absente"
	done
	if [ -n "$README_LOGIN" ] && [ -n "$GROUP_SIZE" ] \
		&& [ "$(echo "${README_LOGIN//,/ }" | wc -w)" -ne "$GROUP_SIZE" ]; then
		issue "README.md : projet a $GROUP_SIZE, la 1re ligne doit donner les $GROUP_SIZE logins (ici : $README_LOGIN)"
	fi
	local topic label re old_ifs="$IFS"
	IFS=';'
	for topic in $README_TOPICS; do
		IFS="$old_ifs"
		label="${topic%%:*}"
		re="${topic#*:}"
		grep -qiE "$re" "$readme" || issue "README.md : le sujet demande aussi $label"
		IFS=';'
	done
	IFS="$old_ifs"
	if ! sed -n '/^#\+ *[Rr]esources/,$p' "$readme" | grep -qiE '(^|[^a-z])(AI|IA)([^a-z]|$)|intelligence artificielle|artificial intelligence|Claude|ChatGPT|Copilot|LLM'; then
		issue "README.md : la section Resources doit dire comment l'IA a ete utilisee"
	fi
}

check_headers() {
	local src="$1" f name hname logins="" bad_name="" missing="" n=0 l
	while IFS= read -r f; do
		n=$((n + 1))
		name="$(basename "$f")"
		if ! head -n 11 "$f" | grep -q "By: .* <.*>"; then
			missing="$missing $name"; continue
		fi
		hname="$(sed -n 4p "$f" | sed -E 's|^/\* +([^ ]+) .*|\1|')"
		[ "$hname" != "$name" ] && bad_name="$bad_name $name($hname)"
		l="$(sed -n 6p "$f" | sed -E 's|.*By: ([^ ]+) <.*|\1|')"
		logins="$logins $l"
	done < <(src_files "$src")
	logins="$(echo $logins | tr ' ' '\n' | sort -u | tr '\n' ' ' | sed 's/ $//')"
	if [ -z "$missing" ]; then ok "header 42 present dans les $n fichiers .c/.h (login : ${logins:-?})"
	else issue "header 42 absent :$(echo $missing | cut -c1-200)"; fi
	[ -n "$bad_name" ] && issue "le nom dans le header ne correspond pas au fichier :$bad_name"
	if [ "$(echo $logins | wc -w)" -gt "${GROUP_SIZE:-1}" ]; then
		issue "trop de logins differents dans les headers : $logins"
	elif [ -n "$RAKSA_LOGIN" ] && [ -n "$logins" ] && [[ " $logins " != *" $RAKSA_LOGIN "* ]]; then
		issue "les headers sont au nom de $logins, pas de $RAKSA_LOGIN"
	fi
	for l in $logins; do
		if [ -n "$README_LOGIN" ] && [[ " ${README_LOGIN//,/ } " != *" $l "* ]]; then
			issue "le login $l est dans les headers mais pas sur la 1re ligne du README ($README_LOGIN)"
		fi
	done
}

check_delivery() {
	title "Rendu"
	README_LOGIN=""
	check_files "$1"
	check_readme "$1"
	check_headers "$1"
}

check_symbols() {
	local defined missing="" f
	defined="$(nm --defined-only "$@" 2>/dev/null | awk 'NF>=3 && $2 ~ /[TW]/ {print $3}' | sort -u)"
	for f in $SYMBOLS; do
		grep -qx "$f" <<< "$defined" || missing="$missing $f"
	done
	if [ -z "$missing" ] && [ "$(echo $SYMBOLS | wc -w)" -eq 1 ]; then ok "la fonction $SYMBOLS est bien definie"
	elif [ -z "$missing" ]; then ok "les $(echo $SYMBOLS | wc -w) fonctions demandees sont presentes"
	else ko "fonction(s) demandee(s) absente(s) :$missing"; fi
}

check_globals() {
	local g
	g="$(nm "$@" 2>/dev/null | awk 'NF>=3 && $2 ~ /^[BDCG]$/ {print $3}' | sort -u | tr '\n' ' ')"
	if [ -z "$g" ]; then ok "aucune variable globale"
	else ko "variable(s) globale(s) interdite(s) : $g"; fi
}

state_path() {
	local key
	key="$(printf '%s' "$(cd "$2" && pwd)" | md5sum | cut -c1-10)"
	mkdir -p "$LOG_DIR/etat"
	echo "$LOG_DIR/etat/$1_$key.etat"
}

state_init() {
	STATE_PATH="$(state_path "$1" "$2")"
	STATE_FILE="$(mktemp)"
	export STATE_FILE
}

pretty_state() {
	case "$1" in
		"test : "*) echo "${1#test : }" ;;
		"section : "*) echo "${1#section : } ${DIM}(partie)${RESET}" ;;
		*) echo "$1" ;;
	esac
}

compare_state() {
	local s st cur prev line name now before regress=() fixed=() when
	[ -n "$FILTER" ] && return
	[ -z "$STATE_FILE" ] && return
	for s in "${SECTIONS_SEEN[@]}"; do
		st="OK"
		for f in "${FAILED_SECTIONS[@]}"; do [ "$f" = "$s" ] && st="KO"; done
		printf '%s\tsection : %s\n' "$st" "$s" >> "$STATE_FILE"
	done
	for f in "${FAILED_SECTIONS[@]}"; do
		case " ${SECTIONS_SEEN[*]} " in *" $f "*) ;; *) printf 'KO\t%s\n' "$f" >> "$STATE_FILE" ;; esac
	done
	title "Par rapport au dernier passage"
	if [ ! -f "$STATE_PATH" ]; then
		skip "premier passage pour ce dossier : la prochaine fois, tu verras ce qui a change"
	else
		when="$(date -r "$STATE_PATH" '+%d/%m a %H:%M')"
		while IFS=$'\t' read -r now name; do
			before="$(grep -F -- "$name" "$STATE_PATH" | awk -F'\t' -v n="$name" '$2 == n {print $1; exit}')"
			[ "$now" = "KO" ] && [ "$before" = "OK" ] && regress+=("$name")
			[ "$now" = "OK" ] && [ "$before" = "KO" ] && fixed+=("$name")
		done < "$STATE_FILE"
		for name in "${regress[@]}"; do say "   ${RED}✘ ne passe plus :${RESET} $(pretty_state "$name")"; done
		for name in "${fixed[@]}"; do say "   ${GREEN}✔ corrige :${RESET} $(pretty_state "$name")"; done
		if [ ${#regress[@]} -eq 0 ] && [ ${#fixed[@]} -eq 0 ]; then
			skip "rien n'a change depuis le dernier passage (le $when)"
		else
			say "   ${DIM}(dernier passage le $when)${RESET}"
		fi
	fi
	cp "$STATE_FILE" "$STATE_PATH"
	rm -f "$STATE_FILE"
}

sheet_line() {
	local label="$1" st="OK" s pat
	shift
	for pat in "$@"; do
		for s in "${FAILED_SECTIONS[@]}" "${FAILED_TESTS[@]}"; do
			[[ "$s" == $pat ]] && st="KO"
		done
	done
	if [ "$st" = OK ]; then SHEET+=("  ${GREEN}✔${RESET} $label")
	else SHEET+=("  ${RED}✘ $label${RESET}"); SHEET_KO+=("$label"); fi
}

eval_sheet() {
	local note reason
	SHEET=("${BOLD}Fiche de correction (estimation)${RESET}" "" "${BOLD}Preliminaires${RESET}")
	SHEET_KO=()
	sheet_line "Fichiers rendus, README, header" "Rendu" "Fichiers"
	sheet_line "Norminette" "Norminette"
	[[ " ${SECTIONS_SEEN[*]} " == *" Makefile "* ]] \
		&& sheet_line "Makefile (regles, flags, cc, pas de relink)" "Makefile"
	sheet_line "Compilation et fonctions autorisees" "Fonctions*" "Compilation*"
	local prelim=${#SHEET_KO[@]}
	SHEET+=("" "${BOLD}Partie obligatoire${RESET}")
	SHEET_KO=()
	sheet_line "Tests ($SUM_OK/$SUM_TOTAL)" "Tests*" "BUFFER_SIZE=*"
	sheet_line "Pas de crash ni de boucle infinie" "__crash__"
	[ "$SUM_CRASH" -gt 0 ] && { SHEET[-1]="  ${RED}✘ Pas de crash ni de boucle infinie ($SUM_CRASH)${RESET}"; SHEET_KO+=("crash"); }
	sheet_line "Aucune fuite memoire" "Fuites*" "valgrind*"
	if [ "$NO_VALGRIND" = 1 ] || ! command -v valgrind >/dev/null 2>&1; then
		SHEET[-1]="  ${YELLOW}?${RESET} Aucune fuite memoire ${DIM}(valgrind absent : non verifie)${RESET}"
	fi
	if [ $prelim -gt 0 ]; then
		note=0; reason="une erreur dans les preliminaires met 0 directement"
	elif [ "$SUM_CRASH" -gt 0 ]; then
		note=0; reason="un crash ou une boucle infinie met 0"
	elif printf '%s\n' "${SHEET_KO[@]}" | grep -q "fuite"; then
		note=0; reason="une fuite memoire met 0"
	elif [ "$SUM_TOTAL" -gt 0 ] && [ "$SUM_OK" -lt "$SUM_TOTAL" ]; then
		note=0; reason="en correction, une fonction qui ne marche pas suffit en general a avoir 0"
	elif [ "$SUM_TOTAL" -gt 0 ]; then
		note=100; reason=""
	else
		note=0; reason="aucun test n'a pu etre lance"
	fi
	SHEET+=("")
	if [ $note -eq 100 ]; then SHEET+=("${GREEN}${BOLD}Note estimee : 100 / 100${RESET}")
	else SHEET+=("${RED}${BOLD}Note estimee : $note / 100${RESET}" "${DIM}$reason${RESET}"); fi
	say ""
	if [ $note -eq 100 ]; then box "$GREEN" "${SHEET[@]}"; else box "$RED" "${SHEET[@]}"; fi
}

parse_args() {
	local a
	for a in "$@"; do
		case "$a" in
			--no-valgrind) NO_VALGRIND=1 ;;
			--eval) EVAL=1 ;;
			--only=*) FILTER="${a#--only=}" ;;
		esac
	done
	export TEST_FILTER="$FILTER"
	OPTS=""
	[ "$NO_VALGRIND" = 1 ] && OPTS="$OPTS --no-valgrind"
	[ "$EVAL" = 1 ] && OPTS="$OPTS --eval"
	[ -n "$FILTER" ] && OPTS="$OPTS --only=$FILTER"
}

intro() {
	show_project "$TITLE" "$(cd "$SRC" && pwd)"
	[ "$EVAL" = 1 ] && say "  ${MAGENTA}${BOLD}Mode correction${RESET} ${DIM}: tout est verifie comme en evaluation${RESET}"
	[ -n "$FILTER" ] && say "  ${YELLOW}Seulement les tests dont le nom contient \"$FILTER\"${RESET} ${DIM}(rendu et norme non verifies)${RESET}"
	[ -z "$FILTER" ] && state_init "$ID" "$SRC"
}

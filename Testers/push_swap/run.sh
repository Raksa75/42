#!/bin/bash

cd "$(dirname "$0")" || exit 1
source ../common/common.sh
ID="push_swap"
load_conf "$ID"
SRC="${PROJECT_DIR:-$TESTERS_DIR/../$REPO_DIR}"
TMP="$(mktemp -d)"
parse_args "$@"

HAD_BIN=0
[ -f "$SRC/$BIN" ] && HAD_BIN=1
cleanup() {
	[ $HAD_BIN -eq 0 ] && make -C "$SRC" fclean >/dev/null 2>&1
	rm -rf "$TMP"
}
trap cleanup EXIT

log_init "$ID" "$SRC" "$OPTS"
intro

[ -z "$FILTER" ] && check_delivery "$SRC"
check_makefile "$SRC" "$BIN" || { log_end "$TITLE"; exit 1; }

if [ -z "$FILTER" ]; then
	check_norm $(src_files "$SRC")
	title "Fonctions"
	if [ -d "$SRC/libft" ]; then
		if grep -qE -- '-C *[^ ]*libft|libft/Makefile|-C *\$\(LIBFT|-C *\$\{LIBFT' "$SRC/Makefile"; then
			ok "le Makefile compile la libft avec son propre Makefile (dossier libft/)"
		else
			issue "dossier libft/ present, mais le Makefile ne semble pas lancer make -C libft"
		fi
	fi
	OBJS="$(find "$SRC" -name '*.o' -not -path '*/.git/*')"
	if [ -n "$OBJS" ]; then
		check_externals "$ALLOWED" $OBJS
		check_globals $OBJS
	else
		check_externals "$ALLOWED" "$SRC/$BIN"
		warn "aucun .o trouve : les variables globales n'ont pas pu etre verifiees"
	fi
fi

if ! cc -Wall -Wextra -g test_push_swap.c -o "$TMP/test" -lm 2>"$TMP/cc.log"; then
	title "Compilation du testeur"
	ko "le testeur ne compile pas :"
	show_file "$TMP/cc.log" 20
	log_end "$TITLE"; exit 1
fi

PUSH_SWAP="$(cd "$SRC" && pwd)/$BIN" FAIL_DIR="$LOG_DIR" run_tests "$TMP/test" "Tests"

title "Fuites et erreurs memoire"
if [ "$NO_VALGRIND" = 1 ] || ! command -v valgrind >/dev/null 2>&1; then
	skip "valgrind non utilise (absent ou --no-valgrind)"
else
	R20="$(shuf -i 0-9999 -n 20 | tr '\n' ' ')"
	R100="$(shuf -i 0-9999 -n 100 | tr '\n' ' ')"
	R500="$(shuf -i 0-99999 -n 500 | tr '\n' ' ')"
	CASES=("3 2 1" "--simple $R20" "--medium $R100" "--complex $R100" "--adaptive $R100"
		"--bench $R100" "$R500" "1 2 3" "1 2 1" "1 a 2" "2147483648" "--simple 3 2 3" ""
		"--simple")
	LABELS=("3 2 1" "--simple, 20 nombres" "--medium, 100 nombres" "--complex, 100 nombres"
		"--adaptive, 100 nombres" "--bench, 100 nombres" "500 nombres" "deja trie" "doublon (Error)"
		"lettre (Error)" "trop grand (Error)" "--simple + doublon (Error)" "sans argument"
		"--simple seul")
	BAD=()
	: > "$TMP/vg_all"
	for i in "${!CASES[@]}"; do
		[ -t 1 ] && printf "   ${DIM}…  valgrind : %s${RESET}\033[K\r" "${LABELS[$i]}"
		if ! valgrind -q --leak-check=full --show-leak-kinds=definite --errors-for-leak-kinds=definite \
			--error-exitcode=42 "$SRC/$BIN" ${CASES[$i]} >/dev/null 2>"$TMP/vg"; then
			if grep -qE "definitely lost|Invalid|uninitialised|Conditional" "$TMP/vg"; then
				BAD+=("${LABELS[$i]}")
				{ echo "--- ${LABELS[$i]} : ./push_swap ${CASES[$i]}"; sed -E 's/^==[0-9]+== ?//' "$TMP/vg"; } >> "$TMP/vg_all"
			fi
		fi
	done
	[ -t 1 ] && printf "\033[K"
	if [ ${#BAD[@]} -eq 0 ]; then
		ok "valgrind : aucune fuite ni erreur memoire (${#CASES[@]} cas, dont les erreurs et le mode --bench)"
	else
		ko "valgrind : fuite ou erreur memoire dans ${#BAD[@]} cas sur ${#CASES[@]} :"
		printf '%s\n' "${BAD[@]}" | sed 's/^/• /' > "$TMP/vg_short"
		echo "premiere erreur :" >> "$TMP/vg_short"
		awk 'NR>1 && /^--- /{exit} NR>1{print}' "$TMP/vg_all" | grep -vE "vgpreload|libc\.so" \
			| head -n 12 | sed 's/^/  /' >> "$TMP/vg_short"
		strip_colors < "$TMP/vg_short" | sed "s/^/       $(printf '%b' "$DIM")│$(printf '%b' "$RESET") /"
		log "       ${DIM}sortie complete de valgrind :${RESET}"
		sed 's/^/       │ /' "$TMP/vg_all" >> "$LOG_FILE"
	fi
fi

log_end "$TITLE"
exit $STATUS

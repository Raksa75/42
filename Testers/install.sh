#!/bin/bash

TESTERS="$(dirname "$(readlink -f "$0")")"
BIN="$HOME/.local/bin"
GREEN="\033[32m"; YELLOW="\033[33m"; BOLD="\033[1m"; RESET="\033[0m"

chmod +x "$TESTERS"/*.sh "$TESTERS"/*/run.sh "$TESTERS"/common/*.sh
mkdir -p "$BIN"
ln -sf "$TESTERS/raksa.sh" "$BIN/raksa"
[ -L "$BIN/correcteur" ] && rm -f "$BIN/correcteur"
echo -e "${GREEN}Commande installee :${RESET} $BIN/raksa -> $TESTERS/raksa.sh"

case ":$PATH:" in
	*":$BIN:"*) ;;
	*)
		for rc in "$HOME/.bashrc" "$HOME/.zshrc"; do
			if [ -f "$rc" ] && ! grep -q '(Testers 42)' "$rc"; then
				printf '\n# raksa (Testers 42)\nexport PATH="$HOME/.local/bin:$PATH"\n' >> "$rc"
				echo -e "${YELLOW}$BIN ajoute au PATH dans $rc${RESET}"
			fi
		done
		echo -e "Ouvre un nouveau terminal (ou lance ${BOLD}source ~/.bashrc${RESET}) pour l'utiliser."
		;;
esac
echo -e "\nTape ${BOLD}raksa${RESET} pour ouvrir le menu."

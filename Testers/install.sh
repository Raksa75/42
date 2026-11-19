#!/bin/bash
# Installe la commande "correcteur" pour pouvoir l'appeler depuis n'importe ou.
# A lancer une seule fois : ./Testers/install.sh

TESTERS="$(dirname "$(readlink -f "$0")")"
BIN="$HOME/.local/bin"
GREEN="\033[32m"; YELLOW="\033[33m"; BOLD="\033[1m"; RESET="\033[0m"

chmod +x "$TESTERS"/*.sh "$TESTERS"/*/run.sh
mkdir -p "$BIN"
ln -sf "$TESTERS/correcteur.sh" "$BIN/correcteur"
echo -e "${GREEN}Commande installee :${RESET} $BIN/correcteur -> $TESTERS/correcteur.sh"

# ajoute ~/.local/bin au PATH si ce n'est pas deja le cas
case ":$PATH:" in
	*":$BIN:"*) ;;
	*)
		for rc in "$HOME/.bashrc" "$HOME/.zshrc"; do
			if [ -f "$rc" ] && ! grep -q 'correcteur (Testers 42)' "$rc"; then
				printf '\n# correcteur (Testers 42)\nexport PATH="$HOME/.local/bin:$PATH"\n' >> "$rc"
				echo -e "${YELLOW}$BIN ajoute au PATH dans $rc${RESET}"
			fi
		done
		echo -e "Ouvre un nouveau terminal (ou lance ${BOLD}source ~/.bashrc${RESET}) pour l'utiliser."
		;;
esac
echo -e "\nEssaie : ${BOLD}correcteur -h${RESET}"

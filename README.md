# Labyrinthe à l'aveugle

Jeu de labyrinthe dont la progression se fait dans le noir guidé par différents sons.

## Prerequis

- Posséder un clavier AZERTY est recommandé, car les touches de déplacement sont conçues pour ce type de clavier. Cependant, il est possible de configurer les touches dans le code source si besoin.
- Avoir un IDE similaire à Visual Studio Code est conseillé, mais n'est pas obligatoire.

## Installation

- Clonez le dépôt [GitHub](https://github.com/paulbretonpro/labyrinthe/) sur votre ordinateur en utilisant la commande :
```bash
git clone https://github.com/paulbretonpro/labyrinthe.git
```

- Assurez-vous d'avoir installé les bibliothèques nécessaires en exécutant la commande suivante à la racine du projet :
```bash
make install
```

- Pour lancer le projet, il suffit d'exécuter la commande suivante à la racine du projet :
```bash
make run
```
Cette commande ouvre une fenêtre de jeu où vous pouvez commencer à jouer.

## Le jeu

Le but de ce jeu est de trouver la sortie d'un labyrinthe plongé dans l'obscurité.

Pour vous déplacer, vous pouvez utiliser les touches suivantes :

- La touche "Z" pour avancer
- La touche "Q" pour tourner à gauche
- La touche "D" pour tourner à droite

Votre progression sera guidée par des sons émis par les murs afin de vous repérer.

En mode simple (par défaut), appuyez sur :
- La touche "U" pour qu'un son soit émis si un mur se trouve devant vous
- La touche "K" pour qu'un son soit émis si un mur se trouve à votre droite
- La touche "H" pour qu'un son soit émis si un mur se trouve à votre gauche

La touche "J" est une combinaison des trois touches précédentes ('U', 'K', 'H').

En mode avancé (activer/désactiver avec la touche "L"), le son est émis en permanence.

Par ailleurs, un son de pas sera émis lorsque vous avancez et un son de collision si vous heurtez un mur.
	
#### Fin

Lorsque vous atteindrez la case de sortie, le jeu vous félicitera et se terminera.

#### Aide

Un mode graphique est disponible via la touche "B" (activer/désactiver).

*Made by: Paul Breton, Léane Diraison et Florian Monfort*

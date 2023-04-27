# Labyrinthe à l'aveugle

Jeu de labyrinthe dont la progression se fait dans le noir guidé par différents sons.

## Prerequis
- Posséder un clavier AZERTY est recommandé, car les touches de déplacement sont conçues pour ce type de clavier. Cependant, il est possible de configurer les touches dans le code source si besoin.

- Avoir un IDE similaire à Visual Studio Code est conseillé, mais n'est pas obligatoire.


## Installation
- Clonez le dépôt [GitHub](https://github.com/paulbretonpro/labyrinthe/) sur votre ordinateur en utilisant la commande:
```bash
git clone
```
- Assurez-vous d'avoir installé les bibliothèques nécessaires en exécutant la commande suivante à la racine du projet:
```bash
make install
```

- Pour lancer le projet, il suffit d'exécuter la commande suivante à la racine du projet:
```bash
make run
```
Cette commande ouvre une fenêtre de jeu où vous pouvez commencer à jouer.


## Le jeu
Le but de ce jeu est de trouver la sortie du labyrinthe plongé dans l'obscurité.

Pour vous déplacer, vous pouvez utiliser les touches suivantes :

- La touche "Z" pour avancer
- La touche "D" pour tourner à droite
- La touche "Q" pour tourner à gauche

Votre progression sera guidée par différents sons :

#### Début :
- Le premier son que vous entendrez est celui du départ, symbolisé par le son "top".
	
#### Déroulement :
Les sons pour se repérer:
- En appuyant sur la touche "U", un son sera émis si un mur se trouve devant vous.
- En appuyant sur la touche "K", un son sera émis si un mur se trouve à votre droite.
- En appuyant sur la touche "H", un son sera émis si un mur se trouve à votre gauche.
- La touche "J" est une combinaison des trois touches précédentes ('U', 'K', 'H').

Les sons prouvant un déplacements:
- Lorsque vous avancez, un son de pas sera émis.
- Si vous touchez un mur en avançant, un son de collision sera émis.
	
#### Fin :
- Plus vous vous rapprocherez de la sortie, plus un son de canard intense sera émis.
- Lorsque vous atteindrez la case de sortie, un son de fin sera émis.




*Made by: Paul Breton, Léane Diraison et Florian Monfort*

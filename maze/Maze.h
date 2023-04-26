#ifndef MAZE_H
#define MAZE_H

const int N = 1, S = 2, E = 4, W = 8;
const char DELIMITER = '\t';

using namespace std;

// Définition de la classe Maze

class Maze
{
private:
    size_t m_Width;
    size_t m_Height;
    vector<vector<int>> m_Matrix;

public:
    /** constructeur, crée le labyrinthe */
    Maze(size_t width, size_t height, size_t weight, int seed);

    /** constructeur, charge le labyrinthe */
    Maze(vector<vector<int>> matrix);

    /**
     * rend la classe indexable pour y accéder dans la Scene
     * @param i : indice à lire
     */
    vector<int>& Maze::operator[] (int i);

    /**
     * exporte le labyrinthe au format numérique dans un fichier CSV (tabs)
     * @param filename : chemin du fichier dans lequel sauvegarder le labyrinthe (écrasé si déjà existant)
     */
    void export_digital(const string& filename);

    /**
     * exporte le labyrinthe au format graphique
     * @param filename : chemin du fichier dans lequel sauvegarder le labyrinthe (écrasé si déjà existant)
     */
    void export_graphical(const string& filename);

    /**
     * convertit un fichier .num en matrice de nombres
     */
    static Maze import_from_file(const string& filename);

    /*
     * affiche la matrice dans STDOUT
     */
     void print();
};

#endif

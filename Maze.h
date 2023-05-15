#ifndef MAZE_H
#define MAZE_H

// Définition de la classe Maze

class Maze
{
private:
    /** constructeur interne */
    std::vector<std::vector<int>> create_matrix(size_t width, size_t height, size_t weight, int seed);

public:
    const size_t m_Height;
    const size_t m_Width;
    const std::vector<std::vector<int>> m_Matrix;

    /** constructeur, crée le labyrinthe */
    Maze(size_t width, size_t height, size_t weight, int seed);

    /** constructeur, charge le labyrinthe */
    Maze(std::vector<std::vector<int>> matrix);

    /**
     * exporte le labyrinthe au format numérique dans un fichier CSV (tabs)
     * @param filename : chemin du fichier dans lequel sauvegarder le labyrinthe (écrasé si déjà existant)
     */
    void export_digital(const std::string& filename);

    /**
     * exporte le labyrinthe au format graphique
     * @param filename : chemin du fichier dans lequel sauvegarder le labyrinthe (écrasé si déjà existant)
     */
    void export_graphical(const std::string& filename);

    /**
     * convertit un fichier .num en matrice de nombres
     */
    static Maze* import_from_file(const std::string& filename);

    /*
     * affiche la matrice dans STDOUT
     */
     void print();
};

#endif

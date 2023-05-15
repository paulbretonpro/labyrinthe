#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "Maze.h"

/** constructeur */
/**
 * génère un labyrinthe
 * @param width : largeur du labyrinthe (en cases)
 * @param height : hauteur du labyrinthe (en cases)
 * @param weight : poids dans la création
 * @param seed : graine de génération
 */
Maze::Maze(size_t width, size_t height, size_t weight, int seed):
m_Height(height),
m_Width(width),
// Crée une matrice vide correspondant aux dimensions fournies
m_Matrix(create_matrix(width, height, weight, seed))
{
}

/** constructeur */
/**
 * charge un labyrinthe
 * @param matrix : matrice des cases du labyrinthe
 */
Maze::Maze(std::vector<std::vector<int>> matrix):
m_Height(matrix.size()),
m_Width(matrix[0].size()),
m_Matrix(matrix)
{
    if (m_Height == 0 || m_Width == 0)
    {
        std::cerr << "cannot load an empty maze" << std::endl;
        exit(1);
    }
}

std::vector<std::vector<int>> Maze::create_matrix(size_t width, size_t height, size_t weight, int seed)
{
    std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 0));

    // Définit la graine de génération aléatoire
    srand(seed);

    // Pour chaque ligne
    for (size_t y = 0; y < height; y++)
    {
        int run_start = 0;

        // Pour chaque colonne
        for (size_t x = 0; x < width; x++)
        {
            // Si on a dépassé la première ligne (on ne crée pas de murs sur la première ligne)
            // Termine ce couloir horizontal si on a atteint la dernière colonne ou sinon de manière aléatoire (1 chance sur weigth)
            if (y > 0 && (x + 1 == width || rand() % weight == 0))
            {
                // Sélectionne une case parmi celles du couloir pour créer une ouverture vers le haut
                int cell = run_start + rand() % (x - run_start + 1);

                // Crée cette ouverture vers le haut dans cette cellule
                matrix[y][cell] |= NORTH;
                // Et son réciproque vers le bas dans la cellule du dessus
                matrix[y - 1][cell] |= SOUTH;

                // Et prépare un nouveau couloir horizontal
                run_start = x + 1;
            }

            // Sinon, le couloir horizontal n'est pas terminé
            // Et si on n'a pas atteint la dernière colonne (pas d'ouverture permise à droite)
            else if (x + 1 < width)
            {
                // Crée une ouverture à droite dans cette cellule
                matrix[y][x] |= EAST;
                // Et son réciproque à gauche dans la cellule de droite
                matrix[y][x + 1] |= WEST;
            }
        }
    }

    return matrix;
}

/**
 * exporte le labyrinthe au format numérique dans un fichier CSV (tabs)
 * @param filename : chemin du fichier dans lequel sauvegarder le labyrinthe (écrasé si déjà existant)
 */
void Maze::export_digital(const std::string& filename)
{
    // Crée et ouvre le fichier
    std::ofstream output_file(filename);

    // En cas d'erreur, termine
    if (! output_file)
    {
        std::cerr << "unable to open file " << filename << std::endl;
        exit(1);
    }

    // Pour chaque ligne
    for (size_t y = 0; y < m_Height; y++)
    {
        // Pour chaque colonne
        for (size_t x = 0; x < m_Width; x++)
        {
            // Affiche la valeur de la cellule
            output_file << m_Matrix[y][x];

            // Et un séparateur si on n'a pas atteint la dernière colonne
            if (x + 1 < m_Width)
                output_file << "\t";
        }

        output_file << std::endl;
    }

    output_file.close();
}


/**
 * exporte le labyrinthe au format graphique
 * @param filename : chemin du fichier dans lequel sauvegarder le labyrinthe (écrasé si déjà existant)
 */
void Maze::export_graphical(const std::string& filename)
{
    // Crée et ouvre le fichier
    std::ofstream output_file(filename);

    // Affiche la bordure Nord du labyrinthe
    output_file << " ";
    for (size_t i = 0; i < m_Width * 2 - 1; i++)
    {
        output_file << "_";
    }
    output_file << std::endl;

    // Pour chaque ligne
    for (size_t y = 0; y < m_Height; y++)
    {
        // Affiche la bordure Ouest du labyrinthe
        output_file << "|";

        // Puis pour chaque colonne
        for (size_t x = 0; x < m_Width; x++)
        {
            int cell = m_Matrix[y][x];

            // Affiche la bordure en bas s'il n'y a pas d'ouverture vers le bas
            output_file << ((cell & SOUTH) == 0 ? "_" : " ");

            // Affiche la bordure à droite s'il n'y a pas d'ouverture vers la droite
            output_file << ((cell & EAST) == 0 ? "|" : " ");
        }

        // Move to the next row
        output_file << std::endl;
    }

    // Close the output file
    output_file.close();
}

/**
 * importe un labyrinthe depuis un fichier au format numérique
 * @param filename : chemin du fichier depuis lequel charger le labyrinthe
 * @return matrice : matrice des nombres représentant les cases du labyrinthe
 */
Maze* Maze::import_from_file(const std::string& filename)
{
    // Crée la matrice à retourner
    std::vector<std::vector<int>> matrix;

    // Crée et ouvre le fichier
    std::ifstream input_file(filename);

    // En cas d'erreur, termine
    if (! input_file) {
        std::cerr << "unable to open file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    std::string token;
    std::vector<int> row;
    while (getline(input_file, line))
    {
        row.clear();
        std::stringstream linestream(line);

        while (getline(linestream, token, DELIMITER))
        {
            row.push_back(std::stoi(token));
        }

        matrix.push_back(row);
    }

    return new Maze(matrix);
}

/*
 * affiche la matrice dans STDOUT
 */
void Maze::print()
{
    for (size_t i = 0; i < m_Height; i++)
    {
        for (size_t j = 0; j < m_Width; j++)
        {
            std::cout << m_Matrix[i][j] << '\t';
        }
        std::cout << std::endl;
    }
}

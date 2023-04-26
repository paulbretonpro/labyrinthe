#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "Maze.h"

using namespace std;

/** constructeur */
/**
 * génère un labyrinthe
 * @param width : largeur du labyrinthe (en cases)
 * @param height : hauteur du labyrinthe (en cases)
 * @param weight : poids dans la création
 * @param seed : graine de génération
 */
Maze::Maze(size_t width, size_t height, size_t weight, int seed)
// Crée une matrice vide correspondant aux dimensions fournies
: m_Matrix(height, vector<int>(width, 0))
{
    m_Width = width;
    m_Height = height;

    // Définit la graine de génération aléatoire
    srand(seed);

    // Pour chaque ligne
    for (size_t y = 0; y < m_Height; y++)
    {
        int run_start = 0;

        // Pour chaque colonne
        for (size_t x = 0; x < m_Width; x++)
        {
            // Si on a dépassé la première ligne (on ne crée pas de murs sur la première ligne)
            // Termine ce couloir horizontal si on a atteint la dernière colonne ou sinon de manière aléatoire (1 chance sur weigth)
            if (y > 0 && (x + 1 == m_Width || rand() % weight == 0))
            {
                // Sélectionne une case parmi celles du couloir pour créer une ouverture vers le haut
                int cell = run_start + rand() % (x - run_start + 1);

                // Crée cette ouverture vers le haut dans cette cellule
                m_Matrix[y][cell] |= N;
                // Et son réciproque vers le bas dans la cellule du dessus
                m_Matrix[y - 1][cell] |= S;

                // Et prépare un nouveau couloir horizontal
                run_start = x + 1;
            }

            // Sinon, le couloir horizontal n'est pas terminé
            // Et si on n'a pas atteint la dernière colonne (pas d'ouverture permise à droite)
            else if (x + 1 < m_Width)
            {
                // Crée une ouverture à droite dans cette cellule
                m_Matrix[y][x] |= E;
                // Et son réciproque à gauche dans la cellule de droite
                m_Matrix[y][x + 1] |= W;
            }
        }
    }
}

/** constructeur */
/**
 * charge un labyrinthe
 * @param matrix : matrice des cases du labyrinthe
 */
Maze::Maze(vector<vector<int>> matrix)
{
    const size_t height = matrix.size();
    const size_t width = matrix[0].size();

    if (height == 0 || width == 0)
    {
        cerr << "cannot load an empty maze" << endl;
        exit(1);
    }

    m_Width = width;
    m_Height = height;
    m_Matrix = matrix;
}

/**
 * rend la classe indexable pour y accéder dans la Scene
 * @param i : indice à lire
 */
vector<int>& Maze::operator[] (int i)
{
    return m_Matrix[i];
}

/**
 * exporte le labyrinthe au format numérique dans un fichier CSV (tabs)
 * @param filename : chemin du fichier dans lequel sauvegarder le labyrinthe (écrasé si déjà existant)
 */
void Maze::export_digital(const string& filename)
{
    // Crée et ouvre le fichier
    ofstream output_file(filename);

    // En cas d'erreur, termine
    if (! output_file)
    {
        cerr << "unable to open file " << filename << endl;
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

        output_file << endl;
    }

    output_file.close();
}


/**
 * exporte le labyrinthe au format graphique
 * @param filename : chemin du fichier dans lequel sauvegarder le labyrinthe (écrasé si déjà existant)
 */
void Maze::export_graphical(const string& filename)
{
    // Crée et ouvre le fichier
    ofstream output_file(filename);
    

    // Affiche la bordure Nord du labyrinthe
    output_file << " ";
    for (size_t i = 0; i < m_Width * 2 - 1; i++)
    {
        output_file << "_";
    }
    output_file << endl;

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
            output_file << ((cell & S) == 0 ? "_" : " ");

            // Affiche la bordure à droite s'il n'y a pas d'ouverture vers la droite
            output_file << ((cell & E) == 0 ? "|" : " ");
        }

        // Move to the next row
        output_file << endl;
    }

    // Close the output file
    output_file.close();
}

/**
 * importe un labyrinthe depuis un fichier au format numérique
 * @param filename : chemin du fichier depuis lequel charger le labyrinthe
 * @return matrice : matrice des nombres représentant les cases du labyrinthe
 */
Maze Maze::import_from_file(const string& filename)
{
    // Crée la matrice à retourner
    vector<vector<int>> matrix;

    // Crée et ouvre le fichier
    ifstream input_file(filename);

    // En cas d'erreur, termine
    if (! input_file) {
        cerr << "unable to open file " << filename << endl;
        exit(1);
    }

    string line;
    string token;
    vector<int> row;
    while (getline(input_file, line))
    {
        row.clear();
        stringstream linestream(line);

        while (getline(linestream, token, DELIMITER))
        {
            row.push_back(stoi(token));
        }

        matrix.push_back(row);
    }

    return Maze(matrix);
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
            cout << m_Matrix[i][j] << '\t';
        }
        cout << endl;
    }
}


int main(int argc, char** argv) {
    size_t width = argc > 1 ? stoi(argv[1]) : 10;
    size_t height = argc > 2 ? stoi(argv[2]) : width;
    size_t weight = argc > 3 ? stoi(argv[3]) : 2;
    int seed = argc > 4 ? stoi(argv[4]) : time(nullptr);

    Maze maze = Maze(width, height, weight, seed);
    maze.export_digital("maze.num");
    maze.export_graphical("maze.txt");
    Maze newMaze = Maze::import_from_file("maze.num");

    // cout << argv[0] << " " << width << " " << height << " " << weight << " " << seed << endl;

    return 0;
}

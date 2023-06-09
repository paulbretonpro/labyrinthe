#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>

#include "constants.h"
#include "Cube.h"
#include "Maze.h"

class Scene
{
private:
    // matrices de transformation des objets de la scène
    mat4 m_MatP;
    mat4 m_MatV;
    mat4 m_MatVM;
    mat4 m_MatTMP;

    // caméra table tournante
    float m_Azimut;
    float m_Elevation;
    float m_Distance;
    bool m_SimplePlayerMode;
    bool m_DebugMode;
    vec3 m_Center;

    // souris
    bool m_Clicked;
    double m_MousePrecX;
    double m_MousePrecY;

    // Labyrinthe
    const Maze* m_Maze;

    // Matrice des cubes
    const std::vector<std::vector<Cube*>> m_Cube;

    // Sources liées au joueur 
    ALuint m_Walkource;
    ALuint m_CollisionSource;

    // Sources autour du joueur
    ALuint m_Source[4];

    // Orientation du joueur
    const float* m_ListenerOrientation;

    // Position in Maze and orientation N S E W
    int m_Position[2];
    int m_Direction;

    // Constructor auxiliary method
    std::vector<std::vector<Cube*>> make_cube_matrix();

    // Moving
    void rotateLeft();
    void rotateRight();
    bool stepForward();

public:
    /** constructeur, crée les objets 3D à dessiner */
    Scene(const std::string& filename);

    /** destructeur, libère les ressources */
    ~Scene();

    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    void onSurfaceChanged(int width, int height);

    /**
     * appelée quand on enfonce un bouton de la souris
     * @param btn : GLFW_MOUSE_BUTTON_LEFT pour le bouton gauche
     * @param x coordonnée horizontale relative à la fenêtre
     * @param y coordonnée verticale relative à la fenêtre
     */
    void onMouseDown(int btn, double x, double y);

    /**
     * appelée quand on relache un bouton de la souris
     * @param btn : GLFW_MOUSE_BUTTON_LEFT pour le bouton gauche
     * @param x coordonnée horizontale relative à la fenêtre
     * @param y coordonnée verticale relative à la fenêtre
     */
    void onMouseUp(int btn, double x, double y);

    /**
     * appelée quand on bouge la souris
     * @param x coordonnée horizontale relative à la fenêtre
     * @param y coordonnée verticale relative à la fenêtre
     */
    void onMouseMove(double x, double y);

    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    void onKeyDown(unsigned char code);

    void updateListener();
    void updateSound();
    void playSoundLeft();
    void playSoundFront();
    void playSoundRight();

    /** Dessine l'image courante */
    void onDrawFrame();
};

#endif

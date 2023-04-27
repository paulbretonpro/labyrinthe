#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <utils.h>

#include "Scene.h"

/** Directions constants */
const int NORTH = 1;
const int SOUTH = 2;
const int EAST = 4;
const int WEST = 8;

const int SOURCE_NORTH = 0;
const int SOURCE_SOUTH = 1;
const int SOURCE_EAST = 2;
const int SOURCE_WEST = 3;

/** constructeur */
Scene::Scene()
{
    // INIT position in maze
    this->position[0] = 0;
    this->position[1] = 0;
    this->direction = SOUTH;

    // INIT Maze
    m_labyrinthe[0][0] = 2;
    m_labyrinthe[0][1] = 4;
    m_labyrinthe[0][2] = 14;
    m_labyrinthe[0][3] = 10;
    m_labyrinthe[0][4] = 2;

    m_labyrinthe[1][0] = 7;
    m_labyrinthe[1][1] = 8;
    m_labyrinthe[1][2] = 3;
    m_labyrinthe[1][3] = 5;
    m_labyrinthe[1][4] = 9;

    m_labyrinthe[2][0] = 5;
    m_labyrinthe[2][1] = 14;
    m_labyrinthe[2][2] = 15;
    m_labyrinthe[2][3] = 12;
    m_labyrinthe[2][4] = 10;

    m_labyrinthe[3][0] = 4;
    m_labyrinthe[3][1] = 9;
    m_labyrinthe[3][2] = 5;
    m_labyrinthe[3][3] = 8;
    m_labyrinthe[3][4] = 1;

    // CREATE cube with maze
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            m_Cube[row][col] = new Cube(m_labyrinthe[row][col]);
        }
    }

    // caractéristiques de la lampe
    m_Light = new Light();
    m_Light->setColor(500.0, 500.0, 500.0);
    m_Light->setPosition(0.0, 16.0, 13.0, 1.0);
    m_Light->setDirection(0.0, -1.0, -1.0, 0.0);
    m_Light->setAngles(30.0, 40.0);

    // couleur du fond : gris foncé
    glClearColor(0.333, 0.333, 0.333, 0.0);

    // activer le depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // initialiser les matrices
    m_MatP = mat4::create();
    m_MatV = mat4::create();
    m_MatVM = mat4::create();
    m_MatTMP = mat4::create();

    // gestion vue et souris
    m_Azimut = 180.0;
    m_Elevation = 0.0;
    m_Distance = 0.0;
    m_Center = vec3::create();
    m_Clicked = false;

    // ouverture du flux audio à placer dans le buffer
    ALuint buffer = alutCreateBufferFromFile("data/Aaah.wav");
    if (buffer == AL_NONE) {
        std::cerr << "unable to open sound file " << std::endl;
        alGetError();
        throw std::runtime_error("file not found or not readable");
    }

    // lien buffer -> sources
    alGenSources(4, m_Source);

    for (int i = 0; i < 4; i++)
    {
        alSourcei(m_Source[i], AL_BUFFER, buffer);

        // propriétés de la source à l'origine
        alSource3f(m_Source[i], AL_VELOCITY, 0, 0, 0);
        alSourcei(m_Source[i], AL_LOOPING, AL_FALSE);

        // diminue le gain du son
        alSourcef(m_Source[i], AL_GAIN, 0.5f);

        // dans un cone d'angle [-inner/2, inner/2] il n'y a pas d'attenuation
        alSourcef(m_Source[i], AL_CONE_INNER_ANGLE, 20);

        // dans un cone d'angle [-outer/2, outer/2] il y a une attenuation linéaire entre 0 et le gain
        alSourcef(m_Source[i], AL_CONE_OUTER_GAIN, 0);

        // à l'extérieur de [-outer/2, outer/2] il y a une attenuation totale
        alSourcef(m_Source[i], AL_CONE_OUTER_ANGLE, 80);
    }

    // on positionne les sources autour du joueur (0, 0, 0) par défaut
    alSource3f(m_Source[SOURCE_NORTH], AL_POSITION, 0.5, 0, 0);
    alSource3f(m_Source[SOURCE_SOUTH], AL_POSITION, 0.5, 1, 0);
    alSource3f(m_Source[SOURCE_EAST], AL_POSITION, 1, 0.5, 0);
    alSource3f(m_Source[SOURCE_WEST], AL_POSITION, 0, 0.5, 0);
}

/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // met en place le viewport
    glViewport(0, 0, width, height);

    // matrice de projection (champ de vision)
    mat4::perspective(m_MatP, Utils::radians(80.0), (float)width / height, 0.1, 100.0);
}

/**
 * appelée quand on enfonce un bouton de la souris
 * @param btn : GLFW_MOUSE_BUTTON_LEFT pour le bouton gauche
 * @param x coordonnée horizontale relative à la fenêtre
 * @param y coordonnée verticale relative à la fenêtre
 */
void Scene::onMouseDown(int btn, double x, double y)
{
    if (btn != GLFW_MOUSE_BUTTON_LEFT)
        return;
    m_Clicked = true;
    m_MousePrecX = x;
}

/**
 * appelée quand on relache un bouton de la souris
 * @param btn : GLFW_MOUSE_BUTTON_LEFT pour le bouton gauche
 * @param x coordonnée horizontale relative à la fenêtre
 * @param y coordonnée verticale relative à la fenêtre
 */
void Scene::onMouseUp(int btn, double x, double y)
{
    m_Clicked = false;
}

/**
 * appelée quand on bouge la souris
 * @param x coordonnée horizontale relative à la fenêtre
 * @param y coordonnée verticale relative à la fenêtre
 */
void Scene::onMouseMove(double x, double y)
{
    if (!m_Clicked)
        return;
    m_Azimut += (x - m_MousePrecX) * 0.1;
    if (m_Elevation > 90.0)
        m_Elevation = 90.0;
    if (m_Elevation < -90.0)
        m_Elevation = -90.0;
    m_MousePrecX = x;
}

/**
 * appelée quand on appuie sur une touche du clavier
 * @param code : touche enfoncée
 */
void Scene::onKeyDown(unsigned char code)
{
    // construire la matrice inverse de l'orientation de la vue à la souris
    mat4::identity(m_MatTMP);
    mat4::rotateY(m_MatTMP, m_MatTMP, Utils::radians(-m_Azimut));
    mat4::rotateX(m_MatTMP, m_MatTMP, Utils::radians(-m_Elevation));

    // vecteur indiquant le décalage à appliquer au pivot de la rotation
    vec3 offset = vec3::create();

    switch (code)
    {
        case GLFW_KEY_W: // avant
            if (this->canMove() == true)
            {
                vec3::transformMat4(offset, vec3::fromValues(0, 0, 1), m_MatTMP);

                // obtenir la direction relative à la caméra
                vec4 dir = vec4::fromValues(0, 0, 1, 0);
                vec4::transformMat4(dir, dir, m_MatVM);

                // on repositionne les sources autour du joueur
                for (int i = 0; i < 4; i++)
                {
                    float x, y, z;
                    alGetSource3f(m_Source[i], AL_POSITION, &x, &y, &z);

                    switch(direction)
                    {
                        case NORTH:
                            alSource3f(m_Source[i], AL_POSITION, x, y - 1, z);
                            alSource3f(m_Source[i], AL_DIRECTION, dir[0], dir[1], dir[2]);
                            break;
                        case SOUTH:
                            alSource3f(m_Source[i], AL_POSITION, x, y + 1, z);
                            alSource3f(m_Source[i], AL_DIRECTION, dir[0], dir[1], dir[2]);
                            break;
                        case EAST:
                            alSource3f(m_Source[i], AL_POSITION, x + 1, y, z);
                            alSource3f(m_Source[i], AL_DIRECTION, dir[0], dir[1], dir[2]);
                            break;
                        case WEST:
                            alSource3f(m_Source[i], AL_POSITION, x - 1, y, z);
                            alSource3f(m_Source[i], AL_DIRECTION, dir[0], dir[1], dir[2]);
                            break;
                    }
                }
            }
            else
            {
                std::cout << "PAS POSSIBLE" << std::endl;
            }
            break;

        case GLFW_KEY_D: // rotation à droite
            m_Azimut += 90;
            rotateRight();
            break;

        case GLFW_KEY_A: // rotation à gauche
            m_Azimut -= 90;
            rotateLeft();
            break;

        case GLFW_KEY_H: // son à gauche
            playSoundLeft();
            break;

        case GLFW_KEY_U: // son devant
            playSoundFront();
            break;

        case GLFW_KEY_K: // son à droite
            playSoundRight();
            break;

        case GLFW_KEY_J: // son des 3 directions à la fois
            playSoundLeft();
            playSoundFront();
            playSoundRight();
            break;

        default:
            return;
    }

    // appliquer le décalage au centre de la rotation
    vec3::add(m_Center, m_Center, offset);
}

void Scene::playSoundLeft()
{
    int cellValue = m_labyrinthe[position[0]][position[1]];

    switch(direction)
    {
        case NORTH:
            if ((cellValue & WEST) == 0)
            {
                alSourcePlay(m_Source[SOURCE_WEST]);
            }
            break;

        case SOUTH:
            if ((cellValue & EAST) == 0)
            {
                alSourcePlay(m_Source[SOURCE_EAST]);
            }
            break;

        case EAST:
            if ((cellValue & NORTH) == 0)
            {
                alSourcePlay(m_Source[SOURCE_NORTH]);
            }
            break;

        case WEST:
            if ((cellValue & SOUTH) == 0)
            {
                alSourcePlay(m_Source[SOURCE_SOUTH]);
            }
            break;
    }
}

void Scene::playSoundFront()
{
    int cellValue = m_labyrinthe[position[0]][position[1]];

    switch(direction)
    {
        case NORTH:
            if ((cellValue & NORTH) == 0)
            {
                alSourcePlay(m_Source[SOURCE_NORTH]);
            }
            break;

        case SOUTH:
            if ((cellValue & SOUTH) == 0)
            {
                alSourcePlay(m_Source[SOURCE_SOUTH]);
            }
            break;

        case EAST:
            if ((cellValue & EAST) == 0)
            {
                alSourcePlay(m_Source[SOURCE_EAST]);
            }
            break;

        case WEST:
            if ((cellValue & WEST) == 0)
            {
                alSourcePlay(m_Source[SOURCE_WEST]);
            }
            break;
    }
}

void Scene::playSoundRight()
{
    int cellValue = m_labyrinthe[position[0]][position[1]];

    switch(direction)
    {
        case NORTH:
            if ((cellValue & EAST) == 0)
            {
                alSourcePlay(m_Source[SOURCE_EAST]);
            }
            break;

        case SOUTH:
            if ((cellValue & WEST) == 0)
            {
                alSourcePlay(m_Source[SOURCE_WEST]);
            }
            break;

        case EAST:
            if ((cellValue & SOUTH) == 0)
            {
                alSourcePlay(m_Source[SOURCE_SOUTH]);
            }
            break;

        case WEST:
            if ((cellValue & NORTH) == 0)
            {
                alSourcePlay(m_Source[SOURCE_NORTH]);
            }
            break;
    }
}

/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    /** préparation des matrices **/

    // positionner la caméra
    mat4::identity(m_MatV);

    // éloignement de la scène
    mat4::translate(m_MatV, m_MatV, vec3::fromValues(0.0, 0.0, -m_Distance));

    // rotation demandée par la souris
    mat4::rotateX(m_MatV, m_MatV, Utils::radians(m_Elevation));
    mat4::rotateY(m_MatV, m_MatV, Utils::radians(m_Azimut));

    // centre des rotations
    mat4::translate(m_MatV, m_MatV, m_Center);

    /** gestion des lampes **/

    // calculer la position et la direction de la lampe par rapport à la scène
    m_Light->transform(m_MatV);

    // fournir position et direction en coordonnées caméra aux objets éclairés

    /** dessin de l'image **/

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            m_Cube[row][col]->onRender(m_MatP, m_MatV);
            mat4::translate(m_MatV, m_MatV, vec3::fromValues(1.0, 0.0, 0.0));
        }
        mat4::translate(m_MatV, m_MatV, vec3::fromValues(-5.0, 0.0, 1.0));
    }
}

void Scene::rotateLeft()
{
    switch (this->direction)
    {
    case NORTH:
        direction = WEST;
        break;
    case SOUTH:
        direction = EAST;
        break;
    case EAST:
        direction = NORTH;
        break;
    case WEST:
        direction = SOUTH;
        break;

    default:
        break;
    }
}

void Scene::rotateRight()
{
    switch (direction)
    {
    case NORTH:
        direction = EAST;
        break;
    case SOUTH:
        direction = WEST;
        break;
    case EAST:
        direction = SOUTH;
        break;
    case WEST:
        direction = NORTH;
        break;

    default:
        break;
    }
}

bool Scene::canMove()
{
    int x = this->position[0];
    int y = this->position[1];

    int mazeValue = this->m_labyrinthe[x][y];

    if (this->direction == NORTH && (mazeValue == 1 || mazeValue == 3 || mazeValue == 5 || mazeValue == 7 || mazeValue == 9 || mazeValue == 11 || mazeValue == 13 || mazeValue == 15))
    {
        this->position[0] = x - 1;
        return true;
    }
    if (this->direction == SOUTH && (mazeValue == 2 || mazeValue == 3 || mazeValue == 6 || mazeValue == 7 || mazeValue == 10 || mazeValue == 11 || mazeValue == 14 || mazeValue == 15))
    {
        this->position[0] = x + 1;
        return true;
    }
    if (this->direction == EAST && (mazeValue == 4 || mazeValue == 5 || mazeValue == 6 || mazeValue == 7 || mazeValue == 12 || mazeValue == 13 || mazeValue == 14 || mazeValue == 15))
    {
        this->position[1] = y + 1;
        return true;
    }
    if (this->direction == WEST && (mazeValue == 8 || mazeValue == 9 || mazeValue == 10 || mazeValue == 11 || mazeValue == 12 || mazeValue == 13 || mazeValue == 14 || mazeValue == 15))
    {
        this->position[1] = y - 1;
        return true;
    }
    return false;
}

/** supprime tous les objets de cette scène */
Scene::~Scene()
{
}

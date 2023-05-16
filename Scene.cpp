#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <utils.h>

#include "Scene.h"

/** constructeur */
Scene::Scene(const std::string& filename):
m_Maze(Maze::import_from_file(filename)),
m_Cube(Scene::make_cube_matrix())
{
    // couleur du fond : bleu
    glClearColor(0.3, 0.3, 0.4, 0);

    // activer le depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // initialiser les matrices
    m_MatP = mat4::create();
    m_MatV = mat4::create();
    m_MatVM = mat4::create();
    m_MatTMP = mat4::create();

    // gestion vue et souris
    m_Azimut = 0;
    m_Elevation = 0;
    m_Distance = 0;
    m_SimplePlayerMode = false;
    m_DebugMode = false;
    m_Center = vec3::create();
    m_Clicked = false;

    // Initialize position in the maze
    m_Position[0] = 0;
    m_Position[1] = 0;
    m_Direction = EAST;

    switch (m_Direction)
    {
        case NORTH:
            m_Azimut = 0;
            break;

        case SOUTH:
            m_Azimut = 180;
            break;

        case EAST:
            m_Azimut = 90;
            break;

        case WEST:
            m_Azimut = 270;
            break;
    }

    // ouverture des fichiers audios à placer dans les buffers
    ALuint walkBuffer = alutCreateBufferFromFile("data/walk.wav");
    ALuint collisionBuffer = alutCreateBufferFromFile("data/wall.wav");
    ALuint noiseBuffer = alutCreateBufferFromFile("data/white_noise.wav");
    if (walkBuffer == AL_NONE || collisionBuffer == AL_NONE || noiseBuffer == AL_NONE)
    {
        std::cerr << "unable to open sound file" << std::endl;
        alGetError();
        throw std::runtime_error("file not found or not readable");
    }

    // Creates walk, collision and ambiant sources (walls' noise)
    alGenSources(1, &m_Walkource);
    alGenSources(1, &m_CollisionSource);
    alGenSources(4, m_Source);

    // Attribue des buffers aux sources
    alSourcei(m_Walkource, AL_BUFFER, walkBuffer);
    alSourcei(m_CollisionSource, AL_BUFFER, collisionBuffer);

    // Diminue le gain des sources du joueur
    alSourcef(m_Walkource, AL_GAIN, 0.5);
    alSourcef(m_CollisionSource, AL_GAIN, 0.2);

    for (int i = 0; i < 4; i++)
    {
        alSourcei(m_Source[i], AL_BUFFER, noiseBuffer);

        // propriétés de la source à l'origine
        alSourcei(m_Source[i], AL_LOOPING, AL_FALSE);

        // définit la distance à partir de laquelle on atténue le son des murs
        alSourcef(m_Source[i], AL_REFERENCE_DISTANCE, 1.0);

        // diminue le gain du son
        alSourcef(m_Source[i], AL_GAIN, 0.5);
    }

    // positionne le joueur au centre
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);

    // on positionne les sources autour de lui à une distance de 1
    alSource3f(m_Source[SOURCE_NORTH], AL_POSITION, 0, 0, -1);
    alSource3f(m_Source[SOURCE_SOUTH], AL_POSITION, 0, 0, 1);
    alSource3f(m_Source[SOURCE_EAST], AL_POSITION, 1, 0, 0);
    alSource3f(m_Source[SOURCE_WEST], AL_POSITION, -1, 0, 0);

    updateListener();

    if (m_SimplePlayerMode)
    {
        updateSound();
    }
}

std::vector<std::vector<Cube*>> Scene::make_cube_matrix() {
    std::vector<std::vector<Cube*>> cube_matrix(m_Maze->m_Height, std::vector<Cube*>(m_Maze->m_Width, nullptr));

    // Create the cube matrix based on maze
    for (size_t row = 0; row < m_Maze->m_Height; row++)
    {
        for (size_t col = 0; col < m_Maze->m_Width; col++)
        {
            cube_matrix[row][col] = new Cube(m_Maze->m_Matrix[row][col]);
        }
    }

    return cube_matrix;
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
    mat4::perspective(m_MatP, Utils::radians(100), (float)width / height, 0.1, 100);
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
        if (stepForward() == true)
        {
            alSourcePlay(m_Walkource);
            vec3::transformMat4(offset, vec3::fromValues(0, 0, 1), m_MatTMP);

            // obtenir la direction relative à la caméra
            vec4 dir = vec4::fromValues(0, 0, 1, 0);
            vec4::transformMat4(dir, dir, m_MatVM);

            if (m_SimplePlayerMode)
            {
                updateSound();
            }
        }
        else
        {
            alSourcePlay(m_CollisionSource);
            std::cout << "Action impossible" << std::endl;
        }
        break;

    case GLFW_KEY_D: // rotation à droite
        m_Azimut += 90;
        rotateRight();
        updateListener();
        if (m_SimplePlayerMode)
        {
            updateSound();
        }
        break;

    case GLFW_KEY_A: // rotation à gauche
        m_Azimut -= 90;
        rotateLeft();
        updateListener();
        if (m_SimplePlayerMode)
        {
            updateSound();
        }
        break;

    case GLFW_KEY_H: // son à gauche
        if (!m_SimplePlayerMode)
        {
            playSoundLeft();
        }
        break;

    case GLFW_KEY_U: // son devant
        if (!m_SimplePlayerMode)
        {
            playSoundFront();
        }
        break;

    case GLFW_KEY_K: // son à droite
        if (!m_SimplePlayerMode)
        {
            playSoundRight();
        }
        break;

    case GLFW_KEY_J: // son des 3 directions à la fois
        if (!m_SimplePlayerMode)
        {
            playSoundLeft();
            playSoundFront();
            playSoundRight();
        }
        break;

    case GLFW_KEY_L: // change le mode de son
        m_SimplePlayerMode = !m_SimplePlayerMode;

        for (int i = 0; i < 4; i++)
        {
            alSourcei(m_Source[i], AL_LOOPING, m_SimplePlayerMode ? AL_TRUE : AL_FALSE);
        }

        if (m_SimplePlayerMode)
        {
            updateSound();
        }
        break;

    case GLFW_KEY_B: // change le mode d'affichage (vide ou plein)
        m_DebugMode = !m_DebugMode;
        break;

    default:
        return;
    }

    // appliquer le décalage au centre de la rotation
    vec3::add(m_Center, m_Center, offset);
}

void Scene::updateListener()
{
    // Set the listener orientation to point towards a cardinal point
    switch (m_Direction)
    {
    case NORTH:
        m_ListenerOrientation = ORIENTATION_NORTH;
        break;

    case SOUTH:
        m_ListenerOrientation = ORIENTATION_SOUTH;
        break;

    case EAST:
        m_ListenerOrientation = ORIENTATION_EAST;
        break;

    case WEST:
        m_ListenerOrientation = ORIENTATION_WEST;
        break;
    }
    alListenerfv(AL_ORIENTATION, m_ListenerOrientation);
}

void Scene::updateSound()
{
    int cellValue = m_Maze->m_Matrix[m_Position[0]][m_Position[1]];

    if ((cellValue & NORTH) == 0 && m_Direction != SOUTH)
    {
        alSourcePlay(m_Source[SOURCE_NORTH]);
    }
    else
    {
        alSourcePause(m_Source[SOURCE_NORTH]);
    }

    if ((cellValue & SOUTH) == 0 && m_Direction != NORTH)
    {
        alSourcePlay(m_Source[SOURCE_SOUTH]);
    }
    else
    {
        alSourcePause(m_Source[SOURCE_SOUTH]);
    }

    if ((cellValue & EAST) == 0 && m_Direction != WEST)
    {
        alSourcePlay(m_Source[SOURCE_EAST]);
    }
    else
    {
        alSourcePause(m_Source[SOURCE_EAST]);
    }

    if ((cellValue & WEST) == 0 && m_Direction != EAST)
    {
        alSourcePlay(m_Source[SOURCE_WEST]);
    }
    else
    {
        alSourcePause(m_Source[SOURCE_WEST]);
    }
}

void Scene::playSoundLeft()
{
    int cellValue = m_Maze->m_Matrix[m_Position[0]][m_Position[1]];

    switch (m_Direction)
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
            alSourcePlay(m_Source[SOURCE_WEST]);
        }
        break;

    case EAST:
        if ((cellValue & NORTH) == 0)
        {
            alSourcePlay(m_Source[SOURCE_WEST]);
        }
        break;

    case WEST:
        if ((cellValue & SOUTH) == 0)
        {
            alSourcePlay(m_Source[SOURCE_WEST]);
        }
        break;
    }
}

void Scene::playSoundFront()
{
    int cellValue = m_Maze->m_Matrix[m_Position[0]][m_Position[1]];

    switch (m_Direction)
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
            alSourcePlay(m_Source[SOURCE_NORTH]);
        }
        break;

    case EAST:
        if ((cellValue & EAST) == 0)
        {
            alSourcePlay(m_Source[SOURCE_NORTH]);
        }
        break;

    case WEST:
        if ((cellValue & WEST) == 0)
        {
            alSourcePlay(m_Source[SOURCE_NORTH]);
        }
        break;
    }
}

void Scene::playSoundRight()
{
    int cellValue = m_Maze->m_Matrix[m_Position[0]][m_Position[1]];

    switch (m_Direction)
    {
    case NORTH:
        if ((cellValue & EAST) == 0)
        {
            alSourcePlay(m_Source[SOURCE_NORTH]);
        }
        break;

    case SOUTH:
        if ((cellValue & WEST) == 0)
        {
            alSourcePlay(m_Source[SOURCE_NORTH]);
        }
        break;

    case EAST:
        if ((cellValue & SOUTH) == 0)
        {
            alSourcePlay(m_Source[SOURCE_NORTH]);
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
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_DebugMode)
    {
        /** préparation des matrices **/

        // positionner la caméra
        mat4::identity(m_MatV);

        // éloignement de la scène
        mat4::translate(m_MatV, m_MatV, vec3::fromValues(0, 0, -m_Distance));

        // rotation demandée par la souris
        mat4::rotateX(m_MatV, m_MatV, Utils::radians(m_Elevation));
        mat4::rotateY(m_MatV, m_MatV, Utils::radians(m_Azimut));

        // centre des rotations
        mat4::translate(m_MatV, m_MatV, m_Center);

        // Dessine le labyrinthe
        for (size_t row = 0; row < m_Maze->m_Height; row++)
        {
            for (size_t col = 0; col < m_Maze->m_Width; col++)
            {
                m_Cube[row][col]->onRender(m_MatP, m_MatV);
                mat4::translate(m_MatV, m_MatV, vec3::fromValues(1, 0, 0));
            }

            mat4::translate(m_MatV, m_MatV, vec3::fromValues(0. - (m_Maze->m_Width), 0, 1));
        }
    }
}

void Scene::rotateLeft()
{
    switch (m_Direction)
    {
    case NORTH:
        m_Direction = WEST;
        break;
    case SOUTH:
        m_Direction = EAST;
        break;
    case EAST:
        m_Direction = NORTH;
        break;
    case WEST:
        m_Direction = SOUTH;
        break;

    default:
        break;
    }
}

void Scene::rotateRight()
{
    switch (m_Direction)
    {
    case NORTH:
        m_Direction = EAST;
        break;
    case SOUTH:
        m_Direction = WEST;
        break;
    case EAST:
        m_Direction = SOUTH;
        break;
    case WEST:
        m_Direction = NORTH;
        break;

    default:
        break;
    }
}

bool Scene::stepForward()
{
    int x = m_Position[0];
    int y = m_Position[1];

    int cellValue = m_Maze->m_Matrix[x][y];
    bool canMove = false;

    if (m_Direction == NORTH && (cellValue & NORTH) != 0)
    {
        m_Position[0] = x - 1;
        canMove = true;
    }
    if (m_Direction == SOUTH && (cellValue & SOUTH) != 0)
    {
        m_Position[0] = x + 1;
        canMove = true;
    }
    if (m_Direction == EAST && (cellValue & EAST) != 0)
    {
        m_Position[1] = y + 1;
        canMove = true;
    }
    if (m_Direction == WEST && (cellValue & WEST) != 0)
    {
        m_Position[1] = y - 1;
        canMove = true;
    }

    if (m_Position[0] == (m_Maze->m_Height - 1) && m_Position[1] == (m_Maze->m_Width - 1))
    {
        std::cout << std::endl;
        std::cout << "Bravo, vous avez trouvé la sortie !" << std::endl;
        exit(0);
    }

    return canMove;
}

/** supprime tous les objets de cette scène */
Scene::~Scene()
{
}

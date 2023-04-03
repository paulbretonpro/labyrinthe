// Définition de la classe Duck

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <utils.h>

#include <Duck.h>
#include <Light.h>
#include <Texture2D.h>

using namespace mesh;


/** constructeur */
Duck::Duck(): Mesh("Duck")
{
    // matériaux
    m_Material = new MaterialTexture("data/10602_Rubber_Duck_v1_diffuse.jpg");
    setMaterials(m_Material);

    // charger le fichier obj
    loadObj("data/10602_Rubber_Duck_v1_L3.obj");

    // mise à l'échelle et rotation du canard (son .obj est mal orienté et trop grand)
    mat4 correction = mat4::create();
    mat4::identity(correction);
    mat4::scale(correction, correction, vec3::fromValues(0.15, 0.15, 0.15));
    mat4::rotateX(correction, correction, Utils::radians(-90));
    transform(correction);

    // recalcul des normales
    computeNormals();

    // ouverture du flux audio à placer dans le buffer
    std::string soundpathname = "data/Duck-quacking-sound.wav";
    buffer = alutCreateBufferFromFile(soundpathname.c_str());
    if (buffer == AL_NONE) {
        std::cerr << "unable to open file " << soundpathname << std::endl;
        alGetError();
        throw std::runtime_error("file not found or not readable");
    }

    // lien buffer -> source
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);

    // propriétés de la source à l'origine
    alSource3f(source, AL_POSITION, 0, 0, 0); // on positionne la source à (0,0,0) par défaut
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_TRUE);
    // dans un cone d'angle [-inner/2,inner/2] il n'y a pas d'attenuation
    alSourcef(source, AL_CONE_INNER_ANGLE, 80);
    // dans un cone d'angle [-outer/2,outer/2] il y a une attenuation linéaire entre 0 et le gain
    alSourcef(source, AL_CONE_OUTER_GAIN, 0);
    alSourcef(source, AL_CONE_OUTER_ANGLE, 120);
    // à l'extérieur de [-outer/2,outer/2] il y a une attenuation totale

    alSourcePlay(source);
}


/**
 * définit la lampe
 * @param light : instance de Light spécifiant les caractéristiques de la lampe
 */
void Duck::setLight(Light* light)
{
    m_Material->setLight(light);
}


/**
     * dessiner le cube
     * @param matP : matrice de projection
     * @param matMV : matrice view*model (caméra * position objet)
 */
void Duck::onRender(const mat4& matP, const mat4& matVM)
{
    /** dessin OpenGL **/

    onDraw(matP, matVM);

    /** sonorisation OpenAL **/

    // obtenir la position relative à la caméra
    vec4 pos = vec4::fromValues(0,0,0,1);   // point en (0,0,0)
    vec4::transformMat4(pos, pos, matVM);
    //std::cout << "Position = " << vec4::str(pos);
    alSource3f(source, AL_POSITION, pos[0], pos[1], pos[2]);

    // obtenir la direction relative à la caméra
    vec4 dir = vec4::fromValues(0,0,1,0);   // vecteur +z
    vec4::transformMat4(dir, dir, matVM);
    //std::cout << "    Direction = " << vec4::str(dir) << std::endl;
    alSource3f(source, AL_DIRECTION, dir[0], dir[1], dir[2]);
}



vec3& Duck::getPosition()
{
    return m_Position;
}

void Duck::setPosition(vec3 pos)
{
    vec3::copy(m_Position, pos);
}



/** destructeur */
Duck::~Duck()
{
    // libération du matériau
    delete m_Material;

    // libération des ressources openal
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
}

// Définition de la classe Cube

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <utils.h>

#include <Cube.h>
#include <Material.h>

using namespace mesh;



/** matériau pour le cube **/
class MaterialCube: public Material
{
public:

    MaterialCube() : Material("MatCube")
    {
        // vertex shader
        std::string srcVertexShader =
                "#version 300 es\n"
                "uniform mat4 matP;\n"
                "uniform mat4 matVM;\n"
                "in vec3 glVertex;\n"
                "in vec3 glColor;\n"
                "out vec3 frgColor;\n"
                "void main()\n"
                "{\n"
                "    gl_Position = matP * matVM * vec4(glVertex, 1.0);\n"
                "    frgColor = glColor;\n"
                "}";

        // fragment shader
        std::string srcFragmentShader =
                "#version 300 es\n"
                "precision mediump float;\n"
                "in vec3 frgColor;\n"
                "out vec4 glFragColor;\n"
                "void main()\n"
                "{\n"
                "    glFragColor = vec4(frgColor, 1.0);\n"
                "}";

        setShaders(srcVertexShader, srcFragmentShader);
    }
};





/** constructeur */
Cube::Cube(std::string soundpathname): Mesh("cube")
{
    // matériau
    m_Material = new MaterialCube();
    setMaterials(m_Material);

    // maillage
    const float b = 0.5;

    // sommets
    /*
          p3  ____    p0
        p2  /|___/| p1
         p7 |/___|/   p4
        p6          p5
    */
    Vertex* P0 = new Vertex(this, +b, +b, +b);
    Vertex* P1 = new Vertex(this, +b, +b, -b);
    Vertex* P2 = new Vertex(this, -b, +b, -b);
    Vertex* P3 = new Vertex(this, -b, +b, +b);
    Vertex* P4 = new Vertex(this, +b, -b, +b);
    Vertex* P5 = new Vertex(this, +b, -b, -b);
    Vertex* P6 = new Vertex(this, -b, -b, -b);
    Vertex* P7 = new Vertex(this, -b, -b, +b);

    // couleurs
    P0->setColor(0.0, 0.5, 1.0);  // P0 bleu
    P1->setColor(1.0, 0.5, 0.0);  // P1 orange
    P2->setColor(0.0, 0.7, 0.0);  // P2 vert foncé
    P3->setColor(0.7, 0.0, 0.7);  // P3 violet
    P4->setColor(0.0, 1.0, 1.0);  // P4 cyan
    P5->setColor(0.5, 1.0, 0.0);  // P7 vert clair
    P6->setColor(0.5, 0.0, 0.5);  // P5 magenta
    P7->setColor(1.0, 1.0, 0.0);  // P6 jaune

    // quads
    addQuad(P1,P0,P3,P2);
    addQuad(P4,P5,P6,P7);
    addQuad(P5,P4,P0,P1);
    addQuad(P4,P7,P3,P0);
    addQuad(P7,P6,P2,P3);
    addQuad(P6,P5,P1,P2);

    // ouverture du flux audio à placer dans le buffer
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
    alSourcef(source, AL_CONE_INNER_ANGLE, 20);
    // dans un cone d'angle [-outer/2,outer/2] il y a une attenuation linéaire entre 0 et le gain
    alSourcef(source, AL_CONE_OUTER_GAIN, 0);
    alSourcef(source, AL_CONE_OUTER_ANGLE, 80);
    // à l'extérieur de [-outer/2,outer/2] il y a une attenuation totale

    alSourcePlay(source);
}


/**
     * dessiner le cube
     * @param matP : matrice de projection
     * @param matMV : matrice view*model (caméra * position objet)
 */
void Cube::onRender(const mat4& matP, const mat4& matVM)
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



vec3& Cube::getPosition()
{
    return m_Position;
}

void Cube::setPosition(vec3 pos)
{
    vec3::copy(m_Position, pos);
}



/** destructeur */
Cube::~Cube()
{
    // libération du matériau
    delete m_Material;

    // libération des ressources openal
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
}

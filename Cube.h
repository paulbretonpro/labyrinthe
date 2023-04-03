#ifndef CUBE_H
#define CUBE_H

// Définition de la classe Cube

#include <Mesh.h>
#include <gl-matrix.h>

class Cube: public Mesh
{
private:

    /** matériau */
    Material* m_Material;

    /** buffers pour la gestion du son */
    ALuint buffer, source;

    /** position 3D du cube */
    vec3 m_Position;


public:

    /** constructeur, crée le VBO et le shader */
    Cube(std::string soundpathname);

    /** destructeur, libère le VBO et le shader */
    ~Cube();

    /**
     * dessiner le cube
     * @param matP : matrice de projection
     * @param matMV : matrice view*model (caméra * position objet)
     */
    void onRender(const mat4& matP, const mat4& matMV);

    /**
     * retourne la position % scèce du cube
     * @return vec3 position
     */
    vec3& getPosition();

    /**
     * affecte la position % scène du cube
     * @param vec3 pos position
     */
    void setPosition(vec3 pos);
};

#endif

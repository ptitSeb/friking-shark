
class CEntityPartJoint
{
    CVector vPoint;     // En el sistema de referencia del padre
    CVector vAxises[3]; // En el sistema de referencia del padre
};

class CEntityAnimation
{
    CAnimation              *pAnimation;
    CEntityAnimationObjects *pObjects; // fijos, especificados por el 
    CEntityAnimationEffect  *pEffects; // dinamicos ,introducidos desde el editor
};

class CEntityPart
{
    CEntityAnimation *pAnimations;
    CEntityPartJoint joint;
};

class CEntityType
{
    CEntityPart *pRootParts;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Especificacion de tipos de unidades
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

las unidades tienen partes, que se organizan en forma de arbol 
cada una de esas partes se enlaza....
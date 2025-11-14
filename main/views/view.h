#ifndef VIEW_H
#define VIEW_H

#include "../lgfx_custom.h"

class View
{
public:
    virtual ~View() = default;
    virtual void render(LGFX &display, LGFX_Sprite &spr) = 0;

    // Méthode virtuelle pour gérer les touches (optionnelle)
    // Retourne true si la vue a consommé le touch (ne pas changer de vue)
    virtual bool handleTouch(int x, int y) { return false; }
};

#endif // VIEW_H

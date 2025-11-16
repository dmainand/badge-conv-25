#ifndef VIEW_PROGRAM_H
#define VIEW_PROGRAM_H

#include "view.h"
#include "../state.h"
#include "../lgfx_custom.h"

class ViewProgram : public View
{
public:
    ViewProgram(AppState &state, LGFX &lcd);
    void render(LGFX &display, LGFX_Sprite &spr) override;

private:
    AppState &m_state;
    LGFX &m_lcd;

    void renderProgramItem(LGFX_Sprite &spr, const char *time, const char *title, int y, uint16_t color, bool isLast = false);
};

#endif // VIEW_PROGRAM_H

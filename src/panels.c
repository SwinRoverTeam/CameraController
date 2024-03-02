#include "panels.h"
#include "logging.h"

#include <stdio.h>

void panel_debug(panel_t* p)
{
	LOG_DEBUG("x:%i, y:%i, width:%i, height:%i\n", p->x, p->y, p->w, p->h);
}

void stack_top(panel_t* base, panel_t* new_panel)
{
	// create child panel in same position
	new_panel->x = base->x;
	new_panel->y = base->y;

	// shrink the base panel
	base->h = base->h / 2;
	base->y = base->y + base->h / 2;

	// resize child panel
	new_panel->w = base->w;
	new_panel->h = base->h;
}

void stack_bottom(panel_t* base, panel_t* new_panel)
{
	// create child panel overlapping base
	new_panel->x = base->x;
	new_panel->y = base->y + (base->h / 2);

	// shrink the base panel
	base->h = base->h / 2;
	base->y = base->y - base->h;
}

void stack_left(panel_t* base, panel_t* new_panel)
{
}

void stack_right(panel_t* base, panel_t* new_panel)
{
}

// test the panel stacking
int main()
{
	panel_t root;

	root.x = root.y = 0;
	root.w = 1920;
	root.h = 1080;

	panel_debug(&root);

	// push a panel ontop
	
	panel_t new;
	stack_top(&root, &new);

	puts("root: ");
	panel_debug(&root);
	puts("new: ");
	panel_debug(&new);

	return 0;
}

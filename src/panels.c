#include "panels.h"
#include "logging.h"

#include <stdio.h>

void panel_debug(panel_t* p)
{
	printf("x:%i, y:%i, width:%i, height:%i\n", p->x, p->y, p->w, p->h);
}

void stack_top(panel_t* base, panel_t* new_panel)
{
	new_panel->x = base->x;
	new_panel->y = base->y;

	base->h = base->h / 2;
	base->y = base->y + base->h / 2;

	new_panel->w = base->w;
	new_panel->h = base->h;
}

void stack_bottom(panel_t* base, panel_t* new_panel)
{
	new_panel->x = base->x;
	new_panel->y = base->y + (base->h / 2);

	base->h = base->h / 2;
	base->y = base->y - base->h;

	new_panel->w = base->w;
	new_panel->h = base->h;
}

void stack_left(panel_t* base, panel_t* new_panel)
{
	new_panel->x = base->x;
	new_panel->y = base->y;

	base->w = base->w / 2;
	base->x = base->x + base->w;

	new_panel->w = base->w;
	new_panel->h = base->h;
}

void stack_right(panel_t* base, panel_t* new_panel)
{
	new_panel->x = base->x + (base->w / 2);
	new_panel->y = base->y;

	base->w = base->w / 2;
	
	new_panel->w = base->w;
	new_panel->h = base->h;
}
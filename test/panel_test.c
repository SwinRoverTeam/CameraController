#include "../src/panels.h"

#include <stdio.h>

// compile with:
// gcc test/panel_test.c src/panels.c

// test the panel stacking
int main(int argc, char** argv)
{
	panel_t root;

	root.x = root.y = 0;
	root.w = 1920;
	root.h = 1080;

	panel_debug(&root);

	// push a panel ontop
        printf("stacking top\n");
	panel_t new;
	stack_top(&root, &new);
	printf("root: ");
	panel_debug(&root);
	printf("new: ");
	panel_debug(&new);

        // push a panel from left
        printf("stacking left\n");
        panel_t left;
        stack_left(&new, &left);
        printf("new: ");
        panel_debug(&new);
        printf("left: ");
        panel_debug(&left);

	return 0;
}
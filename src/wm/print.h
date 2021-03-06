void print_insertfill(struct ring *ring, unsigned int source, struct element_fill *fill, unsigned int z);
void print_insertmouse(struct ring *ring, unsigned int source, struct element_mouse *mouse, unsigned int z);
void print_insertpanel(struct ring *ring, unsigned int source, struct element_panel *panel, unsigned int z);
void print_inserttext(struct ring *ring, unsigned int source, struct element_text *text, unsigned int z, unsigned int count);
void print_insertwindow(struct ring *ring, unsigned int source, struct element_window *window, unsigned int z);
void print_removefill(struct ring *ring, unsigned int source, struct element_fill *fill, unsigned int z);
void print_removemouse(struct ring *ring, unsigned int source, struct element_mouse *mouse, unsigned int z);
void print_removepanel(struct ring *ring, unsigned int source, struct element_panel *panel, unsigned int z);
void print_removetext(struct ring *ring, unsigned int source, struct element_text *text, unsigned int z);
void print_removewindow(struct ring *ring, unsigned int source, struct element_window *window, unsigned int z);

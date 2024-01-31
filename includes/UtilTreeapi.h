
#ifndef UTILTREEAPI_H
#define UTILTREEAPI_H

int dui_tree_node_add (
char       *device,
char       *component,
char       *name,
wchar_t    *label,
char       *parent,
char       *type,
...
);

int dui_tree_node_add_before (
char       *device,
char       *component,
char       *name,
wchar_t    *label,
char       *parent,
char       *type,
char       *before,
...
);

int dui_tree_node_modify (
char   *device,
char   *component,
char   *name,
...
);

int dui_tree_node_inquire (
char   *device,
char   *component,
char   *name,
...
);

int dui_tree_create_attr_col (
char   *device,
char   *component,
char   *name,
...
);

int dui_tree_modify_attr_col (
char   *device,
char   *component,
char   *name,
...
);

int dui_tree_inquire_attr_col (
char   *device,
char   *component,
char   *name,
...
);



#endif

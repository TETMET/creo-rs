#ifndef PDPROTOH
#define PDPROTOH


extern void user_terminate();

extern int promenu_create (
                        char    *menu_title,
                        char    *menu_file
                        );

extern int promenu_expand (
                        char    *menu_title,
                        char    *menu_file
                        );

extern int promenu_on_button (
                        char            *menu_title,
                        char            *button_name,
                        int             (*function)(),
                        void            *arg1,
                        int             arg2
                        );

extern int promenu_make (
                        int     type,
                        char    *menu_title
                        );

extern int promenu_action (
                        char    *root_menu
                        );

extern int promenu_exit_up ();

extern int promenu_no_exit ();
extern int promenu_set_mode (
                        char    *menu_title,
                        int     menu_mode
                        );

extern int promenu_set_data_mode (
                        char    *menu_title,
                        int     checkmark_highlight
                        );

extern int promenu_set_item (
                        char    *menu_title,
                        char    *item_name
                        );

extern int promenu_make_item_accessible (
                        char    *menu_title,
                        char    *item_name
                        );

extern int promenu_make_item_inaccessible (
                        char    *menu_title,
                        char    *item_name
                        );

extern int pro_select_strings (
                        wchar_t *menu_title,
                        wchar_t *string_list[],
                        int     max_count,
                        wchar_t *help_list[],
                        int     scroll_size,
#ifdef HP_aCC
                        wchar_t **selected_strings[]
#else
                        wchar_t *(*selected_strings[])
#endif
                        );

extern void promsg_print (
                        wchar_t *msg_fil,
                        char    *msg_title,
                        void *a0,
                        void *a1,
                        void *a2,
                        void *a3,
                        void *a4,
                        void *a5,
                        void *a6,
                        void *a7,
                        void *a8,
                        void *a9
                        );

extern int promsg_getstring (
                        wchar_t *buffer,
                        int     maxlen
                        );

extern int promsg_getint (
                        int     *answer,
                        int     range[2]
                        );

extern int promsg_getdouble (
                        double  *answer,
                        double  range[2]
                        );

extern int progr_color (
                        int     new_color
                        );

#endif

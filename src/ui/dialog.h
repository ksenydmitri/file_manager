#ifndef DIALOG_H
#define DIALOG_H

#include "../../include/types.h"
#include "../../include/enums.h"

DialogResult show_dialog(DialogType type, const char* title, const char* message);

void show_create_object_dialog(ApplicationState* state);
void show_search_dialog(ApplicationState* state);
void show_file_dialog(ApplicationState* state, const char* path, FileEntry* entry);
void show_error_dialog( const char* message);
void show_search_result_dialog(ApplicationState* state, FileSearchResult* results);
void show_rename_dialog(ApplicationState* state, FileEntry* entry);
void show_change_permissions_dialog(ApplicationState* state, FileEntry* entry);
void show_file_contents_dialog(const char* filepath);
int show_change_owner_dialog(ApplicationState* state, FileEntry* entry);
void show_file_entry_dialog(ApplicationState *state, FileEntry* file_entry);
void show_system_stat_dialog(ApplicationState *state);
#endif
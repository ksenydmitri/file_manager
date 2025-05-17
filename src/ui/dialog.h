#ifndef DIALOG_H
#define DIALOG_H

#include "../../include/types.h"
#include "../../include/enums.h"

DialogResult show_dialog(DialogType type, const char* title, const char* message);

void show_create_object_dialog(ApplicationState* state);
void show_search_dialog(const ApplicationState* state);
void show_file_dialog(const char* path,const FileEntry* entry);
void show_error_dialog( const char* message);
void show_search_result_dialog(const FileSearchResult* results);
void show_rename_dialog(ApplicationState* state,const FileEntry* entry);
void show_change_permissions_dialog(ApplicationState* state,const FileEntry* entry);
void show_file_contents_dialog(const char* filepath);
int show_change_owner_dialog(ApplicationState* state, FileEntry* entry);
void show_file_entry_dialog(const FileEntry* file_entry);
void show_system_stat_dialog(const ApplicationState *state);
#endif
#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "../../include/types.h"

void copy_to_clipboard(Clipboard *cb, const char *path, OperationType type);
int paste_from_clipboard(Clipboard *cb, const char *dest);
void clear_clipboard(Clipboard *cb);

#endif
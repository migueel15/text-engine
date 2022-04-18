/* editor.h
 *
 * Copyright 2022 Matthew Jakeman <mjakeman26@outlook.co.nz>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include <glib-object.h>

#include "../model/document.h"

G_BEGIN_DECLS

#define TEXT_TYPE_EDITOR (text_editor_get_type())

G_DECLARE_FINAL_TYPE (TextEditor, text_editor, TEXT, EDITOR, GObject)

TextEditor *text_editor_new        (TextDocument *document);

void        text_editor_move_first (TextEditor *self, TextMark *mark);
void        text_editor_move_last  (TextEditor *self, TextMark *mark);
void        text_editor_move_right (TextEditor *self, TextMark *mark, int amount);
void        text_editor_move_left  (TextEditor *self, TextMark *mark, int amount);
void        text_editor_insert     (TextEditor *self, TextMark *start, gchar *str);
void        text_editor_delete     (TextEditor *self, TextMark *start, int length);
void        text_editor_replace    (TextEditor *self, TextMark *start, TextMark *end, gchar *text);

G_END_DECLS

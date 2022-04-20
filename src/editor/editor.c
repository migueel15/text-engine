/* editor.c
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

#include "editor.h"

#include "../model/paragraph.h"

struct _TextEditor
{
    GObject parent_instance;

    TextDocument *document;
};

G_DEFINE_FINAL_TYPE (TextEditor, text_editor, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_DOCUMENT,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

TextEditor *
text_editor_new (TextDocument *document)
{
    return g_object_new (TEXT_TYPE_EDITOR,
                         "document", document,
                         NULL);
}

static void
text_editor_finalize (GObject *object)
{
    TextEditor *self = (TextEditor *)object;

    G_OBJECT_CLASS (text_editor_parent_class)->finalize (object);
}

static void
text_editor_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    TextEditor *self = TEXT_EDITOR (object);

    switch (prop_id)
    {
    case PROP_DOCUMENT:
        g_value_set_object (value, self->document);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
text_editor_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    TextEditor *self = TEXT_EDITOR (object);

    switch (prop_id)
    {
    case PROP_DOCUMENT:
        self->document = g_value_get_object (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
text_editor_class_init (TextEditorClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = text_editor_finalize;
    object_class->get_property = text_editor_get_property;
    object_class->set_property = text_editor_set_property;

    properties [PROP_DOCUMENT]
        = g_param_spec_object ("document",
                               "Document",
                               "Document",
                               TEXT_TYPE_DOCUMENT,
                               G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static TextItem *
go_up (TextItem *item,
       gboolean  forwards)
{
    TextNode *parent;
    TextNode *sibling;

    g_return_val_if_fail (TEXT_IS_ITEM (item), NULL);

    printf ("item: %s\n", g_type_name_from_instance ((GTypeInstance *) item));

    parent = text_node_get_parent (TEXT_NODE (item));

    printf ("parent: %s\n", g_type_name_from_instance ((GTypeInstance *) parent));

    if (parent && TEXT_IS_ITEM (parent))
    {
        sibling = forwards
            ? text_node_get_next (parent)
            : text_node_get_previous (parent);

        printf ("sibling: %s\n", g_type_name_from_instance ((GTypeInstance *) sibling));

        if (sibling && TEXT_IS_ITEM (sibling))
        {
            printf("sibling\n");
            return TEXT_ITEM (sibling);
        }
        else
        {
            printf("recurse\n");
            return go_up (TEXT_ITEM (parent), forwards);
        }
    }

    return NULL;
}

static TextRun *
walk_until_previous_run (TextItem *item)
{
    TextNode *child;
    TextNode *sibling;
    TextItem *parent;

    g_return_val_if_fail (TEXT_IS_ITEM (item), NULL);

    child = text_node_get_last_child (TEXT_NODE (item));

    if (child && TEXT_IS_ITEM (child)) {
        if (TEXT_IS_RUN (child)) {
            return TEXT_RUN (child);
        }

        return walk_until_previous_run (TEXT_ITEM (child));
    }

    sibling = text_node_get_previous (TEXT_NODE (item));

    if (sibling && TEXT_IS_ITEM (sibling)) {
        if (TEXT_IS_RUN (sibling)) {
            return TEXT_RUN (sibling);
        }

        return walk_until_previous_run (TEXT_ITEM (sibling));
    }

    parent = go_up (item, FALSE);

    if (parent) {
        if (TEXT_IS_RUN (parent)) {
            return TEXT_RUN (parent);
        }

        return walk_until_previous_run (parent);
    }

    return NULL;
}

static TextRun *
walk_until_next_run (TextItem *item)
{
    TextNode *child;
    TextNode *sibling;
    TextItem *parent;

    g_return_val_if_fail (TEXT_IS_ITEM (item), NULL);

    child = text_node_get_first_child (TEXT_NODE (item));

    if (child && TEXT_IS_ITEM (child)) {
        if (TEXT_IS_RUN (child)) {
            return TEXT_RUN (child);
        }

        return walk_until_next_run (TEXT_ITEM (child));
    }

    sibling = text_node_get_next (TEXT_NODE (item));

    if (sibling && TEXT_IS_ITEM (sibling)) {
        if (TEXT_IS_RUN (sibling)) {
            return TEXT_RUN (sibling);
        }

        return walk_until_next_run (TEXT_ITEM (sibling));
    }

    parent = go_up (item, TRUE);

    if (parent) {
        if (TEXT_IS_RUN (parent)) {
            return TEXT_RUN (parent);
        }

        return walk_until_next_run (parent);
    }

    return NULL;
}

static TextParagraph *
walk_until_previous_paragraph (TextItem *item)
{
    TextNode *child;
    TextNode *sibling;
    TextItem *parent;

    g_return_val_if_fail (TEXT_IS_ITEM (item), NULL);

    child = text_node_get_last_child (TEXT_NODE (item));

    if (child && TEXT_IS_ITEM (child)) {
        if (TEXT_IS_PARAGRAPH (child)) {
            return TEXT_PARAGRAPH (child);
        }

        return walk_until_previous_paragraph (TEXT_ITEM (child));
    }

    sibling = text_node_get_previous (TEXT_NODE (item));

    if (sibling && TEXT_IS_ITEM (sibling)) {
        if (TEXT_IS_PARAGRAPH (sibling)) {
            return TEXT_PARAGRAPH (sibling);
        }

        return walk_until_previous_paragraph (TEXT_ITEM (sibling));
    }

    parent = go_up (item, FALSE);

    if (parent) {
        if (TEXT_IS_PARAGRAPH (parent)) {
            return TEXT_PARAGRAPH (parent);
        }

        return walk_until_previous_paragraph (parent);
    }

    return NULL;
}

static TextParagraph *
walk_until_next_paragraph (TextItem *item)
{
    TextNode *child;
    TextNode *sibling;
    TextItem *parent;

    g_return_val_if_fail (TEXT_IS_ITEM (item), NULL);

    child = text_node_get_first_child (TEXT_NODE (item));

    if (child && TEXT_IS_ITEM (child)) {
        if (TEXT_IS_PARAGRAPH (child)) {
            return TEXT_PARAGRAPH (child);
        }

        return walk_until_next_paragraph (TEXT_ITEM (child));
    }

    sibling = text_node_get_next (TEXT_NODE (item));

    if (sibling && TEXT_IS_ITEM (sibling)) {
        if (TEXT_IS_PARAGRAPH (sibling)) {
            return TEXT_PARAGRAPH (sibling);
        }

        return walk_until_next_paragraph (TEXT_ITEM (sibling));
    }

    parent = go_up (item, TRUE);

    if (parent) {
        if (TEXT_IS_PARAGRAPH (parent)) {
            return TEXT_PARAGRAPH (parent);
        }

        return walk_until_next_paragraph (parent);
    }

    return NULL;
}

TextRun *
text_editor_get_run_at_mark (TextEditor *self,
                             TextMark   *mark)
{
    g_return_val_if_fail (TEXT_IS_EDITOR (self), NULL);
    g_return_val_if_fail (mark != NULL, NULL);

    return text_paragraph_get_run_at_index (mark->paragraph,
                                            mark->index,
                                            NULL);
}

void
text_editor_move_mark_left (TextEditor *self,
                            TextMark   *mark,
                            int         amount)
{
    TextParagraph *iter;
    int amount_moved;

    g_return_if_fail (TEXT_IS_EDITOR (self));
    g_return_if_fail (TEXT_IS_DOCUMENT (self->document));
    g_return_if_fail (amount >= 0);

    if (amount == 0)
        return;

    iter = mark->paragraph;
    amount_moved = 0;

    // Simple case: The movement is contained entirely
    // within the current paragraph.
    if (mark->index - amount >= 0)
    {
        mark->index -= amount;
        return;
    }

    // Crossing one or more paragraphs
    amount_moved += mark->index;

    while (amount_moved < amount)
    {
        int num_indices;

        // Go to previous paragraph
        iter = walk_until_previous_paragraph (TEXT_ITEM (iter));

        // Check if NULL (e.g. start of document)
        if (!TEXT_IS_ITEM (iter))
            break;

        // Num of indices is the paragraph length
        // plus the final index
        num_indices = text_paragraph_get_length (iter) + 1;

        // Paragraph is entirely contained within amount to move
        if (amount_moved + num_indices < amount)
        {
            // We are now at index 0 of the previous paragraph -> repeat
            amount_moved += num_indices;
            continue;
        }

        // Move partially through the paragraph by
        // the amount to move remaining
        mark->index = num_indices - (amount - amount_moved);
        mark->paragraph = iter;
        return;
    }

    // Reached start of document
    text_editor_move_mark_first (self, mark);
}

void
text_editor_move_mark_right (TextEditor *self,
                             TextMark   *mark,
                             int         amount)
{
    TextParagraph *iter;
    int amount_moved;
    int last_index;

    g_return_if_fail (TEXT_IS_EDITOR (self));
    g_return_if_fail (TEXT_IS_DOCUMENT (self->document));
    g_return_if_fail (amount >= 0);

    if (amount == 0)
        return;

    iter = mark->paragraph;
    amount_moved = 0;

    // Simple case: The movement is contained entirely
    // within the current paragraph.
    last_index = text_paragraph_get_length (iter);
    if (mark->index + amount <= last_index)
    {
        mark->index += amount;
        return;
    }

    // Crossing one or more paragraphs
    amount_moved += (last_index - mark->index);

    while (amount_moved < amount)
    {
        int num_indices;

        // Go to next paragraph
        iter = walk_until_next_paragraph (TEXT_ITEM (iter));

        // Check if NULL (end of document)
        if (!TEXT_IS_ITEM (iter))
            break;

        // Num of indices is the paragraph length
        // plus the final index
        num_indices = text_paragraph_get_length (iter) + 1;

        // Paragraph is entirely contained within amount to move
        if (amount_moved + num_indices < amount)
        {
            // We are now at the last index of the next paragraph -> repeat
            amount_moved += num_indices;
            continue;
        }

        // Move partially through the paragraph by
        // the amount to move remaining
        mark->index = (amount - amount_moved) - 1;
        mark->paragraph = iter;
        return;
    }

    // Reached end of document
    text_editor_move_mark_last (self, mark);
}

void
text_editor_move_mark_first (TextEditor *self,
                             TextMark   *mark)
{
    g_return_if_fail (TEXT_IS_EDITOR (self));
    g_return_if_fail (TEXT_IS_DOCUMENT (self->document));

    mark->paragraph = walk_until_next_paragraph (TEXT_ITEM (self->document->frame));
    mark->index = 0;
}

void
text_editor_move_mark_last (TextEditor *self,
                            TextMark   *mark)
{
    g_return_if_fail (TEXT_IS_EDITOR (self));
    g_return_if_fail (TEXT_IS_DOCUMENT (self->document));

    mark->paragraph = walk_until_previous_paragraph (TEXT_ITEM (self->document->frame));
    mark->index = 0;

    if (mark->paragraph)
    {
        mark->index = text_paragraph_get_length (mark->paragraph);
    }
}

static void
_ensure_paragraph (TextEditor *self)
{
    TextFrame *document_frame;
    TextParagraph *paragraph;

    document_frame = self->document->frame;

    // Add paragraph with run if none exists
    if (!walk_until_next_run (TEXT_ITEM (document_frame)))
    {
        paragraph = text_paragraph_new ();
        text_frame_append_block (document_frame, TEXT_BLOCK (paragraph));
        text_paragraph_append_run (paragraph, text_run_new (""));
    }
}

void
_delete_run (TextEditor *self,
             TextRun    *run)
{
    TextNode *parent;

    parent = text_node_get_parent (TEXT_NODE (run));

    if (TEXT_IS_PARAGRAPH (parent) &&
        text_node_get_num_children (parent) == 1)
    {
        // Delete paragraph if we are the last run
        text_node_delete (&parent);

        // Ensure there is a paragraph remaining
        _ensure_paragraph (self);

        return;
    }

    // Delete run
    parent = TEXT_NODE (run);
    text_node_delete (&parent);

    return;
}

static void
_erase_text (TextRun *run,
             int      index,
             int      length)
{
    GString *modified;
    char *text;

    g_object_get (run, "text", &text, NULL);

    // Assumes index and length are within range
    modified = g_string_new (text);
    modified = g_string_erase (modified, index, length);
    g_object_set (run, "text", modified->str, NULL);
    g_string_free (modified, TRUE);
}

void
text_editor_delete_at_mark (TextEditor *self,
                            TextMark   *start,
                            int         length)
{
    // TODO: Rework to use an Iter instead of a Mark
    // TODO: Update all marks associated with a document

    TextMark *cursor;
    TextRun *run;
    TextRun *iter;
    int run_length;
    int cur_deleted;
    int run_start_index;
    int run_offset;

    g_return_if_fail (TEXT_IS_EDITOR (self));
    g_return_if_fail (TEXT_IS_DOCUMENT (self->document));
    g_return_if_fail (TEXT_IS_PARAGRAPH (start->paragraph));
    g_return_if_fail (start != NULL);

    if (length < 0)
    {
        text_editor_move_mark_left (self, self->document->cursor, -length);
        text_editor_delete_at_mark (self, start, -length);
        return;
    }

    run = text_paragraph_get_run_at_index (start->paragraph, start->index, &run_start_index);
    run_offset = start->index - run_start_index;

    run_length = text_run_get_length (run);
    cursor = self->document->cursor;

    // Case 1: Deleting a single run
    if (run_offset == 0 &&
        length == run_length)
    {
        // TODO: Broken!
        cursor->index = run_start_index;
        _delete_run (self, run);
        return;
    }

    // Case 2: Deletion is contained within current run
    if (start->index + length <= text_run_get_length (run))
    {
        _erase_text (run, start->index, length);
        return;
    }

    // Case 3: Deletion spans multiple runs
    cur_deleted = 0;
    iter = run;
    gboolean first = TRUE;

    while (cur_deleted < length)
    {
        int delta_length;

        g_assert (iter != NULL);

        delta_length = text_run_get_length (iter);

        // Handle first element
        if (first)
        {
            _erase_text (iter, start->index, delta_length - start->index);
            iter = walk_until_next_run (TEXT_ITEM (iter));
            first = FALSE;
            continue;
        }

        // Check if the run is entirely contained within the deletion
        if (cur_deleted + delta_length < length)
        {
            TextRun *next;

            next = walk_until_next_run (TEXT_ITEM (iter));
            _delete_run (self, iter);
            cur_deleted += delta_length;
            iter = next;
            continue;
        }

        // Handle last element
        _erase_text (iter, 0, length - cur_deleted);

        return;
    }
}

int
_length_between_marks (TextMark *start,
                       TextMark *end)
{
    TextParagraph *iter;
    int length;

    // TODO: Find a way to determine whether start is before end?

    if (start->paragraph == end->paragraph)
        return end->index - start->index;

    iter = start->paragraph;
    length = text_paragraph_get_length (iter) - start->index;

    while ((iter = walk_until_next_paragraph (TEXT_ITEM (iter))) != NULL)
    {
        if (iter == end->paragraph)
        {
            length += end->index;
            break;
        }

        length += text_paragraph_get_length (iter);
    }

    return length;
}

void
text_editor_replace_at_mark (TextEditor *self,
                             TextMark   *start,
                             TextMark   *end,
                             gchar      *text)
{
    // TODO: Find a way to determine whether start is before end?

    int length;

    g_return_if_fail (TEXT_IS_PARAGRAPH (start->paragraph));
    g_return_if_fail (TEXT_IS_PARAGRAPH (end->paragraph));

    length = _length_between_marks (start, end);
    g_print ("length %d\n", length);
    text_editor_delete_at_mark (self, start, length);
    text_editor_insert_at_mark (self, start, text);
}

void
text_editor_insert_at_mark (TextEditor *self,
                            TextMark   *start,
                            gchar      *str)
{
    // Encapsulates insertion inside an editor module/object.
    // This should accept user input in the form of Operational
    // Transformation commands. This will aid with undo/redo.

    char *text;
    GString *modified;
    TextRun *run;
    int run_start_index;
    int run_offset;

    g_return_if_fail (TEXT_IS_EDITOR (self));
    g_return_if_fail (TEXT_IS_DOCUMENT (self->document));
    g_return_if_fail (TEXT_IS_PARAGRAPH (start->paragraph));

    run = text_paragraph_get_run_at_index (start->paragraph, start->index, &run_start_index);

    run_offset = start->index - run_start_index;

    // TODO: Replace with hybrid tree/piece-table structure?
    // Textual data is stored in buffers and indexed by the tree
    g_object_get (run, "text", &text, NULL);
    modified = g_string_new (text);
    modified = g_string_insert (modified, run_offset, str);
    g_object_set (run, "text", modified->str, NULL);
    g_string_free (modified, TRUE);

    // TODO: Update all marks

    // Move cursor left/right by amount changed
    // This should be handled by an auxiliary anchor object which
    // remains fixed at a given point in the text no matter how
    // the text changes (i.e. a cursor).
    text_editor_move_mark_right (self, start, strlen (str));
}

TextMark *
_get_mark (TextEditor         *self,
           TextEditorMarkType  type)
{
    g_return_val_if_fail (TEXT_IS_EDITOR (self), NULL);
    g_return_val_if_fail (TEXT_IS_DOCUMENT (self->document), NULL);

    return (type == TEXT_EDITOR_CURSOR)
        ? self->document->cursor
        : self->document->selection;
}

void
text_editor_move_first (TextEditor         *self,
                        TextEditorMarkType  type)
{
    text_editor_move_mark_first (self, _get_mark (self, type));
}

void
text_editor_move_last (TextEditor         *self,
                       TextEditorMarkType  type)
{
    text_editor_move_mark_last (self, _get_mark (self, type));
}

void
text_editor_move_right (TextEditor         *self,
                        TextEditorMarkType  type,
                        int                 amount)
{
    text_editor_move_mark_right (self, _get_mark (self, type), amount);
}

void
text_editor_move_left (TextEditor         *self,
                       TextEditorMarkType  type,
                       int                 amount)
{
    text_editor_move_mark_left (self, _get_mark (self, type), amount);
}

void
text_editor_insert (TextEditor         *self,
                    TextEditorMarkType  type,
                    gchar              *str)
{
    text_editor_insert_at_mark (self, _get_mark (self, type), str);
}

void
text_editor_delete (TextEditor         *self,
                    TextEditorMarkType  type,
                    int                 length)
{
    text_editor_delete_at_mark (self, _get_mark (self, type), length);
}

void
text_editor_replace (TextEditor         *self,
                     TextEditorMarkType  start_type,
                     TextEditorMarkType  end_type,
                     gchar              *text)
{
    text_editor_replace_at_mark (self, _get_mark (self, start_type), _get_mark (self, end_type), text);
}

TextRun *
text_editor_get_run (TextEditor         *self,
                     TextEditorMarkType  type)
{
    return text_editor_get_run_at_mark (self, _get_mark (self, type));
}

static void
text_editor_init (TextEditor *self)
{
}

/*
ISC License

Copyright 2022-2024 BeamNG

Permission to use, copy, modify, and/or distribute this software for
any purpose with or without fee is hereby granted, provided that the
above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
OF THIS SOFTWARE.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if __linux__
#include <errno.h>
#endif

#define BUF_SIZE 1024
#define INITIAL_CAPACITY 1
#define MAX_FIELD_LENGTH 1024

typedef struct _csv_table {
    char** item;
    struct _csv_table** line;
    long itemCount;
    long lineCount;
    long itemCapacity;
    long lineCapacity;
} csv_table;

csv_table* new_csv_table(void)
{
    csv_table* table = malloc(sizeof(*table));
    if (!table)
        return NULL;

    table->item = NULL;
    table->line = NULL;
    table->itemCount = 0;
    table->lineCount = 0;
    table->itemCapacity = 0;
    table->lineCapacity = 0;

    return table;
}

void free_csv_table(csv_table** table)
{
    if (!table)
        return;

    long i;
    csv_table* t = *table;

    for (i = 0; i < t->itemCount; ++i)
        free(t->item[i]);
    free(t->item);

    for (i = 0; i < t->lineCount; ++i)
        free_csv_table(&t->line[i]);
    free(t->line);

    free(t);
    *table = NULL;
}

void push_item(csv_table* table, const char* item)
{
    long newCapacity;
    char** newItemArray;

    if (table->itemCount == table->itemCapacity) {
        newCapacity = table->itemCapacity > 0 ?
            table->itemCapacity * 2 : INITIAL_CAPACITY;
        newItemArray = realloc(table->item, sizeof(char*) * newCapacity);
        if (!newItemArray)
            return;

        table->item = newItemArray;
        table->itemCapacity = newCapacity;
    }

    table->item[table->itemCount] = strdup(item);
    if (table->item[table->itemCount])
        table->itemCount++;
}

void push_line(csv_table* table, csv_table* line)
{
    long newCapacity;
    csv_table** newLineArray;

    if (table->lineCount == table->lineCapacity) {
        newCapacity = table->lineCapacity > 0 ?
            table->lineCapacity * 2 : INITIAL_CAPACITY;
        newLineArray = realloc(table->line, sizeof(*line) * newCapacity);
        if (!newLineArray)
            return;

        table->line = newLineArray;
        table->lineCapacity = newCapacity;
    }

    table->line[table->lineCount++] = line;
}

csv_table* csv_get_line(csv_table* table, long lineID)
{
    return table->line[lineID];
}

const char* csv_get_item(csv_table* line, long fieldID)
{
    return line->item[fieldID];
}

long csv_get_next_line_ID(csv_table* table, long lineID,
    long fieldID, const char* s)
{
    for (long i = lineID; i < table->lineCount; ++i) {
        csv_table* line = table->line[i];
        if (!strcmp(line->item[fieldID], s)) {
            return i;
        }
    }

    return -1;
}

long csv_count_field_entries(csv_table* line, long fieldID, const char* s)
{
    long count = 0;
    long lineID = csv_get_next_line_ID(line, 0, fieldID, s);

    while (lineID > -1) {
        count += 1;
        lineID = csv_get_next_line_ID(line, lineID + 1, fieldID, s);
    }

    return count;
}

long csv_check_field_value(csv_table* line, long fieldID, const char* s)
{
    return !strcmp(line->item[fieldID], s);
}

long csv_check_if_number(csv_table* line, long fieldID)
{
    return csv_check_field_value(line, fieldID, "number");
}

long csv_check_if_boolean(csv_table* line, long fieldID)
{
    return csv_check_field_value(line, fieldID, "boolean");
}

double csv_get_item_as_double(csv_table* line, long fieldID)
{
    return strtod(line->item[fieldID], NULL);
}

csv_table* csv_decode(const char* filename, char sep)
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file %s\n", filename);
        return NULL;
    }

    char buffer[BUF_SIZE] = {0};
    long fAlloc = BUF_SIZE;
    long fLen = 0;
    char *s = malloc(fAlloc + 1);
    if (!s) {
        printf("Failed to allocate memory when reading %s.\n", filename);
        fclose(file);
        free(s);
        return NULL;
    }

    size_t bread = 0;
    while ((bread = fread(buffer, sizeof(char), BUF_SIZE, file)) > 0) {
        if (fLen + bread > fAlloc) {
            char *s_old = s;
            fAlloc += BUF_SIZE;
            s = realloc(s, fAlloc + 1);
            if (!s) {
                printf("Failed to allocate memory when reading %s.\n", filename);
                fclose(file);
                free(s_old);
                return NULL;
            }
        }
        memcpy(s + fLen, buffer, bread);
        fLen += bread;
    }
    if (ferror(file) != 0) {
        printf("Failed to read file %s. Error code %d.\n", filename, errno);
        fclose(file);
        free(s);
        return NULL;
    }
    s[fLen] = '\0';
    fclose(file);

    csv_table* res = new_csv_table();
    csv_table* currentLine = new_csv_table();

    char field[MAX_FIELD_LENGTH];
    long fieldIndex = 0, inQuotes = 0;

    for (int i = 0; s[i] != '\0'; ++i) {
        char c = s[i];

        if (c == '"') {
            if (inQuotes && s[i + 1] == '"') {
                i++;
            }
            else {
                inQuotes = !inQuotes;
                continue;
            }
        }

        if ((c == sep && !inQuotes) || (c == '\n' && !inQuotes) || c == '\0') {
            field[fieldIndex] = '\0';
            push_item(currentLine, field);
            fieldIndex = 0;

            if (c == '\n' || c == '\0') {
                if (currentLine->itemCount > 0 || c == '\n') {
                    push_line(res, currentLine);
                    currentLine = new_csv_table();
                }
            }
            if (c == '\0')
                break;
            continue;
        }

        if (fieldIndex < MAX_FIELD_LENGTH - 1) {
            field[fieldIndex++] = c;
        }
    }

    if (currentLine->itemCount > 0) {
        push_line(res, currentLine);
    }
    else {
        free_csv_table(&currentLine);
    }

    free(s);
    return res;
}

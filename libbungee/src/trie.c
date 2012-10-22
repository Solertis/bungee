/*
  trie.c: Trie data structure.

  Copyright (c) 2012 Red Hat, Inc. <http://www.redhat.com>

  Author: Anand V. Avati <aavati@redhat.com>
  git://github.com/avati/did-you-mean.git

  This file is part of Bungee.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

/*
 * Search for the 'most similar' word in the dictionary
 * Usage: ./trie /usr/share/dict/words funkyword
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <glib.h>

#define _min(a,b) ((a) < (b) ? (a) : (b))

#define DISTANCE_EDIT 1
#define DISTANCE_INS  1
#define DISTANCE_DEL  1

bng_trie_t *
bng_trie_new (const gchar *word)
{
  bng_trie_t  *trie = NULL;

  trie = calloc (sizeof (*trie), 1);
  if (!trie)
    return NULL;

  trie->word = g_strdup (word);
  if (!trie->word)
    {
      free (trie);
      trie = NULL;
    }

  trie->root.data = calloc (sizeof (int), strlen (trie->word));
  if (!trie->root.data)
    {
      free ((void *)trie->word);
      free (trie);
      trie = NULL;
    }

  return trie;
}

bng_trie_node_t *
bng_trie_sub_node (bng_trie_t *trie, bng_trie_node_t *node, gint id)
{
  bng_trie_node_t *sub_node = NULL;

  sub_node = node->sub_nodes[id];
  if (!sub_node)
    {
      sub_node = calloc (sizeof (*sub_node), 1);
      if (!sub_node)
	return NULL;

      sub_node->id        = id;
      sub_node->depth     = node->depth + 1;
      sub_node->data      = calloc (sizeof (int), strlen (trie->word));

      node->sub_nodes[id] = sub_node;
      sub_node->parent    = node;
      trie->node_count++;
    }

  return sub_node;
}


gint bng_trie_add (bng_trie_t *trie, const gchar *word)
{
  bng_trie_node_t *node = NULL;
  int              i = 0;
  char             id = 0;
  bng_trie_node_t *sub_node = NULL;

  node = &trie->root;

  for (i = 0; i < strlen (word); i++)
    {
      if (isspace (word[i]))
	break;

      id = word[i];

      sub_node = trie_sub_node (trie, node, id);
      if (!sub_node)
	return -1;
      node = sub_node;
    }

  node->eow = 1;

  return 0;
}

gint
trie_node_walk (bng_trie_node_t *node, gint (*fn)(bng_trie_node_t *node,
                                                void *data),
               void *data, int eow_only)
{
  bng_trie_node_t *trav = NULL;
  int              i = 0;
  int              ret = 0;

  if (!eow_only || node->eow)
    ret = fn (node, data);

  if (ret)
    goto out;

  for (i = 0; i < 255; i++) {
    trav = node->sub_nodes[i];

    if (trav)
      ret += bng_trie_node_walk (trav, fn, data, eow_only);

    if (ret < 0)
      goto out;
  }

 out:
  return ret;
}

gint
bng_trie_walk (bng_trie_t *trie, gint (*fn)(bng_trie_node_t *node, void *data),
           void *data, gint eow_only)
{
  return bng_trie_node_walk (&trie->root, fn, data, eow_only);
}

gint
bng_load_dict (bng_trie_t *trie, const gchar *filename)
{
  FILE *fp = NULL;
  char  word[128] = {0};
  int   cnt = 0;
  int   ret = -1;

  fp = fopen (filename, "r");
  if (!fp)
    return ret;

  while (fgets (word, 128, fp)) {
    ret = bng_trie_add (trie, word);
    if (ret)
      break;
    cnt++;
  }

  fclose (fp);

  return cnt;
}

gint bng_calc_dist (bng_trie_node_t *node, void *data)
{
  const char *word = NULL;
  int         i = 0;
  int        *row = NULL;
  int        *uprow = NULL;
  int         distu = 0;
  int         distl = 0;
  int         distul = 0;

  word = data;

  row = node->data;

  if (!node->parent) {
    for (i = 0; i < strlen (word); i++)
      row[i] = i+1;

    return 0;
  }

  uprow = node->parent->data;

  distu = node->depth;          /* up node */
  distul = node->parent->depth; /* up-left node */

  for (i = 0; i < strlen (word); i++) {
    distl = uprow[i];     /* left node */

    if (word[i] == node->id)
      row[i] = distul;
    else
      row[i] = _min ((distul + DISTANCE_EDIT),
		    _min ((distu + DISTANCE_DEL),
			 (distl + DISTANCE_INS)));

    distu  = row[i];
    distul = distl;
  }

  return 0;
}

gint
bng_trie_measure (bng_trie_t *trie, const gchar *word, gchar **matched_words)
{
  int len = 0;
  int ret = 0;
  int i = 0;

  len = strlen (word);

  /* fprintf (stderr, "Calculating distances ... "); */

  ret = bng_trie_walk (trie, calc_dist, (void *)word, 0);

  /*
    fprintf (stderr, "done.\n");
    fprintf (stderr, "Did you mean: ");
  */

  struct {
    int len;
    int wordlen;
  } msg;

  msg.wordlen = len;
  for (i = 0; i < len; i++)
    {
      msg.len = i;
      ret = bng_trie_walk (trie, print_if_equal, &msg, 1);
      if (ret)
	break;
    }
  fflush (stdout);

  /* fprintf (stderr, "\n"); */

  return i;
}

/*
gint
print_node (bng_trie_node_t *node)
{
  if (node->parent)
    {
      print_node (node->parent);
      printf ("%c", node->id);
    }

  return 0;
}

gint
bng_print_if_equal (bng_trie_node_t *node, void *data)
{
  int *row = NULL;
  int  ret = 0;
  struct {
    int   len;
    int   wordlen;
  } *msg;

  msg = data;
  row = node->data;

  if (row[msg->wordlen - 1] == msg->len) {
    print_node (node);
    printf (" ");
    ret = 1;
  }

  return ret;
}

void
bng_trie_node_print (bng_trie_node_t *node)
{
  bng_trie_node_t *trav = NULL;
  int              i = 0;

  printf ("%c", node->id);

  for (i = 0; i < 255; i++) {
    trav = node->sub_nodes[i];
    if (trav) {
      printf ("(");
      bng_trie_node_print (trav);
      printf (")");
    }
  }
}


void bng_trie_print (bng_trie_t *trie)
{
  bng_trie_node_t *node = NULL;

  node = &trie->root;

  bng_trie_node_print (node);
  printf ("\n");
}

int
main (int argc, char *argv[])
{
  bng_trie_t *trie = NULL;
  int          ret = 0;
  char        *word = NULL;

  if (argc != 3) {
    fprintf (stderr, "Usage: %s <dictfile> <word>\n",
	     argv[0]);
    return 1;
  }

  word = argv[2];

  trie = bng_trie_new (word);

  ret = bng_load_dict (trie, argv[1]);

  if (ret <= 0)
    return 1;

  fprintf (stderr, "Loaded %d words (%d nodes)\n",
	   ret, trie->node_count);


  bng_trie_measure (trie, word);
  bng_trie_print (trie);

  return 0;
}
*/

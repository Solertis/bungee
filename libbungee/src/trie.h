/*
  trie.h: Trie data structure.

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

#ifndef _TRIE_H
#define _TRIE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  gchar id;
  gchar eow;
  gint depth;
  void *data;
  bng_trie_node_t *parent;
  bng_trie_node_t *sub_nodes[255];
} bng_trie_node_t;

typedef struct
{
  bng_trie_node_t root;
  gint node_count;
  const gchar *word;
} bng_trie_t;

bng_trie_t *trie_new (const gchar *word);
bng_trie_node_t *trie_sub_node (bng_trie_t *trie, bng_trie_node_t *node, gint id);
gint trie_add (bng_trie_t *trie, const gchar *word);
gint trie_node_walk (bng_trie_node_t *node, gint (*fn)(bng_trie_node_t *node, void *data), \
		     void *data, gint eow_only);
gint trie_walk (bng_trie_t *trie, gint (*fn)(bng_trie_node_t *node, void *data), \
		void *data, gint eow_only);
gint bng_load_dict (bng_trie_t *trie, const gchar *filename);
gint bng_calc_dist (bng_trie_node_t *node, void *data);
gint bng_trie_measure (bng_trie_t *trie, const gchar *word);



#ifdef __cplusplus
}
#endif

#endif /* _TRIE_H */

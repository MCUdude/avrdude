/*
 * AVRDUDE - A Downloader/Uploader for AVR device programmers
 * Copyright (C) 2023 Hans Eirik Bull
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ac_cfg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libavrdude.h"

#ifdef HAVE_LIBSERIALPORT

SERIALADAPTER *serialadapter_new(void) {
  SERIALADAPTER *ser = (SERIALADAPTER *) cfg_malloc("serialadater_new()", sizeof(*ser));
  const char *nulp = cache_string("");

  // Initialise const char * and LISTID entities
  ser->id = lcreat(NULL, 0);
  //ser->usbpid = lcreat(NULL, 0);
  ser->desc = nulp;
  ser->port_path = nulp;
  return ser;
}

SERIALADAPTER *serialadapter_dup(const SERIALADAPTER *src) {
  SERIALADAPTER *ser = serialadapter_new();

  if (src) {
    ldestroy_cb(ser->id, free);
    //ldestroy_cb(ser->usbpid, free);

    memcpy(ser, src, sizeof(*ser));

    ser->id = lcreat(NULL, 0);
    //ser->usbpid = lcreat(NULL, 0);

    // Leave id list empty but copy usbpid over
    /*if (src->usbpid) {
      for (LNODEID ln = lfirst(src->usbpid); ln; ln = lnext(ln)) {
        int *ip = cfg_malloc("serialadapter_dup_dup()", sizeof(int));
        *ip = *(int *) ldata(ln);
        ladd(ser->usbpid, ip);
      }
    }*/
  }
  return ser;
}

void serialadapter_free(SERIALADAPTER *s) {
  if (s) {
    if (s->id) {
      ldestroy_cb(s->id, free);
      s->id = NULL;
    }
    /*if (s->usbpid) {
      ldestroy_cb(s->usbpid, free);
      s->usbpid = NULL;
    }*/
    // Never free const char *, eg, s->desc, which are set by cache_string()
    free(s);
  }
}

SERIALADAPTER *locate_serialadapter_set(const LISTID serialadapters, const char *configid, const char **setid) {
  for (LNODEID ln1=lfirst(serialadapters); ln1; ln1=lnext(ln1)) {
    SERIALADAPTER *s = ldata(ln1);
    for (LNODEID ln2=lfirst(s->id); ln2; ln2=lnext(ln2)) {
      const char *id = (const char *) ldata(ln2);
      if (str_caseeq(configid, id)) {
        if (setid)
          *setid = id;
        return s;
      }
    }
  }
  return NULL;
}

SERIALADAPTER *locate_serialadapter(const LISTID serialadapters, const char *configid) {
  return locate_serialadapter_set(serialadapters, configid, NULL);
}

static int sort_serialadapter_compare(const SERIALADAPTER *s1, const SERIALADAPTER *s2) {
  if(s1 == NULL || s1->id == NULL || s2 == NULL || s2->id == NULL)
    return 0;

  return strcasecmp(ldata(lfirst(s1->id)), ldata(lfirst(s2->id)));
}

void sort_serialadapters(LISTID serialadapters) {
  lsort(serialadapters,(int (*)(void*, void*)) sort_serialadapter_compare);
}

#else

#endif
